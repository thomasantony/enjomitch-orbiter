#include "DataSourceTransX.h"
#include <EnjoLib/ModuleMessagingExt.hpp>

using namespace EnjoLib;

DataSourceTransX::DataSourceTransX()
: m_ibt(0)
, m_dv(0)
{}

bool DataSourceTransX::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mm;
    if (mm.ModMsgGet(GetName(), "dv", &m_dv) &&
        mm.ModMsgGet(GetName(), "InstantaneousBurnTime", &m_ibt)) {
        // Received TransX
      return true;
    } else {
      m_dv = 0;
      m_ibt = 0;
      return false;
    }
}
