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
// LaunchCompass.cpp - Calculations of compass for vertical launches
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


#define STRICT
#include "windows.h"
#include "Orbitersdk.h"
#include <cmath>
#include <Orbiter/SpaceMathOrbiter.hpp>
#include "LaunchCompass.h"
#include "MFDDataLaunchMFD.hpp"
#include "localisation.h"
#include "globals.h"
#include "Math/MaxMin.hpp"
#include "Math/GeneralMath.hpp"
#include <Utils/MFDTextCalculator.hpp>

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

#define COMPASS_ROSE_SCALE	0.9

static int mode;

#ifdef ORB2006
	void Circle(HDC hDC, int x, int y, double radius)
	{

		MoveToEx(hDC, x + (int)radius, y, NULL);
		AngleArc(hDC, x, y, (int)radius, 0, 360);
	}
#else
	#ifdef ORB2009
		void Circle(oapi::Sketchpad * skp, int x, int y, double radius)
		{
			skp->Ellipse((int)(x - radius), (int)(y - radius), (int)(x + radius), (int)(y + radius));
		}
	#endif
#endif


int DisplayEngUnit(char* buffer, size_t sizeOfBuffer, char* pattern, double value)
{
	char Big[]=  " kMGTPEZY";
	char Small[]=" mµnpfazy";
	int ptr = 0;
	if(fabs(value) > 1e-24)
	{
		if(fabs(value) > 1)
		{
			while(fabs(value) >= 1000 && ptr <= 8)
			{
				ptr++;
				value = value / 1000.0;
			}
			if(ptr == 8)
			{
				ptr = 8;
				value = 999.999 * ((value > 0) ? 1 : -1);
			}
			return sprintf_s(buffer, sizeOfBuffer, pattern, value, Big[ptr]);
		}
		else
		{
			while(fabs(value) < 1 && ptr < 8)
			{
				ptr++;
				value = value * 1000.0;
			}
			return sprintf_s(buffer, sizeOfBuffer, pattern, value, Small[ptr]);
		}
	}
	else
	{
		return sprintf_s(buffer, sizeOfBuffer, pattern, value, Small[0]);
	}
}

///////////////////////////////////////////////////////////////
// MFD class implementation
LaunchCompass::LaunchCompass (DWORD w, DWORD h, VESSEL *v)
: MFDPage (w, h, v)
, targetBearing(0.0)
{
    isHUDDrawing = true;
    mode = NorthUp;
    PeA = ApA = 0;
}

LaunchCompass::~LaunchCompass()
{}

///////////////////////////////////////////////////////////////
//		General functions
MFD_RETURN_TYPE LaunchCompass::Update (MyDC hDC, MFDDataLaunchMFD * data)
{
	vessel = oapiGetFocusInterface();
	mode = data->m_compassDisplayMode;
	DrawCompassRose(hDC);
	DrawVessel(hDC);
	PrintWords(hDC, data->launched);

    return MFD_RETURN_VALUE( true );
}

void LaunchCompass::SetTargetAzimuth(double azimuth, LAUNCH_AZIMUTH azFlag)
{
	targetBearing = azimuth;
	this->azFlag = azFlag;
}

