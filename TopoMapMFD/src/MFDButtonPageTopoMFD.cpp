// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#include "MFDButtonPageTopoMFD.h"
#include "TopoMapMFD.h"

MFDButtonPageTopoMFD::MFDButtonPageTopoMFD()
{
    // The menu descriptions of all buttons
    static const MFDBUTTONMENU mnu1[] =
    {
        {"Switch RGB/Grey", 0, 'R'},
        {"Increment refresh rate", 0, ']'},
        {"Decrement refresh rate", 0, '['},
        {"Zoom In", 0, 'Z'},
        {"Zoom Out", 0, 'X'},
        {"Zoom Auto", 0, 'A'},
        {"Zoom Maximal", 0, 'M'},
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("RGB", OAPI_KEY_R,         &TopoMapMFD::SwitchRGBGrey);
    RegisterFunction("INC", OAPI_KEY_RBRACKET,  &TopoMapMFD::RefreshIncrement);
    RegisterFunction("DEC", OAPI_KEY_LBRACKET,  &TopoMapMFD::RefreshDecrement);
    RegisterFunctionCont("Z+",  OAPI_KEY_Z,         &TopoMapMFD::ZoomIn);
    RegisterFunctionCont("Z-",  OAPI_KEY_X,         &TopoMapMFD::ZoomOut);
    RegisterFunction("ZA",  OAPI_KEY_A,         &TopoMapMFD::ZoomAutoSwitch);
    RegisterFunction("ZMx", OAPI_KEY_M,         &TopoMapMFD::ZoomMaximal);
}

MFDButtonPageTopoMFD::~MFDButtonPageTopoMFD()
{
    //dtor
}
