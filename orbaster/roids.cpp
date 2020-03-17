#define STRICT
#define ORBITER_MODULE

#include "orbitersdk.h"
#include <time.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "CppFileReader.h"

#define BUFLEN 255
//#define FOV 80*RAD

int GetRand(int from, int to)
{
	return rand() % to + from;
}
double GDRand(double from, double  span)
{
	return (double)rand() / (double)RAND_MAX * span + from;
}
int _OBJ_COUNT = 10;
#define KM 1000
std::vector<OBJHANDLE> hVes;
std::vector<std::string> meshes;
BOOL Init = FALSE;
bool space = true;
FILEREADER g_FR;
VECTOR3 c_ofs = {50,50,300};
VECTOR3 d_ofs = {100,100,500}; 
VECTOR3 vel = {5, 5, 20};
VECTOR3 rot = {1.8, 1.8, 1.8};
//VECTOR3 relV = {0,0,-120};
VECTOR3 relV = {0,0,0};
double render_ofs = 50;
void init(void)
{
	//char cbuf[BUFLEN];
	//char str[BUFLEN];
	/*
    g_FR.LoadVector(ini,"SETTINGS","destroy_ofs",d_ofs);
	g_FR.LoadVector(ini,"SETTINGS","create_ofs",c_ofs);
	g_FR.LoadInt(ini,"SETTINGS","count",_OBJ_COUNT);
	g_FR.LoadVector(ini,"SETTINGS","relvel",relV);
	g_FR.LoadDouble(ini,"SETTINGS","renderalt",render_alt);
	*/
/*	
	std::ifstream ini;
	ini.open ("./Modules/Plugin/orbscn.ini");
	if (ini.is_open())
	{
		while (!ini.eof())
		{
			std::string line;
			std::getline(ini, line);
			size_t len = strlen("count=");
			if (!line.compare(0,len,"count=") && line.size() > len)  
			{
				line.assign(line, len, line.size());
				std::stringstream ssbuf(line);
				ssbuf >> _OBJ_COUNT;
			}
		}
		ini.close();
	}
*/	

	if (g_FR.LoadFile("./Modules/Plugin/orbscn.ini") == true)
	{
		_OBJ_COUNT = g_FR.LoadInt("SETTINGS","count");
		c_ofs = g_FR.LoadVector("SETTINGS","create_ofs");
		d_ofs = g_FR.LoadVector("SETTINGS","destroy_ofs");
		vel = g_FR.LoadVector("SETTINGS","velocity_max");
		rot = g_FR.LoadVector("SETTINGS","rotation_max");
		render_ofs = g_FR.LoadDouble("SETTINGS","render_ofs");
		space = g_FR.LoadBool("MODES","space");
		int mCount = g_FR.LoadInt("SETTINGS","mesh_count");
		for (int i = 0; i < mCount; i++)
		{
			string mesh;
			char cbuf[BUFLEN];
			memset(cbuf, 0, BUFLEN);
			sprintf(cbuf,"mesh_%d_name",i+1);
			mesh = g_FR.LoadString("SETTINGS",cbuf);
			meshes.push_back(mesh);
		}	
	}
	else 
	{
		sprintf(oapiDebugString(), "Faied to open \"./Modules/Plugin/orbscn.ini\"");
		meshes.push_back("deimos");
		meshes.push_back("phobos");
	}

	for (int i = 0; i < _OBJ_COUNT; i++)
	{
		OBJHANDLE h = 0;
		char name[BUFLEN];
		sprintf(name, "obj_%d",i+1);
		h = oapiGetVesselByName(name);
		// NULL handle is OK. We'll create the vessel then
		hVes.push_back(h);
		
	}
}


