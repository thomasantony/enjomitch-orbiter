#ifndef LOSSFUNCTIONS_H
#define LOSSFUNCTIONS_H

#include "VectorD.hpp"

namespace EnjoLib
{
class LossFunctions
{
    public:
        LossFunctions();
        virtual ~LossFunctions();

        double GetMAE(const EnjoLib::VectorD & diffs) const;
        double GetRMS(const EnjoLib::VectorD & diffs) const;
        double GetHuber(const EnjoLib::VectorD & diffs, const double delta) const;
        double GetLogCosh(const EnjoLib::VectorD & diffs) const;
        double GetQuantileMAE(const EnjoLib::VectorD & diffs, const double quantile) const;

    protected:

    private:
};
}

#endif // LOSSFUNCTIONS_H
