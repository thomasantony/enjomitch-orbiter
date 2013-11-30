// ==============================================================
//          ORBITER AUX LIBRARY: Multiple Vessels Support
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2008      Mohd "Computerex" Ali       - original concept
// Copyright (C) 2011      Szymon "Enjo" Ender         - object oriented design,
//														 simplifications, fixes
//                         All rights reserved
//
// MFDData.cpp - Base structure storing vessel pointer. To be derived.
//
// Multiple Vessels Support is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Multiple Vessels Support is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Multiple Vessels Support. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#include <orbitersdk.h>
#include "MFDData.hpp"
using namespace EnjoLib::MFDGoodies;

MFDData::MFDData (VESSEL * vessel)
{
    this->m_vessel = vessel;
    if ( m_vessel != NULL && oapiIsVessel( m_vessel->GetHandle() ) )
    {
        m_isValid = true;
    }
    else
    {
        m_isValid = false;
    }
}

MFDData::~MFDData()
{

}

VESSEL * MFDData::GetVessel() const
{
    return m_vessel;
}

void MFDData::Invalidate()
{
    m_isValid = false;
}

bool MFDData::IsValid() const
{
    return m_isValid;
}
