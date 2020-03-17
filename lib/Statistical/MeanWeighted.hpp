#ifndef MEANWEIGHTED_H
#define MEANWEIGHTED_H

#include <vector>

namespace EnjoLib
{
class MeanWeighted
{
    public:
        MeanWeighted();
        virtual ~MeanWeighted();

        void AddValWeight(double val, double weight);
        void AddValDist  (double val, double dist);
        double GetMean() const;
    protected:

    private:
        std::vector<double> m_vals, m_weights;
};
}

#endif // MEANWEIGHTED_H
