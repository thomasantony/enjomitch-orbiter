#include "MFDButtonPageBTC.h"

MFDButtonPageBTC::MFDButtonPageBTC()
{
    static const MFDBUTTONMENU mnu1[] = {
        {"Switch buttons page", 0, 'P'},
        {"Enter target or deltaV", 0, 'V'},
        {"Enter time to Maneuver", 0, 'T'},
        {"Enter offset distance", "to target", 'O'},
        {"Set Target for", "distance calculation", 'S'},
        {"Reset MFD", 0, 'R' },

        {"Change mode", 0, 'M'},
        {"Arm Autoburn", 0, 'A'},
        {"Burn Now", 0, 'B'},
        {"Arm Autocirc", 0, 'C'},
        {"Switch SI / US", "unit system", 'U'},
        {"Select engine", 0, 'E'}
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PG",  OAPI_KEY_P, &BurnTimeMFD::HandlerSwitchButtonPage);
    RegisterFunction("DV",  OAPI_KEY_V, &BurnTimeMFD::HandlerTargetOrDV);
    RegisterFunction("DT",  OAPI_KEY_T, &BurnTimeMFD::HandlerTimeOfManoeuvre);
    RegisterFunction("OS",  OAPI_KEY_O, &BurnTimeMFD::HandlerOffsetDistance);
    RegisterFunction("ST",  OAPI_KEY_S, &BurnTimeMFD::HandlerTargetForDistanceCalc);
    RegisterFunction("RST", OAPI_KEY_R, &BurnTimeMFD::HandlerReset);

    RegisterFunction("MD",  OAPI_KEY_M, &BurnTimeMFD::HandlerChangeMode);
    RegisterFunction("ARM", OAPI_KEY_A, &BurnTimeMFD::HandlerAutoBurn);
    RegisterFunction("BRN", OAPI_KEY_B, &BurnTimeMFD::HandlerBurnNow);
    RegisterFunction("CIR", OAPI_KEY_C, &BurnTimeMFD::HandlerAimAutoCirc);
    RegisterFunction("UNT", OAPI_KEY_U, &BurnTimeMFD::HandlerSwitchSI_US);
    RegisterFunction("ENG", OAPI_KEY_E, &BurnTimeMFD::HandlerSelectEngine);

// {"DV","DT","OS","ST","RST", "MD", "ARM","BRN", "CIR","UNT","ENG","EXT"};
    static const MFDBUTTONMENU mnu2[] = {
      {"Switch buttons page", 0, 'B'},
      {"Enter extra fuel", 0, 'X'},
      {"Get from TransX", 0, 'G'}
    };
    RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PG",  OAPI_KEY_P, &BurnTimeMFD::HandlerSwitchButtonPage);
    RegisterFunction("EXT", OAPI_KEY_X, &BurnTimeMFD::HandlerEnterExtraFuel);
    RegisterFunction("TX",  OAPI_KEY_G, &BurnTimeMFD::HandlerGetFromTransX);
}

MFDButtonPageBTC::~MFDButtonPageBTC()
{}

bool MFDButtonPageBTC::SearchForKeysInOtherPages() const
{
    return true;
}

