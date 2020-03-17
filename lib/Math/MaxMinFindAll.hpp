#ifndef MAXMINFINDALL_H
#define MAXMINFINDALL_H

#include <vector>
#include "MaxMinFind.hpp"

namespace EnjoLib
{
template <class T>
class MaxMinFindAll
{
    public:
        MaxMinFindAll(const std::vector<T> & data, int surrounding = 1)
        : m_surrounding(surrounding)
        {
            CalcMinMax(data);
        }
        virtual ~MaxMinFindAll(){}

        typedef std::vector<std::pair<T, int> > Extrema;

        const Extrema & GetMinima() const {return m_minima;}
        const Extrema & GetMaxima() const {return m_maxima;}

    protected:

    private:
        void CalcMinMax(const std::vector<T> & data);

        int m_surrounding = 1;
        Extrema m_minima, m_maxima;
};


template <class T>
void MaxMinFindAll<T>::CalcMinMax(const std::vector<T> & data)
{
    for (int i = m_surrounding; i < int(data.size()) - m_surrounding; ++i)
    {
        MaxMinFind<T> minMax;
        for (int j = i - m_surrounding; j < i + m_surrounding + 1; ++j)
        {
            const T & val = data.at(j);
            if (minMax.UpdateMax(val, j))
            {
                int a = 0;
                int b = a;
            }
            if (minMax.UpdateMin(val, j))
            {
                int a = 0;
                int b = a;
            }
        }
        const int maxIdx = minMax.GetMaxIdx();
        const int minIdx = minMax.GetMinIdx();
        //std::cout
        if (maxIdx == i)
        {
            m_maxima.push_back(std::make_pair(minMax.GetMax(), maxIdx));
        }
        else
        if (minIdx == i)
        {
            m_minima.push_back(std::make_pair(minMax.GetMin(), minIdx));
        }
    }
}


}

#endif // MAXMINFINDALL_H
