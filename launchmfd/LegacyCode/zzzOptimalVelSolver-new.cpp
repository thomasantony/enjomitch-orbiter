// ==============================================================
//                 ORBITER MODULE: LaunchMFD
//                  Part of the ORBITER SDK
//
// Copyright (C) 2007      "Vanguard"                  - author (dressed up calcualtions into an MFD)
// Copyright (C) 2007      Pawel "She'da'Lier" Stiasny - contributor (yaw error visual representation)
// Copyright (C) 2008      Mohd "Computerex" Ali       - borrowed his code (multiple vessels support)
// Copyright (C) 2008      Chris "Kwan" Jeppesen       - borrowed his code (peg guidance)
// Copyright (C) 2008      Steve "agentgonzo" Arch     - co-developer (lots of goodies!)
// Copyright (C) 2007-2010 Szymon "Enjo" Ender          - author and maintainer (everything else ;> )
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

const double PI = 3.14159265358979312;
#define GGRAV 6.67259e-11
#define RAD PI/180
#define DEG 180/PI

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

// Simpson integration
// a - starting argument of function
// b - ending argument of function
// data - function specific data. Can be NULL
// function pointer (define your own function)
// epsilon - specific preision that you may want to set
double simpson_int (double a, double b, const void * data, double (*function)(double x, const void * data), double epsilon = 0.001)
{
	double h, integ = 0, integ_prev = 1;
	int m = 2;
	do
	{
		double sum = 0;
		h = (b-a) / (double)(2.0*m);
		for ( int i = 1; i < 2*m; ++i )
		{
			if (i % 2) sum = sum + 4.0*(*function)(a + h*i, data);
			else sum = sum + 2.0*(*function)(a + h*i, data);
		}
		integ_prev = integ;
		integ = (h/3.0) * ( (*function)(a, data) + sum + (*function)(b, data) );
		m = m * 2;
	} while ( fabs(integ - integ_prev) > epsilon );
	return integ;
}


class CaVertParams
{
  public:
    CaVertParams( const BODYPHYS & bodyPhys, const SHIP_PARAMS & shipParam, const OptimalVelSolverParams & optVelPar, double parVelHoriMax )
    {
      m_shipParam = shipParam;
      vx = vy = 0;
      vsatx = 0;
      vsaty = 0;
      true_az = 0;
      t0 = 0;
      massCurr = av_const = 0;

      double mi = GGRAV * bodyPhys.mass;
      //double alt = 350e3;
      //halfwayR = ( R + alt * 0.3714286 ); // rough estimate - g changes along the path
      tgtRad = optVelPar.target_radius; // rough estimate - g changes along the path
      g = mi / (optVelPar.target_radius * optVelPar.target_radius);
      //g = 9.81;

		  velOrb = sqrt(mi/optVelPar.target_radius); // target's northern velocity


      velHoriMax = parVelHoriMax * velOrb;
      // solve velHoriMax = F/(m + mflowvert(t)*t + mflowhover(t)*t) * t for time and you will get:
      // NOTE: assuming vertical acceleration if vertical velocity has been reached
      tVelHoriMax = shipParam.massFull * velHoriMax /
        ( shipParam.thrustHori + shipParam.massFlowVert * velHoriMax + shipParam.massFlowHori * velHoriMax );

      rotVel = bodyPhys.radius * (2.0*PI/bodyPhys.sidDay) * cos(optVelPar.latStart);
      bodyR = bodyPhys.radius;

    }

    double acc ( double t, bool bZeroG = false ) const
    {
      double locG = g;
      if ( bZeroG )
        locG = 0;

//      double locT2 = t;
//      if ( t > t2 )
//        locT2 = t2;
      double vHori;
      double massTotal = m_shipParam.massFull - m_shipParam.massFlowVert * t;

      if ( t > tVelHoriMax )
      {
        massTotal -= m_shipParam.massFlowHori * tVelHoriMax;
        vHori = velHoriMax;
      }
      else
      {
        massTotal -= m_shipParam.massFlowHori * t;
        vHori = m_shipParam.thrustHori / massTotal * t;
      }

      double vh = vHori + rotVel;
      //double vh = rotVel;

      return m_shipParam.thrustVert / massTotal - locG + (vh*vh) / bodyR;
    }

    double g, t0, vx, vy, vsatx, vsaty, true_az, velOrb, tgtRad, velHoriMax, tVelHoriMax, rotVel, bodyR;
    double massCurr, av_const;
    SHIP_PARAMS m_shipParam;
};

double aacc(double t, const void * data)
{
  if ( data == NULL )
    return 0;
  CaVertParams * p = (CaVertParams *) data;
  if ( typeid(*p) != typeid(CaVertParams) )
    return 0;

	return 0.5 * t * p->acc(t);
}

// vertical acceleration needed to counter gravity
double aVert(double t, const void * data)
{
  if ( data == NULL )
    return 0;
  const CaVertParams * vpar = (const CaVertParams *) data;
  if ( typeid(*vpar) != typeid(CaVertParams) )
    return 0;

  t = t;// + vpar->t0;

  double ret = 0;//vpar->g - (t*vpar->a * t*vpar->a) / vpar->rFin;

  //cout << "aVert = " << ret << endl;

	return ( ret );
}

