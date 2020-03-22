/////////////////////////////////////////////////////////////////////////////////
//
//	GuidanceMFD.cpp - Source for the STS Guidance & Control MFD 
//  (STSGuidanceMFD.dll)
//
//	by Richard Croy (rjcroy)
//
//  Credit and thanks to:
//	Chris Knestrick		   - For the Attitude controller 
//						     (Control.cpp) and some maths functions.
//						   - Chris also re-organised the structure
//							 of the guidance programs to remove
//							 the external view bug.  
//
//  Manuel Amorim (McDope) - This MFD was written using McDopes's 
//							 RendezvousMFD as an MFD template.
//
//	 
//	
// Copyright (C) 2003 Richard Croy
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License.txt for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ORBITER_MODULE
#include <stdio.h>
#include <math.h>
#include "orbitersdk.h"
#include "GuidanceMFD.h"
#include "OrbiterMath.h"
#include "Control.h"

static struct {  
	int mode; 
    GuidanceMFD *CurrentMFD;
} g_GuidanceMFD;

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "Guidance & Control";
	MFDMODESPEC spec;
	spec.name    = name;
	spec.key     = OAPI_KEY_P;
	spec.msgproc = GuidanceMFD::MsgProc;	
	g_GuidanceMFD.mode = oapiRegisterMFDMode (spec);
	g_GuidanceMFD.CurrentMFD = NULL;

	Done.a = false;
	Done.b = false;
	Done.c = false;
	Done.d = false;
	Done.e = false;

	AttDone.a = false;
	AttDone.b = false;
	AttDone.c = false;
	AttDone.d = false;
	AttDone.e = false;
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	oapiUnregisterMFDMode (g_GuidanceMFD.mode);
}

DLLCLBK void opcPreStep(double simt, double simdt, double mjd)
{
	static double prevCalcTime = 0;		// CCK - note that this MUST be static to work!
		
	// Don't try to call the function before the class has been created
	// (after Chris Knestrick)
	if (g_GuidanceMFD.CurrentMFD != NULL) { 
		
		// CCK - Call the code to do the guidance and control calculations.  This should only
		//       be done at CALC_INTERVAL
		if ((simt - prevCalcTime) >= CALC_INTERVAL) {
			prevCalcTime = simt;
			
			switch(MFDMode) {
				case MENU:	
				// CCK Nothing needs to be done for MENU or INFO
				//g_GuidanceMFD.CurrentMFD->CalcMenuDisplay();
				break;
				case INFO:
				// Orbit Information
				//g_GuidanceMFD.CurrentMFD->UpdateInfoDisplay();
				break;
			case ORBIT:
				// Orbit Height adjustment
				g_GuidanceMFD.CurrentMFD->CalcOrbitDisplay();
				break;
			case PLANE:
				// Orbital plane alignment
				g_GuidanceMFD.CurrentMFD->CalcPlaneDisplay();
				break;
			case RENDEZVOUS:
				// Rendezvous routine
				g_GuidanceMFD.CurrentMFD->CalcRendezvousDisplay();
				break;
			}
		}
		
		// Orbit height adjustment Burn 1
		if (EngineController1) {
			g_GuidanceMFD.CurrentMFD->EngController1(simt, VnewBurn1, BurnTInit1);
		}
		// Orbit height adjustment Burn 2
		if (EngineController2) {
			g_GuidanceMFD.CurrentMFD->EngController1(simt, VnewBurn2, BurnTInit2);
		}
		// Orbital plane alignment burn
		if (EngineController3) {
			g_GuidanceMFD.CurrentMFD->EngController2(simt, TimeOfBurn);
		}
		// Attitude controller
		if (AttitudeControllerOn) {
			g_GuidanceMFD.CurrentMFD->AttitudeController(DeltaVAttitude, simt, simdt);
		}
		// Star track attitude controller
		if (StarTrackOn) {
			g_GuidanceMFD.CurrentMFD->StarTrackAttController(DeltaVAttitude);
		}
		// On-orbit cruise attitude controller
		if (LHLV_AttControlOn) {
			g_GuidanceMFD.CurrentMFD->LHLV_AttControl(simt, LHLV_AC_OFF);
		}
		// Fixed time burn
		if (EngineController4) {
			g_GuidanceMFD.CurrentMFD->EngController4(simt, BurnDuration);
		}
		// Station Keeping Controller
		if (StationKeepingOn) {
			g_GuidanceMFD.CurrentMFD->StationKeeping(SKTarget);
		}
		// Kill all thrusters caller
		if (KillAllThrustersSwitch) {
			g_GuidanceMFD.CurrentMFD->KillAllThrusters();
		}

	}
	//sprintf(oapiDebugString(),"v %f R %f R.V %f Tperi %f TApo %f Tsp %f", v, R, RDotV, TtPeri, TtApo, tsp);
	//sprintf(oapiDebugString(),"vnew1 %f vnew2 %f", VnewBurn1, VnewBurn2);
	//sprintf(oapiDebugString(),"BurnTInit %.2lf Simt: %.2lf", BurnTInit2, SimT);
	//sprintf(oapiDebugString(),"MFDMode: %d", MFDMode);
	//sprintf(oapiDebugString(),"R_targ abs: %f", AbsOfVector(R_targ));
}

GuidanceMFD::GuidanceMFD (DWORD w, DWORD h, VESSEL *vessel)
: MFD (w, h, vessel)
{
	g_GuidanceMFD.CurrentMFD = this;
		
	Vessel = vessel;
	hVessel = vessel->GetHandle();
	hInfoVessel = hVessel;

	KAxis.x = 0;
	KAxis.y = 0;
	KAxis.z = 1;

	OrbitMode = ORBIT_MODE_NEED_PARAMS;
	RendMode = REND_MODE_NEED_TARGET;
	PlaneMode = PLANE_MODE_NEED_TARGET;

	// CCK Scale for MFD size
	ROWH = h / 22;

}

int GuidanceMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
			return (int)(new GuidanceMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

void GuidanceMFD::Update(HDC hdc)
{	
	hDC = hdc;
	Title (hDC, "STS Guidance & Control");
	
	switch(MFDMode) {
	case MENU:
		UpdateMenuDisplay();
		break;
	case INFO:
		// Orbit Information
		UpdateInfoDisplay();
		break;
	case ORBIT:
		// Orbit Height adjustment
		UpdateOrbitDisplay();
		break;
	case PLANE:
		// Orbital plane alignment
		UpdatePlaneDisplay();
		break;
	case RENDEZVOUS:
		// Rendezvous routine
		UpdateRendezvousDisplay();
		break;
	}
}

bool GuidanceMFD::ConsumeKeyBuffered(DWORD key)
{
	bool APInput (void *id, char *str, void *data);
	bool MenuInput (void *id, char *str, void *data);
	bool InfoInput (void *id, char *str, void *data);
	bool PlaneInput (void *id, char *str, void *data);
	bool StationInput (void *id, char *str, void *data);
	bool TimeInput (void *id, char *str, void *data);
	bool RendezvousInput(void *id, char *str, void *data);
	
	switch (key) {
	case OAPI_KEY_T:
		if (MFDMode == ORBIT ) {
			oapiOpenInputBox ("Perigee & Apogee (km x 10) [e.g. 1805 2200 = 180.5 x 220.0km]", APInput, 0, 20, (void*)this);
			return true;
		} else if (MFDMode == MENU ) {
			oapiOpenInputBox ("Select a guidance mode. [1, 2, 3, 4]", MenuInput, 0, 20, (void*)this);
			return true;
		} else if (MFDMode == INFO ) {
			oapiOpenInputBox ("Target vessel", InfoInput, 0, 20, (void*)this);
			return true;
		} else if (MFDMode == PLANE ) {
			oapiOpenInputBox ("Target vessel", PlaneInput, 0, 20, (void*)this);
			return true;
		} else if (MFDMode == RENDEZVOUS ) {
			oapiOpenInputBox ("Target vessel", RendezvousInput, 0, 20, (void*)this);
			return true;
		} else {
			return false;
		}

	case OAPI_KEY_H:
		if (MFDMode == INFO ){
			if (LHLV_AttControlOn) {
				LHLV_AC_OFF = true;
			} else {
				LHLV_AttControlOn = true;
			}
			return true;
		} else {
			return false;
		}

	case OAPI_KEY_C:
		if (MFDMode == ORBIT) {
			MFDMode = MENU;
			EngineController1 = false;
			EngineController2 = false;
			Done.a = false;
			Done.b = false;
			Done.c = false;
			BurnComplete = false;
			OrbitMode = ORBIT_MODE_NEED_PARAMS;
			return true;
		} else if (MFDMode == INFO) {
			MFDMode = MENU;
			return true;
		} else if (MFDMode == PLANE) {
			MFDMode = MENU;
			EngineController3 = false;
			Done.a = false;
			Done.b = false;
			Done.c = false;
			BurnComplete = false;
			PlaneMode = PLANE_MODE_NEED_TARGET;
			return true;
		} else if (MFDMode == RENDEZVOUS) {
			MFDMode = MENU;
			StationKeepingOn = false;
			BurnComplete = false;
			AttitudeControllerOn = false;
			RendMode = REND_MODE_NEED_TARGET;
			KillAllThrustersSwitch = true;
			return true;
		} else {
			return false;
		}
	}

	return false;
}

bool GuidanceMFD::ConsumeButton (int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;
	static const DWORD btkey[1] = { OAPI_KEY_T };
	if (bt < 1) return ConsumeKeyBuffered (btkey[bt]);
	else return false;
}

char *GuidanceMFD::ButtonLabel (int bt)
{
	char *label[1] = {"L"};
	return (bt < 1 ? label[bt] : 0);
}

int GuidanceMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static const MFDBUTTONMENU mnu[1] = {
		{"Guidance & Control", 0, 'L'}
	};
	if (menu) *menu = mnu;
	return 1;
}

void inline GuidanceMFD::UpdateMenuDisplay()
{
	char Buffer[80];
	int Column = 115;
	bool dummy = TRUE;
	Row = line+10;

	// MFD Display

	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::Program Selection::");
	TextOut(hDC, 5, 2*ROWH, Buffer, strlen(Buffer));

	SetTextColor(hDC, GREEN);
	sprintf(Buffer, "1. Orbit Information");
	TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "   & -ZLV -XVV attitude hold");
	TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
	
	sprintf(Buffer, "2. Orbit Height Adjustment");
	TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "3. Orbital Plane Alignment");
	TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "4. Rendezvous");
	TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
	SetTextColor(hDC, GREY);
	sprintf(Buffer, "Press Shift + T to select");
	TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));

}

