//  ==============================================================================================================================================
//	Copyright (C) 2002 - 2015 Jarmo Nikkanen
//                2016 - 2018  Andrew Stokes
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


#ifndef __ORBIT_ORBIT_H
#define __ORBIT_ORBIT_H

#include "BSTools.hpp"
#include "OrbiterSDK.h"

// ==============================================================================================================================================
// Hyperbolic orbit:
// -----------------
// Eccentric anomaly is negative before Periapsis passage. Maximum range used in this software
// is [-10, 10] for eccentric anomaly
// Mean anomaly is also negative before Periapsis passage. Maximum range is [-inf, inf]
// True anomaly and true longitude are defined [0, 2*PI]
// Semi-major axis is negative.
//
// Elliptic orbit:
// ---------------
// Eccentric/Mean/True anomaly, Mean/True longiture are defined [0, 2*PI]
// Passing values to this software those are out of range may cause an error
// ==============================================================================================================================================

#define ORBIT_ECLIPTIC


class Orbit {

public:

	Orbit();
	Orbit(VESSEL *vessel);
	Orbit(OBJHANDLE body, OBJHANDLE ref);		
	Orbit(char *body, char *ref);				
	Orbit(VECTOR3 pos, VECTOR3 vel, double myy);	

	~Orbit();

	// Describe new orbit
	void	Elements(VECTOR3 r, VECTOR3 v,double myy,bool reset_proj=true);
	void	Elements(char *body,char *ref);	
	void	Elements(OBJHANDLE body,OBJHANDLE ref);	

	void	GEO(OBJHANDLE ref);	 // Geostationary orbit
	void	LEO(OBJHANDLE ref);  // Local equatorial orbit for reference body
	void	Ecliptic();          // Ecliptic orbit around the sun

	// Create a special orbit with known initial position and position of Periapsis
	bool	ApproachOrbit(OBJHANDLE ref, VECTOR3 pos, VECTOR3 Periapsis);
	
	// Define an orbit with these parameters
	bool	DefineOrbit(OBJHANDLE ref, VECTOR3 pos, VECTOR3 normal, double tra, double ecc);

	// Compute escape orbit
	void	EscapeOrbit(OBJHANDLE ref,VECTOR3 Pos,VECTOR3 Esc,VECTOR3 normal);

	// Create new circular orbit at orbital plane of orbit "plane" with radius of "rad"
	void	CreateNewCircularOrbit(Orbit *plane,double rad);

  // Make the orbit two dimensional with ecliptic, this is used with co-planar transfers. 
	// Longitude of AN and relative inclination must be calculated before calling this function
  void  Make2DimensionalWithEcliptic();

	void	Reset();						
	void	SetProjection(class Orbit *);  // One orbit is used as projection plane for drawing purposes.
	void	SetProjection(VECTOR3 zero, VECTOR3 nine);
	void	CreateProjectionPlane(VECTOR3 normal,VECTOR3 zero);
	void	CreateProjectionOrbit(VECTOR3 norv);
	void	SetTime(double);			
	double	GetTime();					
	
	void    SetOrbitPosition(double trl);

	double	TimeToPeriapsis();			
	double  PeriapsisVelocity();		
	double  PeriapsisDistance();			
	double  AopapsisDistance();			
	
	double	Radius(double trl);			
	double	Velocity(double trl);		
	double  VelocityByRadius(double rad);   

	double	TimeToNode(Orbit *,double *trl_of_node=NULL);				
	double  TrlOfNode(Orbit *);	
	double  TrlOfNode(VECTOR3 normal);	
	
	double  TrlOfIntersection(Orbit *,bool x=false); 		
	double  TimeToIntersection(Orbit *,bool x=false); 	
	  
	double  TrlByRadius(double);
	double	TrlByTime(double);	
	double	TimeTo(double);	
	double  TimeToPoint(double longitude);
	double	SemiMinor();		
	double	MeanMotion();		
	double	Period();	// Orbit period
	double  Energy();	// Orbital Energy
	double  TimeBetween(double trla, double trlb);

