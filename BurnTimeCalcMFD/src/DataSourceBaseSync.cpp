#include "DataSourceBaseSync.h"
#include "MFDDataBurnTime.h"
#include "BaseSyncExports.hpp"
#include <EnjoLib/ModuleMessagingExt.hpp>

using namespace EnjoLib;

DataSourceBaseSync::DataSourceBaseSync()
: m_ibt(0)
, m_dv(0)
{}

bool DataSourceBaseSync::GetFromMM(MFDDataBurnTime * data)
{
    ModuleMessagingExt mmext;
    bool receivingBS = false;

    if (data->BS_burn != NULL) {
      receivingBS = data->BS_burn->dataValid;
    } else {
      receivingBS = mmext.ModMsgGetByRef(GetName(),"BaseSyncBurn",1,&(data->BS_burn));
      if (receivingBS) receivingBS = data->BS_burn->dataValid;
    }

    if (receivingBS) {
      m_dv = data->BS_burn->dV;
      m_ibt = data->BS_burn->tToInstBurn;
      data->BSori = data->BS_burn->orientation;
      return true;
    } else {
      m_dv = 0;
      m_ibt = 0;
      return false;
    }
}
