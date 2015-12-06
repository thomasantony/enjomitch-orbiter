#include "ORBITERTOOLS.h"

double ORBITERTOOLS::getFlightVectorHeading(VESSEL *v)
{
    VECTOR3 shipAirspeedVector;
    v->GetHorizonAirspeedVector(shipAirspeedVector);
    normalise(shipAirspeedVector);
    double vector = acos(shipAirspeedVector.z/sqrt((shipAirspeedVector.x*shipAirspeedVector.x)+(shipAirspeedVector.z*shipAirspeedVector.z)));
    vector = atan2(shipAirspeedVector.x, shipAirspeedVector.z);
    //if (vector > 2*PI) vector -= 2*PI;
    return vector;
}

void ORBITERTOOLS::pointRadialDistance(double lat1, double lon1, double bearing, double distance, VESSEL * v, double *lat2, double *lon2)
{
    double rdistance = distance / oapiGetSize(v->GetGravityRef());
    * lat2 = asin(sin(lat1) * cos(rdistance) + cos(lat1) * sin (rdistance) * cos(bearing) );
    * lon2 = lon1 + atan2( sin (bearing) * sin (rdistance) * cos (lat1), cos (rdistance) - sin(lat1) * sin (* lat2 ));
}