  // Receive some vector information
	VECTOR3 Velocity();	
	VECTOR3 Position();	
	VECTOR3 Position(double trl);				
	VECTOR3 Tangent(double trl);
	VECTOR3 Asymptote();
	VECTOR3 EscapeAsymptote();
	VECTOR3 Projection(VECTOR3 pr);
	VECTOR3 PositionByEcA(double eca);
	
	void	Longitude(VECTOR3 pr,double *rad,double *height,double *angle);		
	double	Longitude(VECTOR3 pr);												
	double	TangentialAngle(double longitude);
	double  FlightPathAngle(double longitude);
	
  //Draw
	void    Point(double xp,double yp,double l,double *x,double *y,double s);
	void    Point(VECTOR3 pr,double xp,double yp,double s,double *x,double *y);
	void    Vertex(VECTOR3 pr,double xp,double yp,double s,VECTOR3 *vert);

	double  DrawPlC(HDC hDC,VESSEL *ship,int pos,double apc,double reli);
	void    DrawPlaneIntersection(HDC hDC,double lan,double cx,double cy,double zoom,int color,bool box=true);
	void    DrawPlaneIntersection(HDC hDC,Orbit *t,double cx,double cy,double zoom,int color,bool box=true);
	
	// Primary GDI drawing functions
	void    GDIDraw(HDC hDC,DWORD color,double w,double h,double size,bool pe,bool ra);
	void    GDIDrawSector(HDC hDC, DWORD color, double w,double h, double size,double start,double end,bool pe,bool ra); 
	void    GDIDrawLineOfNodes(HDC hDC, double w,double h, double hpa, DWORD color, double cx, double cy, double zoom, bool box, bool ext);
	void    GDIDrawRadius(HDC hDC, DWORD color, double w,double h, double size);


	void	Displacement(VECTOR3);

	// Special function used with hyperbolas only.
	void    SetTimeToPeriapsis(double);
	double	Translate(Orbit *,double longitude);
	void    SetNULL();
	bool    Defined();
	bool    IsValid(double trl);

	// READ ONLY

	double	sma;		// Semi-major axis
	double	ecc;		// Eccentricity
	double	lan;		// Longitude of ascenting node
	double	agp;		// Argument of Periapsis
	double	inc;		// Inclination
	double	tra;		// True anomaly
	double	trl;		// True longitude;
	double	lpe;		// Longitude of Periapsis
	double	rad;		// Radius
	double  vel;		// Velocity
	double	myy;		// Reduced mass = G * "mass of central body"
	double	mna;		// Mean anomaly
	double	par;		// Parameter = a * fabs(1-e*e)
	double  ang;        // Angular momentum
	double	mnm;		// MeanMotion

	VECTOR3 norv;		// Unit length ! Normal Vector
	VECTOR3 majv;		// Unit length ! Major axis, Pointing Periapsis 
	VECTOR3 minv;		// Unit length ! Minor axis, (norv X majv)
	VECTOR3 zero;		// Projection vectors	
	VECTOR3 nine;
	VECTOR3 Xmajv;		
	VECTOR3 Xminv;	
	VECTOR3 Xcv;

private:

	double  MaxTra();	
	void	CreateProjection();														
	double	IntersectionInterpolate(Orbit *,double start,double end,bool dir);	
	void	IntersectionAll(Orbit *,double *lpa,double *lpd);					
	void    GDIDrawEllipseSector(HDC hDC, DWORD color, double cx, double cy, double w, double h, double size, double start, double end);
	void    GDIDrawHyperbolaSector(HDC hDC, DWORD color, double cx, double cy, double w, double h, double size, double start, double end);

	double	time;					// MJD of current orbit position
	bool    EclipticPlane;
	OBJHANDLE LeoRef, GeoRef;
	VECTOR3 displacement;			// Zero, if not in use. Clear in reset
	VECTOR3 vv;						// Current velocity vector
	VECTOR3 rv;						// Current position vector
	class	Orbit *intersection;	// Clear in reset
	class	Orbit *node_trl_o;		// Clear in reset
	double	first, second, node_trl, timeto;
};

#endif