void LaunchCompass::DrawCompassRose( MyDC hDC )
{
    MFDSetPen(hDC, pens.GetPen(Pens::Green));

	const double radius = COMPASS_ROSE_SCALE * MaxMin<DWORD>(m_W, m_H).GetMin() / 2;
	int w = m_W / 2,
		h = m_H / 2;

	// Draw 3 rings, one for 80, 70 and 60° (90° is centre of circle);
	Circle(hDC, w, h, radius);
	Circle(hDC, w, h, radius * 2 / 3);
	Circle(hDC, w, h, radius / 3);
	// Draw degree markers
	double sqrt1_2 = sqrt(2.0) / 2;
	//int nudge = 10;

	MFDTextOut(hDC, (int)(w + sqrt1_2 * radius + 10),
				 (int)(h - sqrt1_2 * radius - 10), WHITE, "60°");
	MFDTextOut(hDC, (int)(w + sqrt1_2 * radius * 2 / 3 + 10),
				 (int)(h - sqrt1_2 * radius * 2 / 3 - 10), WHITE, "70°");
	MFDTextOut(hDC, (int)(w + sqrt1_2 * radius / 3 + 10),
				 (int)(h - sqrt1_2 * radius / 3 - 10), WHITE, "80°");

	// Draw the NSEW - note, draw it with E on left as per astronomical charts
	const int textheight = 18;
	const int textwidth = 10;
	double heading;
	oapiGetFocusHeading(&heading);
	heading = -heading;
	double bearing = - SpaceMathOrbiter().GetHeadBearing(oapiGetFocusInterface());
	switch(mode)
	{
	case NorthUp:
		MFDTextOut(hDC, w - textwidth / 2, h - (int)radius - textheight, WHITE, NORTH);
		MFDTextOut(hDC, w - textwidth / 2, h + (int)radius, WHITE, SOUTH);
		MFDTextOut(hDC, w - (int)radius - textwidth, h - textheight / 2, WHITE, EAST);
		MFDTextOut(hDC, w + (int)radius + textwidth / 2, h - textheight / 2, WHITE, WEST);
		break;
	case HeadUp:
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(bearing) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(bearing) - textheight / 2),
					 WHITE, NORTH);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(bearing + PI / 2) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(bearing + PI / 2) - textheight / 2),
					 WHITE, EAST);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(bearing + PI) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(bearing + PI) - textheight / 2),
					 WHITE, SOUTH);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(bearing + 3 * PI / 2) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(bearing + 3 * PI / 2) - textheight / 2),
					 WHITE, WEST);
		break;
	case HeadingUp:
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(heading) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(heading) - textheight / 2),
					 WHITE, NORTH);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(heading + PI / 2) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(heading + PI / 2) - textheight / 2),
					 WHITE, EAST);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(heading + PI) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(heading + PI) - textheight / 2),
					 WHITE, SOUTH);
		MFDTextOut(hDC, (int)(w - (radius + textheight / 2) * sin(heading + 3 * PI / 2) - textwidth / 2),
					 (int)(h - (radius + textheight / 2) * cos(heading + 3 * PI / 2) - textheight / 2),
					 WHITE, WEST);
		break;
	}

	// Draw the target marker - a yellow dashed line from the centre to the
	// outer ring on the target bearing
	double targetasc, targetdesc;
	switch(mode)
	{
	case NorthUp:
		targetasc = targetBearing;
		targetdesc = PI - targetBearing;
		break;
	case HeadUp:
		targetasc = targetBearing + bearing;
		targetdesc = PI - targetBearing + bearing;
		break;
	case HeadingUp:
		targetasc = targetBearing + heading;
		targetdesc = PI - targetBearing + heading;
		break;
	}

	// Draw the target lines and target pitch
	if(azFlag == NORTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
	{
		DrawTargetPitchAndLines(hDC, targetasc, radius);
	}
	if(azFlag == SOUTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
	{
		DrawTargetPitchAndLines(hDC, targetdesc, radius);
	}
}

void LaunchCompass::DrawTargetPitchAndLines(MyDC hDC, double targetIncl, double radius)
{
	// Target pitch variables preparation
	double rad = (PI / 2 - targetPitch) * DEG * m_W * COMPASS_ROSE_SCALE / 30.0 / 2.0;
	const int linelength = 10;
	int w = m_W / 2,
		h = m_H / 2;

	MFDSetPen(hDC, pens.GetPen(Pens::GreenDashed));

	// lines
	MFDLine(hDC, w, h, (int)(w - radius * sin(targetIncl)),
		(int)(h - radius * cos(targetIncl)));

	MFDSetPen(hDC, pens.GetPen(Pens::Green));

	// target pitch
	int x = (int)(m_W / 2 - rad * sin(targetIncl));	// - as east is left (star-map style)
	int y = (int)(m_H / 2 - rad * cos(targetIncl));


	MFDLine(hDC, x, y, (int)(x + linelength),(int)(y)	);
	MFDLine(hDC, x, y, (int)(x - linelength),(int)(y)	);
	MFDLine(hDC, x, y, (int)(x),(int)(y + linelength)	);
	MFDLine(hDC, x, y, (int)(x),(int)(y - linelength)	);
}

void LaunchCompass::DrawVessel (MyDC hDC)
{
	MFDSetPen(hDC, pens.GetPen(Pens::Yellow));

	OBJHANDLE vessel = oapiGetFocusObject();
	double pitch, heading,
		   up = SpaceMathOrbiter().GetHeadBearing(oapiGetFocusInterface());	// up is the direction that your head is pointing
	oapiGetPitch(vessel, &pitch);
	oapiGetHeading(vessel, &heading);

	switch(mode)
	{
	case NorthUp:
		break;	// do nothing
	case HeadUp:
		// rotate such that head is up
		heading -= up;
		up -= up;
		break;
	case HeadingUp:
		// rotate such that heading is up
		up -= heading;
		heading -= heading;
		break;
	}

	//if(pitch > 60.0 * RAD)	// pitch > 60°
	{
		// Position of the craft in the MFD
		double rad = (PI / 2 - pitch) * DEG * m_W * COMPASS_ROSE_SCALE / 30.0 / 2.0;

		int x = (int)(m_W / 2 - rad * sin(heading));	// - as east is left (star-map style)
		int y = (int)(m_H / 2 - rad * cos(heading));

		const int linelength = 15;

		MFDLine(hDC, x, y,	(int)(x - linelength * cos(up)),
							(int)(y + linelength * sin(up)));
		MFDLine(hDC, x, y,	(int)(x + linelength * cos(up)),
							(int)(y - linelength * sin(up)));
		MFDLine(hDC, x, y,	(int)(x + linelength * cos(up + PI / 2)),
							(int)(y - linelength * sin(up + PI / 2)));
	}
}

void LaunchCompass::PrintWords (MyDC hDC, bool flying)
{
    GeneralMath gm;
    MFDTextCalculator c(m_W, m_H);
	const int x = 5;
	const int tab = 12;
	int y = 1;
	const int BUFFER_SIZE = 64;
	char buffer[BUFFER_SIZE];

	MFDTextOut(hDC, x, c.Y(y), YELLOW, MODE);
	switch(mode)
	{
        case NorthUp:	MFDTextOut(hDC, x + c.X(tab), c.Y(y++), GREEN, NORTH_UP); break;
        case HeadUp:	MFDTextOut(hDC, x + c.X(tab), c.Y(y++), GREEN, HEAD_UP); break;
        case HeadingUp:	MFDTextOut(hDC, x + c.X(tab), c.Y(y++), GREEN, HEADING_UP); break;
	}

	OBJHANDLE hVessel = oapiGetFocusObject();
	double pitch;
	oapiGetPitch(hVessel, &pitch);
	// convert from radians to degrees
	pitch *= DEG;

	if(azFlag == NORTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
	{
		MFDTextOut(hDC, x,              c.Y(y), YELLOW, TARGET_ASC);
		MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, "%.2lf°", targetBearing * DEG);
	}

	if(azFlag == SOUTHERN_AZIMUTH || azFlag == BOTH_AZIMUTHS)
	{
		MFDTextOut(hDC, x,              c.Y(y), YELLOW, TARGET_DSC);
		double tgt = gm.GetIn0_2PIRange(PI - targetBearing) * DEG;
		MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, "%.2lf°", tgt);
	}

	MFDTextOut(hDC, x,              c.Y(y), YELLOW, PITCH);
	MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, "%.2lf°", pitch);

	double headbearing = gm.GetIn0_2PIRange(SpaceMathOrbiter().GetHeadBearing(oapiGetFocusInterface()));
	double altitude;
	oapiGetAltitude(hVessel, &altitude);
	MFDTextOut(hDC, x,              c.Y(y), YELLOW, HEAD_BEARING);
	MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, "%.2lf°", flying ? headbearing * DEG : 0);

	double footbearing = gm.GetIn0_2PIRange(headbearing - PI);
	VECTOR3 vel;
	oapiGetAirspeedVector(hVessel, &vel);
	MFDTextOut(hDC, x,              c.Y(y),     YELLOW, FOOT_BEARING);
	MFDTextOut(hDC, x + c.X(tab),   c.Y(y++),   GREEN, "%.2lf°", flying ? footbearing * DEG : 0);
	MFDTextOut(hDC, x,              c.Y(y), YELLOW, ALTITUDE);
	DisplayEngUnit(buffer, BUFFER_SIZE, "%.2lf%cm", altitude);
	MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, buffer);
	MFDTextOut(hDC, x, c.Y(y), YELLOW, VERTICAL_SPEED);
	DisplayEngUnit(buffer, BUFFER_SIZE, "%.2lf%cm/s", vel.y);
	MFDTextOut(hDC, x + c.X(tab),   c.Y(y++), GREEN, buffer);
}

void LaunchCompass::SetApses(double PeA, double ApA)
{
	if (PeA > ApA)
	{
		double temp = ApA;
		ApA = PeA;
		PeA = temp;
	}
	this->PeA = PeA;
	this->ApA = ApA;
}

void LaunchCompass::SetTargetPitch(double pitch)
{
    targetPitch = pitch;
}

