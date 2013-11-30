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
// MFDData.hpp - Base structure storing vessel pointer. To be derived.
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

#ifndef __MFDDATA_H
#define __MFDDATA_H

class VESSEL;
namespace EnjoLib
{
namespace MFDGoodies
{
/// Class used for storing a given vessel's data
/**
Stores a given vessel's data, that can be retrieved later,
or updated on every MFD refresh, or every simulation step if needed.
*/
class MFDData
{
public:
    /// Default constructor
    /** Constructs MFDData object for a given vessel.
    Should only be called from within PluginMultipleVessels::AssociateMFDData().
    \param vessel passed by PluginMultipleVessels::AssociateMFDData()
    */
    MFDData (VESSEL * vessel);

    /// Destructor
    virtual ~MFDData();

    /// Updates the vessel's state
    /** Updates the vessel's state as in for example - position, velocity, reference planet, etc.
    Should be called from MyMFDMultipleVessels::Update() and on every simulation step from PluginMultipleVessels::UpdateClient() if necessary.
    This method should update all the required data of the vessel to be able operate properly without
    an opened MFD.
    */
    virtual void Update() = 0;

    /// Returns associated vessel
    /** Returns the vessel associated with this object */
    VESSEL * GetVessel() const;

    /// Sets this object as invalid
    /** Sets this object as invalid in the event of deleting the associated vessel.
    Called automatically from PluginMultipleVessels::DeleteVessel().
    */
    void Invalidate();

    /// Checks if this object is still valid
    /**	Checks if this object is still, because it may have been invalidated
    upon vessel deletion. In such case, no operations should be performed on this object.
    */
    bool IsValid() const;

protected:


private:
	VESSEL * m_vessel; ///< Vessel associated with this object
    bool m_isValid;
};
}
}

#endif
