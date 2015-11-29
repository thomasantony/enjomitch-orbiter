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

#include "AutopilotDirectAscentStd.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include <Orbiter/VesselCapabilities.hpp>
#include "Math/Constants.hpp"
#include "../PEG/PEGDirectAscentOpti.h"
#include "../localisation.h"
#include <Math/Root/BinSearchArg.hpp>
#include "DirectAscentOptiEngineLevel.hpp"

using namespace EnjoLib;

AutopilotDirectAscentStd::AutopilotDirectAscentStd()
{}

AutopilotDirectAscentStd::~AutopilotDirectAscentStd()
{}

std::string AutopilotDirectAscentStd::GetIdentifier() const
{
    return AUTOPILOT_DA;
}

bool AutopilotDirectAscentStd::Guide( MFDDataLaunchMFD * data, double dt ) const
{
    if ( ! AutopilotBase::Guide( data, dt ) )
        return false;
    if ( ! data->GetTargetHandle() )
        return false;


    data->pegDA.SetApses(data->PeA, data->ApA);
    data->pegDA.Update( data );



    if ( data->pegDA.IsToTarget() )
    {
        VESSEL * v = data->GetVessel();
        THGROUP_HANDLE mainThrusters = VesselCapabilities().GetMainEnginesHandle(v);
        // Returns optimal engine level for which the DeltaT is near 0
        // Tests the PEGDirectAscent blackbox to find an argument - engine level,
        // which makes the blackbox return a value of 0 - DeltaT - diff between ship MECO
        // and target arrival time at ship's MECO position.
        // To achieve this, it uses a binary search.
        DirectAscentOptiEngineLevel directAscentOptiEngineLevel(data);
        Result<double> optiEngineLevel = BinSearchArg(0, 1, 0.00002).Run(directAscentOptiEngineLevel);
        //sprintf( oapiDebugString(), "tmeco = %.2lf, status = %d",data->pegDA.GetTMECO(), optiEngineLevel.status );
        if ( ! optiEngineLevel.isSuccess ) // invalid output
            v->SetThrusterGroupLevel(mainThrusters, 1); // max power in hope for future guidance convergence
        else // all is fine! Go with engine convergence
        {
            double engineLevel = v->GetThrusterGroupLevel(mainThrusters);
            const double x = optiEngineLevel.value - engineLevel;
            const double y = -data->m_pidAP.m_pidEngDA.Update(x, dt);
            engineLevel -= y;
            v->SetThrusterGroupLevel(mainThrusters, engineLevel);
        }
    }
    else if ( data->pegDA.GetDeltaT() < 0 && data->m_pidAP.prevDeltaT > 0 // sign changed
              && data->pegDA.GetDeltaT() > - 10) // and not because the distance was 180 deg (a huge negative number)
    {
        data->pegDA.SetToTarget(true);
        data->drawPitchError = true;
    }

    data->m_pidAP.prevDeltaT = data->pegDA.GetDeltaT();
    return true;
}

bool AutopilotDirectAscentStd::NeedPitch( MFDDataLaunchMFD * data ) const
{
    return data->pegDA.IsToTarget();
}

bool AutopilotDirectAscentStd::NeedPullUp( MFDDataLaunchMFD * data ) const
{
    return data->pegDA.IsToTarget();
}

bool AutopilotDirectAscentStd::NeedYaw( MFDDataLaunchMFD * data ) const
{
    return true;
}

bool AutopilotDirectAscentStd::NeedMECO( MFDDataLaunchMFD * data ) const
{
    return true;
}

bool AutopilotDirectAscentStd::TerminationRequested( MFDDataLaunchMFD * data ) const
{
    return false;
}
