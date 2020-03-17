// ==============================================================
//                 ORBITER MODULE: LaunchMFD
//                  Part of the ORBITER SDK
//
// Copyright (C) 2004      rjcroy                      - robust time based pitch autopilot (borrowed code)
// Copyright (C) 2004      Dave "Daver" Rowbotham      - conversion of rjcroy's autopolot to C++ (borrowed code)
// Copyright (C) 2004      Erik H. "Sputnik" Anderson  - conversion of the autopilot to energy based (borrowed code)
// Copyright (C) 2007      "Vanguard"                  - dressing up azimuth calcualtions into an MFD (author)
// Copyright (C) 2007      Pawel "She'da'Lier" Stiasny - yaw error visual representation (contributor)
// Copyright (C) 2008      Mohd "Computerex" Ali       - borrowed his code (multiple vessels support) (borrowed code)
// Copyright (C) 2008      Chris "Kwan" Jeppesen       - borrowed his code (peg guidance) (borrowed code)
// Copyright (C) 2008      Steve "agentgonzo" Arch     - peg integration, offplane correction, compass, hud display (co-developer)
// Copyright (C) 2007-2012 Szymon "Enjo" Ender         - everything else ;> (author and maintainer)
//                         All rights reserved
//
// LaunchMFD.cpp - Main MFD class
// Authors - "Vanguard", Szymon "Enjo" Ender, Steve "agentgonzo" Arch, Mohd "Computerex" Ali
//
// This module calculates the appropriate launch azimuth given
// desired orbital inclination and desired orbit altitude. This
// MFD takes the planets rotation into account, which provides a
// much more accurate azimuth. The calculations are performed
// 'on the fly' (technically and methaphorically), meaning that
// you get info about necessary course corrections.
//
// This file is part of LaunchMFD.
//
// LaunchMFD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaunchMFD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaunchMFD.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#define STRICT
#define ORBITER_MODULE

#include "globals.h"
#include "Orbitersdk.h"
#include "LaunchMFD.h"
#include "PluginLaunchMFD.hpp"
#include "MFDDataLaunchMFD.hpp"
#include <cmath>
#include <sstream>
#include "localisation.h"

#include <MFDSound++/Sound.hpp>
#include "Sound/SoundSampleIDEnum.hpp"
#include "Math/GeneralMath.hpp"
#include <Orbiter/SpaceMathOrbiter.hpp>
#include "Util/CharManipulations.hpp"
#include "Utils/Targeting.hpp"
#include "Dialogs/DialogAlt.h"
#include "Dialogs/DialogDistDA.h"
#include "Dialogs/DialogTarget.h"
#include "Dialogs/DialogPID.h"
#include <Utils/MFDTextCalculator.hpp>

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

// ==============================================================
// Global variables
extern PluginLaunchMFD * plugin;

// ==============================================================
// MFD class implementation
const double LaunchMFD::MAX_ERROR = 0.2;

// Constructor
LaunchMFD::LaunchMFD (DWORD w, DWORD h, VESSEL *vessel, PluginLaunchMFD * plugin )
    : MY_MFD (w, h, vessel)
// Initialisation list
// init m_data with plugin's return value, depending on the vessel pointer
    , m_data(dynamic_cast<MFDDataLaunchMFD *>(plugin->AssociateMFDData(vessel)))
    , m_sound(plugin->GetSound())
    , m_buttonPages(plugin->GetButtonPages())
    , compass(w, h, vessel)
    , daAtmo(w, h, vessel, plugin->GetSound())
{
    m_beep = false;
    if ( m_data == NULL ) // Programming error
        sprintf_s(oapiDebugString(), 512, "m_data pointer type is not compatible "
                  "with the pointer that was being assigned to it in Ctor");
}

// Destructor
LaunchMFD::~LaunchMFD ()
{
    // Add MFD cleanup code here
}

#include "MessagingSender.h"

