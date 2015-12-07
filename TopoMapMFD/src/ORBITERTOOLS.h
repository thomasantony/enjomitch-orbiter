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
