/* 
   OpOrbiterVessel.h

   2005, Oliver Pieper (pieper@viaregio.de)

   Helper functions for Orbiter vessels.
*/

#include "Orbitersdk.h"

double GetMaxThrust0(VESSEL *vessel) {
/* Return the maximum thrust of the vessel (the sum of the maximum thrust of all thrusters) */
	double res = 0.0;
	for (BYTE i = 0; i < vessel->GetThrusterCount(); i++)
	{
		res = res + vessel->GetThrusterMax0(vessel->GetThrusterHandleByIndex(i));
	}
	return res;
}

double GetCurrentThrust0(VESSEL *vessel) {
/* Return the current thrust of the vessel (the sum of the current thrust of all thrusters) */
	double res = 0.0;
	for (BYTE i = 0; i < vessel->GetThrusterCount(); i++)
	{
		res = res + vessel->GetThrusterLevel(vessel->GetThrusterHandleByIndex(i)) * vessel->GetThrusterMax0(vessel->GetThrusterHandleByIndex(i));
	}
	return res;
}

VESSEL *GetFirstAttachment(VESSEL *vessel, bool toparent) {
/* Get the first attachment of the specified type or NULL, if no attached vessel is present. */
	OBJHANDLE att_obj;
	for (BYTE i = 0; i < vessel->AttachmentCount(toparent); i++)
	{
		att_obj = vessel->GetAttachmentStatus(vessel->GetAttachmentHandle(toparent, i));
		if (att_obj) return oapiGetVesselInterface(att_obj);
	}
	return NULL;
}

VESSEL *GetUltimateParent(VESSEL *vessel, VESSEL *notfound_vessel = NULL) {
/* Get the last parent of a vessel attached as child in a string of attachments. */
	VESSEL *last_vessel = notfound_vessel;
	VESSEL *this_vessel = vessel;
	while (this_vessel = GetFirstAttachment(this_vessel, true))
		last_vessel = this_vessel;
	return last_vessel;
}