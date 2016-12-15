#ifndef COMMS_H
#define COMMS_H

#include <OrbiterAPI.h>

class BurnTimeMFD;
class MFDDataBurnTime;
class DataSourceBase;
class Comms
{
    public:
        Comms();
        virtual ~Comms();

        bool ParseInput(char * str, MFDDataBurnTime * data);
        bool HandlerGetFromOtherMFD(BurnTimeMFD * btcMFD, MFDDataBurnTime * data);
        bool GetFromTransX(double *dV, double *IBT);
        bool GetFromLagrange(VECTOR3 *vel, double *dV, double *IBT);
        bool GetFromBaseSync(double *dV, double *IBT, MFDDataBurnTime * data);
        bool SetOtherMFDBurnVars(bool TransX, const double dV, double IBT, MFDDataBurnTime * data);

        bool SetOtherMFDBurnVars(const DataSourceBase * source, MFDDataBurnTime * data);

    protected:

    private:


};

#endif // COMMS_H
