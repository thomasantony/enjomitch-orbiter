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


#ifndef __ORBIT_TOOLKIT_H
#define __ORBIT_TOOLKIT_H

#include <OrbiterSDK.h>
#include <float.h>

extern unsigned int green;
extern unsigned int dgreen;	 
extern unsigned int lgreen;	 
extern unsigned int hgreen;	 
extern unsigned int yellow;  
extern unsigned int lyellow; 
extern unsigned int grey;    
extern unsigned int dgrey;  
extern unsigned int red;  
extern unsigned int white; 
extern unsigned int blue;
extern unsigned int itemcolor;

extern unsigned int defEquDir;
extern unsigned int defDispMode;
extern unsigned int defEncMode;
extern unsigned int defNumSol;
extern float defReAng;
extern float defReAnt;
extern float defReAlt;

extern double _NAN;


#define PI2		(PI*2.0)
#define PI15    (PI*1.5)
#define GMsun	1.32712440018e20
#define AU		1.49597870691e11
#define GC      6.67259e-11
#define EPOCH   51544.5
#define MIN_ECC 0.000001
#define SUM(v)  v.x+v.y+v.z
#define INVALIDV(v) if (_isnan(SUM(v)) || !_finite(SUM(v)))
#define INVALIDD(d) if (_isnan(d) || !_finite(d))
#define IS_VALIDV(v) (!_isnan(SUM(v)) || _finite(SUM(v)))
#define IS_VALIDD(d) (!_isnan(d) || _finite(d))
#define ZERO(v) v.x==0 && v.y==0 && v.z==0

#define DARK(x) ( (((x&0xFF)/3)&0xFF) + (((x&0xFF00)/3)&0xFF00) + (((x&0xFF0000)/3)&0xFF0000) )
#define DIMM(x) ( (((x&0xFF)/2)&0xFF) + (((x&0xFF00)/2)&0xFF00) + (((x&0xFF0000)/2)&0xFF0000) )
#define LIGHT(x) ( (((x&0xFF)<<1)&0xFF) + (((x&0xFF00)<<1)&0xFF00) + (((x&0xFF0000)<<1)&0xFF0000) )


//  ==============================================================================================================================================
// Hyperbolic orbit:
// ----------------
// Eccentric anomaly is negative before Periapsis passage. Maximum range used in this software
// is [-10, 10] for eccentric anomaly
// Mean anomaly is also negative before Periapsis passage. Maximum range is [-inf, inf]
// True anomaly and true longitude are defined [0, 2PI]
// Semi-major axis is negative.
// Parameter par is always positive


// Elliptic orbit:
// --------------
// Eccentric/Mean/True anomaly, Mean/True longiture are defined [0, 2PI]
// Passing values to this software those are out of range may cause an error
//  ==============================================================================================================================================


double MIN(double a,double b);
double MAX(double a,double b);
double asinh(double);
double acosh(double);

extern class Orbit Ecliptic;

struct Ship_s {
	OBJHANDLE ref;
	VECTOR3 pos, vel;
	double  mass;
	double  thr;
	double  isp;
};

inline double limit(double x)
{
	if (x>PI2) return fmod(x,PI2);
    if (x<0)   return PI2-fmod(-x,PI2);
	return x;
}


inline VECTOR3 set_length(VECTOR3 v, double l)
{
	double x=l/sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	v.x*=x; v.y*=x; v.z*=x;
	return(v);
}





// Text Functions

void    NameFix(char *);
void    NameUnFix(char *);

bool	text2doublevalue(char *txt, char*& ptr, double& amt1, double& amt2, bool allowdot);
bool	text2value(char *txt, char*& ptr, double& amt, bool allowdot);
void	value(double real,char *buf,bool angle=false,bool au=true);
void	Text(HDC hdc,int x,int y,char *txt);
void	Text(HDC hdc,int x,int y,char *txt,char *str);
void	Text(HDC hdc,int x,int y,char *txt,double val,char *un=NULL);
void	TextA(HDC hdc,int x,int y,char *txt,double angle,char *un=NULL);
void    Multiline(HDC,int,int,int,int,char *);
char    *MJDToClock(double mjd, char *buf);
char    *TimeToClock(double mjd, char *buf);

double	Time2MJD(double t);
double	MJD2Time(double m);

