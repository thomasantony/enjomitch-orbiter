#ifndef OFFPLANECORRECTORSTANDARD_H
#define OFFPLANECORRECTORSTANDARD_H

#include "OffPlaneCorrector.hpp"


class OffPlaneCorrectorStandard : public OffPlaneCorrector
{
    public:
        OffPlaneCorrectorStandard();
        virtual ~OffPlaneCorrectorStandard();

        double CalculateClient( double timeToNode, MFDDataLaunchMFD * data );

    protected:
    private:
};

#endif // OFFPLANECORRECTORSTANDARD_H
