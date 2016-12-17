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

#ifndef ModuleMessagingExtStor_H
#define ModuleMessagingExtStor_H

#include <OrbiterSDK.h>
#include <map>
#include <string>
#include "ModuleMessagingExtBase.hpp"

namespace EnjoLib
{
/*
	Purpose:

	Internal implementation of the Module Messaging Library.

	Developer Instructions:

	None. Do not call this directly or try to use this directly.
*/

	class ModuleMessagingExtPut;
	class ModuleMessagingExtStor
	{
	public:
		ModuleMessagingExtStor();
		virtual ~ModuleMessagingExtStor();

		static void Put(const ModuleMessagingExtPut& sender, const char* varName, bool var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, int var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, double var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, const VECTOR3& var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX3& var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX4& var, const VESSEL* myVessel);
		static void Put(const ModuleMessagingExtPut& sender, const char* varName, const ModuleMessagingExtBase* var, const VESSEL* myVessel);

		static bool Get(const char* moduleName, const char* varName, bool* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, int* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, double* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, VECTOR3* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, MATRIX3* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, MATRIX4* var, const VESSEL* myVessel);
		static bool Get(const char* moduleName, const char* varName, const ModuleMessagingExtBase** var, const VESSEL* myVessel);

		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, bool var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, int var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, double var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, const VECTOR3& var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX3& var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, const MATRIX4& var, const VESSEL* myVessel);
		static bool Delete(const ModuleMessagingExtPut& sender, const char* varName, const ModuleMessagingExtBase* var, const VESSEL* myVessel);

	protected:
	private:
		static std::string MakeID(const ModuleMessagingExtPut& sender, const VESSEL* myVessel, const char* varName);
		static std::string MakeID(const char* moduleName, const VESSEL* myVessel, const char* varName);
		template<class T> static bool SearchMap(const char* moduleName, const VESSEL*myVessel,
												const char* varName, const std::map<std::string, T>& mapToSearch, T* returnValue);
        template<class T> static bool SearchMapDelete(  const char* moduleName, const VESSEL*myVessel,
                                                        const char* varName, std::map<std::string, T>& mapToSearch);

		static const char m_token;
		static std::map<std::string, bool> m_bools;
		static std::map<std::string, int> m_ints;
		static std::map<std::string, double> m_doubles;
		static std::map<std::string, VECTOR3> m_vectors;
		static std::map<std::string, MATRIX3> m_matrices3;
		static std::map<std::string, MATRIX4> m_matrices4;
		static std::map<std::string, const ModuleMessagingExtBase*> m_basepointers;
	};

}

#endif // ModuleMessagingExtStor_H
