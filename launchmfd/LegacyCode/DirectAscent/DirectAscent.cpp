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
// DirectAscent.cpp - algorithms for direct ascent
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

#include <cmath>

#include "DirectAscent.h"
#include "OptimalVelSolver.hpp"
#include "Systems/SystemsConverter.hpp"
#include "Systems/ThreeSphericals.hpp"
#include "Math/Constants.hpp"
#include "Math/VectorMath.hpp"
#include "Math/AzimuthMath.hpp"
#include "Math/GreatCircleMath.hpp"
#include "Math/GreatCircleAdvMath.hpp"
#include "Math/SpaceMath.hpp"

using namespace EnjoLib;

const int DirectAscent::max_i = 500;

DirectAscent::DirectAscent ()
{
}

DirectAscent::~DirectAscent()
{
}

void DirectAscent::SetAllVars( const BODYPHYS & bodyPhysParam, const TGTPARAM & tgtParamArg,
                               const SHIP_PARAMS & shipParamsArg )
{
    bodyPhys = bodyPhysParam;
    m_ship.sphPos.r = target_radius;
    tgtParam = tgtParamArg;
    shipParams = shipParamsArg;
}

Spherical DirectAscent::CalculateSatellitePosition()
{
    m_satellite.orbitPeriod = CalcSatFullRevolutionTime();
    double stationTime = m_satellite.GetTime( m_t );

    Spherical satSph = CalcPointOnWholeGreatCircle(stationTime/m_satellite.orbitPeriod, target_radius, tgtParam.incl, m_satellite.angEqu, true);
    m_satellite.pos2D = SystemsConverter(satSph).GetPoint();

    return satSph;
}


void DirectAscent::main_loop_prepare(bool timeEfficient)
{
    Ldistance = 1000; // for InnerLoop
    angDistance = 0;

    if (solNotPossible)
    {
        return;
    }

    double dist = distanceShipSatFinal;
    if ( xDistance > 0 )
        dist = -dist;

    double tdiff = dist / tgt_orbit_v2_ref_module;
    if ( m_t.waiting_t + tdiff < 0 )
    {
        tdiff += CalcSatFullRevolutionTime();
    }
    m_t.waiting_t += tdiff;

    double planeRotation = CalcPlaneRotationLongitude(m_t.waiting_t);
    lonCurrent = lonStart + planeRotation;

    /// TODO: update this method
    if ( ! IsNodeInFrontOfShip() )
    {
        solNotPossible = true;
        return;
    }
}

double DirectAscent::CalcAngDistInPlane( const Spherical & shPos, const Spherical & satPos )
{
    Vect3 v3ShipPos = SystemsConverter(shPos).GetVector();
    Vect3 v3SatPos = SystemsConverter(satPos).GetVector();
    double angle = VectorMath().angle(v3ShipPos, v3SatPos);

    return angle;
}

double DirectAscent::CalcSatFullRevolutionTime()
{
    return 2 * PI * target_radius / tgt_orbit_v2_ref_module;
}

double DirectAscent::CalcPlaneRotationLongitude( double waitingTime )
{
    return 2 * PI / bodyPhys.sidDay * waitingTime;
}

void DirectAscent::main_loop_finalise()
{
    double angle = CalcAngDistInPlane( m_ship.sphPos, m_satellite.sphPos );

    distanceShipSatFinal = angle * target_radius; // Longitudinal distance of final orbit planes.
    xPrvDistance = xDistance;
    xDistance = m_satellite.pos2D.x - m_ship.pos2D.x;
    m_t.flight_t = m_t.total_t - m_t.waiting_t;

    ++main_iter;
}

