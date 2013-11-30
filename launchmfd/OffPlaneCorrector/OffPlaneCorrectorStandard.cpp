#include "OffPlaneCorrectorStandard.hpp"
#include "../MFDDataLaunchMFD.hpp"

OffPlaneCorrectorStandard::OffPlaneCorrectorStandard()
{}

OffPlaneCorrectorStandard::~OffPlaneCorrectorStandard()
{}

double OffPlaneCorrectorStandard::CalculateClient( double timeToNode, MFDDataLaunchMFD * data )
{
    double correctionAngle = 0;
    if ( timeToNode < 0 )
    {
        isValid = false;
        return 0; // we are too far out. Return 0 as you won't be able to get back on plane.
    }
    else
    {
        const double tMECO = data->GetTMECO();
        correctionAngle = GetCorrectionTimePositive( tMECO, timeToNode, data, limit );
    }

    return correctionAngle;
}
