#define STRICT
#include "OptimiserVar.h"
#include "basefunction.h"

#include <dlib/optimization.h>
using namespace std;
using namespace dlib;
typedef matrix<double,0,1> column_vector;

OptimiserVar::OptimiserVar()
{
    //ctor
}

OptimiserVar::~OptimiserVar()
{
    //dtor
}

void OptimiserVar::Init( basefunction * base, Intercept * icept )
{
    m_base = base;
    m_icept = icept;
}

class Rosen
{
    public:
        Rosen(basefunction * base, Intercept * icept, double * value)
        {
            m_base = base;
            m_icept = icept;
            m_value = value;
        }
        double operator()(const column_vector& m) const
        {
            const double x = m(0);
            *m_value = x;
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
};

double OptimiserVar::GetOpti( double * currentValue ) const
{
    column_vector starting_point(1);
    bfgs_search_strategy stratSearch;
    objective_delta_stop_strategy stratStop(1e-7, 30);
    starting_point = *currentValue;

    find_min_using_approximate_derivatives
    (stratSearch, stratStop, Rosen(m_base, m_icept, currentValue), starting_point, -1);

    return starting_point;
}