// Repaint the MFD
MFD_RETURN_TYPE LaunchMFD::Update (MyDC hDC)
{
    if ( m_data == NULL )
        return MFD_RETURN_VALUE( false );

    //MessagingSender mm; // Testing BTC
    //mm.ModMsgPut("InstantaneousBurnTime", 50.0);
    //mm.ModMsgPut("dv", 50.0);

    // Update all ship's variables
    m_data->Update();
    m_data->peg.GetTargetPitch(); // Get TMECO
    m_data->m_greatCircle.Update(hDC, W, H, m_data); // GC will be displayed for all modes

    ReactOnShipStatus();
    ReactOnReachingOrbit(hDC);
    SendModuleMessages();

    if (m_data->pageView == AZIMUTH)
    {
        // Draws the MFD title
        Title (hDC, TITLE_AZIMUTH);
    }
    else if (m_data->pageView == COMPASS)
    {
        Title (hDC, TITLE_COMPASS);
        TGTPARAM target = m_data->GetTgtParam();
        compass.SetTargetAzimuth(m_data->GetAzimuth(), target.azFlag);
        compass.SetApses(m_data->PeA, m_data->ApA);
        compass.SetTargetPitch(m_data->GetTargetPitch());
        return compass.Update(hDC, m_data);
    }

    if ( ! m_data->IsTargetSet() )	m_data->GetTgtParam().timeToNode = 0;

    if (m_data->pageView == DIRECT_ASCENT)
    {
        Title (hDC, TITLE_DIRECT_ASCENT);
        daAtmo.Update(hDC, m_data);
    }
    // everything below this line concerns both Azmimuth and Direct Ascent modes, unless the functions' names tell different
    SetError();
    CheckErrorExcess();
    PrintResults(hDC);
    PrintTargetParamsForAzimuthMode(hDC);
    DrawErrorAndMarks(hDC, m_data->GetMovParams().vesselStatus.status);

    //sprintf(oapiDebugString(), "newAtmosphericModel = %d", m_data->newAtmosphericModel  );
    //sprintf(oapiDebugString(), "GetTrimScale() = %.20lf", pV->GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM)  );
    //sprintf(oapiDebugString(), "VESSEL version = %d", pV->Version()  );

    return MFD_RETURN_VALUE( true );
}

// Return button labels
char * LaunchMFD::ButtonLabel (int bt)
{
    return m_buttonPages.ButtonLabel(bt);
}

// Return button menus
int LaunchMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
    return m_buttonPages.ButtonMenu( menu );
}

bool LaunchMFD::ConsumeButton (int bt, int event)
{
    return m_buttonPages.ConsumeButton(this, bt, event);
}

bool LaunchMFD::ConsumeKeyBuffered(DWORD key)
{
    return m_buttonPages.ConsumeKeyBuffered(this, key);
}

bool LaunchMFD::ConsumeKeyImmediate( char * kstate )
{
    return m_buttonPages.ConsumeKeyImmediate(this, kstate);
}

// Event handlers
void LaunchMFD::SwitchButtonsPage()
{
    m_buttonPages.SwitchPage(this);
}

void LaunchMFD::SwitchMode()
{
        switch(m_data->pageView)
        {
        case AZIMUTH:
            m_data->pageView = COMPASS;
            break;
        case COMPASS:
            m_data->pageView = DIRECT_ASCENT;
            break;
        case DIRECT_ASCENT:
            m_data->pageView = AZIMUTH;
            break;
        default:
            m_data->pageView = AZIMUTH;
            break;	// fallback in case something goes wrong
        }
}
void LaunchMFD::SwitchHUD()
{
    m_data->hud = ! m_data->hud;
    m_sound.PlayWave( m_data->hud ? HUD_ENABLED : HUD_DISABLED );
}

void LaunchMFD::SwitchAutopilot()
{
    if ( m_data->pageView == DIRECT_ASCENT )
        m_data->SwitchAutopilot( m_data->m_daSynchroOrbit ? AP_DIRECT_ASCENT_SYNCHRO : AP_DIRECT_ASCENT_STD );
    else
        m_data->SwitchAutopilot( AP_STANDARD );
}

void LaunchMFD::SwitchPitchGuidance()
{
    m_data->drawPitchError = ! m_data->drawPitchError;
}

void LaunchMFD::SwitchOffplaneCorrection()
{
    m_data->m_useOffplaneCorrector = ! m_data->m_useOffplaneCorrector;	// toggle off-plane corrections
}

