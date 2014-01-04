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
// MFDDataLaunchMFD.cpp - class holding seperate data of individual vessels
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


#include <Orbitersdk.h>
#include <fstream>
#include "MFDDataLaunchMFD.hpp"
#include "localisation.h"
#include "Math/SpaceMath.hpp"
#include "Math/SpaceMathBody.hpp"
#include "Math/AzimuthMath.hpp"
#include "Math/VectorMath.hpp"
#include "Math/GeneralMath.hpp"
#include <Orbiter/SystemsConverterOrbiter.hpp>
#include "Systems/Geo.hpp"
#include "Utils/FileUtilsOrbiter.hpp"
#include <Orbiter/VesselCapabilities.hpp>
#include <Orbiter/SpaceMathOrbiter.hpp>
#include "Utils/Targeting.hpp"

using namespace EnjoLib;

MFDDataLaunchMFD::MFDDataLaunchMFD (VESSEL * vessel)
    : MFDData(vessel)
    , pageView( AZIMUTH )
    , autopilotType( AP_NONE )
    , InclinationFactor( 0.5 )
    , AltitudeFactor( 10 )
    , m_ManualAlt( false )
    , CutEngines( false )
    , half_ov_reached( false )
    , AzimuthLock( BOTH_AZIMUTHS )
    , start_time( 0 )
    , m_numStage( 0 )
    , tgt_set( false )
    , hud( true )
    , drawPitchError( true )
    , m_daSynchroOrbit( false )
    , m_useOffplaneCorrector( false )
    , m_isTgtLowInclination( false )
    , m_justReachedOrbit( false )
    , m_compassDisplayMode( LaunchCompass::NorthUp )
    , m_pidAP( vessel )
    , peg( vessel )
    , m_pitchProgram( vessel )
    , m_stageDetector( vessel )
    , m_shipVariables( vessel, m_pidAP )
    , m_tagPitchProgramEnergy( "PITCH_PROGRAM_ENERGY_BASED" )
    , pegDA( vessel, GetShipVariables() )
    , pegDAS( vessel, GetShipVariables() )
    , pegMulti( vessel )
    , m_mov( vessel )
    , m_tgtParam( m_mov.m_lat + SMALL_DOUBLE )
{
    m_hudDrawer.SetMFDData( this );
    hRef = NULL;
    //if ( IsValid() )
    {
        SetTargetStr( MANUAL, MANUAL );
        ReadConfig();
        flightDirector.SetTailSitter(VesselCapabilities().IsTailSitter(vessel));

        Update();
        double alt = GetAutomaticAlt(hRef);
        if (alt != 0)
            PeA = ApA = alt;
        else
            PeA = ApA = 0;
        m_tgtParam.timeToNode = 0;
    }
}

void MFDDataLaunchMFD::Update()
{
    // check if a planet has changed. If so, reset some parameters
    CheckPlanetChange();
    // Check velocity and position
    m_mov.Update( GetVessel() );
    // if the ship's latitude has exceeded target inclination, reset the target inclunation
    CheckLatitudeVsInclination();
    // get realtime target's orbital inclination and other parameters defined in TGTPARAM
    UpdateTgtParam();
    // Things that we should do if we launch or land
    ReactOnShipStatus();

    DetectStaging();

    m_azimuth = calculate_azimuth();

    //sprintf(oapiDebugString(), "%d", m_shipVariables.m_stagingContainer.GetTMECOs().size());
}

double MFDDataLaunchMFD::GetAzimuth() const
{
    return m_azimuth;
}

void MFDDataLaunchMFD::ReadConfig()
{
    m_energyBased = false;
    std::string fname = FileUtilsOrbiter().GetVesselConfigFileName(GetVessel());
    FileAccessMode famode = FILE_IN;
    FILEHANDLE fh = oapiOpenFile( fname.c_str(), famode );
    if ( fh )
    {
        oapiReadItem_bool( fh, (char *)m_tagPitchProgramEnergy.c_str(), m_energyBased );
        oapiCloseFile(fh, famode);
    }
    m_recordTMECO = m_energyBased || m_pitchProgram.HasProgram( GetVessel() );
}

char * MFDDataLaunchMFD::GetTargetStr() const
{
    return (char*)strTarget.c_str();
}

char * MFDDataLaunchMFD::GetTargetDisplStr() const
{
    return (char*)strTargetDispl.c_str();
}

void MFDDataLaunchMFD::SetTargetStr( const std::string & tgt, const std::string & tgtDispl )
{
    strTarget = tgt;
    strTargetDispl = tgtDispl;
    m_distDA = 0;
}

const std::string & MFDDataLaunchMFD::GetTargetStdStr() const
{
    return strTarget;
}

OBJHANDLE MFDDataLaunchMFD::GetTargetBody() const
{
    return Targeting().GetTargetBody( strTarget );
}

