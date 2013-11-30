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
// Authors - Chris "Kwan" Jeppesen - original Java version author
//           Szymon "Enjo" Ender - C++ conversion
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

#include "PEGMulti.h"
#include "../Utils/VesselCapabilities.hpp"

#include <Systems/Vect3.hpp>
using namespace EnjoLib;

double lastPitch;

//double A_end, B_end, r_T_end, omega_T, omega_T_end, h_T, tau_end, Isp_end, rbar, rbar_end, f_theta;
//double fdot_theta, fdotdot_theta;
//double DeltaA_, DeltaB_;
//double T_end, T, lastT;

  double r_T[3];
  double rdot_T[3];
  double v_thetaT[3];
  //Derived target variables
  double omega_T[3];
  double h_T[3];
  //double energy_T[3];
  //Navigation variables
  double r;
  double v;
  double z;
  Vect3 hvec;
  double h;
  Vect3 rhat;
  Vect3 hhat;
  Vect3 thetahat;
  double thrust;
  double mass;
  double a_[3];
  double v_e[3];
  double tau_[3];
  double omega;
  double rdot;
//  // @VariableDesc(desc="Downrange speed",units="m/s",Major=true,Minor=true)
//  double vtheta;
  //Clock variables
  double Deltat;
  double tlast;
  //Estimation variables
  double Deltah;
  double rbar_[3];
  double f_r[3];
  double f_rT[3];
  double a_T[3];
  double fdot_r[3];
  double f_theta[3];
  double fdot_theta[3];
  double fdotdot_theta[3];
  double N1;
  double N2a;
  double N2b;
  double N2;
  double N3;
  double N;
  double D0;
  double D1;
  double D2;
  double D;
  double Deltav;
  double k_b;
  double k_c;
  double aa;
  double bb;
  double cc;
  double dd;
  double DeltaA_[3];
  double DeltaB_[3];
  //Feedback variables
  double A_[3];
  double B_[3];
  double T_[3];
  //Guidance variables
  double g;
  double cent;
  double g_term;
  double cent_term;
  double fhatdotrhat;
  double fhatdothhat;
  double fhatdotthetahat;
  // @VariableDesc(desc="Current specific mechanical energy",units="m^2/s^2",Major=true,Minor=false)
  double energy;
  // @VariableDesc(desc="Specific mechanical energy history",units="m^2/s^2",Major=true,Minor=false)
  double Deltaenergy[3];
  // @VariableDesc(desc="Specific mechanical energy time history",units="s",Major=true,Minor=false)
  double energyTime[3];
  // @VariableDesc(desc="Cutoff enabled",units="",Major=true,Minor=false)
  boolean cutoffEnable;
  // @VariableDesc(desc="Do we want to go to the moon or not?",units="",Major=true,Minor=false)
  boolean toTheMoon;
  // @VariableDesc(desc="Predicted accurate cutoff time",units="s",Major=true,Minor=true)
  double t_cutoff;
  // @VariableDesc(desc="Coriolis constant term",units="",Major=true,Minor=false)
  double d_3;
  // @VariableDesc(desc="Coriolis constant term at burnout",units="",Major=true,Minor=false)
  double d_3T;
  // @VariableDesc(desc="Coriolis linear term",units="",Major=true,Minor=false)
  double d_4;
  // @VariableDesc(desc="Estimated travel angle to burnout",units="",Major=true,Minor=false)
  double Deltatheta;
  // @VariableDesc(desc="Sine of polar angle",units="",Major=true,Minor=false)
  double sintheta;
  // @VariableDesc(desc="Cosine of polar angle",units="",Major=true,Minor=false)
  double costheta;
  // @VariableDesc(desc="Polar angle",units="rad",Major=true,Minor=false)
  double theta;
  // @VariableDesc(desc="True anomaly at end of powered flight",units="rad",Major=true,Minor=false)
  double eta_T;
  // @VariableDesc(desc="Total inertial velocity at end of powered flight",units="rad",Major=true,Minor=false)
  double v_T[3];

  const int n = 2;
  int j = 1;


PEGMulti::PEGMulti( const VESSEL * v )
    : vessel( v )
{
    Init();
    hpTarget=100000; //m above surface of launch planet
    haTarget=100000; //m above surface of launch planet
    taTarget=35*RAD;
    TMajorCycle=1.0;
    target_pitch=0;
    last_target_pitch=0;
    CmdPDot=0;
    //T_end=145; //Initial estimate of time to burnout
    T_[n]=403.98;
//    A = -0.02966;
//    B = -1.77e-4;

    A_[n-1]=-0.02966;
    B_[n-1]=-1.77e-4;
}

