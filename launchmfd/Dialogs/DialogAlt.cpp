#include "DialogAlt.h"
#include "../LaunchMFD.h"
#include "../MFDDataLaunchMFD.hpp"
#include <MFDSound++/Sound.hpp>
#include "../Sound/SoundSampleIDEnum.hpp"
#include <Math/MaxMin.hpp>
#include <EnjoLib/ModuleMessaging.hpp>
#include <algorithm>

using namespace EnjoLib;

bool DialogAlt::clbk(void *id, char *str, void *usrdata)
{
    LaunchMFD* cLaunchMFD = (LaunchMFD*)usrdata;
    MFDDataLaunchMFD * data = cLaunchMFD->GetData();
    std::string input = str;
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    double PeA, ApA;

    if (input == "tx" || input == "transx")
    {
        Result<double> rad = ModuleMessaging().GetDouble("TransX", "PeRadius");
        if (rad.isSuccess)
        {
            PeA = rad.value - oapiGetSize(data->hRef);
            if (PeA < 0) return false;
            data->PeA = data->ApA = PeA;
            cLaunchMFD->GetSound().PlaySound(ALTITUDE_SET);
        }
        else
            return false;
    }
    else if (sscanf_s(str, "%lf%lf",&PeA, &ApA) == 2)
    {
        if (PeA < 0 || ApA < 0) return false;
        MaxMin<double> maxMinApses(PeA,ApA);
        data->PeA = maxMinApses.GetMin() * 1000;
        data->ApA = maxMinApses.GetMax() * 1000;
        cLaunchMFD->GetSound().PlaySound(ALTITUDE_SET);
    }
    else if (sscanf_s(str, "%lf",&PeA) == 1)
    {
        if (PeA < 0) return false;
        data->PeA = data->ApA = PeA * 1000;
        cLaunchMFD->GetSound().PlaySound(ALTITUDE_SET);
    }
    else if (!_strnicmp(str, "a", 1))
    {
        double alt = data->GetAutomaticAlt(data->hRef);
        if (alt != 0)
        {
            data->PeA = data->ApA = alt;
            cLaunchMFD->GetSound().PlaySound(ALTITUDE_AUTO);
        }
        else return (false);
    }
    else if (!_strnicmp(str, "t", 1))
    {
        ELEMENTS e;
        ORBITPARAM op;
        VESSEL * v = data->GetTargetVessel();
        if (v == NULL)
        {
            return false;
        }
        v->GetElements(NULL, e, &op, 0,FRAME_EQU);
        double rad = data->GetMovParams().m_rad;
        data->PeA = data->ApA = (op.PeD + op.ApD) / 2 - rad;
        cLaunchMFD->GetSound().PlaySound(ALTITUDE_SET);
    }
    else return (false);

    if (data->PeA < 0) data->PeA = 0;
    if (data->ApA < 0) data->ApA = 0;
    return (true);
}
