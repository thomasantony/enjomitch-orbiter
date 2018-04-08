//  ==============================================================================================================================================
//	Copyright (C) 2002 - 2015 Jarmo Nikkanen
//                2016 - 2018 Andrew Stokes
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
#include "BSReference.hpp"
#include "BSOrbit.hpp"

unsigned int green	   = 0x00009900; // Ship orbit
unsigned int dgreen	   = 0x00006600; // Dark green, map lines
unsigned int lgreen	   = 0x0000CC00; // Text color
unsigned int hgreen	   = 0x0000EEAA; // Hilighted text
unsigned int yellow    = 0x0000BBBB; // Target orbit
unsigned int lyellow   = 0x0000DDDD; // Yellow text
unsigned int grey      = 0x00999999; // Light grey lines, intersection
unsigned int dgrey     = 0x00666666; // Dark Grey, planet, actual ship orbit in HTO
unsigned int red       = 0x000000DD; // Warning lights
unsigned int white     = 0x00EEEEEE; // White
unsigned int blue      = 0x00FF0000; // Blue
unsigned int itemcolor = 0x00FF8888; //
unsigned int defEquDir = 0;
unsigned int defDispMode = 0;
unsigned int defEncMode = 0;
unsigned int defNumSol = 0;
float defReAng = 0.0;
float defReAnt = 0.0;
float defReAlt = 0.0;
class Orbit Ecliptic;

// ==============================================================================================================================================
//
double __acosh(double x)
{
	return( log( x + sqrt( x*x - 1.0)) );
}

// ==============================================================================================================================================
//
double __asinh(double x)
{
	return( log( x + sqrt( x*x + 1.0)) );
}

// ==============================================================================================================================================
//
double MIN(double a,double b)
{
	if (a<b) return(a);
	return(b);
}

// ==============================================================================================================================================
//
double MAX(double a,double b)
{
	if (a<b) return(b);
	return(a);
}

// ==============================================================================================================================================
//
void NameFix(char *name)
{
    unsigned int i;
	if (name==NULL) return;
	for (i=0;i<strlen(name);i++) if (name[i]==' ') name[i]='_';
}

// ==============================================================================================================================================
//
void NameUnFix(char *name)
{
    unsigned int i;
	if (name==NULL) return;
	for (i=0;i<strlen(name);i++) if (name[i]=='_') name[i]=' ';
}

// ==============================================================================================================================================
//
bool text2value(char *txt, char*& ptr, double& amt, bool allowdot)
{
	double m=1.0;
	int dotcnt = 0;
	amt = 0.0;
	
	while (*ptr == ' ' || *ptr == '\t') ptr++;
	if (*ptr=='\0') return false;

	char* sptr = ptr;
	if (*ptr == '+' || *ptr == '-') ptr++;

	char* nptr = ptr;
	while ((*ptr >= '0' && *ptr <= '9') || (allowdot && *ptr == '.')) {
		if (*ptr == '.') dotcnt++;
		ptr++;
	}
	if ((nptr == ptr) || (dotcnt > 1)) return false;
	char* eptr = ptr;
	char ech = *eptr;
	*eptr = '\0';
	amt = atof(sptr);
	*eptr = ech;

	while (*ptr == ' ' || *ptr == '\t') ptr++;
	if (*ptr == '\0') return true;

	char c=*ptr++;

	if (c=='d' || c=='D')				m=86400;
	else if (c=='h' || c=='H')	m=3600;
	else if (c=='k' || c=='K')	m=1000;
	else if (c=='M')   					m=1e6;
	else if (c=='g' || c=='G')	m=1e9;
	else if (c=='a' || c=='A')	m=AU;
	else if (c=='b' || c=='B')	m=1e5;
	else if (c=='u' || c=='U')	m=1e-6;
	else if (c=='m')		    	  m=1e-3;
	else return false;

	while (*ptr == ' ' || *ptr == '\t') ptr++;
	if (*ptr != '\0') return false;
	amt = amt * m;
	return true;
}

// ==============================================================================================================================================
//
bool text2doublevalue(char *txt, char*& ptr, double& amt1, double& amt2, bool allowdot)
{
	// parse N28.345 W33.456
	amt1 = 0.0;
	amt2 = 0.0;
	double *curamt = &amt1;
	
	for (int i=0; i<2; i++) {
		if (i==1) curamt = &amt2;

		while (*ptr == ' ' || *ptr == '\t') ptr++;
		if (*ptr=='\0') return false;

		char* sptr = ptr;

		if (*ptr == 'N' && i == 0) *ptr = '+';
		else if (*ptr == 'S' && i == 0) *ptr = '-';
		else if (*ptr == 'E' && i == 1) *ptr = '+';
		else if (*ptr == 'W' && i == 1) *ptr = '-';

		if (*ptr == '+' || *ptr == '-') ptr++;

		char* nptr = ptr;
		int dotcnt = 0;
		while ((*ptr >= '0' && *ptr <= '9') || (allowdot && *ptr == '.')) {
			if (*ptr == '.') dotcnt++;
			ptr++;
		}
		if ((nptr == ptr) || (dotcnt > 1)) return false;
		char* eptr = ptr++;
		if ((*eptr != ' ') && (*eptr != '\t') && (*eptr != '\0')) return false;
		if ((i==0) && (*eptr == '\0')) return false;

		*eptr = '\0';
		*curamt = atof(sptr);
	}
	ptr++;
	return true;
}



