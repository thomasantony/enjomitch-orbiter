#include "MultiDimIterTpl.hpp"
#include "IMultiDimIterConsumerTpl.hpp"

using namespace EnjoLib;


template class MultiDimIterTpl<bool>;
template class MultiDimIterTpl<int>;
template class MultiDimIterTpl<float>;
template class MultiDimIterTpl<double>;
template class MultiDimIterTpl<unsigned>;


template <class T>
void MultiDimIterTpl<T>::IterateTpl(const IMultiDimIterConsumerTpl<T> & consumer, const VVt & data, Vt & res, unsigned d)
{
    const unsigned n = data.size();
    if (d >= n)   //stop clause
    {
        consumer.Consume(res);
        return;
    }
    const Vt & dataEl = data[d];
    //const unsigned sz = dataEl.size(); // slower
    for (unsigned i = 0; i < dataEl.size(); ++i)
    {
        res[d] = dataEl[i];
        IterateTpl(consumer, data,res,d+1);
    }
}

template <class T>
void MultiDimIterTpl<T>::StartIteration(const VVt & data, const IMultiDimIterConsumerTpl<T> & consumer)
{
    Vt res(data.size());
    IterateTpl(consumer, data, res);
}