OBJHANDLE MFDDataLaunchMFD::GetTargetHandle() const
{
    VESSEL * v = GetTargetVessel();
    if ( v )
        return v->GetHandle();
    else
        return GetTargetBody();
}

VESSEL * MFDDataLaunchMFD::GetTargetVessel() const
{
    return Targeting().GetTargetVessel( strTarget );
}

bool MFDDataLaunchMFD::IsTargetSet() const
{
    return GetTargetBody() || GetTargetVessel();
}

void MFDDataLaunchMFD::DetectStaging()
{
    if ( ! m_recordTMECO )
        return;

    if ( m_stageDetector.Update( GetVessel() ) )
    {
        m_shipVariables.m_stagingContainer.AddStage( Stage(GetTimeSinceLaunch(), GetVessel()) );
        ++m_numStage;
    }
//    else if ( launched && ! m_justReachedOrbit && IsStopConditionPitch() )
//    {
//        m_justReachedOrbit = true;
//        m_shipVariables.m_stagingContainer.AddStage( Stage(GetTimeSinceLaunch(), GetVessel()) );
//        ++m_numStage;
//    }

    //sprintf(oapiDebugString(), "%d", m_shipVariables.m_stagingContainer.GetTMECOs().size());

//    peg.SetTMECOLaterStages(m_stageContainer.GetTMECORemaining( m_numStage ));
}

double MFDDataLaunchMFD::GetTMECO() const
{
    double TMECO;
    const double staging = m_shipVariables.m_stagingContainer.GetTMECORemaining( m_numStage );
    if ( staging != 0 )
        TMECO = staging - GetTimeSinceLaunch();
    else
        TMECO = peg.GetTMECO();

    return TMECO;
}

double MFDDataLaunchMFD::GetCurrentAlt() const
{
    return m_mov.m_rad - m_bodyPhys.radius;
}

AutopilotType MFDDataLaunchMFD::GetAutopilotType() const
{
    return autopilotType;
}

double MFDDataLaunchMFD::GetAutomaticAlt(OBJHANDLE hRef)
{
    double alt = 0;
    if (oapiPlanetHasAtmosphere(hRef))
    {
        const ATMCONST * atm = oapiGetPlanetAtmConstants(hRef);
        if (atm)
        {
#ifdef ORB2009
            alt = 0.03453 * oapiGetSize(hRef); // gives 220 km for Earth
#else
            // old atmospheric model
            // 20 km above the atmosphere
            alt = atm->altlimit + 20e3;
#endif
        }
    }
    else
    {
        // if the body has no atmosphere then the default target alt is 50 km above the surface
        alt = 50e3;
    }
    return alt;
}

void MFDDataLaunchMFD::SwitchAutopilot( AutopilotType type )
{
    if (autopilotType != AP_NONE )
        // Deactivate, no matter what was on
        autopilotType = AP_NONE;
    else
        autopilotType = type;

    GetVessel()->DeactivateNavmode( NAVMODE_PROGRADE );
    GetVessel()->DeactivateNavmode( NAVMODE_RETROGRADE );
    GetVessel()->DeactivateNavmode( NAVMODE_NORMAL );
    GetVessel()->DeactivateNavmode( NAVMODE_ANTINORMAL );
    GetVessel()->DeactivateNavmode( NAVMODE_HLEVEL );

    if ( autopilotType == AP_NONE )
    {
        m_pidAP.Reset();
        // Disable automatic control
        GetVessel()->SetAttitudeRotLevel ( _V(0, 0, 0) );
        GetVessel()->ActivateNavmode( NAVMODE_KILLROT );

        GetVessel()->SetControlSurfaceLevel( AIRCTRL_RUDDERTRIM, 0 );
        GetVessel()->SetControlSurfaceLevel( AIRCTRL_AILERON, 0 );
        if ( drawPitchError ) // Only reset trim when using pitch guidance
            GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATORTRIM, 0 );
    }
    else
    {
        // Let the AP take control and don't stop it
        GetVessel()->DeactivateNavmode( NAVMODE_KILLROT );
        if ( autopilotType == AP_DIRECT_ASCENT_STD
            || autopilotType == AP_DIRECT_ASCENT_SYNCHRO )
        {
            drawPitchError = false;
        }
    }
}

void MFDDataLaunchMFD::UpdateTgtParam()
{
    m_tgtParam = TGTPARAM_ORB( this, m_tgtParam.incl );
    CalcAzimuthLock();
    final_vel_module = sqrt( GetMi() / m_tgtParam.rad);
    if ( GetAutopilotType() == AP_DIRECT_ASCENT_STD )
    {
        PeA = ApA = m_tgtParam.rad - m_bodyPhys.radius + m_distDA;
    }
}

