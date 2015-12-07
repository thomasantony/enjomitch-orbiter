#include "ORBITERTOOLS.h"

double ORBITERTOOLS::getFlightVectorHeading(const VESSEL *v)
{
    VECTOR3 shipAirspeedVector;
    v->GetHorizonAirspeedVector(shipAirspeedVector);
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
