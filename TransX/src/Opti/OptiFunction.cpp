#include "OptiFunction.h"
#include "../basefunction.h"

double OptiFunctionBase::RecalculateGetValue()
{
    m_icept->ShouldUpdateBarycenter(false); // Barycenter mode is a CPU hog, because of uncaught divisions by 0
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
		//for (int i = 0; i < 50000; ++i) // for testing
            m_base->calculate(&tmp); // twice as fast, but works only in Eject mode
    }
    m_icept->ShouldUpdateBarycenter(true);
    VECTOR3 craftpos, targetpos;
    m_icept->getpositions(&craftpos,&targetpos);
    double closestApproach = length(craftpos-targetpos);
    return closestApproach;
}

// Deduct the minimized value, based on tested input, supplied by the optimizer
double OptiFunction::UpdateGetValue( double arg )
{
    if (fabs(arg) <= 0.1)
        arg = 0.11; // Has to be done this way because otherwise TransX would ignore this variable
    *m_toOpti.var = arg; // Provide feedback to TransX's calculation functions
    double closestApproach = RecalculateGetValue(); // Get feedback FROM TransX
    return closestApproach; // Return the feedback to the binary search algorithm
}
