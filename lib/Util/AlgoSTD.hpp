#ifndef ALGOSTD_H
#define ALGOSTD_H

#include <vector>
#include <deque>

namespace EnjoLib
{
    template <class T>
    class AlgoSTD
    {
        public:
            void Sort(std::vector<T> * v);
            void Sort(std::deque<T> * v);

            void Reverse(std::vector<T> * v);
            void Reverse(std::deque<T> * v);
    };

}

#endif // ALGOSTD_H
