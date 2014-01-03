#include <orbitersdk.h>
#include "Autopilot.h"
#include "../MathUtil.h"
//#include "../mfd.h"

extern VECTOR3 gTargetVec;

Autopilot::Autopilot()
{
    m_isEnabled = false;
}

Autopilot::~Autopilot(){}

void Autopilot::Update(double SimDT)
{
    if (!m_isEnabled)
        return;
    VESSEL * vessel = oapiGetFocusInterface();
    if (!vessel)
        return;

//VECTOR3 temp;
//vessel->GetGlobalPos(temp);
	//	temp=gTargetVec-temp;
    VECTOR3 trtarget = MathUtil::GetRotationToTarget(vessel, gTargetVec);
    //VECTOR3 trtargetUnit = unitise(trtarget);
    //vessel->SetAttitudeRotLevel();
    sprintf(oapiDebugString(), "(%.3lf, %.3lf, %.3lf)",gTargetVec.x, gTargetVec.y, gTargetVec.z );
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
