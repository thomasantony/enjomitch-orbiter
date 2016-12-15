#ifndef DATASOURCETRANSX_H
#define DATASOURCETRANSX_H

#include "DataSourceBase.h"

class DataSourceTransX : public DataSourceBase
{
    public:
        DataSourceTransX();
        virtual ~DataSourceTransX() {}

        const char * GetName() const { return "TransX"; }
        bool GetFromMM(MFDDataBurnTime * data);
        double GetIBT() const { return m_ibt; }
        double GetDV() const { return m_dv; }

    protected:

    private:
        double m_ibt;
        double m_dv;
};

#endif // DATASOURCETRANSX_H
