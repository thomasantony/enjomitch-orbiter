#ifndef VARIABLEWHINT_H
#define VARIABLEWHINT_H

#include "ConstraintType.h"

class MFDvarfloat;
struct VarConstraint
{
    VarConstraint(MFDvarfloat * var, ConstraintType::e type);

    MFDvarfloat * var;
    ConstraintType::e constraintType;
};

#endif // VARIABLEWHINT_H
