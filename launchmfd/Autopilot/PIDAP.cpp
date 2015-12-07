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
#include "PIDAP.hpp"
#include <Orbiter/SystemsConverterOrbiter.hpp>
#include <Orbiter/AngularAcc.hpp>
#include <Utils/FileUtilsOrbiter.hpp>

using namespace EnjoLib;
// Experimentally obtained values for all axes
const Vect3 PIDAP::m_statDeltaGliderRefRotAcc(0.125, 0.066, 0.189);

PIDAP::PIDAP( const VESSEL * vessel )
    : m_pidAPSpaceX(2.5, 16)
    , m_pidAPSpaceY(m_pidAPSpaceX)
    , m_pidAPSpaceBank(1, 12)

    , m_refKdAtmo(2.5)
    , m_pidAPAtmX(2)
    , m_pidAPAtmY(m_pidAPAtmX)
    , m_pidAPAtmBankTarget(3, 6)
    , m_pidAPAtmBankWingsLevel(m_pidAPAtmBankTarget)
    , m_atmXYContinuousControlSwitch(10.0 / 180.0)
    , m_atmBankTargetWindsLevelSwitch(20.0 / 180.0)
    , m_pidEngDA(0.1)
{
    m_rotationRatioNeeded = true;
    m_upsideDown = m_rollNeeded = false;
    m_rollAlt = 0;
    ReadVesselPIDConfig( vessel );

    Reset();
}

PIDAP::~PIDAP() {}

void PIDAP::Reset()
{
    m_pidAPSpaceX.Reset();
    m_pidAPSpaceY.Reset();
    m_pidAPSpaceBank.Reset();

    m_pidAPAtmX.Reset();
    m_pidAPAtmY.Reset();
    m_pidAPAtmBankTarget.Reset();
    m_pidAPAtmBankWingsLevel.Reset();

    m_prevNeededPitch = false;
    // DA vars
    prevDeltaT = 0;
    timeOfSynchroBurn = 0;
    dvReallyUsedSynchro = 0;
}

void PIDAP::SetSpaceXY( const PID & pid )
{
    m_pidAPSpaceX = pid;
    m_pidAPSpaceY = pid;
    m_rotationRatioNeeded = false;
}
void PIDAP::SetSpaceBank( const PID & pid )
{
    m_pidAPSpaceBank = pid;
    m_rotationRatioNeeded = false;
}
void PIDAP::SetAtmoXY( const PID & pid )
{
    m_pidAPAtmX = m_pidAPAtmY = pid;
    m_refKdAtmo = pid.GetKd();
    m_rotationRatioNeeded = false;
}
void PIDAP::SetAtmoBank( const PID & pid )
{
    m_pidAPAtmBankTarget = m_pidAPAtmBankWingsLevel = pid;
    m_rotationRatioNeeded = false;
}

VECTOR3 PIDAP::GetVesselAngularAccelerationRatio( const VESSEL * vessel )
{
    VECTOR3 accRatio;
    if ( m_rotationRatioNeeded )
    {
        VECTOR3 refMaxAngAcc = SystemsConverterOrbiter(m_statDeltaGliderRefRotAcc).GetVECTOR3();
        VECTOR3 currMaxAngAcc = AngularAcc().GetMaxAngAcc( vessel );
        accRatio.x = refMaxAngAcc.x / currMaxAngAcc.x;
        accRatio.y = refMaxAngAcc.y / currMaxAngAcc.y;
        accRatio.z = refMaxAngAcc.z / currMaxAngAcc.z;
    }
    else
    {
        accRatio.x = accRatio.y = accRatio.z = 1;
    }

    return accRatio;
}

void PIDAP::ReadVesselPIDConfig( const VESSEL * vessel )
{
    // Searching for vessel's corresponding PID config file
    std::string configFile = FileUtilsOrbiter().GetVesselConfigFileName(vessel);
    FILEHANDLE fh = oapiOpenFile (configFile.c_str(), FILE_IN);
    if (! fh)
    {
        return;
    }
    VECTOR3 vecRead;
    if( oapiReadItem_vec( fh, "SPACE_XY", vecRead ) )
        SetSpaceXY( CreatePID( vecRead ) );

    if( oapiReadItem_vec( fh, "SPACE_BANK", vecRead ) )
        SetSpaceBank( CreatePID( vecRead ) );

    if( oapiReadItem_vec( fh, "ATMO_XY", vecRead ) )
        SetAtmoXY ( CreatePID( vecRead ) );

    if( oapiReadItem_vec( fh, "ATMO_BANK", vecRead ) )
        SetAtmoBank( CreatePID( vecRead ) );

    double dRead;
    if ( oapiReadItem_float( fh, "ATMO_CONTINUOUS_CONTROL_SWITCH_ANGLE", dRead ) )
        m_atmXYContinuousControlSwitch = dRead / 180.0;
    if ( oapiReadItem_float( fh, "ATMO_BANK_TARGET_WINGS_LEVEL_SWITCH_ANGLE", dRead ) )
        m_atmBankTargetWindsLevelSwitch = dRead / 180.0;

    oapiReadItem_bool( fh, "ROLL_NEEDED", m_rollNeeded );
    oapiReadItem_bool( fh, "UPSIDE_DOWN", m_upsideDown );
    if ( m_upsideDown )
        m_rollNeeded = true; // implies roll

    oapiReadItem_float( fh, "ROLL_ALT", m_rollAlt );
    if ( m_rollAlt > 0 )
        m_rollNeeded = true; // implies roll
}

void PIDAP::SaveConfig( FILEHANDLE fh ) const
{
    oapiWriteItem_vec( fh, "SPACE_XY", PID2Vector(m_pidAPSpaceX) );
    oapiWriteItem_vec( fh, "SPACE_BANK", PID2Vector(m_pidAPSpaceBank) );
    oapiWriteItem_vec( fh, "ATMO_XY", PID2Vector(m_pidAPAtmX) );
    oapiWriteItem_vec( fh, "ATMO_BANK", PID2Vector(m_pidAPAtmBankTarget) );
}

PID PIDAP::CreatePID( const VECTOR3 & vect ) const
{
    return PID( vect.x, vect.y, vect.z );
}

VECTOR3 PIDAP::PID2Vector( const PID & pid ) const
{
    return _V( pid.GetKp(), pid.GetKd(), pid.GetKi() );
}

double PIDAP::GetReferenceKdForAtmo() const
{
    return m_refKdAtmo;
}

const double PIDAP::GetAtmXYContinuousControlSwitch() const
{
    return m_atmXYContinuousControlSwitch;
}

const double PIDAP::GetAtmBankTargetWindsLevelSwitch() const
{
    return m_atmBankTargetWindsLevelSwitch;
}

bool PIDAP::IsUpsideDown() const
{
    return m_upsideDown;
}

bool PIDAP::IsRollNeeded() const
{
    return m_rollNeeded;
}

double PIDAP::GetRollAlt() const
{
    return m_rollAlt;
}

