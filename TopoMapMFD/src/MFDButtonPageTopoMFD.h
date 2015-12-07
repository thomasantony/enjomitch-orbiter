#ifndef MFDBUTTONPAGETOPOMFD_H
#define MFDBUTTONPAGETOPOMFD_H

#include <Utils/MFDButtonPage.hpp>
#include "TopoMapMFD.h"

class MFDButtonPageTopoMFD : public EnjoLib::MFDGoodies::MFDButtonPage<TopoMapMFD>
{
    public:
        MFDButtonPageTopoMFD();
        virtual ~MFDButtonPageTopoMFD();
    protected:
        bool SearchForKeysInOtherPages() const {return false;}
    private:
};

#endif // MFDBUTTONPAGETOPOMFD_H
