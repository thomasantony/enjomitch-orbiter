// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

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
