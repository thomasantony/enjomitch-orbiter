// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#ifndef ORBITERTOOLS_H
#define ORBITERTOOLS_H

#include <OrbiterSDK.h>
#include <Systems/Geo.hpp>
class ORBITERTOOLS
{
    public:
        static double getFlightVectorHeading(const VESSEL *v);
        static EnjoLib::Geo pointRadialDistance(const EnjoLib::Geo & geo, double bearing, double distance, const VESSEL * v);
};

#endif // ORBITERTOOLS_H