void LaunchMFD::SwitchAzimuth()
{
    m_data->SwitchAzimuth();
}

void LaunchMFD::SwitchSound()
{
    m_beep = ! m_beep;
    if ( m_beep ) m_sound.PlayWave(BEEP_ENABLED);
}
void LaunchMFD::SwitchGreatCircleUse()
{
    m_data->m_greatCircle.SwitchUse();
}
void LaunchMFD::SwitchGreatCircleTrack()
{
    m_data->m_greatCircle.SwitchTrack();
}
void LaunchMFD::SwitchGreatCircleLines()
{
    m_data->m_greatCircle.m_continuous = ! m_data->m_greatCircle.m_continuous;
}
void LaunchMFD::GreatCircleZoomIn()
{
    m_data->m_greatCircle.ZoomIn();
}
void LaunchMFD::GreatCircleZoomOut()
{
    m_data->m_greatCircle.ZoomOut();
}
void LaunchMFD::GreatCircleIncreasePlotPrecision()
{
    m_data->m_greatCircle.IncreasePlotPrecision();
}
void LaunchMFD::GreatCircleDecreasePlotPrecision()
{
    m_data->m_greatCircle.DecreasePlotPrecision();
}
void LaunchMFD::IncreaseInclination()
{
    const double & lat = m_data->GetMovParams().m_lat;
    double & incl = m_data->GetTgtParam().incl;
    incl += m_data->InclinationFactor * RAD;
    if ( incl > PI - fabs(lat) )   incl = PI - fabs(lat) - SMALL_DOUBLE;
    if ( m_data->tgt_set )
    {
        m_sound.PlayWave(TARGET_MANUAL);
        m_data->tgt_set = false;
    }
    m_data->SetTargetStr( MANUAL, MANUAL );
}
void LaunchMFD::DecreaseInclination()
{
    const double & lat = m_data->GetMovParams().m_lat;
    double & incl = m_data->GetTgtParam().incl;
    incl -= m_data->InclinationFactor * RAD;
    if ( incl < fabs(lat) )     incl = fabs(lat) + SMALL_DOUBLE;
    if ( m_data->tgt_set )
    {
        m_sound.PlayWave(TARGET_MANUAL);
        m_data->tgt_set = false;
    }
    m_data->SetTargetStr( MANUAL, MANUAL );
}
void LaunchMFD::IncreaseInclinationFactor()
{
        m_data->InclinationFactor *= 2;
        if (m_data->InclinationFactor > 1) m_data->InclinationFactor = 1;
}
void LaunchMFD::DecreaseInclinationFactor()
{
        m_data->InclinationFactor /= 2;
        if (m_data->InclinationFactor < 0.01) m_data->InclinationFactor = 0.01;
}

void LaunchMFD::DefaultAction()
{
    switch (m_data->pageView)
    {
    case AZIMUTH:
        m_data->drawPitchError = ! m_data->drawPitchError;
        break;
    case COMPASS:
        m_data->ChangeCompassDisplayMode();
        break;
    case DIRECT_ASCENT:
        m_data->m_daSynchroOrbit = ! m_data->m_daSynchroOrbit;
        break;
    default:
        break;	// fallback in case something goes wrong
    }
}

void LaunchMFD::OpenDialogTarget()
{
    oapiOpenInputBox(TARGET, DialogTarget::clbk, "", 30, this);
}
void LaunchMFD::OpenDialogAltitude()
{
    if ( m_data->pageView == DIRECT_ASCENT && ! m_data->m_daSynchroOrbit )
        oapiOpenInputBox(ENTER_FINAL_DIST, DialogDistDA::clbk, "", 20, this);
    else
        oapiOpenInputBox(ENTER_ALTITUDE, DialogAlt::clbk, "", 20, this);
}
void LaunchMFD::OpenDialogPIDXY()
{
    DialogPID().PIDTuningXY( this );
}
void LaunchMFD::OpenDialogPIDBank()
{
    DialogPID().PIDTuningBank( this );
}

void LaunchMFD::DoNothing()
{
}

