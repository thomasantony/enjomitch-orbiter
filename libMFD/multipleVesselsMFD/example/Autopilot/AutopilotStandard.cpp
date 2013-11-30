#include "AutopilotStandard.hpp"

void AutopilotStandard::Guide( MyMFDData * data ) const
{
  AutopilotBase::Guide( data );
}

bool AutopilotStandard::NeedPitch( MyMFDData * data ) const
{
    return true;
}
bool AutopilotStandard::NeedYaw( MyMFDData * data ) const
{
    return true;
}
bool AutopilotStandard::NeedMECO( MyMFDData * data ) const
{
    return true;
}
