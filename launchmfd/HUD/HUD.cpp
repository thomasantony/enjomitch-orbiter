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
// HUD.cpp - Draws Flight Director on HUD
// Authors - Steve "agentgonzo" Arch
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

#include "HUD.h"
#include "Math/GeneralMath.hpp"

using namespace EnjoLib;

HUD::HUD()
{
}

HUD::~HUD()
{
}

void HUD::SetScale(const HUDPAINTSPEC *hps)
{
	pixelsPerDegree = hps->H / 2 / (oapiCameraAperture() / RAD); // Temporaty ! Sorry for the mess !
	W = hps->W;
	H = hps->H;
	cX = W / 2;
	cY = H / 2;
}

void HUD::DrawCross(MyDC hDC, POINT p, double bank, int crossLength)
{
    GeneralMath gm;
    int xa0 = gm.round( p.x - crossLength * cos(bank) );
    int ya0 = gm.round( p.y - crossLength * sin(bank) );
    int xa1 = gm.round( p.x + crossLength * cos(bank) );
    int ya1 = gm.round( p.y + crossLength * sin(bank) );
	MFDLine(hDC, xa0, ya0, xa1, ya1);

    int xb0 = gm.round( p.x + crossLength * sin(bank) );
    int yb0 = gm.round( p.y - crossLength * cos(bank) );
    int xb1 = gm.round( p.x - crossLength * sin(bank) );
    int yb1 = gm.round( p.y + crossLength * cos(bank) );
	MFDLine(hDC, xb0, yb0, xb1, yb1);
}

void HUD::DrawArrow(MyDC hDC, double angle, double triangleAngle, int sideLength)
{
	// Draw a triangle that points in the direction of 'angle' near the edge of the HUD
	const int radius = int(H / 2 * 0.7);	// distance from the centre to the marker

    GeneralMath gm;
	const POINT tip = {gm.round(cX + radius * sin(angle)),
						gm.round(cY - radius * cos(angle))};	// tip of the triangle

    int xa1 = gm.round(tip.x - sideLength * sin(angle + triangleAngle));
    int ya1 = gm.round(tip.y + sideLength * cos(angle + triangleAngle));
    int xb1 = gm.round(tip.x - sideLength * sin(angle - triangleAngle));
    int yb1 = gm.round(tip.y + sideLength * cos(angle - triangleAngle));

	MFDLine(hDC, tip.x, tip.y, xa1, ya1 );
	MFDLine(hDC, tip.x, tip.y, xb1, yb1 );
	MFDLine(hDC, xa1, ya1, xb1, yb1 );
}

void HUD::DrawCircle(MyDC hDC, POINT centre, double radius)
{
	// Move to the right of the circle
	#ifdef ORB2006
        MoveToExFloat(hDC, centre.x + radius, centre.y, NULL);
        AngleArc(hDC, centre.x, centre.y, (DWORD)radius, 0, 360);
    #else
        #ifdef ORB2009
                hDC->Ellipse((int)(centre.x - radius), (int)(centre.y - radius),
                               (int)(centre.x + radius), (int)(centre.y + radius));
        #endif
    #endif
}
