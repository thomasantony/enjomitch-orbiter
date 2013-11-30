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
// HUDCompass.cpp - Draws compass on HUD for vertical launches
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
#include "HUDCompass.h"
#include "Math/GeneralMath.hpp"
using namespace EnjoLib;

#define RINGS	3
extern double getdeterminant(const MATRIX3 mat);
extern MATRIX3 getinvmatrix(const MATRIX3 mat);

HUDCompass::HUDCompass(void)
{
}

HUDCompass::~HUDCompass(void)
{
}

void HUDCompass::DrawHUD(int mode, const HUDPAINTSPEC *hps, MyDC hDC)
{
	if(oapiGetFocusInterface()->GetPitch() < PI / 4)
		return;	// don't draw it if you are pitched too low

	double fov = oapiCameraAperture() / RAD;
	int innerRing = 10;

	VECTOR3 zenith = GetUpVector();	// Gets the zenith unit vector in vessel's frame
	fov = oapiCameraAperture();
	double y = sin(fov);
	double scale = H / 2 / y;	// scale factor to multiply unit vectors up to the screen size
	POINT centre = {(long)(cX + zenith.x * scale + 0.5),
					(long)(cY - zenith.y * scale + 0.5)};
	if(zenith.z > 0)	// only draw compass if it is in front of us!
	{
		for(int i = 0; i < RINGS; i++)
			DrawRing(hDC, centre, innerRing * (i + 1) * pixelsPerDegree, headBearing, i == RINGS - 1);
		DrawTargetMarker(hDC, centre, innerRing * RINGS * pixelsPerDegree, headBearing);
	}
}

void HUDCompass::SetParams(double target, double headBearing, LAUNCH_AZIMUTH flag)
{
	targetBearing = target;
	this->headBearing = headBearing;
	azFlag = flag;
}

void HUDCompass::DrawRing(MyDC hDC, POINT centre, double radius, double headBearing, bool drawText)
{
	DrawCircle(hDC, centre, radius);
	// Draw the ticks

	const int tickShort = 5;
	const int tickLong = 10;
	const int textOffset = 20;
	// Draw ticks every 10°

	// Set the text to be aligned to the centre of the marker point
	/// dropped
	//UINT previousAlignment = SetTextAlign(hDC, TA_CENTER);
	MFDTextAlignCenter(hDC);

	for(int angle = 0; angle < 360; angle += 10)
	{
		double rad = angle * RAD - headBearing;
		// Use -sin so that East is on the left when north is up (astronomical charts)
		GeneralMath gm;
		const int length = (angle % 90 == 0) ? tickLong : tickShort;
		int x0 = gm.round(centre.x - radius * sin(rad));
		int y0 = gm.round(centre.y - radius * cos(rad));
		int x1 = gm.round(centre.x - (radius + length) * sin(rad));
		int y1 = gm.round(centre.y - (radius + length) * cos(rad));
        MFDLine(hDC, x0, y0, x1, y1);
		if(drawText)
		{
		    int xt = gm.round(centre.x - (radius + textOffset) * sin(rad));
		    int yt = gm.round(centre.y - (radius + textOffset) * cos(rad) - 8);
		    MFDTextOut(hDC, xt, yt, GREEN, "%d", angle);
		}
	}

	// Reset the text alignment
	/// dropped
	//SetTextAlign(hDC, previousAlignment);
}

void HUDCompass::DrawTargetMarker(MyDC hDC, POINT centre, double radius, double headBearing)
{
	double target = targetBearing - headBearing;
	double complimentaryTarget = PI - targetBearing - headBearing;
	GeneralMath gm;
	int x0 = gm.round(centre.x + 0.5);
	int y0 = gm.round(centre.y + 0.5);
	if(azFlag == NORTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
        MFDLine( hDC, x0, y0,
                gm.round(centre.x - radius * sin(target) + 0.5),
                gm.round(centre.y - radius * cos(target) + 0.5) );

	if(azFlag == SOUTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
            MFDLine( hDC, x0, y0,
                gm.round(centre.x - radius * sin(complimentaryTarget) + 0.5),
                gm.round(centre.y - radius * cos(complimentaryTarget) + 0.5) );
}

VECTOR3 HUDCompass::GetUpVector()
{
	VECTOR3 up = {0, 1, 0},
			zenith;
	VESSEL * vessel = oapiGetFocusInterface();
	vessel->HorizonInvRot(up, zenith);
	return zenith;
}
