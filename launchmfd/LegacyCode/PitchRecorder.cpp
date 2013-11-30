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
// PitchRecorder.cpp
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


#include "PitchRecorder.h"

#define LOG_STEP 1000	// log every km

PitchRecorder::PitchRecorder(HWND hWnd)
: nextLogAltitude(0)
, window(hWnd)
{
	vessel = oapiGetFocusInterface();
	gBody = vessel->GetGravityRef();

	char filename[_MAX_PATH];
	char directory[_MAX_PATH];
	char bodyname[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, directory);
	oapiGetObjectName(gBody, bodyname, _MAX_PATH);
	sprintf_s(filename, _MAX_PATH, "%s\\Pitch-%s-%s.ini",
				directory, bodyname, vessel->GetName());

	file.open(filename);
	LogFileHeader();

	SetTimer(hWnd, (UINT_PTR)this, 1000, AltitudeLoop);
}

PitchRecorder::~PitchRecorder(void)
{
	file.close();
}

void PitchRecorder::LogFileHeader()
{
	if(file)
	{
		char bodyname[_MAX_PATH];
		oapiGetObjectName(gBody, bodyname, _MAX_PATH);
		file << "# Recorded pitch profile for " << vessel->GetName()
			<< " launching from " << bodyname << endl;
		file << "# Sim start time: " << oapiGetSimMJD() << endl;

		file << "# Sim start time: " << oapiGetSimMJD() << endl;

		SYSTEMTIME realTime;
		GetSystemTime(&realTime);
		file << "# Real Time: " << realTime.wYear << "-" << realTime.wMonth << "-" << realTime.wDay
			<< ", " << realTime.wHour << ":" << realTime.wMinute << endl;

		// finish with a empty line separator
		file << endl;
	}
}

void PitchRecorder::WriteAltitude(int altitude, double pitch)
{
	if(file)
		file << altitude << " = " << pitch * DEG << endl;
}

void PitchRecorder::AltitudeLoop(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	PitchRecorder *ptr = (PitchRecorder*)idEvent;
	if(ptr->BeforeMECO())
	{
		if(ptr->vessel->GetAltitude() > ptr->nextLogAltitude)
		{
			ptr->WriteAltitude(ptr->nextLogAltitude, ptr->vessel->GetPitch());
			ptr->nextLogAltitude += LOG_STEP;
		}
	}
	else
	{
		KillTimer(hwnd, idEvent);	// no need for the timer anymore
		ptr->file.close();			// close the log file
	}
}

bool PitchRecorder::BeforeMECO()
{
	static bool ignitionHasOccurred = false;
	static bool MECOHasOccurred = false;

	if(!MECOHasOccurred)
	{
		bool thrusting = (	vessel->GetThrusterGroupLevel(THGROUP_MAIN) > 0 ||
							vessel->GetThrusterGroupLevel(THGROUP_HOVER)> 0);

		ignitionHasOccurred |= thrusting;
		if(ignitionHasOccurred && !thrusting)
			MECOHasOccurred = true;
	}

	return !MECOHasOccurred;
}


