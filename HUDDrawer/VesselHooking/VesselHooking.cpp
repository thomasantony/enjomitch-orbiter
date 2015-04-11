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

#include "VesselHooking.hpp"
#include <algorithm>

using namespace EnjoLib;
using namespace std;

// To upgrade the library at some point, when VESSEL4, or VESSEL5 is created,
// count the number of virtual functions of the VESSELx,
// note the index of HUD drawing method,
// and update the following two variables:
#ifdef ORB2006
    #define VTABLE_SIZE		28
    #define DRAW_HUD_INDEX	11
#endif
#ifdef ORB2009
    #define VTABLE_SIZE		35
    #define DRAW_HUD_INDEX	32
#endif
#ifdef ORB2015
    #define VTABLE_SIZE		36
    #define DRAW_HUD_INDEX	32
#endif
VesselHooking::VesselHooking(OBJHANDLE hVessel, int fmodel)
    :
#ifdef ORB2006
    VESSEL2(hVessel, fmodel)
#endif
#ifdef ORB2009
    VESSEL3(hVessel, fmodel)
#endif
#ifdef ORB2015
    VESSEL4(hVessel, fmodel)
#endif
{
}

VesselHooking::~VesselHooking()
{}

VesselHooking::VEC_HUD_DRAWERS VesselHooking::vecHUDrawers;
std::vector<string> VesselHooking::moduleBlackList;

bool VesselHooking::IsVesselCompatible( VESSEL * v )
{
    #ifdef ORB2006
         return v->Version() == 1;
    #endif
    #ifdef ORB2009
         return v->Version() == 2;
    #endif
    #ifdef ORB2015
         return v->Version() == 3;
    #endif
}

bool VesselHooking::CanDrawHUD( IDrawsHUD * mfd )
{
    bool isOnBlacklist = find(moduleBlackList.begin(), moduleBlackList.end(), mfd->GetModuleName()) != moduleBlackList.end();
    return ! isOnBlacklist && mfd->ShouldDrawHUD();
}

// Generate a dummy class so that we can bounce around its vtables
static VesselHooking dummy((OBJHANDLE)6969, 0);

void VesselHooking::FocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
    // Should be called whenever orbiter changes the focus vessel
    // Unhook the clbkDrawHUD method for the old focus vessel (if it existed) and
    // hook the new vessel
    TryUnHook(old_focus);
    TryHook(new_focus);
}

//////////////////////////////////////////////////////////
// Callbacks that will be hooked to the vessel class
void VesselHooking::clbkDrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
    // Invoke the old HUD method - this will have been hooked and so
    // will call the original vessel->clbkDrawHUD method
    UnHook();
    this->clbkDrawHUD(mode, hps, hDC);
    Hook();
    for (VEC_HUD_DRAWERS::const_iterator it = vecHUDrawers.begin(); it != vecHUDrawers.end(); ++it)
        if (CanDrawHUD(*it))	// Draw the new HUD
            (*it)->DrawHUD(mode, hps, hDC);
}

void VesselHooking::OldclbkDrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC)
{}

#if defined(ORB2009) || defined(ORB2015)
bool VesselHooking::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp )
{
    // Invoke the old HUD method - this will have been hooked and so
    // will call the original vessel->clbkDrawHUD method
    UnHook();
    this->clbkDrawHUD(mode, hps, skp);
    Hook();
    for (VEC_HUD_DRAWERS::const_iterator it = vecHUDrawers.begin(); it != vecHUDrawers.end(); ++it)
        if (CanDrawHUD(*it))	// Draw the new HUD
            (*it)->DrawHUD(mode, hps, skp);

    return true;
}

bool VesselHooking::OldclbkDrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp)
{return true;}
#endif

/////////////////////////////////////////////////////////////
// Hooking functions

void VesselHooking::TryHook( OBJHANDLE new_focus )
{
    VESSEL * v = oapiGetVesselInterface(new_focus);
    if ( ! IsVesselCompatible( v ) )
        return;

    ((VesselHooking*)v)->Hook();
}

