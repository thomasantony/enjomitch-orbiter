// ==============================================================
//                 ORBITER MODULE: AbsoluteKillrot v.1.1
//                  Part of the ORBITER SDK
// 	   Copyright (C) 2007  Simon Ender "Enjo" ender-sz@go2.pl
//					    All rights reserved
//
//
// This plugin helps you to cancel all rotation, after regular
// killrot has finished its job and the ship still rotates, which 
// is noticable under high time accelerations, thus during 
// interplanetary travel. The inconvenience may also be spotted
// during docking.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ========================================================================

#define STRICT
#define ORBITER_MODULE 
#include "orbitersdk.h" 
 
HINSTANCE g_hDLL; 
 
DLLCLBK void InitModule (HINSTANCE hModule) // perform global module initialisation here 
{  g_hDLL = hModule;   } 

DLLCLBK void ExitModule (HINSTANCE hModule) // perform module cleanup here 
{ } 

DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel
	VECTOR3 avel; // will be a rotation vector
	if (vessel!=NULL) // check if pointer is valid
	{
		if (vessel->GetNavmodeState (NAVMODE_KILLROT))  // check if Killrot autopilot has been enabled
		{
			OBJHANDLE objPlanet = vessel->GetSurfaceRef();  // Grab the closest planet
			double CutoffAlt=0;
			if (oapiPlanetHasAtmosphere(objPlanet)==1) // Sun returns 48, hence == 1
			{
				const ATMCONST *atm = oapiGetPlanetAtmConstants (objPlanet);  // Get atmospheric parameters
				if (atm!=NULL) 	CutoffAlt = atm->altlimit;  // if pointer is valid assign a value of max ATM alt
			}
			if (CutoffAlt > 10e15) CutoffAlt=0; // sanity check
			if (vessel->GetAltitude() > CutoffAlt)    
			// ^ Check if you are above atmosphere to eliminate deliberate killrot holding in ATM
			{
				vessel->GetAngularVel(avel);  // get current angular velocity
				if (sqrt(avel.data[0]*avel.data[0] + avel.data[1]*avel.data[1] + avel.data[2]*avel.data[2] ) < 0.001 )
				// ^ check if current ang. vel. is small, ie. if killrot is finishing the manoeuvre
				{
				for (int i = 0; i < 3; i++) 
					avel.data[i] = 0.0;	 // set all angular velocity axes to 0
				vessel->SetAngularVel (avel);   // perform Der Uber Killrot
				}
			}
		}
	}
}
