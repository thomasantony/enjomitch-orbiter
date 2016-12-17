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

#include "Orbitersdk.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "BSTools.hpp"
#include "BaseSyncMFD.hpp"
#include "BSReference.hpp"
#include "BSOrbit.hpp"


// ==============================================================================================================================================
//
SyncMFD::SyncMFD(GeoSyncMFD *m)
{
	MFD=m;
	strcpy(bstrgt.name,"none");
	strcpy(bstrgt.ref,"none");
	time_to_int=0;
	mode.dir = (defEquDir==2);
  usingGS2 = false;
  trgt = &bstrgt;
	trgt->lon=trgt->lat=0;
	display_texts=defDispMode?defDispMode:3;
	mode.enc=defEncMode?defEncMode-1:1;

	sync_line=0;
	sync_num=defNumSol;
	sync_dispmin=1;

	bstrgt.ang=defReAng?defReAng*RAD:0.7*RAD;
	bstrgt.ant=defReAnt?defReAnt*RAD:75*RAD;
	bstrgt.alt=defReAlt?defReAlt:120;

	mode.deo = false;
	MMPut_done = false;

  sol.dataValid = false;
  deo.dataValid = false;
  burn.dataValid = false;
}


// ==============================================================================================================================================
//
SyncMFD::~SyncMFD()
{

}


// ==============================================================================================================================================
//
void SyncMFD::Write(FILEHANDLE scn)
{
	unsigned int i;
	char save_Target[32];
	char save_Reference[32];
	strncpy(save_Target,trgt->name,31);
	strncpy(save_Reference,trgt->ref,31);

	for (i=0;i<strlen(save_Target);i++) if (save_Target[i]==' ') save_Target[i]='_';
	for (i=0;i<strlen(save_Reference);i++) if (save_Reference[i]==' ') save_Reference[i]='_';

	oapiWriteScenario_string (scn, "Target", save_Target);
	oapiWriteScenario_string (scn, "Reference", save_Reference);

	sprintf(LineBuffer,"%lg %lg %d %d %lg %d",trgt->lon, trgt->lat, mode.dir?2:1, mode.enc, sync_line, sync_num);

	oapiWriteScenario_string (scn, "Data",LineBuffer);
}


// ==============================================================================================================================================
//
void SyncMFD::Read(FILEHANDLE scn)
{
	bool go=true;
	int tmpi;
	char *line;

	while (go) {

		go=oapiReadScenario_nextline (scn, line);

		if (go) {
			if (!_strnicmp (line, "Target", 6)) {
        usingGS2 = false;
        trgt = &bstrgt;
        sscanf (line+6, "%s",bstrgt.name);
			}
			else
			if (!_strnicmp (line, "Reference", 9)) {
        usingGS2 = false;
        trgt = &bstrgt;
				sscanf (line+9, "%s",bstrgt.ref);
			}
			else
			if (!_strnicmp (line, "Data", 4)) {
        usingGS2 = false;
        trgt = &bstrgt;
				sscanf(line+4,"%lg %lg %d %d %lg %d",
					&bstrgt.lon, &bstrgt.lat, &tmpi, &mode.enc,&sync_line,&sync_num);
				mode.dir = (tmpi==2);
			}
			else go=false;
		}
	}

	unsigned int i;
	for (i=0;i<strlen(bstrgt.name);i++) if (bstrgt.name[i]=='_') bstrgt.name[i]=' ';
	for (i=0;i<strlen(bstrgt.ref);i++) if (bstrgt.ref[i]=='_') bstrgt.ref[i]=' ';
}


// ==============================================================================================================================================
//
void SyncMFD::Direct()
{
	mode.dir = !mode.dir;
}


// ==============================================================================================================================================
//
void SyncMFD::ChangeMode()
{
	display_texts++;
	if (display_texts>3) display_texts=1;
}


// ==============================================================================================================================================
//
void SyncMFD::EncMode()
{
	mode.enc++;
	if (mode.enc>3) mode.enc=0;
}


// ==============================================================================================================================================
//
void SyncMFD::Deorbit()
{
	mode.deo=!mode.deo;
}


// ==============================================================================================================================================
//
void SyncMFD::LineAdjust(double x)
{
	//sync_orbit+=(int)x;
}


