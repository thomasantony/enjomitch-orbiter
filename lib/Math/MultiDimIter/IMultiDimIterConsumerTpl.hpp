#ifndef MULTIDIMITERCONSUMERTPL_H
#define MULTIDIMITERCONSUMERTPL_H

#include <vector>

namespace EnjoLib
{
    template <class T>
    class IMultiDimIterConsumerTpl
    {
        public:
            IMultiDimIterConsumerTpl(){}
            virtual ~IMultiDimIterConsumerTpl(){}

            virtual void Consume(const std::vector<T> & data) const = 0;
        protected:
        private:
    };

    typedef IMultiDimIterConsumerTpl<float> IMultiDimIterConsumerF;
}


#endif // MULTIDIMITERCONSUMERTPL_H