// ==============================================================================================================================================
//
void value(double real,char *buf,bool angle,bool au)
{
	 char buffer[512];

     double v=fabs(real);
     int n;
     memset(buffer,0,12);
	
	 char *c={""};		char *k={"k"};
	 char *k2={"M"};	char *k3={"G"};
	 char *k4={"AU"};	char *k5={"m"};
	 char *k6={"µ"};	char *k7={"T"};

     n=(int)floor(log10(v))+1;
     
     if (angle==false) {
		 if (n>15) {
			 sprintf(buffer,"%1.3e\n",real); 
			 strncpy(buf,buffer,10); 
			 return;
		 }
		 else if (n>12 && !au) real/=1e12,c=k7;
		 else if (n>11 && au) real/=149.5979e9,c=k4;
		 else if (n>9) real/=1e9,c=k3; 
		 else if (n>6) real/=1e6,c=k2; 
		 else if (n>3) real/=1e3,c=k; 
		 else if (n<(-4)) real*=1e6,c=k6; 
		 else if (n<(-1)) real*=1e3,c=k5; 
	 }  
	 if (angle) { 
		 sprintf(buffer,"%1.2f",real); 
		 strncpy(buf,buffer,6);
		 strcat(buf,"°");
	 } else {
		 sprintf(buffer,"%1.3f",real); 
		 strncpy(buf,buffer,6);
		 strcat(buf,c);
	 }
}

// ==============================================================================================================================================
//
char *MJDToClock(double mjd, char *buf)
{
	char b[32];
	double sec=fabs((mjd-floor(mjd))*86400);
	double h=floor(sec/3600);
	double m=floor((sec-(h*3600))/60);
	double s=floor(sec - (h*3600) - (m*60) );
  
	sprintf(b,"%0.0f",h); 
	if (h<10) strcpy(buf,"0"), strcat(buf,b), strcat(buf,":");
	else strcpy(buf,b), strcat(buf,":");
	sprintf(b,"%0.0f",m);
	if (m<10) strcat(buf,"0"), strcat(buf,b), strcat(buf,":");
	else strcat(buf,b), strcat(buf,":");
	sprintf(b,"%0.0f",s); 
	if (s<10) strcat(buf,"0"), strcat(buf,b);
	else strcat(buf,b);
	return buf;
}


// ==============================================================================================================================================
//
char *TimeToClock(double sec, char *buf)
{
	char b[32];
	double h=floor(sec/3600);
	double m=floor((sec-(h*3600))/60);
	double s=sec - (h*3600) - (m*60);

	sprintf(b,"%0.0f",h); 
	if (h<10) strcpy(buf,"0"), strcat(buf,b), strcat(buf,":");
	else strcpy(buf,b), strcat(buf,":");
	sprintf(b,"%0.0f",m);
	if (m<10) strcat(buf,"0"), strcat(buf,b), strcat(buf,":");
	else strcat(buf,b), strcat(buf,":");
	sprintf(b,"%0.2f",s); 
	if (s<10) strcat(buf,"0"), strcat(buf,b);
	else strcat(buf,b);
	return buf;
}

// Text output functions
// ==============================================================================================================================================
//
void Text(HDC hdc,int x,int y,char *txt)
{
	TextOut(hdc,x,y,txt,(int)strlen(txt));
}

// ==============================================================================================================================================
//
void Text(HDC hdc,int x,int y,char *txt,char *str)
{
	char buf[64];
	strcpy(buf,txt);
	strcat(buf,str);
	TextOut(hdc,x,y,buf,(int)strlen(buf));
}

// ==============================================================================================================================================
//
void Text(HDC hdc,int x,int y,char *txt,double val,char *un)
{
	char buf[64];
	char buf2[64];
	memset(buf2,0,15);
	strcpy(buf,txt);
	value(val,buf2,0);
	strcat(buf,buf2);
	if (un) strcat(buf,un);
	TextOut(hdc,x,y,buf,(int)strlen(buf));
}

// ==============================================================================================================================================
//
void Multiline(HDC hdc,int x,int y,int width,int height,char *text)
{
	int tot=(int)strlen(text);
	int start=0;
	int end=0;
	
	while (end<tot) {
		end=start+width;
		if (end>tot) end=tot;
		if (end<tot) while (text[end]!=' ' && end>0) end--;
		int wi=end-start;
		TextOut(hdc,x,y,text+start,wi);
		start+=(wi+1);
		y+=height;
	}
}

// ==============================================================================================================================================
//
void TextA(HDC hdc,int x,int y,char *txt,double val,char *un)
{
	char buf[64];
	char buf2[64];
	memset(buf2,0,10);
	strcpy(buf,txt);
	value(val,buf2,1);
	strcat(buf,buf2);
	if (un) strcat(buf,un);
	TextOut(hdc,x,y,buf,(int)strlen(buf));
}


// ==============================================================================================================================================
//
double Time2MJD(double t)
{
	if (t<0) t=0;
	return(oapiGetSimMJD()+(t/86400));
}


