//  ==============================================================================================================================================
//	Copyright (C) 2002 - 2015 Jarmo Nikkanen
//                2016        Andrew Stokes
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>
//
//  ==============================================================================================================================================


// ****************************************************************************
// TODO -- TODO -- TODO -- TODO -- TODO -- TODO -- TODO -- TODO
// Change everything from TrL (true longitude) to TrA (true anomaly) basis.
// **************************************************************************** 

#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOKANJI
#define NOIMAGE
#define NOTAPE

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "OrbiterSDK.h"
#include "BSTools.hpp"
#include "BSReference.hpp"
#include "BSOrbit.hpp"

double Bug,Bug2;
Orbit DebugOrbit;
VECTOR3 BugVector;


// ==============================================================================================================================================
//
Orbit::Orbit()
{
	Reset();
	Ecliptic();
}

// ==============================================================================================================================================
//
Orbit::Orbit(VESSEL *ves)
{
	OBJHANDLE obj=ves->GetHandle();
	OBJHANDLE ref=ves->GetGravityRef();
	Elements(obj,ref);
}

// ==============================================================================================================================================
//
Orbit::Orbit(OBJHANDLE body,OBJHANDLE ref) 
{
	Elements(body,ref);
}

// ==============================================================================================================================================
//
Orbit::Orbit(char *bo,char *re) 
{
	Elements(bo,re);
}

// ==============================================================================================================================================
//
Orbit::Orbit(VECTOR3 r, VECTOR3 v,double myy)
{
	Elements(r,v,myy);
}

// ==============================================================================================================================================
//
Orbit::~Orbit()
{	
}

// ==============================================================================================================================================
//
void Orbit::Reset()
{
	LeoRef=NULL;
	GeoRef=NULL;
	EclipticPlane=false;
	displacement=_V(0,0,0);
	intersection=NULL;	
	node_trl_o=NULL;
	SetTime(0);
}

// ==============================================================================================================================================
//
void Orbit::Displacement(VECTOR3 d)
{
	displacement=d;
}

// ==============================================================================================================================================
//
void Orbit::Elements(OBJHANDLE body,OBJHANDLE ref) 
{
	VECTOR3 r,v;

  if (body==NULL || ref==NULL) return;

	double myy = oapiGetMass(ref) * GC;

	oapiGetRelativePos(body,ref,&r);
	oapiGetRelativeVel(body,ref,&v);

	Elements(r,v,myy);
}

// ==============================================================================================================================================
//
void Orbit::Elements(char *bo,char *re) 
{
	VECTOR3 r,v;

	OBJHANDLE body=oapiGetObjectByName(bo);
	OBJHANDLE ref=oapiGetObjectByName(re);

	if (body==NULL || ref==NULL) return;

	double myy = oapiGetMass(ref) * GC;

	oapiGetRelativePos(body,ref,&r);
	oapiGetRelativeVel(body,ref,&v);

	Elements(r,v,myy);
}

// ==============================================================================================================================================
//
void Orbit::Elements(VECTOR3 r, VECTOR3 v,double m,bool reset_proj)
{	
	Reset();

	if (fabs(r.x)<0.01) r.x=0;	if (fabs(r.y)<0.01) r.y=0;
	if (fabs(r.z)<0.01) r.z=0;	if (fabs(v.x)<0.01) v.x=0;
	if (fabs(v.y)<0.01) v.y=0;	if (fabs(v.z)<0.01) v.z=0;

	vv=v; rv=r;

	double vel2;
	VECTOR3 z; z.x=0; z.z=0; z.y=1.0;
   
	vel2 = dotp(v,v);
	vel  = sqrt(vel2);
	rad  = length(r);
	myy  = m;

	// Computer normal vector and vector pointing ascenting node
	norv	= crossp(r,v);
	ang		= length(norv);
	norv	= norv*(1.0/ang);

	VECTOR3 ANv = unit(crossp(z,norv));
    
	// Inclination
	inc = acos(-norv.y);
	par = ang*ang/myy; 
	majv =( r * (vel2-myy/rad) ) - (v * dotp(r,v));

	double ml=length(majv);

	ecc  = ml/myy;
	
	if (ecc<1e-6) ecc=0;
	if (inc<1e-6) inc=0;

	sma = par / (1.0-ecc*ecc);

	r=r*(1.0/rad);					// Make the radius vector to unit size, After computing ecc-vector

	if (inc==0) ANv=_V(1,0,0);		// Place ANv to vernal equinox
	if (ecc!=0) majv=majv*(1.0/ml); // Make the major vector to unit size	
	else majv=ANv;					// Place major vector to ascenting node
	
	// Longitude of ascenting node
	if (inc!=0) {
		double  x=ANv.x;
		if (x>=1) {
			lan=0;
		} else if (x<=-1) {
			lan=PI;
		} else {		
			lan=acos(x);		
			if (ANv.z<0) lan=PI2-lan;		
		}
	} else {
		lan=0.0;
	}
	
	// Argument of Periapsis
	if (inc!=0 && ecc!=0) {
		double x=dotp(ANv,majv);
		if (x>1) x=1; // Avoid some precision problems
		else if (x<-1) x=-1;
		agp=acos(x);
		if (majv.y<0) agp=PI2-agp;
	} else if (ecc!=0) {
		agp=acos(majv.x);
		if (majv.z<0) agp=PI2-agp;
	}
	else agp=0.0;

  // True anomaly
	if (ecc!=0) {
		double x=dotp(majv,r);
		if      (x>=1)  tra=0;
		else if (x<=-1) tra=PI;
		else {
			tra=acos(x);
			x=dotp(r,v);
			if (fabs(x)<1e-6) x=0; // Avoid some precision problems
			if (x<0) tra=PI2-tra;
		}
	} else if (inc!=0) {
		tra=acos(dotp(ANv,r));
		if (dotp(ANv,v)>0) tra=PI2-tra;
	} else {
		tra=acos(r.x);
		if (v.x>0) tra=PI2-tra;
	}

	lpe=limit(agp+lan);				// Longitude of Periapsis
	trl=limit(lpe+tra);				// True longitude
	minv=unit(crossp(norv,majv));	// Minor axis vector
	mna=tra2mna(tra,ecc);			// Mean anomaly
	mnm=MeanMotion();

	Xmajv = majv * sma;
	Xminv = minv * sqrt(fabs(sma*par));
	Xcv	  = majv * (sma*ecc);

	if (reset_proj) SetProjection(this);
}

