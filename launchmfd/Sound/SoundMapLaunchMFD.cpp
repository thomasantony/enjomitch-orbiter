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

#include "SoundMapLaunchMFD.hpp"
#include "SoundSampleIDEnum.hpp"

SoundMapLaunchMFD::SoundMapLaunchMFD()
{
    const std::string dir = "Sound/LaunchMFDEnjo/";
    AddSample( HALF_ORBITAL_VEL , 			dir + "HalfOV.wav");
    AddSample( TARGET_MANUAL , 				dir + "ManualTarget.wav");
    AddSample( TARGET_SELECTED , 			dir + "TargetSelected.wav");
    AddSample( CUT_ENGINES , 				dir + "CutEngines.wav");
    AddSample( ALTITUDE_SET , 				dir + "alt_set.wav");
    AddSample( ALTITUDE_AUTO , 				dir + "AltitudeAutomatic.wav");
    AddSample( HUD_ENABLED , 				dir + "hud_en.wav");
    AddSample( HUD_DISABLED , 				dir + "hud_dis.wav");
    AddSample( BEEP_ENABLED , 				dir + "beep_en.wav");
    AddSample( VOICE_ENABLED , 				dir + "voice_en.wav");
    AddSample( VOICE_DISABLED , 			dir + "voice_dis.wav");
    AddSample( BEEP , 						dir + "beep_am_light22.wav");
    AddSample( BEEP_DA , 					dir + "da_available.wav");
    AddSample( TARGET_INCL_LOWER_THAN_LAT , dir + "TgtInclLessThanLat.wav");
}