PEGMulti::~PEGMulti()
{
}

void PEGMulti::SetStage( const Stage & stage, double timeSinceLaunch  )
{
    m_stage = stage;

//    T = stage.tmeco;
//    tau_end = stage.isp / stage.accel;
//    Isp_end = stage.isp;

    v_e[2]=stage.isp;
    T_[1]=stage.tmeco - timeSinceLaunch;
    a_[2]=stage.accel;
    tau_[2]=v_e[2]/a_[2];
}

void PEGMulti::SetApses(double PeAlt, double ApAlt)
{
    hpTarget = PeAlt;
    haTarget = ApAlt;
}

double PEGMulti::GetTargetPitch(double timeSinceLaunch)
{
    met=oapiGetSimTime() - t0;
    Navigate(timeSinceLaunch);
    Estimate();
    Guide();

    return CmdP;
}

Engine GetEngineCapabilities(const VESSEL * v)
{
    VesselCapabilities vc;
    Engine eng;
	eng = vc.GetMainThrustParm(v);
	if(eng.F == 0)
	{
        //If no thrust now, get the max thrust
		eng = vc.EstimateMainThrustParm(v);
	}
	return eng;
}

void PEGMulti::Navigate(double timeSinceLaunch)
{
    VECTOR3 rv,vv;
	vessel->GetRelativePos(vessel->GetGravityRef(),rv);
	vessel->GetRelativeVel(vessel->GetGravityRef(),vv);
	double Re=oapiGetSize(vessel->GetGravityRef());
	rpTarget=hpTarget+Re;
	raTarget=haTarget+Re;
	eTarget=(raTarget-rpTarget)/(rpTarget+raTarget);

	r = length(rv);
	VECTOR3 hv=crossp(rv,vv);
	h = length(hv);
	VECTOR3 rh = rv*(1/r);
	VECTOR3 hh = hv*(1/h);
	VECTOR3 thetah = crossp(hh,rh);
	vr=dotp(vv,rh);
	double vtheta=dotp(vv,thetah);
	omega=vtheta/r;
    //w=x/(r*r);
    last_target_pitch=target_pitch;
        eng = GetEngineCapabilities(vessel);
	if(eng.F<1) //If there's still no thrust, there's no main thruster group, so we're outtahere.
		return;

    a0 = eng.F/vessel->GetMass();
    tau = eng.isp/a0;

    T_[1]=m_stage.tmeco - timeSinceLaunch;

    Navigate2();

    //sprintf(oapiDebugString(), "T = %.02lf, met = %.02lf, metCutoff = %.02lf", T, met, metCutoff);
}