// ==============================================================================================================================================
//
void Orbit::CreateProjectionOrbit(VECTOR3 normal)
{	
	majv = unit(crossp(crossp(normal,zero),normal));
	norv = unit(normal);
	minv = unit(crossp(norv,majv));
	lan = 0.0;
	agp = 0.0;
	tra = 0.0;
	sma = AU;
	par = AU; 
	lpe = lan;
	trl = lan;
	Xmajv = majv * sma;
	Xminv = minv * sqrt(fabs(sma*par));
	Xcv	  = majv * (sma*ecc);
}

// ==============================================================================================================================================
//
void Orbit::SetOrbitPosition(double t)
{
	VECTOR3 x=Position(t), xx=Tangent(t);
	rv=x, vv=xx;
	trl = t;
	tra = limit(trl-lpe);
	mna = tra2mna(tra,ecc);
}

// ==============================================================================================================================================
//
void Orbit::Ecliptic()
{	
	if (EclipticPlane) return;
	EclipticPlane=true;

	Reset();

	myy = 1.327e20;
	ecc = 0;
	sma = AU;
	rad = AU;
	inc = 0;
	lan = 0;
	agp = 0;
	tra = 0;
	par = sma;
	mna = 0;
	mnm = MeanMotion();
	ang = sqrt(myy*par);
	lpe = 0;
	trl = 0;

	majv  = _V(1,0,0);
	minv  = _V(0,0,1);
	norv  = _V(0,-1,0);
	Xmajv = majv * sma;
	Xminv = minv * sma;
	Xcv	  = _V(0,0,0);

	SetProjection(this);
}

// ==============================================================================================================================================
//
void Orbit::SetNULL()
{
	sma=0;
}

// ==============================================================================================================================================
//
double Orbit::Energy()
{
	return -myy/(2*sma);
}

// ==============================================================================================================================================
//
bool Orbit::Defined()
{
	if (sma!=0) return true;
	return false;
}

// ==============================================================================================================================================
//
void Orbit::LEO(OBJHANDLE ref)
{	
	if (!ref) {	LeoRef=NULL; return; }

	double obli  = oapiGetPlanetObliquity(ref);
	double peri  = oapiGetPlanetPeriod(ref);
	double trans = oapiGetPlanetTheta(ref);

	VECTOR3 rota, refd;
	PlanetAxis(obli,trans,&rota,&refd);

	VECTOR3 velo = crossp(rota,refd);
	double  myy  = oapiGetMass(ref)*GC;
	double  rad  = oapiGetSize(ref);
	double  vel  = sqrt(myy/rad);

	refd=set_length(refd,rad);
	velo=set_length(velo,vel);
	if (peri<0) velo=_V(0,0,0)-velo;

	Elements(refd,velo,myy);
	LeoRef=ref;
}

// ==============================================================================================================================================
//
void Orbit::GEO(OBJHANDLE ref)
{	
	if (!ref) {	GeoRef=NULL; return; }

	double obli  = oapiGetPlanetObliquity(ref);
	double peri  = oapiGetPlanetPeriod(ref);
	double trans = oapiGetPlanetTheta(ref);

	VECTOR3 rota, refd;
	PlanetAxis(obli,trans,&rota,&refd);

	VECTOR3 velo = crossp(rota,refd);
	double  myy  = oapiGetMass(ref)*GC;
	double  rad  = pow(fabs(peri)*sqrt(myy)/PI2, 2.0 / 3.0);
	double  vel  = sqrt(myy/rad);

	refd=set_length(refd,rad);
	velo=set_length(velo,vel);
	if (peri<0) velo=_V(0,0,0)-velo;

	Elements(refd,velo,myy);
	GeoRef=ref;
}


// ==============================================================================================================================================
// Radius vector will point in vernal equinox
//
void Orbit::CreateNewCircularOrbit(Orbit *plane, double rad)
{
	double mu=plane->myy;
	double vcir=sqrt(mu/rad);
	VECTOR3 rv=plane->Position(0);
	VECTOR3 vv=crossp(plane->norv,rv);
	Elements(set_length(rv,rad),set_length(vv,vcir),mu,false);
}





