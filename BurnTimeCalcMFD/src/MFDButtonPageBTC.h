#ifndef MFDBUTTONPAGEBTC_H
#define MFDBUTTONPAGEBTC_H

#include <MFDButtonPage.hpp>
#include "BurnTimeMFD.h"

class MFDButtonPageBTC : public EnjoLib::MFDGoodies::MFDButtonPage<BurnTimeMFD>
{
    public:
        MFDButtonPageBTC();
        virtual ~MFDButtonPageBTC();
        bool SearchForKeysInOtherPages() const;
    protected:
    private:
};

#endif // MFDBUTTONPAGEBTC_H
