#include "MFDButtonPageTopoMFD.h"
#include "TopoMapMFD.h"

MFDButtonPageTopoMFD::MFDButtonPageTopoMFD()
{
    // The menu descriptions of all buttons
    static const MFDBUTTONMENU mnu1[] =
    {
        {"Switch RGB/Grey", 0, 'R'},
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("RGB", OAPI_KEY_R, &TopoMapMFD::SwitchRGBGrey);
}

MFDButtonPageTopoMFD::~MFDButtonPageTopoMFD()
{
    //dtor
}
