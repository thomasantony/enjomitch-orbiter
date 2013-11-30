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

#ifndef PIDAP_HPP_INCLUDED
#define PIDAP_HPP_INCLUDED

#include <orbitersdk.h>
#include "../../lib/PID/PID.hpp"
#include "../../lib/Systems/Vect3.hpp"

class PIDAP
{
public:
    PIDAP( const VESSEL * vessel );
    virtual ~PIDAP();
	void Reset();
	VECTOR3 GetVesselAngularAccelerationRatio( const VESSEL * vessel );

    EnjoLib::PID m_pidAPSpaceX;
    EnjoLib::PID m_pidAPSpaceY;
    EnjoLib::PID m_pidAPSpaceBank;

    EnjoLib::PID m_pidAPAtmX;
    EnjoLib::PID m_pidAPAtmY;
    EnjoLib::PID m_pidAPAtmBankTarget;
    EnjoLib::PID m_pidAPAtmBankWingsLevel;

    EnjoLib::PID m_pidEngDA; // Engine PID

    void SetSpaceXY( const EnjoLib::PID & pid );
    void SetSpaceBank( const EnjoLib::PID & pid );
    void SetAtmoXY( const EnjoLib::PID & pid );
    void SetAtmoBank( const EnjoLib::PID & pid );

    double GetReferenceKdForAtmo() const;
    const double GetAtmXYContinuousControlSwitch() const;
    const double GetAtmBankTargetWindsLevelSwitch() const;
    double GetRollAlt() const;
    bool IsUpsideDown() const;
    bool IsRollNeeded() const;
    void SaveConfig( FILEHANDLE fhVar ) const;

    bool m_prevNeededPitch;
    // For direct ascent
    double prevDeltaT;
    double timeOfSynchroBurn;
    double dvReallyUsedSynchro;

private:
    void ReadVesselPIDConfig( const VESSEL * vessel );
    EnjoLib::PID CreatePID( const VECTOR3 & vect ) const;
    VECTOR3 PID2Vector( const EnjoLib::PID & pid ) const;

    double m_refKdAtmo; // maximal Kd in thin atmosphere
    bool m_rotationRatioNeeded;
    double m_atmXYContinuousControlSwitch;
    double m_atmBankTargetWindsLevelSwitch;
    double m_rollAlt;
    bool m_upsideDown; // Implies roll
    bool m_rollNeeded;



	const static EnjoLib::Vect3 m_statDeltaGliderRefRotAcc; // reference pitch, yaw and bank angular accelerations
};

#endif // PIDAP_HPP_INCLUDED
