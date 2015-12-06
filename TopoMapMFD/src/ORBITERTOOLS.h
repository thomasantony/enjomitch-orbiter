#ifndef ORBITERTOOLS_H
#define ORBITERTOOLS_H

#include <OrbiterSDK.h>

class ORBITERTOOLS
{
    public:
        static double getFlightVectorHeading(VESSEL *v);
        static void pointRadialDistance(double lat1, double lon1, double bearing, double distance, VESSEL * v, double *lat2, double *lon2);
};

#endif // ORBITERTOOLS_H
