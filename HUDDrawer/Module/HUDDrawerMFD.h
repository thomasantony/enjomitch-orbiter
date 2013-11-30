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

#ifndef HUDDRAWERMFD_H
#define HUDDRAWERMFD_H

#include <OrbiterSDK.h>
#include "globals.h"
#include <MyDC.h>

class HUDDrawerMFD : public MY_MFD
{
public:
    /// Default constructor
    /**
    Constructs the MFD.
    \param w - MFD's width
    \param h - MFD's height
    \param vessel for which the MFD has been opened
    */
    HUDDrawerMFD (DWORD w, DWORD h, VESSEL * vessel );

    /// Destructor
    ~HUDDrawerMFD ();

    /// Creates button label
    /** Refer to Orbiter SDK doc */
    char *ButtonLabel (int bt);

    /// Creates button label
    /** Refer to Orbiter SDK doc */
    int ButtonMenu (const MFDBUTTONMENU **menu) const;

    /// Updates MFD
    /** Refer to Orbiter SDK doc */
    MFD_RETURN_TYPE Update ( MyDC myDC );

    /// Defines key reaction
    /** Refer to Orbiter SDK doc */
    bool ConsumeKeyBuffered(DWORD key);

    /// Defines mouse reaction
    /** Refer to Orbiter SDK doc */
    bool ConsumeButton (int bt, int event);

    /// Message parser
    /** Refer to Orbiter SDK doc */
    static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

    // MFD button callbacks
    void OnPrevItem();
    void OnNextItem();
    void OnModItem();
    void OnEnableAll();
    void OnDisableAll();

private:
    int iSelected;

};

#endif // HUDDRAWERMFD_H
