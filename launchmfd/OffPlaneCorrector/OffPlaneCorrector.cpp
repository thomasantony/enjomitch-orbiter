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
// Authors:
// Steve "agentgonzo" Arch - extending and coding concept
// Szymon "Enjo" Ender - small fixes and extending to direct ascent
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

#include <orbitersdk.h>
#include "OffPlaneCorrector.hpp"
#include "../Types.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include <Orbiter/SpaceMathOrbiter.hpp>

using namespace EnjoLib;

OffPlaneCorrector::OffPlaneCorrector()
: limit(PI / 2.0) // 45 degrees
{
}

// by agentgonzo
double OffPlaneCorrector::Calculate( char * tgt, MFDDataLaunchMFD * data )
{
    double timeToNode = data->GetTgtParam().timeToNode;
    correctionAngle = 0;
    // A perfect ascent can be achieved by trying to keep the time to the next node (on the align planes MFD)
    // to be half the time to MECO. This will result in a 0.00° RInc pair of orbits.
    isValid = true;


    OBJHANDLE hTarget = oapiGetObjectByName(tgt);
    if(!hTarget)
    {
        isValid = false;	// no target
        return 0;
    }

    correctionAngle = CalculateClient( timeToNode, data );
    return correctionAngle;
}

double OffPlaneCorrector::GetCorrectionTimePositive( double tMECO, double timeToNode, MFDDataLaunchMFD * data, double limit )
{
    // The time to the node should be half tMECO.
    // To incrase time to node, turn right, to decrease, turn left
    const double tError = tMECO / 2 - fabs(timeToNode);
    if(fabs(tError) > tMECO)
    {
        isValid = false;
        return 0; // we are too far out. Return 0 as you won't be able to get back on plane.
    }
    double adjust = tError / tMECO;
    if(adjust > limit)
        adjust = limit;
    else if(adjust < -limit)
        adjust = -limit;

    adjust = AdjustSilencer( adjust, data );

    return adjust;
}

double OffPlaneCorrector::AdjustSilencer( double adjust, MFDDataLaunchMFD * data ) const
{
    const double rinc = SpaceMathOrbiter().GetPlaneAngle(data->GetVessel(), data->GetTargetHandle());
    // Below this threshold we will silence the adjust in a continuous way.
    const double rincThreshold = 0.10 * RAD;
    double rincRatio = rinc / rincThreshold;
    if (rincRatio > 1)
        rincRatio = 1; // the rinc is above the threshold

    const double silencedAdjust = adjust * rincRatio;
    return silencedAdjust;
}

double OffPlaneCorrector::GetCorrectionAngle() const
{
    return correctionAngle;
}
bool OffPlaneCorrector::IsValid() const
{
    return isValid;
}
