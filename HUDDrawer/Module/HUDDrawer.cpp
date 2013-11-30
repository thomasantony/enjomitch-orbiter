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

#define STRICT
#define ORBITER_MODULE

#include "HUDDrawer.hpp"
#include "HUDDrawerMFD.h"
#include "../VesselHooking/VesselHooking.hpp"

HUDDrawer * module = NULL;
int g_MFDmode; // identifier for new MFD mode

// Called on module init
DLLCLBK void InitModule (HINSTANCE hDLL)
{
    static char *name = "HUDDrawer MFD";   // MFD mode name
    MFDMODESPEC spec;
    // Set MFDMODESPEC's fields
    spec.name = name;
    spec.key = OAPI_KEY_U;                // MFD mode selection key
    spec.msgproc = HUDDrawerMFD::MsgProc;  // MFD mode callback function

    // Register the new MFD mode with Orbiter
    g_MFDmode = oapiRegisterMFDMode (spec);

    module = new HUDDrawer( hDLL );
#ifndef ORB2006
    oapiRegisterModule (module);
#endif
}

// Called on module exit
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
    oapiUnregisterMFDMode (g_MFDmode);
    // Unregister the custom MFD mode when the module is unloaded
#ifdef ORB2006
    // Orbiter 2009 deletes this automatically, because it was registered
    delete module;
#endif
}

HUDDrawer::HUDDrawer( HINSTANCE hDLL )
#ifndef ORB2006
: oapi::Module( hDLL )
#endif
{}

HUDDrawer::~HUDDrawer()
{}

void HUDDrawer::clbkFocusChanged (OBJHANDLE new_focus, OBJHANDLE old_focus)
{
    EnjoLib::VesselHooking::FocusChanged(new_focus, old_focus);
}

#ifdef ORB2006
DLLCLBK void opcFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
	EnjoLib::VesselHooking::FocusChanged(new_focus, old_focus);
}
#endif