// MFD message parser
int LaunchMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case OAPI_MSG_MFD_OPENED:
        // Our new MFD mode has been selected, so we create the MFD and
        // return a pointer to it.
        return (int) new LaunchMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, plugin);
    }
    return 0;
}

void LaunchMFD::WriteStatus (FILEHANDLE scn) const
{
    oapiWriteScenario_int (scn, "BOOLALT",  m_data->m_ManualAlt);
    oapiWriteScenario_int (scn, "BOOLENG",  m_data->CutEngines );
    oapiWriteScenario_int (scn, "HOV",      m_data->half_ov_reached);
    oapiWriteScenario_int (scn, "BOOLLNC",  m_data->launched);
    oapiWriteScenario_int (scn, "BOOLTGT",  m_data->tgt_set);
    oapiWriteScenario_int (scn, "BOOLHUD",  m_data->hud);
    oapiWriteScenario_int (scn, "BOOLPTC",  m_data->drawPitchError);
    oapiWriteScenario_int (scn, "BOOLCOR",  m_data->m_useOffplaneCorrector);
    oapiWriteScenario_int (scn, "BOOLDAS",  m_data->m_daSynchroOrbit);
    oapiWriteScenario_int (scn, "BOOLLOWINC", m_data->m_isTgtLowInclination);
    oapiWriteScenario_int (scn, "GCCONT",   m_data->m_greatCircle.m_continuous);
    oapiWriteScenario_int (scn, "ALOCK",    m_data->AzimuthLock);
    oapiWriteScenario_int (scn, "PVIEW",    m_data->pageView);
    oapiWriteScenario_float (scn, "INCL",   m_data->GetTgtParam().incl);
    oapiWriteScenario_float (scn, "IFACT",  m_data->InclinationFactor);
    oapiWriteScenario_float (scn, "AFACT",  m_data->AltitudeFactor);
    oapiWriteScenario_float (scn, "PEA",    m_data->PeA);
    oapiWriteScenario_float (scn, "APA",    m_data->ApA);
    oapiWriteScenario_float (scn, "GCSTEP", m_data->m_greatCircle.GetPlotPrecision() * 1000); // Mutiplied due to loss of precission

    std::string tgt = CharManipulations().Replace(m_data->GetTargetStdStr(), " ",">");
    std::string tgtDisp = CharManipulations().Replace(m_data->GetTargetDisplStr(), " ",">");
    oapiWriteScenario_string (scn, "TGT", (char*)tgt.c_str());
    oapiWriteScenario_string (scn, "TGTDISP", (char*)tgtDisp.c_str());
}

void LaunchMFD::ReadStatus (FILEHANDLE scn)
{
    std::string strTgt, strTgtDispl;
    char * line;
    while (oapiReadScenario_nextline (scn, line))
    {
        std::istringstream ss;
        ss.str(line);
        std::string id;
        if ( ss >> id )
        {
            if ( id == "BOOLALT" )      ss >> m_data->m_ManualAlt;
            else if ( id == "BOOLENG" ) ss >> m_data->CutEngines;
            else if ( id == "HOV" )     ss >> m_data->half_ov_reached;
            else if ( id == "BOOLLNC" ) ss >> m_data->launched;
            else if ( id == "BOOLTGT" ) ss >> m_data->tgt_set;
            else if ( id == "BOOLHUD" ) ss >> m_data->hud;
            else if ( id == "BOOLPTC" ) ss >> m_data->drawPitchError;
            else if ( id == "BOOLCOR" ) ss >> m_data->m_useOffplaneCorrector;
            else if ( id == "BOOLDAS" ) ss >> m_data->m_daSynchroOrbit;
            else if ( id == "BOOLLOWINC" ) ss >> m_data->m_isTgtLowInclination;
            else if ( id == "GCCONT" )  ss >> m_data->m_greatCircle.m_continuous;
            else if ( id == "INCL" )    ss >> m_data->GetTgtParam().incl;
            else if ( id == "IFACT" )   ss >> m_data->InclinationFactor;
            else if ( id == "AFACT" )   ss >> m_data->AltitudeFactor;
            else if ( id == "ALOCK" )   ss >> m_data->AzimuthLock;
            else if ( id == "PEA" )     ss >> m_data->PeA;
            else if ( id == "APA" )     ss >> m_data->ApA;
            else if ( id == "GCSTEP" )
            {
                double gcStep;
                if ( ss >> gcStep && gcStep >= 0)
                    m_data->m_greatCircle.SetPlotPrecision(gcStep / 1000.0);
            }
            else if ( id == "PVIEW" )
            {
                int view;
                ss >> view;
                m_data->pageView = (View)view;
            }
            else if ( id == "TGT" )
            {
                std::string tgt;
                ss >> tgt;
                strTgt = CharManipulations().Replace(tgt, ">", " ");
            }
            else if ( id == "TGTDISP" )
            {
                std::string tgt;
                ss >> tgt;
                strTgtDispl = CharManipulations().Replace(tgt, ">", " ");
            }
        }
    }
    if ( ! strTgt.empty() )
    {
        if ( ! strTgtDispl.empty() )
            m_data->SetTargetStr( strTgt, strTgtDispl );
        else
            m_data->SetTargetStr( strTgt, strTgt );
    }
}

