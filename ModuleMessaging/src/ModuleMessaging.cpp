// ==============================================================
//                ORBITER AUX LIBRARY: ModuleMessaging
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Allows Orbiter modules to communicate with each other,
// using predefined module and variable names.
//
// Copyright (C) 2014 Szymon "Enjo" Ender
//
//                         All rights reserved
//
// ModuleMessaging is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ModuleMessaging is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ModuleMessaging.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#include <OrbiterSdk.h>
#include "ModuleMessaging.hpp"
#include "Storage.hpp"

using namespace EnjoLib;

ModuleMessaging::ModuleMessaging()
{}

ModuleMessaging::~ModuleMessaging()
{}

Result<bool> ModuleMessaging::GetBool(const char * moduleName, const char * varName)
{
    return Storage::GetBool(moduleName, varName);
}
Result<int> ModuleMessaging::GetInt(const char * moduleName, const char * varName)
{
    return Storage::GetInt(moduleName, varName);
}
Result<double> ModuleMessaging::GetDouble(const char * moduleName, const char * varName)
{
    return Storage::GetDouble(moduleName, varName);
}
Result<VECTOR3> ModuleMessaging::GetVECTOR3(const char * moduleName, const char * varName)
{
    return Storage::GetVECTOR3(moduleName, varName);
}
Result<MATRIX3> ModuleMessaging::GetMATRIX3(const char * moduleName, const char * varName)
{
    return Storage::GetMATRIX3(moduleName, varName);
}
Result<MATRIX4> ModuleMessaging::GetMATRIX4(const char * moduleName, const char * varName)
{
    return Storage::GetMATRIX4(moduleName, varName);
}