// ==============================================================================================================================================
//
double MJD2Time(double m)
{
	double x=(m-oapiGetSimMJD())*86400;
	if (x<0) x=0;
	return x;	
}

// ==============================================================================================================================================
//
bool IsDateFormat(char *date)
{
	int i,q=0,l=(int)strlen(date);
	if (l<8 || l>11) return false;
	for (i=0;i<l;i++) if (date[i]=='.') q++;
	if (q!=2)
	for (i=0,q=0;i<l;i++) if (date[i]=='-') q++;
	if (q==2) return true;
	return false;
}

// ==============================================================================================================================================
//
double DateFormat2MJD(char *date)
{

	double dpm[2][12]={31,28,31,30,31,30,31,31,30,31,30,31,31,29,31,30,31,30,31,31,30,31,30,31};
	
	double day=0,month=0,year=0,days;
	char string[8];
	if (date==NULL) return 0;

	int i,q,l=(int)strlen(date);
	if (l<8 || l>11) return 0;

	if (date[2]=='-' || date[1]=='-') {
		for (i=0; i<l; i++) if (date[i]=='-') date[i]=' ';
		sscanf(date,"%lg %s %lg",&day,&string,&year);
		if (strcmp(_strlwr(string),"jan")==0) month=1;
		if (strcmp(_strlwr(string),"feb")==0) month=2;
		if (strcmp(_strlwr(string),"mar")==0) month=3;
		if (strcmp(_strlwr(string),"apr")==0) month=4;
		if (strcmp(_strlwr(string),"may")==0) month=5;
		if (strcmp(_strlwr(string),"jun")==0) month=6;
		if (strcmp(_strlwr(string),"jul")==0) month=7;
		if (strcmp(_strlwr(string),"aug")==0) month=8;
		if (strcmp(_strlwr(string),"sep")==0) month=9;
		if (strcmp(_strlwr(string),"oct")==0) month=10;
		if (strcmp(_strlwr(string),"nov")==0) month=11;
		if (strcmp(_strlwr(string),"dec")==0) month=12;
	}
	else
	if (date[2]=='.' || date[1]=='.') {
		for (i=0; i<l; i++) if (date[i]=='.') date[i]=' ';
		sscanf(date,"%lg %lg %lg",&day,&month,&year);
	}
	else return 0;

	if ((int(year)&7)==4) q=1;
	else q=0;

	if (month>12 || month<1) month=1;

	year -= 1860;
	month--;
	
	days=0;
	for (i=0;i<int(month);i++) days += dpm[q][i];
	days += day;
	days += 408 + ceil(year * 365.25);

	return days;
}

// ==============================================================================================================================================
//
double difference(double a,double b)
{	
	double n;
	a=limit(a);
	b=limit(b);	
	if (a>b) n=(a-b);
	else n=(b-a);
	if (n>PI) n=PI2-n;
	return(n);
}

// ==============================================================================================================================================
//
double angular_distance(double a,double b,bool pro)
{	
	double n;
    if (a<b) n=b-a;
	else n=PI2-(a-b);
	if (!pro) n=PI2-n;
	return(n);
}

// ==============================================================================================================================================
//
double angle(VECTOR3 v,VECTOR3 h)
{
	double x = dotp(v,h) / (length(h)*length(v));
	if (x>1)  x=1; // Avoid precision problems
	else if (x<-1) x=-1;
	return( acos( x ) );
}



// *******************************************************************
// Return the angle of "p" measured around the normal vector "normal"
// Zero angle is defined by "zero" vector. "zero" and "normal" must be
// perpendicular to each other. Input vector "p" doesn't need to be on 
// the plane. Range of the returned angle is (0, 2PI)
// *******************************************************************

double nangle(VECTOR3 p, VECTOR3 zero, VECTOR3 normal)
{
	VECTOR3 nine=crossp(normal,zero);
	double y=dotp(p,zero)/length(zero);
    double x=dotp(p,nine)/length(nine);
	return ( limit( atan2(x,y) ) );
}



// *********************************************************************
// * Rotate the vector "in" into the plane defined by "normal" by rotating
//   the vector "in" around the rotation axis "rot_axis"
//
// * Rot_axis must lie on the same plane defined by "normal"
//
// * returned vector will maintain it's magnitude and angle with rot_axis
//
// * used with a plane change computations
// *********************************************************************

VECTOR3 FixVector(VECTOR3 in, VECTOR3 normal, VECTOR3 rot_axis)
{
	VECTOR3 prod = unit(crossp(rot_axis, normal));
	    rot_axis = unit(rot_axis);

	// Square length on "in"
	double dotin = dotp(in,in);
	// scalar component paraller to rot axis
	double paral = dotp(in, rot_axis);
	// scalar component perpendicular to rot axis
	double perpe = sqrt(dotin - paral*paral);
	// Wheter the product component is positive or negative
	double sign  = dotp(in, prod);
		   sign  = sign/fabs(sign);
	//Compute fixed vector
	return rot_axis * paral + prod * (perpe*sign);
}



// ****************************************************************************
// Create a new unit length vector. "normal" and "zero" will define 
// the coordinate space and the "angle" will define the direction of the vector
// angle is defined (0, 2PI)
// ****************************************************************************

