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
    this->operator[]( HALF_ORBITAL_VEL ) =				"Sound/LaunchMFDEnjo/HalfOV.wav";
    this->operator[]( TARGET_MANUAL ) =					"Sound/LaunchMFDEnjo/ManualTarget.wav";
    this->operator[]( TARGET_SELECTED ) =				"Sound/LaunchMFDEnjo/TargetSelected.wav";
    this->operator[]( CUT_ENGINES ) =					"Sound/LaunchMFDEnjo/CutEngines.wav";
    this->operator[]( ALTITUDE_SET ) =					"Sound/LaunchMFDEnjo/alt_set.wav";
    this->operator[]( ALTITUDE_AUTO ) =					"Sound/LaunchMFDEnjo/AltitudeAutomatic.wav";
    this->operator[]( HUD_ENABLED ) =					"Sound/LaunchMFDEnjo/hud_en.wav";
    this->operator[]( HUD_DISABLED ) =					"Sound/LaunchMFDEnjo/hud_dis.wav";
    this->operator[]( BEEP_ENABLED ) =					"Sound/LaunchMFDEnjo/beep_en.wav";
    this->operator[]( VOICE_ENABLED ) =					"Sound/LaunchMFDEnjo/voice_en.wav";
    this->operator[]( VOICE_DISABLED ) =				"Sound/LaunchMFDEnjo/voice_dis.wav";
    this->operator[]( BEEP ) =							"Sound/LaunchMFDEnjo/beep_am_light22.wav";
    this->operator[]( BEEP_DA ) =						"Sound/LaunchMFDEnjo/da_available.wav";
    this->operator[]( TARGET_INCL_LOWER_THAN_LAT ) =	"Sound/LaunchMFDEnjo/TgtInclLessThanLat.wav";
}
