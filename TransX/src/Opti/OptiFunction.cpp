#include "OptiFunction.h"
#include "../basefunction.h"
#include "ConstraintFactory.h"
#include "Constraint.h"

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


double OptiMultiFunction::Get(const double * in, int n)
{

    for (int i = 0; i < n; ++i)
    {
        double arg = *(in + i);
        if (fabs(arg) <= 0.1)
            arg = 0.11; // Has to be done this way because otherwise TransX would ignore this variable
        *(m_toOpti.at(i)).var = arg; // Provide feedback to TransX's calculation functions
    }

    double closestApproach = RecalculateGetValue(); // Get feedback FROM TransX

    //penality function
    double penalitySum = 0;
    ConstraintFactory costrFact(m_base); // Setup constraints and precision
    for (int i = 0; i < n; ++i)
    {
        const VarConstraint & item = m_toOpti.at(i);
        Constraint cstr = costrFact.Create(item.constraintType);
        double arg = *(in + i);
        double diff = 0;
        if (arg > cstr.upper)
            diff = fabs(cstr.upper - arg);
        else if (arg < cstr.lower)
            diff = fabs(arg - cstr.lower);

        double penality = diff * diff * diff * diff * 10e3;
        penalitySum += penality;
    }
    double ret = closestApproach + penalitySum;

    return ret; // Return the feedback to the binary search algorithm
}

std::vector<double> OptiMultiFunction::GetStart() const
{
    std::vector<double> start;
    ConstraintFactory costrFact(m_base);
    for (size_t i = 0; i < m_toOpti.size(); ++i)
	{
        //if (m_toOpti.at(i).startFrom0)
            //start.push_back(0);
        //else
          //  start.push_back(*(m_toOpti.at(i)).var);

		const VarConstraint & item = m_toOpti.at(i);
        Constraint cstr = costrFact.Create(item.constraintType);
		start.push_back((cstr.lower + cstr.upper) / 2.0);
	}
    return start;
}
std::vector<double> OptiMultiFunction::GetStep() const
{
    std::vector<double> step;
    for (size_t i = 0; i < m_toOpti.size(); ++i)
        step.push_back(10);
    return step;
}
