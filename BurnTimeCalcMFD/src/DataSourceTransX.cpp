#include "DataSourceTransX.h"
#include <EnjoLib/ModuleMessagingExt.hpp>

using namespace EnjoLib;

DataSourceTransX::DataSourceTransX()
: m_ibt(0)
, m_dv(0)
, m_vel(_V(0, 0, 0))
{}

bool DataSourceTransX::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mm;
    if (mm.ModMsgGet(GetName(), "InstantaneousBurnTime", &m_ibt))
    {
        if (mm.ModMsgGet(GetName(), "dv", &m_dv))
        {
            mm.ModMsgGet(GetName(), "TargetVelocity", &m_vel); // Try getting the target vector as well
        // Received TransX
            return true;
        }
    }
    *this = DataSourceTransX(); // Reinitialize
    //m_ibt = m_dv = 0;
    return false;
}
