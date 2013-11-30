#include "AutopilotSophisticated.hpp"

void AutopilotSophisticated::Guide( MyMFDData * data ) const
{
    AutopilotBase::Guide( data );
    // Automatically fire up main engines
    data->MainEngineOn();
}

bool AutopilotSophisticated::NeedPitch( MyMFDData * data ) const
{
    // Could depend on some MyMFDData state
    return true;
}
bool AutopilotSophisticated::NeedYaw( MyMFDData * data ) const
{
    // Could depend on some MyMFDData state
    return true;
}
bool AutopilotSophisticated::NeedMECO( MyMFDData * data ) const
{
    // Could depend on some MyMFDData state
    return true;
}
