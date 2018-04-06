// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#include "ORBITERTOOLS.h"

double ORBITERTOOLS::getFlightVectorHeading(const VESSEL *v)
{
	VECTOR3 shipAirspeedVector = {0,0,0};
    if (! v->GetHorizonAirspeedVector(shipAirspeedVector))
		return 0;
	if (length(shipAirspeedVector) == 0)
		return 0; // Protect from division by 0 by normalise
    normalise(shipAirspeedVector);
    double vector = acos(shipAirspeedVector.z/sqrt((shipAirspeedVector.x*shipAirspeedVector.x)+(shipAirspeedVector.z*shipAirspeedVector.z)));
    vector = atan2(shipAirspeedVector.x, shipAirspeedVector.z);
    //if (vector > 2*PI) vector -= 2*PI;
    return vector;
}

EnjoLib::Geo ORBITERTOOLS::pointRadialDistance(const EnjoLib::Geo & g, double bearing, double distance, const VESSEL * v)
{
    double rdistance = distance / oapiGetSize(v->GetGravityRef());
    double lat2 = asin(sin(g.lat) * cos(rdistance) + cos(g.lat) * sin (rdistance) * cos(bearing) );
    double lon2 = g.lon + atan2( sin (bearing) * sin (rdistance) * cos (g.lat), cos (rdistance) - sin(g.lat) * sin ( lat2 ));
    return EnjoLib::Geo(lat2, lon2);
}
