#ifndef SPLINELIN_H
#define SPLINELIN_H

#include <vector>
#include <utility>
#include "../Systems/Point.hpp"

namespace EnjoLib
{
class SplineLin
{
    public:
        SplineLin(const std::vector<double> & in);
        virtual ~SplineLin();

        double Interpol(double x) const;

        std::vector<double> Scale(int outSize) const;

        std::vector<double> ScaleUp(int outSize) const;
        std::vector<double> ScaleDown(int outSize) const;

    protected:

    private:
        int GetSplineSize() const;
        std::vector<std::pair<EnjoLib::Point, EnjoLib::Point> > spline;
};
}
#endif // SPLINELIN_H
