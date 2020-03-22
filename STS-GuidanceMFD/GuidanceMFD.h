/////////////////////////////////////////////////////////////////////////////////
//
//	GuidanceMFD.h - Header file for the STS Guidance & Control MFD 
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

#ifndef GuidanceMFD_H
#define GuidanceMFD_H

#define RED RGB(255, 0, 0)
#define YELLOW RGB(255, 255, 0)
#define GREEN RGB(0, 255, 0)
#define WHITE RGB(255, 255, 255)
#define BLUE RGB(0, 0, 255)
#define GREY RGB(160, 160, 160)

typedef struct {     // Orbital periapsis and apoapsis
	double peri;        //   periapsis (km)
	double apo;        //    apoapsis (km)
} ORBIT_AP;

typedef struct {     // a stack of doubles for calcs.
	double One;       
	double Two;
	double Three;
} CALC_STACK;

typedef struct {   // Array of switches for done flags
	bool a ;
	bool b ;
	bool c ;
	bool d ;
	bool e ;
} DONE_STACK;

typedef union {      // 2x2 matrix
	double data[4];
	struct { double m1, m2, m3, m4; };
} MATRIX2;

typedef union {      // 2D vector
	double data[2];
	struct { double x, y; };
} VECTOR2;


// CCK How often should update calculatons be done?  
const double CALC_INTERVAL = 1.0;	

const double R_Earth = 6371.01; 
const double R_Earth_SI = 6371.01 * 1000;
//const int ROWH = 16; // Height of a row of text
//MFD mode codes
enum MFD_MODE { MENU, INFO, ORBIT, PLANE, RENDEZVOUS };
const double GK = 6.67259e-20 ;//  Gravitational constant (km^3/(kg sec^2))
const double GKSI = 6.67259e-20 * 1e9;
double J2000 = 51544; // J2000 reference epoch in MJD
const double Ob_EARTH = 23.45 * RAD ; // Obliquity of the Earth (rad)
const double ecl = -23.429292*RAD;
VESSEL *Vessel;
OBJHANDLE hVessel;
char Planet_Name[6] = "Earth";
VECTOR3 KAxis;
double ISP_VAC = 5000; // ISP of main engine in a vacuum m/s (for Soyuz 7K-OK)
// Controller constants
const double D = 3;
const double K = 0.4;
const double Dr = 1.2;
const double Dp = 5;
const double Dh = 4;
const double Kr = 4;
const double MaxVRot = 0.10;
const double MaxAttThrust = 0.5;
const double MaxRollThrust = 0.5;
const double MaxHeadThrust = 0.3;
const double MaxPitchThrust = 0.2;


// Global Functions
double AbsOfVector(const VECTOR3 &Vec);

// UPDATE ORBIT
MFD_MODE MFDMode = MENU;
double PerigeeSP = 5;			  // Perigee setpoint (km x 10)
double ApogeeSP = 10;			  // Apogee setpoint (km x 10)

// The modes that are possible for the Orbit program
enum ORBIT_MODE { 
	ORBIT_MODE_NEED_PARAMS,		// No target orbit has been entered
	ORBIT_MODE_CALC1,			// Calculate the first burn
	ORBIT_MODE_BURN1,			// Setup and perform the first burn
	ORBIT_MODE_CALC2,			// Calculate the second burn
	ORBIT_MODE_BURN2			// Setup and perform the second burn
};