// integrate mass flow along the constant flight
double aVertConstMassFlow(double t, const void * data)
{
  if ( data == NULL )
    return 0;
  const CaVertParams * p = (const CaVertParams *) data;
  if ( typeid(*p) != typeid(CaVertParams) )
    return 0;

  const SHIP_PARAMS & s = p->m_shipParam;
  double accMaxCurr = s.thrustVert / ( p->massCurr - s.massFlowVert * t );

  double ret = p->av_const / accMaxCurr * s.massFlowVert; // how much mass are we using up at this slice?

	return ( ret );
}

// vertical acceleration needed to counter gravity when reaching final velocity
double aVert2(double t, const void * data)
{
  if ( data == NULL )
    return 0;
  const CaVertParams * vpar = (const CaVertParams *) data;
  if ( typeid(*vpar) != typeid(CaVertParams) )
    return 0;

  t = t;// + vpar->t0;

//  double vnx = vpar->vx + vpar->a * t * sin(vpar->az);
//  double vny = vpar->vy + vpar->a * t * cos(vpar->az);
//
//  double vn_mod = sqrt( vnx*vnx + vny*vny );

  double ret = 0;//vpar->gFin - (vn_mod * vn_mod) / vpar->rFin;

  //cout << "vn_mod = " << vn_mod << endl;
  //cout << "aVert2 = " << ret << ", vn_mod = " << vn_mod << endl;

	return ( ret );
}

VelIntegral OptimalVelSolver::CalcFuelOptim( const int maxIter, const double minDiff )
{
  double vel = 0.5; // starting vel
  double resultDiff = 0;
  int iter = 0;
  VelIntegral res;
  CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, vel );
  do
  {
    double prevResult = vel;
    res = FuelOptimIteration( vel );
    vel = res.vel;
    resultDiff = fabs( vel - prevResult );
    ++iter;
  } while ( iter < maxIter && resultDiff > minDiff && vel != 1 );

  res.vel *= aVpar.velOrb;
  return res;
}

double OptimalVelSolver::GetTrueVelOrb()
{
  CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, 0 );
  return aVpar.velOrb;
}

double OptimalVelSolver::GetDVAccEastAndHover( const CaVertParams & apar, double * tVert )
{
  double dvVert = 0;
  double tincr = 100;
  double t = tincr;
  double alt = apar.tgtRad - apar.bodyR;
  double prevDiff = alt;
  int i = 0;
  int iMax = 50;

  do
  {
    double integ = simpson_int(0, t, &apar, aacc);
    double a0 = apar.acc(0);
    double at = apar.acc(t);
    double nonint = t*t / 4 * ( a0 + a0*a0 / (2 * apar.g) + at*at/( 2 * apar.g ) );
    double diff = alt - (nonint + integ);

    if ( diff * prevDiff < 0 ) // sign changed
    {
      tincr /= -3;
    }
    t += tincr;
    prevDiff = diff;
    ++i;

    //std::cout << "And the integral is...\n" << nonint + integ << std::endl;
    a0 = apar.acc(0, true);
    at = apar.acc(t, true);
    dvVert = a0*t / 2 + at*t/2; // pole pod wykresem przyspieszenia
    //std::cout << "dv = " << dvVert + velHori << std::endl << std::endl;

  } while ( fabs(prevDiff) > 0.01 * alt && i < iMax );
 //   std::cout << "i = " << i << ", t = " << t << std::endl;

  *tVert = t;

  return dvVert + apar.velHoriMax;
}

