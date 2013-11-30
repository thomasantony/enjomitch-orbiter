#include "OffPlaneCorrectorDirectAscent.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include <Math/MaxMin.hpp>

using namespace EnjoLib;

OffPlaneCorrectorDirectAscent::OffPlaneCorrectorDirectAscent()
{}

OffPlaneCorrectorDirectAscent::~OffPlaneCorrectorDirectAscent()
{}

double OffPlaneCorrectorDirectAscent::CalculateClient( double timeToNode, MFDDataLaunchMFD * data )
{
    double correctionAngle = 0;

    if ( data->pegDA.IsToTarget() && data->peg.IsValid() )
    {
        if ( timeToNode > 0 )
        {
            const double tMECO = data->pegDA.GetTMECO();
            correctionAngle = GetCorrectionTimePositive( tMECO, timeToNode, data, limit );
        }
        else
        {
            isValid = false;
            return 0;
        }
    }
    else
    {
        if ( timeToNode > 0 )
        {
            const double deltaT = data->pegDA.GetDeltaT(); // Gets under plane quicker
            const double tMECO = data->pegDA.GetTMECO(); // helps with inclined orbits
            const double tArg = MaxMin<double>(deltaT, tMECO).GetMax(); // Best of both worlds
            correctionAngle = GetCorrectionTimePositive( tArg, timeToNode, data, limit );
            if ( ! isValid )
            {
                isValid = true;
                double adjust = -limit;
                correctionAngle = adjust;
            }
        }
        else
        {
            isValid = false;
            return 0;
        }
    }
    return correctionAngle;
}

// buggy code
bool OffPlaneCorrectorDirectAscent::IsAbovePlane( double timeToNode, MFDDataLaunchMFD * data ) const
{
    double inclVessel = data->GetMovParams().ele.i;
    bool goingSouth = timeToNode > 0 && fabs(inclVessel) > fabs(data->GetTgtParam().incl);
    bool goingNorth = timeToNode < 0 && fabs(inclVessel) < fabs(data->GetTgtParam().incl);
    sprintf(oapiDebugString(), "goingSouth = %d, goingNorth = %d", goingSouth, goingNorth);
    return goingSouth || goingNorth;
}