VECTOR3 create_vector(VECTOR3 normal,VECTOR3 zero,double angle)
{
	zero=unit(zero);
	normal=unit(normal);
	zero=unit(zero-(zero*dotp(zero,normal)));
	VECTOR3 per=unit(crossp(normal,zero));
	VECTOR3 vec=zero*cos(angle) + per*sin(angle);
	return vec;
}




// *******************************************************************
// Tangent angle functions
// "tan" is the angle betveen the tangent vector and semi-major axis
// Checked OK, Elliptic and Hyperbolic orbits
// *******************************************************************

double eca2tan(double eca, double e)
{
	if (e<1.0) {
		double ce=cos(eca);
		double t=acos(ce*sqrt((1.0-e*e)/(1.0-e*e*ce*ce)));
		if (eca>PI) t=PI2-t;	
		t+=PI05;
		if (t>PI2) t-=PI2;
		return t;
	}

	double ce=cosh(eca);
	double t=acos(ce*sqrt((1.0-e*e)/(1.0-e*e*ce*ce)));
	
	if (eca<0) t=PI2-t;	
	t+=PI05;
	if (t>PI2) t-=PI2;
	return t;
}


// *******************************************************************
//
double tra2tan(double tra,double e)
{
	double ce=(cos(tra)+e)/(1.0+e*cos(tra));
	double t=acos(ce*sqrt((1.0-e*e)/(1.0-e*e*ce*ce)));
	
	if (tra>PI) t=PI2-t;
	t+=PI05;
	if (t>PI2) t-=PI2;

	return t;	
}


// *******************************************************************
//
double tan2eca(double t,double e)
{
	t-=PI05;
	if (t<0) t+=PI2;
	double st=sin(t);

	if (e<1.0) {
		double eca=acos(cos(t)/sqrt(1.0-e*e*st*st));
		if (t>PI) eca=PI2-eca;
		return eca;
	}

	double eca=__acosh(cos(t)/sqrt(1.0-e*e*st*st));
	if (t>PI) eca=-eca;
	return eca;
}


// *******************************************************************
//
double tan2tra(double t,double e)
{
	t-=PI05;
	if (t<0) t+=PI2;

	double st=sin(t);
	double cos_ea=cos(t)/sqrt(1.0-e*e*st*st);
	double tra=acos( ( cos_ea - e ) / ( 1.0 - e * cos_ea ) );     
	if (t>PI) tra=PI2-tra;
	return tra;
}





// *******************************************************************
// Flight-path angle and gamma functions
// fpa and gamma are complementary angles
// *******************************************************************

double tra2gamma(double tra,double e)
{ 
	if (e==0) return PI05;

	double co = cos(tra); 
	double x  = 2*co*e+1.0;
	double ta = asin( sqrt(co*co*e*e+x) / sqrt(e*e+x) );
	if (tra>PI) return PI-ta;
	return ta;
}


// *******************************************************************
//
double tra2fpa(double tra,double e)
{ 
	if (e==0) return 0;

	double co = cos(tra); 
	double x  = 2.0*co*e+1.0;
	double ta = acos( sqrt(co*co*e*e+x) / sqrt(e*e+x) );
	if (tra>PI) return -ta;
	return ta;
}



// *******************************************************************
// Tra, Eca, Mna conversion functions
// *******************************************************************

double tra2eca(double tra,double e)
{ 
	double eca; 
	double cos_tra = cos(tra);
    	
	if (e>1.0) {
        eca = __acosh( (e+cos_tra) / (1.0+e*cos_tra) );	
		if (tra>PI) eca=-eca;
		return(eca);
	}

	if (e>0) {
		eca = acos( (e+cos_tra) / (1.0+e*cos_tra) );
	    if (tra>PI) eca = PI2 - eca;
		return(eca);
	}

	return(tra);
}

// *******************************************************************
//
double tra2mna(double tra,double ecc)
{	
    if (ecc==0) return(tra);
	double eca = tra2eca(tra,ecc);	
	return(eca2mna(eca,ecc));
}

// *******************************************************************
//
double eca2mna(double eca,double ecc)
{
	double mna;
	if (ecc==0) return(eca);
	if (ecc<1.0) {
	    mna = eca - ( ecc * sin( eca ) ); 
		return (limit(mna));
	}
    return ( ( ecc * sinh(eca) ) - eca );	
}


// *******************************************************************
// Avoid calling this very often
//
double mna2eca(double mna,double ecc)
{
    int i,c;
	double delta,M,eca,mnamp;
  
	if (ecc==0) return(mna);

	// For Elliptic orbit
    if (ecc<1.0) { 

		mnamp=mna;
		if (mna>PI) mnamp=PI2-mna;

        if (ecc<0.9) { // Solve using newton's method
	
			if (ecc<0.4) c=4;
			else if (ecc<0.7) c=5;
			else c=7;
		
			eca=mnamp;
			for (i=0;i<c;i++) eca = eca + (mnamp + ecc*sin(eca) - eca) / (1.0 - ecc*cos(eca));		
			
			if (mna>PI) eca=PI2-eca;
			return eca;
		}

	   	eca=PI05; delta=eca/2.0;
   
		for (i=0;i<32;i++) {   // Must be 28
			M=eca-ecc*sin(eca);
			if (M>mnamp) eca-=delta;
			else		 eca+=delta;
			delta/=2;
		}

		if (mna>PI) eca=PI2-eca;
		return(eca);
	} 
	
    // For Hyperbolic orbit
	eca=10.0; delta=eca/2.0;
    mnamp=fabs(mna);

	for (i=0;i<32;i++) {	
		M=ecc*sinh(eca)-eca;
		if (M>mnamp) eca-=delta;
		else		 eca+=delta;
		delta/=2;
	}

	if (mna<0) eca=-eca;
	return(eca);
}