void DirectAscent::InitInitialShipPosition()
{
    double angDistToPass = CalcAngDistStartPosToNode() + angDistance;
    double distToPass = angDistToPass * target_radius;
    double angDistPassedRatio = angDistToPass / (2.0 * PI);
    Spherical shSph = CalcPointOnWholeGreatCircle(angDistPassedRatio, target_radius, latStart, lonCurrent);
    double shAsymptoteAzimuth = CalculateAsymptore( shSph );
    Point shP = SystemsConverter( shSph ).GetPoint();
    m_ship.pos2D_prev.y = shP.y;

    if ( iter < max_i / 3 ) // safety
    {
        double azimuth = AzimuthMath().CalcAzimuth( m_ship.pos2D_prev.y / target_radius, tgtParam.incl );
        OptimalVelSolverParams ovspar;
        ovspar.latStart = latStart;
        ovspar.angDist = angDistToPass;
        ovspar.shAsymptoteAzimuth = shAsymptoteAzimuth;
        ovspar.azimuth = azimuth;
        ovspar.target_radius = target_radius;

        OptimalVelSolver optimalVelSover( bodyPhys, ovspar, shipParams );

        VelIntegral opt = optimalVelSover.CalcFuelOptim();
        max_xvel = opt.vel;
        dvUsed = opt.dv();
        if ( max_xvel >= tgt_orbit_v2_ref_module )
        {
            max_xvel = 0.999 * tgt_orbit_v2_ref_module;
        }
    }
    //max_xvel = 0.5* tgt_orbit_v2_ref_module;     // Override

    m_t.t1_excession = (max_xvel - initial_rotation_vel_wlat) / initialAccel;
    /// sacc pobrac z opt vel solver:
    double sacc = initial_rotation_vel_wlat * m_t.t1_excession + (max_xvel - initial_rotation_vel_wlat) * m_t.t1_excession / 2.0;
    double sconst = distToPass - sacc;
    m_t.t2 = sconst/max_xvel + m_t.t1_excession;

    //sshx = initial_rotation_vel_wlat * t1_excession + (max_xvel - initial_rotation_vel_wlat) *(t1_excession) / 2 + max_xvel * ( t2 - t1_excession );

    m_t.total_t = m_t.waiting_t + m_t.t2;
    m_ship.pos2D = Point( distToPass, 0 );
    m_ship.vel = Point( max_xvel, 0 );
}


void DirectAscent::medium_loop()
{
    InitInitialShipPosition();
    // for inner loop
    m_t.const_flight_t = m_t.total_t - m_t.waiting_t;
    m_t.const_flight_t -= m_t.t1_excession;

    double angDistPassedRatio = m_ship.pos2D.x / (target_radius * 2.0 * PI);
    m_ship.sphPos = CalcPointOnWholeGreatCircle(angDistPassedRatio, target_radius, latStart, lonCurrent);
    m_ship.pos2D_prev = SystemsConverter( m_ship.sphPos ).GetPoint();
    double velMod = m_ship.vel.len();
    double shAsymptoteAzimuth = CalculateAsymptore( m_ship.sphPos );
    m_ship.prev_vel = m_ship.vel = Point(velMod * sin(shAsymptoteAzimuth), velMod * cos(shAsymptoteAzimuth) );

    inner_loop();

    m_satellite.sphPos = CalculateSatellitePosition();
    m_ship.sphPos = SystemsConverter( m_ship.pos2D, target_radius ).GetSpherical();
    double angle = CalcAngDistOfPlanes( m_ship.sphPos, m_satellite.planeN );
    angDistance += angle;
    Ldistance = angle * target_radius; // Longitudinal distance of final orbit planes.

    ++iter;
}

