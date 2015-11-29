#ifndef IROOTALGO_H
#define IROOTALGO_H

#include "../../Util/Result.hpp"

namespace EnjoLib
{
    class RootSubject;

    class IRootAlgo
    {
        public:
            IRootAlgo();
            virtual ~IRootAlgo();
            virtual Result<double> Run( RootSubject & subj ) const = 0;
            virtual int GetIterations() const = 0;

        protected:
        private:
    };
}

#endif // IROOTALGO_H
