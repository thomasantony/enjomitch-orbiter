// ==============================================================
//                ORBITER AUX LIBRARY: HUDDrawer
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Allows modules (MFDs or plain modules) to draw on HUD.
// WARNING! This library is a HACK and it may stop working
// in future versions of Orbiter API if the library isn't
// maintained anymore. Note however that it's very easy to do so,
// by updating the VesselHooking class
//
// Copyright (C) 2008 Steve "agentgonzo" Arch - VESSEL virtual table hijacking
// Copyright (C) 2012-2013 Szymon "Enjo" Ender - created a framework
//
//                         All rights reserved
//
// HUDDrawer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// HUDDrawer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HUDDrawer.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#include "MFDButtonPageHUDDrawer.h"

MFDButtonPageHUDDrawer::MFDButtonPageHUDDrawer()
{
    static const MFDBUTTONMENU mnu1[] =
    {
        {"Prev item", 0, '-'},
        {"Next item", 0, '+'},
        {"Modify", 0, 'M'},
        {"Enable all", 0, 'E'},
        {"Disable all", 0, 'D'},
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PRV", OAPI_KEY_MINUS,    &HUDDrawerMFD::OnPrevItem);
    RegisterFunction("NXT", OAPI_KEY_EQUALS,     &HUDDrawerMFD::OnNextItem);
    RegisterFunction("MOD", OAPI_KEY_M,         &HUDDrawerMFD::OnModItem);
    RegisterFunction("EA",  OAPI_KEY_E,         &HUDDrawerMFD::OnEnableAll);
    RegisterFunction("DA",  OAPI_KEY_D,         &HUDDrawerMFD::OnDisableAll);
}

MFDButtonPageHUDDrawer::~MFDButtonPageHUDDrawer()
{
}

 bool MFDButtonPageHUDDrawer::SearchForKeysInOtherPages() const
 {
     return true;
 }