// *******************************************************************
//
double eca2tra(double eca,double ecc)
{
	if (ecc==0) return(eca);

	if (ecc<1.0) {
		double cos_ea=cos(eca);
		double tra=acos( ( cos_ea - ecc ) / ( 1.0 - ecc * cos_ea ) );     
		if (eca > PI) tra=PI2-tra;
		return(limit(tra));
	}

    double cos_ea=cosh(eca);
	double tra=acos( ( ecc - cos_ea ) / (  ecc * cos_ea - 1.0 ) ); 
	if (eca < 0) tra=PI2-tra;
	return(tra);
}


// *******************************************************************
//
double mna2tra(double mna,double ecc)
{
	double eca=mna2eca(mna,ecc);
	double tra=eca2tra(eca,ecc);
    return(tra);
}



// Input: Obliquity of ecliptic, longitude of Sun´s transit		=angle(ecliptic normal , rot-axis)
// Warning: "Obliquity of orbit" is not the same ! ! !			=angle(planet orbit normal , rot-axis)

// ************************************************************************************************
// ATTENTION ! !    Rotation axis is pointing SOUTH pole
// When orbiting on pro-grade orbit the orbit normal (angularmomentum) is also pointing south pole
// ************************************************************************************************

VECTOR3 RotationAxis(double obliquity, double suntransit)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	VECTOR3 norv=_V(0,-1,0);
	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
    VECTOR3 per = crossp(norv,lan);
	VECTOR3 rot = norv * cos(obliquity) - per * ( sin(obliquity) / sqrt(dotp(per,per)) );
	return rot;
}


// ************************************************************************************************
//
void PlanetAxis(double obliquity, double suntransit, double offset, double period, double since_epoch,VECTOR3 *Rot,VECTOR3 *Off)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	VECTOR3 norv=_V(0,-1,0);
	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
    VECTOR3 per = crossp(norv,lan);
	VECTOR3 rot = norv * cos(obliquity) - per * ( sin(obliquity) / sqrt(dotp(per,per)) );

	if (Rot) *Rot=rot;

    double posit = limit((since_epoch*86400.0/period)*PI2 + offset); 

	        per = crossp(rot,lan);
	VECTOR3 off = lan * cos(posit) + per * (sin(posit) / sqrt(dotp(per,per)) );
 
    if (Off) *Off=off;
}


// ************************************************************************************************
// Warning Offset is not a rotation offset
//
void PlanetAxis(double obliquity, double suntransit, VECTOR3 *Rot, VECTOR3 *Off)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	VECTOR3 norv=_V(0,-1,0);
	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
    VECTOR3 per = crossp(norv,lan);
	VECTOR3 rot = norv * cos(obliquity) - per * ( sin(obliquity) / sqrt(dotp(per,per)) );

	if (Rot) *Rot=rot;
    if (Off) *Off=lan;
}


// ************************************************************************************************
//
void PlanetAxis(OBJHANDLE ref, double time, VECTOR3 *Rot, VECTOR3 *Off)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	VECTOR3 norv=_V(0,-1,0);

	double suntransit = oapiGetPlanetTheta(ref);
	double obliquity  = oapiGetPlanetObliquity(ref);
	double offset     = oapiGetPlanetCurrentRotation(ref);
	double period     = oapiGetPlanetPeriod(ref);

	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
    VECTOR3 per = crossp(norv,lan);
	VECTOR3 rot = norv * cos(obliquity) - per * ( sin(obliquity) / sqrt(dotp(per,per)) );

	if (Rot) *Rot=rot;

	if (Off) {
		double posit = limit((time*86400.0/period)*PI2 + offset); 
				 per = crossp(rot,lan);
		VECTOR3  off = lan * cos(posit) + per * (sin(posit) / sqrt(dotp(per,per)) );

		 *Off=off;
	}
}


// ************************************************************************************************
//
VECTOR3 RotationAxis(OBJHANDLE ref)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	VECTOR3 norv=_V(0,-1,0);

	double suntransit = oapiGetPlanetTheta(ref);
	double obliquity  = oapiGetPlanetObliquity(ref);
	
	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
    VECTOR3 per = crossp(norv,lan);
	VECTOR3 rot = norv * cos(obliquity) - per * ( sin(obliquity) / sqrt(dotp(per,per)) );

	return rot;
}


// ************************************************************************************************
//
VECTOR3 SunTransitAxis(OBJHANDLE ref)
{
	VECTOR3 majv=_V(1,0,0);
	VECTOR3 minv=_V(0,0,1);
	double suntransit = oapiGetPlanetTheta(ref);
	VECTOR3 lan = majv * cos(suntransit) + minv * sin(suntransit);
	return lan;
}


