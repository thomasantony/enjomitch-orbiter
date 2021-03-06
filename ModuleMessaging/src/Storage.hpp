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

#ifndef STORAGE_H
#define STORAGE_H

#include <OrbiterSDK.h>
#include <map>
#include <string>
#include "Result.hpp"

namespace EnjoLib
{
class IMessagingSender;
class Storage
{
    public:
        Storage();
        virtual ~Storage();

        static void StoreBool(const IMessagingSender & sender, const char * varName, bool var);
        static void StoreInt(const IMessagingSender & sender, const char * varName, int var);
        static void StoreDouble(const IMessagingSender & sender, const char * varName, double var);
        static void StoreVECTOR3(const IMessagingSender & sender, const char * varName, const VECTOR3 & var);
        static void StoreMATRIX3(const IMessagingSender & sender, const char * varName, const MATRIX3 & var);
        static void StoreMATRIX4(const IMessagingSender & sender, const char * varName, const MATRIX4 & var);
        static void StoreString(const IMessagingSender & sender, const char * varName, const char * var);

        static Result<bool>     GetBool(    const char * moduleName, const char * varName);
        static Result<int>      GetInt(     const char * moduleName, const char * varName);
        static Result<double>   GetDouble(  const char * moduleName, const char * varName);
        static Result<VECTOR3>  GetVECTOR3( const char * moduleName, const char * varName);
        static Result<MATRIX3>  GetMATRIX3( const char * moduleName, const char * varName);
        static Result<MATRIX4>  GetMATRIX4( const char * moduleName, const char * varName);

    protected:
    private:
        static std::string MakeID(const IMessagingSender & sender, const char * varName);
        static std::string MakeID(const char * moduleName, const char * varName);
        template<class T> static Result<T> SearchMap(const char * moduleName, const char * varName, const std::map<std::string, T> & mapToSearch, const T & defaultValue);

        static std::map<std::string, bool> m_bools;
        static std::map<std::string, int> m_ints;
        static std::map<std::string, double> m_doubles;
        static std::map<std::string, VECTOR3> m_vectors;
        static std::map<std::string, MATRIX3> m_matrices3;
        static std::map<std::string, MATRIX4> m_matrices4;
};

    template<class T>
    Result<T> Storage::SearchMap(const char * moduleName, const char * varName, const std::map<std::string, T> & mapToSearch, const T & defaultValue)
    {
        const string & id = MakeID(moduleName, varName);
        map<string, T>::const_iterator it = mapToSearch.find(id);
        if(it != mapToSearch.end())
            return Result<T>(it->second, true);
        else
            return Result<T>(defaultValue, false);
    }
}

#endif // STORAGE_H
