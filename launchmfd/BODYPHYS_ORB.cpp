#include "BODYPHYS_ORB.h"

BODYPHYS_ORB::BODYPHYS_ORB()
{
}

BODYPHYS_ORB::BODYPHYS_ORB(OBJHANDLE hRef)
{
    radius = oapiGetSize(hRef);
    mass = oapiGetMass(hRef);
    sidDay = oapiGetPlanetPeriod(hRef);
    cutoffAlt = pressure0 = 0;
    if ( oapiPlanetHasAtmosphere(hRef) )
    {
        if ( const ATMCONST * atm = oapiGetPlanetAtmConstants(hRef) )
        {
            cutoffAlt = atm->altlimit;
            pressure0 = atm->p0;
        }
    }
}

