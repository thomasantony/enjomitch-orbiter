#ifndef PEGDIRECTASCENTSTD_H
#define PEGDIRECTASCENTSTD_H

#include "PEGDirectAscent.h"


class PEGDirectAscentStd : public PEGDirectAscent
{
    public:
        PEGDirectAscentStd( const VESSEL * v, const ShipVariables & vars );
        virtual ~PEGDirectAscentStd();
        double GetTargetT() const;
        double GetDeltaT() const;
        double GetCruiseEngineLevel() const;

    protected:
        void UpdateClient( MFDDataLaunchMFD * data );
        virtual Engine GetEngineCapabilities(const VESSEL * v) const;
    private:
        double GetFinalTrueLongitudeVessel( const VESSEL * v, const ORBITPARAM & op );

        double targetT, deltaT;
        const double cruiseEngineLevel;
};

#endif // PEGDIRECTASCENTSTD_H
