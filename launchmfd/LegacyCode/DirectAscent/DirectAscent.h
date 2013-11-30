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
// DirectAscent.h - algorithms for direct ascent
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

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <utility>

#include "../../BODYPHYS.h"
#include "../../TGTPARAM.h"
#include "SHIP_PARAMS.h"
#include "../../../lib/Systems/Systems.hpp"
#include "Satellite.h"
#include "Ship.h"
#include "Times.h"

class DirectAscent
{

public:
    DirectAscent();
    virtual ~DirectAscent();
    void SetAllVars( const BODYPHYS &, const TGTPARAM &, const SHIP_PARAMS &);
    // call before each new calculation
    void reset_values();

private:

protected:
    EnjoLib::Spherical CalculateSatellitePosition();
    bool IsNodeInFrontOfShip();
    EnjoLib::Vect3 CalcNode( );
    double CalcAngDistToNode( const EnjoLib::Spherical & sph );
    double CalcAngDistStartPosToNode();
    void InitInitialShipPosition( );
    double CalcAngDistInPlane( const EnjoLib::Spherical & shPos, const EnjoLib::Spherical & satPos );
    double CalcTimeToNodeOnGround();

protected:

    virtual bool InitInternalVars() = 0;
    virtual double GetShipsInitialAcceleration() = 0;

    void UpdateGreatCircleShip();
    void UpdateGreatCircleSatellite();
    void UpdatePlaneMomentumShip();
    void UpdatePlaneMomentumSatellite();

    double CalcSatFullRevolutionTime();
    double CalcPlaneRotationLongitude( double waitingTime );


    EnjoLib::Spherical CalcPointOnWholeGreatCircle(double angRatio, double r, double latStart, double lonStart, bool bSatellite = false );

    double CalcAngDistOfPlanes( const EnjoLib::Spherical & sph, EnjoLib::Vect3 v3SatPlaneN );
    double CalculateAsymptore(const EnjoLib::Spherical & sphPos);
    EnjoLib::Vect3 CalcShipVel3D(const EnjoLib::Vect3 & shPos);
    EnjoLib::Point ConvertShipVel3Dto2D(const EnjoLib::Spherical & sphPos, const EnjoLib::Vect3 & shVel3D);
    EnjoLib::Vect3 ConvertShipVel2Dto3D(const EnjoLib::Spherical & sphPos, const EnjoLib::Point & shVel2D);
    EnjoLib::Vect3 Get2dPlaneY();
    EnjoLib::Vect3 Get2dPlaneX(const EnjoLib::Spherical & sphPos);

    BODYPHYS bodyPhys;  // source body's physical parameters
    TGTPARAM tgtParam;  // target's parameters
    SHIP_PARAMS shipParams;

    // loop defining a curved path ( stores it in s_x and s_y vectors )
    // accepts a true azimuth parameter - azimuth needed to reach a certain final velocity depending on current velocity
    //void inner_loop( double true_azimuth = 0);
    void inner_loop( );
    // loop which increments the time of flying and calls inner_loop at each increment
    void medium_loop();
    // preparation of medium_loop for time efficient solution. Call before medium loop
    void main_loop_prepare(bool timeEfficient);
    // preparation of medium_loop for fuel efficient solution. Call before medium loop
    // void main_loop_fuel_efficient_prepare();
    // finalisation of medium_loop. Call after looping the medium_loop
    void main_loop_finalise();

    int main_iter;
    int iter;       // number of medium loop's iterations
    int inner_iter; // number of inner loop's iterations
    static const int max_i;  // maximal number of medium loop's iterations. Helps quiting calculations from a dead loop

    Times m_t;

    bool solNotPossible;        // time efficient solution not possible (from left to right)
    bool fuelEfSolNotPossible;  // fuel efficient solution not possible (from right to left)

    double initial_rotation_vel;
    double initial_rotation_vel_wlat; // velocity resulting from body's rotation, taking latitude into account

    double initialAccel;  // ship's initial acceleration. Acceleration will be changing as ship looses fuel

    double Ldistance;   // Longitudinal difference at y > 0 (final point) between distance covered by accelerating and orbit's plane.
    double angDistance;
    double distanceShipSatFinal;  // 2D final distance between positions of the ship and the sattelite itself

    double xDistance;       // x distance between positions of the ship and the satellite
    double xPrvDistance;

    double mi;              // gravitational parameter  mi = GGRAV * mass_of_body

    Satellite m_satellite;
    double target_radius;   // target sattelite's radius. NOT altitude.
    EnjoLib::Point tgt_orbit_v2_ref; // this is station's velocity which we use as reference
    double tgt_orbit_v2_ref_module;

    Ship m_ship;
    double sshx_excession;
    double dvUsed;
    double sshx_waiting; // the amount of distance you will cover by just waiting, resulting from body's rotation
    double max_xvel;    // maximal eastern velocity, after achieving which, we fly with a constant vel.
    double latStart, lonStart, lonCurrent;

    double debug, debug2, debug3, debug4, debug5;
    bool debugbool;
};

