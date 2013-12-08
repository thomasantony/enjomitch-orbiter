#include "VarConstraint.h"

VarConstraint::VarConstraint(MFDvarfloat * var, ConstraintType::e type)
: var(var)
, constraintType(type)
{
}
