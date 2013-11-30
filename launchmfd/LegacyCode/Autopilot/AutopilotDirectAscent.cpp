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

#include "AutopilotDirectAscent.hpp"
#include "../DirectAscent/DirectAscentPhase.hpp"
#include "../MFDDataLaunchMFD.hpp"

bool AutopilotDirectAscent::NeedPitch( MFDDataLaunchMFD * data ) const
{
    return false;
}

bool AutopilotDirectAscent::TerminationRequested( MFDDataLaunchMFD * data ) const
{
    return false;
}

bool AutopilotDirectAscent::NeedYaw( MFDDataLaunchMFD * data ) const
{
    double currentTime = oapiGetSimTime();
    DirectAscentPhase p = data->GetDAResults().GetCurrentPhase(currentTime);

    if ( p == DA_PHASE_ACC_FINAL )
    {
        return true;
    }
    if ( p == DA_PHASE_CONST && data->GetDAResults().GetTimeOfCurrentPhase(currentTime) < 50)
    {
        return true;
    }
    return false;
}

bool AutopilotDirectAscent::NeedMECO( MFDDataLaunchMFD * data ) const
{
    DirectAscentPhase p = data->GetDAResults().GetCurrentPhase(oapiGetSimTime());
    if (  p == DA_PHASE_END )
        return true;

    return false;
}

bool AutopilotDirectAscent::Guide( MFDDataLaunchMFD * data, double dt ) const
{
    if ( ! AutopilotBase::Guide( data, dt ) )
        return false;

    static DirectAscentPhase previousPhase = DA_PHASE_END;
    DirectAscentPhase currentPhase = data->GetDAResults().GetCurrentPhase(oapiGetSimTime());
    if ( previousPhase == DA_PHASE_WAIT && currentPhase == DA_PHASE_ACC )
    {
        data->SetHeading(PI/2.0);
        data->HoverEngineOn();
        data->MainEngineOn();
        data->GetVessel()->ActivateNavmode( NAVMODE_HLEVEL );
    }
    else if ( previousPhase == DA_PHASE_ACC && currentPhase == DA_PHASE_CONST )
    {
        data->MECO();
    }
    else if ( previousPhase == DA_PHASE_CONST && currentPhase == DA_PHASE_ACC_FINAL )
    {
        data->MainEngineOn();
    }
    previousPhase = currentPhase;

    if ( data->m_dRadius > data->GetTgtParam().rad && !data->GetVessel()->GetNavmodeState(NAVMODE_HOLDALT) )
    {
        data->GetVessel()->ActivateNavmode( NAVMODE_HOLDALT );
    }
    return true;
}
