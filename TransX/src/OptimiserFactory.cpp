#include "OptimiserFactory.h"
#include "Optimiser.h"

OptimiserFactory::OptimiserFactory(basefunction * base, Intercept * icept)
: m_base(base)
, m_icept(icept)
{
}

OptimiserFactory::~OptimiserFactory()
{
}

std::auto_ptr<Optimiser*> OptimiserFactory::Create(const std::vector<MFDvarfloat*> & pArgs2Find)
{
    return new Optimiser(m_base, m_icept, pArgs2Find);
}
std::auto_ptr<Optimiser*> OptimiserFactory::Create(MFDvarfloat* pArg2Find)
{
    return new Optimiser(m_base, m_icept, pArg2Find);
}
