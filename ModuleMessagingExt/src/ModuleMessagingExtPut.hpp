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

#ifndef ModuleMessagingExtPut_H
#define ModuleMessagingExtPut_H

#include <OrbiterSdk.h>
#include "ModuleMessagingExtBase.hpp"

namespace EnjoLib
{
/*
	Purpose:

	Makes your data available to other MFD's using the common ModuleMessagingExt library. Data can be
	passed by value or by reference. For pass by reference, the ModuleMessagingExt library implements
	several further checks to ensure type and version compatibility between you and your receiver,
	and that the receiver cannot modify your data.

	Developer Instructions:

	1. #include "EnjoLib/ModuleMessagingExtPut.hpp"

	2. Inherit your putting class from EnjoLib::ModuleMessagingExtPut

	3. Implement ModuleMessagingGetModuleName ... to return a literal string identifying your MFD. E.g.

		const char* ModuleMessagingGetModuleName() const { return "XYZ"; }  // Replace XYZ with your name

	4. Use ModuleMessagingExtPut(yourVarName, yourVarValue, {anotherVessel}) for any simple data type:
	   bool, int, double, VECTOR3, MATRIX3, MATRIX4 (with an optional VESSEL* to another vessel if you
	   are setting for anything but your current vessel).

	5. If you want to do pass by reference (e.g. for data structures that you update all the time, where
	   Put by value would be a huge overhead), then:

		5.1 Create a struct and inherit from ModuleMessagingExtBase, using this template:


			#pragma pack(push)
			#pragma pack(8)
			struct XYZ : public EnjoLib::ModuleMessagingExtBase {
				XYZ():EnjoLib::ModuleMessagingExtBase(17,sizeof(XYZ)) {};
				...
				... your data structures (don't use STL's like std::string
				... or std::map, etc as these tend not to be byte-compatible
				... across compiler versions)
				...
			};
			#pragma pack(pop)


		5.2 The #pragma pack directives attempt to keep the structure aligned across compiler versions.
		    #pragma pack(push) and #pragma pack(pop) just save and restore default settings for the rest
			of your code. The #pragma pack(8) makes each struct element 8-byte aligned. Choose 16 or 32
			if you want ... but keep it consistent once you set it. Your structure MUST inherit
			ModuleMessagingExtBase, which MUST be initialized with a version number (17 above), and the
			size of the structure. Make sure to increment the version number every time your structure
			definition changes, to prevent the receiving client from using an out-of-date structure
			defintion to point to you. Please also make sure that your structure has a fixed size and does
			not use any standard template libraries (e.g. no std::string or std::map). Why? Because you
			will find that the byte-layouts of these things vary across compiler versions, so a client
			with a future version of VC++ will very likely break badly trying to map on top of your legacy
			std::vector or std::string type, etc!

		5.2 Instantiation of your struct... make sure that any instances of this struct that you create are
		    in memory that will not be destroyed on the exit from a function or when the MFD class is destroyed
			on e.g. a switch to virtual cockpit view with F8. I.e. put it in your persistent vessel class,
			or instantiate it with new, so it's in a safe memory location.

		5.3 You want to ensure that any old clients do not accidentally access your struct with the old
		    definition. You fix this by incrementing the version count on the ModuleMessaging Validator any
			time you chnage your struct. The library will then ensure the client will not connect to you.

		5.4 In your code, call ModMsgPutByRef(varName, structVer, structRef, {anotherVessel}), where varName
			is your structure's lookupname (e.g. "CurrentBurn"), structVer is the current version of
			your structure (e.g. 17), structRef is your actual structure (e.g. curBurn). Optionally add a
			VESSEL* parameter, if you are referring to a vessel that's not your own (e.g. a target vessel).

		5.5 The ModMsgPutByRef will do the following safety checks: (1) check that your structure
		    is inheriting ModuleMessagingExtBase correctly,	(2) check that the base class version
			matches your structVer parameter (i.e. check at instantiation and at call), (3) check that the
			size of your structure matches the size at instantiation time. If good, you will get status = true
			back, and the structure will become accessible via this library.

	6. To Link your code, make sure your Linker, Input, AdditionalDependencies includes ModuleMessagingExt.lib.
	   Have a look at http://orbiter-forum.com/showthread.php?t=34971 for advice on setting up Property Pages
	   for Orbiter development (highly recommended, and it makes things much easier). If you do this, then
	   you can edit the Linker Input Additional Directories in the orbiter_vs2005 proprty page and this will
	   let you resolve any ModuleMessagingExt for this and future projects.

*/
	class __declspec(dllexport) ModuleMessagingExtPut
	{
	public:
		ModuleMessagingExtPut();
		virtual ~ModuleMessagingExtPut();

		/// Any user of this library *must* implement the following functions
		virtual const char* ModuleMessagingGetModuleName() const = 0;  // Return your MFD name as a const string .. e.g. return "MyMFD";

		// Library functions provided for you (no need to redefine)
		bool ModMsgPut(const char* varName, const bool var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgPut(const char* varName, const int var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgPut(const char* varName, const double var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgPut(const char* varName, const VECTOR3& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgPut(const char* varName, const MATRIX3& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgPut(const char* varName, const MATRIX4& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;

		bool ModMsgDelete(const char* varName, const bool var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgDelete(const char* varName, const int var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgDelete(const char* varName, const double var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgDelete(const char* varName, const VECTOR3& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgDelete(const char* varName, const MATRIX3& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
		bool ModMsgDelete(const char* varName, const MATRIX4& var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;

		// Safe put-by-reference function (for anything inherited from ModuleMessagingExtBase)
		template<class T>
		bool ModMsgPutByRef(const char* varName, const int structVer, T& structRef,
							const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const {
			if (iVer!=1) return false;
			const ModuleMessagingExtBase *structBasePtr = &structRef;
			return PutBasePtr(varName, structVer, sizeof(T), structBasePtr, myVessel, iVer);
		}

		template<class T>
		bool ModMsgDeleteByRef(const char* varName, const int structVer, T& structRef,
							const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const {
			if (iVer!=1) return false;
			const ModuleMessagingExtBase *structBasePtr = &structRef;
			return DeleteBasePtr(varName, structVer, sizeof(T), structBasePtr, myVessel, iVer);
		}

		// Internal implementation, but need to be public for the template to work
		bool PutBasePtr(const char* varName, const int structVer, const unsigned int structSize,
						const ModuleMessagingExtBase* var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
        bool DeleteBasePtr(const char* varName, const int structVer, const unsigned int structSize,
						const ModuleMessagingExtBase* var, const VESSEL* myVessel = oapiGetFocusInterface(), const int iVer = 1) const;
        bool IsCorrectPtr(const int structVer, const unsigned int structSize, const ModuleMessagingExtBase* structBasePtr, const int iVer) const;
	protected:
	private:
	};
}

#endif // ModuleMessagingExtPut_H
