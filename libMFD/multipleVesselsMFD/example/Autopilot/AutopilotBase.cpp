#include "AutopilotBase.hpp"
#include "../MyMFDData.hpp"

AutopilotBase::AutopilotBase()
{

}

bool AutopilotBase::IsLaunched( MyMFDData * data ) const
{
  VESSELSTATUS2 vs2;
  memset(&vs2, 0, sizeof(VESSELSTATUS2));
  vs2.version = 2;
  data->GetVessel()->GetStatusEx(&vs2);

  return vs2.status == 0;
}

void AutopilotBase::Guide( MyMFDData * data ) const
{
    if (!IsLaunched( data ) )
      return;

    data->Update();

	// Common code, like the following:
/*
    double targetPitch;
    if ( NeedPitch( data ) )
        targetPitch = data->ascentPitchPredictor.GetTargetPitch();
    else
        targetPitch = data->GetVessel()->GetPitch();

	double azimuth;
    if ( NeedYaw( data ) )
        azimuth = data->calculate_azimuth();
    else
        // Artificially get straight line azimuth
        azimuth = data->GetYaw();

	data->Set(targetPitch, azimuth);

	*/
	// For a working example, refer to LaunchMFD source

}
