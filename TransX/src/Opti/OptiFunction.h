#ifndef OPTIFUNCTION_H
#define OPTIFUNCTION_H

#include <Math/BinSearchOptiSubject.hpp>
#include "VarConstraint.h"
class basefunction;
class Intercept;

// Base for the "Optimisation Problem". Could be used for multiple dimension optimisation
class OptiFunctionBase
{
    public:
        OptiFunctionBase(basefunction * base, Intercept * icept)
           : m_base(base)
           , m_icept(icept)
        {}
    protected:
        double RecalculateGetValue();

    private:
        basefunction * m_base;
        Intercept * m_icept;
};

// Defines the one dimensional "Optimisation Problem"
class OptiFunction : public OptiFunctionBase, public EnjoLib::BinSearchOptiSubject
{
    public:
        OptiFunction(VarConstraint toOpti, basefunction * base, Intercept * icept)
        : OptiFunctionBase(base, icept)
        , m_toOpti(toOpti)
        {
        }
        double UpdateGetValue( double arg );
    private:
        VarConstraint m_toOpti;
};

#endif // OPTIFUNCTION_H
