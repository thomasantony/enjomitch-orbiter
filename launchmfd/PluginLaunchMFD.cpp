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
// Authors - Szymon "Enjo" Ender
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

#include "PluginLaunchMFD.hpp"
#include "Autopilot/AutopilotBase.hpp"
#include <MFDSound++/Sound.hpp>
#include "MFDDataLaunchMFD.hpp"
#include "Utils/FileUtilsOrbiter.hpp"

using namespace EnjoLib::MFDGoodies;

PluginLaunchMFD * plugin;
static int g_MFDmode; // identifier for new MFD mode
// ==============================================================
// API interface

// Called on module init
DLLCLBK void InitModule (HINSTANCE hDLL)
{
    static char *name = "LaunchMFD";   // MFD mode name
#ifdef ORB2006
    MFDMODESPEC spec;
#else
    MFDMODESPECEX spec;
#endif
    // Set MFDMODESPEC's fields
    spec.name = name;
    spec.key = OAPI_KEY_T;                // MFD mode selection key
    spec.msgproc = LaunchMFD::MsgProc;  // MFD mode callback function
#ifndef ORB2006
    spec.context = NULL;
#endif
    // Register the new MFD mode with Orbiter
    g_MFDmode = oapiRegisterMFDMode (spec);

    plugin = new PluginLaunchMFD( hDLL );
#ifndef ORB2006
    oapiRegisterModule (plugin);
#endif
}

// Called on module exit
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
    // Unregister the custom MFD mode when the module is unloaded
    oapiUnregisterMFDMode (g_MFDmode);
#ifdef ORB2006
    // Orbiter 2009 deletes this automatically, because it was registered
    delete plugin;
#endif
}

#ifdef ORB2006
// Called every timestep
DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd)
{
    plugin->UpdatePreStep( SimulationTimeData(SimT, SimDT, mjd) ); // This sould be called on each step
}

// Called on vessel deletion
DLLCLBK void opcDeleteVessel( OBJHANDLE  hVessel )
{
    // Signal the plugin that this vessel should be removed from it's structure
    plugin->DeleteVessel( hVessel );
}

DLLCLBK void opcOpenRenderViewport  ( HWND  hRenderWnd, DWORD  width, DWORD  height, BOOL  fullscreen)
{
    plugin->Init();
}

DLLCLBK void opcCloseRenderViewport()
{
    plugin->Cleanup();
}
#endif

PluginLaunchMFD::PluginLaunchMFD( HINSTANCE hDLL )
: PluginMultipleVessels( hDLL )
, m_sound( m_soundMap )
{
}

PluginLaunchMFD::~PluginLaunchMFD()
{
}

void PluginLaunchMFD::InitClient()
{
    m_isFuelLimited = CfgCheckIsFuelLimited();
    m_sound.Reinit();
    m_sound.SetUseSound( CfgCkeckVariable("USE_SOUND") );
}

void PluginLaunchMFD::CleanupClient()
{
}

MFDData * PluginLaunchMFD::ConstructNewMFDData( VESSEL * vessel )
{
    return new MFDDataLaunchMFD( vessel );
}

Sound & PluginLaunchMFD::GetSound()
{
    return m_sound;
}

const MFDButtonPageLaunchMFD & PluginLaunchMFD::GetButtonPages() const
{
    return m_buttonPages;
}

void PluginLaunchMFD::UpdateClientPreStep( MFDData * data, const SimulationTimeData & timeData )
{
    MFDDataLaunchMFD * dataLaunchMFD = static_cast<MFDDataLaunchMFD *>( data );
    const AutopilotBase * apBase = m_apMan.GetAP(dataLaunchMFD->GetAutopilotType());
    if (apBase != NULL)
    {
        // Only now it's reasonable to call data->Update(). The Guide method calls it
		apBase->Guide(dataLaunchMFD, timeData.GetSimDT() );
    }
}

void PluginLaunchMFD::UpdateClientPostStep( MFDData * data, const SimulationTimeData & timeData )
{
}

void PluginLaunchMFD::clbkFocusChanged (OBJHANDLE new_focus, OBJHANDLE old_focus)
{
    //EnjoLib::MFDGoodies::MFDHUDDrawer::FocusChanged(new_focus, old_focus);
}


bool PluginLaunchMFD::IsFuelLimited()
{
    return m_isFuelLimited;
}

bool PluginLaunchMFD::CfgCheckIsFuelLimited()
{
    bool unlimited = false;
    FILEHANDLE fh = oapiOpenFile  ( "Orbiter.cfg",  FILE_IN );
    if ( fh )
    {
        // if found, it's usually true. If not found then it's false;
        if ( ! oapiReadItem_bool  ( fh,  "UnlimitedFuel",  unlimited) )
            unlimited = false;
        oapiCloseFile( fh, FILE_IN );
    }
    return ! unlimited;
}

bool PluginLaunchMFD::CfgCkeckVariable( const std::string & varName )
{
    bool varValue = false;
    std::string fname = FileUtilsOrbiter().GetMFDConfigFile();
    FileAccessMode famode = FILE_IN;
    FILEHANDLE fh = oapiOpenFile  (fname.c_str(), famode);
    if ( fh )
    {
        bool found = oapiReadItem_bool  ( fh, (char*)varName.c_str(), varValue );
        if ( ! found )
            sprintf_s(oapiDebugString(), 255, (varName + " not found in " + fname + " or uses wrong syntax").c_str());

        oapiCloseFile(fh, famode);
    }
    else
        sprintf_s(oapiDebugString(), 255, (std::string("Launch MFD's config file - ") + fname + " not found!").c_str());

    return varValue;
}