void PEGMulti::Navigate2()
{
    r_T[n]=rpTarget;
    rdot_T[n]=0;
    v_thetaT[n]=sqrt(mu/rpTarget);
    //These are constant if the targets are constant
    h_T[n]=v_thetaT[n]*r_T[n];
    omega_T[n]=v_thetaT[n]/r_T[n];
    //Energy=vvec dot vvec-2*mu/r=|vvec|^2-2*mu/r
//    energy_T[n]=v_thetaT[n]*v_thetaT[n]+rdot_T[n]*rdot_T[n]-2*mu/r_T[n];


    a_[j]=a0;
    v_e[j]=eng.isp;
    tau_[j]= eng.isp/a0;
    //Components of velocity in various directions
    if(omega_T[j]==0) omega_T[j]=omega; //Seed value for angular speed at first staging
    rdot=vr;
}
void PEGMulti::Estimate()
{
    // Estimate staging
        //Ghosts of stages past
	  for (int i = 0; i < j; ++i)
	  {
		    h_T[i]=h;
		    omega_T[i]=omega;
		    r_T[i]=r;
		    rdot_T[i]=rdot;
		    T_[i]=0;
		    DeltaA_[i]=0;
		    DeltaB_[i]=0;
	  }
    if(j < 2) {
      rdot_T[j]=rdot+b0(1,T_[j])*A_[j]+b_(1,1,T_[j])*B_[j];
      r_T[j]=r+rdot*T_[j]+c_(0,1,T_[j])*A_[j]+c_(1,1,T_[j])*B_[j];
      rbar_[j]=(r_T[j]+r)/2;
      f_r[j]=A_[j]+((mu/(r*r)-omega*omega*r)/a_[j]);
      a_T[j]=a(j,T_[j]);
      f_rT[j]=A_[j]+B_[j]*T_[j]+((mu/(r_T[j]*r_T[j])-omega_T[j]*omega_T[j]*r_T[j])/a_T[j]);
      fdot_r[j]=(f_rT[j]-f_r[j])/T_[j];
      f_theta[j]=1-f_r[j]*f_r[j]/2;
      fdot_theta[j]=-f_r[j]*fdot_r[j];
      fdotdot_theta[j]=-fdot_r[j]*fdot_r[j]/2;
      h_T[j]=h+rbar_[j]*(f_theta[j]*b0(1,T_[j])+fdot_theta[j]*b_(1,1,T_[j])+fdotdot_theta[j]*b_(2,1,T_[j]));
      v_thetaT[j]=h_T[j]/r_T[j];
      omega_T[j]=v_thetaT[j]/r_T[j];
      DeltaA_[j]=(mu/(r_T[j]*r_T[j])-omega_T[j]*omega_T[j]*r_T[j])*(1/a(j,T_[j])-1/a_[j+1]);
      DeltaB_[j]=(mu/(r_T[j]*r_T[j])-omega_T[j]*omega_T[j]*r_T[j])*(1/v_e[j]-1/v_e[j+1])
        +(3*omega_T[j]*omega_T[j]-2*mu/(r_T[j]*r_T[j]*r_T[j]))*rdot_T[j]*(1/a(j,T_[j])-1/a_[j+1]);
      A_[j+1]=A_[j]+DeltaA_[j]+B_[j]*T_[j];
      B_[j+1]=B_[j]+DeltaB_[j];
    }

    // Estimate T
        if(a_[j]<=0) {
      //Just keep the old estimate if coasting
      return;
    }
    rbar_[n]=(r_T[n]+r_T[n-1])/2;
    Deltah=h_T[n]-h_T[n-1];
    //Calculate current programmed pitch and approximate pitch rate
    f_r[n]=A_[n]+((mu/(r*r)-omega_T[n-1]*omega_T[n-1]*r)/a_[n]);
    a_T[n]=a(n,T_[n]);
    f_rT[n]=A_[j]+B_[j]*T_[j]+((mu/(r_T[n]*r_T[n])-omega_T[n]*omega_T[n]*r_T[n])/a_T[n]);
    fdot_r[n]=(f_rT[n]-f_r[n])/T_[n];
    //Calculate downrange thrust component coefficients
    f_theta[n]=1-f_r[n]*f_r[n]/2;
    fdot_theta[n]=-f_r[n]*fdot_r[n];
    fdotdot_theta[n]=-fdot_r[n]*fdot_r[n]/2;
    //Calculate required Deltav to gain targeted Deltah
    N1=Deltah/rbar_[n];
    N2a=v_e[j]*T_[n];
    N2b=fdot_theta[n]+fdotdot_theta[n]*tau_[n];
    N2=N2a*N2b;
    N3=fdotdot_theta[n]*v_e[n]*T_[n]*T_[n]/2;
    N=N1+N2+N3;
    D0=f_theta[n];
    D1=fdot_theta[n]*tau_[n];
    D2=fdotdot_theta[n]*tau_[n]*tau_[n];
    D=D0+D1+D2;
    if(abs(D)<1.0) {
      Deltav=N/D; //Normal mode, denominator is sane
      valid = true;
    } else {
      //Something weird with the denominator, just use N1
      //equivalent to assuming rocket is pointing horizontal
      D=0.001;
      Deltav=N1;
      valid = false;
    }
    if(!cutoffEnable) T_[n]=tau_[n]*(1-exp(-Deltav/v_e[n]));
}

