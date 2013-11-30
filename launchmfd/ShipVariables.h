#ifndef SHIPVARIABLES_H
#define SHIPVARIABLES_H

#include "StagingContainer.hpp"
#include "Util/NonCopyable.hpp"

class PIDAP;

class ShipVariables : public EnjoLib::NonCopyable
{
    public:
        ShipVariables( const VESSEL * vessel, const PIDAP & m_pidAP );
        virtual ~ShipVariables();

		double m_next_launch_time;
		double m_synchroAscentOffBy;
		bool oldAtmosphericModel;
		StagingContainer m_stagingContainer;

    protected:

    private:
		void ReadConfig();
        void SaveConfig() const;

		const std::string m_configFileName;
		const std::string m_configFileNameVar;
		const std::string m_tagSynchroOffByRatio;
		const std::string m_tagNextLaunchTime;
		const std::string m_tagOldAtmoModel;
		const PIDAP & m_pidAP;
};

#endif // SHIPVARIABLES_H
