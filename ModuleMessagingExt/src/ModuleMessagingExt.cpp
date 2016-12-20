// ==============================================================
//                ORBITER AUX LIBRARY: ModuleMessagingExt
//             http://sf.net/projects/enjomitchsorbit
//
// Allows Orbiter modules to communicate with each other,
// using predefined module and variable names.
//
// Copyright  (C) 2014-2016 Szymon "Enjo" Ender and Andrew "ADSWNJ" Stokes
//
//                         All rights reserved
//
// ModuleMessagingExt is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// ModuleMessagingExt is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with ModuleMessagingExt. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#include <OrbiterSdk.h>
#include "ModuleMessagingExt.hpp"
#include "ModuleMessagingExtStor.hpp"
#include <string>


using namespace EnjoLib;

ModuleMessagingExt::ModuleMessagingExt()
{}

ModuleMessagingExt::~ModuleMessagingExt()
{}

bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, bool* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, int* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, double* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, VECTOR3* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, MATRIX3* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, MATRIX4* value, const VESSEL* myVessel, const int iVer)
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGetBasePtr(const char* moduleName, const char* varName, const int structVer, const unsigned int structSize,
									  	  const ModuleMessagingExtBase** structBasePtr, const VESSEL* myVessel, const int iVer)
{
	if (iVer != 1) return false;
	if (!ModuleMessagingExtStor::Get(moduleName, varName, structBasePtr, myVessel)) return false;
	if (!(*structBasePtr)->IsCorrectVersion(structVer)) return false;
	if (!(*structBasePtr)->IsCorrectSize(structSize)) return false;
	return true;
}

bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, bool* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, int* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, double* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, VECTOR3* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, MATRIX3* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGet(const char* moduleName, const char* varName, MATRIX4* value, const VESSEL* myVessel, const int iVer) const
{
	return (iVer == 1) ? ModuleMessagingExtStor::Get(moduleName, varName, value, myVessel) : false;
}
bool ModuleMessagingExt::ModMsgGetBasePtr(const char* moduleName, const char* varName, const int structVer, const unsigned int structSize,
									  	  const ModuleMessagingExtBase** structBasePtr, const VESSEL* myVessel, const int iVer)  const
{
	if (iVer != 1) return false;
	if (!ModuleMessagingExtStor::Get(moduleName, varName, structBasePtr, myVessel)) return false;
	if (!(*structBasePtr)->IsCorrectVersion(structVer)) return false;
	if (!(*structBasePtr)->IsCorrectSize(structSize)) return false;
	return true;
}