void PEGMulti::Guide()
{
    if (T_[j] < 30)
        return;
    // Guide Major
     //Calculate energy to go (needed for cutoff)
//    Deltaenergy[0]=Deltaenergy[n-1];  energyTime[0]=energyTime[n-1];
//    Deltaenergy[n-1]=Deltaenergy[n];  energyTime[n-1]=energyTime[n];
//    energy=v*v-2*mu/r;
//    Deltaenergy[n]=energy_T[n]-energy; energyTime[n]=met;
    //if(T_[n]>10) {
//      if(toTheMoon) {
//        CalculateTargets();
//      }
      cutoffEnable=false;
      aa=b0(1,T_[n-1])+b0(2,T_[n]);
      bb=b_(1,1,T_[n-1])+b_(1,2,T_[n])+b0(2,T_[n-1]);
      cc=c0(1,T_[n-1])+c0(2,T_[n])+b0(1,T_[n-1])*T_[n];
      dd=c_(1,1,T_[n-1])+b_(1,1,T_[n-1])*T_[n]+c0(2,T_[n])*T_[n-1]+c_(1,2,T_[n]);
      k_b=rdot_T[n]-rdot-b0(2,T_[n])*DeltaA_[n-1]-b_(1,2,T_[n])*DeltaB_[n-1];
      k_c=r_T[n]-r-rdot*(T_[n-1]+T_[n])-c0(2,T_[n])*DeltaA_[n-1]-c_(1,2,T_[n])*DeltaB_[n-1];

      B_[j]=(k_c*aa-cc*k_b)/(dd*aa-cc*bb);
      A_[j]=k_b/aa-bb*B_[j]/aa;
  //  }
//    else {
//      cutoffEnable=true;
//      //Cutoff guidance - use the energy history to calculate the cutoff time
//      double x0,x1,x2,y0,y1,y2,CC,BB,AA;
//      x0=energyTime[0];y0=Deltaenergy[0];
//      x1=energyTime[n-1];y1=Deltaenergy[n-1];
//      x2=energyTime[n];y2=Deltaenergy[n];
//      double Delta=x1*x2*x2+x0*x1*x1+x0*x0*x2-x1*x1*x2-x0*x2*x2-x0*x0*x1;
//      double DeltaC=y0*x1*x2*x2 +x0*x1*x1*y2 +x0*x0*y1*x2 -y0*x1*x1*x2 -x0*y1*x2*x2 -x0*x0*x1*y2;
//      double DeltaB=   y1*x2*x2 +y0*x1*x1    +x0*x0*y2    -   x1*x1*y2 -   y0*x2*x2 -x0*x0*y1;
//      double DeltaA=      x1*y2 +x0*y1       +y0*x2       -y1*x2       -x0*y2       -y0*x1;
//      CC=DeltaC/Delta;
//      BB=DeltaB/Delta;
//      AA=DeltaA/Delta;
//      double Q=-(BB+(BB>0?1:-1)*sqrt(BB*BB-4*AA*CC))/2;
//      double t1=Q/AA;
//      double t2=CC/Q;
//      if((t1>met && t1<t2) || t2<met) {
//        t_cutoff=t1;
//      } else {
//        t_cutoff=t2;
//      }
//    }

    // Guide Minor
    g=-mu/(r*r);
    cent=omega*omega*r;
    if(a_[j]>0) { //Otherwise, we are in a coast, just keep the old acceleration to get us through
      g_term=-g/a_[j];
      cent_term=-cent/a_[j];
    }
    tlast = met; // as in PEG
    fhatdotrhat=A_[j]+B_[j]*(met-tlast)+g_term+cent_term;
    //System.out.println("Pitch = " + Math.toDegrees(PI/2-acos(fhatdotrhat)));
    target_pitch=PI/2-acos(fhatdotrhat);
    target_pitch-=eng.ThrAngle;



    CmdP = target_pitch;


}

