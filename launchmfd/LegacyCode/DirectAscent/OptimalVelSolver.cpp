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

#include <typeinfo>
#include <cmath>

#include "OptimalVelSolver.hpp"
#include "OptimalVelSolverFunctions.hpp"
#include "Math/GeneralMath.hpp"
#include "Math/SimpsonFunctor.hpp"
using namespace EnjoLib;

OptimalVelSolver::OptimalVelSolver()
{
}

OptimalVelSolver::OptimalVelSolver( const BODYPHYS & bodyPhysPar, const OptimalVelSolverParams & optVelPar, const SHIP_PARAMS & shipPar )
{
    SetParams( bodyPhysPar, optVelPar, shipPar );
}

void OptimalVelSolver::SetParams( const BODYPHYS & bodyPhysPar, const OptimalVelSolverParams & optVelPar, const SHIP_PARAMS & shipPar )
{
    m_pars = optVelPar;
    m_bodyPhys = bodyPhysPar;
    m_shipPars = shipPar;
}

std::vector< Point > getGraph (double a, double b, const SimpsonFunctor & function, double step = 1)
{
    std::vector< Point > vret;
    double x = a;
    do
    {
        x += step;
        vret.push_back( Point( x, function(x) ) );

    }
    while ( x < b );

    return vret;
}

VelIntegral OptimalVelSolver::CalcFuelOptim( double parVel, const int maxIter, const double minDiff )
{
    double vel = parVel; // starting vel. Normally 0.5
    double resultDiff = 0;
    m_iter = 0;
    VelIntegral res;
    CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, vel );
    do
    {
        double prevResult = vel;
        res = FuelOptimIteration( vel );
        vel = res.vel;
        resultDiff = fabs( vel - prevResult );
        //++m_iter;
    }
    while ( m_iter < maxIter && resultDiff > minDiff && vel != 1 );

    res.vel *= aVpar.velOrb;
    return res;
}

double OptimalVelSolver::GetTrueVelOrb()
{
    CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, 0 );
    return aVpar.velOrb;
}

VelIntegral OptimalVelSolver::CalcIntegralFromVel( double vel, bool calcValuesOfFunctions )
{
    CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, vel );

    vel *= aVpar.velOrb;
    GeneralMath gm;
    t_acc = (vel - aVpar.initial_rotation_vel_wlat)/aVpar.a;
    double av_acc = gm.SimpsonInt(0, t_acc, aVert(aVpar));

    t_const = m_pars.angDist * m_pars.target_radius / vel;
    av_const = aVpar.g - vel*vel / aVpar.r;
    double av_const_int = av_const * t_const;

    aVpar.vx = vel * sin(m_pars.shAsymptoteAzimuth);
    aVpar.vy = vel * cos(m_pars.shAsymptoteAzimuth);

    aVpar.vsatx = aVpar.velOrb * sin(m_pars.azimuth); // target's northern velocity
    aVpar.vsaty = aVpar.velOrb * cos(m_pars.azimuth); // target's eastern velocity

    double lnch_vy = fabs (aVpar.vsaty - (aVpar.vy) );
    double lnch_vx = aVpar.vsatx - (aVpar.vx);

    //if (lnch_v[1] < 0) lnch_v[1] = - lnch_v[1];
    if (lnch_vy==0) lnch_vy=0.01; //div by zero protection
    aVpar.az = atan(lnch_vx/lnch_vy);

    double vshsatx = aVpar.vsatx - aVpar.vx;
    double vshsaty = aVpar.vsaty - aVpar.vy;
    double vshsat = sqrt( vshsatx*vshsatx + vshsaty*vshsaty );
    t_final = vshsat / aVpar.a;

    double av_final = gm.SimpsonInt(0, t_final, aVert2(aVpar) );
    //for (int i = 0; i < t_final; i += 5)
    {
        // aVert2(i, &aVpar);
    }

    double ah_acc = aVpar.a * t_acc;
    double ah_final = aVpar.a * t_final;

    double sum1 = av_acc + av_const_int + av_final;
    double sum2 = ah_acc + ah_final;

    test1 = av_acc;
    test2 = ah_acc;

    if ( calcValuesOfFunctions )
    {
        aVpar.t0 = 0;
        vGraphAccValues = getGraph(0, t_acc, aVert(aVpar) );
        aVpar.t0 = t_const + t_acc;
        vGraphEqualizingValues = getGraph(aVpar.t0, aVpar.t0 + t_final, aVert2(aVpar) );
    }

    return VelIntegral(vel, sum1, sum2);
}


VelIntegral OptimalVelSolver::FuelOptimIteration( double vel, bool acquireIntegralsFromFirst )
{
    double velLeft = vel - 0.0005;
    double velRight = vel + 0.0005;

    VelIntegral base = CalcIntegralFromVel( vel, acquireIntegralsFromFirst );
    VelIntegral left =  CalcIntegralFromVel( velLeft );
    VelIntegral right = CalcIntegralFromVel( velRight );

    double angDiff = CalcAnglesDiff( base, left, right );
    m_angDiff = angDiff;

    // a simple and clever controller if you think of it
    vel += 0.05 * sin( angDiff );

    if ( vel > 1 )
    {
        vel = 1; // end
    }

    ++m_iter;

    // normally we don't care about dVs in the algorithm itself, only in the visualisation
    return VelIntegral(vel, base.dvVert, base.dvHori);
}

double OptimalVelSolver::CalcAnglesDiff( const VelIntegral & base, const VelIntegral & left, const VelIntegral & right )
{
    double angLeftBase = atan( (left.dv() - base.dv()) / fabs( left.vel - base.vel ) );
    double angRightBase = atan( (right.dv() - base.dv()) / fabs( right.vel - base.vel ) );

    if (angLeftBase > 0 && angRightBase > 0)
    {
        // minimum has been reached
        return 0;
    }

    return angLeftBase - angRightBase;
}
