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

#ifndef PLUGINMyMFDMultipleVessels_HPP_INCLUDED
#define PLUGINMyMFDMultipleVessels_HPP_INCLUDED

#include "../PluginMultipleVessels.hpp"
#include "Autopilot/AutopilotManager.hpp"
#include "../../MFDSound++/Sound.hpp"
#include "Sound/SoundMapMyMFD.hpp"
#include "MyMFDButtonsPage.hpp"

class MyPluginMultipleVessels : public EnjoLib::MFDGoodies::PluginMultipleVessels
{
public:
    MyPluginMultipleVessels( HINSTANCE hDLL );
    ~MyPluginMultipleVessels();
    bool IsFuelLimited();
    EnjoLib::MFDGoodies::Sound & GetSound();
	MyMFDButtonsPage & GetButtons();


protected:
    void UpdateClientPreStep( EnjoLib::MFDGoodies::MFDData * data, const EnjoLib::MFDGoodies::SimulationTimeData & timeData );
	void UpdateClientPostStep( EnjoLib::MFDGoodies::MFDData * data, const EnjoLib::MFDGoodies::SimulationTimeData & timeData );
    void InitClient();
    void CleanupClient();
    EnjoLib::MFDGoodies::MFDData * ConstructNewMFDData( VESSEL * vessel );

private:
    bool CfgCheckIsFuelLimited();
    bool CfgCheckUseSound();

    bool m_isFuelLimited;
    AutopilotManager m_apMan;
    EnjoLib::MFDGoodies::Sound m_sound;
	SoundMapMyMFD m_soundMap;
	MyMFDButtonsPage m_buttons;
};

#endif // PLUGINMyMFDMultipleVessels_HPP_INCLUDED
