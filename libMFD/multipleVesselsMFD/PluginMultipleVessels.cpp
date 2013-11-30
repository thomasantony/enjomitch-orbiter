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
// PluginMultipleVessels.cpp - Base plugin functionalities. To be derived.
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

#include <OrbiterSDK.h>
#include "PluginMultipleVessels.hpp"
using namespace EnjoLib::MFDGoodies;

PluginMultipleVessels::PluginMultipleVessels( HINSTANCE hDLL )
#ifndef ORB2006
    : oapi::Module( hDLL )
#endif
{
}

PluginMultipleVessels::~PluginMultipleVessels()
{
    CleanVectors();
}

void PluginMultipleVessels::CleanVectors()
{
    for (VMFDData::const_iterator it = m_vMFDData.begin(); it != m_vMFDData.end(); ++it)
        delete *it;
    for (VMFDData::const_iterator it = m_vMFDDataInvalid.begin(); it != m_vMFDDataInvalid.end(); ++it)
        delete *it;
    m_vMFDData.clear();
    m_vMFDDataInvalid.clear();
}

void PluginMultipleVessels::clbkPreStep (double simt, double simdt, double mjd)
{
    UpdatePreStep( SimulationTimeData(simt, simdt, mjd) ); // This sould be called on each step
}

void PluginMultipleVessels::clbkPostStep (double simt, double simdt, double mjd)
{
    UpdatePostStep( SimulationTimeData(simt, simdt, mjd) ); // This sould be called on each step
}

void PluginMultipleVessels::clbkDeleteVessel (OBJHANDLE hVessel)
{
    // Signal the plugin that this vessel should be removed from it's structure
    DeleteVessel( hVessel );
}

void PluginMultipleVessels::clbkSimulationStart(
                                                #ifndef ORB2006
                                                RenderMode  mode
                                                #endif
                                                )
{
    Init();
}

void PluginMultipleVessels::clbkSimulationEnd()
{
    Cleanup();
}

void PluginMultipleVessels::Init()
{
    InitClient();
}

void PluginMultipleVessels::Cleanup()
{
    CleanupClient();
    CleanVectors();
}

void PluginMultipleVessels::UpdatePreStep( const SimulationTimeData & timeData )
{
    // Do necessary PluginMultipleVessels code first

    // and now call the overriden class' UpdateClient method for each MFDData *
    for (VMFDData::const_iterator it = m_vMFDData.begin(); it != m_vMFDData.end(); ++it)
        UpdateClientPreStep( *it, timeData ); // Call the derived update on this MFDData
}

void PluginMultipleVessels::UpdatePostStep( const SimulationTimeData & timeData )
{
    for (VMFDData::const_iterator it = m_vMFDData.begin(); it != m_vMFDData.end(); ++it)
        UpdateClientPostStep( *it, timeData ); // Call the derived update on this MFDData
}

MFDData * PluginMultipleVessels::AssociateMFDData(VESSEL * vessel)
{
    // Scan through the MFDData vector
    for (VMFDData::const_iterator it = m_vMFDData.begin(); it != m_vMFDData.end(); ++it)
    {
        // if the handle of the vessel this MFD is linked to is the same as the handle of the vessel
        // inside the MFDData object that is currently being scanned
        if (vessel == (*it)->GetVessel())
        {
            // Return a pointer to this MFDData (dereferenced iterator)
            return *it;
        }
    }
    // If no match is found, let's create one from scratch.
    // Create a new MFDData object for the given vessel, effectively linking them together
    MFDData * dat = ConstructNewMFDData(vessel);
    m_vMFDData.push_back(dat); // store this new MFDData object inside the MFDData structure.
    return dat; // Return a pointer to the new MFDData object.
}

void PluginMultipleVessels::DeleteVessel( OBJHANDLE  hVessel )
{
    for (VMFDData::iterator it = m_vMFDData.begin(); it != m_vMFDData.end(); ++it)
    {
        // Searching for the argument vessel in our structure
        if ( (*it)->GetVessel()->GetHandle() == hVessel )
        {
            // found it
            (*it)->Invalidate();  // Simply invalidate this MFDData object. A CTD-secure method
            m_vMFDDataInvalid.push_back(*it);
            m_vMFDData.erase(it);
            break;
        }
    }
}
