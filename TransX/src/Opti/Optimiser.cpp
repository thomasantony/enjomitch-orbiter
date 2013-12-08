#include "Optimiser.h"
#include <Math/BinSearchOpti.hpp>
#include "ConstraintFactory.h"
#include "Constraint.h"
#include "OptiFunction.h"
#include "../mfdvartypes.h"

using namespace std;

const double Optimiser::m_cdefaultMin = -4.5e3;
const double Optimiser::m_cdefaultMax = +4.5e3;
const double Optimiser::m_cdefaultRatioHohmann = 0.5;

Optimiser::Optimiser(basefunction * base, Intercept * icept, const std::vector<VarConstraint> & pArgs2Find)
{
	m_pArgs2Find = pArgs2Find;
    m_base = base;
    m_icept = icept;
}

Optimiser::Optimiser(basefunction * base, Intercept * icept, VarConstraint pArg2Find)
{
	m_pArgs2Find.push_back(pArg2Find);
    m_base = base;
    m_icept = icept;
}

Optimiser::~Optimiser()
{}

// Prepares the optimisation for all registered variables
void Optimiser::Optimise() const
{
    for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
    {
        const VarConstraint & item = m_pArgs2Find.at(i);
        if (!item.var->ShouldBeOptimised())
            continue;   // Auto-Min not selected
        double variableBackup = *item.var; // Backup the variable, in case the Auto-Min goes wrong
        OptiFunction optiFunction(item, m_base, m_icept); // Defines the "Optimisation Problem"
        ConstraintFactory costrFact(m_base); // Setup constraints and precision
        Constraint cstr = costrFact.Create(item.constraintType);
        // Pass them to the binary search algorithm
        EnjoLib::BinSearchOpti binSearch(cstr.lower, cstr.upper, cstr.precision);
        EnjoLib::Result<double> xopt = binSearch.Run(optiFunction);
        if (!xopt.status)
            *item.var = variableBackup;
       //    cout << "SUCCESS!" << endl;
        //else
        //    cout << "FAILURE!" << endl;
        //cout << "opti x = " << xopt.value << ", y = " << f.UpdateGetValue(xopt.value) << endl;
    }
}
