//  ==============================================================================================================================================
//	Copyright (C) 2002 - 2015 Jarmo Nikkanen
//                2016        Andrew Stokes
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>
//
//  ==============================================================================================================================================

#define STRICT
#define ORBITER_MODULE

#include "OrbiterSDK.h"

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "BSTools.hpp"
#include "BaseSyncMFD.hpp"
#include "BSReference.hpp"
#include "EnjoLib\ModuleMessagingExt.hpp"


HBRUSH green_brush=NULL, dgreen_brush=NULL;
HBRUSH yellow_brush=NULL;
HBRUSH black_brush=NULL,grey_brush=NULL;
HPEN dash_pen=NULL,solid_pen=NULL,solid_pen_y=NULL,dash_pen_y=NULL,solid_pen_grey=NULL;
HPEN dash_pen_dgrey=NULL,solid_pen_dgrey=NULL,solid_pen_dgreen=NULL, solid_pen_white=NULL;
HBRUSH white_brush=NULL,red_brush=NULL,dgrey_brush=NULL,no_brush=NULL;



//	GLOBAL DATA AND FUNCTIONS
int mode,CW;
bool error_flag;
VESSEL  *ship=NULL;
char LineBuffer[4096];
class SyncMFD *gSync[8];


// ==============================================================================================
//
DLLCLBK void InitModule(HINSTANCE hDLL)
{
	int i;

	FILE *fil;
	char *line;
	char temptxt[256];

	int letter = OAPI_KEY_B;

	fil=fopen("Config/MFD/BaseSyncMFD.cfg","r");

	if (fil) {
		while ((line=fgets(LineBuffer,128,fil)) != NULL) {		

			if (!_strnicmp (line, "Color_01", 8)) sscanf (line+8, "%x",&green);
			else if (!_strnicmp (line, "Color_02", 8)) sscanf (line+8, "%x",&dgreen);
			else if (!_strnicmp (line, "Color_03", 8)) sscanf (line+8, "%x",&lgreen);
			else if (!_strnicmp (line, "Color_04", 8)) sscanf (line+8, "%x",&hgreen);
			else if (!_strnicmp (line, "Color_05", 8)) sscanf (line+8, "%x",&yellow);
			else if (!_strnicmp (line, "Color_06", 8)) sscanf (line+8, "%x",&lyellow);
			else if (!_strnicmp (line, "Color_07", 8)) sscanf (line+8, "%x",&blue);
			else if (!_strnicmp (line, "Color_08", 8)) sscanf (line+8, "%x",&dgrey);
			else if (!_strnicmp (line, "Color_09", 8)) sscanf (line+8, "%x",&red);
			else if (!_strnicmp (line, "Color_10", 8)) sscanf (line+8, "%x",&white);
			else if (!_strnicmp (line, "Color_11", 8)) sscanf (line+8, "%x",&grey);
			else if (!_strnicmp (line, "Color_12", 8)) sscanf (line+8, "%x",&itemcolor);
			else if (!_strnicmp (line, "EQU_DIR_MODE", 12)) {
				sscanf (line+13, "%256s",temptxt);
				if (!_stricmp(temptxt,"EQU")) defEquDir = 1;
				else if (!_stricmp(temptxt,"DIR")) defEquDir = 2;
			}
			else if (!_strnicmp (line, "DISPLAY_MODE", 12)) {
				sscanf (line+13, "%256s",temptxt);
				if (!_stricmp(temptxt,"TXT")) defDispMode = 1;
				else if (!_stricmp(temptxt,"GRA")) defDispMode = 2;
				else if (!_stricmp(temptxt,"BOTH")) defDispMode = 3;
			}
			else if (!_strnicmp (line, "ENC_MODE", 8)) {
				sscanf (line+9, "%256s",temptxt);
				if (!_stricmp(temptxt,"CLOSEST")) defEncMode = 2;
				else if (!_stricmp(temptxt,"APOAPSIS")) defEncMode = 3;
				else if (!_stricmp(temptxt,"PERIAPSIS")) defEncMode = 4;
				else if (!_stricmp(temptxt,"LATITUDE")) defEncMode = 1;
			}
			else if (!_strnicmp (line, "NUM_SOLUTIONS", 13)) {
				sscanf (line+14, "%d",&defNumSol);
				if (defNumSol <1 || defNumSol > 99) defNumSol = 8;
			}
			else if (!_strnicmp (line, "RE_ANGLE", 8)) {
				sscanf (line+9, "%f",&defReAng);
				if (defReAng <=0.0 || defReAng > 90.0) defReAng = 0.7;
			}
			else if (!_strnicmp (line, "RE_ANTICIPATION", 15)) {
				sscanf (line+16, "%f",&defReAnt);
				if (defReAnt <0.0 || defReAnt > 360.0) defReAng = 75.0;
			}
			else if (!_strnicmp (line, "RE_ALTITUDE", 11)) {
				sscanf (line+12, "%f",&defReAlt);
				if (defReAlt <0.0) defReAlt = 120.0;
			}
		}			
		fclose(fil); 
	}

	static char *name = "BaseSync";
	MFDMODESPEC spec;
	spec.name    = name;
	spec.key     = letter;
	spec.msgproc = MsgProc;

	mode = oapiRegisterMFDMode (spec);	

	green_brush=CreateSolidBrush(green);
	dgreen_brush=CreateSolidBrush(dgreen);

	dash_pen=CreatePen(PS_DOT,1,green);     
	solid_pen=CreatePen(PS_SOLID,1,green);  
	black_brush=CreateSolidBrush(0x00000000);      
	grey_brush=CreateSolidBrush(grey);   

	dash_pen_y=CreatePen(PS_DOT,1,yellow);     
	solid_pen_y=CreatePen(PS_SOLID,1,yellow);  
	yellow_brush=CreateSolidBrush(yellow);

	solid_pen_grey=CreatePen(PS_SOLID,1,grey);  
	solid_pen_dgreen=CreatePen(PS_SOLID,1,dgreen);  
	solid_pen_dgrey=CreatePen(PS_SOLID,1,dgrey);  
	dash_pen_dgrey=CreatePen(PS_DOT,1,dgrey);  

	white_brush=CreateSolidBrush(0x00ffffff);  
	red_brush=CreateSolidBrush(red); 
	dgrey_brush=CreateSolidBrush(dgrey);

	solid_pen_white=CreatePen(PS_SOLID,1,white);  

	LOGBRUSH br;
	br.lbStyle=BS_HOLLOW;
	no_brush=CreateBrushIndirect(&br);

	// Setup Classes
	
	ship  = NULL;
	Refer = NULL;	

	for (i=0;i<8;i++) gSync[i]=NULL;
}


