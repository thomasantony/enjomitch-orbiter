#include "HUDDrawer.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include <Orbiter/SpaceMathOrbiter.hpp>

using namespace EnjoLib;

HUDDrawer::HUDDrawer()
: m_data(NULL)
{
}

HUDDrawer::~HUDDrawer()
{
}

void HUDDrawer::SetMFDData( MFDDataLaunchMFD * data )
{
    m_data = data;
}

const char * HUDDrawer::GetModuleName() const
{
    return "Launch MFD";
}

bool HUDDrawer::ShouldDrawHUD() const
{
    if (m_data)
        return m_data->hud && m_data->GetVessel() == oapiGetFocusInterface();
    else
        return false;
}

void HUDDrawer::DrawHUD(int mode, const HUDPAINTSPEC *hps, MyDC hDC)
{
    const VESSEL * v = m_data->GetVessel();
    const TGTPARAM & target = m_data->GetTgtParam();
    HUD::SetScale(hps);
    m_data->SetPEGApses();
    // The following call is controversial, because if autopilot is enabled, the data is updated anyway.
    // Secondly, the user may want to control the data update frequency with MFD refresh rate
    // However experiments show that there's little difference in runtime.
    m_data->Update();
    const double azimuth = m_data->GetAzimuth();
    if(oapiGetHUDMode() == HUD_SURFACE)
	{
		HUD.SetParams(azimuth, SpaceMathOrbiter().GetHeadBearing(v), target.azFlag);
		HUD.DrawHUD(mode, hps, hDC);
	}

    const double targetPitch = m_data->drawPitchError ? m_data->GetTargetPitch() : v->GetPitch();

    if(target.azFlag == NORTHERN_AZIMUTH || target.azFlag == BOTH_AZIMUTHS)
    {
        m_data->flightDirector.SetParams(targetPitch, azimuth);
        m_data->flightDirector.DrawHUD(mode, hps, hDC);
    }
    // We may need to draw the descending azimuth on top of the ascending one
    if(target.azFlag == SOUTHERN_AZIMUTH || target.azFlag == BOTH_AZIMUTHS)
    {
        m_data->flightDirector.SetParams(targetPitch, PI - azimuth);
        m_data->flightDirector.DrawHUD(mode, hps, hDC);
    }
}
