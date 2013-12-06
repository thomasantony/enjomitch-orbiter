#include "Optimiser.h"
#include "basefunction.h"
#include <Math/BinSearchOptiSubject.hpp>
#include <Math/BinSearchOpti.hpp>
#include "ConstraintFactory.h"
#include "Constraint.h"

using namespace std;
using namespace EnjoLib;

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
{
    int test = 1;
    int test2 = 23;
}

class OptiFunction : public EnjoLib::BinSearchOptiSubject
{
    public:
        OptiFunction(VarConstraint toOpti, basefunction * base, Intercept * icept, const std::vector<VarConstraint> & pArgs2Find)
        : m_pArgs2Find(pArgs2Find)
        , m_base(base)
        , m_icept(icept)
        , m_toOpti(toOpti)
		, m_iter(0)
        {
        }

        // Deduct the minimized value, based on tested input, supplied by the optimizer
		double UpdateGetValue( double arg )
        {
			++m_iter;
			double xx = arg; // for debugging

			*m_toOpti.var = arg;
			//*m_pArgs2Find.at(0) = arg;
            for (int i = 0; i < 50; ++i)
			{
				// needs at least 7 iterations to converge in eject mode, and 40 in slingshot mode!
				m_base->UpdateAllPlans();
			}
            VECTOR3 craftpos, targetpos;
            m_icept->getpositions(&craftpos,&targetpos);
            double len = length(craftpos-targetpos);
            return len;
        }
    private:
        basefunction * m_base;
        Intercept * m_icept;
        VarConstraint m_toOpti;
        std::vector<VarConstraint> m_pArgs2Find;
		mutable int m_iter;
};

void Optimiser::Optimise() const
{
    for (size_t i = 0; i < m_pArgs2Find.size(); ++i) // quick and dirty
    {
		const VarConstraint & item = m_pArgs2Find.at(i);
		if (!item.var->ShouldBeOptimised())
            continue;
		ConstraintFactory costrFact(m_base);
		Constraint constraint = costrFact.Create(item.constraintType);
        double variablesBackup; // will revert to this

        OptiFunction optiFunction(item, m_base, m_icept, m_pArgs2Find);
        //for (size_t i = 0; i < sz; ++i)
        //    starting_point(i) = *m_pArgs2Find.at(i);

        variablesBackup = *item.var;
        double min_point = constraint.lower;
        double max_point = constraint.upper;
        BinSearchOpti bs(min_point, max_point, 0.001);
        Result<double> xopt = bs.Run(optiFunction);
        //if (xopt.status)
       //    cout << "SUCCESS!" << endl;
        //else
        //    cout << "FAILURE!" << endl;
        //cout << "opti x = " << xopt.value << ", y = " << f.UpdateGetValue(xopt.value) << endl;
    }
}