double tsp = 0 ;      // time since periapsis (s)
double T = 0; // orbital period (s)
double R; // current radius of the orbit (km).
double p; // semi-latus rectum (km)
double v; // true anomaly (rad)
double E; // eccentric anomaly
double Me; // mean anomaly
double TtApo; // time to apoapsis (s)
double TtPeri; // time to periapsis (s)
double TimeBurn1;  // Sim time of Burn 1 (s)
double TimeBurn2;
double SimT ;  // Sim Time (s)
ORBIT_AP Orbit1; // [km]
ORBIT_AP Orbit2;
CALC_STACK Ans;
double Mu_Planet; // Gravitational parameter for planet
double GbodySize;  // mean radius (km)
double RDotV;   // Dot product of velocity and postion vector
double VnewBurn1; // new velocity for Burn 1
double VnewBurn2;
double DeltaVBurn1;
double DeltaVBurn2;
double DeltaTBurn1; // Burn time 1
double DeltaTBurn2; 
double BurnTInit1 = 0;   // Time of engine startup (Simt [s])
double BurnTInit2 = 100;
double Orbit1StaticR; // Orbit one static point Radius [km]
double Orbit2StaticR;
double MaxThrust;    // Maximum engine thrust [N]
double VesselMass;   // [kg]
bool Prograde = true;  //  true = Prograde,  false = retrograde 
bool EngineController1 = false ; //Hi res controller for Orbit height adjustment 
bool EngineController2 = false ; // orbit height  second burn controller
double Velocity;		// current vessel velocity
DONE_STACK Done; // array of 5 'done' switches
DONE_STACK AttDone; // array of 5 done switches for the attitude controller
double VelError;  // Difference between Vel and VelSP (absolute)
VECTOR3 n;		  // Node vector.
double GbodyMass; //mass of Gbody in kg

// Plane MFD
// The modes that are possible for the plane alignment program
enum PLANE_MODE { 
	PLANE_MODE_NEED_TARGET,		// No target plane has been entered
	PLANE_MODE_CALC_BURN,		// Calculating the required burn
	PLANE_MODE_BURN,			// Plane change setup and burn
};

VESSEL *TVessel;  // handle of target vessel
char *TVesselName;	// Target vessel name
OBJHANDLE hTargetVessel;
bool GotPlaneTarget = false;
VECTOR3 h_targ;   // Angular momentum of target
VECTOR3 R_targ;   // position of target
VECTOR3 V_targ;	  // Velocity of target
VECTOR3 K_targ;   // unit vector K (Z axis) for target orbit.
double h_targ_abs; // magnitude of angular momentum
VECTOR3 I_targ;   // x axis
VECTOR3 J_targ;   // y axis
MATRIX3 TMatrix;	// translation matrix ecliptic to target orbit
VECTOR3 h;			// angular momentum of our ship.
double inc;			// inclination of our ship.
VECTOR3 RPos;			// state vectors of our ship.
VECTOR3 VVel;
VECTOR3 RPosF;      // element order rearranged
VECTOR3 VVelF;      
double LongOfAN;		// Longitude of the ascending node (ours)
VECTOR3 eVec;		// vector of eccentricity
double eVec_abs;
VECTOR3 RefYAxis;
double RefYAxis_abs;
double f;			// True anomaly
double ArgOfPeri;	// Argument of periapsis
double ArgOfAscension; // angle since ascension
double EccAnomaly;
double MnAnomaly;
double PeriodT;
double TimeToAscendingNode;
double TimeOfAdjAOP;   // time of adjusted argument of periapsis
double TimeOfTrueAnomaly;
double sma;				// semi-major axis.
double SpMechE;		// Specific mechanical energy
double ArgOfPeriPlusPI;	// descending node 
double TimeOfAdjAOPPlusPI; // time of descending node
double TimeToDescendingNode;
double PADeltaV;		// plane alignment deltaV
double SemiLatRectum;				//semi latus rectum
double RadiusAtANorDN;	// radius at ascending or descending node
double VelocityAtANorDN;
double TimeTillBurn;
double VelAtNode;		// velocity at AN node or DN node
double BurnTime;
bool LimitedBurnT;
double TimeOfBurn;  // time of plane alignment burn (SimT s )
bool EngineController3 = false;
bool Normal;		// Normal burn or Antinormal burn?
double RelIncLast = PI;  // The relatitive inclination of the previous step.

// Info mode
char *ITVesselName;
double LatitudeLast = 0;
double Latitude = 0;