// ************************************************************************************************
//
void LonLat(VECTOR3 in,VECTOR3 rot,VECTOR3 off,double *Lon,double *Lat)
{	
	double l=nangle(in,off,rot);
	if (l>PI) l=l-PI2;
    double h=dotp(in,rot)/length(rot);
	double t=asin(h/length(in));
	if (Lon) *Lon=l;
	if (Lat) *Lat=-t;	
}


// ************************************************************************************************
// Rot-Axis must point into SOUTH pole
// Positive latutudes are on the north side and negative latitudes on the south side

VECTOR3 VectorByLonLat(VECTOR3 rot, VECTOR3 off, double Lon, double Lat)
{
	return unit( unit(rot)*sin(-Lat) + 
		   unit(off)*(cos(Lon)*cos(Lat)) +
		   unit(crossp(rot,off))*(sin(Lon)*cos(Lat)) );
}



// ************************************************************************************************
// Ship relative Air Speed = Rel.Velocity - AirSpeed
// Rotation velocity of atmosphere

VECTOR3 AirSpeed(VECTOR3 pos, VECTOR3 rot_axis, double rotper)
{
	double p = dotp(pos, rot_axis) / length(rot_axis);
	double l = length(pos);
	double r = sqrt(l*l - p*p);
	double v = PI2 * r / rotper;
	return ( unit( crossp(rot_axis, pos) ) * v );
}


// ************************************************************************************************
// 
VECTOR3 GroundSpeedVector(OBJHANDLE ref, double time, double lon, double lat)
{
	VECTOR3 rot,off;
	
	if (time==0) time=oapiGetSimMJD();
	PlanetAxis(ref,time,&rot,&off);

	VECTOR3 gpv = VectorByLonLat(rot,off,lon,lat);   
    double v = PI2 * cos(lat) * oapiGetSize(ref) / oapiGetPlanetPeriod(ref);
	return ( unit( crossp(rot, gpv) ) * v );
}	

// ************************************************************************************************
// 
VECTOR3 GroundSpeedVector(OBJHANDLE ref, VESSEL *shp)
{
	VECTOR3 rot,off;	
	PlanetAxis(ref,0,&rot,&off);

	VECTOR3 gpv,vel;
	shp->GetRelativePos(ref,gpv);
	shp->GetRelativeVel(ref,vel);

	vel=vel*(oapiGetSize(ref)/length(gpv));

	double lat = angle(gpv,rot)-PI05;
    double v = PI2 * cos(lat) * oapiGetSize(ref) / oapiGetPlanetPeriod(ref);
	
	VECTOR3 GSp = unit( crossp(rot, gpv) ) * v;
	
	return (GSp-vel);
}

// ************************************************************************************************
// 
double BurnTimeBydV(double dv, VESSEL *ship)
{
	double isp=ship->GetISP();
	double mas=ship->GetMass();
	double th=ship->GetMaxThrust(ENGINE_MAIN);
	double time = ((1.0-exp(-dv/isp))*mas*isp) / th;
	return(time);
}

// ************************************************************************************************
// 
double dVByBurnTime(double time, VESSEL *ship,THGROUP_HANDLE engine)
{
	double isp=0;
	double mas=ship->GetMass();
	double th=0;
	int i,c=ship->GetGroupThrusterCount(engine);

	for (i=0;i<c;i++) {
		THRUSTER_HANDLE hand=ship->GetGroupThruster(engine,i);
		th+=ship->GetThrusterMax(hand);
		isp+=ship->GetThrusterIsp0(hand);
	}

	isp/=(double)c;

	double dv = -isp * log(-(time*th-mas*isp)/(mas*isp));

	return(dv);
}

// ************************************************************************************************
// 
double FixTimeToBurn(Orbit *src,double ttb, double eject_trl)
{
	double trl = src->TrlByTime(ttb);
	double a = src->TimeBetween(trl, eject_trl);
	double b = src->Period() - a;
	if (b<a && (ttb-b)>0) return ttb-b;
	return ttb+a;
}
		

// ************************************************************************************************
// 
double SystemMass(OBJHANDLE obj)
{
	int i;
	if (obj==NULL) return 0;
	if (obj==Refer->StarHandle) return oapiGetMass(obj);

	double sys_mass=oapiGetMass(obj);
	int c=Refer->GetSystemCount(obj);

	if (c>0) {		
		OBJHANDLE *list=Refer->GetSystemList(obj);     
		for (i=0;i<c;i++) sys_mass+=oapiGetMass(list[i]);	
	}
	return sys_mass;
}

// ************************************************************************************************
//
double EccOfOrbit(VECTOR3 r, VECTOR3 v, double myy)
{	
	double vel2 = dotp(v,v);
	double rad  = length(r);
	VECTOR3 majv = (r * (vel2-myy/rad)) - (v * dotp(r,v));
	return length(majv)/myy;
}

// ************************************************************************************************
//
void DrawEllipse(HDC hDC,double x,double y,double xx,double yy,double w,double h)
{
	if (xx<0 || x>w || y>h || yy<0) return;
	if (x<-3000 || xx>3000 || yy>3000 || y<-3000) return;
	Ellipse(hDC,(int)x,(int)y,(int)xx,(int)yy);
}

