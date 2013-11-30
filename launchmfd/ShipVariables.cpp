#include "ShipVariables.h"
#include "Autopilot/PIDAP.hpp"
#include "Utils/FileUtilsOrbiter.hpp"
#include <WinAPIFileUtils.hpp>
#include "Util/FileUtils.hpp"

using namespace EnjoLib;

ShipVariables::ShipVariables(const VESSEL * vessel, const PIDAP & pidAP )
: m_tagNextLaunchTime("NEXT_LAUNCH_TIME")
, m_tagSynchroOffByRatio("SYNCHRO_OFF_BY_RATIO")
, m_tagOldAtmoModel("OLD_ATMO_MODEL")
, m_configFileNameVar( FileUtilsOrbiter().GetVesselConfigFileName(vessel, "_variables") )
, m_configFileName( FileUtilsOrbiter().GetVesselConfigFileName(vessel) )
, m_pidAP( pidAP )
, oldAtmosphericModel( false )
{
    std::string configDir = FileUtilsOrbiter().GetVesselConfigDir(vessel);
    WinAPIFileUtils().CreateDirs( configDir );
    ReadConfig();
}

ShipVariables::~ShipVariables()
{
    SaveConfig();
}

void ShipVariables::ReadConfig()
{
    FileAccessMode famode = FILE_IN;
    FILEHANDLE fh = oapiOpenFile( m_configFileName.c_str(), famode );
    FILEHANDLE fhVar = oapiOpenFile( m_configFileNameVar.c_str(), famode );
    if ( fh && fhVar )
    {
		m_stagingContainer.ReadConfig( fh, fhVar );
		if ( ! oapiReadItem_bool( fh, (char *)m_tagOldAtmoModel.c_str(), oldAtmosphericModel ) )
            oldAtmosphericModel = false;
        if ( ! oapiReadItem_float( fhVar, (char *)m_tagNextLaunchTime.c_str(), m_next_launch_time ) )
            m_next_launch_time = 0;
        if ( ! oapiReadItem_float( fhVar, (char *)m_tagSynchroOffByRatio.c_str(), m_synchroAscentOffBy ) )
            m_synchroAscentOffBy = 0;

        oapiCloseFile(fh, famode);
        oapiCloseFile(fhVar, famode);
    }
}

void ShipVariables::SaveConfig() const
{
    FileAccessMode famode = FILE_OUT;
    FILEHANDLE fhVar = oapiOpenFile( m_configFileNameVar.c_str(), famode );
    if ( fhVar )
    {
        m_pidAP.SaveConfig( fhVar );
		m_stagingContainer.SaveConfig( fhVar );
        oapiWriteItem_float( fhVar, (char *)m_tagNextLaunchTime.c_str(), m_next_launch_time );
        oapiWriteItem_float( fhVar, (char *)m_tagSynchroOffByRatio.c_str(), m_synchroAscentOffBy );


        oapiCloseFile(fhVar, famode);
    }
    std::ofstream file(m_configFileNameVar.c_str(), std::ios::app);
    file << "\n; This file outputs variables that you've set during the simulation, as well as other collected variables.";
    file << "\n; If you want these vars to be permanent, place them in file named:";
    file << "\n; " << m_configFileName;
    file << "\n; Don't store anything in the current file, as it will be all overwritten on next simulation exit.";
    file << "\n";

    if ( ! FileUtils().FileExists(m_configFileName) )
    {
        std::ofstream file(m_configFileName.c_str());
        file << "\n; Put your variables in this file to make them permanent.";
        file << "\n; You might want to copy here some vars collected from the previous sim session from the file:";
        file << "\n; " << m_configFileNameVar;
        file << "\n";
    }
}
