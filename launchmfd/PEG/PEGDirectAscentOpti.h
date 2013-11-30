#ifndef PEGDIRECTASCENTOPTI_H
#define PEGDIRECTASCENTOPTI_H

#include "PEGDirectAscentStd.h"

/// Serves as a subject for DA engine PID optimisation
class PEGDirectAscentOpti : public PEGDirectAscentStd
{
    public:
        PEGDirectAscentOpti( const VESSEL * v, const ShipVariables & vars );
        virtual ~PEGDirectAscentOpti();
        void SetEngineLevel( double level );
    protected:
        Engine GetEngineCapabilities(const VESSEL * v) const;
    private:
        double m_engineLevel;
};

#endif // PEGDIRECTASCENTOPTI_H