// Rendezvous
// CCK The modes that are possible for the Rendezvous program
enum REND_MODE { 
	REND_MODE_NEED_TARGET,		// No target has been entered
	REND_MODE_HOHMANN_SETUP,	// Setting up the Hohmann transfer
	REND_MODE_WAIT_SETUP_ANGLE,	// Waiting until we reach the correct angle for the burn
	REND_MODE_CALC_CWBURN1,		// Calculate the CW burn number 1
	REND_MODE_CWBURN1,			// CW Burn 1
	REND_MODE_COAST_CCB1,		// Coasting toward the first CCB
	REND_MODE_CCB1_SETUP,		// Setup the first CCB
	REND_MODE_COAST_CCB2,		// Coasting towards the second CCB
	REND_MODE_CCB2_SETUP,		// Setup the second CCB
	REND_MODE_COAST_CCB3,		// Coasting towards the third CCB
	REND_MODE_CCB3_SETUP,		// Setup the second CCB
	REND_MODE_COAST_REND,		// Coasting towards the Rendezvous burn
	REND_MODE_REND_SETUP,		// Setup the Rendezvous burn
	REND_MODE_REND_BURN,		// Rendezvous burn
	REND_MODE_POST_REND,		// Post-rendezvous burn
	REND_MODE_STATIONKEEPING	// Post-Rendezvous stationkeeping
};
 
double RendDistance = 250;
double GlobalPitch;
double GlobalHeading;
bool AttitudeControllerOn = false;
double PitchErrLast = 0;
double HeadErrLast = 0;
double PitchErr, HeadErr, RollErr;
VESSEL *RTVessel;  // handle of Rendezvous target vessel
char *RTVesselName;	// Target vessel name
OBJHANDLE hRTargetVessel;
bool BurnComplete = false;
double RendvOrbit_tf; // transfer time
double B1LeadAngle; // Burn 1 lead angle for Hohmann transfer
double SynodicP;  // Synodic period (s)
MATRIX3 TransMat; // rotation matrix for current CW frame
MATRIX3 InvTransMat;  //inverse of above
double Burn1ITime;  // Burn 1 Impulse time (simt (s))
double CCB1_Time;  // Course Correction burn 1 time
double CCB2_Time;
double CCB3_Time;
double BurnDuration;
double Burn1IgnitTime;
double dV0_abs;
bool EngineController4 = false;
double TimeStop;  // EngineController 4 Stop time.
double CCB1_tf;   // Course correction burn 1 transfer time
double CCB2_tf;
double CCB3_tf;
bool AttitudeSet = false;
bool CCB1_AttTargetSet = false;
bool ProgradeOn;
double RO_vapo, TargetCircV, dVtf_classic;
double Rendezvous_I_Time;  // time of Rendezvous (tf) in simt.
double CCB1SetupInitTime; // time at which the CCB1 setup begins
bool CCB1SetupInitTimeDone;
double CCB2SetupInitTime; // time at which the CCB2 setup begins
bool CCB2SetupInitTimeDone;
double CCB3SetupInitTime;
bool CCB3SetupInitTimeDone;
VESSEL *SKTarget;
bool StationKeepingOn = false;

// Attitude controller variables
VECTOR3 DeltaVAttitude;
bool StarTrackOn = false;
bool LHLV_AttControlOn = false;
bool LHLV_AC_OFF = false;
VECTOR3 LTargetVector3, Pos3, TargetVector3;
double PitchLast = 0;
double TimeLast = 0;

// KillallThrusters delay boolean
bool HaveWaitedOneCycle = false;
bool KillAllThrustersSwitch = false;

class GuidanceMFD:public MFD {
public:
	OBJHANDLE hInfoVessel;
	GuidanceMFD(DWORD w, DWORD h, VESSEL *vessel);
	bool ConsumeKeyBuffered (DWORD key);
	bool ConsumeButton (int bt, int event);
	char *ButtonLabel (int bt);
	int  ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	void DisplayMFDName();
	void Calculation();
	void CalcOrbitDisplay();		// CCK
	void CalcPlaneDisplay();		// CCK
	void CalcRendezvousDisplay();	// CCK
	void UpdateMenuDisplay();
	void UpdateOrbitDisplay();
	void UpdateInfoDisplay();
	void UpdatePlaneDisplay();
	void UpdateRendezvousDisplay();
	bool SetTargetAP (char *rstr);
	bool SetTargetMenu (char *rstr);
	bool SetTargetTime (char *rstr);
	bool SetTargetInfo (char *rstr);
	bool SetTargetPlane (char *rstr);
	bool SetTargetRendezvous (char *rstr);
	
