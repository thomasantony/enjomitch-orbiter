#include "MathUtil.h"
#include "mfd.h"

MathUtil::MathUtil()
{
    //ctor
}

MathUtil::~MathUtil()
{
    //dtor
}

VECTOR3 MathUtil::GetRotationToTarget(VESSEL * vessel, const VECTOR3 & target)
{
    VECTOR3 trtarget;
	VESSELSTATUS status;
    vessel->GetStatus(status);
	VECTOR3 arot=status.arot;
	MATRIX3 rotmatrix;
	getinvrotmatrix(arot,&rotmatrix);
	trtarget = mul(rotmatrix, target);

	return trtarget;
}