// ==============================================================================================================================================
// Define orbit by using ship's position vector and Periapsis vector 
//
bool Orbit::ApproachOrbit(OBJHANDLE ref, VECTOR3 pos, VECTOR3 peri)
{
	VECTOR3 normal = crossp(pos,peri);
	VECTOR3 minv   = crossp(normal,peri);

	double x,y;
	double tra = nangle(pos,peri,normal);
	double myy = oapiGetMass(ref) * GC;
	double ped = length(peri);
	double rad = length(pos);
	double ecc = ( ped - rad ) / ( rad*cos(tra)-ped );  // Eccentricity of orbit	
	double sma = ped/(1.0-ecc); // Semi-major axis
	double par = sma * (1.0-ecc*ecc); // Parameter
	double vel = sqrt(2.0*myy/rad - myy/sma); // Ship's velocity
	double smi = sqrt( fabs(sma) * par );
	double eca = tra2eca(tra,ecc);	

	if (ecc<1) { 
		x = -sma * sin(eca);
		y =  smi * cos(eca);
	} else {
		x = sma * sinh(eca);
		y = smi * cosh(eca);
	}
  
	double l = sqrt(x*x + y*y);

	VECTOR3 an = unit(peri) * (vel * x / l);
	VECTOR3 di = unit(minv) * (vel * y / l);
	
	Elements(pos,an+di,myy,false);

	return true;
}


// ==============================================================================================================================================
//
bool Orbit::DefineOrbit(OBJHANDLE ref, VECTOR3 pos, VECTOR3 normal,double tra,double ecc)
{

	VECTOR3 ma  = create_vector(normal,pos,PI2-tra);
	VECTOR3 mi  = unit(crossp(normal,ma));

	double x,y;
	double myy = oapiGetMass(ref) * GC;
	double rad = length(pos);
	double par = rad * (1.0+ecc*cos(tra));
	double sma = par / (1.0-ecc*ecc);
	double vel = sqrt(2.0*myy/rad - myy/sma); // Ship's velocity	
	double smi = sqrt( fabs(sma) * par );
	double eca = tra2eca(tra,ecc);	

	INVALIDD(par*sma*vel) return false;

	if (ecc<1.0) { 
		x = -sma * sin(eca);
		y =  smi * cos(eca);
	} else {
		x = sma * sinh(eca);
		y = smi * cosh(eca);
	}
  
	double l = sqrt(x*x + y*y);

	VECTOR3 an = ma * (vel * x / l);
	VECTOR3 di = mi * (vel * y / l);
	
	Elements(pos,an+di,myy,false);

	return true;
}


// ==============================================================================================================================================
//
void Orbit::Make2DimensionalWithEcliptic()
{
	if (Defined()) {
		VECTOR3 zero=_V(1,0,0);
		VECTOR3 normal=_V(0,-1,0);
		VECTOR3 ANv = create_vector(normal,zero,lan);
		
		double an = tra2gamma(tra,ecc);		
	
		// Compute new major axis 
		VECTOR3 newmaj = create_vector(normal,ANv,agp);
		// Compute new position vector
		VECTOR3 newpos = create_vector(normal,newmaj,tra);
		// Compute new velocity vector
		VECTOR3 newvel = create_vector(normal,newpos,an);
	
		// Setup lengths
		newpos=set_length(newpos,rad);
		newvel=set_length(newvel,vel);

		Elements(newpos,newvel,myy,false);
	}
}
		
// ==============================================================================================================================================
// Define planet excape orbit, Pos = vessel position, Esc = Escape vector, 
//
void Orbit::EscapeOrbit(OBJHANDLE ref,VECTOR3 Pos,VECTOR3 Esc,VECTOR3 normal)
{
	double myy=GC*oapiGetMass(ref);
	double rad=length(Pos);
	double Esc2=dotp(Esc,Esc);
	double ang=nangle(Pos,Esc,normal);
	double sma = -myy / Esc2;  // (constant)
	double sq=sin(ang);
	double q=rad*rad*sq*sq;
	double w=2.0*sma*rad*(cos(ang)-1.0);
	double ecc=sqrt((q-rad*sq*sqrt(q+2.0*w)+w)/(2.0*sma*sma)+1.0);
	double par=sma*(1.0-ecc*ecc);
 
	// Compute Tangential angle 
	double x=(par-rad)/(rad*ecc);
	if (x>1) x=1; if (x<-1) x=-1; 

	double tra=acos(x);
	double y=PI+acos(1.0/ecc);
	if (ang<y) tra=PI2-tra;

	double ta=tra2gamma(tra,ecc);

	VECTOR3 Vel=create_vector(normal,Pos,ta);
	Vel=set_length(Vel,sqrt(2*myy/rad - myy/sma));

	Elements(Pos,Vel,myy);
}

// ==============================================================================================================================================
//
void Orbit::SetProjection(VECTOR3 z, VECTOR3 n)
{
	zero=unit(z);
	nine=unit(n);
}

