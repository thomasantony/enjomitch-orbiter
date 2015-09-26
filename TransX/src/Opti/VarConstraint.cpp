#include "VarConstraint.h"

VarConstraint::VarConstraint(MFDvarfloat * var, ConstraintType::e type, bool startFrom0)
: var(var)
, constraintType(type)
, startFrom0(startFrom0)
{
}
