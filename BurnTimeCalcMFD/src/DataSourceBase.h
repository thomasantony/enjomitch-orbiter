#ifndef DATASOURCEBASE_H
#define DATASOURCEBASE_H

#include <OrbiterAPI.h>
class MFDDataBurnTime;
class DataSourceBase
{
    public:
        DataSourceBase();
        virtual ~DataSourceBase() {}

        virtual const char * GetName() const = 0;
        virtual bool GetFromMM(MFDDataBurnTime * data) = 0;
        virtual double GetIBT() const = 0;
        virtual double GetDV() const { return length(GetVelVec()); }
        virtual VECTOR3 GetVelVec() const { return _V(0, 0, 0); }
    protected:

    private:
};

#endif // DATASOURCEBASE_H
