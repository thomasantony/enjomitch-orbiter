#ifndef SCALINGOPSTANDARDIZE_H
#define SCALINGOPSTANDARDIZE_H

#include <string>

namespace EnjoLib
{
class VectorD;

class ScalingOpStandardize
{
public:
    ScalingOpStandardize(const VectorD & refVec);
    ScalingOpStandardize(double refMean = 0, double refStdDev = 1);
    ScalingOpStandardize(const std::string & str);
    double operator()(const double inVal) const;
    double GetRefMean() const { return m_refMean; }
    double GetRefStdDev() const { return m_refStdDev; }
    void SetRefMean(double mean);
    void SetRefStdDev(double stdDev);
    std::string ToStr() const;

private:
    double m_refMean;
    double m_refStdDev;
};
}

#endif // SCALINGOPSTANDARDIZE_H