// ==============================================================================================================================================
//
void Orbit::CreateProjectionPlane(VECTOR3 normal, VECTOR3 zero)
{	
	Reset();

	myy = 1.327e20;
	ecc = 0;
	sma = AU;
	rad = AU;
	inc = 0;
	lan = 0;
	agp = 0;
	tra = 0;
	par = sma;
	mna = 0;
	mnm = MeanMotion();
	ang = sqrt(myy*par);
	lpe = 0;
	trl = 0;

	majv  = unit(crossp(crossp(normal,zero),normal));
	norv  = unit(normal);
	minv  = unit(crossp(norv,majv));
	Xmajv = majv * sma;
	Xminv = minv * sma;
	Xcv	  = _V(0,0,0);

	SetProjection(this);
}




// ==============================================================================================================================================
// Hyberbolic use only
// ReCalculate orbit position (mna,tra,trl,rad,vel)
//
void Orbit::SetTimeToPeriapsis(double t)
{
	if (Defined()) {
		mna = -t * MeanMotion();
		tra = mna2tra(mna,ecc);
		trl = limit(tra+lpe);
		vv  = Tangent(trl);
		rv  = Position(trl);
		rad = length(rv);
		vel = length(vv);
	}
}

// ==============================================================================================================================================
//
void Orbit::SetTime(double t)
{
	if (Defined()) {
		timeto=0;
		time=oapiGetSimMJD()+(t/86400);
	}
}

// ==============================================================================================================================================
//
double Orbit::GetTime()
{
	if (Defined()) {
		if (timeto>0) return timeto;
		timeto=(time-oapiGetSimMJD())*86400;
		if (timeto<0) timeto=0;
		return timeto;
	}
	else return 0;
}


// ==============================================================================================================================================
//
double Orbit::MeanMotion()
{
	if (Defined()) return(sqrt(myy/(fabs(sma*sma*sma))));
	return 0;
}


// ==============================================================================================================================================
//
double Orbit::Period()
{
	if (Defined()) return (PI2*sqrt(fabs(sma*sma*sma)/myy));
	return 0;
}

// ==============================================================================================================================================
//
double Orbit::TimeToPeriapsis()
{
	if (Defined()) {
		// For ellipse
		if (ecc<1.0) return( (PI2-mna) / mnm );

		// For hyperbola
		return( (-mna) / mnm );	
	}
	return 0;
}

// ==============================================================================================================================================
//
double Orbit::TimeToNode(Orbit *tgt, double *trl_of_node)
{
	if (Defined()) {
		double hpa=TrlOfNode(tgt);
		double hpd=limit(hpa+PI);

		// Avoid oscilation when the ship is at the node.
		if (fabs(hpa-trl)<1e-6) hpa=hpd;
		if (fabs(hpd-trl)<1e-6) hpd=hpa;

		double tn;
		double a=TimeTo(hpa);
		double d=TimeTo(hpd);
		
		if (ecc>1.0) {
			if (a>0 && d>0) tn=MIN(a,d);
			else tn=MAX(a,d);
		}
		else tn=MIN(a,d);
		
		if (trl_of_node) {
			if (tn==a) *trl_of_node=hpa;
			else if (tn==d) *trl_of_node=hpd;
			else *trl_of_node=0;
		}
		return(tn);
	}
	return 0;
}

// ==============================================================================================================================================
//
double Orbit::TimeBetween(double trla, double trlb)
{
	double traa=limit(trla-lpe);
	double trab=limit(trlb-lpe);

	if (ecc<1.0) {
		double a = angular_distance(tra2mna(traa,ecc), tra2mna(trab,ecc));
		return (a/mnm);
	}
	
	double max = MaxTra();

	if (traa>max && traa<(PI2-max)) return(0);
	if (trab>max && trab<(PI2-max)) return(0);

	double a = tra2mna(trab,ecc) - tra2mna(traa,ecc);	
	return(a / mnm);
}

// ==============================================================================================================================================
//
double Orbit::PeriapsisVelocity()
{
	if (Defined()) return( sqrt( myy * (1.0+ecc) / (sma * (1.0-ecc)) ) );
	return 0;
}

// ==============================================================================================================================================
//
double Orbit::TangentialAngle(double longitude)
{
	if (ecc==0) return PI05;
	return( tra2gamma( limit(longitude-lpe), ecc) );
}

// ==============================================================================================================================================
//
double Orbit::FlightPathAngle(double longitude)
{
	if (ecc==0) return 0;
	return( tra2fpa( limit(longitude-lpe), ecc) );
}

// ==============================================================================================================================================
//
double Orbit::PeriapsisDistance()
{
	return(sma*(1.0-ecc));
}

// ==============================================================================================================================================
// There are no Aopapsis on hyperbola but
// return value anyway
double Orbit::AopapsisDistance()
{
	return(sma*(1.0+ecc));
}

// ==============================================================================================================================================
//
double Orbit::TrlOfNode(Orbit *tgt)
{
	return TrlOfNode(tgt->norv);
}

// ==============================================================================================================================================
//
double Orbit::TrlOfNode(VECTOR3 tgt_norv)
{
	if (Defined()) {
		VECTOR3 z; z.x=0; z.z=0; z.y=1.0;
		VECTOR3 vlan,vect;

		if (length(crossp(z,norv))==0) vlan=majv, vect=minv;
		else {
			vlan = unit(crossp(z,norv));
			vect = unit(crossp(norv,vlan));
		}

		VECTOR3 lv = crossp(norv, tgt_norv);
		
		double xf=dotp(lv,vect);                
		double xl=dotp(lv,vlan); 
			
		return limit(atan2(xf,xl)+lan);	
	}
	return 0;
}


