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


#ifndef __ORBIT_ReferenceClass_H
#define __ORBIT_ReferenceClass_H


#include "OrbiterSDK.h"


class ReferenceClass {
	
public:

				ReferenceClass();
				~ReferenceClass();

	OBJHANDLE   FindStar();
	OBJHANDLE   Get2ndReferenceForShip();
	OBJHANDLE	GetReference(OBJHANDLE);
	OBJHANDLE	GetReferenceByName(char *);
	double		GetSOI(OBJHANDLE obj);
	OBJHANDLE * GetSystemList(OBJHANDLE x);
	int			GetSystemCount(OBJHANDLE x);
	bool		IsGbody(OBJHANDLE x);

	OBJHANDLE	StarHandle;

private:

	OBJHANDLE	FindGravityReference(OBJHANDLE);
	void		CreateDatabase();

	struct ReferenceClass_info {	
		OBJHANDLE	handle;
		OBJHANDLE	grf_handle;
		double		soi;
		double		dist;  // Distance where this ReferenceClass is active
		OBJHANDLE   system[256];
		int         sys_index;
	};

	int total_count;
	struct ReferenceClass_info *References;
	
};

extern class ReferenceClass *Refer;

#endif