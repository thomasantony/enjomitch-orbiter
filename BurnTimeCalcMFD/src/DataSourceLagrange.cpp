#include "DataSourceLagrange.h"
#include <EnjoLib/ModuleMessagingExt.hpp>

using namespace EnjoLib;

DataSourceLagrange::DataSourceLagrange()
: m_ibt(0)
, m_vel(_V(0, 0, 0))
{
    //ctor
}

bool DataSourceLagrange::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mm;
    if (mm.ModMsgGet(GetName(), "TargetVelocity", &m_vel) &&
        mm.ModMsgGet(GetName(), "InstantaneousBurnTime", &m_ibt)) {
        // Received LagrangeMFD
      return true;
    } else {
      *this = DataSourceLagrange(); // Reinitialize
      return false;
    }
}