void VesselHooking::TryUnHook( OBJHANDLE old_focus )
{
    if ( ! old_focus )
        return;

    VESSEL * v = oapiGetVesselInterface(old_focus);
    if ( ! IsVesselCompatible( v ) )
        return;

    ((VesselHooking*)v)->UnHook();
}

void VesselHooking::Hook()
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

void VesselHooking::UnHook()
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

void VesselHooking::OnMFDConstruction( IDrawsHUD * mfd )
{
    vecHUDrawers.push_back( mfd );
}

void VesselHooking::OnMFDDestruction( IDrawsHUD * mfd )
{
    VEC_HUD_DRAWERS::iterator it = find(vecHUDrawers.begin(), vecHUDrawers.end(), mfd);
    if ( it != vecHUDrawers.end())
        vecHUDrawers.erase(it);
    // else - shouldn't happen
}

vector<VesselHooking::Drawer> VesselHooking::GetDrawersList()
{
    vector<VesselHooking::Drawer> moduleList;
    vector<string> moduleIDList;
    // Add to the module list only non-empty string identifiers of the currently registered modules
    for (VEC_HUD_DRAWERS::const_iterator it = vecHUDrawers.begin(); it != vecHUDrawers.end(); ++it)
    {
        const string & id = (*it)->GetModuleName();
        if ( id.empty() )
            continue;
        moduleIDList.push_back(id);
    }
    //sort(moduleBlackList.begin(), moduleBlackList.end()); // should be already sorted
    sort(moduleIDList.begin(), moduleIDList.end()); // sort
    // Remove duplicates
    moduleIDList.resize( std::distance(moduleIDList.begin(), unique(moduleIDList.begin(), moduleIDList.end())) );
    // Create a list of all of the modules, adding info, if it was blacklisted.
    for (vector<string>::const_iterator currentID = moduleIDList.begin(); currentID != moduleIDList.end(); ++currentID)
    {
        vector<string>::const_iterator itBlackList = find(moduleBlackList.begin(), moduleBlackList.end(), *currentID);
        bool isActive = itBlackList == moduleBlackList.end(); // active if not found on blacklist
        moduleList.push_back(VesselHooking::Drawer(*currentID, isActive));
    }
    return moduleList;
}

void VesselHooking::UpdateDrawer( const VesselHooking::Drawer & drawer )
{
    vector<string>::iterator itBlackList = find(moduleBlackList.begin(), moduleBlackList.end(), drawer.id);
    if (itBlackList != moduleBlackList.end()) // found this ID on black list
    {
        if ( drawer.isActive ) // Asked to remove from blacklist
            moduleBlackList.erase( itBlackList );
    }
    else  // Drawer not found on black list. Perhaps it should be added?
    {
        if ( ! drawer.isActive ) // Asked to add to black list
           moduleBlackList.push_back( drawer.id );
    }
}

void VesselHooking::EnableAllDrawers()
{
    moduleBlackList.clear();
}

void VesselHooking::DisableAllDrawers()
{
    moduleBlackList.clear();
    const vector<VesselHooking::Drawer> & drawers = GetDrawersList();
    for( vector<VesselHooking::Drawer>::const_iterator it = drawers.begin(); it != drawers.end(); ++it )
        moduleBlackList.push_back(it->id);
}

/*
void VesselHooking::SetDrawersList( const std::vector<VesselHooking::Drawer> & modules )
{
    vector<string> tmpBlackList;
    // Iterate the user's choice list
    for (vector<VesselHooking::Drawer>::const_iterator module = modules.begin(); module != modules.end(); ++module)
    {
        vector<string>::iterator itBlackList = find(moduleBlackList.begin(), moduleBlackList.end(), module->id);
        if (itBlackList != moduleBlackList.end()) // found this ID on black list
        {
            if ( module->active ) // Asked to remove from blacklist
                moduleBlackList.erase(itBlackList);
        }
        else // Drawer not found on black list. Perhaps it should be added?
        {
            if ( ! module->active ) // Asked to add to black list
               tmpBlackList.push_back( module->id );
        }

            moduleBlackList.push_back(module->id);
    }
    copy(tmpBlackList.begin(), tmpBlackList.end(), moduleBlackList.end());
    sort(moduleBlackList.begin(), moduleBlackList.end());
}
*/

