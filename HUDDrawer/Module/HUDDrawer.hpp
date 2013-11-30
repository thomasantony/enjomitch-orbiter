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

#ifndef VESSEL_HOOKING_MODULE_HPP
#define VESSEL_HOOKING_MODULE_HPP

#include <OrbiterSDK.h>

/// Module that performs a call for VESSEL virtual table hijacking.
/**
    This module is the only place where the hijacking should be performed. Otherwise CTDs on vessel switching will occur.
    For other modules to receive a HUD drawing context, this module has to be activated in Orbiter's Modules tab.
*/
class HUDDrawer
#ifndef ORB2006
 : public oapi::Module
#endif
{
    public:
        HUDDrawer( HINSTANCE hDLL );
        ~HUDDrawer();
        void clbkFocusChanged (OBJHANDLE new_focus, OBJHANDLE old_focus);

    protected:

    private:
};




#endif // VESSEL_HOOKING_MODULE_HPP