// ************************************************************************************************
//
void DrawRectangle(HDC hDC,double x,double y,double xx,double yy,double w,double h)
{
	if (xx<0 || x>w || y>h || yy<0) return;
	Rectangle(hDC,(int)x,(int)y,(int)xx,(int)yy);
}

// ************************************************************************************************
//
bool DrawLine(HDC hDC,double x,double y,double xx,double yy,double w,double h,bool ex)
{
	double x0,y0,x1,y1;
	bool a=true,b=true;

	double dx=xx-x;
	double dy=yy-y;
	double left=1;
	double right=w-2;
	double top=1;
	double bottom=h-2;

    // Extend endpoint
	if (ex) xx=xx+dx*400, yy=yy+dy*400;
	if (x<left  || x>right  || y<top  || y>bottom)  a=false;
	if (xx<left || xx>right || yy<top || yy>bottom) b=false;

	if (a && b) {   // Entirely inside
		MoveToEx(hDC,(int)x, (int)y,NULL);
		LineTo(hDC,  (int)xx,(int)yy);
		return true;
	}
	if (!a && !b) {  // Totaly Outside
		if (x<left   && xx<left)   return false;
		if (x>right  && xx>right)  return false;
		if (y<top    && yy<top)    return false;
		if (y>bottom && yy>bottom) return false;
	}

	// Boundry line cross sections
	double xa=(top-y)   * dx/dy + x;
	double xw=(bottom-y)* dx/dy + x;
	double ya=(left-x)  * dy/dx + y;
	double yh=(right-x) * dy/dx + y;

  // First point
	if      (xa>=left && xa<=right)  x0=xa,   y0=top; 
	else if (xw>=left && xw<=right)  x0=xw,   y0=bottom;
	else if (ya>=top  && ya<=bottom) x0=left, y0=ya; 
	else if (yh>=top  && yh<=bottom) x0=right,y0=yh; 
	else return false; // Line not visible

	// Second point
	if      (yh>=top  && yh<=bottom) x1=right,y1=yh; 	
	else if (ya>=top  && ya<=bottom) x1=left, y1=ya; 
	else if (xw>=left && xw<=right)  x1=xw,   y1=bottom;
	else if (xa>=left && xa<=right)  x1=xa,   y1=top;
	else return false;
    
	if (!a && !b) x=x0,y=y0,xx=x1,yy=y1;	  
	else if (!a) {  // Select closest point for starting
		if (((x-x0)*(x-x0) + (y-y0)*(y-y0)) < ((x-x1)*(x-x1) + (y-y1)*(y-y1))) x=x0,y=y0;
		else x=x1,y=y1;
	}
	else if (!b) {  // Select closest point for end
		if (((xx-x0)*(xx-x0) + (yy-y0)*(yy-y0)) < ((xx-x1)*(xx-x1) + (yy-y1)*(yy-y1))) xx=x0,yy=y0;
		else xx=x1,yy=y1;
	}	
   
	MoveToEx(hDC,(int)x, (int)y,NULL);
	LineTo(hDC,  (int)xx,(int)yy);
	return true;
}

// ************************************************************************************************
//
bool DrawLine(HDC hDC, VECTOR3 a, VECTOR3 b, double w, double h)
{
	return DrawLine(hDC, a.x, a.y, b.x, b.y, w, h, false);
}

// ************************************************************************************************
//
double CalculateSOI(OBJHANDLE obj, OBJHANDLE ref)  
{
	if (obj==NULL || ref==NULL) return 0;
	if (obj==ref) return 100*AU;

	double myyref = oapiGetMass(ref) * GC;
	double myy    = oapiGetMass(obj) * GC;

	VECTOR3 pos;
	oapiGetRelativePos(obj,ref,&pos);
	double rad = ( length(pos) * sqrt(myy) ) / ( sqrt(myy) + sqrt(myyref) );
	return(rad);
}


// ************************************************************************************************
//
void DrawDirectionSelector(HDC hDC,VESSEL *ship,VECTOR3 vel,int width,int height)
{
	HPEN pen1=CreatePen(PS_SOLID,1,dgreen); 
	HPEN pen2=CreatePen(PS_SOLID,1,green); 
	HPEN pen3=CreatePen(PS_SOLID,1,grey); 

	VECTOR3 thrustdir=_V(0,0,1);
	VECTOR3 accel_loc=_V(0,0,0)-thrustdir;
	VECTOR3 upwards_loc=_V(0,-1,0);
	VECTOR3 side_loc=_V(1,0,0);
  
	VECTOR3 accel,upwards,side;
	ship->GlobalRot(accel_loc,accel);
	ship->GlobalRot(upwards_loc,upwards);
	ship->GlobalRot(side_loc,side);

	double line=dotp(vel,accel)/length(accel);
	double vert=dotp(vel,upwards)/(length(upwards)*length(vel));
	double horiz=dotp(vel,side)/(length(side)*length(vel));

	SelectObject(hDC,pen1);

	MoveToEx(hDC,20,height/2, NULL);
	LineTo(hDC,width/2 - 20, height/2);
	MoveToEx(hDC,width/2 + 20 ,height/2, NULL);
	LineTo(hDC,width - 20, height/2);
	MoveToEx(hDC,width/2, 20, NULL);
	LineTo(hDC,width/2, height/2 - 20);
	MoveToEx(hDC,width/2, height/2 + 20, NULL);
	LineTo(hDC,width/2, height - 20);

	Ellipse(hDC,width/2 - 90,height/2 - 90,width/2 + 90,height/2 + 90);
	Ellipse(hDC,width/2 - 20,height/2 - 20,width/2 + 20,height/2 + 20);

	int x=(int)(horiz * width/2);
	int y=(int)(vert * height/2);

	SelectObject(hDC,pen2);

	MoveToEx(hDC,width/2, height/2, NULL);
	LineTo(hDC,width/2 + x , height/2 + y);

	if (line<0) {
		if (abs(x)<4 && abs(y)<4) SelectObject(hDC,pen3);
		MoveToEx(hDC,width/2 + x - 16, height/2 + y, NULL);
		LineTo(hDC,width/2 + x + 16, height/2 + y);
		MoveToEx(hDC,width/2 + x , height/2 + y - 16, NULL);
		LineTo(hDC,width/2 + x , height/2 + y + 16);
	}

	SelectObject(hDC,GetStockObject(WHITE_PEN));
	DeleteObject(pen1); 
	DeleteObject(pen2);  
	DeleteObject(pen3);
}