bool	IsDateFormat(char *date);
double  DateFormat2MJD(char *date);  // Convert date like "02-Feb-2005" to MJD 

// Conversion Functions

// base for elliptic and hyperbolic orbit
double	mna2eca(double mna,double ecc);
double	eca2mna(double eca,double ecc);
double	tra2eca(double tra,double ecc);
double	eca2tra(double eca,double ecc);

// Conversion between tangential angle and true anomaly
double  tra2gamma(double tra,double ecc);
double  tra2fpa(double tra,double ecc);

// Tangent Angle Functions
double  tra2tan(double tra,double ecc);
double  tan2tra(double tan,double ecc);
double  eca2tan(double eca,double ecc);
double  tan2eca(double tan,double ecc);

// advanced for elliptic and hyperbolic orbit
double	tra2mna(double tra,double ecc);
double	mna2tra(double mna,double ecc);


// General functions
// For elliptic and hyperbolic orbit
double	difference(double a,double b);
double	angular_distance(double a,double b,bool pro=true);
double	angle(VECTOR3 v,VECTOR3 h);
double  nangle(VECTOR3 p,VECTOR3 zero,VECTOR3 normal);
VECTOR3 create_vector(VECTOR3 normal,VECTOR3 zero,double angle);
VECTOR3 FixVector(VECTOR3 in, VECTOR3 normal, VECTOR3 rot_axis);
double  EccOfOrbit(VECTOR3 r, VECTOR3 v,double myy);
double  BurnTimeBydV(double dv,VESSEL *ship);
double  dVByBurnTime(double time, VESSEL *ship,THGROUP_HANDLE engine);
double  FixTimeToBurn(Orbit *src,double ttb, double eject_trl);
VECTOR3 NonSphericalGravityPerturbations(VECTOR3 pos, VECTOR3 rot, double dist, double mu, double rad, double J2);
VECTOR3 RotationAxis(double,double);
VECTOR3 RotationAxis(OBJHANDLE ref);
VECTOR3 SunTransitAxis(OBJHANDLE ref);
VECTOR3 AirSpeed(VECTOR3,VECTOR3,double);
VECTOR3 GroundSpeedVector(OBJHANDLE ref,double time,double lon, double lat);
VECTOR3 GroundSpeedVector(OBJHANDLE ref,VESSEL *shp);
void    PlanetAxis(double obliquity, double suntransit, double offset, double period, double since_epoch, VECTOR3 *Rot, VECTOR3 *Off);
void    PlanetAxis(double obliquity, double suntransit,VECTOR3 *Rot,VECTOR3 *Off);
void    PlanetAxis(OBJHANDLE ref, double time, VECTOR3 *Rot, VECTOR3 *Off);
void    LonLat(VECTOR3 in,VECTOR3 rot,VECTOR3 off,double *Lon,double *Lat);
VECTOR3 VectorByLonLat(VECTOR3 rot,VECTOR3 off,double Lon,double Lat);
double  CalculateSOI(OBJHANDLE obj, OBJHANDLE ref);


// Drawing functions
void	DrawEllipse(HDC hDC,double x,double y,double xx,double yy,double w,double h);
void	DrawRectangle(HDC hDC,double x,double y,double xx,double yy,double w,double h);
bool	DrawLine(HDC hDC,double x,double y,double xx,double yy,double w,double h,bool ex);
bool	DrawLine(HDC hDC,VECTOR3 vert, VECTOR3 vert2);


// For elliptic and hyperbolic orbit
double  GetHeading(VESSEL *ship, OBJHANDLE Ref, VECTOR3 direction);
double  GetHeading(OBJHANDLE Ref, VECTOR3 ship_pos, VECTOR3 direction);
double  EscapeBurn(VESSEL *ship, OBJHANDLE ref, VECTOR3 Esc, double sa);
void	DrawDirectionSelector(HDC hDC,VESSEL *ship,VECTOR3 vel,int width,int height);
void	DrawLatitude(HDC hDC,OBJHANDLE handle,class Orbit *prj,VECTOR3 rot,VECTOR3 off,double Lat,double zoom,double w,double h);
void	DrawLongitude(HDC hDC,OBJHANDLE handle,class Orbit *prj,VECTOR3 rot,VECTOR3 off,double Lon,double zoom,double w,double h);

VECTOR3 GetThrusterGroupDir(VESSEL *ship, THGROUP_HANDLE handle);

#endif
