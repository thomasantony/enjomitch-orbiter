#ifndef TGTPARAM_ORB_H
#define TGTPARAM_ORB_H

#include <OrbiterSDK.h>
#include "TGTPARAM.h"
class MFDDataLaunchMFD;

struct TGTPARAM_ORB : public TGTPARAM
{
	TGTPARAM_ORB( double inclPrev );
    TGTPARAM_ORB( MFDDataLaunchMFD * data, double inclPrev );

    LAUNCH_AZIMUTH CalcLaunchAzimuth( MFDDataLaunchMFD * data );
};

#endif // TGTPARAM_ORB_H