	// Math functions
	VECTOR3 CrossProduct(const VECTOR3 &a, const VECTOR3 &b);
	VECTOR3 DivideVector(const VECTOR3 &a, double b);
	VECTOR3 MultiplyVector(const VECTOR3 &a, double b);
	VECTOR3 Ecliptic2Equatorial(const VECTOR3 &Vec);
	void MultiplyByMatrix(const VECTOR3 &Initial, const MATRIX3 &RotMatrix, VECTOR3 &Result);
	MATRIX3 MakeTranslationMatrix( const VECTOR3 &I, const VECTOR3 &J,const VECTOR3 &K );
	double DotProduct (const VECTOR3 &a, const VECTOR3 &b);
	void GaussJordan();
	void invert();
	double det();
	VECTOR3 ReorderVector(const VECTOR3 &Vect);
	MATRIX3 XRotMatrix(double theta);
	MATRIX3 YRotMatrix(double theta2);
	MATRIX3 ZRotMatrix(double theta3);
	double GetAngle(const VECTOR3 &Vect);
	double GetAngle_arot(const VECTOR3 &Vect);
	MATRIX2 Matrix2Inverse(MATRIX2 &mat);
	MATRIX2 Matrix2Multiply(MATRIX2 &mata, MATRIX2 &matb);
	MATRIX3 Matrix3Multiply(MATRIX3 &a, MATRIX3 &b);
	VECTOR3 MatrixVec2Multiply(MATRIX2 &mat, VECTOR3 &vec);
	MATRIX3 Matrix3Inverse(const MATRIX3 &a);
	VECTOR3 Matrix3TimesVector3( const MATRIX3 &a, const VECTOR3 &b);
	
	// Orbital Mechanics
	double ArgOfPeriapsis(VECTOR3 &n, VECTOR3 &e);
	double TrueAnomaly(VECTOR3 &e, VECTOR3 &R, VECTOR3 &V);
	double NewVelocity_AorP(double &Rapo, double &Rperi, double &Rnew);
	VECTOR3 AngularMomentum(const VECTOR3 &R, const VECTOR3 &V);
	double LAN( const VECTOR3 &n, double i);
	double Inclination(const VECTOR3 &h);
	VECTOR3 EccentricityVector(double u, const VECTOR3 R2, const VECTOR3 V2);
	double EccentricAnomaly(const VECTOR3 &e, double f);
	double MeanAnomaly(double E, const VECTOR3 &e);
	double Period(double a, double u);
	double SemiMajorAxis(double E, double u);
	double TimeSincePeriapsis(double Me, double T);
	double SpecMechEnergy(const VECTOR3 &V, const VECTOR3 &R, double u);
	double SemiLatusRectum(double u, const VECTOR3 &h);
	double TrueAnomaly2Radius(double p, double e, double f);
	double VisViva(double u, double r, double a);
	VECTOR3 CWSolutionB1(double tf, double n, VECTOR3 &dRZero, VECTOR3 &dVMinusZero);
	MATRIX3 CWM(double n, double t);
	MATRIX3 CWN(double n, double t);
	MATRIX3 CWS(double n, double t);
	MATRIX3 CWT(double n, double t);

	// Controllers
	void EngController1(double simt, double Vnew, double BurnTInit);
	void EngController2(double simt, double BurnTInit);
	void AttitudeController(VECTOR3 TargetVector, double simt, double simdt);
	void StarTrackAttController(VECTOR3 TargetVector);
	void LHLV_AttControl(double simt, bool Off);
	void EngController4(double simt, double Duration);
	void inline StationKeeping(VESSEL *TargetVessel);
	void KillAllThrusters();

private:
	HDC	hDC;
	int Row;
	int line;
	OBJHANDLE ref;

	// CCK
	ELEMENTS Elements;
	VECTOR3 RelPosition;
	VECTOR3 RelVelocity;
	int ROWH;
		
	OBJHANDLE hSetGbody; //handle of the current set Gbody (e.g. planet)

	double CurrLeadAngle, SetupAngle;
	VECTOR3 dRZeroB1, dVZeroB1, dV0, dV0_eclp;

	// CCK
	ORBIT_MODE OrbitMode;	// The mode of the Orbit program - ORBIT_MODE_***
	REND_MODE RendMode;		// The mode of the Rendezvous program - REND_MODE_***	
	PLANE_MODE PlaneMode;	// The mode of the Plane-change program - PLANE_MODE_***

	double MyPitch, MyHeading;
	// CCK End
	
};

#endif