// ==============================================================================================================================================
//
double SyncMFD::Trl2Time(double trl, Orbit *src)
{
	if (trl<=src->trl) return 0;

	double tr = trl - src->trl;

	double per = floor(tr/PI2);

	trl-=(per*PI2);

	if (trl<=src->trl) {
		return (per*PI2) / src->mnm;
	}

	double mna = angular_distance(src->mna, tra2mna(limit(trl-src->lpe),src->ecc));

	double time = ((per*PI2+mna) / src->mnm);

	return time;
}


// ==============================================================================================================================================
//
VECTOR3 SyncMFD::ComputeDeOrbit(double mu, double rea, VECTOR3 _entry, VECTOR3 _cp)
{
	double alpha  = angle(_entry, _cp);
	double refalt = length(_entry);
	double ca     = length(_cp);
	double vup    = sqrt(2.0*mu/refalt);	// Upper Limit = escape velocity
	double vlw    = 0.0;
	double v			= 0.0;
	double p			= 0.0;
	double a			= 0.0;
	double e			= 0.0;
	double t			= 0.0;
	double r			= 0.0;

	for (int i=0;i<32;i++) {

		v = (vup + vlw) / 2.0;

		p = cos(rea) * refalt * v; p*=p;	p/=mu;
		a = -mu*refalt / (v*v*refalt - 2.0*mu);
		e = sqrt(1.0 - p/a);
		t = PI2 - acos((p-refalt)/(refalt*e));
		r = p / (1.0+e*cos(t-alpha));

		if (r>ca) vup=v; else vlw=v;
	}

	VECTOR3 _n = crossp(_entry, _cp);
	VECTOR3 _q = crossp(_n, _cp);

	double fpa = tra2fpa(t-alpha, e);
	double ve  = sqrt(2.0*mu/ca - mu/a);
	VECTOR3 _v = unit(_q)*cos(fpa)*ve + unit(_cp)*sin(fpa)*ve;
	return _v;
}


// ==============================================================================================================================================
//
bool SyncMFD::ComputeDeOrbit(double mu, double apd, double refalt, double rea, double *pre, double *dV)
{

	double s   = sin(PI05-rea)*sin(PI05-rea);
	double sma = ((apd*apd) - (refalt*refalt*s)) / ((2.0*apd) - (2.0*refalt*s));
	double ecc = (apd / sma) - 1.0;
	double vel = sqrt(2.0*mu/apd - mu/sma);
	double par = sma * (1.0-(ecc*ecc));

	*pre = PI - acos( ((par/refalt)-1.0) / ecc);
	*dV = vel;

	return true;
}