VelIntegral OptimalVelSolver::CalcIntegralFromVel( double vel )
{
  CaVertParams aVpar( m_bodyPhys, m_shipPars, m_pars, vel );

  vel = aVpar.velHoriMax; // recalculated in constructor

  double t_accVert = 0;
  double t_accVertAfterVert = 0;
  double dv_accEastAndHover = GetDVAccEastAndHover( aVpar, &t_accVert );

  // The horizontal acceleration time exceeded vertical acceleration time
  // meaning that we haven't reached eastern velocity before than the vertial one
  // therfore we need to continue accelerating
  if ( t_accVert < aVpar.tVelHoriMax )
  {
    // t_accVertAfterVert =
  }
  double velFin = vel + aVpar.rotVel;

  // constant flight
  double t_vertRiseAndFall = t_accVert + (dv_accEastAndHover - aVpar.velHoriMax) / aVpar.g;
  double t_const = m_pars.angDist * m_pars.target_radius / velFin - t_vertRiseAndFall;

  double massFlow = m_shipPars.massFlowVert * t_accVert + m_shipPars.massFlowHori * ( aVpar.tVelHoriMax + t_accVertAfterVert );
  double massTotal = m_shipPars.massFull - massFlow;
  // countering gravity


  double av_const = aVpar.g - velFin*velFin / aVpar.tgtRad;
  double dvv_const = av_const * t_const;

  aVpar.massCurr = massTotal;
  aVpar.av_const = av_const;
  double massFlowConst = simpson_int(0, t_const, &aVpar, aVertConstMassFlow);

  aVpar.massCurr = massTotal -= massFlowConst;


  // turning phase
  aVpar.vx = velFin * sin(m_pars.shAsymptoteAzimuth);
  aVpar.vy = - velFin * cos(m_pars.shAsymptoteAzimuth);

  aVpar.vsatx = aVpar.velOrb * sin(m_pars.azimuth); // target's northern velocity
  aVpar.vsaty = aVpar.velOrb * cos(m_pars.azimuth); // target's eastern velocity

  double lnch_vy = fabs (aVpar.vsaty - (aVpar.vy) );
  double lnch_vx = aVpar.vsatx - (aVpar.vx);

  //if (lnch_v[1] < 0) lnch_v[1] = - lnch_v[1];
  if (lnch_vy==0) lnch_vy=0.01; //div by zero protection
  aVpar.true_az = atan(lnch_vx/lnch_vy);

  double vshsatx = aVpar.vsatx - aVpar.vx;
  double vshsaty = aVpar.vsaty - aVpar.vy;
  double vshsat = sqrt( vshsatx*vshsatx + vshsaty*vshsaty );
  double t_final = vshsat / aVpar.a;

  double av_final = simpson_int(0, t_final, &aVpar, aVert2);
  //for (int i = 0; i < t_final; i += 5)
  {
   // aVert2(i, &aVpar);
  }

  double ah_acc = aVpar.a * t_acc;
  double ah_final = aVpar.a * t_final;

  double sumVertical = av_acc + av_const + av_final;
  double sumHorizontal = ah_acc + ah_final;

  return VelIntegral(vel, sumVertical + sumHorizontal);
/*
  double initial_rotation_vel_wlat = m_bodyPhys.radius * (2.0*PI/m_bodyPhys.sidDay) * cos(m_pars.latStart);
  // accelerating east
  double t_acc = (vel - initial_rotation_vel_wlat)/aVpar.a;
  // countering gravity while accelerating
  double av_acc = simpson_int(0, t_acc, &aVpar, aVert);



  // constant flight
  double t_const = m_pars.angDist * m_pars.target_radius / vel;
  // countering gravity
  double av_const = (aVpar.gFin - vel*vel / aVpar.rFin) * t_const;

  // turning phase
  aVpar.vx = vel * sin(m_pars.shAsymptoteAzimuth);
  aVpar.vy = - vel * cos(m_pars.shAsymptoteAzimuth);

  aVpar.vsatx = aVpar.velOrb * sin(m_pars.azimuth); // target's northern velocity
  aVpar.vsaty = aVpar.velOrb * cos(m_pars.azimuth); // target's eastern velocity

  double lnch_vy= fabs (aVpar.vsaty - (aVpar.vy) );
  double lnch_vx = aVpar.vsatx - (aVpar.vx);

  //if (lnch_v[1] < 0) lnch_v[1] = - lnch_v[1];
  if (lnch_vy==0) lnch_vy=0.01; //div by zero protection
  aVpar.true_az = atan(lnch_vx/lnch_vy);

  double vshsatx = aVpar.vsatx - aVpar.vx;
  double vshsaty = aVpar.vsaty - aVpar.vy;
  double vshsat = sqrt( vshsatx*vshsatx + vshsaty*vshsaty );
  double t_final = vshsat / aVpar.a;

  double av_final = simpson_int(0, t_final, &aVpar, aVert2);
  //for (int i = 0; i < t_final; i += 5)
  {
   // aVert2(i, &aVpar);
  }

  double ah_acc = aVpar.a * t_acc;
  double ah_final = aVpar.a * t_final;

  double sumVertical = av_acc + av_const + av_final;
  double sumHorizontal = ah_acc + ah_final;

  return VelIntegral(vel, sumVertical + sumHorizontal);
  */
  //return VelIntegral(0,0);
}


VelIntegral OptimalVelSolver::FuelOptimIteration( double vel )
{
  double velLeft = vel - 0.0005;
  double velRight = vel + 0.0005;

  VelIntegral base = CalcIntegralFromVel( vel );
  VelIntegral left =  CalcIntegralFromVel( velLeft );
  VelIntegral right = CalcIntegralFromVel( velRight );

  double angDiff = CalcAnglesDiff( base, left, right );

  vel += 0.05 * sin( angDiff );

  if ( vel > 1 )
  {
    vel = 1; // end
  }

  //VelIntegral ret = CalcIntegralFromVel( vel ); // wasteful but accurate
  return VelIntegral(vel, base.dv);
  //return VelIntegral(vel, ret.y);
}

double OptimalVelSolver::CalcAnglesDiff( const VelIntegral & base, const VelIntegral & left, const VelIntegral & right )
{
  double angLeftBase = atan( (left.dv - base.dv) / fabs( left.vel - base.vel ) );
  double angRightBase = atan( (right.dv - base.dv) / fabs( right.vel - base.vel ) );

  return angLeftBase - angRightBase;
}
