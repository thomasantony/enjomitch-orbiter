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
// Authors - Chris "Kwan" Jeppesen - original Java version author
//           Szymon "Enjo" Ender - C++ conversion
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

#ifndef PEGMULTI_H
#define PEGMULTI_H

#include <orbitersdk.h>
#include "../PitchGuidance.h"
#include "../Stage.h"
#include "../Utils/Engine.hpp"

class PEGMulti
{
    public:
        PEGMulti( const VESSEL * v );
        virtual ~PEGMulti();
        bool IsValid() {return valid;};
        virtual double GetTargetPitch(double timeSinceLaunch);
        void SetApses(double PeAlt, double ApAlt);
        double GetTMECO() {return metCutoff - met;};
        void SetStage( const Stage & stage, double timeSinceLaunch );

private:

    const VESSEL * vessel;
    Stage m_stage;
    Engine eng;

	bool valid;

    double rbar_end;
	// From PEGAutopilot
//	double b0(double T);
//	double bn(double T, int n);
//	double c0(double T);
//	double cn(double T, int n);

    double b0(int j,double t_j);
    double b_(int n, int j, double t_j);
    double c0(int j,double t_j);
    double c_(int n, int j, double t_j);
    double a(int j, double t_j);

	void Navigate(double timeSinceLaunch);
	void Navigate2();
	void Estimate();
	void Guide();
	void Init();

	//Targeting variables
	double raTarget,rpTarget,eTarget,hpTarget,haTarget,taTarget;
	//Navigation variables
	double r,h,omega;
	double vr,vh;

	double mu,g;
	double met;
	double a0,tau;

	//Estimation variables
	//double r_T,rdot_T_end;
	double p,aOrbit,metCutoff;
    double A,B;
    double CmdPDot, CmdP;
	double target_pitch,last_target_pitch;
	double TMajorCycle;
	double t0;          // reference time: designated liftoff time
};

#endif // PEGMULTI_H
