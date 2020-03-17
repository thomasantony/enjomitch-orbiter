#include "Correlation.hpp"
#include <sstream>

double Correlation::GetCorrelation() const
{
    return m_correlation;
}


Correlation::Correlation ( const std::vector <double> & x, const std::vector <double> & y )
    :
    m_correlation(0) // suppress warning
{
    if ( x.size() != y.size() )
    {
        std::ostringstream oss;
        oss << "Correlation::Correlation(): Sizes should be equal. ";
        oss << "Got = " << x.size() << " and " << y.size();
        throw std::length_error(oss.str());
    }
    if ( x.empty() || y.empty() )
    {
        std::ostringstream oss;
        oss << "Correlation::Correlation(): Size should be greater than 0. ";
        oss << "Got = " << x.size() << " and " << y.size();
        throw std::length_error(oss.str());
    }

    m_correlation = Calculate(x, y);
}


double Correlation::Calculate(const std::vector <double> & x, const std::vector <double> & y) const
{
    double meanX = Mean(x);
    double meanY = Mean(y);

    double sumNumerator = SumNumerator(x, y, meanX, meanY);

    double sqrtSumSquareX = SqrtSumSquare( x, meanX );
    double sqrtSumSquareY = SqrtSumSquare( y, meanY );

    return sumNumerator / (sqrtSumSquareX * sqrtSumSquareY);
}


double Correlation::Mean(const std::vector <double> & ar) const
{
    double sum = std::accumulate(ar.begin(), ar.end(), 0.0);
    return sum / (double) ar.size();
}


double Correlation::SqrtSumSquare(const std::vector <double> & ar, double mean) const
{
    double sum = 0;
    for (size_t i = 0; i < ar.size(); ++i)
    {
        double var = (ar[i] - mean);
        sum += var * var;
    }
    return std::sqrt(sum);
}


double Correlation::SumNumerator(const std::vector <double> & x, const std::vector <double> & y, double meanX, double meanY) const
{
    double sumNumerator = 0;
    for (size_t i = 0; i < x.size() && i < y.size(); ++i)
    {
        sumNumerator += (x[i] - meanX) * (y[i] - meanY);
    }
    return sumNumerator;
}
