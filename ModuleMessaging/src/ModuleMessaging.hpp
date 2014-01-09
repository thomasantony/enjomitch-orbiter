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

#ifndef MESSAGINGMODULE_H
#define MESSAGINGMODULE_H

#include <OrbiterSDK.h>

#include <string>
#include "Result.hpp"

namespace EnjoLib
{
/// Lets obtaining variables from other modules, that were exposed by them
/**
    Provided that you know the name of a module that exposes a variable, and its type,
    you may use the public methods of this class to receive them.
    Be sure to check for Result<T>::isSuccess, to make sure that the variable was even exposed.
    If it weren't, then the variable's default value is returned.
    For example:
    \code
    #include <EnjoLib/ModuleMessaging.hpp>
    #include <EnjoLib/Result.hpp>

    void BurnTimeMFD::HandlerGetFromTransX()
    {
        using namespace EnjoLib;
        Result<double> dvRes = ModuleMessaging().GetDouble("TransX", "dv");
        Result<double> IManualRes = ModuleMessaging().GetDouble("TransX", "TBurn");
        if (dvRes.isSuccess && IManualRes.isSuccess) // Are both values exposed?
        {
            IsArmed=IsEngaged=false;
            mode = BURNMODE_MAN;

            dv = dvRes.value;
            if (IManualRes.value > 0) // We're not interested in negative times
            {
                IManual = IManualRes.value;
                HandlerAutoBurn();
            }
        }
    }
    \endcode
*/
class __declspec(dllexport) ModuleMessaging
{
    public:
        ModuleMessaging();
        virtual ~ModuleMessaging();

        /// Returns a bool
        Result<bool>    GetBool(    const char * moduleName, const char * varName);
        /// Returns an int
        Result<int>     GetInt(     const char * moduleName, const char * varName);
        /// Returns a double
        Result<double>  GetDouble(  const char * moduleName, const char * varName);
        /// Returns a VECTOR3
        Result<VECTOR3> GetVECTOR3( const char * moduleName, const char * varName);
        /// Returns a MATRIX3
        Result<MATRIX3> GetMATRIX3( const char * moduleName, const char * varName);
        /// Returns a MATRIX4
        Result<MATRIX4> GetMATRIX4( const char * moduleName, const char * varName);

    protected:
    private:
};
}

#endif // MESSAGINGMODULE_H
