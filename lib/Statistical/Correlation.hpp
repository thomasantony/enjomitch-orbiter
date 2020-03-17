#ifndef CORRELATION_HPP_INCLUDED
#define CORRELATION_HPP_INCLUDED

#include <vector>
#include <stdexcept>
#include <numeric>
#include <cmath>

/**
http://en.wikipedia.org/wiki/Pearson_product-moment_correlation_coefficient
*/
class Correlation
{
public:
    Correlation( const std::vector <double> & x, const std::vector <double> & y);
    double GetCorrelation() const;

private:
    double Calculate(const std::vector <double> & x, const std::vector <double> & y) const;
    double Mean(const std::vector <double> & ar) const;
    double SqrtSumSquare(const std::vector <double> & ar, double mean) const;
    double SumNumerator(const std::vector <double> & x, const std::vector <double> & y, double meanX, double meanY) const;

    double m_correlation;
};

#endif // CORRELATION_HPP_INCLUDED
