#include "OptiBrent.hpp"
#include "OptiSubject.hpp"
#include "../GeneralMath.hpp"
#include <cmath>
#include <utility>
#include "../Burkardt/BrentBurkardt.hpp"
#include "../Burkardt/FuncBase.hpp"

using namespace EnjoLib;

OptiBrent::OptiBrent(double minArg, double maxArg, double eps)
{
    if (minArg > maxArg)
        std::swap(minArg, maxArg);

    this->m_minArg = minArg;
    this->m_maxArg = maxArg;
    this->m_eps = eps;
    // Bound binary seach should finish in log2(n) iterations. Let's allow for max 2 logs.
	GeneralMath gm;
    const double numSlices = gm.round( (maxArg-minArg) / eps);
    this->m_maxIter = 2 * gm.round(gm.Log2(numSlices));
}

OptiBrent::~OptiBrent(){}

Result<double> OptiBrent::Run( OptiSubject & subj ) const
{
    BrentBurkardt bkrd;
    double y = 0;
    return bkrd.local_min(m_minArg, m_maxArg, m_eps, m_maxIter, subj, y);
}