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
// PitchSchedule.cpp
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

#include "PitchSchedule.h"

PitchSchedule::PitchSchedule(void)
{
	char filename[_MAX_PATH];
	char directory[_MAX_PATH];
	char bodyname[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, directory);
	VESSEL* vessel = oapiGetFocusInterface();
	oapiGetObjectName(vessel->GetGravityRef(), bodyname, _MAX_PATH);
	sprintf_s(filename, _MAX_PATH, "%s\\InPitch-%s-%s.ini",
				directory, bodyname, vessel->GetName());

	ReadPitches(filename);
}

PitchSchedule::~PitchSchedule()
{
}

void PitchSchedule::ReadPitches(char *filename)
{
	file.open(filename);
	if(file)
	{
		isValid = true;
		AltitudePitchPair pair;

		char unused[1024];
		while(file && !file.eof())
		{
			if(file.peek() == '#')
				file.getline(unused, sizeof(unused));	// skip over comment line
			else
			{
				pair.altitude = -1;
				pair << file;
				pair.pitch *= RAD;	// convert from degrees in file to rads
				if(pair.altitude >= 0)	// only add valid pairs
					pitches.push_back(pair);
			}
		}

		pitches.sort();
	}
	else
		isValid = false;
	file.close();
}

double PitchSchedule::GetTargetPitch()
{
	static list<AltitudePitchPair>::iterator above = pitches.begin();	// pointer to the pair below the current altitude
	static list<AltitudePitchPair>::iterator below = above++;;	// pointer to the pair above the current altitude

	double altitude;
	oapiGetFocusAltitude(&altitude);	// current altitude

	// Get the appropriate above and below values for our current altitude
	while(above->altitude <= altitude && *above != pitches.back())
	{
		below++;
		above++;
	}
	while(below->altitude > altitude && below != pitches.begin())
	{
		below--;
		above--;
	}

	// linearly interpret between the above and below pitch values for our altitude
	double altitudeDiff = above->altitude - below->altitude;
	double pitch;
	if(altitude < below->altitude)
		pitch = below->pitch;
	else if(altitude > above->altitude)
		pitch = above->pitch;
	else
		pitch = below->pitch + (altitude - below->altitude) * (above->pitch - below->pitch) / (above->altitude - below->altitude);

	return pitch;
}
