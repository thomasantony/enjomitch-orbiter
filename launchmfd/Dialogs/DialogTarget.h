#ifndef DialogTarget_H
#define DialogTarget_H

#include <string>
#include <OrbiterSDK.h>

class VESSEL;
class MFDDataLaunchMFD;

class DialogTarget
{
    public:
        static bool clbk(void *id, char *str, void *usrdata);

    protected:
    private:
        static VESSEL * UpdateProbe( MFDDataLaunchMFD * data, double inclDeg, double LANDeg, int frame, VESSEL * copyFrom = NULL );
        static std::string GetProbeName( VESSEL * v );
        static void SetTarget(MFDDataLaunchMFD * data, OBJHANDLE hTgt, double incl, const std::string & dispTarget);
};

#endif // DialogTarget_H
