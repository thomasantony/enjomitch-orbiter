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
// LaunchMFD.h - Main MFD class
// Authors - "Vanguard", Szymon "Enjo" Ender, Steve "agentgonzo" Arch
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


#ifndef __LAUNCHMFD_H
#define __LAUNCHMFD_H

#include <orbitersdk.h>

#include "LaunchCompass.h"
#include "DirectAscentPage.hpp"
#include "globals.h"
#include "Utils/Pens.h"
#include "HUD/HUDDrawer.hpp"

#include <vector>
#include <string>
#include <sstream>

class MFDDataLaunchMFD;
class MFDButtonPageLaunchMFD;
class PluginLaunchMFD;

namespace EnjoLib
{
    namespace MFDGoodies
    {
       class Sound;
    }
}

/// Main MFD class
/**
Main MFD class. Constructed each time the MFD is opened or vessel is changed.
Therefore, if you need to store the values of this and other vessels,
you need to use and external data structure, that stores these values.
Here, it's called MFDData.
*/
class LaunchMFD: public MY_MFD
{
public:
    /// Default constructor
    /**
    Constructs the MFD.
    \param w - MFD's width
    \param h - MFD's height
    \param vessel for which the MFD has been opened
    \param plugin contains configuration and other data neede by the MFD
    */
    LaunchMFD (DWORD w, DWORD h, VESSEL * vessel, PluginLaunchMFD * plugin );

    /// Destructor
    ~LaunchMFD ();
    char *ButtonLabel (int bt);
    int ButtonMenu (const MFDBUTTONMENU **menu) const;

    MFD_RETURN_TYPE Update ( MyDC myDC );

    bool ConsumeKeyBuffered(DWORD key);
    bool ConsumeKeyImmediate(char * kstate );
    bool ConsumeButton (int bt, int event);


    /// Returns Sound class
    /** Returns a reference to the Sound class, needed outside the MFD's context
    - for example, in callbacks. */
    EnjoLib::MFDGoodies::Sound & GetSound();

    /// Returns MFDData class
    /** Returns a reference to the MFDData class, needed outside the MFD's context
     - for example, in callbacks. */
    MFDDataLaunchMFD * GetData();
    const MFDDataLaunchMFD * GetData() const;

    void WriteStatus (FILEHANDLE scn) const;
    void ReadStatus (FILEHANDLE scn);

    static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

    void SwitchButtonsPage();
    void SwitchMode();
    void SwitchAutopilot();
    void SwitchPitchGuidance();
    void SwitchOffplaneCorrection();
    void SwitchAzimuth();
    void SwitchGreatCircleUse();
    void SwitchGreatCircleTrack();
    void SwitchGreatCircleLines();
    void SwitchHUD();
    void SwitchSound();
    void GreatCircleZoomIn();
    void GreatCircleZoomOut();
    void GreatCircleIncreasePlotPrecision();
    void GreatCircleDecreasePlotPrecision();
    void IncreaseInclination();
    void DecreaseInclination();
    void IncreaseInclinationFactor();
    void DecreaseInclinationFactor();
    void DefaultAction();
    void OpenDialogTarget();
    void OpenDialogAltitude();
    void OpenDialogPIDXY();
    void OpenDialogPIDBank();
    void DoNothing();

protected:

private:

    void SetError();
    void CheckErrorExcess();
	void ReactOnShipStatus();


    void DrawErrorAndMarks(MyDC hDC, const int status);
    void PrintResults(MyDC hDC);
    void PrintTargetParamsForAzimuthMode(MyDC hDC);
    void ReactOnReachingOrbit(MyDC hDC);


    double az, az_desc, error, heading, pitchError;

    LaunchCompass compass;
    DirectAscentPage daAtmo;
    HUDDrawer m_hudDrawer;

    /// A reference to the MFDData object.
    /** Reference is prefered to pointer, to make sure it's initialised */
    MFDDataLaunchMFD * m_data;

    /// A reference to the Sound object.
    /** Reference is prefered to pointer, to make sure it's initialised */
    EnjoLib::MFDGoodies::Sound & m_sound;
    const MFDButtonPageLaunchMFD & m_buttonPages;
    bool m_beep;
    const static double MAX_ERROR;

    Pens m_pens;
};

#endif // !__LAUNCHMFD_H
