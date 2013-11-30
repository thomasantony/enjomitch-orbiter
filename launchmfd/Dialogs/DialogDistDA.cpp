#include "DialogDistDA.h"
#include "../LaunchMFD.h"
#include "../MFDDataLaunchMFD.hpp"
#include <MFDSound++/Sound.hpp>
#include "../Sound/SoundSampleIDEnum.hpp"

using namespace EnjoLib;

bool DialogDistDA::clbk(void *id, char *str, void *usrdata)
{
    LaunchMFD* cLaunchMFD = (LaunchMFD*)usrdata;
    MFDDataLaunchMFD * data = cLaunchMFD->GetData();
    double distDA;

    if (sscanf_s(str, "%lf",&distDA) == 1)
    {
        data->m_distDA = distDA;
        cLaunchMFD->GetSound().PlaySound(ALTITUDE_SET);
    }
    else
        return (false);

    return true;
}
