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
// MFDData.h - class holding seperate data of individual vessels
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

#ifndef __MFDDATALAUNCHMFD_H
#define __MFDDATALAUNCHMFD_H

#include "globals.h"
#include "Types.hpp"
#include "BODYPHYS_ORB.h"
#include "TGTPARAM_ORB.h"
#include "LaunchMFD.h"
#include "HUD/FlightDirector.h"
#include "HUD/HUDDrawer.hpp"
#include "PEG/PEGNormal.h"
#include "PEG/PEGDirectAscentStd.h"
#include "PEG/PEGDirectAscentSynchro.h"
#include "PEG/PEGMulti.h"
#include "EnergyPitchProgram.hpp"
#include "ShipVariables.h"
#include "StagingDetector.hpp"
#include "GreatCircle.h"
#include "LaunchCompass.h"

#include <multipleVesselsMFD/MFDData.hpp>
#include <Systems/Point.hpp>

#include "Autopilot/AutopilotType.hpp"
#include "OffPlaneCorrector/OffPlaneCorrector.hpp"
#include "OffPlaneCorrector/OffPlaneCorrectorStandard.hpp"
#include "OffPlaneCorrector/OffPlaneCorrectorDirectAscent.hpp"
#include "Autopilot/PIDAP.hpp"
#include "Autopilot/PitchProgramReader.h"

#include "MovementParams.h"

enum View
{
    AZIMUTH,
    DIRECT_ASCENT,
    COMPASS
};

enum NavMode
{
    NAV_STANDARD,
    NAV_DIRECT_ASCENT
};

/// Class used for storing a given vessel's data
/**
Stores a given vessel's data, that can be retrieved later,
or updated on every MFD refresh, or every simulation step if needed.
*/
class MFDDataLaunchMFD : public EnjoLib::MFDGoodies::MFDData
{
public:
    /// Default constructor
    /** Constructs MFDData object for a given vessel.
    Should only be called from within Plugin::AssociateMFDData().
    \param vessel passed by Plugin::AssociateMFDData()
    */
    MFDDataLaunchMFD (VESSEL * vessel);

    /// Updates the vessel's state
    /** Updates the vessel's state as in for example - position, velocity, reference planet, etc.
    Should be called from MyMFD::Update() and on every simulation step from Plugin::UpdateClient() if necessary.
    This method should update all the required data of the vessel to be able operate properly without
    an opened MFD.
    */
    void Update();
    void ReactOnShipStatus();
    double GetCurrentAlt() const;
    double GetAzimuth() const;
    double GetAutomaticAlt(OBJHANDLE hRef);
    void SwitchAutopilot( AutopilotType type );


    AutopilotType GetAutopilotType() const;
    void MECO();
    void MainEngineOn( double level = 1 );
    double GetYaw();
    void CheckLatitudeVsInclination();
    void UpdateTgtParam();
    void CheckPlanetChange();
    TGTPARAM_ORB & GetTgtParam();
    bool IsInAtmosphere() const;
    double GetTargetPitch();
    double GetTimeSinceLaunch() const;
    bool IsStopConditionPitch() const;
    double GetTMECO() const;
    NavMode GetNavMode() const;
    OffPlaneCorrector * GetOffPlaneCorrector();
    char * GetTargetStr() const;
    const std::string & GetTargetStdStr() const;
    char * GetTargetDisplStr() const;
    const MovementParams & GetMovParams() const;
    void SetTargetStr( const std::string & tgt, const std::string & tgtDispl );
    OBJHANDLE GetTargetBody() const;
    VESSEL * GetTargetVessel() const;
    OBJHANDLE GetTargetHandle() const;
    bool IsTargetSet() const;
	const ShipVariables & GetShipVariables() const;
	ShipVariables & GetShipVariablesRW();
    void ChangeCompassDisplayMode();
    void SetPEGApses();
    void SwitchAzimuth();
    double GetMi() const;

    //bool active;
    bool CutEngines;
    bool launched;
    bool half_ov_reached;
    bool tgt_set;
    bool hud;
	//bool newAtmosphericModel;
    bool drawPitchError;

    double InclinationFactor;
    int AltitudeFactor;
    bool m_ManualAlt;
    double PeA, ApA;
    int AzimuthLock;

    OBJHANDLE hRef;

    TGTPARAM_ORB m_tgtParam;
    BODYPHYS_ORB m_bodyPhys;

    FlightDirector flightDirector;
    PEGNormal peg;
    PEGDirectAscentStd pegDA;
    PEGDirectAscentSynchro pegDAS;
    PEGMulti pegMulti;
    GreatCircle m_greatCircle;
    HUDDrawer m_hudDrawer;

	double start_time;

    View pageView;
    PIDAP m_pidAP;
    PitchProgramReader m_pitchProgram;
    EnergyPitchProgram m_energyProgram;

    LaunchCompass::DisplayMode m_compassDisplayMode;
    bool m_daSynchroOrbit;
    bool m_useOffplaneCorrector;
    bool m_isTgtLowInclination;
    double m_distDA;

private:
    double calculate_azimuth();
    void CalcAzimuthLock();
    void DetectStaging();
    void ReadConfig();

   // TGTPARAM CheckTgtPosition();

    StagingDetector m_stageDetector;
    ShipVariables m_shipVariables;
    OffPlaneCorrectorStandard m_offPlaneCorrStandard;
    OffPlaneCorrectorDirectAscent m_offPlaneCorrDA;
    AutopilotType autopilotType;
    MovementParams m_mov;



    const std::string m_tagPitchProgramEnergy;


    bool m_energyBased;
    bool m_justReachedOrbit;
    bool m_recordTMECO;
    unsigned m_numStage;
    unsigned m_numStageSaved;
    bool m_stageSaved;
    double final_vel_module;
    double m_azimuth;


    std::string strTarget;
    std::string strTargetDispl;
};



#endif
