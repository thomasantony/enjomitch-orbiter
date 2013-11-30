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

#include "StagingContainer.hpp"
#include "Util/CharManipulations.hpp"
using namespace EnjoLib;

StagingContainer::StagingContainer()
: m_tagStageTMECO("STAGE_TMECO")
, m_tagStageACCEL("STAGE_ACCEL")
, m_tagStageISP("STAGE_ISP")
{
}

StagingContainer::~StagingContainer()
{
}

const std::vector<Stage> & StagingContainer::GetStages() const
{
    return m_stagesRead;
}

double StagingContainer::GetTMECORemaining( unsigned currentStage ) const
{
    if ( m_stagesRead.empty() )
        return 0;

    if ( currentStage > m_stagesRead.size() - 1 )
        return 0;

  return m_stagesRead.back().tmeco;
}

void StagingContainer::AddStage( const Stage & stage )
{
    m_stagesNew.push_back( stage );
}

std::string StagingContainer::GetTag(const std::string & tag, int i) const
{
    return tag + "_" + CharManipulations().ToStr(i);
}

void StagingContainer::ReadConfig(FILEHANDLE fh, FILEHANDLE fhVar)
{
	// First look in the static config file, modified only by user
    m_stagesRead = ReadFromFileHandle( fh );
    // if no info found there, look in the variables file
    if ( m_stagesRead.empty() )
        m_stagesRead = ReadFromFileHandle( fhVar );
}

std::vector<Stage> StagingContainer::ReadFromFileHandle( FILEHANDLE fh ) const
{
    std::vector<Stage> stages;
    bool success = true;
    for (int i = 0; success; ++i)
    {
        Stage stage;
        success &= oapiReadItem_float( fh, (char *)GetTag(m_tagStageTMECO, i).c_str(), stage.tmeco );
        success &= oapiReadItem_float( fh, (char *)GetTag(m_tagStageACCEL, i).c_str(), stage.accel );
        success &= oapiReadItem_float( fh, (char *)GetTag(m_tagStageISP, i).c_str(),   stage.isp );
        if ( success )
            stages.push_back( stage );
    }
    return stages;
}

void StagingContainer::SaveConfig(FILEHANDLE fhVar) const
{
    for (size_t i = 0; i < m_stagesNew.size(); ++i )
    {
        oapiWriteItem_float( fhVar, (char *)GetTag(m_tagStageTMECO, i).c_str(), m_stagesNew[i].tmeco );
        oapiWriteItem_float( fhVar, (char *)GetTag(m_tagStageACCEL, i).c_str(), m_stagesNew[i].accel );
        oapiWriteItem_float( fhVar, (char *)GetTag(m_tagStageISP, i).c_str(),   m_stagesNew[i].isp );
    }
}