void inline GuidanceMFD::CalcOrbitDisplay()
{

	SimT = oapiGetSimTime();
	hSetGbody = oapiGetGbodyByName(Planet_Name);
	
	if (OrbitMode == ORBIT_MODE_CALC1) {
		// Calculate the burn:
		
		// Planet parameters
		hSetGbody = oapiGetGbodyByName(Planet_Name);
		GbodyMass = oapiGetMass(hSetGbody);
		GbodySize = oapiGetSize(hSetGbody) / 1000;
		Mu_Planet = GK * GbodyMass;

		// Get eccentricity and orbital radius
		Vessel=oapiGetFocusInterface();
		Vessel->GetElements(Elements, J2000);
		Vessel->GetRelativePos(hSetGbody, RelPosition);
		Vessel->GetRelativeVel(hSetGbody, RelVelocity);

		R = AbsOfVector(RelPosition) / 1000;
		
		// Calculate semi-latus rectum and true anomaly
		p = Elements.a/1000 *(1 - Elements.e*Elements.e);
		v = acos((1/Elements.e)*(p/R - 1));
		
		RDotV = dotp(RelVelocity, RelPosition);
		if ( RDotV < 0 ) {
			v = 2*PI - v;
		}
	
		// Determine the time since periapsis
		//   - Eccentric anomaly
		E = 2 * atan(sqrt((1-Elements.e)/(1+Elements.e))*tan(v/2));
		//   - Mean anomaly
		Me = E - Elements.e*sin(E);
		//   - Period of orbit
		T = 2*PI*sqrt((Elements.a*Elements.a*Elements.a/1e9)/Mu_Planet);

		// Time since periapsis is
		tsp = Me/(2*PI)*T;

		// Time to next periapsis & apoapsis
		TtPeri = T - tsp;
		if (RDotV < 0) {
			TtPeri = -1 * tsp;
		}

		if (TtPeri > (T / 2 )) {
			TtApo = fabs((T/2) - TtPeri);
		} else {
			TtApo = fabs(TtPeri + (T/2));
		}

		if (TtApo < TtPeri) {
			Orbit1.peri = PerigeeSP/10 + GbodySize;
			Orbit1.apo = Elements.a*(1+Elements.e) / 1000;
			Orbit2.peri = Orbit1.peri;
			Orbit2.apo = ApogeeSP/10 + GbodySize;
			TimeBurn1 = SimT + TtApo;
			TimeBurn2 = SimT + TtPeri;
			Orbit1StaticR = Orbit1.apo;
			Orbit2StaticR = Orbit2.peri;
		} else {
			Orbit1.apo = ApogeeSP/10 + GbodySize;
			Orbit1.peri = Elements.a*(1-Elements.e) / 1000;
			Orbit2.apo = Orbit1.apo;
			Orbit2.peri = PerigeeSP/10 + GbodySize;
			TimeBurn1 = SimT + TtPeri;
			TimeBurn2 = SimT + TtApo;
			Orbit1StaticR = Orbit1.peri;
			Orbit2StaticR = Orbit2.apo;
		}

		// Calculate new velocities at Apo and Peri.
		VnewBurn1 = NewVelocity_AorP(Orbit1.peri, Orbit1.apo, Orbit1StaticR);
		VnewBurn2 = NewVelocity_AorP(Orbit2.peri, Orbit2.apo, Orbit2StaticR);
		Ans.One = sqrt(Mu_Planet * (Elements.a/1000) * (1 - Elements.e*Elements.e)); // h
		Ans.Two = Ans.One / Orbit1StaticR; // h / (R static) 
		DeltaVBurn1 = (VnewBurn1 - Ans.Two) * 1000;

		Ans.One = (Orbit1.peri + Orbit1.apo) / 2 ; // a for orbit 1
		Ans.Two = (Orbit1.apo / Ans.One) - 1;	   // e for orbit 1
		Ans.Three = sqrt(Mu_Planet * Ans.One * (1 - Ans.Two*Ans.Two)); // h 
		Ans.One = Ans.Three / Orbit2StaticR ; // h / (R static)
		DeltaVBurn2 = (VnewBurn2 - Ans.One) * 1000 ;
		
		// Burn times
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();
		DeltaTBurn1 = fabs(DeltaVBurn1) / (MaxThrust / VesselMass ) ;
		DeltaTBurn2 = fabs(DeltaVBurn2) / (MaxThrust / VesselMass ) ;
		
		if (TtApo < TtPeri) {
			BurnTInit1 = SimT + TtApo - (DeltaTBurn1 / 2);
		} else {
			BurnTInit1 = SimT + TtPeri - (DeltaTBurn1 / 2);
		}

		if (DeltaVBurn1 < 0) {
			Prograde = false;
		} else {
			Prograde = true;
		}

		Done.a = false;
		Done.b = false;
		Done.c = false;
		Done.d = false;
		Done.e = false;

		OrbitMode = ORBIT_MODE_BURN1;

	}
	
	if (OrbitMode == ORBIT_MODE_BURN1) {
		// Switches for the control system
		if ((BurnTInit1 - SimT) < 60 && !BurnComplete) {
			EngineController1 = true;
		} else if (BurnComplete) {
			EngineController1 = false;
			Done.a = false;
			Done.b = false;
			Done.c = false;

			BurnComplete = false;
			OrbitMode = ORBIT_MODE_CALC2;

		}
	
	} 

	if (OrbitMode == ORBIT_MODE_CALC2) {
		// Burn 2 setup
		
		// Re-calculate the time to next periapsis/apoapsis
		// Get eccentricity and orbital radius
		Vessel=oapiGetFocusInterface();
		Vessel->GetElements(Elements, J2000);
		Vessel->GetRelativePos(hSetGbody, RelPosition);
		Vessel->GetRelativeVel(hSetGbody, RelVelocity);

		R = AbsOfVector(RelPosition) / 1000;
		
		// Calculate semi-latus rectum and true anomaly
		p = Elements.a/1000 *(1 - Elements.e*Elements.e);
		v = acos((1/Elements.e)*(p/R - 1));
		
		RDotV = dotp(RelVelocity, RelPosition);
		if ( RDotV < 0 ) {
			v = 2*PI - v;
		}
		
		// Determine the time since periapsis
		//   - Eccentric anomaly
		E = 2 * atan(sqrt((1-Elements.e)/(1+Elements.e))*tan(v/2));
		//   - Mean anomaly
		Me = E - Elements.e*sin(E);
		//   - Period of orbit
		T = 2*PI*sqrt((Elements.a*Elements.a*Elements.a/1e9)/Mu_Planet);

		// Time since periapsis is
		tsp = Me/(2*PI)*T;

		// Time to next periapsis & apoapsis
		TtPeri = T - tsp;
		if (RDotV < 0) {
			TtPeri = -1 * tsp;
		}

		if (TtPeri > (T / 2 )) {
			TtApo = fabs((T/2) - TtPeri);
		} else {
			TtApo = fabs(TtPeri + (T/2));
		}

		// Set the Burn 2 time
		if (TtApo < TtPeri) {
			if (TtApo < (T/4)) {
				BurnTInit2 = TimeBurn2 - (DeltaTBurn2 / 2);
			} else {
				BurnTInit2 = SimT + TtApo - (DeltaTBurn2 / 2);
			}
		} else {
			if (TtPeri < (T/4)) {
				BurnTInit2 = TimeBurn2 - (DeltaTBurn2 / 2);
			} else {
				BurnTInit2 = SimT + TtPeri - (DeltaTBurn2 / 2);
			}
		}

		if (DeltaVBurn2 < 0) {
			Prograde = false;
		} else {
			Prograde = true;
		}
			
		OrbitMode = ORBIT_MODE_BURN2;

	} 
	
	if (OrbitMode == ORBIT_MODE_BURN2) {
		// Switches for the control system
		if ((BurnTInit2 - SimT) < 60 && !BurnComplete) { 
			EngineController2 = true;
		} else if (BurnComplete) {
			EngineController2 = false;
			Done.a = false;
			Done.b = false;
			Done.c = false;
			OrbitMode = ORBIT_MODE_NEED_PARAMS;
			BurnComplete = false;
			MFDMode = MENU; // 0;
						
		}
	}

}

void inline GuidanceMFD::UpdateOrbitDisplay()
{
	char Buffer[80];
	int Column = 115;
	bool dummy = TRUE;
	Row = line+10;

	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::Orbit Height Adjustment::");
	TextOut(hDC, 5, 2*ROWH, Buffer, strlen(Buffer));


	SetTextColor(hDC, GREEN);

	if (OrbitMode == ORBIT_MODE_NEED_PARAMS) {
		// Need to get parameters for orbit adjustment
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Press Shift + T to enter target");
		TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "orbit perigee & apogee.");
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Press Shift + C to go back");
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
	} 

	if (OrbitMode == ORBIT_MODE_BURN1) {

		// Display the info
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Orbit 1: %.1f x %.1f km", Orbit1.peri - GbodySize, Orbit1.apo - GbodySize);
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Orbit 2: %.1f x %.1f km", Orbit2.peri - GbodySize, Orbit2.apo - GbodySize);
		TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn 1 delta V: %.2f m/s", DeltaVBurn1);
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn 2 delta V: %.2f m/s", DeltaVBurn2);
		TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn time 1: %.1f s (aprx)", DeltaTBurn1);
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn time 2: %.1f s (aprx)", DeltaTBurn2);
		TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Time until Auto Cntrl: %.0f s ", BurnTInit1 - SimT - 60);
		TextOut(hDC, 5, 13*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Time until Burn 1: %.0f s ", BurnTInit1 - SimT);
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));


	} 

	if (OrbitMode == ORBIT_MODE_BURN2) {
		// Display info for Burn 2
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Orbit 2: %.1f x %.1f km", Orbit2.peri - GbodySize, Orbit2.apo - GbodySize);
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn 2 delta V: %.2f m/s", DeltaVBurn2);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn time 2: %.1f s (aprx)", DeltaTBurn2);
		TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Time until Auto Cntrl: %.0f s ", BurnTInit2 - SimT - 60);
		TextOut(hDC, 5, 13*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Time until Burn 2: %.0f s ", BurnTInit2 - SimT);
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
	}	

}

void inline GuidanceMFD::UpdateInfoDisplay()
{
	// Displays information about the target orbit
	char Buffer[80];
	char Travelling[7];
	char Att_hold_mode_ind[4];
	OBJHANDLE hSetGbody; //handle of the current set Gbody (e.g. planet)
	VECTOR3 Rel_Vel; // vector of velocities relative to earth
	VECTOR3 Rel_Pos ; // vector of our position relative to earth
	VECTOR3 h_vector ; 
	double n_abs;
	double i ; // Inclination (equatorial ref)
	double h_abs; // magnitude of angular momentum
	ELEMENTS Elements;
	double Alt_Apoapsis;
	double Alt_Periapsis;
	double OmegaInfo; // longitude of the ascending node (radians)
	VESSEL *VesselInfo;
	VECTOR3 EccVec;
	double Longitude, Radius;
	VESSELSTATUS InfoStatus;

	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::Orbit Information::");
	TextOut(hDC, 5, 2*ROWH, Buffer, strlen(Buffer));
	SetTextColor(hDC, GREEN);

	hSetGbody = oapiGetGbodyByName(Planet_Name);

	// Calculate the current orbital inclination
	oapiGetRelativeVel(hInfoVessel, hSetGbody, &Rel_Vel);
	oapiGetRelativePos(hInfoVessel, hSetGbody, &Rel_Pos);

	// Change ecliptic rel. vectors to equatorial vectors.
	Rel_Vel = ReorderVector(Rel_Vel);
	Rel_Pos = ReorderVector(Rel_Pos);
	Rel_Vel = Ecliptic2Equatorial(Rel_Vel);
	Rel_Pos = Ecliptic2Equatorial(Rel_Pos);
	h_vector = AngularMomentum(Rel_Pos, Rel_Vel);
	h_abs = sqrt((h_vector.x*h_vector.x) + (h_vector.y*h_vector.y) + (h_vector.z*h_vector.z));
	i = Inclination(h_vector)*DEG;

	// retrieve the final orbital elements
	VesselInfo = oapiGetVesselInterface (hInfoVessel);
	VesselInfo->GetElements(Elements, J2000);
	Alt_Apoapsis = (Elements.a * ( 1 + Elements.e )/1000) - R_Earth; 
	Alt_Periapsis = (Elements.a * ( 1 - Elements.e )/1000) - R_Earth; 

	// calculate vector n and magnitude of n
	n = CrossProduct(KAxis, h_vector);
	n_abs = AbsOfVector(n);
	OmegaInfo = LAN(n, i);

	// Latitude and Latitude
	LatitudeLast = Latitude;
	VesselInfo->GetEquPos(Longitude, Latitude, Radius);
	if (Latitude > LatitudeLast) {
		sprintf(Travelling, "North");
	} else {
		sprintf(Travelling, "South");
	}

	Vessel->GetStatus(InfoStatus);
	
	// -ZLV -XVV attitude hold indicator
	if (LHLV_AttControlOn) {
		sprintf(Att_hold_mode_ind, "On");
	} else {
		sprintf(Att_hold_mode_ind, "Off");
	}

	// Some extras
	GbodyMass = oapiGetMass(hSetGbody);
	Mu_Planet = GKSI * GbodyMass;
	EccVec = EccentricityVector(Mu_Planet, Rel_Pos, Rel_Vel);

	ITVesselName = VesselInfo->GetName();

	sprintf(Buffer, "Target: %s", ITVesselName);
	TextOut(hDC, 5, 3*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Apoapsis Alt. :      %.2f km", Alt_Apoapsis);
	TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Periapsis Alt.:      %.2f km", Alt_Periapsis);
	TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Inclination (equa) : %.4f °", i);
	TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Long. of Asc. Node : %.4f °", OmegaInfo*DEG);
	TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Semi-major axis alt: %.2f km", (Elements.a/1000)-R_Earth );
	TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Latitude : %3.3f °", Latitude*DEG);
	TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Longitude: %3.3f °", Longitude*DEG);
	TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Travelling: %s ", Travelling);
	TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));

	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::On-Orbit Attitude Controller::");
	TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
	SetTextColor(hDC, YELLOW);
	sprintf(Buffer, "-ZLV -XVV attitude hold: %s ", Att_hold_mode_ind);
	TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));
		
	SetTextColor(hDC, GREY);
	sprintf(Buffer, "Shift + H to switch Att Controller ", InfoStatus.fuel*100);
	TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Shift + T to change target");
	TextOut(hDC, 5, 18*ROWH, Buffer, strlen(Buffer));
	sprintf(Buffer, "Shift + C to go back");
	TextOut(hDC, 5, 19*ROWH, Buffer, strlen(Buffer));

}

