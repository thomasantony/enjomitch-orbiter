#include "ConstraintFactory.h"
#include "Constraint.h"
#include "../basefunction.h"
#include <Math/Constants.hpp>

static const double precisionVel = 0.001;
//static const double precisionVel = 0.00001;
static const double precisionAngle = 0.000001;
//static const double precisionAngle = 0.000001;

ConstraintFactory::ConstraintFactory(basefunction * base)
: m_base(base)
{
}

ConstraintFactory::~ConstraintFactory()
{
}

Constraint ConstraintFactory::Create(ConstraintType::e type)
{
    switch (type)
    {
    case ConstraintType::PROGRADE_HOHMANN:
        return CreateProgradeHohmann(m_base);
    case ConstraintType::PROGRADE_MANOEUVRE:
        return CreateProgradeManoeuvre(m_base);
    case ConstraintType::CHANGE_PLANE:
        return CreatePlane();
    case ConstraintType::OUTWARD:
        return CreateOutward();
    case ConstraintType::ANGLE:
        return CreateAngle();
    }
    return Constraint(0, 0, precisionVel);
}

Constraint ConstraintFactory::CreateOutward()
{
    // Outward velocity requires a lot of energy. Keep it small
    const double lower = -3000;
    const double upper = +3000;
    return Constraint(lower, upper, precisionVel);
}

Constraint ConstraintFactory::CreatePlane()
{
    const double lower = -4.5e3;
    const double upper = +4.5e3;
    return Constraint(lower, upper, precisionVel);
}

Constraint ConstraintFactory::CreateAngle()
{
    const double lower = -180 * RAD;
    const double upper = +180 * RAD;
    return Constraint(lower, upper, precisionAngle);
}

Constraint ConstraintFactory::CreateProgradeHohmann(basefunction * base)
{
    const double defaultRatioHohmann = 0.15;
    const double hohmanDV = base->GetHohmannDV();
    const double lower = hohmanDV * (1 - defaultRatioHohmann);
    const double upper = hohmanDV * (1 + defaultRatioHohmann);

    return Constraint(lower, upper, precisionVel);
}

// Doesn't work very well
Constraint ConstraintFactory::CreateProgradeManoeuvre(basefunction * base)
{
    const double defaultRatioHohmann = 0.05;
    const double hohmanDV = base->GetHohmannDV();
    const double lower = 0;//hohmanDV * (1 - defaultRatioHohmann);
    const double upper = hohmanDV * (1 + defaultRatioHohmann);
    return Constraint(lower, upper, precisionVel);
}

