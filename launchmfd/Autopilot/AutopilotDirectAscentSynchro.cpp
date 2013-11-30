#include "AutopilotDirectAscentSynchro.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include "../Utils/VesselCapabilities.hpp"
#include "../localisation.h"

AutopilotDirectAscentSynchro::AutopilotDirectAscentSynchro()
{}

AutopilotDirectAscentSynchro::~AutopilotDirectAscentSynchro()
{}

std::string AutopilotDirectAscentSynchro::GetIdentifier() const
{
    return AUTOPILOT_DA + std::string(" Sync");
}

bool AutopilotDirectAscentSynchro::Guide( MFDDataLaunchMFD * data, double dt ) const
{
    if ( ! AutopilotBase::Guide( data, dt ) )
        return false;
    if ( ! data->GetTargetVessel() )
        return false;

    double deltaT = data->pegDAS.GetDeltaT();
    if ( data->pegDAS.IsToTarget() )
    {
        const double ApDShip = data->GetMovParams().opar.ApD;
        if ( ApDShip > data->GetTgtParam().rad && data->m_pidAP.dvReallyUsedSynchro == 0)
        {
            data->m_pidAP.timeOfSynchroBurn = oapiGetSimTime() - data->m_pidAP.timeOfSynchroBurn;
            const double acc = VesselCapabilities().GetMainThrustParm(data->GetVessel()).F / data->GetVessel()->GetMass();
            data->m_pidAP.dvReallyUsedSynchro = acc * data->m_pidAP.timeOfSynchroBurn;
            const double ratio = data->m_pidAP.dvReallyUsedSynchro / data->pegDAS.lastCalculatedDVSum;
            data->GetShipVariablesRW().m_synchroAscentOffBy = ratio;
        }
    }
    else if ( deltaT < 0 && deltaT > -30)
    {
        data->pegDAS.SetToTarget(true);
        data->drawPitchError = false;
        data->MainEngineOn();
        data->m_pidAP.timeOfSynchroBurn = oapiGetSimTime();
    }

    return true;
}

bool AutopilotDirectAscentSynchro::NeedPitch( MFDDataLaunchMFD * data ) const
{
    return false;
}
bool AutopilotDirectAscentSynchro::NeedPullUp( MFDDataLaunchMFD * data ) const
{
    return false;
}
bool AutopilotDirectAscentSynchro::NeedYaw( MFDDataLaunchMFD * data ) const
{
    return true;
}

bool AutopilotDirectAscentSynchro::NeedMECO( MFDDataLaunchMFD * data ) const
{
    return true;
}

bool AutopilotDirectAscentSynchro::TerminationRequested( MFDDataLaunchMFD * data ) const
{
    return data->m_pidAP.dvReallyUsedSynchro != 0;
}
