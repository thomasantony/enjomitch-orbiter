#ifndef OPTIMULTIBINSEARCH_H
#define OPTIMULTIBINSEARCH_H


#include <vector>
#include "../../Util/Result.hpp"
#include <Math/MultiDimIter/IMultiDimIterConsumerTpl.hpp>

namespace EnjoLib
{
    class OptiMultiSubject;

    class OptiMultiBinSearch : public IMultiDimIterConsumerTpl<double>
    {
        public:
            OptiMultiBinSearch();
            virtual ~OptiMultiBinSearch();

            Result<std::vector<double> > Run( OptiMultiSubject & subj, int numSlices = 6, double eps = 0.001) const;

            void Consume(const std::vector<double> & data) const override;
        protected:
        private:
           mutable std::vector<double> m_vopt;
           mutable double m_yopt = 0;
           mutable double m_epsReached = 0;
           mutable OptiMultiSubject * m_subj;

    };

}
#endif // OPTIMULTIBINSEARCH_H
