#ifndef AUTIPILOTSTANDARD_HPP_INCLUDED
#define AUTIPILOTSTANDARD_HPP_INCLUDED

#include "AutopilotBase.hpp"

class AutopilotStandard : public AutopilotBase
{
public:
    bool NeedPitch( MyMFDData * data ) const;
    bool NeedYaw( MyMFDData * data ) const;
    bool NeedMECO( MyMFDData * data ) const;
    void Guide( MyMFDData * data ) const;
};

#endif // AUTIPILOTSTANDARD_HPP_INCLUDED