/*
void PEGMulti::Estimate()
{
    // Estimate staging
    //Estimate vertical state and steering discontinuities at each staging point
   if(T > 0) // one stage before last
	// if(true) // one stage before last
    {
        double rdot_T=vr+b0(1,T)*A+b_(1,1,T)*B;
        r_T=r+vr*T+c_(0,1,T)*A+c_(1,1,T)*B;
        rbar=(r_T+r)/2;
        double f_r=A+((mu/(r*r)-omega*omega*r)/a0);
        double a_T=a(1,T);
        double f_rT=A+B*T+((mu/(r_T*r_T)-omega_T*omega_T*r_T)/a_T);
        double fdot_r=(f_rT-f_r)/T;
        f_theta=1-f_r*f_r/2;
        fdot_theta=-f_r*fdot_r;
        fdotdot_theta=-fdot_r*fdot_r/2;
        h_T=h+rbar*(f_theta*b0(1,T)+fdot_theta*b_(1,1,T)+fdotdot_theta*b_(2,1,T));
        double v_thetaT=h_T/r_T;
        omega_T=v_thetaT/r_T;
        DeltaA_=(mu/(r_T*r_T)-omega_T*omega_T*r_T)*(1/a(1,T)-1/m_stage.accel);
        DeltaB_=(mu/(r_T*r_T)-omega_T*omega_T*r_T)*(1/eng.isp-1/Isp_end)
                +(3*omega_T*omega_T-2*mu/(r_T*r_T*r_T))*rdot_T*(1/a(1,T)-1/m_stage.accel);

		A_end=A+DeltaA_+B_end*T;
        B_end=B+DeltaB_;
        sprintf(oapiDebugString(), "RT = %.02lf", r_T);

    }
    else
    {
        h_T=h;
        omega_T=omega;
        r_T=r;
        T=0;
        DeltaA_=0;
        DeltaB_=0;
    }


    ELEMENTS currentEle;
    double MJD;
    vessel->GetElements(currentEle,MJD);

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
        //Estimate radius at cutoff
    r_T_end=p/(1+eTarget*cos(taTarget));
    //estimate vertical speed at cutoff
    rdot_T_end=sqrt(mu/p)*eTarget*sin(taTarget);

    // Estimate cutoff
    rbar_end=(r_T_end + r_T)/2;

    //Calculate current programmed pitch and approximate pitch rate
    double fr_end = A_end + ((mu/(r*r) -omega_T*omega_T*r) / m_stage.accel);
    double a_T_end = a(2, T_end);
    double fr_T_end = A + B * T +((mu/(r_T_end*r_T_end)-omega_T_end*omega_T_end*r_T_end)/a_T_end);
    double fdotr_end = (fr_T_end-fr_end)/T_end;
    //Calculate downrange thrust component coefficients
    double f_theta_end=1-fr_end*fr_end/2;
    double fdot_theta_end=-fr_end*fdotr_end;
    double fdotdot_theta_end=-fdotr_end*fdotr_end/2;

    //Estimate time of cutoff
    double h_T_end=sqrt(mu*p);
    double deltah=h_T_end-h_T;

    //Calculate required Deltav to gain targeted Deltah
    double N1=deltah/rbar_end;
    double N2a=eng.isp*T_end;
    double N2b=fdot_theta_end+fdotdot_theta_end*tau_end;
    double N2=N2a*N2b;
    double N3=fdotdot_theta_end*Isp_end*T_end*T_end/2;
    double N=N1+N2+N3;
    double D0=f_theta_end;
    double D1=fdot_theta_end*tau_end;
    double D2=fdotdot_theta_end*tau_end*tau_end;
    double D=D0+D1+D2;
    double deltav;
    if(fabs(D)<1.0)
    {
        deltav=N/D; //Normal mode, denominator is sane
    }
    else
    {
        //Something weird with the denominator, just use N1
        //equivalent to assuming rocket is pointing horizontal
        D=1;
        deltav=N1;
    }

    //  deltav2=(deltah/rbar+Isp*T2*(fdottheta+fdotdottheta*tau)+fdotdottheta*Isp*T2*T2/2)/
    //                (ftheta+fdottheta*tau+fdotdottheta*tau*tau);
    //double deltav=deltah/rbar;
    //  if (met<(TPEGMultiStart+5)) {
    T_end=tau_end*(1-exp(-deltav/Isp_end));
    if(T_end>1000)
        T_end=1000;
    //    sprintf(oapiDebugString(),"T: %f",T);
    //    T2=tau*(1-exp(-deltav2/Isp));
    //  } else {
    //    T2=tau*(1-exp(-deltav/Isp));
    //    T=tau*(1-exp(-deltav2/Isp));
    //  }
    //Any comparison with NaN is false, so
    //invert these tests so it's true if NaN
//    if(!(tau>T)|!(0<T))
//    	T=tau-10;
    //  if(!(tau>T)|!(0<T))T=T2;
    metCutoff=T_end+met;
}

void PEGMulti::Guide()
{
    //Calculate pitch steering coefficients
//    double a = b0(T);
//    double b = bn(T,1);
//    double c = c0(T);
//    double d = cn(T,1);
//    double y1 = rdot_T_end-vr;
//    double y2 = r_T-vr*T-r;
//    double D = a*d-b*c;
//    A = (d*y1-b*y2)/D;
//    B = (a*y2-c*y1)/D;
//
//
//    C = (mu/(r*r)-(omega*omega*r))/a0;

    //Guide major
    double aa=b0(1,T)+b0(2,T_end);
    double bb=b_(1,1,T)+b_(1,2,T_end)+b0(2,T);
    double cc=c0(1,T)+c0(2,T_end)+b0(1,T)*T_end;
    double dd=c_(1,1,T)+b_(1,1,T)*T_end+c0(2,T_end)*T+c_(1,2,T_end);
    double k_b=rdot_T_end-vr-b0(2,T_end)*DeltaA_-b_(1,2,T_end)*DeltaB_;
    double k_c=r_T_end-r-vr*(T+T_end)-c0(2,T_end)*DeltaA_-c_(1,2,T_end)*DeltaB_;

    B = (k_c*aa-cc*k_b)/(dd*aa-cc*bb);
    A = k_b/aa-bb*B/aa;

    // Guide minor
    double g=-mu/(r*r);
    double cent=omega*omega*r;
    double g_term = 0, cent_term = 0;
    if(a0>0) { //Otherwise, we are in a coast, just keep the old acceleration to get us through
      g_term=-g/a0;
      cent_term=-cent/a0;
    }
    double TLastAB=met;
    double fhatdotrhat=A+B*(met-TLastAB)+g_term+cent_term;

    if(fabs(fhatdotrhat)>1)
    {
        valid = false;
        //Commanded to do a cos(pitch) out of range, turn
        //off pitch guidance so pilot has a chance to do something
        //"Handing over manual control... Good Luck! :)"
        //   --Eddie, flight computer of the Heart of Gold
//        if(C < 0.5)
//        sprintf(foo, "LESS THRUST NEEDED");
//        else
//        sprintf(foo, "MORE THRUST NEEDED");
    }
    else
    {
        valid = true;
        target_pitch=PI/2-acos(fhatdotrhat);
    }

    target_pitch-=eng.ThrAngle;
    CmdPDot=(target_pitch-last_target_pitch)/TMajorCycle;

    if(met > metCutoff)
        return;
    CmdP=(target_pitch+CmdPDot * 0.25);
}

*/

  //Steering integral: ideal velocity gained during powered flight
   double PEGMulti::b0(int j,double t_j) {
	   double val = -v_e[j]*log(1-t_j/tau_[j]);
    return val;
  }
  //Steering integral: nth moment of b0 about t=0
   double PEGMulti::b_(int n, int j, double t_j) {
    if(n==0) return b0(j,t_j);
    double val =  b_(n-1,j,t_j)*tau_[j]-v_e[j]*pow(t_j,n)/n;

	    return val;
  }
  //Steering integral: ideal distance travelled during powered flight
   double PEGMulti::c0(int j,double t_j) {
    double val =  b0(j,t_j)*t_j-b_(1,j,t_j);

	    return val;
  }
  //Steering integral: nth moment of c0 about t=0
   double PEGMulti::c_(int n, int j, double t_j) {
    if(n==0) return c0(j,t_j);
    double val =  c_(n-1,j,t_j)*tau_[j]-v_e[j]*pow(t_j,n+1)/(n*(n+1));

	    return val;
  }
   double PEGMulti::a(int j, double t_j) {
    double val =  a_[j]/(1-t_j/tau_[j]);

	    return val;
  }
  /*
//Steering integral: ideal velocity gained during powered flight
double PEGMulti::b0(int j,double t)
{
    double val;
    if ( j == 1 )
        val = -eng.isp*log(1-t/tau);
    else
        val = -Isp_end*log(1-t/tau_end);
    return val;
}

//Steering integral: nth moment of b0 about t=0
double PEGMulti::b_(int n, int j, double t)
{
    if(n==0) return b0(j, t);
    double val;
    if ( j == 1 )
        val = b_(n-1, j, t)*tau-eng.isp*pow(t,n)/n;
    else
        val = b_(n-1, j, t)*tau_end-Isp_end*pow(t,n)/n;
    return val;
}
//Steering integral: ideal distance travelled during powered flight
double PEGMulti::c0(int j,double t)
{
    return b0(j, t)*t-b_(1, j, t);
}
//Steering integral: nth moment of c0 about t=0
double PEGMulti::c_(int n, int j, double t)
{
    if(n==0)
        return c0(j, t);
    double val;
    if ( j == 1 )
        val = c_(n-1, j, t)*tau-eng.isp*pow(t,n+1)/(n*(n+1));
    else
        val = c_(n-1, j, t)*tau_end-Isp_end*pow(t,n+1)/(n*(n+1));
    return val;
}

double PEGMulti::a(int j, double t)
{
    double val;
    if ( j == 1 )
        val = a0/(1-t/tau);
    else
        val = m_stage.accel/(1-t/tau_end);

    return val;
}
*/
//
//double PEGMulti::a_end(double t_j)
//{
//    return /(1-t_j/tau);
//}

void PEGMulti::Init()
{
    t0=oapiGetSimTime();
    mu=GGRAV*oapiGetMass(vessel->GetGravityRef());
//    lastT = oapiGetSimTime();
}

