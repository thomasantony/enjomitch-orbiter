#include <cstdlib>
#include "AutopilotManager.hpp"
#include "AutopilotStandard.hpp"
#include "AutopilotSophisticated.hpp"

AutopilotManager::AutopilotManager()
{
    pAPStandard = new AutopilotStandard();
    pAPSophisticated = new AutopilotSophisticated();
}

AutopilotManager::~AutopilotManager()
{
    delete pAPStandard;
    delete pAPSophisticated;
}

AutopilotBase * AutopilotManager::GetAP(AutopilotType type)
{
    switch (type)
    {
    case AP_STANDARD:
        return pAPStandard;
    case AP_SOPHISTICATED:
        return pAPSophisticated;
    case AP_NONE:
        return NULL;
    }

    return NULL;
}
