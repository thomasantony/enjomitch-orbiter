#ifndef VARIABLEWHINT_H
#define VARIABLEWHINT_H

#include "ConstraintType.h"

class MFDvarfloat;
struct VarConstraint
{
    VarConstraint(MFDvarfloat * var, ConstraintType::e type, bool startFrom0);

    MFDvarfloat * var;
    ConstraintType::e constraintType;
    bool startFrom0;
};

#endif // VARIABLEWHINT_H
