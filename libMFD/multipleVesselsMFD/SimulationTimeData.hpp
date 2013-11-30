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
// SimulationTimeData - Struct conataining read-only simulation times,
//						passed to the client plugin
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

#ifndef SIMULATIONTIMEDATA_HPP_INCLUDED
#define SIMULATIONTIMEDATA_HPP_INCLUDED

namespace EnjoLib
{
namespace MFDGoodies
{
/// Contains simulation times
/**
Convenience read-only struct containing simulation times accuired from Orbiter core.
It is passed to the client plugin.
Refer to Orbiter API reference for exact meaning of the variables, because they differ
for oapi::Module::clbkPreStep and oapi::Module::clbkPostStep
*/
class SimulationTimeData
{
public:
    /// Default Ctor
    SimulationTimeData(double simt, double simdt, double mjd);

    /// Returns simulation time
    double GetSimT() const;
    /// Returns Time step length
    double GetSimDT() const;
    /// Returns simulation time Modified Julian Date format [days]
    double GetMJD() const;

private:
    double m_simt, m_simdt, m_mjd;
};
}
}

#endif // SIMULATIONTIMEDATA_HPP_INCLUDED
