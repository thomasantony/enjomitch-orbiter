#include "LossFunctions.hpp"
#include "Statistical.hpp"

#include <cmath>

using namespace EnjoLib;
LossFunctions::LossFunctions(){}
LossFunctions::~LossFunctions(){}

double LossFunctions::GetMAE(const EnjoLib::VectorD & diffs) const
{
    return Statistical().ME(diffs);
}

double LossFunctions::GetRMS(const EnjoLib::VectorD & diffs) const
{
    return Statistical().RMS(diffs);
}

double LossFunctions::GetHuber(const EnjoLib::VectorD & diffs, const double delta) const
{
    // loss = np.where(np.abs(true-pred) < delta , 0.5*((true-pred)**2), delta*np.abs(true - pred) - 0.5*(delta**2))
    // return np.sum(loss)
    double sum = 0;
    for (const double dif : diffs)
    {
        const double absDif = fabs(dif);
        if (absDif < delta)
        {
            const double err = 0.5 * dif * dif;
            sum += err;
        }
        else
        {
            const double err = delta * absDif - 0.5 * (delta * delta);
            sum += err;
        }
    }
    return sum;
}

double LossFunctions::GetLogCosh(const EnjoLib::VectorD & diffs) const
{
    // loss = np.log(np.cosh(pred - true))
    // return np.sum(loss)
    double sum = 0;
    for (const double dif : diffs)
    {
        const double csh = cosh(dif);
        const double lgg = log(csh);
        sum += lgg;
    }
    return sum;
}

/*def quantile_loss(q, y, f):
  # q: Quantile to be evaluated, e.g., 0.5 for median.
  # y: True value.
  # f: Fitted (predicted) value.
  e = y - f
  return np.maximum(q * e, (q - 1) * e)
  */
double LossFunctions::GetQuantileMAE(const EnjoLib::VectorD & diffs, const double quantile) const
{
    double sum = 0;
    for (const double dif : diffs)
    {
        const double one = fabs(dif) *  quantile;
        const double two = fabs(dif) * (quantile - 1);
        //const double two = (1 - quantile) * dif; // ?
        const double maxx = std::max(one, two);
        sum += maxx;
    }
    return sum;
}


