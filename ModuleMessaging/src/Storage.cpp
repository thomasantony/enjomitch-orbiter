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

#include "Storage.hpp"
#include "IMessagingSender.hpp"

using namespace EnjoLib;
using namespace std;
map<string, bool> Storage::m_bools;
map<string, int> Storage::m_ints;
map<string, double> Storage::m_doubles;
map<string, VECTOR3> Storage::m_vectors;
map<string, MATRIX3> Storage::m_matrices3;
map<string, MATRIX4> Storage::m_matrices4;

Storage::Storage()
{}

Storage::~Storage()
{}

std::string Storage::MakeID(const IMessagingSender & sender, const char * varName)
{
    return MakeID(sender.GetModuleName(), varName);
}

std::string Storage::MakeID(const char * moduleName, const char * varName)
{
    return std::string() + moduleName + "-" + varName;
}

void Storage::StoreBool(const IMessagingSender & sender, const char * varName, bool var)
{
    m_bools[MakeID(sender, varName)] = var;
}
void Storage::StoreInt(const IMessagingSender & sender, const char * varName, int var)
{
    m_ints[MakeID(sender, varName)] = var;
}
void Storage::StoreDouble(const IMessagingSender & sender, const char * varName, double var)
{
    m_doubles[MakeID(sender, varName)] = var;
}
void Storage::StoreVECTOR3(const IMessagingSender & sender, const char * varName, const VECTOR3 & var)
{
    m_vectors[MakeID(sender, varName)] = var;
}
void Storage::StoreMATRIX3(const IMessagingSender & sender, const char * varName, const MATRIX3 & var)
{
    m_matrices3[MakeID(sender, varName)] = var;
}
void Storage::StoreMATRIX4(const IMessagingSender & sender, const char * varName, const MATRIX4 & var)
{
    m_matrices4[MakeID(sender, varName)] = var;
}

Result<bool> Storage::GetBool(const char * moduleName, const char * varName)
{
    return SearchMap<bool>(moduleName, varName, m_bools, false);
}
Result<int> Storage::GetInt(const char * moduleName, const char * varName)
{
    return SearchMap<int>(moduleName, varName, m_ints, 0);
}
Result<double> Storage::GetDouble(const char * moduleName, const char * varName)
{
    return SearchMap<double>(moduleName, varName, m_doubles, 0);
}
Result<VECTOR3> Storage::GetVECTOR3(const char * moduleName, const char * varName)
{
    return SearchMap<VECTOR3>(moduleName, varName, m_vectors, _V(0,0,0));
}
Result<MATRIX3> Storage::GetMATRIX3(const char * moduleName, const char * varName)
{
    return SearchMap<MATRIX3>(moduleName, varName, m_matrices3, _M(0,0,0,0,0,0,0,0,0));
}
Result<MATRIX4> Storage::GetMATRIX4(const char * moduleName, const char * varName)
{
    MATRIX4 defaultVal = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    return SearchMap<MATRIX4>(moduleName, varName, m_matrices4, defaultVal);
}
