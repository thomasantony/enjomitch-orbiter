#ifndef SHIP_PARAMS_H
#define SHIP_PARAMS_H

struct SHIP_PARAMS
{
    double massFlowVert, massFlowHori, massFull, thrustHori, thrustVert;
    SHIP_PARAMS()
    {
      massFlowVert = massFlowHori = massFull = thrustHori = thrustVert = 0;
    }
};

#endif // SHIP_PARAMS_H
