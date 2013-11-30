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
// DirectAscentPage.h - data input and output for and from direct ascent algorithm
// Authors - Szymon "Enjo" Ender
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

#pragma once

#include "../globals.h"
#include "../MFDPage.h"
#include "../Types.hpp"
#include "../localisation.h"
#include "../../lib/Visual/Canvas.hpp"
#include "../../lib/PID/PID.hpp"
#include "../Pens.h"
#include "Systems/RectangleMy.hpp"
#include "DirectAscent.h"
#include "DirectAscentResults.hpp"
#include <vector>
#include <string>
#include <sstream>


class MFDDataLaunchMFD;

class DirectAscentPage : public MFDPage, public DirectAscent, public Canvas
{

public:
	DirectAscentPage(DWORD w, DWORD h, VESSEL *v);
	virtual ~DirectAscentPage();
	#ifdef ORB2006
	    void
	#else
	    bool
	#endif
	    Update ( MyDC hDC );
	bool Reset();

	void ScheduleForCalculation();
	void SetTargetString(char * cTgt);
	void InnerLoop();
	void SetMFDData( MFDDataLaunchMFD * data );

	// Canvas
    Point GetWindowSize() const;
    RectangleMy GetDataSize() const;
    RectangleMy GetDataToDisplaySize() const;

protected:
    // Direct ascent
    bool InitInternalVars();
    double GetShipsInitialAcceleration();

    // Canvas
    void RefreshClient();
    void ScaleData();


private:

	void Center( bool reset );
	void Zoom();
	void UpdateZoom(double diffT);

	void SetLineStyle(MyDC hDC, Pens::LineStyle lineStyle );
    void DrawInnerLoop(MyDC hDC);
    void DrawError(MyDC hDC);
    void PrintWords(MyDC hDC);
    void DrawTrajectory(MyDC hDC, const std::vector <Point> & trajectory, Pens::LineStyle lineStyle, double diffFromRotation = 0);

    Pens pens;

	void InitWords();

	DirectAscentResults CalculateDirectAscent();

	void ScalePath();
	void ScaleGC( bool bSatOnly = true);
	void InitOutputVars();
	double CalcAngSatEqu( VESSEL * target );

	double GetDiffTime();

	double m_zoomTarget;
	Point m_refSystemTarget;
	std::string sTgt;

	bool m_bScheduledForCalculation;

	std::vector <Point> vs;
	std::vector <Point> vsRec;
	std::vector <Point> vGCsatScaled; // sat's path (great circle)
    std::vector <Point> vGCshScaled; // ship's path (great circle)
    std::vector <Point> vGCshStartScaled; // ship's starting path (great circle)

	bool alreadyMoved;
	double needed_east_v_at_this_alt;
	MFDDataLaunchMFD * m_data;
	double diffXFromRotation;
	PID m_pidZoom, m_pidCoordSystem;

	std::vector<std::string> vs_output_da, vs_vars_da;
};
