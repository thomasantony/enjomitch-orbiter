#include "Optimiser.h"
#include "basefunction.h"

#include "dlib/optimization.h"
using namespace std;
using namespace dlib;
typedef dlib::matrix<double,0,1> column_vector;

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
        OptiFunction(basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find)
        : m_pArgs2Find(pArgs2Find)
        , m_base(base)
        , m_icept(icept)
        {
        }

        // Deduct the minimized value, based on tested input, supplied by the optimized
        double operator()(const column_vector& input) const
        {
            // Initialize the basefunction's calculation variables with input
            for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
            {
                double x = input(i);
                *m_pArgs2Find.at(i) = x;
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
        double * m_value;
        std::vector<MFDvarfloat*> m_pArgs2Find;
};

void Optimiser::Optimise() const
{
    column_vector starting_point(m_pArgs2Find.size());
    bfgs_search_strategy stratSearch;
    objective_delta_stop_strategy stratStop(1e-7, 50);
    OptiFunction optiFunction(m_base, m_icept, m_pArgs2Find);
    //for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
    //    starting_point(i) = *m_pArgs2Find.at(i);
    for (size_t i = 0; i < m_pArgs2Find.size(); ++i)
        starting_point(i) = 1.01;

    // The starting point will be modified by the optimiser
    find_min_using_approximate_derivatives
    (stratSearch, stratStop, optiFunction, starting_point, -1);
}
