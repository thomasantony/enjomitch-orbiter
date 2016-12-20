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

#pragma once
#ifndef ModuleMessagingExt_H
#define ModuleMessagingExt_H

#include <string>
#include <OrbiterSDK.h>
#include "ModuleMessagingExtBase.hpp"

namespace EnjoLib
{
/*
	Purpose:

	Gets data from other MFD's using the common ModuleMessagingExt library. Data can be passed by value
	or by reference. For pass by reference, the ModuleMessagingExt library implements several further
	checks to ensure safety and to deal with potential version or definition mismatches.

	Developer Instructions:

	1. #include "EnjoLib/ModuleMessagingExt.hpp" where you need to get data from ModuleMessagingExt.

	2. For get by value ... call EnjoLib::ModuleMessagingExt().Get(MFDname, varName, &value, {vessel}),
		where:	MFDname is a string literal for the sending MFD or module (e.g. "BaseSyncMFD"),
				varName is a string literal for the variable you are looking for
				&value is the address of where you want the value to be received, and is one of the following
					types: bool, int, double, VECTOR3, MATRIX3, and MATRIX4.
				vessel is VESSEL* pointer to the vessel you want to get data from (defaults to your vessel).

		The return is a bool indicating success or failure. If successful, the value will have the new data.

	3. For get by reference (meaning you are addressing memory directly in the other DLL), the author
		of the module PUTting the data will give you a header file with a data structure like this:

			#pragma pack(push)
			#pragma pack(8)
			struct XYZ : public EnjoLib::ModuleMessagingExtBase {
				XYZ():EnjoLib::ModuleMessagingExtBase(13,sizeof(XYZ)) {};  // version 13
				...
				... your data structures (don't use STL's like std::string
				... or std::map, etc as these tend not to be byte-compatible
				... across compiler versions)
				...
			};
			#pragma pack(pop)

		(Obviously with their structure name instead of XYZ). Include this header into your code to give you
		the latest definition of the structure and version you are accessing. Make a note of the version number
		on the ModuleMessagingExtBase constructor (i.e. 13 in this case), as you need it for your ModMsgGetByRef()
		call.

		In your code, define a pointer to a constant XYZ structure (replace XYZ, of course), such as:

			constant struct xyz *remoteData;

		This tells the compiler that you guarantee not to write data via the pointer. (If you wanted to do this,
		and the other module author wanted to cooperate, then have both sides ModMsgGetByRef the other side
		and write local / read remote.)

		Then, call EnjoLib::ModuleMessagingExt().ModMsgGetByRef(MFDname, structName, structVer, &structPtr,
		{vessel}), where the structName and structVer are given to you by the author of the PUTting module,
		and the other parameters are the same as the ModMsgGet(). In addition to looking up the data and returning
		it if found, the library will do additional checks to ensure the structure has a valid
		inheritance from the ModuleMessagigngBaseStruct, the structure version number is identical, and
		the structure size is also correct. (The PUT side should increment the version any time the structure
		is changed, which will prevent you accessing a wrong verion of the structure, and the sizeof()
		check is there as a further failsafe). So if you get a "true" back from ModMsgGetByRef(), you can be
		reasonably sure that everything is safe and you will not crash the Orbiter when you reference the data.

	4. To link your code, make sure your Linker, Input, AdditionalDependencies includes ModuleMessagingExt.lib.
		Have a look at http://orbiter-forum.com/showthread.php?t=34971 for advice on setting up Property Pages
		for Orbiter development (highly recommended, and it makes things much easier). If you do this, then
		you can edit the Linker Input Additional Directories in the orbiter_vs2005 proprty page and this will
		let you resolve any ModuleMessagigngExt for this and future projects.
*/
	class __declspec(dllexport) ModuleMessagingExt
	{
	public:
		ModuleMessagingExt();
		virtual ~ModuleMessagingExt();

		bool ModMsgGet( const char* moduleName, const char* varName, bool* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);
		bool ModMsgGet( const char* moduleName, const char* varName, int* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);
		bool ModMsgGet( const char* moduleName, const char* varName, double* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);
		bool ModMsgGet( const char* moduleName, const char* varName, VECTOR3* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);
		bool ModMsgGet( const char* moduleName, const char* varName, MATRIX3* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);
		bool ModMsgGet( const char* moduleName, const char* varName, MATRIX4* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);

        template<class T>
		bool ModMsgGetByRef(const char* moduleName, const char* structName, int structVer,
							const T** structPtr, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1)
		{
			if (iVer!=1)
                return false;
			const ModuleMessagingExtBase *structBasePtr;
			if (false ==ModMsgGetBasePtr(moduleName, structName, structVer, sizeof(T), &structBasePtr, myVessel, iVer))
                return false;
			*structPtr = dynamic_cast<const T*>(structBasePtr);
            if (*structPtr == NULL) // Is type compliant?
                return false;
			return true; // Passed all safety checks
		}

		bool ModMsgGet( const char* moduleName, const char* varName, bool* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgGet( const char* moduleName, const char* varName, int* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgGet( const char* moduleName, const char* varName, double* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgGet( const char* moduleName, const char* varName, VECTOR3* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgGet( const char* moduleName, const char* varName, MATRIX3* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgGet( const char* moduleName, const char* varName, MATRIX4* value,
						const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;

		template<class T>
		bool ModMsgGetByRef(const char* moduleName, const char* structName, int structVer,
							const T** structPtr, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1)  const
		{
			if (iVer!=1)
                return false;
			const ModuleMessagingExtBase *structBasePtr;
			if (false ==ModMsgGetBasePtr(moduleName, structName, structVer, sizeof(T), &structBasePtr, myVessel, iVer))
                return false;
			*structPtr = dynamic_cast<const T*>(structBasePtr);
            if (*structPtr == NULL) // Is type compliant?
                return false;
			return true; // Passed all safety checks
		}
	protected:
	private:
	    bool ModMsgGetBasePtr(const char* moduleName, const char* varName, const int structVer,
							  const unsigned int structSize, const ModuleMessagingExtBase** value,
							  const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1);

		bool ModMsgGetBasePtr(const char* moduleName, const char* varName, const int structVer,
							  const unsigned int structSize, const ModuleMessagingExtBase** value,
							  const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
	};
}

#endif // ModuleMessagingExt_H
