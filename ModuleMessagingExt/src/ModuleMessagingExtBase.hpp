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
#ifndef ModuleMessagingExtBase_H
#define ModuleMessagingExtBase_H

#include <string>
#include <OrbiterSDK.h>

namespace EnjoLib
{
/*
	Purpose:

	Used as the base class for all ModMsgPutByRef() and ModMsgGetByRef() calls.\
	
	Developer Instructions:

	Please see ModuleMessagingExtPut.hpp for details. 
*/

	class ModuleMessagingExtBase
	{
	public:
		ModuleMessagingExtBase(unsigned int sVer, unsigned int sSize) : _sVer(sVer), _sSize(sSize) {};
		virtual ~ModuleMessagingExtBase() {};

        bool IsCorrectVersion(unsigned int sVer) const;
        bool IsCorrectSize(unsigned int sSize) const;

	private:
		unsigned int _sVer;
		unsigned int _sSize;
	};

}

#endif // ModuleMessagingExtBase_H
