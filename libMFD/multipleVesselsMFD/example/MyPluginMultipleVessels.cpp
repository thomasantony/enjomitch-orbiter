// ==============================================================
//          ORBITER AUX LIBRARY: Multiple Vessels Support
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2008      Mohd "Computerex" Ali       - original concept
// Copyright (C) 2011      Szymon "Enjo" Ender         - object oriented design,
//														 simplifications, fixes
//                         All rights reserved
//
// Multiple Vessels Support is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Multiple Vessels Support is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Multiple Vessels Support. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#define STRICT
#define ORBITER_MODULE

#include <orbitersdk.h>

#include "MyPluginMultipleVessels.hpp"
#include "Autopilot/AutopilotBase.hpp"
#include "MyMFDData.hpp"

using namespace EnjoLib::MFDGoodies;

MyPluginMultipleVessels::MyPluginMultipleVessels( HINSTANCE hDLL )
    :
    PluginMultipleVessels( hDLL )
	, m_sound(m_soundMap)
{

}

MyPluginMultipleVessels::~MyPluginMultipleVessels()
{

}

void MyPluginMultipleVessels::InitClient()
{
    m_isFuelLimited = CfgCheckIsFuelLimited();
    m_sound.Reinit();
    m_sound.SetUseSound( CfgCheckUseSound() );
}

void MyPluginMultipleVessels::CleanupClient()
{
}

void MyPluginMultipleVessels::UpdateClientPreStep( MFDData * data, const SimulationTimeData & timeData )
{
    // Normally you'd use dynamic_cast and check if the returned value is NULL,
    // but this method is supposed to work fast enough.
    MyMFDData * myMFDData = static_cast<MyMFDData *> (data);
    AutopilotBase * apBase = m_apMan.GetAP(myMFDData->GetAutopilotType());
    if (apBase != NULL)
    {
        // Only now it's reasonable to call data->Update(). The Guide method calls it
        // Of course, if there were more methods here needing the updated ship data,
        // you'd move the Update() to this scope, and call it only once.
        apBase->Guide(myMFDData);
    }
}

void MyPluginMultipleVessels::UpdateClientPostStep( MFDData * data, const SimulationTimeData & timeData )
{
}

MFDData * MyPluginMultipleVessels::ConstructNewMFDData( VESSEL * vessel )
{
    return new MyMFDData(vessel);
}

Sound & MyPluginMultipleVessels::GetSound()
{
    return m_sound;
}


bool MyPluginMultipleVessels::IsFuelLimited()
{
    return m_isFuelLimited;
}

bool MyPluginMultipleVessels::CfgCheckIsFuelLimited()
{
    bool unlimited = false;
    FILEHANDLE fh = oapiOpenFile  ( "Orbiter.cfg",  FILE_IN );
    if ( fh )
    {
        // If the function fails (no such entry), fuel is limited, so unlimited stays false
        oapiReadItem_bool  ( fh,  "UnlimitedFuel",  unlimited);
        oapiCloseFile( fh, FILE_IN );
    }
    return ! unlimited;
}

bool MyPluginMultipleVessels::CfgCheckUseSound()
{
    bool useSound = false;
    const char * fname = "MFD/MyMFDMultipleVessels.cfg";
    FileAccessMode famode = FILE_IN;
    FILEHANDLE fh = oapiOpenFile  (fname, famode, CONFIG);
    if ( fh )
    {
        bool found = oapiReadItem_bool  ( fh, "USE_SOUND", useSound );
        if ( ! found )
            sprintf_s(oapiDebugString(), 255, "\"USE_SOUND\" not found in Config\\MFD\\MyMFDMultipleVessels.cfg or uses wrong syntax");

        oapiCloseFile(fh, famode);
    }
    else
    {
        sprintf_s(oapiDebugString(), 255, "My MFD's config file - Config\\MFD\\MyMFDMultipleVessels.cfg not found!");
    }
    return useSound;
}

MyMFDButtonsPage & MyPluginMultipleVessels::GetButtons()
{
    return m_buttons;
}