void DirectAscent::inner_loop()
{
    inner_iter = 0;
    m_ship.s.clear();
    double temp_t = m_t.total_t;

    m_ship.sphPos.r = target_radius;

    Spherical localsphShipPos;
    Vect3 localShPos;
    Vect3 localShVel;

    localShPos = SystemsConverter( m_ship.sphPos ).GetVector();
    localShVel = CalcShipVel3D( localShPos );

    do
    {
        // got all the basic data
        // simulate acceleration in other direction
        // see http://www.euclideanspace.com/maths/geometry/rotations/theory/inaPlane/index.htm

        Point shVel2D = ConvertShipVel3Dto2D(m_ship.sphPos, localShVel);
        AzimuthMath am;
        double true_azimuth = am.CalcTrueAzimuth( m_ship.pos2D_prev.y / target_radius, tgtParam.incl, shVel2D, mi, target_radius );

        double v2DxAdd = initialAccel * m_t.step * sin(true_azimuth);
        double v2DyAdd = initialAccel * m_t.step * cos(true_azimuth);

        shVel2D.x += v2DxAdd;
        shVel2D.y += v2DyAdd;
        // convert back to 3D space
        localShVel = ConvertShipVel2Dto3D(m_ship.sphPos, shVel2D);

        Vect3 vel2Add = localShVel * m_t.step;

        // VectorMath vm;
        bool newWay = false;
        if ( ! newWay )
        {
            //double angVPos = vm.angle(localShPos, vel2Add) * DEG;
            localShPos += vel2Add;
            double magMulPos = target_radius / localShPos.len();
            /// TODO: Rotate this vector!!
            localShPos *= magMulPos ; // keep me at the radius please
            //angVPos = vm.angle(localShPos, vel2Add) * DEG;
        }
        else
        {
            /// ROTATING VECTOR
//            Vect3 localShPos2 = localShPos + vel2Add;
            double angPassed = vel2Add.len() / target_radius;

            Vect3 x = localShPos.norm();
            Vect3 y = vel2Add.norm();
            Point v2D;
            v2D.x = x.x * localShPos.x + x.y * localShPos.y + x.z * localShPos.z;
            v2D.y = y.x * localShPos.x + y.y * localShPos.y + y.z * localShPos.z;

            Point v2D2;
            v2D2.x = v2D.len() * cos(angPassed);
            v2D2.y = v2D.len() * sin(angPassed);

            Vect3 p3;
            p3.x = x.x * v2D2.x + y.x * v2D2.y;
            p3.y = x.y * v2D2.x + y.y * v2D2.y;
            p3.z = x.z * v2D2.x + y.z * v2D2.y;

            localShPos = p3;
/// END ROTATING VECTOR
        }

        SystemsConverter convSh( localShPos );
        m_ship.sphPos = convSh.GetSpherical();
        Point pos = convSh.GetPoint();

        m_ship.vel = shVel2D;
        m_ship.pos2D = m_ship.pos2D_prev = pos;

        m_ship.s.push_back(m_ship.pos2D);
        ++inner_iter;
        m_t.total_t += m_t.step;
    }
    while ( m_ship.vel.len() < tgt_orbit_v2_ref_module );   // while final vel not reached

    m_t.turning_t = m_t.total_t - temp_t;
}

Vect3 DirectAscent::CalcShipVel3D(const Vect3 & shPos)
{
    VectorMath vm;
    Vect3 localShVel = - vm.cross( shPos.norm(), m_ship.planeN.norm() );
    double velModule = max_xvel;
    if ( velModule == 0 )
    {
        // This happens at initialisation. Any value other than 0 is OK
        velModule = 1000;
    }
    double magMulVel = velModule / localShVel.len();
    localShVel *= magMulVel;

    return localShVel;
}

Vect3 DirectAscent::Get2dPlaneY()
{
    // create a plane of reference, rotating along the ship,
    // on which the ship's velocity vector is flat
    Vect3 z(0, 0, -1);
    if ( tgtParam.azFlag == SOUTHERN_AZIMUTH )
        z.z = -z.z;

    return z;
}

Vect3 DirectAscent::Get2dPlaneX(const Spherical & sphPos)
{
    Spherical sphShipPlusQuarter = sphPos;
    sphShipPlusQuarter.phi += PI/2.0;
    sphShipPlusQuarter.theta = PI/2.0;

    Vect3 x = SystemsConverter( sphShipPlusQuarter ).GetVector().norm();

    return x;
}


Point DirectAscent::ConvertShipVel3Dto2D(const Spherical & sphPos, const Vect3 & shVel3D)
{
    Vect3 x = Get2dPlaneX(sphPos);
    Vect3 z = Get2dPlaneY();

    // convert to 2D space where it's easier to add velocity
    return VectorMath().Cast3Donto2D(shVel3D, x, z);
}

Vect3 DirectAscent::ConvertShipVel2Dto3D(const Spherical & sphPos, const Point & shVel2D)
{
    Vect3 x = Get2dPlaneX(sphPos);
    Vect3 z = Get2dPlaneY();

    return VectorMath().Cast2Donto3D(shVel2D, x, z);
}

void DirectAscent::reset_values()
{
    distanceShipSatFinal = 0; // !
    Ldistance = 1000;
    main_iter = iter = inner_iter = 0;

    m_ship = Ship();
    m_satellite = Satellite();
    m_t = Times();

    xPrvDistance = xDistance = 0;
    solNotPossible = fuelEfSolNotPossible = false;
    max_xvel = 0;

    initialAccel = GetShipsInitialAcceleration();

    debug = debug2 = debug3 = debug4 = debug5 = 0;
    debugbool = false;
}

