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

#include "ModuleMessagingExtStor.hpp"
#include "ModuleMessagingExtPut.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

using namespace EnjoLib;
using namespace std;
map<string, bool> ModuleMessagingExtStor::m_bools;
map<string, int> ModuleMessagingExtStor::m_ints;
map<string, double> ModuleMessagingExtStor::m_doubles;
map<string, VECTOR3> ModuleMessagingExtStor::m_vectors;
map<string, MATRIX3> ModuleMessagingExtStor::m_matrices3;
map<string, MATRIX4> ModuleMessagingExtStor::m_matrices4;
map<std::string, const ModuleMessagingExtBase*> ModuleMessagingExtStor::m_basepointers;
const char ModuleMessagingExtStor::m_token = '`';
ModuleMessagingExtStor::ModuleMessagingExtStor()
{}

ModuleMessagingExtStor::~ModuleMessagingExtStor()
{}

template<class T>
static bool ModuleMessagingExtStor::SearchMap(const char* moduleName, const VESSEL* myVessel, const char* varName, const std::map<std::string, T>& mapToSearch, T* returnValue)
{
	const string & id = MakeID(moduleName, myVessel, varName);
	map<string, T>::const_iterator it = mapToSearch.find(id);
	if(it != mapToSearch.end()) {
		*returnValue = it->second;
		return true;
	} else {
		return false;
	}
}

template<class T>
static bool ModuleMessagingExtStor::SearchMapDelete(const char* moduleName, const VESSEL* myVessel, const char* varName, std::map<std::string, T>& mapToSearch)
{
	const string & id = MakeID(moduleName, myVessel, varName);
	map<string, T>::iterator it = mapToSearch.find(id);
	if(it != mapToSearch.end()) {
		mapToSearch.erase(it);
		return true;
	} else {
		return false;
	}
}

std::string ModuleMessagingExtStor::MakeID(const ModuleMessagingExtPut& sender, const VESSEL* myVessel, const char* varName)
{
    return MakeID(sender.ModuleMessagingGetModuleName(), myVessel, varName);
}

std::string ModuleMessagingExtStor::MakeID(const char* moduleName, const VESSEL* myVessel, const char* varName)
{
    string idName = string(myVessel->GetName()) + m_token + moduleName + m_token + varName;
    std::transform(idName.begin(), idName.end(), idName.begin(), ::tolower);
    return idName;
}


void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, bool var, const VESSEL* myVessel)
{
    m_bools[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, int var, const VESSEL* myVessel)
{
    m_ints[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, double var, const VESSEL* myVessel)
{
    m_doubles[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, const VECTOR3& var, const VESSEL* myVessel)
{
    m_vectors[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX3& var, const VESSEL* myVessel)
{
    m_matrices3[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX4& var, const VESSEL* myVessel)
{
    m_matrices4[MakeID(sender, myVessel, varName)] = var;
}
void ModuleMessagingExtStor::Put(const ModuleMessagingExtPut& sender, const char* varName, const ModuleMessagingExtBase* var, const VESSEL* myVessel)
{
    m_basepointers[MakeID(sender, myVessel, varName)] = var;
}

bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, bool value, const VESSEL* myVessel)
{
    return SearchMapDelete<bool>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_bools);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, int value, const VESSEL* myVessel)
{
    return SearchMapDelete<int>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_ints);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, double value, const VESSEL* myVessel)
{
    return SearchMapDelete<double>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_doubles);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, const VECTOR3 & value, const VESSEL* myVessel)
{
    return SearchMapDelete<VECTOR3>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_vectors);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX3 & value, const VESSEL* myVessel)
{
    return SearchMapDelete<MATRIX3>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_matrices3);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX4 & value, const VESSEL* myVessel)
{
    return SearchMapDelete<MATRIX4>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_matrices4);
}
bool ModuleMessagingExtStor::Delete(const ModuleMessagingExtPut& sender, const char* varName, const ModuleMessagingExtBase* value, const VESSEL* myVessel)
{
    return SearchMapDelete<const ModuleMessagingExtBase *>(sender.ModuleMessagingGetModuleName(), myVessel, varName, m_basepointers);
}


bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, bool* value, const VESSEL* myVessel)
{
    return SearchMap<bool>(moduleName, myVessel, varName, m_bools, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, int* value, const VESSEL* myVessel)
{
    return SearchMap<int>(moduleName, myVessel, varName, m_ints, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, double* value, const VESSEL* myVessel)
{
    return SearchMap<double>(moduleName, myVessel, varName, m_doubles, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, VECTOR3* value, const VESSEL* myVessel)
{
    return SearchMap<VECTOR3>(moduleName, myVessel, varName, m_vectors, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, MATRIX3* value, const VESSEL* myVessel)
{
    return SearchMap<MATRIX3>(moduleName, myVessel, varName, m_matrices3, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, MATRIX4* value, const VESSEL* myVessel)
{
    return SearchMap<MATRIX4>(moduleName, myVessel, varName, m_matrices4, value);
}
bool ModuleMessagingExtStor::Get(const char* moduleName, const char* varName, const ModuleMessagingExtBase** value, const VESSEL* myVessel)
{
    return SearchMap<const ModuleMessagingExtBase *>(moduleName, myVessel, varName, m_basepointers, value);
}
