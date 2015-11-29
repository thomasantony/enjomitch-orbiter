#ifndef ROOTFACTORY_H
#define ROOTFACTORY_H

#include <memory>
#include "IRootAlgo.hpp"
#include "RootType.hpp"

#include <Util/AutoPtr.hpp>

namespace EnjoLib
{
    class RootFactory
    {
        public:
            RootFactory();
            virtual ~RootFactory();

            static std::unique_ptr<IRootAlgo> Create(RootType type, double minArg, double maxArg, double eps);

        protected:
        private:
    };
}

#endif // OPTIFACTORY_H
