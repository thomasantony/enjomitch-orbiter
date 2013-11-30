/* This pitch calculation program is derived from the work of NASA (PEG method) and kwan3217 (PEG autopilot)
Rewriting and integration into LaunchMFD was done by Steve Arch (agentgonzo) */

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
// peg.cpp - Main MFD class
// Authors - NASA (PEG method), Chris "Kwan" Jeppesen (PEG autopilot),
// Steve "agentgonzo" Arch (integration into LaunchMFD)
// Szymon "Enjo" Ender (cleanups)
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

#include "PEG.h"

PEG::PEG( const VESSEL * v )
{
	vessel = v;
	Init();
	hpTarget=100000; //m above surface of launch planet
	haTarget=100000; //m above surface of launch planet
	taTarget=35*RAD;
	TMajorCycle=1.0;
	target_pitch=0;
	last_target_pitch=0;
	CmdPDot=0;
	T=145; //Initial estimate of time to burnout
	rT=rpTarget;
}

PEG::~PEG()
{
}

void PEG::SetApses(double PeAlt, double ApAlt)
{
	hpTarget = PeAlt;
	haTarget = ApAlt;
}

double PEG::GetTargetPitch()
{
	met=oapiGetSimTime() - t0;
	Navigate();
	Estimate();
	Guide();

	return CmdP;
}

void PEG::Navigate()
{
    mu=GGRAV*oapiGetMass(vessel->GetGravityRef());
    VECTOR3 rv,vv;
	vessel->GetRelativePos(vessel->GetGravityRef(),rv);
	vessel->GetRelativeVel(vessel->GetGravityRef(),vv);
	const double Re=oapiGetSize(vessel->GetGravityRef());
	rpTarget=hpTarget+Re;
	raTarget=haTarget+Re;
	eTarget=(raTarget-rpTarget)/(rpTarget+raTarget);

	r = length(rv);
	const VECTOR3 & hv=crossp(rv,vv); // Angular momentum vector
	h = length(hv);
	const VECTOR3 & rh = rv/r; // Vertical direction
	const VECTOR3 & hh = hv/h; // Crosstrack direction
	const VECTOR3 & thetah = crossp(hh,rh); // Downtrack direction
	vr=dotp(vv,rh);
	const double vtheta=dotp(vv,thetah);
	omega=vtheta/r; // Angular velocity
	//w=x/(r*r);
	last_target_pitch=target_pitch;
    eng = GetEngineCapabilities(vessel);
	if(eng.F<1) //If there's still no thrust, there's no main thruster group, so we're outtahere.
		return;

	a0 = eng.F / vessel->GetMass();
	tau = eng.isp / a0;

	//sprintf(oapiDebugString(), "ISP = %.02lf, F = %.02lf", Isp, F);
}

void PEG::Estimate()
{
	const double rbar=(r+rT)/2;
	const double num=mu/(rbar*rbar)-omega*omega*r;
	const double fr=A+num/a0;
	const double fdotr=B+(num/a(T)-fr)/T;
	const double ftheta=1-fr*fr/2;
	const double fdottheta=-(fr*fdotr);
	const double fdotdottheta=-(fdotr*fdotr)/2;

    //Estimate true anomaly at cutoff
	const double d3=h*vr/(r*r*r);
	const double d4=(h*vrT/(rT*rT*rT)-d3)/T;
	deltatheta=(h/(r*r))*T+(ftheta*c0(T)+fdottheta*cn(T,1)+fdotdottheta*cn(T,2))/rbar-d3*T*T-d4*T*T*T/3.0;
//	double thetaT=op.TrL+deltatheta;
    taTarget = deltatheta; // ?
	if(eTarget == 0)
	{
		//We're circular, ta doesn't matter
		taTarget=0;
	}

	//Calculate orbit parameters for this ta
	if(eTarget!=1)
	{
		aOrbit=rpTarget/(1-eTarget);
		p=aOrbit*(1-eTarget*eTarget);
	}
	else
	{
		aOrbit=0;
		p=2*rpTarget;
	}
	//Estimate time of cutoff
	const double hT=sqrt(mu*p);
	const double deltah=hT-h;
	//  deltav2=(deltah/rbar+Isp*T2*(fdottheta+fdotdottheta*tau)+fdotdottheta*Isp*T2*T2/2)/
	//                (ftheta+fdottheta*tau+fdotdottheta*tau*tau);
	const double deltav=deltah/rbar;
//	double deltav=(deltah/rbar+eng.isp*T*(fdottheta+fdotdottheta*tau)+fdotdottheta*eng.isp*T*T/2)/
//	                (ftheta+fdottheta*tau+fdotdottheta*tau*tau);
	//  if (met<(TPEGStart+5)) {
	T=tau*(1-exp(-deltav/eng.isp));
	if(T>1000)
		T=1000;
	//    sprintf(oapiDebugString(),"T: %f",T);
	//    T2=tau*(1-exp(-deltav2/Isp));
	//  } else {
	//    T2=tau*(1-exp(-deltav/Isp));
	//    T=tau*(1-exp(-deltav2/Isp));
	//  }
	//Any comparison with NaN is false, so
	//invert these tests so it's true if NaN
	/*if(!(tau>T)|!(0<T))
		T=tau-10;*/
	//  if(!(tau>T)|!(0<T))T=T2;
	metCutoff=T+met;
	//Estimate radius at cutoff
	rT=p/(1+eTarget*cos(taTarget));
	//estimate vertical speed at cutoff
	vrT=sqrt(mu/p)*eTarget*sin(taTarget);
}

void PEG::Guide()
{
	//Calculate pitch steering coefficients
	const double a=b0(T);
	const double b=bn(T,1);
	const double c=c0(T);
	const double d=cn(T,1);
	const double y1=vrT-vr;
	const double y2=rT-vr*T-r;
	const double D=a*d-b*c;
	A = (d*y1-b*y2)/D;
	B = (a*y2-c*y1)/D;
	const double TLastAB=met;

	const double C=(mu/(r*r)-(omega*omega*r))/a0;
	const double fhdotrh=A+B*(met-TLastAB)+C;
	if(fabs(fhdotrh)>1)
	{
		valid = false;
		//Commanded to do a cos(pitch) out of range, turn
		//off pitch guidance so pilot has a chance to do something
	}
	else
	{
		valid = true;
		target_pitch=PI/2-acos(fhdotrh);
	}

	target_pitch-=eng.ThrAngle;
	CmdPDot=(target_pitch-last_target_pitch)/TMajorCycle;

	if(met > metCutoff)
		return;
	CmdP=(target_pitch+CmdPDot * 0.25);
//    CmdP=(target_pitch+CmdPDot);
}

double PEG::a(double t)
{
	return a0/(1-t/tau);
}

double PEG::b0(double TT)
{
	return -eng.isp*log(1-TT/tau);
}

double PEG::bn(double TT, int n)
{
	if(n==0) return b0(TT);
	return bn(TT,n-1)*tau-eng.isp*pow(TT,n)/n;
}

double PEG::c0(double TT)
{
	return b0(TT)*TT-bn(TT,1);
}

double PEG::cn(double TT, int n)
{
	if(n==0)
		return c0(TT);
	return cn(TT,n-1)*tau-eng.isp*pow(TT,n+1)/(n*(n+1));
}

void PEG::Init()
{
	t0=oapiGetSimTime();
//	vessel->GetRelativePos(vessel->GetGravityRef(),rh0);
//	rh0=rh0*(1/length(rh0));
}

double PEG::GetDeltaTheta() const
{
    return deltatheta;
}

double PEG::GetTMECO() const
{
    return T;
};