// ==============================================================================================
//
DLLCLBK void ExitModule(HINSTANCE hDLL)
{

	Refer=NULL;

	oapiUnregisterMFDMode (mode);

	DeleteObject(green_brush);   
	DeleteObject(dgreen_brush);     
	DeleteObject(black_brush);  
	DeleteObject(grey_brush);  
	DeleteObject(dash_pen); 
	DeleteObject(solid_pen); 
	DeleteObject(dash_pen_y); 
	DeleteObject(solid_pen_y);
	DeleteObject(yellow_brush);
	DeleteObject(solid_pen_grey);
	DeleteObject(solid_pen_dgrey);	
	DeleteObject(dash_pen_dgrey);
	DeleteObject(white_brush);
	DeleteObject(red_brush);
	DeleteObject(dgrey_brush);
	DeleteObject(no_brush);
	DeleteObject(solid_pen_dgreen);
	DeleteObject(solid_pen_white);
       
	int i;
	for (i=0;i<8;i++) gSync[i]=NULL;
}


// ==============================================================================================
//
DLLCLBK void opcFocusChanged(OBJHANDLE n, OBJHANDLE o)
{
	if (n) ship=oapiGetVesselInterface(n);
}
                      
// ==============================================================================================
//
DLLCLBK void opcCloseRenderViewport(void)
{  
	if (Refer) delete Refer;

	Refer=NULL;
	error_flag=false;

	int i;
	for (i=0;i<8;i++) gSync[i]=NULL;
}

// ==============================================================================================
//
GeoSyncMFD::GeoSyncMFD (DWORD w, DWORD h, VESSEL *vessel, int mfd)
: MFD (w, h, vessel)
{
  MFD_ID=mfd;

	width=w;
	height=h;
	ship=vessel;
	error_flag=false;

	if (h>280 && h<300) {
		LineHeight=14;
		ButtonSpacing=41;
		FirstButton=40;
		CW=w/36;
	} else {
		LineHeight=(h/21); 
		ButtonSpacing=(h/7); 
		FirstButton=(h/7); 
		CW=w/33;
	}
	
	if (Refer==NULL) Refer = new ReferenceClass();
	
	if (MFD_ID==2) {
		Sync = new SyncMFD(this);
	} else {
		// Do not reconstruct classes if they are already constructed, just get the pointers	
		if (gSync[mfd]) Sync = gSync[mfd], Sync->MFD=this;
		else gSync[mfd] = Sync = new SyncMFD(this);
	}
}

// ==============================================================================================
//
GeoSyncMFD::~GeoSyncMFD ()
{
	if (MFD_ID==2) delete Sync;
}


