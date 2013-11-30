#ifndef TGTPARAM_H
#define TGTPARAM_H

#include "Types.hpp"

struct TGTPARAM
{
	TGTPARAM();

    double incl, timeToNode, angle, rad, TrL, velRad, delayRad;
	LAUNCH_AZIMUTH azFlag;
};


#endif // TGTPARAM_H
