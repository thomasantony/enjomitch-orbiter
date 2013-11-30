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
// PluginMultipleVessels.hpp - Base plugin functionalities. To be derived.
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

#ifndef PLUGINMULTIPLEVESSELS_HPP_INCLUDED
#define PLUGINMULTIPLEVESSELS_HPP_INCLUDED

#include <vector>
#include "MFDData.hpp"
#include "SimulationTimeData.hpp"
#include <orbitersdk.h>

namespace EnjoLib
{
namespace MFDGoodies
{
/// Main plugin class.
/**
Abstract class with encapsulated multiple vessels support.
 \warning No public methods of this class should be overriden. You should rather
 derive from this class and implement the pure virtual methods
*/
class PluginMultipleVessels
#ifndef ORB2006
	: public oapi::Module
#endif
{
public:
    /// Default ctor
    /**
    \param hDLL DLL handle, passed by Orbiter core at InitModule()
    */
    PluginMultipleVessels( HINSTANCE hDLL );

    /// Dtor
    /**
    Deletes all MFDData objects
    \author Szymon "Enjo" Ender
    */
    virtual ~PluginMultipleVessels();

    /// Associates a given vessel with MFDData.
    /**
    Associates a given vessel with MFDData using an internal MFDData register
    by creating a new MFDData object if this is a new vessel, or finds an existing
    MFDData object from internal struct, if the object has been already created.
    Should be called from initialisation list of an MFD.
    \param vessel passed to MFD
    \return a reference to the MFDData object
    \author Mohd "Computerex" Ali (concept, comments)
    \author Szymon "Enjo" Ender (simplifications and other modifications)
    */
    MFDData * AssociateMFDData( VESSEL * vessel );

    /// Updates the plugin.
    /**
    Updates the PluginMultipleVessels, including all its MFDData members.
    Should be called on every simulation step. After necessary checks,
    calls UpdateClient() for each valid MFDData object.
    \author Szymon "Enjo" Ender
    */
    void UpdatePreStep( const SimulationTimeData & timeData );

	void UpdatePostStep( const SimulationTimeData & timeData );

	void clbkSimulationStart(
                          #ifndef ORB2006
                          RenderMode  mode
                          #endif
                          );
	void clbkSimulationEnd();

    /// Initialises the plugin
    /**
    Initialises the PluginMultipleVessels. The internals of this method are executed only once.
    Should be called on every simulation step, because it's the first opportunity
    to gain access to Orbiter's configuration files. Calls InitClient() only once.
    \author Szymon "Enjo" Ender
    */
    void Init();
    void Cleanup();

    /// Deletes vessel
    /**
    Handles vessel deletion in a secure manner, by simply invalidating the MFDData
    associated with the vessel being deleted. This way Orbiter won't crash on random occasions.
    \param hVessel handle to vessel, passed by Orbiter core.
    \author Szymon "Enjo" Ender
    */
    void DeleteVessel( OBJHANDLE  hVessel );

    /// Time step notification before state update.
    /** Refer to OrbiterSDK documentation */
    void clbkPreStep (double simt, double simdt, double mjd);

	/// Time step notification after state update.
    /** Refer to OrbiterSDK documentation */
    void clbkPostStep (double simt, double simdt, double mjd);

    /// Vessel destruction notification.
    /** Refer to OrbiterSDK documentation */
    void clbkDeleteVessel (OBJHANDLE hVessel);

protected:
    /// Updates client (derived) PluginMultipleVessels before simulation state update
    /**
    Updates client (derived) PluginMultipleVessels for each valid MFDData object.
    The method should perform every necessary operations on MFDData object
    for a proper module operation when the MFD itself is turned off.
        \param data - one of the created MFDData object
        \param timeData - simulation timess
    */
    virtual void UpdateClientPreStep( MFDData * data, const SimulationTimeData & timeData ) = 0;

    /// Updates client (derived) PluginMultipleVessels after simulation state update
    /**
    \see UpdateClientPreStep()
    */
	virtual void UpdateClientPostStep( MFDData * data, const SimulationTimeData & timeData ) = 0;

    /// Initialises client (derived) PluginMultipleVessels
    /**
    Initialises client (derived) PluginMultipleVessels. This method should gain access
    to Orbiter's configuration files and store the obtained data in member variables,
    so that they can be passed to MFD, if they are needed.
    This method is called only once in simulation.
    */
    virtual void InitClient() = 0;
    virtual void CleanupClient() = 0;

    /// Creates and returns a new MFDData * compatible pointer
    /** This method should construct and return a new MFDData * compatible pointer,
    ie. a pointer to an object that is derived from MFDData and used in your PluginMultipleVessels
    and MFD implementation
    Called from AssociateMFDData()
    */
    virtual MFDData * ConstructNewMFDData( VESSEL * vessel ) = 0;

private:
    void CleanVectors();

    typedef std::vector<MFDData*> VMFDData;

    VMFDData m_vMFDData; ///< A vector that holds pointers to MFDData objects
    VMFDData m_vMFDDataInvalid; ///< A vector that holds pointers to MFDData objects
};
}
}
#endif // PLUGINMULTIPLEVESSELS_HPP_INCLUDED
