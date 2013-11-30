#include "Stage.h"
#include <Orbitersdk.h>
#include "Utils/VesselCapabilities.hpp"

Stage::Stage()
{
    isp = tmeco = accel = 0;
}

Stage::Stage( double tmeco, const VESSEL * v )
{
    VesselCapabilities vc;
    Engine engCurrent = vc.GetMainThrustParm(v);
    if(engCurrent.F == 0) //If no thrust now, get the max thrust
        engCurrent = vc.EstimateMainThrustParm(v);

    this->isp = engCurrent.isp;
    this->accel = engCurrent.F / v->GetMass();
    this->tmeco = tmeco;
}