double DirectAscent::CalculateAsymptore( const Spherical & sphPos )
{
    Vect3 localShPos = SystemsConverter( sphPos ).GetVector();
    Vect3 localShVel = CalcShipVel3D( localShPos );
    Point shVel2D = ConvertShipVel3Dto2D( sphPos, localShVel );

    double asymptote = PI/2.0 - atan2( shVel2D.y, shVel2D.x );

    return asymptote;
}

Spherical DirectAscent::CalcPointOnWholeGreatCircle(double angRatio, double r, double latStart, double lonCurrent, bool bSatellite )
{
    Geo geo = GreatCircleMath().CalcPointOnWholeGC( angRatio, Geo(latStart, lonCurrent) );
    Spherical sph = SystemsConverter(geo, r).GetSpherical();

    if ( bSatellite )
        sph.phi += PI/2.0;

    while ( sph.phi > PI )
        sph.phi -= 2*PI;

    return sph;
}

double DirectAscent::CalcAngDistOfPlanes( const Spherical & sphShipPos, Vect3 v3SatPlaneN )
{
    Vect3 vsh = SystemsConverter(sphShipPos).GetVector(); // get ship's cartesian coords
    if ( tgtParam.azFlag == SOUTHERN_AZIMUTH )
        v3SatPlaneN = -v3SatPlaneN;

    double ang = - ( PI/2.0 - VectorMath().angle(v3SatPlaneN, vsh)) ;
    return ang;
}

void DirectAscent::UpdateGreatCircleShip()
{
    m_ship.vGC = GreatCircleAdvMath().CalculateGreatCircle( target_radius, latStart, lonCurrent );
}

void DirectAscent::UpdateGreatCircleSatellite()
{
    m_satellite.vGC = GreatCircleAdvMath().CalculateGreatCircle( target_radius, tgtParam.incl, m_satellite.angEqu + PI/2.0 );
}

void DirectAscent::UpdatePlaneMomentumShip()
{
    m_ship.planeN = SpaceMath().CalcPlaneMomentum( latStart, lonCurrent );
}

void DirectAscent::UpdatePlaneMomentumSatellite()
{
    m_satellite.planeN = SpaceMath().CalcPlaneMomentum( tgtParam.incl, m_satellite.angEqu + PI/2.0 );
}

Vect3 DirectAscent::CalcNode()
{
    Vect3 node = VectorMath().cross( m_ship.planeN, m_satellite.planeN ).norm();
    if ( tgtParam.azFlag == SOUTHERN_AZIMUTH )
        node = -node;

    return node;
}

double DirectAscent::CalcAngDistToNode( const Spherical & sphShipStartPos )
{
    Vect3 vesselPos = SystemsConverter(sphShipStartPos).GetVector().norm(); // get ship's cartesian coords
    Vect3 node = CalcNode();

    double angle = PI - VectorMath().angle(node, vesselPos);
    return angle;
}

double DirectAscent::CalcAngDistStartPosToNode()
{
    Spherical sLaunchPos = SystemsConverter( Geo(latStart, lonCurrent), target_radius).GetSpherical();
    return CalcAngDistToNode( sLaunchPos );
}

bool DirectAscent::IsNodeInFrontOfShip()
{
    Spherical sNode = SystemsConverter( -CalcNode() ).GetSpherical();
    double diff = sNode.phi - lonCurrent;

    return diff * DEG > 3;
}

double DirectAscent::CalcTimeToNodeOnGround()
{
    Geo geoShipPos(latStart, lonStart);
    double angle = SpaceMath().CalcAngleToSatelliteNodeOnGround(geoShipPos, tgtParam.incl, m_satellite.angEqu, tgtParam.azFlag == NORTHERN_AZIMUTH);
    double period = bodyPhys.sidDay; // we're rollin' as the ground does
    // Get the time for the vessel to get to this node
    // FIXME This is an approximation that assumes that the orbit is circular - ie constant orbital speed
    double timeToNode = angle / (2 * PI) * period;
    return timeToNode;
}

