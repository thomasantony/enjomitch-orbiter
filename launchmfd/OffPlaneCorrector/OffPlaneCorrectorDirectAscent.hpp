#ifndef OFFPLANECORRECTORDIRECTASCENT_H
#define OFFPLANECORRECTORDIRECTASCENT_H

#include "OffPlaneCorrector.hpp"


class OffPlaneCorrectorDirectAscent : public OffPlaneCorrector
{
    public:
        OffPlaneCorrectorDirectAscent();
        virtual ~OffPlaneCorrectorDirectAscent();

        double CalculateClient( double timeToNode, MFDDataLaunchMFD * data );

    protected:
    private:
        bool IsAbovePlane( double timeToNode, MFDDataLaunchMFD * data ) const;
};

#endif // OFFPLANECORRECTORDIRECTASCENT_H
