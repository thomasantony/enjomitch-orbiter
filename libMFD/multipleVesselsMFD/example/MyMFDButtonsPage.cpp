#include "MyMFDButtonsPage.hpp"

MyMFDButtonsPage::MyMFDButtonsPage()
{
    // The menu descriptions
    static const MFDBUTTONMENU mnu1[] =
    {
        {"Switch buttons page", 0, 'B'},
        {"Select target", 0, 'T'},
        {"Switch sound", 0, 'S'},
        {"Autopilot", 0, 'P'},
        {"Autopilot 2", 0, 'O'}
    };
    RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

    RegisterFunction("PG",  OAPI_KEY_B, &MyMFDMultipleVessels::SwitchButtonsPage);
    RegisterFunction("TGT", OAPI_KEY_T, &MyMFDMultipleVessels::SelectTarget);
    RegisterFunction("AP",  OAPI_KEY_P, &MyMFDMultipleVessels::SwitchAutopilotStandard);
    RegisterFunction("AP2", OAPI_KEY_O, &MyMFDMultipleVessels::SwitchAutopilotSophisticated);
    RegisterFunction("HUD", OAPI_KEY_H, &MyMFDMultipleVessels::SwitchHUD);

    static const MFDBUTTONMENU mnu2[] =
    {
        {"Switch buttons page", 0, 'B'},
        {"Switch sound", 0, 'S'}
    };
    RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));
    RegisterFunction("PG",  OAPI_KEY_B, &MyMFDMultipleVessels::SwitchButtonsPage);
    RegisterFunction("SND", OAPI_KEY_S, &MyMFDMultipleVessels::SwitchUseSound);
}

MyMFDButtonsPage::~MyMFDButtonsPage()
{

}

bool MyMFDButtonsPage::SearchForKeysInOtherPages() const
{
    return true;
}
