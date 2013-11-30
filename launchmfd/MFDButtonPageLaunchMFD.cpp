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
// MFDButtonPageLaunchMFD - implements MFDButtonPage
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

#include "MFDButtonPageLaunchMFD.hpp"
#include "localisation.h"

MFDButtonPageLaunchMFD::MFDButtonPageLaunchMFD()
{
    // The menu descriptions of all buttons
    static const MFDBUTTONMENU mnu1[] =
    {
        {SELECT_TARGET, 0, 'T'},
        {ENTER_ALT, "km", 'A'},
        {INCREASE_INCLINATION, 0, '+'},
        {DECREASE_INCLINATION, 0, '-'},
        {INCREASE_INCLINATION_FACTOR, 0, ']'},
        {DECREASE_INCLINATION_FACTOR, 0, '['},

        {SWITCH_BUTTONS_PAGE, 0, 'B'},
        {OPERATION_MODE, 0, 'M'},
        {DEFAULT_ACTION, 0, 'D'},
        {SWITCH_PITCH_GUIDANCE, 0, 'I'},
        {OFF_PLANE_CORRECTION, 0, 'O'},
        {AUTOPILOT, 0, 'P'}
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("TGT", OAPI_KEY_T, &LaunchMFD::OpenDialogTarget);
    RegisterFunction("ALT", OAPI_KEY_A, &LaunchMFD::OpenDialogAltitude);
    RegisterFunctionCont("I +", OAPI_KEY_EQUALS,    &LaunchMFD::IncreaseInclination,        &LaunchMFD::DecreaseInclination);
    RegisterFunctionCont("I -", OAPI_KEY_MINUS,     &LaunchMFD::DecreaseInclination,        &LaunchMFD::IncreaseInclination);
    RegisterFunction("Ad+", OAPI_KEY_RBRACKET,  &LaunchMFD::IncreaseInclinationFactor,  &LaunchMFD::DecreaseInclinationFactor);
    RegisterFunction("Ad-", OAPI_KEY_LBRACKET,  &LaunchMFD::DecreaseInclinationFactor,  &LaunchMFD::IncreaseInclinationFactor);

    RegisterFunction("PG",  OAPI_KEY_B, &LaunchMFD::SwitchButtonsPage);
    RegisterFunction("MOD", OAPI_KEY_M, &LaunchMFD::SwitchMode);
    RegisterFunction("DEF", OAPI_KEY_D, &LaunchMFD::DefaultAction);
    RegisterFunction("PTC", OAPI_KEY_I, &LaunchMFD::SwitchPitchGuidance);
    RegisterFunction("COR", OAPI_KEY_O, &LaunchMFD::SwitchOffplaneCorrection);
    RegisterFunction("AP",  OAPI_KEY_P, &LaunchMFD::SwitchAutopilot);

    static const MFDBUTTONMENU mnu2[] =
    {
        {GREAT_CIRCLE_SWITCH, GREAT_CIRCLE, 'G'},
        {GREAT_CIRCLE_TRACK, 0, 'K'},
        {GREAT_CIRCLE_ZOOM_IN, 0, 'Z'},
        {GREAT_CIRCLE_ZOOM_OUT, 0, 'X'},
        {GREAT_CIRCLE_PREC_INCR, 0, 'C'},
        {GREAT_CIRCLE_PREC_DECR, 0, 'V'},

        {SWITCH_BUTTONS_PAGE, 0, 'B'},
        {SWITCH_AZIMUTH, MANUAL_TGT_ONLY, 'U'},
        {SWITCH_HUD, 0, 'H'},
        {SWITCH_SOUNDS, 0, 'S'},
        {GREAT_CIRCLE_LINES, GREAT_CIRCLE, 'L'},
    };
    RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

    RegisterFunction("GC",  OAPI_KEY_G, &LaunchMFD::SwitchGreatCircleUse);
    RegisterFunction("TRK", OAPI_KEY_K, &LaunchMFD::SwitchGreatCircleTrack);
    RegisterFunctionCont("ZM+", OAPI_KEY_Z, &LaunchMFD::GreatCircleZoomIn);
    RegisterFunctionCont("ZM-", OAPI_KEY_X, &LaunchMFD::GreatCircleZoomOut);
    RegisterFunction("PR+", OAPI_KEY_C, &LaunchMFD::GreatCircleIncreasePlotPrecision, &LaunchMFD::GreatCircleDecreasePlotPrecision);
    RegisterFunction("PR-", OAPI_KEY_V, &LaunchMFD::GreatCircleDecreasePlotPrecision, &LaunchMFD::GreatCircleIncreasePlotPrecision);
    RegisterFunction("PG",  OAPI_KEY_B, &LaunchMFD::SwitchButtonsPage);
    RegisterFunction("AZI", OAPI_KEY_U, &LaunchMFD::SwitchAzimuth);
    RegisterFunction("HUD", OAPI_KEY_H, &LaunchMFD::SwitchHUD);
    RegisterFunction("SND", OAPI_KEY_S, &LaunchMFD::SwitchSound);
    RegisterFunction("GCL", OAPI_KEY_L, &LaunchMFD::SwitchGreatCircleLines);

    static const MFDBUTTONMENU mnu3[] =
    {
        {PID_ADJUST_XY, 0, '1'},
        {PID_ADJUST_BANK, 0, '2'},
        {0, 0, ' '},
        {0, 0, ' '},
        {0, 0, ' '},
        {0, 0, ' '},

        {SWITCH_BUTTONS_PAGE, 0, 'B'},
    };
    RegisterPage(mnu3, sizeof(mnu3) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PXY", OAPI_KEY_1, &LaunchMFD::OpenDialogPIDXY);
    RegisterFunction("PBN", OAPI_KEY_2, &LaunchMFD::OpenDialogPIDBank);
    RegisterFunction("   ", OAPI_KEY_5, &LaunchMFD::DoNothing);
    RegisterFunction("   ", OAPI_KEY_5, &LaunchMFD::DoNothing);
    RegisterFunction("   ", OAPI_KEY_5, &LaunchMFD::DoNothing);
    RegisterFunction("   ", OAPI_KEY_5, &LaunchMFD::DoNothing);

    RegisterFunction("PG",  OAPI_KEY_B, &LaunchMFD::SwitchButtonsPage);
}

bool MFDButtonPageLaunchMFD::SearchForKeysInOtherPages() const
{
    return true;
}