// ==============================================================================================
//
int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	class GeoSyncMFD *MFD;

  if (msg==OAPI_MSG_MFD_OPENED) {	
	if (mfd>2 || mfd<0) mfd=2;
	MFD = new GeoSyncMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, mfd); 
	return (int)MFD;
  }
  return 0;
}

// ==============================================================================================
//
bool GeoSyncMFD::ConsumeKeyBuffered (DWORD key)
{
  bool DataInput (void *id, char *str, void *data);

	static const DWORD keys[12] = { OAPI_KEY_T, OAPI_KEY_L, OAPI_KEY_A, OAPI_KEY_D,
		OAPI_KEY_M, OAPI_KEY_E, OAPI_KEY_N, OAPI_KEY_R,OAPI_KEY_Z,OAPI_KEY_X,OAPI_KEY_C,OAPI_KEY_V };

	int i;
	for (i=0;i<12;i++) if (keys[i]==key) { InvalidateDisplay(); break; }
			
	switch (key) {	  

		case OAPI_KEY_Z:
      if (Sync->usingGS2) return true; // ANG, ANT, ALT disables when slaved to GS2
			ID=7;
			oapiOpenInputBox ("ReEntry angle (0.0°-90.0°) (typ. 0.5°-4.0°)", DataInput, 0, 20, (void*)this);
			return true;  

		case OAPI_KEY_X:			
      if (Sync->usingGS2) return true; // ANG, ANT, ALT disables when slaved to GS2
			ID=8;
			oapiOpenInputBox ("ReEntry anticipation (0.0°-360.0°) (typ. 45°-180°)", DataInput, 0, 20, (void*)this);
			return true;  

		case OAPI_KEY_A:			
      if (Sync->usingGS2) return true; // ANG, ANT, ALT disables when slaved to GS2
			ID=9;
			oapiOpenInputBox ("ReEntry altitude (km) (typ. 80-120)", DataInput, 0, 20, (void*)this);
			return true;  
				
		case OAPI_KEY_T:			
			ID=4;
			oapiOpenInputBox ("Enter Target Base, or GS2 to slave to Glideslope 2", DataInput, 0, 30, (void*)this);
			return true;  
        
		case OAPI_KEY_L: 
			ID=2;
			oapiOpenInputBox ("Enter Tgt Lat/Lon (e.g. N28.52 W80.67 or +28.52 -80.67)", DataInput, 0, 30, (void*)this);
			return true;

		case OAPI_KEY_C: 
			ID=3;
			Sync->sync_dispmin += 8;
			if (Sync->sync_dispmin > Sync->sync_num) Sync->sync_dispmin = 1;
			return true; 

		case OAPI_KEY_N: 
			ID=5;
			oapiOpenInputBox ("Number of orbits to display (1-99)", DataInput, 0, 20, (void*)this);
			return true; 

		case OAPI_KEY_R: 
			ID=6;
			oapiOpenInputBox ("Reference planet or moon", DataInput, 0, 20, (void*)this);
			return true; 

		case OAPI_KEY_M: 
			Sync->ChangeMode();
			return true; 
		
		case OAPI_KEY_V: 
			Sync->Deorbit();
			return true; 

		case OAPI_KEY_D: 
			Sync->Direct();
			return true; 

		case OAPI_KEY_E: 
			Sync->EncMode();
			return true; 

		case OAPI_KEY_1: 
			Sync->LineAdjust(1);
			return true;
				
		case OAPI_KEY_2: 
			Sync->LineAdjust(-1);
			return true; 
    }

    return false;	
}

// ==============================================================================================
//
bool GeoSyncMFD::ConsumeButton (int bt, int event)
{  
	static const DWORD btkey[12] = { OAPI_KEY_T, OAPI_KEY_L, OAPI_KEY_C, OAPI_KEY_D,
		OAPI_KEY_M, OAPI_KEY_E, OAPI_KEY_N, OAPI_KEY_R,OAPI_KEY_Z,OAPI_KEY_X,OAPI_KEY_A,OAPI_KEY_V };

	if ((event&PANEL_MOUSE_LBDOWN)==0) return false;

	if (bt < 12) return ConsumeKeyBuffered (btkey[bt]);
	return false;
}																						

// ==============================================================================================
//
char *GeoSyncMFD::ButtonLabel (int bt)
{
	char *label[16] = {"TGT", "LL ", "CYC","E/D","MOD","ENC","NUM","REF","ANG","ANT","ALT","DEO"};	
	return (bt < 12 ? label[bt] : 0);
}

