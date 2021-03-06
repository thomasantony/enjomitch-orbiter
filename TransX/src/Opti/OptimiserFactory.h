#ifndef OPTIMISERFACTORY_H
#define OPTIMISERFACTORY_H

#include <memory>
#include <vector>
#include "VarConstraint.h"

class basefunction;
class Intercept;
class Optimiser;

/// Creates optimizers
/**
Should be created on stack, and not stored, due to storing pointers
*/
class OptimiserFactory
{
    public:
        OptimiserFactory(basefunction * base, Intercept * icept);
        virtual ~OptimiserFactory();

        std::auto_ptr<Optimiser> Create(const std::vector<VarConstraint> & pArgs2Find);
        std::auto_ptr<Optimiser> Create(VarConstraint pArg2Find);
        std::auto_ptr<Optimiser> CreateDummy();

    protected:
    private:
        basefunction * m_base;
        Intercept * m_icept;
};

#endif // OPTIMISERFACTORY_H
