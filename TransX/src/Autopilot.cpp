#include <orbitersdk.h>
#include "Autopilot.h"
#include "mfd.h"
#include <Orbiter/AngularAcc.hpp>
#include <Orbiter/SystemsConverterOrbiter.hpp>

using namespace EnjoLib;
extern VECTOR3 gTargetVec;
const Vect3 Autopilot::m_statDeltaGliderRefRotAcc(0.125, 0.066, 0.189);

Autopilot::Autopilot()
: m_pidAPSpaceX(2.5, 16)
, m_pidAPSpaceY(m_pidAPSpaceX)
, m_pidAPSpaceBank(1, 12)
{
    m_isEnabled = false;
}

Autopilot::~Autopilot(){}

void Autopilot::Update(double SimDT)
{
    if (!m_isEnabled)
    {
        sprintf(oapiDebugString(), "TransX: AUTO rotation disabled. Press SHIFT+C in target view to enable");
        return;
    }
    if (gTargetVec.x + gTargetVec.y + gTargetVec.z == 0)
        return;
    sprintf(oapiDebugString(), "TransX: AUTO rotation ENABLED!");

    VESSEL * vessel = oapiGetFocusInterface();
    if (!vessel)
        return;

    VECTOR3 trtarget = GetRotationToTarget(vessel, unitise(gTargetVec));
    const VECTOR3 accRatio = GetVesselAngularAccelerationRatio(vessel);

    const double x = accRatio.x * m_pidAPSpaceX.Update( trtarget.x, SimDT );
    vessel->SetAttitudeRotLevel( 1, -x );
    const double y = accRatio.y * m_pidAPSpaceY.Update( trtarget.y, SimDT );
    vessel->SetAttitudeRotLevel( 0, y );

    const double inputBank = vessel->GetBank() / PI;
    const double b = accRatio.z * m_pidAPSpaceBank.Update( inputBank, SimDT );
    vessel->SetAttitudeRotLevel( 2, b );
}

void Autopilot::Enable(bool val)
{
    m_isEnabled = val;
}
void Autopilot::SwitchEnabled()
{
    m_isEnabled = !m_isEnabled;
}
bool Autopilot::IsEnabled()
{
    return m_isEnabled;
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
