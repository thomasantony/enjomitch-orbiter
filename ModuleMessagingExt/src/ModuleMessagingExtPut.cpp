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

#include "ModuleMessagingExtPut.hpp"
#include "ModuleMessagingExtStor.hpp"

using namespace EnjoLib;

ModuleMessagingExtPut::ModuleMessagingExtPut(){}
ModuleMessagingExtPut::~ModuleMessagingExtPut(){}

bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const bool var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}
bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const int var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}
bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const double var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}
bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const VECTOR3& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}
bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const MATRIX3& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}
bool ModuleMessagingExtPut::ModMsgPut(const char* varName, const MATRIX4& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    ModuleMessagingExtStor::Put(*this, varName, var, myVessel);
	return true;
}

bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const bool var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}
bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const int var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}
bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const double var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}
bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const VECTOR3& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}
bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const MATRIX3& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}
bool ModuleMessagingExtPut::ModMsgDelete(const char* varName, const MATRIX4& var, const VESSEL* myVessel, const int iVer) const
{
	if (iVer != 1) return false;
    return ModuleMessagingExtStor::Delete(*this, varName, var, myVessel);
}

bool ModuleMessagingExtPut::PutBasePtr(const char* varName, const int structVer, const unsigned int structSize,
									   const ModuleMessagingExtBase* structBasePtr, const VESSEL* myVessel, const int iVer) const
{
	if (! IsCorrectPtr(structVer, structSize, structBasePtr, iVer))
        return false;
    ModuleMessagingExtStor::Put(*this, varName, structBasePtr, myVessel);
	return true;
}

bool ModuleMessagingExtPut::DeleteBasePtr(const char* varName, const int structVer, const unsigned int structSize,
									   const ModuleMessagingExtBase* structBasePtr, const VESSEL* myVessel, const int iVer) const
{
    if (! IsCorrectPtr(structVer, structSize, structBasePtr, iVer))
        return false;
    return ModuleMessagingExtStor::Delete(*this, varName, structBasePtr, myVessel);
}

bool ModuleMessagingExtPut::IsCorrectPtr(const int structVer, const unsigned int structSize, const ModuleMessagingExtBase* structBasePtr, const int iVer) const
{
    if (iVer != 1)
		return false;
	if (structBasePtr == NULL)
		return false;
	if (!structBasePtr->IsCorrectVersion(structVer))
		return false;
	if (!structBasePtr->IsCorrectSize(structSize))
		return false;
    return true;
}
