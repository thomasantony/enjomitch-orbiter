#include "Autocorrelation.hpp"
#include "Correlation.hpp"
#include <iostream>

using namespace std;
using namespace EnjoLib;

Autocorrelation::Autocorrelation()
{
    //ctor
}

Autocorrelation::~Autocorrelation()
{
    //dtor
}

VectorD Autocorrelation::Calc(const VectorD & tss, unsigned idx, unsigned period) const
{
    VectorD ret;
    //return tss;
    const VectorD & sample = tss.Slice(idx, period);

    for (size_t i = idx - period; i < idx; ++i)
    {
        //cout << "i = " << i << endl;
        const VectorD & slice = tss.Slice(i, period);
        const Correlation corr(sample, slice);
        const double corVal = corr.GetCorrelation();
        ret.Add(corVal);
    }

    return ret;
}