//calculate the error's (difference between actual and required azumith) reference
void LaunchMFD::SetError()
{
    GeneralMath gm;
    az = m_data->GetAzimuth();
    az_desc = gm.GetIn0_2PIRange(PI - az);
    heading = m_data->GetMovParams().vesselStatus.surf_hdg;

    if (m_data->GetTgtParam().azFlag == NORTHERN_AZIMUTH)
        error = heading - az;
    else if (m_data->GetTgtParam().azFlag == SOUTHERN_AZIMUTH)
        error = heading - az_desc;
    else // BOTH_AZIMUTHS
    {
        double headingRadPI = gm.GetInPIRange( heading );
        if ( fabs( headingRadPI ) < PI/2 ) // Closer to nothern azimuth
            error = heading - az;
        else                               // Closer to southern
            error = heading - az_desc;
    }
    error = gm.GetInPIRange( error ) * DEG;
}

void LaunchMFD::CheckErrorExcess()
{
    if (fabs(error) > MAX_ERROR)
    {
        if (m_data->CutEngines == false && m_data->launched)
        {
            if( m_beep ) m_sound.PlayWaveOnce(BEEP);
        }
    }
    else m_sound.ResetWaveOnce(BEEP);
}

void LaunchMFD::ReactOnShipStatus()
{
    if ( m_data->half_ov_reached )
        m_sound.PlayWaveOnce(HALF_ORBITAL_VEL);
    if ( ! m_data->launched )
        m_sound.ResetWaveOnce(HALF_ORBITAL_VEL);
}

void LaunchMFD::ReactOnReachingOrbit(MyDC hDC)
{
    if 	(m_data->peg.GetTMECO() <= 0 && m_data->launched )
    {
        MFDTextCalculator c(W, H);
        MFDTextOut(hDC, 0, c.Y(12), RED, CUT_YOUR_ENGINES);
        m_sound.PlayWaveOnce(CUT_ENGINES);
    }
    else
        m_sound.ResetWaveOnce(CUT_ENGINES);
}

void LaunchMFD::SendModuleMessages()
{
    int iTgt = m_data->GetTargetHandleIndex();
    m_mext.Put("TargetObjectIndex", iTgt);
}