// ==============================================================================================
//
int GeoSyncMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
  static const MFDBUTTONMENU mnu[12] = {
        {"Target base", 0, 'T'},
        {"Manual Lat/Long", 0, 'L'},
        {"Cycle pages", 0, 'C'},
		{"Equator / Direct", 0, 'D'},	
		{"Display mode", 0, 'M'},
		{"Encouter mode", 0, 'E'},
		{"Number of solutions", 0, 'N'},
		{"Reference planet/moon", 0, 'R'},
		{"ReEntry angle", 0, 'Z'},
		{"ReEntry anticipation", 0, 'X'},
		{"ReEntry altitude", 0, 'A'},
		{"DeOrbit program", 0, 'V'}				
  };
  if (menu) *menu = mnu;
  return 12;
}

// ==============================================================================================
//
void GeoSyncMFD::Update (HDC hDC)
{  
  Sync->Update(hDC);

	// Try to reset device..
	SelectObject(hDC,GetStockObject(NULL_BRUSH));
	SelectObject(hDC,GetStockObject(WHITE_PEN));
	SelectObject(hDC,GetStockObject(SYSTEM_FONT));

}

// ==============================================================================================
//
void GeoSyncMFD::WriteStatus (FILEHANDLE scn) const
{  
  Sync->Write(scn);
}

// ==============================================================================================
//
void GeoSyncMFD::ReadStatus (FILEHANDLE scn)
{
  Sync->Read(scn);	
}

// ==============================================================================================
//
void GeoSyncMFD::StoreStatus (void) const
{ 
}

// ==============================================================================================
//
void GeoSyncMFD::RecallStatus (void)
{	
}

// ==============================================================================================
//
bool DataInput (void *id, char *str, void *data)
{
	class GeoSyncMFD *MFD=(class GeoSyncMFD *)data;
	class SyncMFD    *Sync = MFD->Sync;
	double val = 0.0; 
	double lat = 0.0; 
	double lon = 0.0; 
	int ival;
	char *ptr = str;

	if (ID==6) {                                                                // REF Planet
    Sync->usingGS2 = false;
    Sync->trgt = &Sync->bstrgt;
    Sync->MMPut_done = false;
    strncpy(Sync->trgt->ref,str,31);
	} else if ((ID==7) && text2value(str, ptr, val, true)) {                    // ANG
		if (val > 0.0 && val <= 90.0)	Sync->bstrgt.ang = val * RAD;
	} else if ((ID==8) && text2value(str, ptr, val, true)) {                    // ANT
		if (val > 0.0 && val <= 360.0)	Sync->bstrgt.ant = val * RAD;
	} else if ((ID==9) && text2value(str, ptr, val, true)) {                    // ALT
		if (val >= 0.0)	Sync->bstrgt.alt = val;
	} else if ((ID==5) && text2value(str, ptr, val, false)) {                   // NUM Solutions
		ival = (int)val;
		if (ival > 0 && ival < 100) {
			Sync->sync_num = ival;
			Sync->sync_dispmin = 1;
		}
	} else if ((ID==2) && text2doublevalue(str, ptr, lat, lon, true)) {         // TARGET LAT LON
		if ((lat>-360.0) && (lat<=360.0) && (lon>=-90.0) && (lat<=90.0)) {
			if (lat<=-180.0) lat += 360.0;
			if (lat>180.0) lat -= 360.0;
      Sync->usingGS2 = false;
      Sync->trgt = &Sync->bstrgt;
      Sync->MMPut_done = false;
			Sync->trgt->lat = lat * RAD;
			Sync->trgt->lon = lon * RAD;
			strcpy(Sync->trgt->name,"None");
		}
	} else if (ID==4) {                                                         // TARGET NAME
    if (!_stricmp(str,"GS2")) {
      if (EnjoLib::ModuleMessagingExt().ModMsgGetByRef("GS2","GlideslopeTarget",2,&Sync->gs2trgt)) {
        Sync->usingGS2 = true;


        Sync->trgt = (BaseSyncExportTgtStruct *) Sync->gs2trgt;
        Sync->MMPut_done = false;
        return true;
      } else {
        return false;
      }
    }
    Sync->usingGS2 = false;
    Sync->trgt = &Sync->bstrgt;
    Sync->MMPut_done = false;		OBJHANDLE ref=oapiGetGbodyByName(Sync->trgt->ref);
		if (ref) {
			OBJHANDLE bas=oapiGetBaseByName(ref,str);
			if (bas) {
        strncpy(Sync->trgt->name,str,31);
				oapiGetBaseEquPos(bas,&Sync->trgt->lon, &Sync->trgt->lat);
			} else {
				strcpy(Sync->trgt->name,"None");
			}
		}

  } else {
    return false;
  }
	return true;
}