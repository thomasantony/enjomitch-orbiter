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

#include "IMessagingSender.hpp"
#include "Storage.hpp"

using namespace EnjoLib;

IMessagingSender::IMessagingSender()
{}

IMessagingSender::~IMessagingSender()
{}

void IMessagingSender::SendBool(const char * varName, bool var) const
{
    Storage::StoreBool(*this, varName, var);
}
void IMessagingSender::SendInt(const char * varName, int var) const
{
    Storage::StoreInt(*this, varName, var);
}
void IMessagingSender::SendDouble(const char * varName, double var) const
{
    Storage::StoreDouble(*this, varName, var);
}
void IMessagingSender::SendVECTOR3(const char * varName, const VECTOR3 & var) const
{
    Storage::StoreVECTOR3(*this, varName, var);
}
void IMessagingSender::SendMATRIX3(const char * varName, const MATRIX3 & var) const
{
    Storage::StoreMATRIX3(*this, varName, var);
}
void IMessagingSender::SendMATRIX4(const char * varName, const MATRIX4 & var) const
{
    Storage::StoreMATRIX4(*this, varName, var);
}
