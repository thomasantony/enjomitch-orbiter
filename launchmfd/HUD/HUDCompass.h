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
// HUDCompass.h - Draws compass on HUD for vertical launches
// Authors - Steve "agentgonzo" Arch
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

#pragma once
#include "HUD.h"
#include "../Types.hpp"

class HUDCompass :
	public HUD
{
public:
	HUDCompass(void);
	~HUDCompass(void);
	virtual void DrawHUD(int mode, const HUDPAINTSPEC *hps, MyDC hDC);
	void SetParams(double target, double headBearing, LAUNCH_AZIMUTH flag = BOTH_AZIMUTHS);

private:
	double targetBearing;
	double headBearing;
	LAUNCH_AZIMUTH azFlag;
	void DrawRing(MyDC hDC, POINT centre, double radius, double headBearing, bool drawText);
	void DrawTargetMarker(MyDC hDC, POINT centre, double radius, double headBearing);
	VECTOR3 GetUpVector();	// returns the zenith vector in the vessel's coordinate frame
};
