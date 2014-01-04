#include <orbitersdk.h>
#include "Autopilot.h"
#include "mfd.h"
#include <Orbiter/AngularAcc.hpp>
#include <Orbiter/SystemsConverterOrbiter.hpp>

using namespace EnjoLib;
extern VECTOR3 gTargetVec;
const Vect3 Autopilot::m_statDeltaGliderRefRotAcc(0.125, 0.066, 0.189);

Autopilot::Autopilot()
: m_pidAPSpaceX(0.8, 5)
, m_pidAPSpaceY(m_pidAPSpaceX)
, m_pidAPSpaceBank(1, 12)
{
    m_isEnabled = false;
}

Autopilot::~Autopilot(){}

void Autopilot::Update(double SimDT)
{
    if (gTargetVec.x + gTargetVec.y + gTargetVec.z == 0)
    {
        if (IsEnabled())
            Enable(false);
        //sprintf(oapiDebugString(), "TransX: AUTO rotation disabled. Press SHIFT+C in target view to enable");
        return;
    }
    Enable(true);
    //sprintf(oapiDebugString(), "TransX: AUTO rotation ENABLED!");
    VESSEL * vessel = oapiGetFocusInterface();
    if (!vessel)
        return;
    VECTOR3 angleToTarget = GetRotationToTarget(vessel, unitise(gTargetVec));

    const VECTOR3 accRatio = GetVesselAngularAccelerationRatio(vessel);

    const double inputBank = vessel->GetBank() / PI;
    const double b = accRatio.z * m_pidAPSpaceBank.Update( inputBank, SimDT );
    const double x = accRatio.x * m_pidAPSpaceX.Update( angleToTarget.x, SimDT );
    const double y = accRatio.y * m_pidAPSpaceY.Update( angleToTarget.y, SimDT );

    vessel->SetAttitudeRotLevel( 2, b );
    vessel->SetAttitudeRotLevel( 1, -x );
    vessel->SetAttitudeRotLevel( 0, y );
}

void Autopilot::Enable(bool val)
{
    m_isEnabled = val;
    OnDisabled();
}
void Autopilot::SwitchEnabled()
{
    m_isEnabled = !m_isEnabled;
    OnDisabled();
}
bool Autopilot::IsEnabled()
{
    return m_isEnabled;
}
void Autopilot::OnDisabled()
{
    VESSEL * vessel = oapiGetFocusInterface();
    if (!vessel)
        return;
    if (!IsEnabled())
    {
        vessel->SetAttitudeRotLevel( _V(0, 0, 0) );
        vessel->ActivateNavmode( NAVMODE_KILLROT );
    }
}

VECTOR3 Autopilot::GetVesselAngularAccelerationRatio( const VESSEL * vessel )
{
    VECTOR3 accRatio;
    //if ( m_rotationRatioNeeded )
    {
        VECTOR3 refMaxAngAcc = SystemsConverterOrbiter(m_statDeltaGliderRefRotAcc).GetVECTOR3();
        VECTOR3 currMaxAngAcc = AngularAcc().GetMaxAngAcc( vessel );
        accRatio.x = refMaxAngAcc.x / currMaxAngAcc.x;
        accRatio.y = refMaxAngAcc.y / currMaxAngAcc.y;
        accRatio.z = refMaxAngAcc.z / currMaxAngAcc.z;
    }
   // else
    {
    //    accRatio.x = accRatio.y = accRatio.z = 1;
    }

    return accRatio;
}
