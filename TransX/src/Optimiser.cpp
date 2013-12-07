#include "Optimiser.h"
#include "basefunction.h"
#include <Math/BinSearchOptiSubject.hpp>
#include <Math/BinSearchOpti.hpp>
#include <Math/BinSearchOpti2DSubject.hpp>
#include <Math/BinSearchOpti2D.hpp>
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
}

class OptiFunctionBase
{
    public:
        OptiFunctionBase(basefunction * base, Intercept * icept)
           : m_base(base)
           , m_icept(icept)
        {
        }
    protected:
        double RecalculateGetValue()
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
            double len = length(craftpos-targetpos);
            return len;
        }

    private:
        basefunction * m_base;
        Intercept * m_icept;
};

class OptiFunction : public OptiFunctionBase, public EnjoLib::BinSearchOptiSubject
{
    public:
        OptiFunction(VarConstraint toOpti, basefunction * base, Intercept * icept)
        : OptiFunctionBase(base, icept)
        , m_toOpti(toOpti)
		, m_iter(0)
        {
        }
        // Deduct the minimized value, based on tested input, supplied by the optimizer
		double UpdateGetValue( double arg )
        {
			++m_iter;
			*m_toOpti.var = arg; // Provide feedback to TransX's calculation functions
			double len = RecalculateGetValue();
            return len;
        }
    private:
        VarConstraint m_toOpti;
		mutable int m_iter;
};

void Optimiser::Optimise() const
{
    for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
    {
        const VarConstraint & item = m_pArgs2Find.at(i);
        if (!item.var->ShouldBeOptimised())
            continue;
        ConstraintFactory costrFact(m_base);
        Constraint constraint = costrFact.Create(item.constraintType);
        OptiFunction optiFunction(item, m_base, m_icept);

        double min_point = constraint.lower;
        double max_point = constraint.upper;
        double eps = item.var->GetOptimiserPrecision(); // Slingshot's angles need more precision
        BinSearchOpti bs(min_point, max_point, eps);
        double variableBackup = *item.var;
        Result<double> xopt = bs.Run(optiFunction);
        if (!xopt.status)
            *item.var = variableBackup;
       //    cout << "SUCCESS!" << endl;
        //else
        //    cout << "FAILURE!" << endl;
        //cout << "opti x = " << xopt.value << ", y = " << f.UpdateGetValue(xopt.value) << endl;
    }
}