// Azimuth lock prevents from azimuth switching when you're in flight and you pass the "launch window"
void MFDDataLaunchMFD::CalcAzimuthLock( )
{
    if ( ! launched ) AzimuthLock = BOTH_AZIMUTHS;
    else
    {
        if (AzimuthLock == NORTHERN_AZIMUTH)
            m_tgtParam.azFlag = NORTHERN_AZIMUTH;
        else if (AzimuthLock == SOUTHERN_AZIMUTH)
            m_tgtParam.azFlag = SOUTHERN_AZIMUTH;
        else // AzimuthLock == BOTH_AZIMUTHS
        {
            if (m_tgtParam.azFlag == NORTHERN_AZIMUTH)
                AzimuthLock = NORTHERN_AZIMUTH;
            else if (m_tgtParam.azFlag == SOUTHERN_AZIMUTH)
                AzimuthLock = SOUTHERN_AZIMUTH;
        }
    }
}

void MFDDataLaunchMFD::SwitchAzimuth()
{
    if (AzimuthLock == BOTH_AZIMUTHS)
        AzimuthLock = NORTHERN_AZIMUTH;
    else if (AzimuthLock == NORTHERN_AZIMUTH)
        AzimuthLock = SOUTHERN_AZIMUTH;
    else if (AzimuthLock == SOUTHERN_AZIMUTH)
        AzimuthLock = BOTH_AZIMUTHS;
}

const MovementParams & MFDDataLaunchMFD::GetMovParams() const
{
    return m_mov;
}


TGTPARAM_ORB & MFDDataLaunchMFD::GetTgtParam()
{
    return m_tgtParam;
}

double MFDDataLaunchMFD::GetMi() const
{
    return SpaceMathBody(m_bodyPhys.mass).GetMi();
}

// by Enjo and agentgonzo
double MFDDataLaunchMFD::calculate_azimuth()
{
    const double & target_radius = m_mov.m_rad;
    const double mi = GetMi(); //gravitational parameter
    const double & incl = m_tgtParam.incl;
    double true_azimuth = AzimuthMath().CalcTrueAzimuth(m_mov.m_lat, incl, m_mov.current_vel, mi, target_radius);

    if(m_useOffplaneCorrector)
    {
        GetOffPlaneCorrector()->Calculate( GetTargetStr(), this );
        if(GetOffPlaneCorrector()->IsValid())
        {
            true_azimuth -= GetOffPlaneCorrector()->GetCorrectionAngle();
        }
    }

    return GeneralMath().GetIn0_2PIRange(true_azimuth);
}

void MFDDataLaunchMFD::MECO()
{
    MainEngineOn(0);
}

void MFDDataLaunchMFD::MainEngineOn( double level )
{
    THGROUP_HANDLE h = VesselCapabilities().GetMainEnginesHandle(GetVessel());
    if ( h != NULL )
    {
        if ( level > 1 )
            level = 1;
        else if ( level < 0 )
            level = 0;
        GetVessel()->SetThrusterGroupLevel( h, level );
    }
}

double MFDDataLaunchMFD::GetYaw()
{
    double heading = 0;
#ifdef ORB2006
    heading = m_mov.vesselStatus.surf_hdg;
#else
    heading = GetVessel()->GetYaw();
#endif

    return heading;
}

// if the ship's latitude has exceeded target inclination, reset the target inclunation
void MFDDataLaunchMFD::CheckLatitudeVsInclination()
{
    if (fabs(m_mov.m_lat) > m_tgtParam.incl)
    {
        // Going beyond minimal prograde inclination
        m_tgtParam.incl = fabs(m_mov.m_lat) + SMALL_DOUBLE;
        tgt_set = false;
        SetTargetStr( MANUAL, MANUAL );
    }
    if ( PI - fabs(m_mov.m_lat) < m_tgtParam.incl)
    {
        // Going beyond minimal retorgrade inclination
        m_tgtParam.incl = PI - fabs(m_mov.m_lat) - SMALL_DOUBLE;
        tgt_set = false;
        SetTargetStr( MANUAL, MANUAL );
    }
}

// check if a planet has changed. If so, reset some parameters
void MFDDataLaunchMFD::CheckPlanetChange()
{
    OBJHANDLE currentRef = GetVessel()->GetSurfaceRef();
    if ( hRef == currentRef )
        return;

    hRef = currentRef;
    m_bodyPhys = BODYPHYS_ORB(hRef);
    m_greatCircle.SignalPlanetChange(m_bodyPhys);
}

