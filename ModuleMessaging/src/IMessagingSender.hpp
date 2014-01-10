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
// ModuleMessaging is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// ModuleMessaging is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with ModuleMessaging. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#ifndef IMESSAGINGSENDER_H
#define IMESSAGINGSENDER_H

#include <OrbiterSdk.h>

namespace EnjoLib
{
/// Interface for a class that is supposed to expose its data to another modules
/**
    Make this a base class of one of your module's classes and implement the GetModuleName() method.
    For example:
    \code
    #include <Orbitersdk.h>
    #include <EnjoLib/IMessagingSender.hpp>

    class MySender : public EnjoLib::IMessagingSender
    {
        public:
        // EnjoLib::IMessagingSender methods:
        const char * GetModuleName() const;

        protected:
        private:
    };
    \endcode
    Then create an object of this class on stack and use its public methods to send messages.
    It's required that you name the variable.
    For example:
    \code
    {
        double dv = 1000;
        MySender().SendDouble("DeltaV", dv);
    }
    \endcode
    The receiving module may then search for variables
    with the returned module name and variable name.
*/
class __declspec(dllexport) IMessagingSender
{
    public:
        IMessagingSender();
        virtual ~IMessagingSender();
        /// Should return unique module's name.
        virtual const char * GetModuleName() const = 0;

        /// Sends a bool
        void SendBool(const char * varName, bool var) const;
        /// Sends an int
        void SendInt(const char * varName, int var) const;
        /// Sends a double
        void SendDouble(const char * varName, double var) const;
        /// Sends a VECTOR3
        void SendVECTOR3(const char * varName, const VECTOR3 & var) const;
        /// Sends a MATRIX3
        void SendMATRIX3(const char * varName, const MATRIX3 & var) const;
        /// Sends a MATRIX4
        void SendMATRIX4(const char * varName, const MATRIX4 & var) const;

    protected:

    private:
};
}

#endif // IMESSAGINGSENDER_H
