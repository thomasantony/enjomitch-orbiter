// ==============================================================
// ORBITER AUX LIBRARY: Support for HUD drawing from MFD context
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Warning! This library is a HACK and it may stop working
// in future versions of Orbiter API if the library isn't
// maintained anymore. Note however that it's very easy to do so.
//
// Copyright (C) 2008 Steve "agentgonzo" Arch - most of work
// Copyright (C) 2012 Szymon "Enjo" Ender - created a framework
//
//                         All rights reserved
//
// MFDHUDDrawer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// MFDHUDDrawer is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with MFDHUDDrawer. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#include "MFDHUDDrawer.hpp"

using namespace EnjoLib::MFDGoodies;

IMFDDrawsHUD * MFDHUDDrawer::mfdDrawsHUD = NULL;

// Count the number virtual functions
#ifdef ORB2006
    #define VTABLE_SIZE		28
    #define DRAW_HUD_INDEX	11
#else
    #define VTABLE_SIZE		35
    #define DRAW_HUD_INDEX	32
#endif
MFDHUDDrawer::MFDHUDDrawer(OBJHANDLE hVessel, int fmodel)
    :
#ifdef ORB2006
    VESSEL2(hVessel, fmodel)
#else
    VESSEL3(hVessel, fmodel)
#endif
{
}

MFDHUDDrawer::~MFDHUDDrawer()
{}

bool MFDHUDDrawer::IsVesselCompatible( VESSEL * v )
{
    #ifdef ORB2006
        return v->Version() == 1;
    #else
        return v->Version() == 2;
    #endif // ORB2006
}

// Generate a dummy class so that we can bounce around its vtables
static MFDHUDDrawer dummy((OBJHANDLE)6969, 0);

void MFDHUDDrawer::FocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
    // Should be called whenever orbiter changes the focus vessel
    // Unhook the clbkDrawHUD method for the old focus vessel (if it existed) and
    // hook the new vessel
    TryUnHook(old_focus);
    TryHook(new_focus);
}

//////////////////////////////////////////////////////////
// Callbacks that will be hooked to the vessel class

void MFDHUDDrawer::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
    // Invoke the old HUD method - this will have been hooked and so
    // will call the original vessel->clbkDrawHUD method
    UnHook();
    this->clbkDrawHUD(mode, hps, hDC);
    Hook();
    if (mfdDrawsHUD)
        if (mfdDrawsHUD->ShouldDrawHUD())	// Draw the new HUD
            mfdDrawsHUD->DrawHUD(mode, hps, hDC);
}

void MFDHUDDrawer::OldclbkDrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC)
{}

#ifndef ORB2006
bool MFDHUDDrawer::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp )
{
    // Invoke the old HUD method - this will have been hooked and so
    // will call the original vessel->clbkDrawHUD method
    UnHook();
    this->clbkDrawHUD(mode, hps, skp);
    Hook();
    if (mfdDrawsHUD)
        if (mfdDrawsHUD->ShouldDrawHUD())	// Draw the new HUD
            mfdDrawsHUD->DrawHUD(mode, hps, skp);

    return true;
}

bool MFDHUDDrawer::OldclbkDrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp)
{return true;}
#endif

/////////////////////////////////////////////////////////////
// Hooking functions

void MFDHUDDrawer::TryHook( OBJHANDLE new_focus )
{
    VESSEL * v = oapiGetVesselInterface(new_focus);
    if ( ! IsVesselCompatible( v ) )
        return;

    ((MFDHUDDrawer*)v)->Hook();
}

void MFDHUDDrawer::TryUnHook( OBJHANDLE old_focus )
{
    if ( ! old_focus )
        return;

    VESSEL * v = oapiGetVesselInterface(old_focus);
    if ( ! IsVesselCompatible( v ) )
        return;

    ((MFDHUDDrawer*)v)->UnHook();
}

void MFDHUDDrawer::Hook()
{
    int *thisVTable = *(int**)&dummy;
    int *vesselVTable = *(int**)this;

    // hook OldclbkDrawHUD to vessel->clbkDrawHUD
    DWORD oldthisProtect, oldVesselProtect;
    // Unprotect memory
    BOOL thisProtect = VirtualProtect(thisVTable, (VTABLE_SIZE * 2 + 1) * sizeof(int*), PAGE_EXECUTE_WRITECOPY, &oldthisProtect);
    BOOL vesselProtect = VirtualProtect(vesselVTable, (VTABLE_SIZE + 1) * sizeof(int*), PAGE_EXECUTE_WRITECOPY, &oldVesselProtect);
    if(thisProtect && vesselProtect)
        if(thisVTable[DRAW_HUD_INDEX])	// Is not a pure virtual method (ie, has been implemented)
        {
            thisVTable[VTABLE_SIZE + DRAW_HUD_INDEX] = vesselVTable[DRAW_HUD_INDEX];	// Hook the old function to be the vessel function
            vesselVTable[DRAW_HUD_INDEX] = thisVTable[DRAW_HUD_INDEX]; // Hook the vessel function to be the user implemented function
        }

    // re-protect memory
    if(thisProtect)
        VirtualProtect(thisVTable, (VTABLE_SIZE + 1) * sizeof(int*), oldthisProtect, &oldthisProtect);
    if(vesselProtect)
        VirtualProtect(vesselVTable, (VTABLE_SIZE + 1) * sizeof(int*), oldVesselProtect, &oldVesselProtect);
}

void MFDHUDDrawer::UnHook()
{
    int *thisVTable = *(int**)&dummy;
    int *vesselVTable = *(int**)this;

    // reset the hook
    DWORD oldProtect;
    if(VirtualProtect(vesselVTable, (VTABLE_SIZE + 1) * sizeof(int*), PAGE_EXECUTE_WRITECOPY, &oldProtect))	// Unprotect memory
    {
        if(thisVTable[DRAW_HUD_INDEX])	// is not a virtual function
            vesselVTable[DRAW_HUD_INDEX] = thisVTable[VTABLE_SIZE + DRAW_HUD_INDEX];	// Perform unhook (set to old value)
        VirtualProtect(vesselVTable, (VTABLE_SIZE + 1) * sizeof(int*), oldProtect, &oldProtect);	// re-protect memory
    }
}

void MFDHUDDrawer::OnMFDConstruction( IMFDDrawsHUD * mfd )
{
    MFDHUDDrawer::mfdDrawsHUD = mfd;
}

void MFDHUDDrawer::OnMFDDestruction()
{
    MFDHUDDrawer::mfdDrawsHUD = NULL;
}
