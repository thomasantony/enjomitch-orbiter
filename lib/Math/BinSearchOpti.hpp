#ifndef BINSEARCHOPTI_H
#define BINSEARCHOPTI_H

#include "../Util/Result.hpp"

namespace EnjoLib
{
    class BinSearchOptiSubject;
    class BinSearchOpti
    {
        public:
            BinSearchOpti(double minArg, double maxArg, double eps);
            virtual ~BinSearchOpti();
            Result<double> Run( BinSearchOptiSubject & subj ) const;
        protected:
        private:

            double m_minArg, m_maxArg, m_eps;
            bool m_maxArgValPositive;
            int m_maxIter;
    };
}
#endif // BINSEARCHOPTI_H
