#include "AlgoSTD.hpp"
#include <algorithm>
#include <string>

using namespace std;
using namespace EnjoLib;
template <class T>
void AlgoSTD<T>::Sort(std::vector<T> * v)
{
    std::sort(v->begin(), v->end());
}

template <class T>
void AlgoSTD<T>::Sort(std::deque<T> * v)
{
    std::sort(v->begin(), v->end());
}

template <class T>
void AlgoSTD<T>::Reverse(std::vector<T> * v)
{
    std::reverse(v->begin(), v->end());
}

template <class T>
void AlgoSTD<T>::Reverse(std::deque<T> * v)
{
    std::reverse(v->begin(), v->end());
}

template class AlgoSTD<string>;
template class AlgoSTD<float>;
template class AlgoSTD<double>;
template class AlgoSTD<int>;
template class AlgoSTD<unsigned long>;
template class AlgoSTD<unsigned long long>;
