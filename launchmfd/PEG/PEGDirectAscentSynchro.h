#ifndef PEGDIRECTASCENTSYNCHRO_H
#define PEGDIRECTASCENTSYNCHRO_H

#include "PEGDirectAscent.h"


class PEGDirectAscentSynchro : public PEGDirectAscent
{
    public:
        PEGDirectAscentSynchro( const VESSEL * v, const ShipVariables & vars );
        virtual ~PEGDirectAscentSynchro();

        double GetDeltaT() const;
        double GetTotalDV() const;
        double GetTimeToBurn() const;
        double GetBurnT() const;
        double GetBurnT2() const;

    protected:
        void UpdateClient( MFDDataLaunchMFD * data );
        virtual Engine GetEngineCapabilities(const VESSEL * v) const;
    private:

        double synchroDVTotal, synchroTimeToBurn, synchroBurnT, synchroBurnT2;
        const double m_defaultOffByRatio;
};

#endif // PEGDIRECTASCENTSYNCHRO_H
