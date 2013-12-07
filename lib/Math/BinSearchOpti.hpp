#ifndef BINSEARCHOPTI_H
#define BINSEARCHOPTI_H

#include "../Util/Result.hpp"
#include <vector>
#include "../Statistical/Vector.hpp"
namespace EnjoLib
{
    class BinSearchOptiSubject;
    class BinSearchOpti
    {
        public:
            BinSearchOpti(Vector minArg, Vector maxArg, double eps);
            virtual ~BinSearchOpti();
            Result<Vector> Run( BinSearchOptiSubject & subj ) const;
        protected:
        private:

            Vector m_minArg, m_maxArg;
            double m_eps;
            bool m_maxArgValPositive;
            int m_maxIter;
    };
}
#endif // BINSEARCHOPTI_H
