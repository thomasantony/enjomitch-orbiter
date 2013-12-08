#include "OptiFunction.h"
#include "../basefunction.h"

double OptiFunctionBase::RecalculateGetValue()
{
    if (m_base->IsPlanSlingshot()) // needs more refresh frames
    {
        for (int i = 0; i < 50; ++i)
        // needs at least 7 iterations to converge in eject mode, and 40 in slingshot mode!
            m_base->UpdateAllPlans();
    }
    else
    {
        VECTOR3 tmp;
        for (int i = 0; i < 9; ++i)
            m_base->calculate(&tmp); // twice as fast, but works only in Eject mode
    }
    VECTOR3 craftpos, targetpos;
    m_icept->getpositions(&craftpos,&targetpos);
    double closestApproach = length(craftpos-targetpos);
    return closestApproach;
}

// Deduct the minimized value, based on tested input, supplied by the optimizer
double OptiFunction::UpdateGetValue( double arg )
{
    *m_toOpti.var = arg; // Provide feedback to TransX's calculation functions
    double closestApproach = RecalculateGetValue(); // Get feedback FROM TransX
    return closestApproach; // Return the feedback to the binary search algorithm
}