// Update the Plane alignment display
void inline GuidanceMFD::CalcPlaneDisplay()
{


	SimT = oapiGetSimTime();
	
	if (PlaneMode == PLANE_MODE_CALC_BURN) {

		TVessel = oapiGetVesselInterface (hTargetVessel);
	
		// 1. Get the angular momentum vector on the target orbit
		hSetGbody = oapiGetGbodyByName(Planet_Name);
		TVessel->GetRelativeVel(hSetGbody, V_targ);
		TVessel->GetRelativePos(hSetGbody, R_targ);
		
		R_targ = ReorderVector(R_targ);
		V_targ = ReorderVector(V_targ);

		h_targ = AngularMomentum(R_targ, V_targ);

		// 2. Setup the target orbit plane relative reference
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;

		h_targ_abs = AbsOfVector(h_targ);
		K_targ = DivideVector (h_targ, h_targ_abs);
		
		eVec = EccentricityVector(Mu_Planet, R_targ, V_targ);
		eVec_abs = AbsOfVector(eVec);
		I_targ = DivideVector(eVec, eVec_abs);

		RefYAxis = CrossProduct(K_targ, I_targ);
		RefYAxis_abs = AbsOfVector(RefYAxis);
		J_targ = DivideVector(RefYAxis, RefYAxis_abs);

		TMatrix = MakeTranslationMatrix( I_targ, J_targ, K_targ );
		
		// Get State vectors for our ship.
		Vessel=oapiGetFocusInterface();
		Vessel->GetRelativePos(hSetGbody, RPos);
		Vessel->GetRelativeVel(hSetGbody, VVel);

		RPos = ReorderVector(RPos);
		VVel = ReorderVector(VVel);
		
		MultiplyByMatrix(RPos, TMatrix, RPosF);
		MultiplyByMatrix(VVel, TMatrix, VVelF);
			
		// 4. Calculate n, i, then ascending node relative to the target orbit
		h = AngularMomentum(RPosF, VVelF);
		inc = Inclination(h);
		n = CrossProduct(KAxis, h);
		LongOfAN = LAN(n, inc);
		
		// Eccentricity (vector), True Anomaly and Argument of Periapsis,
		// and Argument of Ascension
		
		eVec = EccentricityVector(Mu_Planet, RPosF, VVelF);
		f = TrueAnomaly(eVec, RPosF, VVelF);
		ArgOfPeri = ArgOfPeriapsis(n,eVec);
		ArgOfAscension = f + ArgOfPeri; // angle since the ascending node.
		if (ArgOfAscension > 2*PI) {
			ArgOfAscension = ArgOfAscension - 2*PI;
		}
		
		// 5.  Calculate the times of the ascending and descending nodes.

		// Time of Adjusted (inverse) Argument of periapsis
		//   - Eccentric anomaly
		EccAnomaly = EccentricAnomaly(eVec, (2*PI - ArgOfPeri));
		//   - Mean anomaly
		MnAnomaly = MeanAnomaly(EccAnomaly, eVec);
		//   - Period of orbit
		SpMechE = SpecMechEnergy(VVelF, RPosF, Mu_Planet);
		sma = SemiMajorAxis(SpMechE, Mu_Planet);
		PeriodT = Period(sma, Mu_Planet);
		// AdjAOP Time since periapsis is
		TimeOfAdjAOP = TimeSincePeriapsis(MnAnomaly, PeriodT);
		
		// Calculate the current time since periapsis
		EccAnomaly = EccentricAnomaly(eVec, f);
		MnAnomaly = MeanAnomaly(EccAnomaly, eVec);
		// Time since periapsis is
		TimeOfTrueAnomaly = TimeSincePeriapsis(MnAnomaly, PeriodT);

		// Time to ascending node.
		TimeToAscendingNode = TimeOfAdjAOP - TimeOfTrueAnomaly;
		if (TimeToAscendingNode < 0) {
			TimeToAscendingNode = PeriodT + TimeToAscendingNode;
		}

		// Time to the descending node
		ArgOfPeriPlusPI = ArgOfPeri + PI; 
		if (ArgOfPeriPlusPI > 2*PI) {
			ArgOfPeriPlusPI = ArgOfPeriPlusPI - 2*PI;
		}
		//   - Eccentric anomaly
		EccAnomaly = EccentricAnomaly(eVec, (2*PI - ArgOfPeriPlusPI));
		//   - Mean anomaly
		MnAnomaly = MeanAnomaly(EccAnomaly, eVec);
		// time between periapsis and the descending node
		TimeOfAdjAOPPlusPI = TimeSincePeriapsis(MnAnomaly, PeriodT);
		TimeToDescendingNode = TimeOfAdjAOPPlusPI - TimeOfTrueAnomaly;
		if (TimeToDescendingNode < 0) {
			TimeToDescendingNode = PeriodT + TimeToDescendingNode;
		}

		// Delta V and burn time
		SemiLatRectum = SemiLatusRectum(Mu_Planet, h);
		if (TimeToDescendingNode < TimeToAscendingNode ){
			RadiusAtANorDN = TrueAnomaly2Radius(SemiLatRectum, AbsOfVector(eVec), (2*PI - ArgOfPeriPlusPI));
			TimeTillBurn = TimeToDescendingNode;
			Normal = true;
		} else {
			RadiusAtANorDN = TrueAnomaly2Radius(SemiLatRectum, AbsOfVector(eVec), (2*PI - ArgOfPeri));
			TimeTillBurn = TimeToAscendingNode;
			Normal = false;
		}
		// velocity at ascending or descending node
		VelAtNode = VisViva(Mu_Planet, RadiusAtANorDN, sma);
		// Delta V of manuever
		PADeltaV = 2 * VelAtNode * sin(inc/2);
		// Acceleration parameters
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();

		//Burn time
		BurnTime = (-exp(-PADeltaV/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust ;
		SimT = oapiGetSimTime();
		TimeOfBurn = SimT + (TimeTillBurn - (BurnTime / 2));

		PlaneMode = PLANE_MODE_BURN;
		RelIncLast = PI;
		TVesselName = TVessel->GetName();

	} 
	
	if (PlaneMode == PLANE_MODE_BURN) {
		// Switches for the control system
		if ((TimeOfBurn - SimT) < 60 && !BurnComplete) {
			EngineController3 = true;
		} else if (BurnComplete) {
			EngineController3 = false;
			Done.a = false;
			Done.b = false;
			Done.c = false;
			BurnComplete = false;
			PlaneMode = PLANE_MODE_NEED_TARGET;		// Cycle around
			MFDMode = MENU; // CCK 0;
		}
	}

}

// Update the Plane alignment display
void inline GuidanceMFD::UpdatePlaneDisplay()
{
	char Buffer[80];
	
	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::Orbital Plane Alignment::");
	TextOut(hDC, 5, 2*ROWH, Buffer, strlen(Buffer));
	SetTextColor(hDC, GREEN);
	
	if (PlaneMode == PLANE_MODE_NEED_TARGET) {
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Press Shift + T to enter target");
		TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "for plane alignment.");
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Press Shift + C to go back");
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
	}

	if (PlaneMode == PLANE_MODE_BURN){
		// Display
		sprintf(Buffer, "Target: %s", TVesselName);
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rel Inc. %.4f°", inc*DEG);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "DeltaV %.2f m/s", PADeltaV);
		TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn Time %.1f s", BurnTime);
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		if (LimitedBurnT) {
			sprintf(Buffer, "Burn time is limited to %.2f s", BurnTime);
			TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));
		}
		sprintf(Buffer, "Time to Auto Cntrl %.0f s", (TimeOfBurn - 60 - SimT) );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Time to Burn       %.0f s", (TimeOfBurn - SimT) );
		TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
	} 

}

