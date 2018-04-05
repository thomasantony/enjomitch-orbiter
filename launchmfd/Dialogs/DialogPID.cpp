#include "DialogPID.h"
#include "../LaunchMFD.h"
#include "../MFDDataLaunchMFD.hpp"
#include <Utils/FileUtilsOrbiter.hpp>
#include "../localisation.h"
#include <MFDSound++/Sound.hpp>
#include "../Sound/SoundSampleIDEnum.hpp"

using namespace EnjoLib;

void DialogPID::PIDTuningBank( LaunchMFD * launchMFD )
{
    const MFDDataLaunchMFD * data = launchMFD->GetData();
    const VESSEL * v = data->GetVessel();
    std::string configFile = FileUtilsOrbiter().GetVesselConfigFileName(v);
    std::ostringstream ssMsg, ssVal;
    if (data->IsInAtmosphere())
    {
        ssMsg << PID_TUNE_ATMO_BANK;
        ssVal << data->m_pidAP.m_pidAPAtmBankTarget.GetKp() << " ";
        ssVal << data->m_pidAP.m_pidAPAtmBankTarget.GetKd() << " ";
        ssVal << data->m_pidAP.m_pidAPAtmBankTarget.GetKi();
    }
    else
    {
        ssMsg << PID_TUNE_SPACE_BANK;
        ssVal << data->m_pidAP.m_pidAPSpaceBank.GetKp() << " ";
        ssVal << data->m_pidAP.m_pidAPSpaceBank.GetKd() << " ";
        ssVal << data->m_pidAP.m_pidAPSpaceBank.GetKi();
    }
    ssMsg << configFile;
    oapiOpenInputBox((char*)ssMsg.str().c_str(), clbkPIDTuningBank, (char*)ssVal.str().c_str(), 20, launchMFD);
}
void DialogPID::PIDTuningXY( LaunchMFD * launchMFD )
{
    const MFDDataLaunchMFD * data = launchMFD->GetData();
    const VESSEL * v = data->GetVessel();
    std::string configFile = FileUtilsOrbiter().GetVesselConfigFileName(v);
    std::ostringstream ssMsg, ssVal;
    if (data->IsInAtmosphere())
    {
        ssMsg << PID_TUNE_ATMO_XY;
        ssVal << data->m_pidAP.m_pidAPAtmX.GetKp() << " ";
        ssVal << data->m_pidAP.GetReferenceKdForAtmo() << " ";
        ssVal << data->m_pidAP.m_pidAPAtmX.GetKi();
    }
    else
    {
        ssMsg << PID_TUNE_SPACE_XY;
        ssVal << data->m_pidAP.m_pidAPSpaceX.GetKp() << " ";
        ssVal << data->m_pidAP.m_pidAPSpaceX.GetKd() << " ";
        ssVal << data->m_pidAP.m_pidAPSpaceX.GetKi();
    }
    ssMsg << configFile;
    oapiOpenInputBox((char*)ssMsg.str().c_str(), clbkPIDTuningXY, (char*)ssVal.str().c_str(), 20, launchMFD);
}

bool DialogPID::clbkPIDTuningXY(void *id, char *str, void *usrdata)
{
    LaunchMFD* cLaunchMFD = (LaunchMFD*)usrdata;
    MFDDataLaunchMFD * data = cLaunchMFD->GetData();
    double kp, kd, ki;

    if (sscanf_s(str, "%lf%lf%lf",&kp, &kd, &ki) == 3)
    {
        if (data->IsInAtmosphere() )
            data->m_pidAP.SetAtmoXY( PID(kp, kd, ki) );
        else
            data->m_pidAP.SetSpaceXY( PID(kp, kd, ki ) );

        cLaunchMFD->GetSound().PlayWave(TARGET_MANUAL);
    }
    else return (false);

    return (true);
}

bool DialogPID::clbkPIDTuningBank(void *id, char *str, void *usrdata)
{
    LaunchMFD* cLaunchMFD = (LaunchMFD*)usrdata;
    MFDDataLaunchMFD * data = cLaunchMFD->GetData();
    double kp, kd, ki;

    if (sscanf_s(str, "%lf%lf%lf",&kp, &kd, &ki) == 3)
    {
        if (data->IsInAtmosphere() )
            data->m_pidAP.SetAtmoBank( PID(kp, kd, ki) );
        else
            data->m_pidAP.SetSpaceBank( PID(kp, kd, ki) );

        cLaunchMFD->GetSound().PlayWave(TARGET_MANUAL);
    }
    else return (false);

    return (true);
}
