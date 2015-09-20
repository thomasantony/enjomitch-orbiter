#ifndef OPTIFACTORY_H
#define OPTIFACTORY_H

#include <memory>
#include "IOptiAlgo.hpp"
#include "OptiType.hpp"

namespace EnjoLib
{
    class OptiFactory
    {
        public:
            OptiFactory();
            virtual ~OptiFactory();

            static std::auto_ptr<IOptiAlgo> Create(OptiType type, double minArg, double maxArg, double eps);

        protected:
        private:
    };
}

#endif // OPTIFACTORY_H
