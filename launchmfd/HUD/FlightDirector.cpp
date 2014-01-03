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
// FlightDirector.cpp - representation of required pitch and yaw on HUD
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

#include "FlightDirector.h"

FlightDirector::FlightDirector(void)
: targetPitch(0)
, targetAzimuth(0)
{
}

FlightDirector::~FlightDirector(void)
{
}

void FlightDirector::DrawHUD(int mode, const HUDPAINTSPEC *hps, MyDC hDC)
{
	if(oapiGetHUDMode() != HUD_SURFACE)
		return;	// only draw the HUD if in surface mode

	// Convert the pitch/azimuth into PRY
	VECTOR3 target = GetTargetVector(oapiGetFocusInterface(), targetPitch, targetAzimuth);
    //sprintf(oapiDebugString(),  "x = %0.2lf, y = %0.2lf, z = %0.2lf",target.x, target.y, target.z);
	// Stategy: Having got this target vector (unit), project this down to the 2D plane, scale it by
	//			a scale factor and then draw the cross if within view or target marker if not

	// Scale. Get the current Field of View
	double fov = oapiCameraAperture();	// this is the number of degrees from centre to top of viewport
	// Get the y component of the unit vector that points to this angle (top of screen)
	double y = sin(fov);

	double scale = H / 2 / y;	// scale factor to multiply unit vectors up to the screen size

	if((abs(target.y) * scale < H / 2) &&
		(abs(target.x) * scale < W / 2) &&
		target.z > 0)
	{
		// The target is within the field of view - draw the target marker
		POINT p = {(long)(cX + target.x * scale),
					(long)(cY - target.y * scale)};
		double bank = 0;
		if(tailSitter)
		{
			// We need to get the bank as this affects the direction of a tailsitter's engine thrust.
			// Get the vector perpendicular to the target vector.
			double azimuth = targetAzimuth;
			double pitch = targetPitch + PI / 2;
			VECTOR3 surfaceRelativeTarget = {sin(azimuth) * cos(pitch),
											 sin(pitch),
											 cos(azimuth) * cos(pitch)};
			VECTOR3 horizontalTarget = {sin(azimuth) * cos(pitch),
										0,
										cos(azimuth) * cos(pitch)};
			VECTOR3 right = crossp(surfaceRelativeTarget, horizontalTarget);	// The vector to the right if you are facing the target vector
			VESSEL *vessel = oapiGetFocusInterface();
			VECTOR3 rightVessel;
			vessel->HorizonInvRot(right, rightVessel);	// Transform the 'right' vector into vessel coordinates
			rightVessel /= length(rightVessel);
			VECTOR3 r = {1, 0, 0};	// the Vector to the right of the vessel in the vessel's coordinates
			bank = asin(length(crossp(rightVessel, r)));	// the angle between the rightvessel vector and the r vector
			if(rightVessel.y > 0)	// This is lazy, but it's too late at night to do a better fix
				bank = -bank;
			if(targetPitch < -PI / 2)
				bank = -bank;
		}
		DrawCross(hDC, p, bank);
	}
	else // The target is not within the field of view - draw a pointer to the target
		DrawArrow(hDC, atan2(target.x, target.y));
}

void FlightDirector::SetParams(double pitch, double azimuth)
{
	targetPitch = pitch;
	targetAzimuth = azimuth;
	if(tailSitter)
		targetPitch -= PI / 2;	// If it is a tail sitter and is using the hover thruster, then pitch 90° down
}

VECTOR3 FlightDirector::GetTargetVector(VESSEL * ves, double pitch, double azimuth)
{
	// Create vector that represents surface-frame pitch-azimuth
	// y is up, z is north, x is east
	VECTOR3 surfaceRelativeTarget = {sin(azimuth) * cos(pitch),
									 sin(pitch),
									 cos(azimuth) * cos(pitch)};

	// Convert to vessel frame of reference
	VECTOR3 vesselRelativeTarget;
	ves->HorizonInvRot(surfaceRelativeTarget, vesselRelativeTarget);
	return vesselRelativeTarget;
}

//////////////////////////////////////////////////////////////
// Static member variables
int HUD::W;
int HUD::H;
int HUD::cX, HUD::cY; // value of the centre of the window
double HUD::pixelsPerDegree;
