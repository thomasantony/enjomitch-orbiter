#ifndef CONSTRAINTFACTORY_H
#define CONSTRAINTFACTORY_H

#include "ConstraintType.h"

class Constraint;
class basefunction;
class ConstraintFactory
{
    public:
        ConstraintFactory(basefunction * base);
        virtual ~ConstraintFactory();
        Constraint Create(ConstraintType::e type);


    protected:
    private:
        Constraint CreateProgradeHohmann(basefunction * base);
        Constraint CreateOutward();
        Constraint CreatePlane();
        Constraint CreateAngle();

        basefunction * m_base;
};

#endif // CONSTRAINTFACTORY_H
