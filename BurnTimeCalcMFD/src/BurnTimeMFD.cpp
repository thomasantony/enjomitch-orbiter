// ==============================================================
// $Id: BurnTimeMFD.cpp,v 1.5 2004/05/25 19:08:46 dh219 Exp $
// BTC by D Henderson orbiter@aibs.org.uk
// Copyright 2004
// Extensively modified by C Jeppesen chrisj@kwansystems.org
// Copyright 2005-2006
// Extended with the ability to use different Engines (Hover,Main,Retro) by Topper
// Copyright 2007
// Version 1.5 by kwan3217 (2008):
//		Some code cleanup,
//		Bug case where engine group existed but was not connected to a propellant resource - FIXED
//		Works in SI or feet (for use with Space Shuttle)
// Version 2.0 ny Topper
//		Var-setting now working via inputbox
//		Entering of Targets (Vessels or Gbodys) instead of dv possible
//		New Autoburnmode "Burn by target distance" added


// Released under the terms of the LGPL: http://www.gnu.org/licenses/lgpl.txt
/*
    This software and sourcecode is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

// ==============================================================

// ==============================================================
// $Id: BurnTimeMFD.cpp,v 1.5 2004/05/25 19:08:46 dh219 Exp $
// BTC by D Henderson orbiter@aibs.org.uk
// Copyright 2004
// Extensively modified by C Jeppesen chrisj@kwansystems.org
// Copyright 2005-2006
// Extended with the ability to use different Engines (Hover,Main,Retro) by Topper
// Copyright 2007
// Version 1.5 by kwan3217 (2008):
//		Some code cleanup,
//		Bug case where engine group existed but was not connected to a propellant resource - FIXED
//		Works in SI or feet (for use with Space Shuttle)
// Version 2.0 by Topper
//		Var-setting now working via inputbox
//		Entering of Targets (Vessels or Gbodys) instead of dv possible
//		New Autoburnmode "Burn by target distance" added


// Released under the terms of the LGPL: http://www.gnu.org/licenses/lgpl.txt
/*
    This software and sourcecode is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

// ==============================================================

#define STRICT
#define ORBITER_MODULE

#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define YELLOW RGB(255, 255, 0)
#define DARK_YELLOW RGB(128, 128, 0)
#define WHITE RGB(255, 255, 255)
#define BLUE RGB(50, 150, 255)
#define GRAY RGB(160, 160, 160)
#define BRIGHTERGRAY RGB(200, 200, 200)
#define ORANGE RGB(255, 130, 0)

#define INPUTMODE_NONE 0
#define INPUTMODE_DV 1
#define INPUTMODE_TARGET 2
#define INPUTMODE_OFFSET 3
#define INPUTMODE_TIME 4
#define INPUTMODE_EXTRA 5

#define BURNMODE_PERI 0
#define BURNMODE_APO  1
#define BURNMODE_MAN  2
#define BURNMODE_TGT  3

int inputmode = 0;

#include "BurnTimeMFD.h"
#include "MFDButtonPageBTC.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <ModuleMessaging.hpp>
#include <Util/Result.hpp>

  int dspunit; //0 - SI
            //1 - US

#define mToft 1.0/0.3048
#define gTolb 1.0/0.45359237
#define mpersToftpers 1.0/9.80665
#define NTolbf 0.224808943
#define mperssqToftperssq 1.0/0.3048

double dv, mextra;
int IndexCenterObj;
MFDButtonPageBTC gButtons;

bool IsEngaged,IsArmed,IsCircular;
double ECutoff,IManual,EReference;
double TDist=0;
double sOffset=500;


const THGROUP_TYPE groups[3]={THGROUP_MAIN,THGROUP_HOVER,THGROUP_RETRO};
const char group_names[3][6]={"Main","Hover","Retro"};

int DisplayEngUnit(char* buffer, char* pattern,double x) {
  char Big[]=  " kMGTPEZY";
  char Small[]=" munpfazy";
  int ptr=0;
  if(fabs(x)>1e-24) {
    if(fabs(x)>1) {
      while(fabs(x)>1000) {
        ptr++;
        x=x/1000.0;
      }
      return sprintf(buffer,pattern,x,Big[ptr]);
    } else {
      while(fabs(x)<1) {
        ptr++;
        x=x*1000.0;
      }
      return sprintf(buffer,pattern,x,Small[ptr]);
    }
  } else {
    return sprintf(buffer,pattern,x,Small[0]);
  }
}

void BurnTimeMFD::PrintEngUnit(HDC hDC, char* format, char* unitSI, char* unitUS, double multSI, double multUS, double value, int x, int l) {
  char buf[256];
  char label[256];
  sprintf(label,"%s%%c%s",format,dspunit==0?unitSI:unitUS);
  int len=DisplayEngUnit(buf,label,value*(dspunit==0?multSI:multUS));
  TextOut(hDC,x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(HDC hDC, char* format, char* units, double mult, double value, int x, int l) {
  char buf[256];
  char label[256];
  sprintf(label,"%s%%c%s",format,units);
  int len=DisplayEngUnit(buf,label,value*mult);
  TextOut(hDC,x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(HDC hDC, char* format, char* units, double value, int x, int l) {
  char buf[256];
  char label[256];
  sprintf(label,"%s%%c%s",format,units);
  int len=DisplayEngUnit(buf,label,value);
  TextOut(hDC,x,line(l),buf,len);
}

void BurnTimeMFD::PrintString(HDC hDC, char* format, const char* value, int x, int l) {
  char buf[256];
  sprintf(buf,format,value);
  TextOut(hDC,x,line(l),buf,strlen(buf));
}

void BurnTimeMFD::PrintString(HDC hDC, const char* format, int x, int l) {
  char buf[256];
  TextOut(hDC,x,line(l),format,strlen(buf));
}

static struct {
  int mode;
} g_BurnTimeMFD;

BurnTimeMFD *burntime;

DLLCLBK void opcDLLInit (HINSTANCE hDLL) {
  static char *name = "BurnTimeMFD";
  MFDMODESPEC spec;
  spec.name    = name;
  spec.key     = OAPI_KEY_T;
  spec.msgproc = BurnTimeMFD::MsgProc;

  g_BurnTimeMFD.mode = oapiRegisterMFDMode (spec);

  burntime = NULL;
}


DLLCLBK void opcDLLExit (HINSTANCE hDLL) {
  oapiUnregisterMFDMode (g_BurnTimeMFD.mode);
}

DLLCLBK void opcPreStep (double ENow, double simdt, double mjd) {
  if(burntime!=NULL) {
    burntime->TimeStep(ENow);
  }
}

BurnTimeMFD::BurnTimeMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w, h, vessel) {
  height = (int)h;
  static bool first=true;
  if(first)
  {
	 mul=1.0;
	 dv = 0.0;
	 mextra = 0.0;
	 ECutoff=0;
	 IsEngaged=false;
	 IsArmed=false;
	 mode=BURNMODE_PERI;
	 IManual=0;
	 IsCircular=false;
	 first = false;
	 Sel_eng = 0;
	 dspunit=0;
  }
  if (mul==0) mul=1.0;
}

BurnTimeMFD::~BurnTimeMFD () {
}

void getGroupThrustParm(VESSEL* vessel, THGROUP_TYPE group, double *F, double *isp) {
  *F = 0;
  *isp = 0;
  int nthr=vessel->GetGroupThrusterCount(group);

  for(int i=0; i<nthr; i++) {
    THRUSTER_HANDLE th=vessel->GetGroupThruster(group,i);
    *F += vessel->GetThrusterMax0(th);
	PROPELLANT_HANDLE ph=vessel->GetThrusterResource(th);
	double eff=1.0; //Some vessels play games with the propellant handles...
	if(ph!=NULL) {
	  //So only measure efficiency if it's attached to a prop tank, else assume 1.0
      eff=vessel->GetPropellantEfficiency(ph);
	}
    *isp += vessel->GetThrusterIsp(th) * vessel->GetThrusterMax0(th)*eff;
  }
  if (*F != 0)	*isp /= *F;
  else *isp=0;

}

double RocketEqnT(double dv, double m, double F, double isp) {

  return ( dv * m / (2.0 * F ) ) * ( 1 + exp( -1.0 * dv / isp ) );
}

void BurnTimeMFD::Update (HDC hDC)
{




  unsigned int thrustercount = 0;

  Title (hDC, "BurnTimeMFD");


  int line1 = 1;
  int line8 = 2;
  int line2 = 3;
  int line9 = 4;
  int line3 = 5;
  int line4 = 6;
  int line5 = 7;
  int line6 = 8;
  int line7 = 9;


  int line10=10;
  int line11=11;
  int line12=12;
  int line13=13;
  int line14=14;
  int line15=15;
  int line16=16;
  int line17=17;
  int line18=18;
  int line19=19;



  SetTextColor(hDC,GRAY);

  if (mode == BURNMODE_PERI) PrintEngUnit(hDC,"Time to Periapse:      %7.3f","s", IPeri, 5, line8 );
  if(IApo>0 && mode == BURNMODE_APO)
  {
    SetTextColor(hDC,GRAY);
    PrintEngUnit(hDC,"Time to Apoapse:       %7.3f","s", IApo, 5, line8 );
  }

  if (mode == BURNMODE_MAN)
  {
    SetTextColor(hDC,((inputmode==INPUTMODE_TIME )?YELLOW:GRAY));
	PrintEngUnit(hDC,"Time to Manual start:  %7.3f","s", IManual, 5, line8 );
  }

  if (mode == BURNMODE_TGT)
  {

    SetTextColor(hDC,GRAY);
	PrintEngUnit(hDC,"Distance to target:    %7.3f","m", "ft",1,mToft,TDist, 5, line8 );
	SetTextColor(hDC,((inputmode==INPUTMODE_OFFSET )?YELLOW:GRAY));
	PrintEngUnit(hDC,"Offset distance:       %7.3f","m", "ft",1,mToft,sOffset, 5, line9 );
  }

  SetTextColor( hDC, BRIGHTERGRAY );
  PrintString(hDC,"Engine:                   %s",&group_names[Sel_eng][0],5,line10);

  if(maxdv==0)
  {
    PrintString(hDC,"Enginetype not available%s","",5,line2);
  }
  else
  {
	if(IsEngaged)
	{
	  SetTextColor( hDC, RED );
	  TextOut(hDC,5,line(line3),"Autoburn Engaged",16);
	  PrintEngUnit(hDC,"Time to cutoff:        %7.3f","s", ECutoff-ENow,5,line4);
	}
	else if(IsArmed)
	{
	  SetTextColor( hDC, YELLOW );

	  if(IsCircular)
	  {
	     PrintEngUnit(hDC,"Autocirc Armed:        %7.3f","s", IBurn,5,line3);
	  }
	  else
	  {
	    PrintEngUnit(hDC,"Autoburn Armed:        %7.3f","s", IBurn,5,line3);
	  }
	  PrintEngUnit(hDC,"Time to ignition:      %7.3f","s", EReference-ENow-IBurn2,5,line4);
	}
	else
	{
	  SetTextColor( hDC, GREEN );
	  PrintEngUnit(hDC,"Estimated burn time:   %7.3f","s", IBurn,5,line3);
	  PrintEngUnit(hDC,"Half DeltaV time       %7.3f","s", IBurn2,5,line4);
	  PrintEngUnit(hDC,"Rel. DeltaS distance   %7.3f","m", "ft",1,mToft,dv*IBurn2, 5, line5 );
	}

	SetTextColor(hDC, (inputmode==INPUTMODE_DV)?YELLOW:GRAY );
	PrintEngUnit(hDC,"Target DeltaV:         %7.3f","m/s","ft/s",1,mToft, dv, 5, line1 );

	SetTextColor(hDC, (inputmode==INPUTMODE_EXTRA)?YELLOW:BLUE );
	PrintEngUnit(hDC,"Extra fuel mass:       %7.3f","g","lbm", 1000,gTolb,mextra,5,line19);
	if (mextra !=0){
		SetTextColor(hDC, ORANGE );
	    PrintEngUnit(hDC,"Extra fuel mass:       %7.3f","g","lbm", 1000,gTolb,mextra,5,line19);
	}



	std::string TargetString;

	char buffer[255];

	if (mode == BURNMODE_TGT)
	{
	  SetTextColor(hDC,inputmode==INPUTMODE_TARGET?YELLOW:GRAY);
	  oapiGetObjectName(oapiGetObjectByIndex(IndexCenterObj),buffer,255);
	  TargetString = "=> Selected Object: " + std::string(buffer);
	  TextOut(hDC,5,line(line2),TargetString.c_str(),TargetString.length());
	}

	SetTextColor( hDC, ORANGE );
	PrintEngUnit(hDC,"Estimate Total dV:     %7.3f","m/s","ft/s",1,mToft, maxdv,5,line7);
	PrintEngUnit(hDC,"Total burn time:       %7.3f","s", TTot,5,line6);
	SetTextColor( hDC, BLUE );
	PrintEngUnit(hDC,"Current vehicle mass:  %7.3f","g","lbm", 1000,gTolb,mv,5,line11);
	PrintEngUnit(hDC,"Current stack mass:    %7.3f","g","lbm", 1000,gTolb, ms,5,line12);
	PrintEngUnit(hDC,"Postburn vehicle mass: %7.3f","g","lbm", 1000,gTolb, mfinal,5,line13);
	PrintEngUnit(hDC,"Empty vehicle mass:    %7.3f","g","lbm", 1000,gTolb, me,5,line14);
	PrintEngUnit(hDC,"mass flow rate:        %7.3f","g/s","lbm/s", 1000,gTolb, mdot,5,line15);
	PrintEngUnit(hDC,"Eng Thrust:           %7.3f","N","lbf",1,NTolbf, F,5,line16);
	PrintEngUnit(hDC,"Eng Isp:              %7.3f","m/s","ft/s",1,mpersToftpers, isp,5,line17);
	PrintEngUnit(hDC,"Eng Acc:              %7.3f","m/s²","ft/s²",1,mperssqToftperssq,F/ms,5,line18);
  }
}

int BurnTimeMFD::MsgProc (UINT msg, UINT mfinald, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
    case OAPI_MSG_MFD_OPENED:
       burntime = (new BurnTimeMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
       return (int)burntime;
  }

  return 0;
}


bool ObjectInput (void *id, char *str, void *usrdata)
{
	if ( str[0] == 0) return true;
	std::string SearchString(str);
	OBJHANDLE hFObj = oapiGetObjectByName(str);
	//Auslesen des letzten Zeichens und Bestimmen des Expondentens
	std::string InputUnits =  "yzafpnum_kMGTPEZY;";
	std::string RightChar = std::string(SearchString.substr(SearchString.length()-1,SearchString.length()));
	std::string numbers = "0123456789;";
	//Überprüfe ob der String eine Zahl ist...


	if (inputmode != INPUTMODE_TARGET )
	{
		for (unsigned int laufNumber=0; laufNumber<SearchString.length()-1;laufNumber++)
		{
			if
			(
				SearchString.substr(laufNumber,1) != "." &&
				SearchString.substr(laufNumber,1) != "0" &&
				SearchString.substr(laufNumber,1) != "1" &&
				SearchString.substr(laufNumber,1) != "2" &&
				SearchString.substr(laufNumber,1) != "3" &&
				SearchString.substr(laufNumber,1) != "4" &&
				SearchString.substr(laufNumber,1) != "5" &&
				SearchString.substr(laufNumber,1) != "6" &&
				SearchString.substr(laufNumber,1) != "7" &&
				SearchString.substr(laufNumber,1) != "8" &&
				SearchString.substr(laufNumber,1) != "9"

			) return false;
		}
		double MyExp=-24;
		int lauf = 0 ;
		while (InputUnits.substr(lauf,1)!=";")
		{
			if (InputUnits.substr(lauf,1) == RightChar.substr(0,1)) break;
			lauf++;
			MyExp +=3;
		}
		// Wenn kein Einheitenfaktor gefunden wurde...
		if (InputUnits.substr(lauf,1)==";")
		{
			if (RightChar != "0" && atoi(RightChar.c_str()) == 0) return false;
			if (inputmode==INPUTMODE_OFFSET )
			{
				sOffset = atof(SearchString.c_str());
				if (dspunit == 1) sOffset = sOffset  * 0.3048;
			}
			else if (inputmode==INPUTMODE_DV )
			{
				dv = atof(SearchString.c_str());
				if (dspunit == 1) dv = dv * 0.3048;
			}
			if (inputmode==INPUTMODE_EXTRA )
			{
				mextra = atof(SearchString.c_str());
				if (dspunit == 1) mextra = mextra * 0.45359237;
			}
			else IManual = atof(SearchString.c_str());
		}
		else
		{
			if (inputmode==INPUTMODE_EXTRA )
			{
				mextra = atof(SearchString.c_str());
				if (dspunit == 1) mextra = mextra * 0.45359237;
			}
			if (inputmode==INPUTMODE_OFFSET )
			{
				sOffset = atof(SearchString.c_str()) * pow(10,MyExp);
				if (dspunit == 1) sOffset = sOffset * 0.3048;
			}
			else if (inputmode==INPUTMODE_DV )
			{
				dv = atof(SearchString.c_str()) * pow(10,MyExp);
				if (dspunit == 1) dv = dv * 0.3048;
			}
			else IManual = atof(SearchString.c_str()) * pow(10,MyExp);
			//sprintf(oapiDebugString(),"v1:%f  v2:%f ",atof(SearchString.c_str()),pow(10,MyExp));
		}
		inputmode=INPUTMODE_NONE;
		return true;
	}
	else
	{
		// Checking if string is equal
		std::transform(SearchString.begin(), SearchString.end(), SearchString.begin(), std::tolower);
		for (unsigned int i=0;i<oapiGetObjectCount();i++)
		{
			OBJHANDLE hSObj = oapiGetObjectByIndex(i);
			char name[255];
			oapiGetObjectName(hSObj,name,255);
			std::string strname(name);
			std::transform(strname.begin(), strname.end(), strname.begin(), std::tolower);
			if (strname == SearchString)
			{
			   IndexCenterObj = i;
			   inputmode=INPUTMODE_NONE;
			   return true;
			}
		}
		//checking if string is a part of the entered value
		for (unsigned int i = 0; i < oapiGetObjectCount();i++)
		{
			char ObjectNameCharStr[255];
			OBJHANDLE hobj = oapiGetObjectByIndex(i);
			oapiGetObjectName (hobj,ObjectNameCharStr,255);
			std::string StrObjectName(ObjectNameCharStr);
			std::transform(StrObjectName.begin(), StrObjectName.end(), StrObjectName.begin(), std::tolower);
			size_t pos;

			pos = StrObjectName.find(SearchString);
			if (pos != std::string::npos)
			{
			   IndexCenterObj = i;
			   inputmode=INPUTMODE_NONE;
			   return true;
			}
		}
		return false;
	}
}

bool BurnTimeMFD::ConsumeKeyBuffered (DWORD key)
{
    return gButtons.ConsumeKeyBuffered(this, key);
}

void BurnTimeMFD::HandlerSwitchButtonPage()
{
    gButtons.SwitchPage(this);
}

void BurnTimeMFD::HandlerTargetOrDV()
{
    inputmode=INPUTMODE_DV;
    bool ObjectInput (void *id, char *str, void *usrdata);
    oapiOpenInputBox("Enter dV + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTimeOfManoeuvre()
{
   inputmode=INPUTMODE_TIME;
   mode = BURNMODE_MAN;
   bool ObjectInput (void *id, char *str, void *usrdata);
   oapiOpenInputBox("Enter dT + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerOffsetDistance()
{
    inputmode=INPUTMODE_OFFSET;
    mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter offset distance + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTargetForDistanceCalc()
{
    inputmode=INPUTMODE_TARGET;
    mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter unique part of the targets name.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerReset()
{
	  mul=1.0;
	  dv = 0.0;
	  mextra = 0.0;
	  ECutoff=0;
	  IsEngaged=false;
	  IsArmed=false;
	  mode=BURNMODE_PERI;
	  IManual=0;
	  IsCircular=false;
}

void BurnTimeMFD::HandlerChangeMode()
{
	  if(IsArmed && IsEngaged) return;
	  mode++;
	  if(mode>=BURNMODE_TGT) mode=0;
}

void BurnTimeMFD::HandlerAutoBurn()
{
      if(IsArmed)
      {
        IsArmed=false;
        return;
      }
      if(IsEngaged)
      {
        IsEngaged=false;
      }
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],0);
      ArmAutoBurn();
}

void BurnTimeMFD::HandlerBurnNow()
{
      IsArmed=false;
      IsEngaged=true;
      ECutoff=oapiGetSimTime()+IBurn;
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],1.0);
}

void BurnTimeMFD::HandlerAimAutoCirc()
{
      if (mode > BURNMODE_APO) mode = BURNMODE_PERI;
      if(IsArmed)
      {
        IsArmed=false;
        IsCircular=false;
        return;
      }
      if(IsEngaged)
      {
        IsEngaged=false;
        oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],0);
      }
      IsCircular=true;
      CalcCircular();
      ArmAutoBurn();
}

void BurnTimeMFD::HandlerSwitchSI_US()
{
    dspunit=1-dspunit;
}

void BurnTimeMFD::HandlerSelectEngine()
{
      Sel_eng++;
      if (Sel_eng==3) Sel_eng=0;
}

void BurnTimeMFD::HandlerEnterExtraFuel()
{
	  inputmode=INPUTMODE_EXTRA;
	  bool ObjectInput (void *id, char *str, void *usrdata);
	  oapiOpenInputBox("Enter extra fuel mass in kilograms",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerGetFromTransX()
{
    using namespace EnjoLib;
    Result<double> dvRes = ModuleMessaging().GetDouble("TransX", "dv");
    Result<double> IManualRes = ModuleMessaging().GetDouble("TransX", "TBurn");
    if (dvRes.status && IManualRes.status)
    {
        dv = dvRes.value;
        IManual = IManualRes.value;

        mode = BURNMODE_MAN;
        IsArmed=IsEngaged=false;
        HandlerAutoBurn();
    }
}

void BurnTimeMFD::ArmAutoBurn()
{
  switch(mode)
  {
    case BURNMODE_PERI:
      IReference=IPeri;
      break;
    case BURNMODE_APO:
      IReference=IApo;
      break;
    case BURNMODE_MAN:
      IReference=IManual;
      break;
	case BURNMODE_TGT:

	  if (dv == 0)
	  {
		  dv = 0;
		  IReference=0;
		  EReference=0;
	  }
	  else
	  {

	    IReference=(TDist-sOffset)/(dv);
	  }

	break;
		  //HIER

  }
  IsArmed=true;
  EReference=oapiGetSimTime()+IReference;
}

void BurnTimeMFD::CalcApses(VESSEL* vessel) {
  ELEMENTS el;
  double MJDRef;
  OBJHANDLE Ref;
  Ref=vessel->GetElements(el,MJDRef);
  e=el.e;
  a=el.a;
  mu=oapiGetMass(Ref)*GGRAV;
  double n=sqrt((e<1?1:-1)*mu/(a*a*a));
  double M=el.L-el.omegab;
  double MJD=oapiTime2MJD(oapiGetSimTime());
  M+=n*(MJD-MJDRef)*86400;
  if(e<1) {
    while(M<0)M+=2*PI;
    while(M>2*PI)M-=2*PI;
  }

  IPeri=-M/n;
  double Period=(2*PI)/n;
  Rperi=a*(1-e);
  if(e<1) {
    Rapo=a*(1+e);
    IApo=IPeri+Period/2;
    while(IPeri<0)IPeri+=Period;
    while(IApo<0)IApo+=Period;
  } else {
    IApo=0;
    Rapo=0;
    if(mode==BURNMODE_APO) mode=BURNMODE_PERI;
  }
}

void BurnTimeMFD::CalcCircular() {
  double Rapse;
  double Vcirc;
  double Vapse;
  switch(mode) {
    case BURNMODE_PERI:
      Rapse=Rperi;
      IReference=IPeri;
      break;
    case BURNMODE_APO:
      Rapse=Rapo;
      IReference=IApo;
      break;
    case BURNMODE_MAN:
      Rapse=Rapo;
      IReference=IManual;
      break;
  }
  Vcirc=sqrt(mu/Rapse);
  Vapse=sqrt(2*mu/Rapse-mu/a);
  dv=fabs(Vcirc-Vapse);
}

void BurnTimeMFD::CalcIBurn(VESSEL* vessel)
{

  getGroupThrustParm(vessel,groups[Sel_eng],&F,&isp);
//vessel = oapiGetFocusInterface();
  mv = vessel->GetMass();
  ms = GetStackMass(vessel);
  //ms = vessel->GetMass();

  // me = vessel->GetEmptyMass();

  THGROUP_HANDLE thgh = vessel->GetThrusterGroupHandle (groups[Sel_eng]);
  if (thgh == NULL)
  {
	  me = 0;
	  IBurn = 0;
	  IBurn2 = 0;
	  return;
  }

  THRUSTER_HANDLE th = vessel->GetGroupThruster(thgh,0);
  if (th == NULL)
  {
	  me = 0;
	  IBurn = 0;
	  IBurn2 = 0;
	  return;
  }
  PROPELLANT_HANDLE ph = vessel->GetThrusterResource(th);
  //double mvvirt = ms - vessel->GetPropellantMass(ph) + vessel->GetPropellantMaxMass(ph);
  if (ph == NULL)
  {
	  me = 0;
	  IBurn = 0;
	  IBurn2 = 0;
	  return;
  }

  mextra = mextra;

  mfuel = vessel->GetPropellantMass(ph) + mextra;
//if (mdot!=0) TTot=(mv-me)/mdot;
  me = ms - mfuel;// + vessel->GetPropellantMaxMass(ph);

  if (mode==BURNMODE_TGT)
  {
    if (dvcurr>dv)
	{
      IBurn = RocketEqnT((((dvcurr-dv)/2)+dv),ms,F,isp);
      IBurn2 = RocketEqnT((((dvcurr-dv)/2)+dv)/2,ms,F,isp);
	}
	else
	{
      IBurn = RocketEqnT((((dv-dvcurr)/2)+dvcurr),ms,F,isp);
      IBurn2 = RocketEqnT((((dv-dvcurr)/2)+dvcurr)/2,ms,F,isp);
	}
  }
  else
  {
    IBurn = RocketEqnT(dv,ms,F,isp);
    IBurn2 = RocketEqnT(dv/2,ms,F,isp);
  }




}

double BurnTimeMFD::GetStackMass(VESSEL* vessel) {
  //We don't have to worry about attachments. They either
  //are compensated for in the main vessel code, or arent,
  //in which case they have no mass. So, it's only docked
  //vessels which we care about

  //So, what we do is:
//Put the current vessel in the vessel-to-check list
  double totalMass=0;
  VESSEL* vesselsToCheck[100];
  int vesselsStored=0;
  vesselsToCheck[vesselsStored]=vessel;
  vesselsStored++;
//For each vessel in the vessel-to-check list
  for(int vesselsChecked=0;vesselsChecked<vesselsStored && vesselsChecked<100;vesselsChecked++) {
//  Accumulate this vessel's mass
    totalMass+=vesselsToCheck[vesselsChecked]->GetMass();
//  For each docking port
    UINT nDockingPorts=vesselsToCheck[vesselsChecked]->DockCount();
    for(UINT i_dock=0;i_dock<nDockingPorts;i_dock++) {
//    Get the docked vessel, if any,
      DOCKHANDLE hDock=vesselsToCheck[vesselsChecked]->GetDockHandle(i_dock);
      OBJHANDLE hVessel=vesselsToCheck[vesselsChecked]->GetDockStatus(hDock);
      VESSEL* pVessel=NULL;
      if(hVessel) pVessel=oapiGetVesselInterface(hVessel);
//    If it is not already in the vessel-to-check list
      bool hasVesselAlready=(pVessel==NULL);
      for(int i_vessel=0;i_vessel<vesselsStored;i_vessel++) if (vesselsToCheck[i_vessel]==pVessel) hasVesselAlready=true;
      if(!hasVesselAlready) {
//      Add it to the end of the list
        vesselsToCheck[vesselsStored]=pVessel;
        vesselsStored++;
//    end if
      }
//  end for
    }
//end for
  }
  return totalMass;
}

void BurnTimeMFD::TimeStep(double ENowTS)
{
  ENow=ENowTS;
  VESSEL *vessel;
  vessel = oapiGetFocusInterface();
  CalcApses(vessel);
  CalcIBurn(vessel);
  if(IsArmed)
  {
    if (mode==BURNMODE_MAN)
	{
      IManual=EReference-ENow;
    }
	if (mode==BURNMODE_TGT)
	{
	  if (dv == 0)
	  {
		  dv = 0;
		  IReference=0;
		  EReference=0;
	  }
	  else
	  {
		 IReference=(TDist-sOffset)/(dv);
	     EReference=oapiGetSimTime()+IReference;
	  }
	}
    if(IsCircular)
    {
      CalcCircular();
      EReference=ENow+IReference;
    }
  }

  if (isp!=0) mdot=F/isp;
  else mdot=0;

  if(IsEngaged)
  {
    mfinal=mv-(ECutoff-ENow)*mdot;
    msfinal=ms-mv+mfinal;
	if (msfinal!=0) dv=isp*log(ms/msfinal);
	else dv=0;
  }
  else
  {
    mfinal=mv-IBurn*mdot;
  }
  msempty=ms-mv+me;

  if (msempty !=0 ) maxdv = isp * log( ms / (ms - mfuel) );
  else maxdv=0;

  if (mdot!=0) TTot=mfuel/mdot;
  else TTot=0;

  static double dvOld = 0;
  OBJHANDLE hSObj;
  VECTOR3 refval;

  vessel = oapiGetFocusInterface();
  OBJHANDLE obj;
  obj = oapiGetFocusObject();
  vessel = oapiGetVesselInterface(obj);
  hSObj = oapiGetObjectByIndex(IndexCenterObj);
  double dvtmp=0;

  if (mode==BURNMODE_TGT)
  {
	vessel->GetRelativeVel(hSObj,refval);
	dvcurr = sqrt((refval.x*refval.x)+(refval.y*refval.y)+(refval.z*refval.z));

	//IReference= (TDist-sOffset)/(((dvcurr-dv)/2)+dv);
	if (dvcurr>dv) IReference= (TDist-sOffset)/(dv+((dvcurr-dv)/2));
	else           IReference= (TDist-sOffset)/(dvcurr +((dv-dvcurr)/2));

    EReference=oapiGetSimTime()+IReference;

//	sprintf(oapiDebugString(),"dVcurr: %f, dV: %f; MID: %f",dvcurr,dv,((dvcurr-dv)/2)+dv);
	vessel->GetRelativePos(hSObj,refval);
	TDist = sqrt((refval.x*refval.x)+(refval.y*refval.y)+(refval.z*refval.z));
  }
  else if (mode >= BURNMODE_TGT) mode = BURNMODE_PERI;


  if(IsEngaged && ENow>ECutoff && mode!=BURNMODE_TGT)
  {
	oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],0);
    IsEngaged=false;
    dv=0;
	IManual = 0;
  }

  if (mode==BURNMODE_TGT && dvOld<dvcurr && IsEngaged)
  {
    oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],0);
    IsEngaged=false;
    dv=0;
  }

  if(IsArmed && ENow>(EReference-IBurn2))
  {
	oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(groups[Sel_eng],1);
    IsEngaged=true;
    IsArmed=false;
    IsCircular=false;
    ECutoff=ENow+IBurn;
  }
  dvOld = dvcurr;
}

int BurnTimeMFD::line( int i ) {
  return (int)((float)i*((float)height/20.0));
}

int BurnTimeMFD::ButtonMenu (const MFDBUTTONMENU **menu) const {
  return gButtons.ButtonMenu(menu);
}

char *BurnTimeMFD::ButtonLabel (int bt) {
  //char *label[12] = {"DV","DT","OS","ST","RST", "MD", "ARM","BRN", "CIR","UNT","ENG","EXT"};
  //return (bt < 12 ? label[bt] : 0);
  return gButtons.ButtonLabel(bt);
}

bool BurnTimeMFD::ConsumeButton(int bt, int event) {
  return gButtons.ConsumeButton(this, bt, event);
}