// ==============================================================================================================================================
//
bool SyncMFD::InterpolateClosestPassage(OBJHANDLE ref, Orbit *src, double lon,double lat,double orbit,double *diff,double *tim,double *head,double *tr)
{
	VECTOR3 Rot,Off;
	Orbit LEO;
	int i;
	double obli = 0.0;
	double trans = 0.0;
	double per = 0.0;
	double offset = 0.0;
	double size = 0.0;
	double time = 0.0;

	// Rotation elements
	obli   = oapiGetPlanetObliquity(ref);
	trans  = oapiGetPlanetTheta(ref);
	per    = oapiGetPlanetPeriod(ref);
	offset = oapiGetPlanetCurrentRotation(ref);
	size   = oapiGetSize(ref);

	LEO.LEO(ref);

	double step  = PI2/16.01;
	double start = orbit;
	double end   = orbit+PI2+(step/2.0);
	double trl, dst2;
	double dot,old_dot=0;


	VECTOR3 pos = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0);
	VECTOR3 gpv = _V(0,0,0);
	VECTOR3 spd = _V(0,0,0);
	VECTOR3 relv = _V(0,0,0);
	VECTOR3 relp = _V(0,0,0);

	bool no_intercept=true;
	bool first=true;

	for (trl=start;trl<end;trl+=step) {

		// Caluclate time to a location
		time=Trl2Time(trl,src)/86400.0;

		// Caluclate Planets Rotation offset at that time
		PlanetAxis(obli,trans,offset,per,time,&Rot,&Off);

		// Calculate Ship's position vector and distance^2
		pos  = src->Position(trl);
		dst2 = dotp(pos,pos);

		// Reset the distance to correspond a surface position.
		pos  = set_length(pos,size);

		// Compute ship's surface velocity vector direction
		vel = crossp(src->norv,pos);

		// Compute ship's surface velocity vector magnitude
		vel = set_length(vel,src->ang*size/dst2);

		// Compute base's position vector
		gpv = VectorByLonLat(Rot,Off,lon,lat)*size;

		// Compute speed of the base
		spd = GroundSpeedVector(ref,time,lon,lat);

		// Copmute relative position to the base
		relv = (vel-spd);
		relp = (pos-gpv);

		dot = dotp(relv,relp);

		if (old_dot<0 && dot>0 && !first) {
			if (angle(pos,gpv)<PI05) {
				start=trl-step;
				no_intercept=false;
				break;
			}
		}

		first=false;
		old_dot=dot;
	}


    // Return N/A if no passage points found
	if (no_intercept) {
		if (head) *head = 0;
		if (diff) *diff = 0;
		if (tim)  *tim  = 0;
		if (tr)   *tr   = start+PI2-PI05;
		return false;
	}


	trl=start;
	old_dot=-1; // We are approaching the base
	double trll=0;
	no_intercept=true;

	for (i=0;i<24;i++) {

		step/=2.0;

		// Caluclate time to a location
		time=Trl2Time(trl,src)/86400.0;

		// Caluclate Planets Rotation offset at that time
		PlanetAxis(obli,trans,offset,per,time,&Rot,&Off);

		// Calculate Ship's position vector and distance^2
		pos  = src->Position(trl);
		dst2 = dotp(pos,pos);

		// Reset the distance to correspond a surface position.
		pos  = set_length(pos,size);

		// Compute ship's surface velocity vector direction
		vel = crossp(src->norv,pos);

		// Compute ship's surface velocity vector magnitude
		vel = set_length(vel,src->ang*size/dst2);

		// Compute base's position vector
		gpv = VectorByLonLat(Rot,Off,lon,lat)*size;

		// Compute speed of the base
		spd = GroundSpeedVector(ref,time,lon,lat);

		// Copmute relative position to the base
		relv = (vel-spd);
		relp = (pos-gpv);

		dot = dotp(relv,relp);

		trll=trl;
		if (dot==0) {
			 no_intercept=false;
			 break; // closest position archived, break
		}

		if (dot>0) { // base is behind of us, step back
			trl=trl-step;
			no_intercept=false;  // Verify, We have a base passage.
		}
		else trl=trl+step;	// base is front of us, cuntinue stepping
	}


	double dif=angle(pos, gpv);

	VECTOR3 zero=crossp(pos,spd);

	if (head) *head = nangle(relp,zero,pos);
	if (diff) *diff = dif;
	if (tim)  *tim  = time*86400.0;
	if (tr)   *tr   = trll;

	return true;
}



