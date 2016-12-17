#include "DataSourceDummy.h"
#include <EnjoLib/ModuleMessagingExt.hpp>
#include "MFDDataBurnTime.h"

using namespace EnjoLib;

DataSourceDummy::DataSourceDummy()
: m_ibt(0)
, m_dv(0)
{}

bool DataSourceDummy::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mm;
    if (mm.ModMsgGet(GetName(), "dv", &m_dv) &&
        mm.ModMsgGet(GetName(), "InstantaneousBurnTime", &m_ibt)) {
        // Received Dummy (Launch MFD)
        data->velVector = GetVelVec(); // Testing
      return true;
    } else {
      *this = DataSourceDummy(); // Reinitialize
      return false;
    }
}
