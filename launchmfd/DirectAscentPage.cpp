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

#include "DirectAscentPage.hpp"
#include "MFDDataLaunchMFD.hpp"
#include "Util/ValueWithDescription.hpp"
#include <Orbiter/SpaceMathOrbiter.hpp>
#include <MFDTextCalculator.hpp>
#include <MFDSound++/Sound.hpp>
#include "Sound/SoundSampleIDEnum.hpp"
#include "localisation.h"


#include <vector>
using std::vector;
using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

DirectAscentPage::DirectAscentPage(DWORD w, DWORD h, VESSEL *v, EnjoLib::MFDGoodies::Sound & sound )
    : MFDPage (w, h, v)
    , m_sound( sound )
{
}

DirectAscentPage::~DirectAscentPage()
{
}

MFD_RETURN_TYPE DirectAscentPage::Update ( MyDC hDC, MFDDataLaunchMFD * data )
{
    data->pegDA.SetApses(data->PeA, data->ApA);
    data->pegDAS.SetApses(data->PeA, data->ApA);
    data->pegDA.Update(data);
    data->pegDAS.Update(data);

    PrintWords( hDC, PrepareData(data) );
    return MFD_RETURN_VALUE( true );
}

vector< const ValueWithDescription > DirectAscentPage::PrepareData(MFDDataLaunchMFD * data)
{
    vector< const ValueWithDescription > vars;
    if ( ! data->GetTargetHandle() )
        return vars;

    const double planeAngle = SpaceMathOrbiter().GetPlaneAngle( data->GetVessel(), data->GetTargetHandle() );
    vars.push_back( ValueWithDescription(RINC, planeAngle * DEG, "°") );
    vars.push_back( ValueWithDescription(TIME_TO_NODE, data->GetTgtParam().timeToNode, " s") );
    vars.push_back( ValueWithDescription() );
    if ( data->m_daSynchroOrbit )
    {
        vars.push_back( ValueWithDescription(SYNCHRO_ASCENT) );
        vars.push_back( ValueWithDescription(DV_NEEDED, data->pegDAS.GetTotalDV(), " m/s") );
        vars.push_back( ValueWithDescription(T_TO_PERIAPSIS, data->pegDAS.GetTimeToBurn(), " s") );
        vars.push_back( ValueWithDescription(T_TO_BURN, data->pegDAS.GetDeltaT(), " s") );
        if ( data->pegDAS.IsToTarget() )
            vars.push_back( ValueWithDescription( APPROACHING + data->GetTargetStdStr(), true) );
    }
    else
    {
        vars.push_back( ValueWithDescription(DIRECT_ASCENT_STR) );
        vars.push_back( ValueWithDescription("Final dist.", data->m_distDA, " m") );
        vars.push_back( ValueWithDescription(SATELLITE_T, data->pegDA.GetTargetT(), " s") );
        vars.push_back( ValueWithDescription(SHIP_TMECO, data->pegDA.GetTMECO(), " s") );
        vars.push_back( ValueWithDescription("Delta T:", data->pegDA.GetDeltaT(), " s") );
        if ( data->pegDA.IsToTarget() )
        {
            vars.push_back( ValueWithDescription( APPROACHING + data->GetTargetStdStr(), true) );
            if ( ! data->m_daSynchroOrbit && data->peg.IsValid() )
                vars.push_back( ValueWithDescription( GUIDANCE_HAS_CONVERGED, true) );

            m_sound.ResetSoundOnce( BEEP_DA );
        }
        else
        {
            const double & dt = data->pegDA.GetDeltaT();
            if ( 0 < dt && dt < 10 )
                m_sound.PlaySoundOnce( BEEP_DA );
        }
    }
    return vars;
}

void DirectAscentPage::PrintWords(MyDC hDC, const vector< const ValueWithDescription > & vars)
{
    int line_offset = 0;
    int char_offset = 20;
    MFDTextCalculator c(m_W, m_H);

    for ( unsigned i = 0, line_offset = 1; i < vars.size(); i++, line_offset++)
    {
        const ValueWithDescription & var = vars[i];
        int xoffset = 18, tab = 1;

        COLORREF nameColor = YELLOW;
        COLORREF varColor = GREEN;
        if ( var.IsHighlighted() )
            nameColor = LIGHT_BLUE;

        MFDTextOut(hDC, c.X(tab),     c.Y(line_offset), nameColor, var.GetDescr() );
        MFDTextOut(hDC, c.X(xoffset), c.Y(line_offset), varColor, var.GetVal() );
    }
}

