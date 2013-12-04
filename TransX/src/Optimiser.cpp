#include "Optimiser.h"
#include "basefunction.h"
#include <Math/BinSearchOptiSubject.hpp>
#include <Math/BinSearchOpti.hpp>
using namespace std;
using namespace EnjoLib;

const double Optimiser::m_cdefaultMin = -4.5e3;
const double Optimiser::m_cdefaultMax = +4.5e3;
const double Optimiser::m_cdefaultRatioHohmann = 0.5;

Optimiser::Optimiser(basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find)
{
	m_pArgs2Find = pArgs2Find;
    m_base = base;
    m_icept = icept;
}

Optimiser::Optimiser(basefunction * base, Intercept * icept, MFDvarfloat* pArg2Find)
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
        OptiFunction(MFDvarfloat * toOpti, basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find)
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

			*m_toOpti = arg;
			//*m_pArgs2Find.at(0) = arg;
            VECTOR3 targetVecUnused;
            for (int i = 0; i < 10; ++i) // needs at least 7 iterations to converge
                m_base->calculate(&targetVecUnused);
            VECTOR3 craftpos, targetpos;
            m_icept->getpositions(&craftpos,&targetpos);
            double len = length(craftpos-targetpos);
            return len;
        }
    private:
        basefunction * m_base;
        Intercept * m_icept;
        MFDvarfloat * m_toOpti;
        std::vector<MFDvarfloat*> m_pArgs2Find;
		mutable int m_iter;
};

void Optimiser::Optimise() const
{
    for (size_t i = 0; i < m_pArgs2Find.size(); ++i) // quick and dirty
    {
        double starting_point;
        double variablesBackup; // will revert to this
        double min_point;
        double max_point;
        OptiFunction optiFunction(m_pArgs2Find.at(i), m_base, m_icept, m_pArgs2Find);
        //for (size_t i = 0; i < sz; ++i)
        //    starting_point(i) = *m_pArgs2Find.at(i);

        variablesBackup = *m_pArgs2Find.at(i);
        if (m_pArgs2Find.at(i)->GetHohmannConstraintHint())
        {
            const double hohmanDV = m_base->GetHohmannDV();
            starting_point = hohmanDV;
            min_point = hohmanDV * (1 - m_cdefaultRatioHohmann);
            max_point = hohmanDV * (1 + m_cdefaultRatioHohmann);
        }
        else
        {
            starting_point = 1.01;
            min_point = m_cdefaultMin;
            max_point = m_cdefaultMax;
        }
        BinSearchOpti bs(min_point, max_point, 0.001);
        Result<double> xopt = bs.Run(optiFunction);
        //if (xopt.status)
       //    cout << "SUCCESS!" << endl;
        //else
        //    cout << "FAILURE!" << endl;
        //cout << "opti x = " << xopt.value << ", y = " << f.UpdateGetValue(xopt.value) << endl;
    }
}
