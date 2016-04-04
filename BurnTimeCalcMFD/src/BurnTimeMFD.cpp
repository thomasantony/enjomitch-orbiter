// ==============================================================
// $Id: BurnTimeMFD.cpp,v 1.5 2004/05/25 19:08:46 dh219 Exp $
// BTC by D Henderson orbiter@aibs.org.uk
// Copyright 2004
// Extensively modified by C Jeppesen chrisj@kwansystems.org
// Copyright 2005-2006
// Extended with the ability to use different Engines (Hover,Main,Retro) by Topper
// Copyright 2007
// Version 1.5 by kwan3217 (Copyright (c) 2008):
//		Some code cleanup,
//		Bug case where engine group existed but was not connected to a propellant resource - FIXED
//		Works in SI or feet (for use with Space Shuttle)
// Version 2.0 by Topper Copyright (c)
//		Var-setting now working via inputbox
//		Entering of Targets (Vessels or Gbodys) instead of dv possible
//		New Autoburnmode "Burn by target distance" added
// Version 2.5 by Enjo (Copyright (c) 2014)
//    Interconnected with TransX via new ModuleMEssaging library (GET button)
//    Added a second button page via MFDButtonPage
// Version 2.6 by Enjo (Copyright (c) 2014)
//    Resolved CTDs when switching the MFD on and off
//    Simultaneous multiple vessels support (can be pre-programmed!) thanks to MultipleVesselsMFD
//    Added RCS Forward, Up, and Retro "engine types"
// Version 2.7 by Enjo (Copyright (c) 2014)
//    Resolved sporadic freeze on startup due to displaying of uninitialized variables
//    Resolved craft's "orbit freeze" bug when the MFD was closed and Time Acc. >= 10000 was applied
//    Added new button - RCS for including RCS fuel in total DV calculations
//    Internal: Changed "TBurn" ModuleMessaging variable to "InstantaneousBurnTime".
//    TransX update required for GET button to operate correctly
// Version 2.8 by ADSWNJ (Copyright (c) 2016)
//    Interconnected with BaseSync MFD for plane change and retro burns. GET button now auto-selects
//    TransX or BaseSync if either is running, or prompts user if both are running.
//    Miscellaneous display mods (removed space between number and symbol when the scale unit is x1,
//    put the Selected Engine Not Available in red, with advice to check doors and fuel, consistent
//    capitalization and decimal point alignment, etc.)
//    Replaced periapse and apoapse text with periapsis and apoapsis.
//    Put version number on MFD title, similar to BaseSyncMFD.
//    Updated the LGPL license text to version 3, below.
// Version 2.9 by ADSWNJ (Copyright (c) 2016)
//    Fixed text overwrite issue when in Target Mode and using invalid engine (i.e. doors closed or no propellant)
//    Fixed bug when using invalid engine and docked (burns instantly complete, instead of stating invalid engine)
//

// Released under the terms of the LGPL: http://www.gnu.org/licenses/lgpl.txt
/*
    This software and sourcecode is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This Orbiter addon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this software and sourcecode; if not, see <http://www.gnu.org/licenses/>.

*/

// ==============================================================

#define STRICT
#define ORBITER_MODULE

#include "globals.h"
#include "MFDDataBurnTime.h"
#include "PluginBurnTime.h"

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
#define INPUTMODE_MFDSEL 6

#include "BurnTimeMFD.h"
#include "MFDButtonPageBTC.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string>
#include <cctype>
#include <algorithm>
//#include <EnjoLib/ModuleMessaging.hpp>
#include "EnjoLib/ModuleMessagingExt.hpp"
#include "BaseSyncExports.hpp"

#define mToft 1.0/0.3048
#define gTolb 1.0/0.45359237
#define mpersToftpers 1.0/9.80665
#define NTolbf 0.224808943
#define mperssqToftperssq 1.0/0.3048

static MFDButtonPageBTC gButtons;
extern PluginBurnTime * plugin;