void LaunchMFD::DrawErrorAndMarks(MyDC hDC, const int status)
{
    if (m_data->pageView == AZIMUTH || m_data->pageView == DIRECT_ASCENT)
    {
        MFDSetPen( hDC, m_pens.GetPen(Pens::Green) );
        double pitch = pV->GetPitch();
        m_data->SetPEGApses();
        pitchError = (pitch - m_data->GetTargetPitch()) * DEG;
        double bank, pitchErrorLocal;
        if (!m_data->drawPitchError || status == 1)
        {
            pitchErrorLocal = 0;
            bank = 0;
        }
        else
        {
            pitchErrorLocal = pitchError;
            bank = pV->GetBank();
        }
        // SDL's mod ***
        // render the graphical representation
        double factor = 180;
        if (fabs(error) < 90 && fabs(pitchErrorLocal) < 90) factor = 90;
        if (fabs(error) < 45 && fabs(pitchErrorLocal) < 45) factor = 45;
        if (fabs(error) < 5 && fabs(pitchErrorLocal) < 5) factor = 5;
        double rad = sqrt (error * error + pitchErrorLocal * pitchErrorLocal);

        int view_pos = W / 2;
        int view_pos_y = H / 2;

        // we must add bank angle to current angle, created by errors (atan(errors)) and cos of this angle defines the correct position of X in our banked reference frame
        view_pos -= (int)(W * (rad * cos(atan2(pitchErrorLocal, error)-bank))/ (factor * 2));
        view_pos_y += (int)(H * (rad * sin(atan2(pitchErrorLocal, error)-bank))/ (factor * 2));

        // x range mark
        MFDTextOut( hDC, (int)(W * 0.02), (int)(H / 2.1), GREEN, "+%.0lf°", factor );
        MFDLine(hDC, 2,(int)(H / 2.1), 2,(int)(H / 1.9 ));

        MFDTextOut( hDC, (int)(W - 40), (int)(H / 2.1), GREEN, "-%.0lf°", factor );
        MFDLine(hDC, W - 3,(int)(H / 2.1), W - 3,(int)(H / 1.9));

        // y range mark
        MFDTextOut( hDC, (int)(W / 2.2), H - 17, GREEN, "+%.0lf°", factor );
        MFDLine(hDC, (int)(W / 2.1), (int)(H - 3), (int)(W / 1.9) ,(int)(H - 3));

        int linelength = 10;
        // new center mark
        MFDLine(hDC, (W / 2) - linelength, (H / 2) - linelength, (W / 2) + linelength, (H / 2) + linelength);
        MFDLine(hDC, (W / 2) - linelength, (H / 2) + linelength, (W / 2) + linelength, (H / 2) - linelength);
        // position indicator
        MFDLine(hDC, view_pos - linelength, view_pos_y, view_pos + linelength, view_pos_y);
        MFDLine(hDC, view_pos, view_pos_y - linelength, view_pos, view_pos_y + linelength);
        // SDL's mod end ***
    }
}

void LaunchMFD::PrintTargetParamsForAzimuthMode(MyDC hDC)
{
    if ( m_data->pageView != AZIMUTH)
        return;
    COLORREF colour;
    unsigned int line_offset = 1, xoffset = 23;
    const int margin = 1;
    double lat = m_data->GetMovParams().m_lat;
    MFDTextCalculator c(W, H);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, CURRENT_LATITUDE);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.2lf°", lat*DEG);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, SELECTED_TARGET);
    MFDTextOut(hDC, c.X(strlen(SELECTED_TARGET)), c.Y(line_offset++), GREEN, m_data->GetTargetDisplStr());
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, DESIRED_INCLINATION);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.2lf°", m_data->GetTgtParam().incl*DEG);
    const double planeAngle = SpaceMathOrbiter().GetPlaneAngle( m_data->GetVessel(), m_data->GetTargetHandle() );
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, RINC);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.2lf°", planeAngle * DEG);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, PEA);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.1lf km", m_data->PeA / 1000);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, APA);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.1lf km", m_data->ApA / 1000);
    line_offset++;
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, INTERSECTION_TIME);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.1lf s", m_data->GetTgtParam().timeToNode);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset),   YELLOW, NEXT_LAUNCH_TIME);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset++), GREEN, "%.1lf s", m_data->GetShipVariables().m_next_launch_time);
    MFDTextOut(hDC, c.X(margin),    c.Y(line_offset), YELLOW, TIME_TO_MECO);
    if (m_data->GetTMECO() < 0) 	colour = RED;
    else 										colour = GREEN;
    MFDTextOut(hDC, c.X(xoffset), c.Y(line_offset++), colour, "%.1lf s", m_data->GetTMECO());
}

