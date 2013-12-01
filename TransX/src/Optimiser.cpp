#include "Optimiser.h"
#include "basefunction.h"

#include "dlib/optimization.h"
using namespace std;
using namespace dlib;
typedef dlib::matrix<double,0,1> column_vector;

const double Optimiser::m_cdefaultMin = -3.5e3;
const double Optimiser::m_cdefaultMax = +3.5e3;
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

class OptiFunction
{
    public:
        OptiFunction(double * toOpti, basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find)
        : m_pArgs2Find(pArgs2Find)
        , m_base(base)
        , m_icept(icept)
        , m_toOpti(toOpti)
        {
        }

        // Deduct the minimized value, based on tested input, supplied by the optimizer
        double operator()(const column_vector& input) const
        {
            // Initialize the basefunction's calculation variables with input
            for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
            {
                double x = input(i);
                //m_pArgs2Find.at(i)->setvalue(x);
                *m_toOpti = x;
            }
            VECTOR3 targetVecUnused;
            m_base->calculate(&targetVecUnused);
            VECTOR3 craftpos, targetpos;
            m_icept->getpositions(&craftpos,&targetpos);
            double len = length(craftpos-targetpos);
            return len;
        }
    private:
        basefunction * m_base;
        Intercept * m_icept;
        double * m_toOpti;
        std::vector<MFDvarfloat*> m_pArgs2Find;
};

void Optimiser::Optimise( double * toOpti ) const
{
    const size_t sz = m_pArgs2Find.size();
    column_vector starting_point(sz);
    column_vector variablesBackup(sz); // will revert to this
    column_vector min_point(sz);
    column_vector max_point(sz);
    bfgs_search_strategy stratSearch;
    objective_delta_stop_strategy stratStop(1e-7, 30);
    OptiFunction optiFunction(toOpti, m_base, m_icept, m_pArgs2Find);
    //for (size_t i = 0; i < sz; ++i)
    //    starting_point(i) = *m_pArgs2Find.at(i);
    for (size_t i = 0; i < sz; ++i)
    {
        variablesBackup(i) = *toOpti;
        if (m_pArgs2Find.at(i)->GetHohmannConstraintHint())
        {
            const double hohmanDV = m_base->GetHohmannDV();
            starting_point(i) = hohmanDV;
            min_point(i) = hohmanDV * (1 - m_cdefaultRatioHohmann);
            max_point(i) = hohmanDV * (1 + m_cdefaultRatioHohmann);
        }
        else
        {
            starting_point(i) = 1.01;
            min_point(i) = m_cdefaultMin;
            max_point(i) = m_cdefaultMax;
        }
    }


    // The starting point will be modified by the optimiser
	//find_min_using_approximate_derivatives
    //(stratSearch, stratStop, optiFunction, starting_point, -1);
    find_min_using_approximate_derivatives
    (stratSearch, stratStop, clamped_function_object<OptiFunction,column_vector,column_vector> (optiFunction, min_point, max_point), starting_point, -1);
    bool success = true;
    for (size_t i = 0; i < sz; ++i)
    {
        if (m_pArgs2Find.at(i)->GetHohmannConstraintHint())
        {
            // Extension over this limit is always OK
			int a = 1;
        }
        //else
        {
            const double x = starting_point(i);
            const double xmin = min_point(i);
            const double xmax = max_point(i);
			const double ratioMin = fabs((x ) / xmin);
			const double ratioMax = fabs((x ) / xmax);
			if ( x > 0 )
			{
				if (ratioMax > 0.95)
					success = false;
			}
			else
			{
				if (ratioMin > 0.95)
					success = false;
			}
        }
    }
    if (!success)
    {
        // The optimizer failed. Revert values
        for (size_t i = 0; i < sz; ++i)
            *toOpti = variablesBackup(i);
    }

}
