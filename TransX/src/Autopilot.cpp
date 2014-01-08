#include <orbitersdk.h>
#include "Autopilot.h"
#include "mfd.h"
#include <Orbiter/AngularAcc.hpp>
#include <Orbiter/SystemsConverterOrbiter.hpp>
#include <Orbiter/VesselCapabilities.hpp>

using namespace EnjoLib;
const Vect3 Autopilot::m_statDeltaGliderRefRotAcc(0.125, 0.066, 0.189);

Autopilot::Autopilot()
: m_targetVector(_V(0,0,0))
, m_targetVectorUnit(_V(0,0,0))
, m_targetLengthPrev(0)
, m_pidAPSpaceX(0.8, 5)
, m_pidAPSpaceY(m_pidAPSpaceX)
, m_pidAPSpaceBank(1, 12)
{
    m_isEnabled = false;
}

Autopilot::~Autopilot(){}

void Autopilot::SetTargetVector(const VECTOR3 & targetVector)
{
    m_targetVector = targetVector;
    m_targetVectorUnit = unit(targetVector);
    double targetLength = length(m_targetVector);
    if (targetLength != 0)
    {
        bool isDVVerySmall = targetLength < 1.0; // Prevent rolling 180 degrees around
        bool isDVIncreasing = targetLength > m_targetLengthPrev; // dV starts increasing = burn complete
        if (isDVIncreasing || isDVVerySmall)
            MECO(oapiGetFocusInterface());
    }
    m_targetLengthPrev = targetLength;
}

void Autopilot::Disable()
{
    SetTargetVector(_V(0,0,0));
}

void Autopilot::Update(double SimDT)
{
    bool isZeroVector = m_targetVector.x + m_targetVector.y + m_targetVector.z == 0;
    if (isZeroVector)
    {
        if (IsEnabled())
            Enable(false);
        //sprintf(oapiDebugString(), "TransX: AUTO rotation disabled. Press SHIFT+C in target view to enable");
        return;
    }
    VESSEL * vessel = oapiGetFocusInterface();
    if (!vessel)
        return;

    if (!IsEnabled())
    {
        Enable(true);
    }
    vessel->DeactivateNavmode( NAVMODE_PROGRADE );
    vessel->DeactivateNavmode( NAVMODE_RETROGRADE );
    vessel->DeactivateNavmode( NAVMODE_NORMAL );
    vessel->DeactivateNavmode( NAVMODE_ANTINORMAL );
    vessel->DeactivateNavmode( NAVMODE_HLEVEL );
    vessel->DeactivateNavmode( NAVMODE_HOLDALT );
    //sprintf(oapiDebugString(), "TransX: AUTO rotation ENABLED!");

    const VECTOR3 & angleToTarget = GetRotationToTarget(vessel, m_targetVectorUnit);
    const VECTOR3 & accRatio = GetVesselAngularAccelerationRatio(vessel);

    const double inputBank = (vessel->GetBank() - PI/2.0) / PI; // Targeting 90* = PI/2, like the Prograde autopilot
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
/*
void Autopilot::SwitchEnabled()
{
    m_isEnabled = !m_isEnabled;
    OnDisabled();
}
*/
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

void Autopilot::MECO(VESSEL * vessel)
{
    MainEngineOn(vessel, 0);
}

void Autopilot::MainEngineOn( VESSEL * vessel, double level )
{
    THGROUP_HANDLE h = VesselCapabilities().GetMainEnginesHandle(vessel);
    if ( h == NULL )
        return;
    if ( level > 1 )
        level = 1;
    else if ( level < 0 )
        level = 0;
    vessel->SetThrusterGroupLevel( h, level );
}