// ==============================================================================================================================================
// Tangent vector is also velocity vector
//
VECTOR3 Orbit::Tangent(double longitude)
{	
	double tr=limit(longitude-lpe); // Must be limited  
	double up=sqrt(myy/par);
	return majv*(-sin(tr)*up) + minv*((ecc+cos(tr))*up);
}


// ==============================================================================================================================================
//
VECTOR3 Orbit::Position(double longitude)
{
	double n=limit(longitude-lpe);
	double cos_n=cos(n); 
	double r = par / ( 1.0 + ecc * cos_n ); 
	
	// This is wrong. 
	if (r<0) r=-r;

	return(majv * (r * cos_n) + minv * (r * sin(n)));
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::EscapeAsymptote()
{
	if (ecc<1.0) return _V(0,0,0);
	double n=limit(PI-acos(1.0/ecc));
	return(majv * cos(n) + minv * sin(n));
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::Asymptote()
{
	if (ecc<1.0) return _V(0,0,0);
	double n=limit(PI+acos(1.0/ecc));
	return(majv * cos(n) + minv * sin(n));	
}

// ==============================================================================================================================================
//
double Orbit::TimeTo(double longitude)
{   
	if (longitude==trl) return GetTime();

	double tr=limit( longitude - lpe );

	// For Hyperbola
	if (ecc>1.0) {
		double max=MaxTra();
		if (tr>max && tr<(PI2-max)) return(-10e20);
		double ang = tra2mna(tr,ecc) - mna;	
		return(GetTime() + (ang / mnm) );
	}

    // For Ellipse
	double ang=angular_distance(mna, tra2mna(tr,ecc));	
	return( GetTime() + (ang / mnm) );
}
  
// ==============================================================================================================================================
// Return negative value after point passage
//
double Orbit::TimeToPoint(double longitude)
{   
    double tr=limit( longitude - lpe );

	// For Hyperbola
	if (ecc>1.0) {
		double max=MaxTra();
		if (tr>max && tr<(PI2-max)) return(-10e20);
		double ang = tra2mna(tr,ecc) - mna;	
		return(GetTime() + (ang / mnm) );
	}


    // For Ellipse
	double ang = angular_distance(mna, tra2mna(tr,ecc));
	if (ang>PI) return -(PI2-ang)/mnm;
	return( ang/mnm );
}
  
// ==============================================================================================================================================
//
bool Orbit::IsValid(double longitude)
{   
	if (ecc>1.0) {
		double tr=limit( longitude - lpe );
		double max=MaxTra();
		if (tr>max && tr<(PI2-max)) return false;
	    return true;
	}
	return true;
}
    
// ==============================================================================================================================================
//
double Orbit::TrlByRadius(double r)
{
	double z = (par-r) / (r*ecc);
	if (z>1.0) z=1.0;
	if (z<-1.0) z=-1.0;
	return limit( acos(z) + lpe );
}

// ==============================================================================================================================================
//
double Orbit::TrlByTime(double ti)
{
	if (ti==0) return trl;
	if (ecc<1.0) { 
		return ( limit( mna2tra(limit(mna + ti * mnm), ecc) + lpe ) );
	} else {
		return ( limit( mna2tra(      mna + ti * mnm , ecc) + lpe ) ); // Can not limit with hyperbola
	}
}

// ==============================================================================================================================================
//
double Orbit::Radius(double longitude)
{  
	if (ecc==0) return par;
	double v = par / ( 1.0 + (ecc * cos( limit(longitude - lpe) ) ) ); 
	if (v<0.0) return -v;
	return v;
}

// ==============================================================================================================================================
//
double Orbit::Velocity(double longitude)
{
	double r = par / ( 1.0 + (ecc * cos( limit(longitude - lpe) ) ) ); 
	return( sqrt( ( 2.0 * myy / r) - (myy / sma)) ); 		
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::Velocity()
{
	return vv;
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::Position()
{
	return rv;
}

// ==============================================================================================================================================
//
double Orbit::VelocityByRadius(double r)
{
	return( sqrt( ( 2 * myy / r) - (myy / sma)) );
}

// ==============================================================================================================================================
//
double Orbit::SemiMinor()
{
	if (ecc<1.0) return sqrt( sma * par );
	return sqrt(-sma * par);	
}



// ==============================================================================================================================================
// [Private]
double Orbit::MaxTra()
{
	double r=50*AU;
	return(acos( (-sma*(ecc*ecc-1.0)) / (ecc*r) - (1.0/ecc) ));
}


// ==============================================================================================================================================
// pr = unit vector[in], rad[out]=orbital radius, height[out]=out of plain angle?, angle[out]=true longitude
//
void Orbit::Longitude(VECTOR3 pr, double *rad, double *height, double *angle)
{	
	double	xf=dotp(pr,minv); 
	double	xl=dotp(pr,majv);
    
	if (rad)	*rad   = sqrt((xl*xl) + (xf*xf));  	    
	if (height) *height= dotp(pr,norv);
	if (angle)	*angle = limit(atan2(xf,xl)+lpe);
}

// ==============================================================================================================================================
// Return True Longitude (TrL) of a specified vector
//
double Orbit::Longitude(VECTOR3 pr)
{	
	double	xf=dotp(pr,minv); 
	double	xl=dotp(pr,majv);
	return limit(atan2(xf,xl)+lpe);
}

// ==============================================================================================================================================
// Translate TrL from one orbit to an other co-planar orbit
//
double Orbit::Translate(Orbit *tgt, double longitude)
{
	VECTOR3 lv=Position(longitude);	
	double xf=dotp(lv,tgt->minv); 
	double xl=dotp(lv,tgt->majv); 
	return limit(atan2(xf,xl)+tgt->lpe);
}

// ==============================================================================================================================================
//
double Orbit::IntersectionInterpolate(Orbit *tgt,double start,double end,bool dir)
{
	int i;
	bool ok=false;
	double dif;
	double gamma,sector,aa,da,db,first=0;
	VECTOR3 v;

	gamma=start;
	sector=end;

	if (dir) sector=-sector;

	sector/=2;

	v=tgt->Position(gamma);
	Longitude(v,&db,NULL,&aa);
	da=Radius(aa);
	first=da-db;

	gamma+=sector;

	for (i=0;i<30;i++) {  
	      
		gamma=limit(gamma);
		v=tgt->Position(gamma);
		Longitude(v,&db,NULL,&aa);
		da=Radius(aa);
		dif=da-db;
	
		if ((first*dif)<0) sector=-sector, ok=true;
		first=dif;
		
		sector/=2;
		gamma+=sector;
        
	}

	if (!ok) return -1.0;

	return(tgt->Translate(this,limit(gamma)));
}




// ==============================================================================================================================================
// Longitudes are on source plane
// [Private]
void Orbit::IntersectionAll(Orbit *tgt, double *lpa, double *lpd)
{   
	if (!Defined()) return;

	double start,end;
	
	start = tgt->lpe;
	end = PI;

	double e=tgt->ecc;

	if (e>1.0) end = tgt->MaxTra();
	
	if (lpa && lpd) {		
		*lpa=IntersectionInterpolate(tgt,start,end,false);	
		*lpd=IntersectionInterpolate(tgt,start,end,true);		
	}
	
}	

// ==============================================================================================================================================
//
double Orbit::TrlOfIntersection(Orbit *tgt, bool point)
{
	IntersectionAll(tgt, &first, &second);
	if (point) return(second);
	return(first);
}

// ==============================================================================================================================================
//
double Orbit::TimeToIntersection(Orbit *tgt, bool point)
{
	return(TimeTo(TrlOfIntersection(tgt,point)));
}

// ==============================================================================================================================================
//
void Orbit::SetProjection(Orbit *p)
{	
	zero = unit(p->Position(0));
	nine = unit(p->Position(90*RAD));
}

// ==============================================================================================================================================
//
void Orbit::DrawPlaneIntersection(HDC hDC,Orbit *t,double cx,double cy,double zoom,int color,bool box)
{
	if (!Defined()) return;

	double x,y,x2,y2,hpa,s,hpd;
	double w=cx*2;
	double h=cy*2;

	if (t==NULL) return;

	HPEN pen=CreatePen(PS_DOT,1,color);  
	HPEN spen=CreatePen(PS_SOLID,1,color); 
	HBRUSH brush=CreateSolidBrush(color);
	HBRUSH brushb=CreateSolidBrush(0);

	hpa=TrlOfNode(t);
	hpd=limit(hpa+PI);

	if (hpa!=-1 && hpd!=-1) {
        
		s=3.0;

		if (length(displacement)>0) Point(displacement,cx,cy,zoom,&cx,&cy);
		
		Point(cx,cy,hpd,&x,&y,zoom);
		Point(cx,cy,hpa,&x2,&y2,zoom);

		SelectObject(hDC,pen);
		DrawLine(hDC,x,y,x2,y2,w,h,false);

		SelectObject(hDC,spen);
		SelectObject(hDC,brushb);
		if (box) DrawRectangle(hDC,x-s,y-s,x+s,y+s,w,h);
	
		SelectObject(hDC,brush);
		if (box) DrawRectangle(hDC,x2-s,y2-s,x2+s,y2+s,w,h);	
	}

	SelectObject(hDC,GetStockObject(NULL_BRUSH));
	SelectObject(hDC,GetStockObject(WHITE_PEN));
  
	DeleteObject(pen); 
	DeleteObject(spen);  
	DeleteObject(brush);
	DeleteObject(brushb);
}


// ==============================================================================================================================================
//
void Orbit::DrawPlaneIntersection(HDC hDC,double hpa,double cx,double cy,double zoom,int color,bool box)
{
  if (!Defined()) return;

	double x,y,x2,y2,s,hpd;
	double w=cx*2;
	double h=cy*2;


	HPEN pen=CreatePen(PS_DOT,1,color);  
	HPEN spen=CreatePen(PS_SOLID,1,color); 
	HBRUSH brush=CreateSolidBrush(color);
	HBRUSH brushb=CreateSolidBrush(0);

	hpd=limit(hpa+PI);

	if (hpa!=-1 && hpd!=-1) {
        
		s=3.0;

		if (length(displacement)>0) Point(displacement,cx,cy,zoom,&cx,&cy);
		
		Point(cx,cy,hpd,&x,&y,zoom);
		Point(cx,cy,hpa,&x2,&y2,zoom);

		SelectObject(hDC,pen);
		DrawLine(hDC,x,y,x2,y2,w,h,false);

		SelectObject(hDC,spen);
		SelectObject(hDC,brushb);
		if (box) DrawRectangle(hDC,x-s,y-s,x+s,y+s,w,h);
	
		SelectObject(hDC,brush);
		if (box) DrawRectangle(hDC,x2-s,y2-s,x2+s,y2+s,w,h);	
	}

	SelectObject(hDC,GetStockObject(NULL_BRUSH));
	SelectObject(hDC,GetStockObject(WHITE_PEN));
  
	DeleteObject(pen); 
	DeleteObject(spen);  
	DeleteObject(brush);
	DeleteObject(brushb);
}

// ==============================================================================================================================================
//
void Orbit::Point(VECTOR3 pr,double xp,double yp,double s,double *x,double *y)
{
	*x = xp + (dotp(pr,zero) * s);
	*y = yp - (dotp(pr,nine) * s);	
}

// ==============================================================================================================================================
//
void Orbit::Point(double xp,double yp,double l,double *x,double *y,double s)
{
	Point(Position(l),xp,yp,s,x,y);
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::PositionByEcA(double t)
{		
	if (ecc<1.0) return Xmajv*cos(t) - Xcv + Xminv*sin(t);
	else return Xmajv*cosh(t) - Xcv + Xminv*sinh(t);
}

// ==============================================================================================================================================
//
VECTOR3 Orbit::Projection(VECTOR3 pr)
{
	return pr-(norv*dotp(pr,norv));
}


// ==============================================================================================================================================
// This will draw an orbit 
// w,h = MFD Center Size, size = scale factor
// What To Draw:  pe = Periapsis, ra = radius line
//
void Orbit::GDIDraw(HDC hDC, DWORD color, double w, double h, double size, bool pe, bool ra) 
{
	GDIDrawSector(hDC,color,w,h,size,0,PI2,pe,ra);
}


// ==============================================================================================================================================
//
void Orbit::GDIDrawSector(HDC hDC, DWORD color, double w, double h, double size, double start, double end, bool pe, bool ra) 
{	
    
	double cx=w/2.0, cy=h/2.0;
	double x,y;
	double r=3.0;

	HPEN spen=CreatePen(PS_SOLID,1,color); 
	HBRUSH brush=CreateSolidBrush(color);
	HBRUSH brushb=CreateSolidBrush(0);

	bool ap=pe;
	double ang = angular_distance(start,end);
	
	if (angular_distance(start,limit(trl-lpe))>ang) ra=false;
	if (angular_distance(start,0.0)>ang) pe=false;
	if (ecc<1.0) if (angular_distance(start,PI)>ang) ap=false;


	// HYPERBOLIC ORBIT	

	if (ecc>1.0) {

		if (length(displacement)>0) {
			Point(displacement,cx,cy,size,&x,&y);
			cx=x, cy=y;
		}
		

		double ang = angular_distance(start,end);
		double mx  = MaxTra();
		double m   = PI2-mx;
		double am  = angular_distance(start,m);

		bool single=false;
		if (difference(start,PI)<(PI-mx)) single=true;
		if (difference(end,PI)<(PI-mx)) single=true;

		if (ang>am && single==false) { // Dual Sectors
			GDIDrawHyperbolaSector(hDC, color, cx, cy, w, h, size, start, mx);
			GDIDrawHyperbolaSector(hDC, color, cx, cy, w, h, size, m, end);
		}	else { //Single Sector
			if (difference(start,PI)<(PI-mx)) start=m;
			if (difference(end,PI)<(PI-mx)) end=mx;
			GDIDrawHyperbolaSector(hDC, color, cx, cy, w, h, size, start, end);
		}
	
		SelectObject(hDC,spen);

		// Radius
		if (ra) {
			Point(Position(trl),cx,cy,size,&x,&y); 
			DrawLine(hDC,x,y,cx,cy,w,h,false);
		}
    
		// Periapsis
		if (pe) {	
			SelectObject(hDC,brush);
			Point(Position(lpe),cx,cy,size,&x,&y); 	
			DrawEllipse(hDC,x-r,y-r,x+r,y+r,w,h);
		}
	}


	// ELLIPTIC ORBIT

	if (ecc<1.0) { 
      
		if (length(displacement)>0) {
			Point(displacement,cx,cy,size,&x,&y);
			cx=x, cy=y;
		}
		
		GDIDrawEllipseSector(hDC, color, cx, cy, w, h, size, start, end);

		SelectObject(hDC,spen);

		// Radius
		if (ra) {
			Point(Position(trl),cx,cy,size,&x,&y); 
			DrawLine(hDC,x,y,cx,cy,w,h,false);
		}
    
		// Periapsis
		if (pe) {	
			SelectObject(hDC,brush);
			Point(Position(lpe),cx,cy,size,&x,&y); 	
			DrawEllipse(hDC,x-r,y-r,x+r,y+r,w,h);
		}

		// Aopapsis
		if (ap) {	
			SelectObject(hDC,brushb);
			Point(Position(lpe+PI),cx,cy,size,&x,&y); 	
			DrawEllipse(hDC,x-r,y-r,x+r,y+r,w,h);
		}
	}

	SelectObject(hDC,GetStockObject(NULL_BRUSH));
	SelectObject(hDC,GetStockObject(WHITE_PEN));
  
	DeleteObject(spen);  
	DeleteObject(brush);
	DeleteObject(brushb);
}


// ==============================================================================================================================================
// [Private]
//
void Orbit::GDIDrawEllipseSector(HDC hDC, DWORD color, double cx, double cy, double w, double h, double size, double start, double end)
{
	double x,y,ox,oy;
	int i;
	double qq;

	HPEN pen1 = CreatePen(PS_SOLID,1,color);
	SelectObject(hDC, pen1);

	start=tra2tan(start,ecc);
	end=tra2tan(end,ecc);

	double ang = angular_distance(start,end);
	int count  = (int)ceil(ang/(PI/180.0));
	if (count==0) count++;
	
	qq = ang/((double)count);
	

	Point(PositionByEcA(tan2eca(start,ecc)),cx,cy,size,&ox,&oy);
	start+=qq;
	
	for(i=0;i<count;i++) {			
		if (start>PI2) start-=PI2;				
		Point(PositionByEcA(tan2eca(start,ecc)),cx,cy,size,&x,&y);
		DrawLine(hDC,x,y,ox,oy,w,h,false);
		start += qq;
		ox=x, oy=y;
	}

	SelectObject(hDC,GetStockObject(WHITE_PEN));
	DeleteObject(pen1);
}

// ==============================================================================================================================================
//
void Orbit::GDIDrawHyperbolaSector(HDC hDC, DWORD color, double cx, double cy, double w, double h, double size, double start, double end)
{
	double x,y,ox,oy;
	int i;
	double qq;

	HPEN pen1 = CreatePen(PS_SOLID,1,color);
	SelectObject(hDC, pen1);

	start=tra2tan(start,ecc);
	end=tra2tan(end,ecc);

	double ang = angular_distance(start,end);
	int count  = (int)ceil(ang/(PI/180.0));
	if (count==0) count++;
	
	qq = ang/((double)count);
	

	Point(PositionByEcA(tan2eca(start,ecc)),cx,cy,size,&ox,&oy);
	start+=qq;
	
	for(i=0;i<count;i++) {			
		if (start>PI2) start-=PI2;				
		Point(PositionByEcA(tan2eca(start,ecc)),cx,cy,size,&x,&y);
		DrawLine(hDC,x,y,ox,oy,w,h,false);
		start += qq;
		ox=x, oy=y;
	}

	SelectObject(hDC,GetStockObject(WHITE_PEN));
	DeleteObject(pen1);
}


// ==============================================================================================================================================
//
void Orbit::GDIDrawLineOfNodes(HDC hDC, double width, double height, double hpa, DWORD color, double cx, double cy, double zoom, bool box, bool line)
{
	double x,y,ox,oy;
	double hpd = limit(hpa+PI);
	VECTOR3 pos;
	double  w = width;
	double  h = height;

	HPEN pen=CreatePen(PS_DOT,1,color);  

	if (length(displacement)>0) {
		Point(displacement,cx,cy,zoom,&x,&y);
		cx=x, cy=y;
	}
	
	pos = Position(hpa);
	x = cx + (dotp(pos,zero) * zoom);
	y = cy - (dotp(pos,nine) * zoom);

	pos = Position(hpd);
	ox = cx + (dotp(pos,zero) * zoom);
	oy = cy - (dotp(pos,nine) * zoom);

	SelectObject(hDC,pen);
	if (line) DrawLine(hDC,x,y,ox,oy,w,h,false);
	
	if (box) {
		
		double r=3.0;

		HPEN spen=CreatePen(PS_SOLID,1,color); 
		HBRUSH brush=CreateSolidBrush(color);
		HBRUSH brushb=CreateSolidBrush(0);

		SelectObject(hDC,spen);
		SelectObject(hDC,brush);
		DrawRectangle(hDC,x-r,y-r,x+r,y+r,w,h);

		SelectObject(hDC,brushb);
		DrawRectangle(hDC,ox-r,oy-r,ox+r,oy+r,w,h);

		DeleteObject(spen);  
		DeleteObject(brush);
		DeleteObject(brushb);
	}

	SelectObject(hDC,GetStockObject(NULL_BRUSH));
	SelectObject(hDC,GetStockObject(WHITE_PEN));
	DeleteObject(pen); 
}


// ==============================================================================================================================================
//
void Orbit::GDIDrawRadius(HDC hDC, DWORD color, double w,double h, double size) 
{	  
	double cx=w/2.0, cy=h/2.0;
	double x,y;

	HPEN spen=CreatePen(PS_SOLID,1,color); 
   
	if (length(displacement)>0) {
		Point(displacement,cx,cy,size,&x,&y);
		cx=x, cy=y;
	}
	
	SelectObject(hDC,spen);
	
	Point(Position(trl),cx,cy,size,&x,&y); 
	DrawLine(hDC,x,y,cx,cy,w,h,false);
	 
	SelectObject(hDC,GetStockObject(WHITE_PEN)); 
	DeleteObject(spen);  
}