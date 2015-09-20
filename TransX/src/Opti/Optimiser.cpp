#include "Optimiser.h"
//#include <Math/BinSearchOpti.hpp>
#include <Math/Opti/OptiFactory.hpp>
#include <Math/Opti/OptiSubject.hpp>
#include "ConstraintFactory.h"
#include "Constraint.h"
#include "OptiFunction.h"
#include "../mfdvartypes.h"

using namespace std;

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
	// for (int k = 0; k < 10000; ++k) // For testing
	//for (int k = 0; k < 10; ++k) // For testing
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
        //EnjoLib::BinSearchOpti binSearch(cstr.lower, cstr.upper, cstr.precision);
        //EnjoLib::Result<double> xopt = binSearch.Run(optiFunction);
        EnjoLib::OptiType optiType = EnjoLib::BRENT;
        optiType = EnjoLib::BIN_SEARCH;
        std::auto_ptr<EnjoLib::IOptiAlgo> optiAlgo = EnjoLib::OptiFactory::Create(optiType, cstr.lower, cstr.upper, cstr.precision);
        EnjoLib::Result<double> xopt = optiAlgo->Run(optiFunction);
        if (!xopt.isSuccess)
            *item.var = variableBackup;
       //    cout << "SUCCESS!" << endl;
        //else
        //    cout << "FAILURE!" << endl;
        //cout << "opti x = " << xopt.value << ", y = " << f.UpdateGetValue(xopt.value) << endl;
    }
}
