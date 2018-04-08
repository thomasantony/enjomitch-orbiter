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
#pragma once

#ifndef __BASESYNCEXPORTS_H
#define __BASESYNCEXPORTS_H

#include "MMExt2_Advanced.hpp"

//
// This defines the five data structures used by BaseSyncMFD to export internal variables via MMExt2:
//
// 1. BaseSyncExportTgtStruct ... current target reference body, name, lat, lon
// 2. BaseSyncExportModeStruct ... information on the modes we are in (lat, apo, peri, closest, plus equ/dir, plus deorbit or not)
// 3. BaseSyncExportSolStruct ... information on the current best solution found (orbit number, time, distance, etc.), or dataValid = false if no solution
// 4. BaseSyncExportBurnStruct ... information on the plain change burn needed to align with the base. The burn data depends on the mode.equ (i.e. direct or equatorial burn)
// 5. BaseSyncExportDeorbitStruct ... information on the deorbit retro burn needed to hit the desired reentry parameters, or dataValid = false if no solution
// 
// 
// See MMExt2 docuemtnation, plus Glideslope for how to consume this. 
//

#pragma pack(push)
#pragma pack(8)
#define BASESYNC_EXPORT_TGT_VER 2
struct BaseSyncExportTgtStruct: public MMExt2::MMStruct {	// Target Data
	BaseSyncExportTgtStruct():MMStruct(BASESYNC_EXPORT_TGT_VER,sizeof(BaseSyncExportTgtStruct)) {};
	char		ref[32];			// Reference planet or moon
	char		name[32];		// Current target base, or "Surface"
	double	lat;				// Target latitude (radians)
	double	lon;				// Target longitude (radians)
	double	alt;				// Altitude at start of glideslope (km)
	double	ang;				// Angle of reentry at start of glideslope (radians)
	double	ant;				// Angle of reentry at start of glideslope (radians)
};

#define BASESYNC_EXPORT_MODE_VER 1
struct BaseSyncExportModeStruct: public MMExt2::MMStruct {	// Current Mode
	BaseSyncExportModeStruct():MMStruct(BASESYNC_EXPORT_MODE_VER,sizeof(BaseSyncExportModeStruct)) {};
	int		enc;				// 0 = Latitude, 1 = Closest, 2 = Aopapsis, 3 = Periapsis
	bool		dir;				// True if E/D is Direct, false if Equator
	bool		deo;				// True if we are in deorbit screen
};

#define BASESYNC_EXPORT_SOL_VER 2
struct BaseSyncExportSolStruct: public MMExt2::MMStruct {	// Best Solution Data
	BaseSyncExportSolStruct():MMStruct(BASESYNC_EXPORT_SOL_VER,sizeof(BaseSyncExportSolStruct)) {};
	bool		dataValid;		// True if we have a solution (e.g. if we are in latitude mode, there may not be a solution)
	int		num;				// Best solution number
	double	tSol;				// Time until best solution
	double	dist;				// Min distance at best solution
	double	hdg;				// Bearing to target at best solution
	double	rIn;				// Relative inclination of target

	double	dV;				// Magnitude of plane change (m/s) to put best solution right over the target
	double	tBurn;			// Magnitude of plane change (secs of burn time)
	double	tToBurn;			// Time to burn (at the node)
	double	trlBurn;			// TRL at the burn point
	bool		nmlBurn;			// Normal (True) or AntiNormal (False) burn needed
};

#define BASESYNC_EXPORT_DEO_VER 4
struct BaseSyncExportDeorbitStruct: public MMExt2::MMStruct {// Deorbit Data Structure
	BaseSyncExportDeorbitStruct():MMStruct(BASESYNC_EXPORT_DEO_VER,sizeof(BaseSyncExportDeorbitStruct)) {};
	bool		dataValid;		// True if we have calculated a deorbit solution (i.e. pressed DEO)
	double	dV;				// Magnitude of deorbit retro burn (m/s)
	double	tBurn;			// Magnitude of deorbit retro burn (secs of burn time)
	double	tInstBurn;			// Time to deorbit burn 
	double	tToBurn;			// Time to deorbit burn 
	double	trlBurn;			// TRL of burn point
};

#define BASESYNC_EXPORT_BURN_VER 1
struct BaseSyncExportBurnStruct: public MMExt2::MMStruct {// Export of required burn (for Burn Time Calculator or other AP's)

	BaseSyncExportBurnStruct():MMStruct(BASESYNC_EXPORT_BURN_VER,sizeof(BaseSyncExportBurnStruct)) {};

	bool	  dataValid;		  // True if there is a calculated solution
	double	dV;				      // Magnitude of burn (m/s)
	double	tToInstBurn;	  // Time to instantaneous burn 
	int	    orientation;	  // -1 for anti-normal, 0 for retro, 1 for normal

};

#pragma pack(pop)
#endif 
