#ifndef BODYPHYS_ORB_H
#define BODYPHYS_ORB_H

#include <OrbiterSDK.h>
#include "BODYPHYS.h"

struct BODYPHYS_ORB : public BODYPHYS
{
    BODYPHYS_ORB();
    BODYPHYS_ORB(OBJHANDLE hRef);
};

#endif // BODYPHYS_ORB_H
