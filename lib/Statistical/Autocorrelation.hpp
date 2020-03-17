#ifndef AUTOCORRELATION_HPP
#define AUTOCORRELATION_HPP

#include "VectorD.hpp"

namespace EnjoLib
{
class Autocorrelation
{
    public:
        Autocorrelation();
        virtual ~Autocorrelation();

        VectorD Calc(const VectorD & data, unsigned idx, unsigned period) const;

    protected:

    private:
};
}

#endif // AUTOCORRELATION_HPP
