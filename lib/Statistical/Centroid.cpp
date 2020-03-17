#include "Centroid.hpp"
#include "VectorD.hpp"
#include "VectorF.hpp"
#include <stdexcept>

using namespace EnjoLib;

template class Centroid<VectorD>;
template class Centroid<VectorF>;
template class Centroid<std::vector<double>>;
template class Centroid<std::vector<float>>;

template<class T>
Centroid<T>::Centroid(){}
template<class T>
Centroid<T>::~Centroid(){}

template<class T>
T Centroid<T>::Calc(const std::vector<T> & vec) const
{
    try
    {
        unsigned numDims = vec.at(0).size();
        T centr(numDims);
        for (unsigned n = 0; n < vec.size(); ++n)
        {
            for (unsigned dim = 0; dim < numDims; ++dim)
            {
                centr.at(dim) += vec.at(n).at(dim);
            }
        }
        for (unsigned dim = 0; dim < numDims; ++dim)
        {
            centr.at(dim) /= double(vec.size());
        }
        return centr;
    }
    catch (std::exception & ex)
    {
        throw std::runtime_error(std::string("Centroid: Wrong size of input vector ") + ex.what());
    }
}