void inline GuidanceMFD::CalcRendezvousDisplay()
{
	// Automatic Rendezvous algorithm
	MATRIX3 RTMatrix;
	VESSELSTATUS Status;
	ELEMENTS SoyuzElem, TargetElem;
	double RendvOrbit_a, RendvOrbit_T,  Hohmann_R;  // tf is transfer time
	OBJHANDLE hSetGbody;
	VECTOR3 R_RTarg, V_RTarg, h_RTarg, K_RTarg, I_RTarg, J_RTarg;
	VECTOR3 R_Soyuz, R_Soyuz_MR, R_RTarg_MR, V_Soyuz;  // MR = Mir (plane) relative
	double h_RTarg_abs, Soyuz_Angle, RTarg_Angle ;
	double Mir_T, Target_n;
	double Soyuz_T, Target_T;

	SimT = oapiGetSimTime();
	hSetGbody = oapiGetGbodyByName(Planet_Name);

	// Note that this can not be a switch because of the "fallthru".  One if block
	// can change the RendMode which means that the block for that mode will also get 
	// executed during the current function invocation
	if (RendMode == REND_MODE_HOHMANN_SETUP) {
		// Calculate Burn 1 lead angle and transfer time for Hohmann tranfer
		RTVessel = oapiGetVesselInterface (hRTargetVessel);
		
		Vessel->GetElements(SoyuzElem, J2000);
		RTVessel->GetElements(TargetElem, J2000);
		
		RendvOrbit_a = (SoyuzElem.a + TargetElem.a) / 2;
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;
		RendvOrbit_T = 2*PI*sqrt(RendvOrbit_a * RendvOrbit_a * RendvOrbit_a / Mu_Planet);
		RendvOrbit_tf = RendvOrbit_T / 2;

		// Synodic Period
		Soyuz_T = 2*PI*sqrt(pow(SoyuzElem.a, 3) / Mu_Planet);
		Target_T = 2*PI*sqrt(pow(TargetElem.a, 3) / Mu_Planet);		
		SynodicP = Soyuz_T * Target_T / (Target_T - Soyuz_T);
	
		Hohmann_R = TargetElem.a / SoyuzElem.a;
		B1LeadAngle = PI*( 1 - pow( (1 + Hohmann_R)/(2*Hohmann_R), 1.5)  );

		RendMode = REND_MODE_WAIT_SETUP_ANGLE;
	
	}
	
	if (RendMode == REND_MODE_WAIT_SETUP_ANGLE) {
		// Current lead angle
		// 1. state of Rendezvous target
		RTVessel->GetRelativeVel(hSetGbody, V_RTarg);
		RTVessel->GetRelativePos(hSetGbody, R_RTarg);
		R_RTarg = ReorderVector(R_RTarg);
		V_RTarg = ReorderVector(V_RTarg);

		h_RTarg = AngularMomentum(R_RTarg, V_RTarg);

		// 2. Setup the target orbit plane relative reference
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;

		h_RTarg_abs = AbsOfVector(h_RTarg);
		K_RTarg = DivideVector (h_RTarg, h_RTarg_abs);
		
		eVec = EccentricityVector(Mu_Planet, R_RTarg, V_RTarg);
		eVec_abs = AbsOfVector(eVec);
		I_RTarg = DivideVector(eVec, eVec_abs);

		RefYAxis = CrossProduct(K_RTarg, I_RTarg);
		RefYAxis_abs = AbsOfVector(RefYAxis);
		J_RTarg = DivideVector(RefYAxis, RefYAxis_abs);

		// translation matrix for ecliptic to target orbital plane
		RTMatrix = MakeTranslationMatrix( I_RTarg, J_RTarg, K_RTarg );

		// Get position and Soyuz and Mir in mir orbit plane ref.
		Vessel->GetRelativePos(hSetGbody, R_Soyuz);
		R_Soyuz = ReorderVector(R_Soyuz);
		MultiplyByMatrix(R_Soyuz, RTMatrix, R_Soyuz_MR);
		MultiplyByMatrix(R_RTarg, RTMatrix, R_RTarg_MR);

		// Current Lead angle
		Soyuz_Angle = GetAngle(R_Soyuz_MR);
		RTarg_Angle = GetAngle(R_RTarg_MR);

		if (RTarg_Angle > Soyuz_Angle) {
			CurrLeadAngle = RTarg_Angle - Soyuz_Angle;
		} else {
			CurrLeadAngle = 2*PI - (Soyuz_Angle - RTarg_Angle);
		}

		// Beta minus 2.5 minutes (150s)
		SetupAngle = ( 130 / (SynodicP)) * 2*PI;

		if (CurrLeadAngle < (SetupAngle + B1LeadAngle)) {
			// CCK SetupAngleReached = true;
			RendMode = REND_MODE_CALC_CWBURN1;

			// Set burn times
			Burn1ITime = SimT + 130;
			Rendezvous_I_Time = Burn1ITime + RendvOrbit_tf;
			CCB1_Time = Burn1ITime + (0.33 * RendvOrbit_tf);
			CCB1_tf = 0.66 * RendvOrbit_tf;
			CCB2_Time = Burn1ITime + (0.66 * RendvOrbit_tf);
			CCB2_tf = 0.33 * RendvOrbit_tf;
			CCB3_Time = Burn1ITime + (0.87 * RendvOrbit_tf);
			CCB3_tf = 0.13 * RendvOrbit_tf;
		}

		RTVesselName = RTVessel->GetName();
		
	} 
	
	if (RendMode == REND_MODE_CALC_CWBURN1) {
		// station orbit angular rotation n
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;
		RTVessel->GetElements(TargetElem, J2000);
		Mir_T = 2*PI*sqrt(TargetElem.a * TargetElem.a * TargetElem.a / Mu_Planet);
		Target_n = 2*PI / (Mir_T);

		//  Rendezvous Burn 1 CW solution
		if ( SimT < Burn1IgnitTime - 120) {
			dV0 = CWSolutionB1(RendvOrbit_tf, Target_n, dRZeroB1, dVZeroB1);
		} else if ( SimT > Burn1IgnitTime - 25 ) {
			dV0 = CWSolutionB1(RendvOrbit_tf, Target_n, dRZeroB1, dVZeroB1);
		}

		// translate TI burn dV to ecliptic frame.
		MultiplyByMatrix(dV0, InvTransMat, dV0_eclp);
		dV0_eclp = ReorderVector(dV0_eclp);
		DeltaVAttitude = dV0_eclp;
		AttitudeControllerOn = true;

		/* 
		// Hohmann Classic solution
		Vessel->GetElements(SoyuzElem, J2000);
		RTVessel->GetElements(TargetElem, J2000);
		RO_a = (SoyuzElem.a + TargetElem.a) / 2;
		RO_e = TargetElem.a / RO_a - 1;
		RO_h = sqrt(Mu_Planet * RO_a * (1 - (RO_e * RO_e)));
		RO_vperi = RO_h / SoyuzElem.a;
		SoyuzCircV = sqrt(Mu_Planet / SoyuzElem.a);
		dV0_classic = RO_vperi - SoyuzCircV;
		RO_vapo = RO_h / TargetElem.a;
		TargetCircV = sqrt(Mu_Planet / TargetElem.a);
		dVtf_classic = RO_vapo - TargetCircV;
		*/

		// Burn duration
		dV0_abs = AbsOfVector(dV0);
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();
		BurnDuration = (-exp(-dV0_abs/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust;

		// Burn 1 ignition time
		Burn1IgnitTime = Burn1ITime - (BurnDuration / 2);
		
		/*
		// translate burn1 dV to ecliptic frame.
		dV0_eclp = Matrix3TimesVector3(InvTransMat, dV0);
		

		// Convert dV0_eclp to pitch and heading.
		Heading = GetAngle_arot(dV0_eclp);
		HdgMag = sqrt(dV0_eclp.x* dV0_eclp.x + dV0_eclp.y*dV0_eclp.y);
		Pitch = atan(dV0_eclp.z / HdgMag);
		if (dV0_eclp.y <= 0 && dV0_eclp.z >= 0 ) {
			Pitch = PI - Pitch;
		} else if (dV0_eclp.y <= 0 && dV0_eclp.z < 0 ) {
			Pitch = -PI - Pitch;
		}
		*/

		// Check if we have reached Burn 1 ignition time.
		if ( SimT > Burn1IgnitTime) {
			RendMode = REND_MODE_CWBURN1;
		}
		
		// Debugging info
		Vessel->GetRelativeVel(hSetGbody, V_Soyuz);
		V_Soyuz = ReorderVector(V_Soyuz);
		Vessel->GetStatus(Status);

	} 
	
	if (RendMode == REND_MODE_CWBURN1) {
		// Burn 1
		EngineController4 = true;

		if (BurnComplete) {
			EngineController4 = false;
			AttitudeControllerOn = false;
			RendMode = REND_MODE_COAST_CCB1;

			// CCK
			KillAllThrustersSwitch = true;
		}
	
	} 
	
	if (RendMode == REND_MODE_COAST_CCB1) {

		Vessel->DeactivateNavmode(NAVMODE_KILLROT);
		
		if (SimT > (CCB1_Time - 80)) {
			RendMode = REND_MODE_CCB1_SETUP;
		}
		BurnComplete = false;
		AttitudeSet = false;

	} 
	
	if (RendMode == REND_MODE_CCB1_SETUP) {
		// Course correction burn 1 setup
		if (!CCB1SetupInitTimeDone) {
			CCB1SetupInitTime = SimT;
			CCB1SetupInitTimeDone = true;
		}
		
		// station orbit angular rotation n
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;
		RTVessel->GetElements(TargetElem, J2000);
		Mir_T = 2*PI*sqrt(TargetElem.a * TargetElem.a * TargetElem.a / Mu_Planet);
		Target_n = 2*PI / (Mir_T);

		//  Course correction burn 1 CW solution
		if ( SimT < CCB1_Time  - 60) {
			dV0 = CWSolutionB1((CCB1_tf + (80 - (SimT - CCB1SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		} else if ( SimT > CCB1_Time - 25 ) {
			dV0 = CWSolutionB1((CCB1_tf + (80 - (SimT - CCB1SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		}
		
		// Burn duration
		dV0_abs = AbsOfVector(dV0);
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();
		BurnDuration = (-exp(-dV0_abs/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust;

		// translate burn1 dV to ecliptic frame.
		MultiplyByMatrix(dV0, InvTransMat, dV0_eclp);
		dV0_eclp = ReorderVector(dV0_eclp);
		DeltaVAttitude = dV0_eclp;
		AttitudeControllerOn = true;

		if (BurnComplete) {
			RendMode = REND_MODE_COAST_CCB2;
			AttitudeControllerOn = false;
			KillAllThrustersSwitch = true;
			Vessel->ActivateNavmode(NAVMODE_KILLROT);
		} else {
			// Main engine on if CCB1 time reached
			if (SimT > CCB1_Time ) {
				EngineController4 = true;
			}
		}
			

	}
	
	if (RendMode == REND_MODE_COAST_CCB2) {
		// Coast to Course Correction burn 2

/*
		// Point Soyuz at the Target
		RTVessel->GetRelativePos(hSetGbody, R_RTarg);
		Vessel->GetRelativePos(hSetGbody, R_Soyuz);
		DeltaVAttitude = R_RTarg - R_Soyuz;
		AttitudeControllerOn = true;
		Vessel->DeactivateNavmode(NAVMODE_KILLROT);
*/

		if (SimT > (CCB2_Time - 80)) {
			RendMode = REND_MODE_CCB2_SETUP;
			AttitudeControllerOn = false;
			KillAllThrustersSwitch = true;
		}

		BurnComplete = false;
		AttitudeSet = false;
	} 
	
	if (RendMode == REND_MODE_CCB2_SETUP) {
		// Course correction burn 2 setup
		if (!CCB2SetupInitTimeDone) {
			CCB2SetupInitTime = SimT;
			CCB2SetupInitTimeDone = true;
		}
		
		// station orbit angular rotation n
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;
		RTVessel->GetElements(TargetElem, J2000);
		Mir_T = 2*PI*sqrt(TargetElem.a * TargetElem.a * TargetElem.a / Mu_Planet);
		Target_n = 2*PI / (Mir_T);

		//  Course correction burn 1 CW solution
		if ( SimT < CCB2_Time  - 60) {
			dV0 = CWSolutionB1((CCB2_tf + (80 - (SimT - CCB2SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		} else if ( SimT > CCB2_Time - 25 ) {
			dV0 = CWSolutionB1((CCB2_tf + (80 - (SimT - CCB2SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		}
		// Burn duration
		dV0_abs = AbsOfVector(dV0);
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();
		BurnDuration = (-exp(-dV0_abs/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust;

		// translate burn2 dV to ecliptic frame.
		MultiplyByMatrix(dV0, InvTransMat, dV0_eclp);
		dV0_eclp = ReorderVector(dV0_eclp);
		DeltaVAttitude = dV0_eclp;
		AttitudeControllerOn = true;

		if (BurnComplete) {
			RendMode = REND_MODE_COAST_CCB3;
			AttitudeControllerOn = false;
			KillAllThrustersSwitch = true;
//			Vessel->ActivateNavmode(NAVMODE_KILLROT);
		} else {
			// Main engine on if attitude correct
			if (SimT > CCB2_Time ) {
				EngineController4 = true;
			}
		}

	} 

	if (RendMode == REND_MODE_COAST_CCB3) {
		// Coast to Course Correction burn 4

		// Point Soyuz at the Target
		RTVessel->GetRelativePos(hSetGbody, R_RTarg);
		Vessel->GetRelativePos(hSetGbody, R_Soyuz);
		DeltaVAttitude = R_RTarg - R_Soyuz;
		AttitudeControllerOn = true;
		Vessel->DeactivateNavmode(NAVMODE_KILLROT);

		if (ProgradeOn) {
//			Vessel->ActivateNavmode(NAVMODE_KILLROT);
			ProgradeOn = false;
		}
		
		if (SimT > (CCB3_Time - 80)) {
			RendMode = REND_MODE_CCB3_SETUP;
			AttitudeControllerOn = false;
			KillAllThrustersSwitch = true;
		}

		BurnComplete = false;
		AttitudeSet = false;

	} 
	
	if (RendMode == REND_MODE_CCB3_SETUP) {
		// Course correction burn 3 setup
		if (!CCB3SetupInitTimeDone) {
			CCB3SetupInitTime = SimT;
			CCB3SetupInitTimeDone = true;
		}
		
		// station orbit angular rotation n
		GbodyMass = oapiGetMass(hSetGbody);
		Mu_Planet = GKSI * GbodyMass;
		RTVessel->GetElements(TargetElem, J2000);
		Mir_T = 2*PI*sqrt(TargetElem.a * TargetElem.a * TargetElem.a / Mu_Planet);
		Target_n = 2*PI / (Mir_T);

		//  Course correction burn 3 CW solution
		if ( SimT < CCB3_Time  - 60) {
			dV0 = CWSolutionB1((CCB3_tf + (80 - (SimT - CCB3SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		} else if ( SimT > CCB3_Time - 25 ) {
			dV0 = CWSolutionB1((CCB3_tf + (80 - (SimT - CCB3SetupInitTime))), Target_n, dRZeroB1, dVZeroB1);
		}

		// Burn duration
		dV0_abs = AbsOfVector(dV0);
		MaxThrust = Vessel->GetMaxThrust(ENGINE_MAIN);
		VesselMass= Vessel->GetMass();
		BurnDuration = (-exp(-dV0_abs/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust;

		// translate burn3 dV to ecliptic frame.
		MultiplyByMatrix(dV0, InvTransMat, dV0_eclp);
		dV0_eclp = ReorderVector(dV0_eclp);
		DeltaVAttitude = dV0_eclp;
		AttitudeControllerOn = true;

		if (BurnComplete) {
			RendMode = REND_MODE_COAST_REND;
			AttitudeControllerOn = false;
			KillAllThrustersSwitch = true;
//			Vessel->ActivateNavmode(NAVMODE_KILLROT);
			
		} else {
			// Main engine on if attitude correct
			if (SimT > CCB3_Time ) {
				EngineController4 = true;
			}
		}
			
	} 
	
	if (RendMode == REND_MODE_COAST_REND) {
		
		// Setup the Vel matching burn time
			RTVessel->GetRelativeVel(hSetGbody, V_RTarg);
			Vessel->GetRelativeVel(hSetGbody, V_Soyuz);
			DeltaVAttitude = V_Soyuz - V_RTarg;
			DeltaVAttitude.x = -DeltaVAttitude.x;
			DeltaVAttitude.y = -DeltaVAttitude.y;
			DeltaVAttitude.z = -DeltaVAttitude.z;
			VesselMass = Vessel->GetMass();
			BurnDuration = (-exp(-AbsOfVector(DeltaVAttitude)/ISP_VAC)+1)*VesselMass*ISP_VAC/MaxThrust;
			Burn1IgnitTime = Rendezvous_I_Time - (BurnDuration / 2);

		if (SimT > (Burn1IgnitTime - 80)) {
			// CCK Rend_Setup_Reached = true;
			RendMode = REND_MODE_REND_SETUP;
			AttitudeControllerOn = true;

		} else {
			// Point Soyuz at the Target
			RTVessel->GetRelativePos(hSetGbody, R_RTarg);
			Vessel->GetRelativePos(hSetGbody, R_Soyuz);
			DeltaVAttitude = R_RTarg - R_Soyuz;
			AttitudeControllerOn = true;
			Vessel->DeactivateNavmode(NAVMODE_KILLROT);
		}
	} 
	
	if (RendMode == REND_MODE_REND_SETUP) {
		// Check if we have reached Rendezvous Ignition time.

		if ( SimT > Burn1IgnitTime) {
			RendMode = REND_MODE_REND_BURN;
			BurnComplete = false;
		}
		
	} else if (RendMode == REND_MODE_REND_BURN) {
		// Rendezvous burn
		EngineController4 = true;

		if (BurnComplete) {
			RendMode = REND_MODE_POST_REND;
			EngineController4 = false;

			// CCK
			KillAllThrustersSwitch = true;
		}
	} 
	
	if (RendMode == REND_MODE_POST_REND) {
		// Post Rendezvous 
		if (ProgradeOn) {
			Vessel->DeactivateNavmode(NAVMODE_PROGRADE);
			KillAllThrustersSwitch = true;
			ProgradeOn = false;
		}

		RendMode = REND_MODE_STATIONKEEPING;

	} 

	if (RendMode == REND_MODE_STATIONKEEPING) {
		// Station Keeping
		SKTarget = RTVessel;
		StationKeepingOn = true;

		// Point Soyuz at the Target
		RTVessel->GetRelativePos(hSetGbody, R_RTarg);
		Vessel->GetRelativePos(hSetGbody, R_Soyuz);
		DeltaVAttitude = R_RTarg - R_Soyuz;
		AttitudeControllerOn = true;
	}
	
}

void inline GuidanceMFD::UpdateRendezvousDisplay()
{
	// Automatic Rendezvous algorithm
	char Buffer[80];
	
	SetTextColor(hDC, WHITE);
	sprintf(Buffer, "::Auto. Rendezvous::");
	TextOut(hDC, 5, 2*ROWH, Buffer, strlen(Buffer));
	SetTextColor(hDC, GREEN);

	if (RendMode == REND_MODE_NEED_TARGET) {
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Press Shift + T to enter target");
		TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "for plane alignment.");
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Press Shift + C to go back");
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));

	} 
	
	if (RendMode == REND_MODE_HOHMANN_SETUP) {

		sprintf(Buffer, "Hohmann Transfer Setup");
		TextOut(hDC, 5, 5*ROWH, Buffer, strlen(Buffer));
	
	} 
	
	if (RendMode == REND_MODE_WAIT_SETUP_ANGLE) {

		// Display
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Approach to TI Burn");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Target: %s", RTVesselName);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Transfer Time: %.1f sec", RendvOrbit_tf);
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Lead Angle: %.3f°", B1LeadAngle*DEG);
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Setup Lead Angle:   %.3f°", (SetupAngle + B1LeadAngle)*DEG);
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Current Lead Angle: %.3f°", CurrLeadAngle*DEG);
		TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));

		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		
	} 
	
	if (RendMode == REND_MODE_CALC_CWBURN1) {

		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "TI Burn Setup");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Burn Duration: %.1f s", BurnDuration);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "TI Burn in:    %.0f s", Burn1IgnitTime - SimT);
		TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV(0): %.2f, %.2f, %.2f", dV0.x, dV0.y, dV0.z );
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dR: %.0f, %.0f, %.0f", dRZeroB1.x, dRZeroB1.y, dRZeroB1.z );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV-: %.2f, %.2f, %.2f", dVZeroB1.x, dVZeroB1.y, dVZeroB1.z );
		TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));
		
		// CCK
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));

		// I just adjusted the line number, that's all
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 17*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 18*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);


		/*sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN); */
		// CCK End

	} 
	
	if (RendMode == REND_MODE_CWBURN1) {
		// Burn 1
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "TI Burn");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

		// CCK
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		// CCK End

	} 
	
	if (RendMode == REND_MODE_COAST_CCB1) {
		// Coast to Course Correction burn 1

		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Cruise to CCB1");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Time to CCB1 setup: %.0f s", (CCB1_Time - 80) - SimT);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
	} 
	
	if (RendMode == REND_MODE_CCB1_SETUP) {

		// Display
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Course Correction Burn 1");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Burn Duration : %.2f s", BurnDuration);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn begins in: %.0f s", CCB1_Time - SimT);
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV(0): %.2f, %.2f, %.2f", dV0.x, dV0.y, dV0.z );
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dR(0): %.0f, %.0f, %.0f", dRZeroB1.x, dRZeroB1.y, dRZeroB1.z );
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV-(0): %.2f, %.2f, %.2f", dVZeroB1.x, dVZeroB1.y, dVZeroB1.z );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		
		// CCK
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 13*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		
		// I just adjusted the line number, that's all
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 17*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 
	
	if (RendMode == REND_MODE_COAST_CCB2) {
		// Coast to Course Correction burn 2
		
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Cruise to CCB2");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Time to CCB2 setup: %.0f s", (CCB2_Time - 80) - SimT);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 
	
	if (RendMode == REND_MODE_CCB2_SETUP) {

		// Display
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Course Correction Burn 2");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Burn Duration : %.2f s", BurnDuration);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn begins in: %.0f s", CCB2_Time - SimT);
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV(0): %.2f, %.2f, %.2f", dV0.x, dV0.y, dV0.z );
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dR(0): %.0f, %.0f, %.0f", dRZeroB1.x, dRZeroB1.y, dRZeroB1.z );
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV-(0): %.2f, %.2f, %.2f", dVZeroB1.x, dVZeroB1.y, dVZeroB1.z );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 13*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		
		
		// I just adjusted the line number, that's all
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 17*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 

	if (RendMode == REND_MODE_COAST_CCB3) {
		// Coast to Course Correction burn 3
		
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Cruise to CCB3");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Time to CCB3 setup: %.0f s", (CCB3_Time - 80) - SimT);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 15*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 
	
	if (RendMode == REND_MODE_CCB3_SETUP) {

		// Display
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Course Correction Burn 3");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Burn Duration : %.2f s", BurnDuration);
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn begins in: %.0f s", CCB3_Time - SimT);
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV(0): %.2f, %.2f, %.2f", dV0.x, dV0.y, dV0.z );
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dR(0): %.0f, %.0f, %.0f", dRZeroB1.x, dRZeroB1.y, dRZeroB1.z );
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "dV-(0): %.2f, %.2f, %.2f", dVZeroB1.x, dVZeroB1.y, dVZeroB1.z );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 13*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 14*ROWH, Buffer, strlen(Buffer));
		
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 17*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 
	
	if (RendMode == REND_MODE_COAST_REND) {

		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Cruise to Rendezvous Burn");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Rendezvous in:             %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous Setup in:       %.0f s", (Burn1IgnitTime - 80) - SimT );
		TextOut(hDC, 5, 8*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Burn Duration:            %.1f s", BurnDuration );
		TextOut(hDC, 5, 11*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous burn begins in: %.0f s", Burn1IgnitTime - SimT );
		TextOut(hDC, 5, 12*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

	} 
	
	if (RendMode == REND_MODE_REND_SETUP) {
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Rendezvous Burn Setup");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		
		sprintf(Buffer, "Rendezvous Ignit in %.0f s", Burn1IgnitTime - SimT );
		TextOut(hDC, 5, 9*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Rendezvous in %.0f s", Rendezvous_I_Time - SimT );
		TextOut(hDC, 5, 10*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);

		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to Abort");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
		
	} 
	
	if (RendMode == REND_MODE_REND_BURN) {
		// Rendezvous burn
		
		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Rendezvous Burn");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);

		// CCK
		sprintf(Buffer, "Pitch:    %.2f°", Degree(MyPitch));
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		sprintf(Buffer, "Yaw:      %.2f°", Degree(MyHeading));
		TextOut(hDC, 5, 7*ROWH, Buffer, strlen(Buffer));
		// CCK End

	} 
	
	if (RendMode == REND_MODE_POST_REND) {
		// Post Rendezvous 
		sprintf(Buffer, "Rendezvous Done.");
		TextOut(hDC, 5, 6*ROWH, Buffer, strlen(Buffer));
		
	} 
	
	if (RendMode == REND_MODE_STATIONKEEPING) {
		// Station Keeping

		SetTextColor(hDC, YELLOW);
		sprintf(Buffer, "Station Keeping");
		TextOut(hDC, 5, 4*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREY);
		sprintf(Buffer, "Shift + C to End");
		TextOut(hDC, 5, 16*ROWH, Buffer, strlen(Buffer));
		SetTextColor(hDC, GREEN);
	}
}


// Station Keeping (A rewrite of code from Chris Knestrick)
void inline GuidanceMFD::StationKeeping(VESSEL *TargetVessel)
{
	VECTOR3 GVel, RelVel, GSpacecraftPos;
	const double DEADBAND = 0.001;
	double signal, Mass, MaxAttThrust;

	Mass = Vessel->GetMass();
	MaxAttThrust = Vessel->GetMaxThrust(ENGINE_ATTITUDE);
	
	// Calculate relative velocity (after Chris Knestrick)
	oapiGetGlobalPos(Vessel->GetHandle(), &GSpacecraftPos);
	Vessel->GetRelativeVel(TargetVessel->GetHandle(), GVel);
	Vessel->Global2Local((GVel + GSpacecraftPos), RelVel);

	// Left/Right
	if (fabs(RelVel.x) > DEADBAND) {
		signal = -(Mass * RelVel.x) / MaxAttThrust;

		if (signal > 1.0) {
			signal = 1.0;
		} else if (RelVel.x < -1.0) {
			signal = -1.0;
		}
	} else {
		signal = 0;
	}
	Vessel->SetAttitudeLinLevel(0, signal);

	// Up/Down
	if (fabs(RelVel.y) > DEADBAND) {
		signal = -(Mass * RelVel.y) / MaxAttThrust;

		if (signal > 1.0) {
			signal = 1.0;
		} else if (RelVel.y < -1.0) {
			signal = -1.0;
		}
	} else {
		signal = 0;
	}
	Vessel->SetAttitudeLinLevel(1, signal);

	// Forward/Backward
	if (fabs(RelVel.z) > DEADBAND) {
		signal = -(Mass * RelVel.z) / MaxAttThrust;

		if (signal > 1.0) {
			signal = 1.0;
		} else if (RelVel.z < -1.0) {
			signal = -1.0;
		}
	} else {
		signal = 0;
	}
	Vessel->SetAttitudeLinLevel(2, signal);

	//sprintf(oapiDebugString(),"Rel Vel x %.3f y %.3f z %.3f", RelVel.x, RelVel.y, RelVel.z);
}

// Kill all attitude and translation thrusters
void GuidanceMFD::KillAllThrusters()
{
	if (!HaveWaitedOneCycle) {
		HaveWaitedOneCycle = true;
	} else {
		Vessel->SetAttitudeLinLevel(0, 0);
		Vessel->SetAttitudeLinLevel(1, 0);
		Vessel->SetAttitudeLinLevel(2, 0);
		Vessel->SetAttitudeRotLevel(0, 0);
		Vessel->SetAttitudeRotLevel(1, 0);
		Vessel->SetAttitudeRotLevel(2, 0);
		HaveWaitedOneCycle = false;
		KillAllThrustersSwitch = false;
	}
}

// Returns the absolute value of a vector
double AbsOfVector(const VECTOR3 &Vec)
{
	double Result;
	Result = sqrt(Vec.x*Vec.x + Vec.y*Vec.y + Vec.z*Vec.z);
	return Result;
}

// Returns the inverse of a 3x3 matrix
MATRIX3 GuidanceMFD::Matrix3Inverse(const MATRIX3 &a)
{
	MATRIX3  z, b, c, d;
	
	b.m11 = 1;
	b.m12 = 0;
	b.m13 = 0;
	b.m21 = 0;
	b.m22 = 1;
	b.m23 = 0;
	b.m31 = 0;
	b.m32 = 0;
	b.m33 = 1;

	z.m11 = a.m11;
	z.m12 = a.m12;
	z.m13 = a.m13;
	z.m21 = a.m21;
	z.m22 = a.m22;
	z.m23 = a.m23;
	z.m31 = a.m31;
	z.m32 = a.m32;
	z.m33 = a.m33;

	// M[1](1/ z.m11)
	c.m11 = z.m11 / z.m11;
	c.m12 = z.m12 / z.m11;
	c.m13 = z.m13 / z.m11;
	d.m11 = b.m11 / z.m11;
	d.m12 = b.m12 / z.m11;
	d.m13 = b.m13 / z.m11;
	// z[1,2](-z.m21)
	c.m21 = z.m21 + (-z.m21 * c.m11);
	c.m22 = z.m22 + (-z.m21 * c.m12);
	c.m23 = z.m23 + (-z.m21 * c.m13);
	d.m21 = b.m21 + (-z.m21 * d.m11);
	d.m22 = b.m22 + (-z.m21 * d.m12);
	d.m23 = b.m23 + (-z.m21 * d.m13);
	// z[1,3](-z.m31)
	c.m31 = z.m31 + (-z.m31 * c.m11);
	c.m32 = z.m32 + (-z.m31 * c.m12);
	c.m33 = z.m33 + (-z.m31 * c.m13);
	d.m31 = b.m31 + (-z.m31 * d.m11);
	d.m32 = b.m32 + (-z.m31 * d.m12);
	d.m33 = b.m33 + (-z.m31 * d.m13);

	// M[2](1/ c.m22)
	z.m21 = c.m21 / c.m22;
	z.m22 = c.m22 / c.m22;
	z.m23 = c.m23 / c.m22;
	b.m21 = d.m21 / c.m22;
	b.m22 = d.m22 / c.m22;
	b.m23 = d.m23 / c.m22;
	// z[2,1](-c.m12)
	z.m11 = c.m11 + (-c.m12 * z.m21);
	z.m12 = c.m12 + (-c.m12 * z.m22);
	z.m13 = c.m13 + (-c.m12 * z.m23);
	b.m11 = d.m11 + (-c.m12 * b.m21);
	b.m12 = d.m12 + (-c.m12 * b.m22);
	b.m13 = d.m13 + (-c.m12 * b.m23);
	// z[2,3](-c.m32)
	z.m31 = c.m31 + (-c.m32 * z.m21);
	z.m32 = c.m32 + (-c.m32 * z.m22);
	z.m33 = c.m33 + (-c.m32 * z.m23);
	b.m31 = d.m31 + (-c.m32 * b.m21);
	b.m32 = d.m32 + (-c.m32 * b.m22);
	b.m33 = d.m33 + (-c.m32 * b.m23);

	// M[3](1/ z.m33)
	c.m31 = z.m32 / z.m33;
	c.m32 = z.m32 / z.m33;
	c.m33 = z.m33 / z.m33;
	d.m31 = b.m31 / z.m33;
	d.m32 = b.m32 / z.m33;
	d.m33 = b.m33 / z.m33;
	// z[3,1](-z.m13)
	c.m11 = z.m11 + (-z.m13 * c.m31);
	c.m12 = z.m12 + (-z.m13 * c.m32);
	c.m13 = z.m13 + (-z.m13 * c.m33);
	d.m11 = b.m11 + (-z.m13 * d.m31);
	d.m12 = b.m12 + (-z.m13 * d.m32);
	d.m13 = b.m13 + (-z.m13 * d.m33);
	// z[3,2](-z.m23)
	c.m21 = z.m21 + (-z.m23 * c.m31);
	c.m22 = z.m22 + (-z.m23 * c.m32);
	c.m23 = z.m23 + (-z.m23 * c.m33);
	d.m21 = b.m21 + (-z.m23 * d.m31);
	d.m22 = b.m22 + (-z.m23 * d.m32);
	d.m23 = b.m23 + (-z.m23 * d.m33);
	return d;
}

VECTOR3 GuidanceMFD::Ecliptic2Equatorial(const VECTOR3 &Vect)
{	
	// convert a vector from Eclitic frame to an equatorial frame
	VECTOR3 temp;
	VECTOR3 temp2;

	// This does nothing.
	temp.x = Vect.x;
	temp.y = Vect.y;
	temp.z = Vect.z;

	// x-axis rotation 
	temp2.x = temp.x ;
	temp2.y = cos(-Ob_EARTH)* temp.y + sin(-Ob_EARTH)*temp.z;
	temp2.z =  -sin(-Ob_EARTH)* temp.y + cos(-Ob_EARTH)*temp.z;
	return temp2;
}

// change the order of the vectors. because they are weird!
VECTOR3 GuidanceMFD::ReorderVector(const VECTOR3 &Vect)
{	
	VECTOR3 temp;

	// change the order of the vectors from left handed to normal.
	temp.x = Vect.x;
	temp.y = Vect.z;
	temp.z = Vect.y;
	return temp;
}

// returns the angle of the x & y components of a vector from the x axis
double GuidanceMFD::GetAngle(const VECTOR3 &Vect)
{	
	double temp;
	
	if (Vect.x > 0 && Vect.y > 0) {
		temp = atan(Vect.y / Vect.x);
	} else if (Vect.x < 0) {
		temp = PI + atan(Vect.y / Vect.x);
	} else { // x+ and y-
		temp = 2*PI + atan(Vect.y / Vect.x);
	}
	return temp;
}

// returns the angle of the y & x components of a vector from the y axis
double GuidanceMFD::GetAngle_arot(const VECTOR3 &Vect)
{	
	double temp;
	
	if (Vect.x >= 0 && Vect.y >= 0) {
		temp = -atan(Vect.x / Vect.y);
	} else if (Vect.y < 0) {
		temp = atan(Vect.x / Vect.y);
	} else { // x- and y+
		temp = -atan(Vect.x / Vect.y);
	}
	return temp;
}

// construct a x-axis rotation matrix
MATRIX3 GuidanceMFD::XRotMatrix(double theta)
{	
	MATRIX3 XRot;
	
	XRot.m11 = 1;
	XRot.m12 = 0;
	XRot.m13 = 0;
	XRot.m21 = 0;
	XRot.m22 = cos(theta);
	XRot.m23 = -sin(theta);
	XRot.m31 = 0;
	XRot.m32 = sin(theta);
	XRot.m33 = cos(theta);
	return XRot;
}

// construct a y-axis rotation matrix
MATRIX3 GuidanceMFD::YRotMatrix(double theta2)
{	
	MATRIX3 XRot;
	
	XRot.m11 = cos(theta2);
	XRot.m12 = 0;
	XRot.m13 = sin(theta2);
	XRot.m21 = 0;
	XRot.m22 = 1;
	XRot.m23 = 0;
	XRot.m31 = -sin(theta2);
	XRot.m32 = 0;
	XRot.m33 = cos(theta2);
	return XRot;
}

// construct a z-axis rotation matrix
MATRIX3 GuidanceMFD::ZRotMatrix(double theta3)
{	
	MATRIX3 XRot;
	
	XRot.m11 = cos(theta3);
	XRot.m12 = -sin(theta3);
	XRot.m13 = 0;
	XRot.m21 = sin(theta3);
	XRot.m22 = cos(theta3);
	XRot.m23 = 0;
	XRot.m31 = 0;
	XRot.m32 = 0;
	XRot.m33 = 1;
	return XRot;
}

// Calculate the Clohessy-Wiltshire M matrix
MATRIX3 GuidanceMFD::CWM(double n, double t)
{	
	MATRIX3 temp;
	double c, s;
	
	c = cos(n*t);
	s = sin(n*t);

	temp.m11 = 4 - 3 * c;
	temp.m12 = 0;
	temp.m13 = 0;
	temp.m21 = 6 * (s - n * t);
	temp.m22 = 1;
	temp.m23 = 0;
	temp.m31 = 0;
	temp.m32 = 0;
	temp.m33 = c;
	return temp;
}

// Calculate the Clohessy-Wiltshire N matrix
MATRIX3 GuidanceMFD::CWN(double n, double t)
{	
	MATRIX3 temp;
	double c, s;
	
	c = cos(n*t);
	s = sin(n*t);

	temp.m11 = s / n;
	temp.m12 = 2 / n * (1 - c);
	temp.m13 = 0;
	temp.m21 = -2 / n * (1 - c);
	temp.m22 = (4 * s - 3 * n * t) / n;
	temp.m23 = 0;
	temp.m31 = 0;
	temp.m32 = 0;
	temp.m33 = s / n;
	return temp;
}

// Calculate the Clohessy-Wiltshire S matrix
MATRIX3 GuidanceMFD::CWS(double n, double t)
{	
	MATRIX3 temp;
	double c, s;
	
	c = cos(n*t);
	s = sin(n*t);

	temp.m11 = 3 * n * s;
	temp.m12 = 0;
	temp.m13 = 0;
	temp.m21 = -6 * n * (1 - c);
	temp.m22 = 0;
	temp.m23 = 0;
	temp.m31 = 0;
	temp.m32 = 0;
	temp.m33 = -n * s;
	return temp;
}

// Calculate the Clohessy-Wiltshire T matrix
MATRIX3 GuidanceMFD::CWT(double n, double t)
{	
	MATRIX3 temp;
	double c, s;
	
	c = cos(n*t);
	s = sin(n*t);

	temp.m11 = c;
	temp.m12 = 2 * s;
	temp.m13 = 0;
	temp.m21 = -2 * s;
	temp.m22 = 4 * c - 3;
	temp.m23 = 0;
	temp.m31 = 0;
	temp.m32 = 0;
	temp.m33 = c;
	return temp;
}

// Inverse of a 2x2 matrix
MATRIX2 GuidanceMFD::Matrix2Inverse(MATRIX2 &mat)
{	
	MATRIX2 temp;
	double a, b, c, d, e, f, g, h, i, j, k, l;
	
	a = mat.m1;
	b = mat.m2;
	c = mat.m3;
	d = mat.m4;

	e = a - (b / d * c);
	f = 0;
	g = 0;
	h = d - (c / a * b);

	i = 1;
	j = -(b / d);
	k = -(c / a);
	l = 1;

	temp.m1 = i / e;
	temp.m2 = j / e;
	temp.m3 = k / h;
	temp.m4 = l / h;
	return temp;
}

// Multiply 2 2x2 matrices
MATRIX2 GuidanceMFD::Matrix2Multiply(MATRIX2 &mata, MATRIX2 &matb)
{	
	MATRIX2 temp;
	temp.m1 = mata.m1 * matb.m1 + mata.m2 * matb.m3;
	temp.m2 = mata.m1 * matb.m2 + mata.m2 * matb.m4;
	temp.m3 = mata.m3 * matb.m1 + mata.m4 * matb.m3;
	temp.m4 = mata.m3 * matb.m2 + mata.m4 * matb.m4;
	return temp;
}

// Multiply 2 3x3 matrices
MATRIX3 GuidanceMFD::Matrix3Multiply(MATRIX3 &a, MATRIX3 &b)
{	
	MATRIX3 c;
	c.m11 = a.m11*b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
	c.m12 = a.m11*b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
	c.m13 = a.m11*b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

	c.m21 = a.m21*b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
	c.m22 = a.m21*b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
	c.m23 = a.m21*b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

	c.m31 = a.m31*b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
	c.m32 = a.m31*b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
	c.m33 = a.m31*b.m13 + a.m32 * b.m23 + a.m33 * b.m33;
	return c;
}

// Multiply 2x2 matrix by a 2D vector 
VECTOR3 GuidanceMFD::MatrixVec2Multiply(MATRIX2 &mat, VECTOR3 &vec)
{	
	VECTOR3 temp;
	temp.x = mat.m1 * vec.x + mat.m2 * vec.y;
	temp.y = mat.m3 * vec.x + mat.m4 * vec.y;
	temp.z = 0;
	return temp;
}

// Clohessy-Wiltshire Solution for the V(0) burn of a rendezvouz
VECTOR3 GuidanceMFD::CWSolutionB1(double tf, double n, VECTOR3 &dRZero, VECTOR3 &dVMinusZero)
{
	OBJHANDLE hSetGbody;
	VECTOR3 R_RTarg, V_RTarg, h_RTarg, R_RTarg_MR, V_RTarg_MR ;
	VECTOR3 R_Soyuz, V_Soyuz, R_Soyuz_MR, V_Soyuz_MR;  // MR = Mir (plane) relative
	VECTOR3 I, J, K;
	double R_RTarg_abs, V_RTarg_abs, h_RTarg_abs;
	VECTOR3 dVPlusZero, dVZero;
	MATRIX3 M_t0, N_t0, InvN_t0, InvNM;
	VECTOR3 Omega_dR;
	
	// 1. Get state vectors of Soyuz and Mir
	hSetGbody = oapiGetGbodyByName(Planet_Name);
	RTVessel->GetRelativeVel(hSetGbody, V_RTarg);
	RTVessel->GetRelativePos(hSetGbody, R_RTarg);
	R_RTarg = ReorderVector(R_RTarg);
	V_RTarg = ReorderVector(V_RTarg);
	
	// 2. Angular momentum
	h_RTarg = CrossProduct(R_RTarg, V_RTarg);
	
	Vessel->GetRelativeVel(hSetGbody, V_Soyuz);
	Vessel->GetRelativePos(hSetGbody, R_Soyuz);
	R_Soyuz = ReorderVector(R_Soyuz);
	V_Soyuz = ReorderVector(V_Soyuz);

	R_RTarg_abs = AbsOfVector(R_RTarg);
	V_RTarg_abs = AbsOfVector(V_RTarg);
	h_RTarg_abs = AbsOfVector(h_RTarg);
	
	I = DivideVector(R_RTarg, R_RTarg_abs);
	J = DivideVector(V_RTarg, V_RTarg_abs);
	K = DivideVector(h_RTarg, h_RTarg_abs);

	TransMat = MakeTranslationMatrix( I, J, K );
	// inverse translation matrix
	InvTransMat = Matrix3Inverse(TransMat);

	// 3. rotate Soyuz and Mir state vectors onto Mir orbital plane
	MultiplyByMatrix(R_Soyuz, TransMat, R_Soyuz_MR);
	MultiplyByMatrix(V_Soyuz, TransMat, V_Soyuz_MR);
	MultiplyByMatrix(R_RTarg, TransMat, R_RTarg_MR);
	MultiplyByMatrix(V_RTarg, TransMat, V_RTarg_MR);

	// 4. calc dr(0) and dv(0)
	dRZero = R_Soyuz_MR - R_RTarg_MR;
	Omega_dR.x = -n * dRZero.y;
	Omega_dR.y = n * dRZero.x;
	Omega_dR.z = 0;
	dVMinusZero = V_Soyuz_MR - V_RTarg_MR - Omega_dR;
	
	// 5. Adjust the Target position to below Mir	
	R_RTarg_MR.x = R_RTarg_MR.x - RendDistance;
	dRZero = R_Soyuz_MR - R_RTarg_MR;

	M_t0 = CWM(n, tf);
	N_t0 = CWN(n, tf);
	InvN_t0 = Matrix3Inverse(N_t0);
	InvNM = Matrix3Multiply(InvN_t0, M_t0);
	dVPlusZero = Matrix3TimesVector3(InvNM, dRZero);
	dVPlusZero = MultiplyVector(dVPlusZero, -1);

	dVZero = dVPlusZero - dVMinusZero;
	return dVZero;
}

// Returns a Velocity at Periapsis or Apoapsis for a projected orbit
inline double GuidanceMFD::NewVelocity_AorP(double &Rapo, double &Rperi, double &Rnew)
{
	double a;
	double e;
	double h;
	double Vnew;

	a = (Rapo + Rperi)/2;
	e = (Rapo / a) - 1;
	h = sqrt(Mu_Planet * a * (1 - e*e));
	Vnew = h / Rnew;
	return Vnew;
}

// Returns the Eccentric Anomaly
inline double GuidanceMFD::EccentricAnomaly(const VECTOR3 &e, double f)
{
	double E;
	double e_abs;
	e_abs = AbsOfVector(e);
	E = 2 * atan(sqrt((1-e_abs)/(1+e_abs))*tan(f/2));
	return E;
}

// Returns the Mean Anomaly
inline double GuidanceMFD::MeanAnomaly(double E, const VECTOR3 &e)
{
	double Me;
	double e_abs;
	e_abs = AbsOfVector(e);
	Me = E - e_abs*sin(E);
	return Me;
}

// Returns the Semi Latus Rectum
inline double GuidanceMFD::SemiLatusRectum(double u, const VECTOR3 &h)
{
	double p;
	double h_abs;
	h_abs = AbsOfVector(h);
	p = (h_abs * h_abs) / u;
	return p;
}

// Returns the Radius for a given true anomaly
inline double GuidanceMFD::TrueAnomaly2Radius(double p, double e, double f)
{
	double R;
	R = p / (1 + e*cos(f));
	return R;
}

// Returns the argument of periapsis
inline double GuidanceMFD::ArgOfPeriapsis(VECTOR3 &n, VECTOR3 &e)
{
	double omega;
	double n_abs;
	double e_abs;

	n_abs = AbsOfVector(n);
	e_abs = AbsOfVector(e);
	omega = acos((n.x * e.x + n.y * e.y + n.z * e.z) / (n_abs * e_abs));
	if (e.z < 0) {
		omega = 2*PI - omega;
	}
	return omega;

}

// Returns the Period of the orbit
inline double GuidanceMFD::Period(double a, double u)
{
	double T;
	T = 2*PI*sqrt((a*a*a)/u);
	return T;

}

// Returns the time since periapsis for an anomaly
inline double GuidanceMFD::TimeSincePeriapsis(double Me, double T)
{
	double TSP;
	TSP = Me / (2*PI)*T;
	return TSP;
}

// Returns the Semi-Major Axis of the orbit
inline double GuidanceMFD::SemiMajorAxis(double E, double u)
{
	// E is specific mechanical energy
	double a;
	a = -u / (2 * E);
	return a;

}

// Returns Specific Mechanical Energy (E)
inline double GuidanceMFD::SpecMechEnergy(const VECTOR3 &V, const VECTOR3 &R, double u)
{
	double SME;
	double V_abs, R_abs;
	V_abs = AbsOfVector(V);
	R_abs = AbsOfVector(R);
	SME = (V_abs*V_abs / 2) - (u / R_abs);
	return SME;

}

// Returns the vector of eccentricity.
inline VECTOR3 GuidanceMFD::EccentricityVector(double u, const VECTOR3 R2, const VECTOR3 V2)
{
	VECTOR3 ecc;
	double V_abs, R_abs, Part1a, Part2a;
	VECTOR3 Part1;
	VECTOR3 Part2;

	V_abs = AbsOfVector(V2);
	R_abs = AbsOfVector(R2);
	Part1a = V_abs*V_abs - (u / R_abs);
	Part1 = MultiplyVector(R2, Part1a);
	Part2a = DotProduct(R2, V2);
	Part2 = MultiplyVector(V2, Part2a); 

	Part1 = Part1 - Part2;
	ecc = Part1 * (1/u);
	return ecc;
}

// Returns the true anomaly
inline double GuidanceMFD::TrueAnomaly(VECTOR3 &e, VECTOR3 &R, VECTOR3 &V)
{
	double f;
	double e_abs;
	double R_abs;

	e_abs = AbsOfVector(e);
	R_abs = AbsOfVector(R);

	f = acos((R.x * e.x + R.y * e.y + R.z * e.z) / (R_abs * e_abs));
	if ( DotProduct(R, V) < 0){
		f = 2*PI - f;
	}
	return f;
}

// Returns the Angular momentum vector (h)
inline VECTOR3 GuidanceMFD::AngularMomentum(const VECTOR3 &R, const VECTOR3 &V)
{
	VECTOR3 h;

	h = CrossProduct(R, V);
	return h;
}

// Returns Longitude of the Ascending Node
inline double GuidanceMFD::LAN(const VECTOR3 &n, double i)
{
	double n_abs;
	double LAN;

	// calculate vector n and magnitude of n
	n_abs = AbsOfVector(n);
	
	// Longitude of the Ascending node.
	if (i == 0) {
		LAN = 0;
	} else if (n.y < 0) {
		LAN = 2*PI - acos(n.x / n_abs);
	} else {
		LAN = acos(n.x / n_abs);
	}
	return LAN;
}

// Returns Inclination
inline double GuidanceMFD::Inclination(const VECTOR3 &h)
{
	double i;
	double h_abs;
	h_abs = AbsOfVector(h);
	i = acos(h.z / h_abs);
	return i;
}

// Returns the velocity for a given radius. Vis-viva equation
inline double GuidanceMFD::VisViva(double u, double r, double a)
{
	double V;
	V = sqrt(u * ( (2/r) - (1/a) ));
	return V;
}

// Makes a prograde or retrograde burn to a target speed.
void GuidanceMFD::EngController1(double simt, double Vnew, double BurnTInit)
{

	OBJHANDLE hSetGbody;
	VECTOR3 RelVelocity;

	hSetGbody = oapiGetGbodyByName(Planet_Name);

	if (Prograde && !Done.a) {
		Vessel->ActivateNavmode(NAVMODE_PROGRADE);
		Done.a = true;
	} else if (!Prograde && !Done.a) {
		Vessel->ActivateNavmode(NAVMODE_RETROGRADE);
		Done.a = true;
	}
	// Get current velocity
	Vessel->GetRelativeVel(hSetGbody, RelVelocity);
	Velocity = AbsOfVector(RelVelocity);
	
	VelError = (Velocity - Vnew * 1000);
	if (Prograde) {
		VelError = VelError * -1;
	}

	if ( (BurnTInit - simt) < 0 && !Done.b) {
		Vessel->SetEngineLevel(ENGINE_MAIN, 1);
		Done.b = true;
	} 
	
	//sprintf(oapiDebugString(),"BurnTInit1 %.2lf Simt: %.2lf, Velocity Err %f, VnewBurn1 %.2f , Velocity %.2f", BurnTInit1, SimT, VelError, VnewBurn1, Velocity);
	if ( Done.b && VelError < 0 && !Done.c)
	{
		oapiSetEngineLevel(hVessel, ENGINE_MAIN, 0);
		if (Prograde) {
			Vessel->DeactivateNavmode(NAVMODE_PROGRADE);
		} else {
			Vessel->DeactivateNavmode(NAVMODE_RETROGRADE);
		}
		Done.c = true;
		
		Vessel->ActivateNavmode(NAVMODE_KILLROT);	
		BurnComplete = true;
	}
}

// Plane alignment engine controller
void GuidanceMFD::EngController2(double simt, double BurnTInit)
{
	OBJHANDLE hSetGbody;
	hSetGbody = oapiGetGbodyByName(Planet_Name);
	//oapiSetTimeAcceleration (1);

	if (Normal && !Done.a) {
		Vessel->ActivateNavmode(NAVMODE_NORMAL);
		RelIncLast = PI;
		Done.a = true;
	} else if (!Normal && !Done.a) {
		Vessel->ActivateNavmode(NAVMODE_ANTINORMAL);
		RelIncLast = PI;
		Done.a = true;
	}

	// Error in inclination
	TVessel->GetRelativeVel(hSetGbody, V_targ);
	TVessel->GetRelativePos(hSetGbody, R_targ);
	
	R_targ = ReorderVector(R_targ);
	V_targ = ReorderVector(V_targ);

	h_targ = AngularMomentum(R_targ, V_targ);

	// 2. Setup the target orbit plane relative reference
	GbodyMass = oapiGetMass(hSetGbody);
	Mu_Planet = GKSI * GbodyMass;

	h_targ_abs = AbsOfVector(h_targ);
	K_targ = DivideVector (h_targ, h_targ_abs);
	
	eVec = EccentricityVector(Mu_Planet, R_targ, V_targ);
	eVec_abs = AbsOfVector(eVec);
	I_targ = DivideVector(eVec, eVec_abs);

	RefYAxis = CrossProduct(K_targ, I_targ);
	RefYAxis_abs = AbsOfVector(RefYAxis);
	J_targ = DivideVector(RefYAxis, RefYAxis_abs);

	TMatrix = MakeTranslationMatrix( I_targ, J_targ, K_targ );
	
	// Get State vectors for our ship.
	Vessel=oapiGetFocusInterface();
	Vessel->GetRelativePos(hSetGbody, RPos);
	Vessel->GetRelativeVel(hSetGbody, VVel);
	
	RPos = ReorderVector(RPos);
	VVel = ReorderVector(VVel);
	
	MultiplyByMatrix(RPos, TMatrix, RPosF);
	MultiplyByMatrix(VVel, TMatrix, VVelF);
		
	// 4. Calculate relative inclination
	h = AngularMomentum(RPosF, VVelF);
	inc = Inclination(h);
	if (inc < 0) {
		inc = -1 * inc;
	}
	
	//sprintf(oapiDebugString(),"Simt %.2f   RInc: %.6f ", simt, inc);
	
	if ( (BurnTInit - simt) < 0 && !Done.b) {
		Vessel->SetEngineLevel(ENGINE_MAIN, 1);
		Done.b = true;
	} else if ( (Done.b && (inc > RelIncLast) ) && !Done.c)
	{
		oapiSetEngineLevel(hVessel, ENGINE_MAIN, 0);
		if (Normal) {
			Vessel->DeactivateNavmode(NAVMODE_NORMAL);
		} else {
			Vessel->DeactivateNavmode(NAVMODE_ANTINORMAL);
		}
		Done.c = true;
		BurnComplete = true;
		Vessel->ActivateNavmode(NAVMODE_KILLROT);	
				
	}
	// Store the current rel inclination for the next interation
	RelIncLast = inc;
}

// Fire the main engine for a set period
void GuidanceMFD::EngController4(double simt, double Duration)
{
	if (!Done.a) {
		TimeStop = simt + Duration;
		Vessel->SetEngineLevel(ENGINE_MAIN, 1);
		Done.a = true;
	}
	if (simt > TimeStop) {
		Vessel->SetEngineLevel(ENGINE_MAIN, 0);
		EngineController4 = false;
		AttitudeControllerOn = false;
		BurnComplete = true;
		Done.a = false;

		// CCK Stop all unwanted rotations
		Vessel->ActivateNavmode(NAVMODE_KILLROT);	
	}
}


// Attitude controller
void GuidanceMFD::AttitudeController(VECTOR3 TargetVector, double simt, double simdt)
{
	double Distance;
	VECTOR3 LTargetVector;
	VECTOR3 Pos;	

	oapiGetGlobalPos(Vessel->GetHandle(), &Pos);

	Vessel->Global2Local(TargetVector + Pos, LTargetVector);

	// CCK - changed RelPos to LTargetVector
	Distance = sqrt(pow(LTargetVector.x, 2) + pow(LTargetVector.y, 2) + pow(LTargetVector.z, 2));

	MyPitch = asin(LTargetVector.y / Distance);
	MyHeading = acos(LTargetVector.z/(Distance * sin(acos(LTargetVector.y / Distance))));
	if (LTargetVector.x > 0) {
		MyHeading = -MyHeading;
	}

	if (LTargetVector.z < 0) {
		MyPitch = -MyPitch;
	}

	SetAttitude(0, -MyPitch, PITCH, DB_FINE);
	SetAttitude(0, -MyHeading, YAW, DB_FINE);

	// Kill any roll 
	NullRate(ROLL);

	//sprintf(oapiDebugString(),"x %.3f y %.3f z %.3f, T x %.3f y %.3f z %.3f, Pitch: %.2f Heading %.2f Roll %.2f VecErr %.3f", MyVector.x, MyVector.y, MyVector.z, TargetVector.x, TargetVector.y, TargetVector.z, MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, VectorErrAbs);
	//sprintf(oapiDebugString(),"Pitch: %.2f Heading %.2f Roll %.2f, Pitchdot %.2f, HeadingDot %.2f", MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, PitchDot*DEG, HeadingDot*DEG);

}

// On-orbit cruise attitude controller
void GuidanceMFD::LHLV_AttControl(double simt, bool Off)
{
	double Distance;
	double Bank, T, XAxisRate, x_vrot_err;
	ELEMENTS Elemnts;
	VESSELSTATUS CurrStatus;
	double Pitch, PitchRate;
	
	// Planet parameters
	hSetGbody = oapiGetGbodyByName(Planet_Name);
	GbodyMass = oapiGetMass(hSetGbody);
	Mu_Planet = GK * GbodyMass;

	Vessel->GetStatus(CurrStatus);

	// Get the retrograde velocity vector 
	Vessel->GetRelativeVel(hSetGbody, TargetVector3);
	TargetVector3.x = -TargetVector3.x;
	TargetVector3.y = -TargetVector3.y;
	TargetVector3.z = -TargetVector3.z;
	
	// CCK - changed RelPos to LTargetVector
	oapiGetGlobalPos(Vessel->GetHandle(), &Pos3);
	Vessel->Global2Local(TargetVector3 + Pos3, LTargetVector3);
	
	Distance = sqrt(pow(LTargetVector3.x, 2) + pow(LTargetVector3.y, 2) + pow(LTargetVector3.z, 2));
	MyPitch = asin(LTargetVector3.y / Distance);
	MyHeading = acos(LTargetVector3.z/(Distance * sin(acos(LTargetVector3.y / Distance))));
	if (LTargetVector3.x > 0) {
		MyHeading = -MyHeading;
	}

	if (LTargetVector3.z < 0) {
		MyPitch = -MyPitch;
	}
	
	// Roll angle
	Bank = Vessel->GetBank();
	if (Bank < 0) {
		Bank = PI + Bank;
	} else {
		Bank = -PI + Bank;
	}
	
	// Required rotation rate.
	Vessel->GetElements(Elemnts, J2000);
	T = 2*PI*sqrt((Elemnts.a*Elemnts.a*Elemnts.a/1e9)/Mu_Planet);
	XAxisRate = 2*PI / T;
	
	Pitch = Vessel->GetPitch();
	PitchRate = (Pitch - PitchLast) / (simt - TimeLast);
	TimeLast = simt;
	PitchLast= Pitch;

	// Send error signals to the controller.
	if (fabs(MyPitch) > 2*RAD || fabs(MyHeading) > 0.2*RAD || fabs(Bank) > 2*RAD) {
		SetAttitude(0, -MyPitch, PITCH, DB_FINE);
		SetAttitude(0, -MyHeading, YAW, DB_FINE);
		SetAttitude(0, -Bank, ROLL, DB_FINE);
	} else {
		// x-axis rotation
		x_vrot_err = CurrStatus.vrot.x - PitchRate;
		if (x_vrot_err < 0 ){
			x_vrot_err = 0;
		}
		Vessel->SetAttitudeRotLevel(0, -20 * x_vrot_err);

		SetAttitude(0, -MyHeading, YAW, DB_FINE);
		SetAttitude(0, -Bank, ROLL, DB_FINE);
		//sprintf(oapiDebugString(),"XAxisRate %.4f, x_vrot_err %.4f, PitchRate %.2f", XAxisRate*DEG, x_vrot_err*DEG, PitchRate*DEG);
	}
			
	// If we get the OFF flag then Disengage Controller
	if (Off) {
		LHLV_AttControlOn = false;
		KillAllThrustersSwitch = true;
		LHLV_AC_OFF = false;
	}

	//sprintf(oapiDebugString(),"x %.3f y %.3f z %.3f, T x %.3f y %.3f z %.3f, Pitch: %.2f Heading %.2f Roll %.2f VecErr %.3f", MyVector.x, MyVector.y, MyVector.z, TargetVector.x, TargetVector.y, TargetVector.z, MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, VectorErrAbs);
	//sprintf(oapiDebugString(),"Pitch: %.2f Heading %.2f Roll %.2f, Pitchdot %.2f, HeadingDot %.2f", MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, PitchDot*DEG, HeadingDot*DEG);

}

// Attitude controller to point the star tracker at a target
void GuidanceMFD::StarTrackAttController(VECTOR3 TargetVector)
{
	double Distance;
	VECTOR3 LTargetVector;
	VECTOR3 Pos;	

	oapiGetGlobalPos(Vessel->GetHandle(), &Pos);
	Vessel->Global2Local(TargetVector + Pos, LTargetVector);
	Distance = sqrt(pow(LTargetVector.x, 2) + pow(LTargetVector.y, 2) + pow(LTargetVector.z, 2));

	MyPitch = asin(LTargetVector.y / Distance);
	MyHeading = acos(LTargetVector.z/(Distance * sin(acos(LTargetVector.y / Distance))));
	if (LTargetVector.x > 0) {
		MyHeading = -MyHeading;
	}

	if (LTargetVector.z < 0) {
		MyPitch = -MyPitch;
	}

	SetAttitude(-90*RAD, -MyPitch, PITCH, DB_FINE);
	SetAttitude(0, -MyHeading, YAW, DB_FINE);

	// Kill any roll 
	NullRate(ROLL);

	//sprintf(oapiDebugString(),"x %.3f y %.3f z %.3f, T x %.3f y %.3f z %.3f, Pitch: %.2f Heading %.2f Roll %.2f VecErr %.3f", MyVector.x, MyVector.y, MyVector.z, TargetVector.x, TargetVector.y, TargetVector.z, MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, VectorErrAbs);
	//sprintf(oapiDebugString(),"Pitch: %.2f Heading %.2f Roll %.2f, Pitchdot %.2f, HeadingDot %.2f", MyPitch*DEG, MyHeading*DEG, Status.arot.z*DEG, PitchDot*DEG, HeadingDot*DEG);

}

// 3x3 matrix times at 3d vector.  Same as function below
VECTOR3 GuidanceMFD::Matrix3TimesVector3( const MATRIX3 &a, const VECTOR3 &b)
{
	VECTOR3 c;
	c.x = a.m11*b.x + a.m12 * b.y + a.m13 * b.z;
	c.y = a.m21*b.x + a.m22 * b.y + a.m23 * b.z;
	c.z = a.m31*b.x + a.m32 * b.y + a.m33 * b.z;
	return c;	
}

// Taken from Chris Knestric code
void GuidanceMFD::MultiplyByMatrix(const VECTOR3 &Initial, const MATRIX3 &RotMatrix, VECTOR3 &Result)
{

	Result.x =	(Initial.x * RotMatrix.m11) 
				+ (Initial.y * RotMatrix.m12) 
				+ (Initial.z * RotMatrix.m13);
	
	Result.y =	(Initial.x * RotMatrix.m21) 
				+ (Initial.y * RotMatrix.m22) 
				+ (Initial.z * RotMatrix.m23);
	
	Result.z =	(Initial.x * RotMatrix.m31) 
				+ (Initial.y * RotMatrix.m32) 
				+ (Initial.z * RotMatrix.m33);
}

// Returns the cross product of two vectors
inline VECTOR3 GuidanceMFD::CrossProduct(const VECTOR3 &a, const VECTOR3 &b)
{
	VECTOR3 c;

	c.x = (a.y * b.z - a.z * b.y);
	c.y = (a.z * b.x - a.x * b.z);
	c.z = (a.x * b.y - a.y * b.x);

	return c;
}

// Divide a vector by a scalar double
inline VECTOR3 GuidanceMFD::DivideVector (const VECTOR3 &a, double b)
{
	VECTOR3 c;
	c.x = a.x / b;
	c.y = a.y / b;
	c.z = a.z / b;

	return c;
}

// Multiply a vector by a scalar double
inline VECTOR3 GuidanceMFD::MultiplyVector (const VECTOR3 &a, double b)
{
	VECTOR3 c;
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;

	return c;
}
// End Chris's code

// Dot product of two vectors
inline double GuidanceMFD::DotProduct (const VECTOR3 &a, const VECTOR3 &b)
{
	double c;
	c = a.x * b.x + a.y * b.y + a.z * b.z;
	return c;
}

bool GuidanceMFD::SetTargetAP (char *rstr)
{
	if (sscanf (rstr, "%lf%lf", &PerigeeSP, &ApogeeSP) == 2 && PerigeeSP < ApogeeSP) {
		OrbitMode = ORBIT_MODE_CALC1;
		return true;
	} else {
	return false;
	}
}

inline MATRIX3 GuidanceMFD::MakeTranslationMatrix( const VECTOR3 &I, const VECTOR3 &J,const VECTOR3 &K )
{
	MATRIX3 TransM;

	TransM.m11 = I.x;
	TransM.m12 = I.y;
	TransM.m13 = I.z;
	
	TransM.m21 = J.x;
	TransM.m22 = J.y;
	TransM.m23 = J.z;

	TransM.m31 = K.x;
	TransM.m32 = K.y;
	TransM.m33 = K.z;

	return TransM;
}

// sets the vessel that the INFO page displays info on.
bool GuidanceMFD::SetTargetInfo (char *rstr)
{
	char target[30];

	if (sscanf (rstr, "%s", &target) < 29) {
		
		if ( oapiGetVesselByName(target) != NULL){
			hInfoVessel = oapiGetVesselByName(target);
			return true;
		} else {
			return false;
		}

	} else {
	return false;
	}
}

// sets the target vessel for plane alignment.
bool GuidanceMFD::SetTargetPlane (char *rstr)
{
	char target[30];

	if (sscanf (rstr, "%s", &target) < 29) {
		
		if ( oapiGetVesselByName(target) != NULL){
			hTargetVessel = oapiGetVesselByName(target);
			PlaneMode = PLANE_MODE_CALC_BURN;
			return true;
		} else {
			return false;
		}

	} else {
	return false;
	}
}

// sets the target vessel for Rendevzous.
bool GuidanceMFD::SetTargetRendezvous (char *rstr)
{
	char target[30];

	if (sscanf (rstr, "%s", &target) < 29) {
		
		if ( oapiGetVesselByName(target) != NULL){
			hRTargetVessel = oapiGetVesselByName(target);
			// CCK GotRendezvousTarget = true;
			RendMode = REND_MODE_HOHMANN_SETUP;
			return true;
		} else {
			return false;
		}

	} else {
	return false;
	}
}

bool APInput(void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetAP (str);
}

bool GuidanceMFD::SetTargetMenu (char *rstr)
{
	if (sscanf (rstr, "%d", &MFDMode) == 1 && MFDMode < 5 && MFDMode > 0) {
		
		return true;
	} else {
	return false;
	}
}

bool MenuInput(void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetMenu (str);
}

bool InfoInput(void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetInfo (str);
}

bool PlaneInput(void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetPlane (str);
}

bool RendezvousInput(void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetRendezvous (str);
}

bool GuidanceMFD::SetTargetTime (char *rstr)
{
	int input;
	sscanf (rstr, "%d", &input);
	//Spacecraft.time=(double)(input*60);
	return true;
}

bool TimeInput (void *id, char *str, void *data)
{	
	return ((GuidanceMFD*)data)->SetTargetTime (str);
}

