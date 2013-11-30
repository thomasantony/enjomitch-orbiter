#ifndef AUTIPILOTSOPHISTICATED_HPP_INCLUDED
#define AUTIPILOTSOPHISTICATED_HPP_INCLUDED

#include "AutopilotBase.hpp"

class AutopilotSophisticated : public AutopilotBase
{
public:
    bool NeedPitch( MyMFDData * data ) const;
    bool NeedYaw( MyMFDData * data ) const;
    bool NeedMECO( MyMFDData * data ) const;
    void Guide( MyMFDData * data ) const;
};

#endif // AUTIPILOTSOPHISTICATED_HPP_INCLUDED
