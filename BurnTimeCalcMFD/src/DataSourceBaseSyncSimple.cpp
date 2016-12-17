#include "DataSourceBaseSyncSimple.h"
#include <EnjoLib/ModuleMessagingExt.hpp>

using namespace EnjoLib;

DataSourceBaseSyncSimple::DataSourceBaseSyncSimple()
: m_ibt(0)
, m_dv(0)
, m_orientation(0)
{}

bool DataSourceBaseSyncSimple::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mm;
    if (mm.ModMsgGet(GetName(), "dv", &m_dv) &&
        mm.ModMsgGet(GetName(), "InstantaneousBurnTime", &m_ibt) &&
        mm.ModMsgGet(GetName(), "Orientation", &m_orientation)
        ) {
        // Got everything
      return true;
    } else {
      *this = DataSourceBaseSyncSimple(); // Reinitialize
      return false;
    }
}

std::string DataSourceBaseSyncSimple::GetDisplayStringDV(MFDDataBurnTime * data) const
{
    switch (m_orientation)
    {
        case -1:
            return("BaseSync Nml- dV:      %7.3f");
        case 0:
            return("BaseSync Retro dV:     %7.3f");
        case 1:
            return("BaseSync Nml+ dV:      %7.3f");
    }
    return "BaseSync inv. orient.  %7.3f";
}