void LaunchMFD::PrintResults(MyDC hDC)
{
    unsigned int i = 0;
    int line_offset = 14; // start point of the below strings
    COLORREF colour;
    const int xoffset = 21;
    const int margin = 1;
    MFDTextCalculator c(W, H);

    MFDTextOut(hDC, c.X(margin),  c.Y(line_offset), YELLOW, HEADING_ERROR);
    if (fabs(error) <= MAX_ERROR) 		colour = GREY;
    else 								colour = GREEN;
    MFDTextOut(hDC, c.X(strlen(HEADING_ERROR)), c.Y(line_offset), colour, "%.2lf°", error);
    MFDTextOut(hDC, c.X(xoffset), c.Y(line_offset), YELLOW, CURRENT);
    MFDTextOut(hDC, c.X(xoffset + strlen(CURRENT)), c.Y(line_offset++), GREEN, "%.2lf°", heading * DEG);
    MFDTextOut(hDC, c.X(margin),  c.Y(line_offset), YELLOW, PITCH_ERROR);
    if (fabs(pitchError) <= MAX_ERROR) 	colour = GREY;
    else 								colour = GREEN;
    MFDTextOut(hDC, c.X(strlen(PITCH_ERROR)), c.Y(line_offset), colour, "%.2lf°", pitchError);
    MFDTextOut(hDC, c.X(xoffset),   c.Y(line_offset), YELLOW, CURRENT);
    MFDTextOut(hDC, c.X(xoffset + strlen(CURRENT)), c.Y(line_offset), GREEN, "%.2lf°", pV->GetPitch() * DEG);

    line_offset += 2;
    MFDTextOut(hDC, c.X(margin), c.Y(line_offset++), YELLOW, REQUIRED_HEADING);
    MFDTextOut(hDC, c.X(margin), c.Y(line_offset), YELLOW, NORTHERN);
    if (m_data->AzimuthLock == NORTHERN_AZIMUTH
            || (m_data->AzimuthLock == BOTH_AZIMUTHS && m_data->GetTgtParam().azFlag == NORTHERN_AZIMUTH))
        colour = GREY;
    else    colour = GREEN;
    MFDTextOut(hDC, c.X(strlen(NORTHERN)),  c.Y(line_offset++), colour, "%.2lf°", az * DEG);
    MFDTextOut(hDC, c.X(margin),            c.Y(line_offset), YELLOW, SOUTHERN);
    if (m_data->AzimuthLock == SOUTHERN_AZIMUTH
            || (m_data->AzimuthLock == BOTH_AZIMUTHS && m_data->GetTgtParam().azFlag == SOUTHERN_AZIMUTH))
        colour = GREY;
    else    colour = GREEN;
    MFDTextOut(hDC, c.X(strlen(SOUTHERN)),  c.Y(line_offset++), colour, "%.2lf°", az_desc * DEG);

    if(m_data->m_useOffplaneCorrector)
    {
        MFDTextOut(hDC, c.X(margin), c.Y(line_offset), YELLOW, CORRECTION);
        if(m_data->GetOffPlaneCorrector()->IsValid())
            MFDTextOut(hDC, c.X(strlen(CORRECTION)), c.Y(line_offset), GREEN, "%.2lf°", m_data->GetOffPlaneCorrector()->GetCorrectionAngle() * DEG)
            else
                MFDTextOut(hDC, c.X(strlen(CORRECTION)), c.Y(line_offset), RED, "---");
    }

    line_offset += 2;
    MFDTextOut(hDC, c.X(margin), c.Y(line_offset), GREY, INCLINATION_ADJ_FACTOR);
    MFDTextOut(hDC, c.X(strlen(INCLINATION_ADJ_FACTOR)), c.Y(line_offset++), GREEN, "%.2lf°", m_data->InclinationFactor);

    std::string environment = m_data->IsInAtmosphere() ? ATMO : SPACE;
    if ( const AutopilotBase * ap = plugin->m_apMan.GetAP( m_data->GetAutopilotType() ) )
        MFDTextOut(hDC, c.X(margin), c.Y(line_offset), GREY, (ap->GetIdentifier() + environment).c_str() );
}

EnjoLib::MFDGoodies::Sound & LaunchMFD::GetSound()
{
    return m_sound;
}

MFDDataLaunchMFD * LaunchMFD::GetData()
{
    return m_data;
}