DLLCLBK void opcPreStep(double simt, double simdt, double mjd)
{
	if (oapiGetTimeAcceleration() > 10) 
	{
		//return;
	}
	char name[BUFLEN];
	VESSEL * v = oapiGetFocusInterface();
	VECTOR3 posRoid, posShip;
	VESSELSTATUS vs;
	memset(&vs, 0, sizeof(vs));
	
	if (Init == FALSE)
	{
		
		srand(time(NULL));
		Init = TRUE;
	
		init();
		vs.rbody = v->GetSurfaceRef();
		double rad = 2 * oapiGetSize (v->GetSurfaceRef());
		if (rad == 0) rad ++;
		double vel = sqrt (GGRAV * oapiGetMass (v->GetSurfaceRef()) / rad);
		vs.rpos = _V(rad,0,0);
		vs.rvel = _V(0,0,vel);


		for (unsigned int i = 0; i < hVes.size(); i++)
		{	
			if (!hVes.at(i))
			{
				memset(name, 0, sizeof(name));
				sprintf(name,"obj_%d",i+1);
				hVes[i] = oapiCreateVessel(name,"roid",vs);
				//VESSEL(hVes[i]).AddMesh(meshes[GetRand(0,meshes.size())].c_str());
			}
			try 
			{
				//VESSEL(hVes[i]).AddMesh(meshes.at(0).c_str());
				
				VESSEL(hVes[i]).AddMesh(meshes.at(GetRand(0, meshes.size())).c_str());
			}
			catch (...)
			{
				sprintf(oapiDebugString(), "Exception at mesh adding");
			}
		}
		
	}
	
	
	if (v->GroundContact())
		return;

	double render_alt = 0;
	OBJHANDLE hRef = v->GetSurfaceRef();
	if (oapiPlanetHasAtmosphere(hRef))
	{
		const ATMCONST * atm = oapiGetPlanetAtmConstants(hRef);
		if (atm)	
		{
			// 50 km above the atmosphere
			render_alt = atm->altlimit + render_ofs * KM;
		}
	}
	else
	{
		// if the body has no atmosphere then render 20 km above the surface
		render_alt = render_ofs * KM;
	}
/*
	if (v->GetAltitude() < render_alt)
		return;
	*/	
	if (FindWindow(NULL,(LPCSTR)"Orbiter: Select spacecraft"))
		return;
	
	double x, y, z, vx, vy, vz, rx, ry, rz;
	x = y = z = vx = vy = vz = rx = ry = rz = 0;
	
	
	
	v->GetStatus(vs);
	//oapiGetGlobalPos(v->GetHandle(),&posShip);
	//v->Global2Local(posShip,posShip);	    
	double fov = oapiCameraAperture();
	fov *= 2.667; // something is wrong with the FOV or my math so I have to increase it
	//double fov = 80 * RAD;
	d_ofs.x = d_ofs.y = d_ofs.z * tan(fov/2);
	//sprintf(oapiDebugString(), "fov = %.2lf", fov*DEG);
	//sprintf(oapiDebugString(), "fov = %.2lf, d_ofs.z= %.2lf, d_ofs.x = %.2lf",fov*DEG,  d_ofs.z, d_ofs.x);
	double radius_ofs = 200;
	double y_ofs = 200;
	MATRIX3 obliquity;
	VECTOR3 pos_ecl, pos, vel, rel_pos;
	memset (&obliquity, 0, sizeof(obliquity));
	OBJHANDLE hGRef = v->GetGravityRef();
	oapiGetPlanetObliquityMatrix (hRef, &obliquity);
	v->GetRelativePos (hGRef, pos);
	pos_ecl = pos;
	v->GetRelativeVel (hGRef, vel);
	pos = tmul (obliquity, pos);
	vel = tmul (obliquity, vel);
	double alpha = abs(atan(vel.x/vel.z));
	

	//sprintf(oapiDebugString(), "vel.z = %.1lf, vel.x = %.1lf",vel.z, vel.x);
	for (unsigned int i = 0; i < hVes.size(); i++)
	{
		
		//OBJHANDLE hRef = v->GetSurfaceRef();
		oapiGetRelativePos(hVes.at(i), hGRef, &posRoid);
		//oapiGetRelativeVel(hVes.at(i), hRef, &vel);
	
		//
		pos = tmul (obliquity, posRoid);		
		rel_pos = pos - posRoid;
		
		
			
		if ( sqrt(rel_pos.x*rel_pos.x + rel_pos.z*rel_pos.z) > radius_ofs )
		{
			VESSEL * vRoid = oapiGetVesselInterface(hVes.at(i));
			if (!vRoid) return;
			memset(&vs, 0, sizeof(vs));
			v->GetStatus(vs);
			
			double radius = GDRand(-radius_ofs, radius_ofs);
			if (GetRand(1,10) > 5)
			{	
				radius = -radius; 
			}
			
			double r_z = radius * sin(alpha);
			double r_x = radius * cos(alpha);
			sprintf(oapiDebugString(), "r = %.1lf,rz = %.1lf,rx = %.1lf", radius,r_z,r_x);
			if ( (vel.z > 0 && vel.x > 0) || (vel.z < 0 && vel.x < 0)) 
			{
				r_z = - r_z;
			}
			//memset(&vs, 0, sizeof(vs));
			
			vs.rpos.x += r_x;
			vs.rpos.z += r_z;
			
			//double rad = sqrt(vs.rpos.x * vs.rpos.x + vs.rpos.z * vs.rpos.z);
			double rad = sqrt(pos.z  * pos.z  + pos.x * pos.x);
			if (radius == 0) radius ++;
			double vel = sqrt (GGRAV * oapiGetMass (hGRef) / rad);		
			vs.rvel = _V(0,0,vel);
			

			vRoid->DefSetState(&vs);

		}
		
	}
	if (space)
	{
		for (unsigned int i = 0; i < hVes.size(); i++)
		{
			oapiGetGlobalPos(hVes[i],&posRoid);
			v->Global2Local(posRoid,posRoid);

			if ( (posRoid.z < 0) || (fabs(posRoid.z) > d_ofs.z)
				|| (fabs(posRoid.y) > posRoid.z / d_ofs.z * d_ofs.y) 
				|| (fabs(posRoid.x) > posRoid.z / d_ofs.z * d_ofs.x))
			{
				VESSEL * vRoid = oapiGetVesselInterface(hVes[i]);
				if (!vRoid) return;
				
				/*
				z = GDRand(1, c_ofs.z);
				x = GDRand(-c_ofs.x,c_ofs.x);
				if (GetRand(1,10) > 5)
					x = -x;
				y = GDRand(0,c_ofs.y);
				if (GetRand(1,10) > 5)
					y = -y;
				*/
				// if thrusting, generate far away
				
				if (v->GetThrusterGroupLevel(THGROUP_MAIN))
					z = GDRand(c_ofs.z * 0.6, c_ofs.z);	
				else
					z = GDRand(c_ofs.z * 0.1, c_ofs.z);	
				bool genClose = false;
				if (z < c_ofs.z*0.5) // if generated at close distance
				{
					// make the objects appear from the borders of your screen
					c_ofs.x = c_ofs.y = z / d_ofs.z * d_ofs.x;	
					genClose = true;
					if (GetRand(1,10) > 5)
					{
						x = c_ofs.x;
						if (GetRand(1,10) > 5)
							x = -x;
						y = GDRand(0,c_ofs.y);
					}			
					else
					{
						y = c_ofs.y;
						if (GetRand(1,10) > 5)
							y = -y;
						x = GDRand(-c_ofs.x,c_ofs.x);
					}
				}			
				else 
				{
					c_ofs.x = c_ofs.y = d_ofs.x / d_ofs.z * d_ofs.x * 0.7;	
					x = GDRand(-c_ofs.x,c_ofs.x);
					y = GDRand(0,c_ofs.y);
					//x = GDRand(- z / c_ofs.z * c_ofs.x, z / c_ofs.z * c_ofs.x);
					if (GetRand(1,10) > 5)
						x = -x;
					//y = GDRand(0,z / c_ofs.z * c_ofs.y);
					if (GetRand(1,10) > 5)
						y = -y;
				}
						
				
				//y = x= 0;	
			    //sprintf(oapiDebugString(), "z = %.2lf, x = %.2lf,y = %.2lf",z,x,y);
				vx = GDRand(-vel.x,vel.x);
				vy = GDRand(0,vel.y);
				vz = GDRand(-vel.z,vel.z);
				
				if (genClose) // prevent disappearing imediately
				{
					if (x > 0 && vx > 0) vx = -vx;
					if (y > 0 && vy > 0) vy = -vy;
					vz /= 3;
					//if (z > 0 && vz > 0) vz = -vz
				}
				else 
				{
					if (GetRand(1,10) > 5)
						vx = -vx;
					if (GetRand(1,10) > 5)
						vy = -vy;
					//if (GetRand(1,10) > 5)
					//	vz = -vz;
				}			
				if (GetRand(1,10) > 5)
					vz = -vz;

				relV.x = vx;	
				relV.y = vy;
				relV.z = vz;
				
				rx = GDRand(-rot.x,rot.x);
				ry = GDRand(-rot.y,rot.y);
				rz = GDRand(-rot.z,rot.z);
				if (GetRand(1,10) > 5)
						rx = -rx;
				if (GetRand(1,10) > 5)
					ry = -ry;
				if (GetRand(1,10) > 5)
						rz = -rz;
				
				posShip.x += x;
				posShip.y += y;
				posShip.z += z;
				v->Local2Rel(posShip,posShip);
			    VECTOR3 rofs;
		        v->GlobalRot(relV,rofs);
		        vs.rpos = posShip;
				
				vs.vrot.x = rx;
				vs.vrot.y = ry;
				vs.vrot.z = rz;
				//vs.rposRoid.x += 30;
				//vs.rpos.x += x;
				//vs.rpos.y += y;
				//vs.rpos.z += z;
	            vs.rvel+=rofs;
				
			    vRoid->DefSetState(&vs);
			}
		}
	} // if space
}