int DisplayEngUnit(char* buffer, char* pattern, char* noscalepattern, double x) {
  char Big[]=  " kMGTPEZY";
  char Small[]=" munpfazy";
  int ptr=0;
  if(fabs(x)>1e-24) {
    if(fabs(x)>1) {
      while(fabs(x)>1000) {
        ptr++;
        x=x/1000.0;
      }
      if (ptr==0) {
        return sprintf(buffer,noscalepattern,x);
      } else {
        return sprintf(buffer,pattern,x,Big[ptr]);
      }
    } else {
      while(fabs(x)<1) {
        ptr++;
        x=x*1000.0;
      }
      if (ptr==0) {
        return sprintf(buffer,noscalepattern,x);
      } else {
        return sprintf(buffer,pattern,x,Small[ptr]);
      }
    }
  } else {
    return sprintf(buffer,noscalepattern,x);
  }
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* unitSI, const char* unitUS, double multSI, double multUS, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,m_data->dspunit==0?unitSI:unitUS);
  sprintf(noscalelabel,"%s%s",format,m_data->dspunit==0?unitSI:unitUS);
  int len=DisplayEngUnit(buf,label,noscalelabel,value*(m_data->dspunit==0?multSI:multUS));
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double mult, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,units);
  sprintf(noscalelabel,"%s%s",format,units);
  int len=DisplayEngUnit(buf,label,noscalelabel,value*mult);
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintEngUnit(oapi::Sketchpad * skp, const char* format, const char* units, double value, int x, int l) {
  char buf[256];
  char label[256];
  char noscalelabel[256];
  sprintf(label,"%s%%c%s",format,units);
  sprintf(noscalelabel,"%s%s",format,units);
  int len=DisplayEngUnit(buf,label,noscalelabel,value);
  skp->Text(x,line(l),buf,len);
}

void BurnTimeMFD::PrintString(oapi::Sketchpad * skp, const char* format, const char* value, int x, int l) {
  char buf[256];
  sprintf(buf,format,value);
  skp->Text(x,line(l),buf,strlen(buf));
}

void BurnTimeMFD::PrintString(oapi::Sketchpad * skp, const char* format, int x, int l) {
  char buf[256];
  skp->Text(x,line(l),format,strlen(buf));
}

BurnTimeMFD::BurnTimeMFD (DWORD w, DWORD h, VESSEL *vessel, PluginBurnTime * plugin)
 : MFD2 (w, h, vessel)
 , m_data(dynamic_cast<MFDDataBurnTime *>(plugin->AssociateMFDData(vessel)))
 {
  height = (int)h;
  if (m_data->mul==0) m_data->mul=1.0;
}

BurnTimeMFD::~BurnTimeMFD ()
{
}