///
void MFDDataLaunchMFD::ReactOnShipStatus()
{
    const int status = m_mov.vesselStatus.status;

    if (status == 0) // If we're in flight
    {
        if ( ! launched ) // Just launched
        {
            launched = true;
            start_time = oapiGetSimTime();
            //newAtmosphericModel = VesselCapabilities().CheckVesselAirControlSurfaces( GetVessel() ); // buggy
        }
        else // was in flight before
            if ( m_mov.current_vel_module > final_vel_module/2 && ! half_ov_reached )
            {
                // if half orbital vel is reached
                m_shipVariables.m_next_launch_time = oapiGetSimTime() - start_time;
                half_ov_reached = true;
            }
    }
    else // if landed
    {
        start_time = oapiGetSimTime();
        CutEngines  = false;
        launched = false;
        half_ov_reached = false;
        //AzimuthLock = BOTH_AZIMUTHS;
        pegDA.Reset();

        m_numStageSaved = 0;
        m_stageSaved = false;
    }
}

bool MFDDataLaunchMFD::IsInAtmosphere() const
{
    return GetVessel()->GetDynPressure() > 500;
}

double MFDDataLaunchMFD::GetTimeSinceLaunch() const
{
    return oapiGetSimTime() - start_time;
}

void MFDDataLaunchMFD::SetPEGApses()
{
    peg.SetApses(PeA, ApA);
    pegMulti.SetApses(PeA, ApA);
}

double MFDDataLaunchMFD::GetTargetPitch()
{
    const std::vector<Stage> & stages = m_shipVariables.m_stagingContainer.GetStages();
    bool useMultiPEG = m_numStage < stages.size(); // There are some stages and we're inside one of them
    useMultiPEG = false; // override
    double targetPitch;
    if ( m_energyBased )
        targetPitch = m_energyProgram.GetPitch(this, GetTimeSinceLaunch());
    else
        targetPitch = m_pitchProgram.GetPitch(this, GetTimeSinceLaunch());

    if ( targetPitch == 0 ) // no readout from pitch programs
    {
        // Since pitch program has ended, now it's time for PEG to kick into action
        bool pegValid = true;
        if ( useMultiPEG )
        {
            if ( ! m_stageSaved )
            {
                m_stageSaved = true;
                m_numStageSaved = m_numStage;
                pegMulti.SetStage( stages.at(m_numStage), GetTimeSinceLaunch() );
            }
            if ( m_numStageSaved == m_numStage ) // Still in the saved stage
            {
                targetPitch = pegMulti.GetTargetPitch(GetTimeSinceLaunch());
                pegValid = pegMulti.IsValid();
                sprintf(oapiDebugString(), "MULTI");
            }
            else
            {
                targetPitch = peg.GetTargetPitch();
                pegValid = peg.IsValid();
                sprintf(oapiDebugString(), "Not anymore");
            }
        }
        else
        {
            targetPitch = peg.GetTargetPitch();
            pegValid = peg.IsValid();
            // sprintf(oapiDebugString(), "standard");
        }


        if ( ! pegValid || targetPitch > 90 * RAD )
        {
            targetPitch = 80 * RAD;
        }
        else if ( GetCurrentAlt() < 60e3 && targetPitch < 20 * RAD )
        {
            // prevent diving
            targetPitch = 80 * RAD;
        }
    }
    return targetPitch;
}

bool MFDDataLaunchMFD::IsStopConditionPitch() const
{
    if ( m_energyBased )
        return m_energyProgram.IsStopCondition( this ) || peg.GetTMECO() <= 0;
    else
        return peg.GetTMECO() <= 0;
}

NavMode MFDDataLaunchMFD::GetNavMode() const
{
    AutopilotType apType = GetAutopilotType();
    if ( apType == AP_NONE )
    {
        switch ( pageView )
        {
        case AZIMUTH:
        case COMPASS:
            return NAV_STANDARD;
        case DIRECT_ASCENT:
            return NAV_DIRECT_ASCENT;
        }
    }
    else
    {
        if ( apType == AP_DIRECT_ASCENT_STD )
            return NAV_DIRECT_ASCENT;
    }
    return NAV_STANDARD;
}

OffPlaneCorrector * MFDDataLaunchMFD::GetOffPlaneCorrector()
{
    switch ( GetNavMode() )
    {
    case NAV_STANDARD:
        return & m_offPlaneCorrStandard;
    case NAV_DIRECT_ASCENT:
        return & m_offPlaneCorrDA;
    }
    return NULL;
}

void MFDDataLaunchMFD::ChangeCompassDisplayMode()
{
    if(m_compassDisplayMode == LaunchCompass::HeadingUp)
        m_compassDisplayMode = LaunchCompass::NorthUp;
    else
    {
        int *i = (int*)&m_compassDisplayMode;
        (*i)++;
    }
}

const ShipVariables & MFDDataLaunchMFD::GetShipVariables() const
{
    return m_shipVariables;
}

ShipVariables & MFDDataLaunchMFD::GetShipVariablesRW()
{
    return m_shipVariables;
}