// ************************************************************************************************
//
double GetHeading(VESSEL *ship, OBJHANDLE Ref, VECTOR3 v)
{
	VECTOR3 Rot,Pos,North;
	Rot = RotationAxis(Ref);
	ship->GetRelativePos(Ref,Pos);
	North=crossp(crossp(Rot,Pos),Pos);
	return nangle(v,North,Pos);
}


// ************************************************************************************************
//
double GetHeading(OBJHANDLE Ref, VECTOR3 Pos, VECTOR3 v)
{
	VECTOR3 Rot,North;
	Rot = RotationAxis(Ref);
	North=crossp(crossp(Rot,Pos),Pos);
	return nangle(v,North,Pos);
}


// ************************************************************************************************
//
VECTOR3 GetThrusterGroupDir(VESSEL *ship, THGROUP_HANDLE handle)
{
	VECTOR3 dir=_V(0,0,0);
	int i, c = ship->GetGroupThrusterCount(handle);
	for (i=0;i<c;i++) {
		THRUSTER_HANDLE th=ship->GetGroupThruster(handle,i);
		VECTOR3 d;
		ship->GetThrusterDir(th,d);
		dir+=d;
	}
	return dir;
}

// ************************************************************************************************
//
void DrawLatitude(HDC hDC,OBJHANDLE handle,Orbit *prj,VECTOR3 rot,VECTOR3 off,double Lat,double zoom,double w,double h)
{

	rot=unit(rot);
	off=unit(off);
	
	VECTOR3 per = unit(crossp(rot,off));
	VECTOR3 pos;
	double  rad = oapiGetSize(handle);
	double  stp = 10*RAD;
	double  xc=w/2;
	double  yc=h/2;
	double  Lon=0;
	double  sin_lat=sin(-Lat);
	double  cos_lat=cos(Lat);
	double  x,y,ox,oy,dot,old_dot;

	pos=unit( rot*sin_lat + off*(cos(Lon)*cos_lat) + per*(sin(Lon)*cos_lat) )*rad;
	prj->Point(pos,xc,yc,zoom,&ox,&oy);
	old_dot = dotp(pos,prj->norv);

	for (Lon=stp;Lon<6.29;Lon+=stp) {

		pos=unit( rot*sin_lat + off*(cos(Lon)*cos_lat) + per*(sin(Lon)*cos_lat) )*rad;
		prj->Point(pos,xc,yc,zoom,&x,&y);
		dot = dotp(pos,prj->norv);
		if (dot<0 && old_dot<0) {
			DrawLine(hDC,x,y,ox,oy,w,h,false);
		}
		old_dot=dot;
		ox=x, oy=y;
	}	
}

// ************************************************************************************************
//
void DrawLongitude(HDC hDC,OBJHANDLE handle,Orbit *prj,VECTOR3 rot,VECTOR3 off,double Lon,double zoom,double w,double h)
{
	rot=unit(rot);
	off=unit(off);
	
	VECTOR3 per = unit(crossp(rot,off));
	VECTOR3 pos;
	double  rad = oapiGetSize(handle);
	double  stp = 5*RAD;
	double  xc=w/2;
	double  yc=h/2;
	double  Lat=0;
	double  sin_lon=sin(Lon);
	double  cos_lon=cos(Lon);
	double  x,y,ox,oy,dot,old_dot;
   
	pos=unit( rot*sin(-Lat) + off*(cos_lon*cos(Lat)) + per*(sin_lon*cos(Lat)) )*rad;
	prj->Point(pos,xc,yc,zoom,&ox,&oy);
	old_dot = dotp(pos,prj->norv);
    
	for (Lat=stp;Lat<6.29;Lat+=stp) {

		pos=unit( rot*sin(-Lat) + off*(cos_lon*cos(Lat)) + per*(sin_lon*cos(Lat)) )*rad;

		prj->Point(pos,xc,yc,zoom,&x,&y);
		dot = dotp(pos,prj->norv);
		if (dot<0 && old_dot<0) {
			DrawLine(hDC,x,y,ox,oy,w,h,false);
		}
		old_dot=dot;
		ox=x, oy=y;
	}	
}
