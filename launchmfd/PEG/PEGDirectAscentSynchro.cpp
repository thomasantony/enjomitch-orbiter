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

#include "PEGDirectAscentSynchro.h"
#include "../MFDDataLaunchMFD.hpp"
#include <Math/SpaceMathBody.hpp>
#include <Math/SpaceMath.hpp>
#include <Math/GeneralMath.hpp>
#include <Orbiter/VesselCapabilities.hpp>

using namespace EnjoLib;

PEGDirectAscentSynchro::PEGDirectAscentSynchro( const VESSEL * v, const ShipVariables & vars )
: PEGDirectAscent( v, vars )
, m_defaultOffByRatio(1.14)
{
    //ctor
}

PEGDirectAscentSynchro::~PEGDirectAscentSynchro()
{
    //dtor
}

void PEGDirectAscentSynchro::UpdateClient( MFDDataLaunchMFD * data )
{
    // Basic parameters
    const ELEMENTS & e = data->GetMovParams().ele;
    const ORBITPARAM & op = data->GetMovParams().opar;
    const TGTPARAM & tgt = data->GetTgtParam();

    // Synchro ascent
    SpaceMathBody bmRad(data->m_bodyPhys.mass, data->m_bodyPhys.radius);
    SpaceMathBody bm(data->m_bodyPhys.mass);

	const double shipVelRad = data->GetMovParams().m_velMod / (data->GetMovParams().m_rad);

	const double T = bmRad.CalcPeriod(data->GetCurrentAlt(), data->ApA); // Assumed target orbit T
	const double interceptT = T / 4.0;  // 1/4 of period
	 // Assume interception in 1/4th of orbit, which is realistic
	double finTrLTgT = tgt.TrL + tgt.velRad * interceptT;
	double finShipTrL = op.TrL + PI/2.0; // estimate final TrL
    finTrLTgT = GeneralMath().GetIn2PIRange( finTrLTgT );
	finShipTrL = GeneralMath().GetIn2PIRange( finShipTrL );

	double deltaTrL2nd = finShipTrL - finTrLTgT;
	deltaTrL2nd = GeneralMath().GetInPIRange( deltaTrL2nd );

    synchroTimeToBurn = deltaTrL2nd  / (tgt.velRad - shipVelRad);

    const bool isAboveCircular = op.ApT > T * 0.75; // Are we approaching apoapsis or periapsis?
    double dvCirc; // Circularise DV
    if ( isAboveCircular )
        dvCirc = 0;
    else
        dvCirc = bm.GetHohmannDVCircularise(op.PeD, op.ApD);
    const double dvExtend = bmRad.GetHohmannDVExtend(data->GetCurrentAlt(), data->ApA); // Orbit extend DV
    const double dvSum = dvCirc + dvExtend;
	double offByRatio = m_shipVariables.m_synchroAscentOffBy;
    if (offByRatio == 0 )
        offByRatio = m_defaultOffByRatio;
    const double dvCorrectionAtmosphere = offByRatio * dvSum; // we will loose some dv due to atmospheric drag
    synchroDVTotal = dvCorrectionAtmosphere;

    if ( ! IsToTarget() )
        lastCalculatedDVSum = dvSum;

    const double & m = vessel->GetMass();
    const Engine & eng = VesselCapabilities().EstimateMainThrustParm(vessel);
    synchroBurnT = SpaceMath().RocketEqnT(synchroDVTotal,m,eng.F,eng.isp);
    synchroBurnT2 = SpaceMath().RocketEqnT(synchroDVTotal/2.0,m,eng.F,eng.isp);
}

Engine PEGDirectAscentSynchro::GetEngineCapabilities(const VESSEL * v) const
{
    VesselCapabilities vc;
    Engine engCurrent = vc.GetMainThrustParm(v);
    return engCurrent;
}

double PEGDirectAscentSynchro::GetDeltaT() const
{
    double deltaT = GetTimeToBurn() - GetBurnT2();
    return deltaT;
}

double PEGDirectAscentSynchro::GetBurnT() const
{
    return synchroBurnT;
}
double PEGDirectAscentSynchro::GetBurnT2() const
{
    return synchroBurnT2;
}

double PEGDirectAscentSynchro::GetTimeToBurn() const
{
    return synchroTimeToBurn;
}

double PEGDirectAscentSynchro::GetTotalDV() const
{
    return synchroDVTotal;
}