// ==============================================================================================================================================
//
void SyncMFD::Update(HDC hDC)
{

	int pos;
	int ld=MFD->LineHeight;
	int fbp=MFD->FirstButton;
	char *Mnu = {"BaseSyncMFD v3.0\0"};

	int width=MFD->width;
	int height=MFD->height;

	double w=(double) width;
	double h=(double) height;

	int i;
	VECTOR3 Rot,Off,opos;
	double obli,trans,per,offset,diff,p,op,t,time,lon,lat;
	double trl,heading;
	char   name[32];
	static const int MAXSOLN = 160;
	double times[MAXSOLN],diffs[MAXSOLN],heads[MAXSOLN];
	int sol_found = 0;
	double max_diff=1000;
	bool   draw_text=false;

	Orbit LEO,ShipOrbit,Ecliptic;

	SetTextColor(hDC,white);
	SetTextAlign(hDC,TA_LEFT);
	TextOut(hDC,5,1,Mnu, (int)strlen(Mnu));

	if (mode.enc==0) Text(hDC,5,1+ld,"Latitude");
	if (mode.enc==1) Text(hDC,5,1+ld,"Closest passage");
	if (mode.enc==2) Text(hDC,5,1+ld,"Apoapsis");
	if (mode.enc==3) Text(hDC,5,1+ld,"Periapsis");

	if (sync_num<1) sync_num=1;
	if (sync_num>99) sync_num=99;


	OBJHANDLE ref = oapiGetObjectByName(trgt->ref);
	if (ref==NULL) {
    usingGS2 = false;
    trgt = &bstrgt;
    ref=ship->GetSurfaceRef();
  }

	if (ref==NULL) return;



	ShipOrbit.Elements(ship->GetHandle(), ref);
	Ecliptic.Ecliptic();

	oapiGetObjectName(ref, trgt->ref, 31);


  // Rotation elements

	obli   = oapiGetPlanetObliquity(ref);
	trans  = oapiGetPlanetTheta(ref);
	per    = oapiGetPlanetPeriod(ref);
	offset = oapiGetPlanetCurrentRotation(ref);

	// LEO
	LEO.LEO(ref);

  if (!usingGS2) {
    OBJHANDLE tgt = oapiGetBaseByName(ref, trgt->name);
    if (tgt) {
		  oapiGetObjectName(tgt,trgt->name,32);
		  oapiGetBaseEquPos(tgt,&trgt->lon, &trgt->lat);
	  }
	  else strcpy(trgt->name,"Surface");
  }


	double trle=ShipOrbit.TrlOfNode(&LEO);
	double EqI=angle(ShipOrbit.norv, LEO.norv);
	double ang=asin(sin(trgt->lat) / sin(EqI));

	double apos=limit(trle+ang);
	double bpos=limit(trle+PI-ang);

	double atime=ShipOrbit.TimeTo(apos);
	double btime=ShipOrbit.TimeTo(bpos);

	double dist=ShipOrbit.PeriapsisDistance();
	double apodist = dist=ShipOrbit.AopapsisDistance();
	if (dist<oapiGetSize(ref)) dist=oapiGetSize(ref);

	double zoom=w/(2.55 * (apodist + apodist + dist) / 3.0);
	double r=oapiGetSize(ref)*zoom;

	double x=w/2,y=h/2;
	double intpos=0;

	if (display_texts&2) {
		SelectObject(hDC,solid_pen_dgrey);
		DrawEllipse(hDC,x-r,y-r,x+r,y+r,w,h);

		if (mode.enc==0) {  // Latitude
			if ((sync_sel&1)==0) SelectObject(hDC,solid_pen_y), intpos=apos;
			else SelectObject(hDC,solid_pen_dgrey);
			r=ShipOrbit.Radius(apos)*zoom;
			DrawLine(hDC,x,y,x+r*cos(apos),x-r*sin(apos),w,h,false);

			if ((sync_sel&1)==1) SelectObject(hDC,solid_pen_y), intpos=bpos;
			else SelectObject(hDC,solid_pen_dgrey);
			r=ShipOrbit.Radius(bpos)*zoom;
			DrawLine(hDC,x,y,x+r*cos(bpos),x-r*sin(bpos),w,h,false);
		}

		if (mode.enc==1) { // Closest Passage
			r=ShipOrbit.Radius(sync_trl)*zoom;
			intpos=sync_trl;

			SelectObject(hDC,solid_pen_y);
			DrawLine(hDC,x,y,x+r*cos(sync_trl),x-r*sin(sync_trl),w,h,false);
		}

		if (mode.enc==2 || mode.enc==3) { // Aopapsis Periapsis
			SelectObject(hDC,solid_pen_y);
			r=ShipOrbit.Radius(sync_trl)*zoom;
			DrawLine(hDC,x,y,x+r*cos(sync_trl),x-r*sin(sync_trl),w,h,false);

			SelectObject(hDC,solid_pen_grey);
			r=ShipOrbit.Radius(sync_line)*zoom;
			DrawLine(hDC,x,y,x+r*cos(sync_line),x-r*sin(sync_line),w,h,false);
		}

		ShipOrbit.SetProjection(&ShipOrbit);
		ShipOrbit.GDIDraw(hDC,green,w,h,zoom,true,true);

		if (mode.deo) {
			r=ShipOrbit.Radius(deo.trlBurn)*zoom;
			SelectObject(hDC,solid_pen_white);
			DrawLine(hDC,x,y,x+r*cos(deo.trlBurn),x-r*sin(deo.trlBurn),w,h,false);
		}
	}



	if (EqI>=trgt->lat || mode.enc!=0) {
		// Usual case... target in range

		draw_text=true;
		SetTextColor(hDC,green);
		pos=(fbp+(ld*8));

		if (display_texts&1 && !mode.deo) {
			if (mode.enc==0) {
				Text(hDC,5,pos," #: Time:");
			Text(hDC,width/2,pos,"Lon Diff:"), pos+=ld;
			}	else {
				Text(hDC,5,pos," #: Time:");
			Text(hDC,5+width/3,pos,"  Dist:");
			Text(hDC,5+width*2/3,pos,"  Heading:"), pos+=ld;
			}
		}

		if (atime<btime && atime>0) sync_sel=0;
		else if (btime>0) sync_sel=1;
		else sync_sel=0;

		for (i=0;i<MAXSOLN;i++) {
			times[i]=0;
			diffs[i]=0;
		}
		sync_min = -1;


		if (ShipOrbit.ecc<1 && mode.enc==0) {
			for (i=0;i<MAXSOLN;i++) {

				if (i&1) time=MAX(atime, btime);
				else     time=MIN(atime, btime);

				if (time==atime) op=apos;
				else             op=bpos;

				p=(double)i;
				if (i>1) time+=ShipOrbit.Period()*floor(p/2);

				t = time/86400;
				PlanetAxis(obli,trans,offset,per,t,&Rot,&Off);

				opos=ShipOrbit.Position(op);

				LonLat(opos,Rot,Off,&lon,&lat);
				diff=lon-trgt->lon;

				if (fabs(diff)<max_diff) {
					max_diff=fabs(diff), sync_time=time, sync_trl=op;
					sync_min=i;
				}

				times[i]=time;
				diffs[i]=diff;
				if (time > 0.0 && diff >0.0) {
					sol_found++;
					if (sol_found==sync_num) break;
				}
			}
		}


		max_diff=1e10;

		if (ShipOrbit.ecc<1 && mode.enc==1) {

			double posit=ShipOrbit.trl;

			for (i=0;i<MAXSOLN;i++) {

				InterpolateClosestPassage(ref,&ShipOrbit,trgt->lon,trgt->lat,posit,&diff,&time,&heading,&trl);

				posit=trl+PI05;

				if (diff<max_diff && diff>0) {
					max_diff=diff, sync_time=time, sync_trl=limit(trl);
					sync_min=i;
				}

				heads[i]=heading;
				times[i]=time;
				diffs[i]=diff;
				if (time > 0.0 && diff >0.0) {
					sol_found++;
					if (sol_found==sync_num) break;
				}
			}
		}

		if (ShipOrbit.ecc<1 && (mode.enc==2 || mode.enc==3)) {
			for (i=0;i<MAXSOLN;i++) {

				if (mode.enc==2) sync_line=limit(ShipOrbit.lpe+PI);
				else sync_line=limit(ShipOrbit.lpe);

				time=ShipOrbit.TimeTo(sync_line) + ShipOrbit.Period() * (double)i;
				t = time / 86400.0;

				PlanetAxis(obli,trans,offset,per,t,&Rot,&Off);

				VECTOR3 gpv=VectorByLonLat(Rot,Off,trgt->lon,trgt->lat);
							VECTOR3 pos=ShipOrbit.Position(sync_line);

				diff = angle(gpv,pos);
				heading = nangle(pos-gpv,Rot,gpv);

				ShipOrbit.Longitude(gpv,NULL,NULL,&trl);

				if (diff<max_diff && diff>0) {
					max_diff=diff, sync_time=time, sync_trl=limit(trl);
					sync_min=i;
				}

				if (time==0) time=0.1;
				heads[i]=heading;
				times[i]=time;
				diffs[i]=diff;
				if (time > 0.0 && diff >0.0) {
					sol_found++;
					if (sol_found==sync_num) break;
				}
			}
		}

		// Hyperbolic Orbit
		if (ShipOrbit.ecc>=1 && (atime>0 || btime>0)) {
			mode.enc=0;
			for (i=0;i<2;i++) {

				if (atime>0 && btime>0) {
					if (i&1) time=MAX(atime, btime);
					else     time=MIN(atime, btime);
				}
				else time=(atime>0 ? atime : btime);

				op = (time==atime? apos : bpos);

				p=(double)i;
				if (i>1) time+=ShipOrbit.Period()*floor(p/2);
				t = time/86400;
				PlanetAxis(obli,trans,offset,per,t,&Rot,&Off);

				opos=ShipOrbit.Position(op);
				LonLat(opos,Rot,Off,&lon,&lat);
				diff=lon-trgt->lon;

				if (fabs(diff)<max_diff) {
					max_diff=fabs(diff), sync_time=time, sync_trl=op;
					sync_min=i;
				}

				times[i]=time;
				diffs[i]=diff;

				if (atime<0 || btime<0) break;
			}
		}

		if (sync_min > -1) {
			sol.num = sync_min+1;
			sol.tSol = times[sync_min];
			sol.dist = diffs[sync_min];
			sol.hdg = heads[sync_min];
			sol.dataValid = true;
		} else {
			sol.dataValid = false;
		}

		double rad=oapiGetSize(ref);
		int no=0;
		int disp_i = 1;

		if (display_texts&1 && !mode.deo) {
			for (i=0;i<MAXSOLN;i++) {
				if (i==sync_min) SetTextColor(hDC,lyellow);
				else SetTextColor(hDC,lgreen);
				if (times[i]>0.0 && diffs[i]>=0.0) {
					if (disp_i >= sync_dispmin && disp_i <= sync_dispmin+7) {
						if (mode.enc==0) {
							sprintf(name,"%2d: ",disp_i);
							Text(hDC,5,pos,name,times[i]);
							TextA(hDC,width/2,pos,"",diffs[i]*DEG), pos+=ld;
						}	else { // enc_mode 1,2,3
							no++;
							sprintf(name,"%2d: ",disp_i);
							Text(hDC,5,pos,name,times[i]);
							Text(hDC,5+width/3,pos,"  ",diffs[i]*rad);
							TextA(hDC,5+width*2/3,pos,"  ",heads[i]*DEG), pos+=ld;
						}
					}
					disp_i++;
					if (disp_i > sync_num) break;
				}
			}
		}
		sync_sel+=sync_min;
	}	else {
		// We are in Latitude mode, and the target is outside of our inclination - i.e. no solution

		sol.dataValid = false;
		SetTextAlign(hDC,TA_CENTER);
		SetTextColor(hDC,yellow);
		pos=(fbp+(ld*8));
		Text(hDC,width/2,pos,"Target Out of Range"); pos+=ld;
	}



	if (mode.dir) {
		// Calculate burn for plane change correction in DIRECT mode

		if (EqI<trgt->lat && mode.enc==0) {
			double trl;
			PlanetAxis(obli,trans,offset,per,0,&Rot,&Off);
			VECTOR3 gpv = VectorByLonLat(Rot,Off,trgt->lon,trgt->lat);
			ShipOrbit.Longitude(gpv,NULL,NULL,&trl);
			sync_time=ShipOrbit.TimeTo(trl);
			sync_trl=trl;
		}

		time_to_int=sync_time;
		double trl=sync_trl;

		PlanetAxis(obli,trans,offset,per,time_to_int/86400.0,&Rot,&Off);
		VECTOR3 pos = ShipOrbit.Position(trl);
		VECTOR3 gpv = VectorByLonLat(Rot,Off,trgt->lon,trgt->lat);
		VECTOR3 lan = crossp(gpv, pos);
		VECTOR3 nor = ShipOrbit.norv;

		ShipOrbit.Longitude(lan,NULL,NULL,&sol.trlBurn);

		sol.rIn = fabs(asin(dotp(gpv,nor)));

		double a = ShipOrbit.TimeToPoint(sol.trlBurn);
		double b = ShipOrbit.TimeToPoint(limit(sol.trlBurn+PI));

		if (fabs(a)<fabs(b)) sol.tToBurn =a, sol.nmlBurn=true;
		else				 sol.tToBurn =b, sol.nmlBurn=false;

		sol.dV = sol.rIn*ShipOrbit.ang/ShipOrbit.Radius(sol.trlBurn);
		sol.tBurn =BurnTimeBydV(sol.dV,ship);

	}	else {
		// Calculate burn for plane change correction in EQUATORIAL mode

		sol.trlBurn=ShipOrbit.TrlOfNode(&LEO);
		sol.rIn=MAX(trgt->lat-EqI,0);

		double a=ShipOrbit.TimeToPoint(sol.trlBurn);
		double b=ShipOrbit.TimeToPoint(limit(sol.trlBurn+PI));

		if (fabs(a)<fabs(b)) sol.tToBurn =a, sol.nmlBurn=true;
		else				 sol.tToBurn =b, sol.nmlBurn=false;

		sol.dV = sol.rIn*ShipOrbit.ang/ShipOrbit.Radius(sol.trlBurn);
		sol.tBurn =BurnTimeBydV(sol.dV,ship);
	}

	if (display_texts&2) {
		ShipOrbit.DrawPlaneIntersection(hDC,sol.trlBurn,w/2,h/2,zoom,grey);
	}

	SetTextAlign(hDC,TA_LEFT);
	pos=1;
	SetTextColor(hDC,grey);
  if (usingGS2) {
	  Text(hDC,width*1/2,pos,"Linked ","to GS2"); pos+=ld;

    // Check if GS2 changed target
    if (bstrgt.lat != gs2trgt->lat || bstrgt.lon != gs2trgt->lon) {
        strcpy(bstrgt.ref, gs2trgt->ref);
        strcpy(bstrgt.name, gs2trgt->name);
        bstrgt.lat = gs2trgt->lat;
        bstrgt.lon = gs2trgt->lon;
        bstrgt.alt = gs2trgt->alt;
        bstrgt.ang = gs2trgt->ang;
        bstrgt.ant = gs2trgt->ant;
    }

  } else {
	  Text(hDC,width*1/2,pos,"Ref ",trgt->ref); pos+=ld;
  }
  Text(hDC,width*1/2,pos,"Tgt ",trgt->name); pos+=ld;

	if (display_texts&1) {
		if (!mode.deo) {
			// Display the burn solution for plane change

			deo.dataValid = false;
			pos=fbp;
			SetTextColor(hDC,lgreen);

			if (trgt->lat<0) TextA(hDC,5,pos,"Lat ",fabs(DEG*trgt->lat),"S"), pos+=ld;
			else TextA(hDC,5,pos,"Lat ",DEG*trgt->lat,"N"), pos+=ld;

			if (trgt->lon<0) TextA(hDC,5,pos,"Lon ",fabs(DEG*trgt->lon),"W"), pos+=ld;
			else TextA(hDC,5,pos,"Lon ",DEG*trgt->lon,"E"), pos+=ld;

			TextA(hDC,5,pos,"EqI ",EqI*DEG), pos+=ld;


			PlanetAxis(ref,0,&Rot,&Off);
			VECTOR3 base_pos = VectorByLonLat(Rot,Off,trgt->lon,trgt->lat);

			VECTOR3 ship_pos; ship->GetRelativePos(ref,ship_pos);
			VECTOR3 ship_vel; ship->GetRelativeVel(ref,ship_vel);

			VECTOR3 gsp=GroundSpeedVector(ref,ship);
			VECTOR3 zero=crossp(ship_pos,ship_vel);
			double head = nangle(gsp,zero,ship_pos);

			TextA(hDC,5,pos,"Hed ",head*DEG), pos+=ld;
			Text(hDC,5,pos, "GSp ",length(gsp)), pos+=ld;
			Text(hDC,5,pos, "Dst ",angle(ship_pos,base_pos)*oapiGetSize(ref)), pos+=ld;
			if (sol.dataValid) {
					Text(hDC,5,pos, "Alt ",length(ShipOrbit.Position(intpos))-oapiGetSize(ref));
      }
			pos+=ld;


			pos=fbp;
			int xx=width/2;
			SetTextColor(hDC,green);
			if (mode.dir) Text(hDC,xx,pos,"Direct:"), pos+=ld;
			else Text(hDC,xx,pos,"Equator:"), pos+=ld;

			SetTextColor(hDC,lgreen);
			TextA(hDC,xx,pos,"RIn ",sol.rIn*DEG), pos+=ld;
			TextA(hDC,xx,pos,"LAN ",sol.trlBurn*DEG), pos+=ld;
			Text(hDC,xx,pos," Tn ",sol.tToBurn), pos+=ld;
      if (abs(sol.tBurn) > 0.1) {
  			if (sol.nmlBurn) Text(hDC,xx,pos,"PlC ",sol.tBurn,"s (+)"), pos+=ld;
	  		else Text(hDC,xx,pos,"PlC ",sol.tBurn,"s (-)"), pos+=ld;
		  	Text(hDC,xx,pos," dV ",sol.dV,"m/s"), pos+=ld;
      } else {
        Text(hDC,xx,pos,"PlC 0.000s"), pos+=ld;
		  	Text(hDC,xx,pos," dV 0.000m/s"), pos+=ld;
      }

      if (sol.dataValid) {
        burn.dV = sol.dV;
        burn.orientation = sol.nmlBurn ? 1 : -1;
        burn.tToInstBurn = sol.tToBurn;
        burn.dataValid = true;
      } else {
        burn.dataValid = false;
      }

		}	else {
			// Display the deorbit parameters and burn solution

			pos=fbp;
			SetTextColor(hDC,white);
			Text(hDC,5,pos,"De-Orbit Program"), pos+=2*ld;

			SetTextColor(hDC,lgreen);
      if (usingGS2) {
			  Text(hDC,5,pos, "Ang, Ant, Alt from GS2"), pos+=ld;
        TextA(hDC,5,pos,"GS2 Ang ",trgt->ang*DEG), pos+=ld;
			  TextA(hDC,5,pos,"GS2 Ant ",trgt->ant*DEG), pos+=ld;
			  Text(hDC,5,pos, "GS2 Alt ",trgt->alt,"km"), pos+=2*ld;
      } else {
        TextA(hDC,5,pos,"Ang ",bstrgt.ang*DEG), pos+=ld;
			  TextA(hDC,5,pos,"Ant ",bstrgt.ant*DEG), pos+=ld;
			  Text(hDC,5,pos, "Alt ",bstrgt.alt,"km"), pos+=2*ld;
      }
			double pre;
			ComputeDeOrbit(ShipOrbit.myy,ShipOrbit.rad,trgt->alt*1000+oapiGetSize(ref),trgt->ang,&pre,&deo.dV);
			deo.dV=ShipOrbit.vel-deo.dV;

			deo.trlBurn = limit(sync_trl - trgt->ant - pre);
			deo.tInstBurn = ShipOrbit.TimeToPoint(deo.trlBurn);
			deo.tBurn = BurnTimeBydV(deo.dV,ship);
			deo.tToBurn = deo.tInstBurn - 0.5 * deo.tBurn;

			Text(hDC,5,pos, "TBn ",deo.tToBurn,"s"), pos+=ld;
			TextA(hDC,5,pos,"TrL ",deo.trlBurn*DEG), pos+=ld;
			Text(hDC,5,pos, " dV ",deo.dV,"m/s"), pos+=ld;
			Text(hDC,5,pos, " BT ",deo.tBurn,"s"), pos+=2*ld;

			if (ShipOrbit.ecc>0.015) {
				SetTextColor(hDC,lgreen);
				Text(hDC,5,pos,"De-orbit burn data only accurate"); pos+=ld;
				Text(hDC,5,pos,"if your Ecc is 0.015 or less."); pos+=ld;
				Text(hDC,5,pos,"(If burn complete, please ignore.)"); pos+=ld;pos+=ld;
			}
			if (!mode.dir) {
				SetTextColor(hDC,lgreen);
				Text(hDC,5,pos,"Use \"Direct\" mode to re-synchronize"); pos+=ld;
				Text(hDC,5,pos,"the approach after the de-orbit");
			}
			deo.dataValid = true;
      burn.dV = -deo.dV;
      burn.orientation = 0;
      burn.tToInstBurn = deo.tToBurn;
      burn.dataValid = true;
		}
	}

	if (!MMPut_done) {
		ModMsgPutByRef("BaseSyncTarget", 2, bstrgt);
		ModMsgPutByRef("BaseSyncMode", 1, mode);
		ModMsgPutByRef("BaseSyncSolution", 2, sol);
		ModMsgPutByRef("BaseSyncDeorbit", 4, deo);
		//ModMsgPutByRef("BaseSyncBurn", 1, burn);
		if (burn.dataValid)
        {
            ModMsgPut("dv", burn.dV);
            ModMsgPut("InstantaneousBurnTime", burn.tToInstBurn);
            ModMsgPut("Orientation", burn.orientation);
        }
        else
        {
            ModMsgDelete("dv", burn.dV);
            ModMsgDelete("InstantaneousBurnTime", burn.tToInstBurn);
            ModMsgDelete("Orientation", burn.orientation);
        }
		MMPut_done = true;
	}
}
