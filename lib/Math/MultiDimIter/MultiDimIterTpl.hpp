#ifndef MULTIDIMITERTPL_H
#define MULTIDIMITERTPL_H

#include <vector>
namespace EnjoLib
{
template <class T> class IMultiDimIterConsumerTpl;

template <class T>
class MultiDimIterTpl
{
    public:
        MultiDimIterTpl(){}
        virtual ~MultiDimIterTpl(){}

        typedef std::vector<T> Vt;
        typedef std::vector<Vt> VVt;

        void StartIteration(const std::vector<std::vector<T>> & data, const IMultiDimIterConsumerTpl<T> & consumer);
    protected:
    private:
        void IterateTpl(const IMultiDimIterConsumerTpl<T> & consumer, const VVt & data, Vt & res, unsigned d = 0);


};
}

#endif // MULTIDIMITERTPL_H
