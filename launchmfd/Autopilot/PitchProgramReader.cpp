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

#include <sstream>
#include <algorithm>
#include <Utils/FileUtilsOrbiter.hpp>
#include "Util/FileUtils.hpp"
#include "Math/GeneralMath.hpp"
#include "PitchProgramReader.h"
#include "PitchPointComparator.h"
#include "../MFDDataLaunchMFD.hpp"

using namespace EnjoLib;

PitchProgramReader::PitchProgramReader( const VESSEL * v )
{
    FileUtilsOrbiter fuo;
    FileUtils fu;
    std::string fileName = fuo.GetVesselConfigFileName(v);
    std::vector<std::vector<std::string> > sectionsTime = fu.GetConfigSections(fileName,
            "PITCH_PROGRAM_TIME", "END_PITCH_PROGRAM_TIME");
    std::vector<std::vector<std::string> > sectionsAlt = fu.GetConfigSections(fileName,
            "PITCH_PROGRAM_ALTITUDE", "END_PITCH_PROGRAM_ALTITUDE");
    ParseSections( sectionsTime, true );
    ParseSections( sectionsAlt, false );

//    m_programmesEnergy["Earth"] = programme;
}

PitchProgramReader::~PitchProgramReader()
{}

bool PitchProgramReader::HasProgram( const VESSEL * vessel ) const
{
    std::string name = GetPlanetName( vessel );
    std::map<std::string, std::vector< Point > >::const_iterator it;
    it = m_programmesTime.find(name); // Try to find time profiles as they are preferred
    if ( it == m_programmesTime.end() )
    {
        it = m_programmesAlt.find(name); // if not found, maybe there's alt program available
        if ( it == m_programmesAlt.end() )
        {
            // No programme found at all
            return false;
        }
    }
    return true;
}

std::string PitchProgramReader::GetPlanetName( const VESSEL * vessel ) const
{
    OBJHANDLE currentRef = vessel->GetSurfaceRef();
    const int len = 1024;
    char name[len];
    oapiGetObjectName(currentRef, name, len);

    return name;
}

double PitchProgramReader::GetPitch( MFDDataLaunchMFD * data, double time ) const
{
    std::string name = GetPlanetName( data->GetVessel() );
    std::map<std::string, std::vector< Point > >::const_iterator it;
    bool timeBased = true;
    it = m_programmesTime.find(name); // Try to find time profiles as they are preferred
    if ( it == m_programmesTime.end() )
    {
        it = m_programmesAlt.find(name); // if not found, maybe there's alt program available
        if ( it == m_programmesAlt.end() )
        {
            // No programme found at all
            return 0;
        }
        else
            timeBased = false;
    }
    double pitchDeg = LinearInterpolationProgramme(it->second, timeBased ? time : data->GetCurrentAlt() );
    double pitchRad = pitchDeg * RAD;
    return pitchRad;
}

double PitchProgramReader::LinearInterpolationProgramme( const std::vector< Point > & programme, double x ) const
{
    if ( programme.empty() )
        return 0;

    if ( x < programme.front().x )
        return programme.front().y;

    if ( x > programme.back().x )
        //return programme.back().y;
        return 0;

    for (size_t i = 1; i < programme.size(); ++i)
    {
        if ( programme[i].x < x )
            continue;
        else
            return GeneralMath().LinearInterpol(x, programme[i-1], programme[i]);
    }

    return 0;
}

void PitchProgramReader::ParseSections( const std::vector<std::vector<std::string> > & sections, bool timeBased)
{
    for (size_t i = 0; i < sections.size(); ++i )
        ParseSection(sections[i], timeBased);
}

void PitchProgramReader::ParseSection( const std::vector<std::string> & section, bool timeBased)
{
    std::vector< Point > programme;
    std::string planetName;
    for (size_t i = 0; i < section.size(); ++i )
    {
        std::istringstream ssLine;
        ssLine.str(section[i]);
        double x, pitch;
        if ( ssLine >> x >> pitch )
            programme.push_back(Point(x * (timeBased ? 1 : 1e3), pitch));
        else if ( i == 0 )
           planetName = ssLine.str();
    }
    std::sort(programme.begin(), programme.end(), PitchPointComparator());

    if ( planetName.empty() )
        planetName = "Earth";

    if ( timeBased )
        m_programmesTime[planetName] = programme;
    else
        m_programmesAlt[planetName] = programme;
}