bool BurnTimeMFD::Update(oapi::Sketchpad * skp)
{
	m_data->Update();
  unsigned int thrustercount = 0;

  Title (skp, "BurnTimeMFD v2.9");

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

  skp->SetTextColor(GRAY);

  if (m_data->mode == BURNMODE_PERI) PrintEngUnit(skp,"Time to Periapsis:     %7.3f","s", m_data->IPeri, 5, line8 );
  if(m_data->IApo>0 && m_data->mode == BURNMODE_APO)
  {
    skp->SetTextColor(GRAY);
    PrintEngUnit(skp,"Time to Apoapsis:      %7.3f","s", m_data->IApo, 5, line8 );
  }

  if (m_data->mode == BURNMODE_MAN)
  {
    switch (m_data->otherMFDsel) {
    case 0:
      skp->SetTextColor(((m_data->inputmode==INPUTMODE_TIME )?YELLOW:GRAY));
      PrintEngUnit(skp,"Time to Manual Start:  %7.3f","s", m_data->IManual, 5, line8 );
      break;
    case 1:
      skp->SetTextColor(YELLOW);
      PrintEngUnit(skp,"TransX Burn Time:      %7.3f","s", m_data->IManual, 5, line8 );
      break;
    case 2:
      skp->SetTextColor(YELLOW);
      PrintEngUnit(skp,"BaseSync Burn Time:    %7.3f","s", m_data->IManual, 5, line8 );
      break;
    }
  }

  if (m_data->mode == BURNMODE_TGT && m_data->maxdv!=0 && m_data->ph != NULL)
  {

    skp->SetTextColor(GRAY);
    PrintEngUnit(skp,"Distance to Target:    %7.3f","m", "ft",1,mToft,m_data->TDist, 5, line8 );
    skp->SetTextColor(((m_data->inputmode==INPUTMODE_OFFSET )?YELLOW:GRAY));
    PrintEngUnit(skp,"Offset Distance:       %7.3f","m", "ft",1,mToft,m_data->sOffset, 5, line9 );
  }

  skp->SetTextColor( BRIGHTERGRAY );
  PrintString(skp,"Engine:                   %s",&m_data->group_names[m_data->Sel_eng][0],5,line10);

  if(m_data->maxdv==0 || m_data->ph == NULL)
  {
	  skp->SetTextColor( RED );
    PrintString(skp,"Selected engine not available%s","",5,line2);
    PrintString(skp,">>> Check engine doors and fuel%s","",5,line9);
  }
  else
  {
	if(m_data->IsEngaged)
	{
	  skp->SetTextColor( RED );
	  skp->Text(5,line(line3),"Autoburn Engaged",16);
	  PrintEngUnit(skp,"Time to Cutoff:        %7.3f","s", m_data->ECutoff-m_data->ENow,5,line4);
	}
	else if(m_data->IsArmed)
	{
	  skp->SetTextColor( YELLOW );

	  if(m_data->IsCircular)
	  {
	     PrintEngUnit(skp,"Autocirc Armed:        %7.3f","s", m_data->IBurn,5,line3);
	  }
	  else
	  {
	    PrintEngUnit(skp,"Autoburn Armed:        %7.3f","s", m_data->IBurn,5,line3);
	  }
	  PrintEngUnit(skp,"Time to Ignition:      %7.3f","s", m_data->EReference-m_data->ENow-m_data->IBurn2,5,line4);
	}
	else
	{
	  skp->SetTextColor( GREEN );
	  PrintEngUnit(skp,"Estimated Burn Time:   %7.3f","s", m_data->IBurn,5,line3);
	  PrintEngUnit(skp,"Half DeltaV Time       %7.3f","s", m_data->IBurn2,5,line4);
	  PrintEngUnit(skp,"Rel. DeltaS Distance   %7.3f","m", "ft",1,mToft,m_data->dv*m_data->IBurn2, 5, line5 );
	}

	skp->SetTextColor( (m_data->inputmode==INPUTMODE_DV)?YELLOW:GRAY );
  if (m_data->mode == BURNMODE_MAN) {
    switch (m_data->otherMFDsel) {
    case 0:
      PrintEngUnit(skp,"Target DeltaV:         %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
      break;
    case 1:
    	skp->SetTextColor(YELLOW);
      PrintEngUnit(skp,"TransX DeltaV:         %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
      break;
    case 2:
    	skp->SetTextColor(YELLOW);
      switch (m_data->BSori) {
      case -1:
        PrintEngUnit(skp,"BaseSync Nml- dV:      %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
        break;
      case 0:
        PrintEngUnit(skp,"BaseSync Retro dV:     %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
        break;
      case 1:
        PrintEngUnit(skp,"BaseSync Nml+ dV:      %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
        break;
      }
      break;
    }
  } else {
    PrintEngUnit(skp,"Target DeltaV:         %7.3f","m/s","ft/s",1,mToft, m_data->dv, 5, line1 );
  }

	skp->SetTextColor( (m_data->inputmode==INPUTMODE_EXTRA)?YELLOW:BLUE );
	PrintEngUnit(skp,"Extra Fuel Mass:       %7.3f","g","lbm", 1000,gTolb,m_data->mextra,5,line19);
	if (m_data->mextra !=0){
		skp->SetTextColor( ORANGE );
	    PrintEngUnit(skp,"Extra Fuel Mass:       %7.3f","g","lbm", 1000,gTolb,m_data->mextra,5,line19);
	}



	std::string TargetString;

	char buffer[255];

	if (m_data->mode == BURNMODE_TGT)
	{
	  skp->SetTextColor(m_data->inputmode==INPUTMODE_TARGET?YELLOW:GRAY);
	  oapiGetObjectName(oapiGetObjectByIndex(m_data->IndexCenterObj),buffer,255);
	  TargetString = "=> Selected Object: " + std::string(buffer);
	  skp->Text(5,line(line2),TargetString.c_str(),TargetString.length());
	}

	skp->SetTextColor( ORANGE );
	std::string totalDV = std::string("Estimate Total dV") + (m_data->includeRCS ? "+RCS: " : ":     ") + "%7.3f";
	PrintEngUnit(skp,totalDV.c_str()                ,"m/s","ft/s",1,mToft, m_data->maxdv,5,line7);
	PrintEngUnit(skp,"Total Burn Time:       %7.3f","s", m_data->TTot,5,line6);
	skp->SetTextColor( BLUE );
	PrintEngUnit(skp,"Current Vehicle Mass:  %7.3f","g","lbm", 1000,gTolb,m_data->mv,5,line11);
	PrintEngUnit(skp,"Current Stack Mass:    %7.3f","g","lbm", 1000,gTolb, m_data->ms,5,line12);
	PrintEngUnit(skp,"Postburn Vehicle Mass: %7.3f","g","lbm", 1000,gTolb, m_data->mfinal,5,line13);
	PrintEngUnit(skp,"Empty Vehicle Mass:    %7.3f","g","lbm", 1000,gTolb, m_data->me,5,line14);
	PrintEngUnit(skp,"Mass Flow Rate:        %7.3f","g/s","lbm/s", 1000,gTolb, m_data->mdot,5,line15);
	PrintEngUnit(skp,"Eng Thrust:            %7.3f","N","lbf",1,NTolbf, m_data->F,5,line16);
	PrintEngUnit(skp,"Eng Isp:               %7.3f","m/s","ft/s",1,mpersToftpers, m_data->isp,5,line17);
	PrintEngUnit(skp,"Eng Acc:               %7.3f","m/s²","ft/s²",1,mperssqToftperssq,m_data->F/m_data->ms,5,line18);
  }
  return true;
}

int BurnTimeMFD::MsgProc (UINT msg, UINT mfinald, WPARAM wparam, LPARAM lparam)
{
  switch (msg)
  {
    case OAPI_MSG_MFD_OPENED:
       return (int)(new BurnTimeMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, plugin));
  }

  return 0;
}

MFDDataBurnTime * BurnTimeMFD::GetData()
{
    return m_data;
}


bool ObjectInput (void *id, char *str, void *usrdata)
{
  if ( str[0] == 0) return true;
	BurnTimeMFD * btcMFD = (BurnTimeMFD*)usrdata;
    MFDDataBurnTime * data = btcMFD->GetData();

	if (data->inputmode == INPUTMODE_MFDSEL ) {
    using namespace EnjoLib;
    if (strlen(str) != 1) return false;
    bool TransX = (*str == '1');
    bool BaseSync = (*str == '2');
    if (!TransX && !BaseSync) return false;

    double dV;
    double IBT;

    if (TransX) {
      TransX = btcMFD->GetFromTransX(&dV, &IBT);
    } else {
      BaseSync = btcMFD->GetFromBaseSync(&dV, &IBT);
    }

    if (!TransX && !BaseSync) {
      // Data currently invalid ... default back to manual burn
      data->mode = BURNMODE_MAN;
      data->otherMFDsel = 0;
      return true;
    }
    btcMFD->SetOtherMFDBurnVars(TransX, dV, IBT);
    return true;
  }


	std::string SearchString(str);
	OBJHANDLE hFObj = oapiGetObjectByName(str);
	//Auslesen des letzten Zeichens und Bestimmen des Expondentens (read last char to check for exponent letter)
	std::string InputUnits =  "yzafpnum_kMGTPEZY;";
	std::string RightChar = std::string(SearchString.substr(SearchString.length()-1,SearchString.length()));
	std::string numbers = "0123456789;";
	//Überprüfe ob der String eine Zahl ist... (check if the string is a number)


	if (data->inputmode != INPUTMODE_TARGET )
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
		// Wenn kein Einheitenfaktor gefunden wurde...  (if an exponent factor is found)
		if (InputUnits.substr(lauf,1)==";")
		{
			if (RightChar != "0" && atoi(RightChar.c_str()) == 0) return false;
			if (data->inputmode==INPUTMODE_OFFSET )
			{
				data->sOffset = atof(SearchString.c_str());
				if (data->dspunit == 1) data->sOffset = data->sOffset  * 0.3048;
			}
			else if (data->inputmode==INPUTMODE_DV )
			{
				data->dv = atof(SearchString.c_str());
				if (data->dspunit == 1) data->dv = data->dv * 0.3048;
			}
			else if (data->inputmode==INPUTMODE_EXTRA )
			{
				data->mextra = atof(SearchString.c_str());
				if (data->dspunit == 1) data->mextra = data->mextra * 0.45359237;
			}
			else if (data->inputmode==INPUTMODE_TIME )
      {
        data->EReference = oapiGetSimTime()+atof(SearchString.c_str());
        data->IManual = atof(SearchString.c_str());
      }
		}
		else
		{
			if (data->inputmode==INPUTMODE_EXTRA )
			{
				data->mextra = atof(SearchString.c_str());
				if (data->dspunit == 1) data->mextra = data->mextra * 0.45359237;
			}
			if (data->inputmode==INPUTMODE_OFFSET )
			{
				data->sOffset = atof(SearchString.c_str()) * pow(10,MyExp);
				if (data->dspunit == 1) data->sOffset = data->sOffset * 0.3048;
			}
			else if (data->inputmode==INPUTMODE_DV )
			{
				data->dv = atof(SearchString.c_str()) * pow(10,MyExp);
				if (data->dspunit == 1) data->dv = data->dv * 0.3048;
			}
			else data->IManual = atof(SearchString.c_str()) * pow(10,MyExp);
			//sprintf(oapiDebugString(),"v1:%f  v2:%f ",atof(SearchString.c_str()),pow(10,MyExp));
		}
		data->inputmode=INPUTMODE_NONE;
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
			   data->IndexCenterObj = i;
			   data->inputmode=INPUTMODE_NONE;
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
			   data->IndexCenterObj = i;
			   data->inputmode=INPUTMODE_NONE;
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
    m_data->inputmode=INPUTMODE_DV;
    m_data->mode = BURNMODE_MAN;
    m_data->otherMFDsel = 0;
    bool ObjectInput (void *id, char *str, void *usrdata);
    oapiOpenInputBox("Enter dV + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTimeOfManoeuvre()
{
   m_data->inputmode=INPUTMODE_TIME;
   m_data->mode = BURNMODE_MAN;
   m_data->otherMFDsel = 0;
   bool ObjectInput (void *id, char *str, void *usrdata);
   oapiOpenInputBox("Enter dT + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerOffsetDistance()
{
    m_data->inputmode=INPUTMODE_OFFSET;
    m_data->mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter offset distance + yzafpnum kMGTPEZY.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerTargetForDistanceCalc()
{
    m_data->inputmode=INPUTMODE_TARGET;
    m_data->mode = BURNMODE_TGT;
    oapiOpenInputBox("Enter unique part of the target's name.",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerReset()
{
	  m_data->mul=1.0;
	  m_data->dv = 0.0;
	  m_data->mextra = 0.0;
	  m_data->ECutoff=0;
	  m_data->IsEngaged=false;
	  m_data->IsArmed=false;
	  m_data->mode=BURNMODE_PERI;
    m_data->otherMFDsel = 0;
	  m_data->IManual=0;
	  m_data->IsCircular=false;
}

void BurnTimeMFD::HandlerChangeMode()
{
	  if(m_data->IsArmed && m_data->IsEngaged) return;
	  m_data->mode++;
	  if(m_data->mode>=BURNMODE_TGT) m_data->mode=0;
}

void BurnTimeMFD::HandlerAutoBurn()
{
      if(m_data->IsArmed)
      {
        m_data->IsArmed=false;
        m_data->otherMFDsel = 0;
        return;
      }
      if(m_data->IsEngaged)
      {
        m_data->IsEngaged=false;
      }
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],0);
      m_data->ArmAutoBurn();
}

void BurnTimeMFD::HandlerBurnNow()
{
      m_data->IsArmed=false;
      m_data->IsEngaged=true;
      m_data->ECutoff=oapiGetSimTime()+m_data->IBurn;
      oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],1.0);
}

void BurnTimeMFD::HandlerAimAutoCirc()
{
      if (m_data->mode > BURNMODE_APO) m_data->mode = BURNMODE_PERI;
      if(m_data->IsArmed)
      {
        m_data->IsArmed=false;
        m_data->IsCircular=false;
        return;
      }
      if(m_data->IsEngaged)
      {
        m_data->IsEngaged=false;
        oapiGetVesselInterface(oapiGetFocusObject())->SetThrusterGroupLevel(m_data->groups[m_data->Sel_eng],0);
      }
      m_data->IsCircular=true;
      m_data->CalcCircular();
      m_data->ArmAutoBurn();
}

void BurnTimeMFD::HandlerSwitchSI_US()
{
    m_data->dspunit=1-m_data->dspunit;
}

void BurnTimeMFD::HandlerSelectEngine()
{
      m_data->Sel_eng++;
      if (m_data->Sel_eng==MFDDataBurnTime::numEngines) m_data->Sel_eng=0;
}

void BurnTimeMFD::HandlerEnterExtraFuel()
{
	  m_data->inputmode=INPUTMODE_EXTRA;
	  bool ObjectInput (void *id, char *str, void *usrdata);
	  oapiOpenInputBox("Enter extra fuel mass in kilograms",ObjectInput,0,20, (void*)this);
}

void BurnTimeMFD::HandlerIncludeRCSFuel()
{
    m_data->includeRCS = ! m_data->includeRCS;
}

void BurnTimeMFD::HandlerGetFromOtherMFD()
{
    double dV_TransX, IBT_TransX;
    double dV_BaseSync, IBT_BaseSync;
    bool TransX, BaseSync;

    TransX = GetFromTransX(&dV_TransX, &IBT_TransX);
    BaseSync = GetFromBaseSync(&dV_BaseSync, &IBT_BaseSync);

    if (TransX && BaseSync) { // 2 valid data sources ... ask user to choose
      m_data->inputmode=INPUTMODE_MFDSEL;
      bool ObjectInput (void *id, char *str, void *usrdata);
      oapiOpenInputBox("Enter 1 for TransX or 2 for BaseSync",ObjectInput,0,20, (void*)this);
      return;
    }

    if (!TransX && !BaseSync) {
      // no data exposed
      return;
    }

    if (TransX) {
      SetOtherMFDBurnVars(true, dV_TransX, IBT_TransX);
    } else {
      SetOtherMFDBurnVars(false, dV_BaseSync, IBT_BaseSync);
    }
}

bool BurnTimeMFD::GetFromTransX(double *dV, double *IBT) {
    using namespace EnjoLib;
    ModuleMessagingExt mm;
    double dvRes = 0, IManualRes = 0;
    if (mm.ModMsgGet("TransX", "dv", &dvRes) &&
        mm.ModMsgGet("TransX", "InstantaneousBurnTime", &IManualRes)) {
        // Receiving TransX
      *dV = dvRes;
      *IBT = IManualRes;
      return true;
    } else {
      *dV = 0;
      *IBT = 0;
      return false;
    }
}


bool BurnTimeMFD::GetFromBaseSync(double *dV, double *IBT) {
    using namespace EnjoLib;
    ModuleMessagingExt mmext;
    bool receivingBS = false;

    if (m_data->BS_burn != NULL) {
      receivingBS = m_data->BS_burn->dataValid;
    } else {
      receivingBS = mmext.ModMsgGetByRef("BaseSyncMFD","BaseSyncBurn",1,&(m_data->BS_burn));
      if (receivingBS) receivingBS = m_data->BS_burn->dataValid;
    }

    if (receivingBS) {
      *dV = m_data->BS_burn->dV;
      *IBT = m_data->BS_burn->tToInstBurn;
      m_data->BSori = m_data->BS_burn->orientation;
      return true;
    } else {
      *dV = 0;
      *IBT = 0;
      return false;
    }
}


void BurnTimeMFD::SetOtherMFDBurnVars(bool TransX, double dV, double IBT) {
    m_data->IsArmed=m_data->IsEngaged=false;
    m_data->mode = BURNMODE_MAN;
    if (abs(dV) > 1e-5 && IBT > 0) { // We're not interested in negative times or zero dV's
      if (TransX) {
        m_data->otherMFDsel = 1;
      } else {
        m_data->otherMFDsel = 2;
      }
      m_data->dv = abs(dV);
      m_data->IManual = IBT;
      HandlerAutoBurn();
    } else {
      m_data->otherMFDsel = 0;
    }
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
