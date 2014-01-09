#include "DialogTarget.h"
#include "../LaunchMFD.h"
#include "../MFDDataLaunchMFD.hpp"
#include "../localisation.h"
#include <MFDSound++/Sound.hpp>
#include <Orbiter/SpaceMathKOST.hpp>
#include <Math/GeneralMath.hpp>
#include <EnjoLib/ModuleMessaging.hpp>
#include "../Sound/SoundSampleIDEnum.hpp"
#include "../Utils/Targeting.hpp"

#include <locale>
#include <algorithm>

using namespace EnjoLib;

// Callback function for target input dialog
bool DialogTarget::clbk(void *id, char *str, void *usrdata)
{
    LaunchMFD * cLaunchMFD = (LaunchMFD*)usrdata;
    MFDDataLaunchMFD * data = cLaunchMFD->GetData();
    const double lat = data->GetMovParams().m_lat;
    std::string dispTarget = str;
    std::string tgtLowerCase = dispTarget;
    std::transform(dispTarget.begin(), dispTarget.end(), tgtLowerCase.begin(), ::tolower);

    VESSEL * tgt = Targeting().GetTargetVessel(str);
    OBJHANDLE hTgt = NULL;
    if (tgt)  // Target VESSEL found
    {
        dispTarget = tgt->GetName();
    }
    // Perhaps targeting TransX' settings?
    else if (tgtLowerCase == "tx" || tgtLowerCase == "transx")
    {
        Result<double> incl = ModuleMessaging().GetDouble("TransX", "Incl");
        Result<double> lan = ModuleMessaging().GetDouble("TransX", "LAN");
        if (incl.isSuccess && lan.isSuccess)
        {
            tgt = UpdateProbe( data, incl.value, lan.value, FRAME_ECL );
            dispTarget = "TransX";
        }
        else
            return false;
    }
    else  // Target VESSEL not found
    {
        // Perhaps we're targeting a celestial body?
        OBJHANDLE hGBody = Targeting().GetTargetBody(str);
        if (hGBody) // Yes, we are
        {
            hTgt = hGBody;
            dispTarget = Targeting().GetName(hTgt); // Get the body's name right
        }
        else // it seems, we're not target=ting a celestial body
        {
            double manIncl, manLAN;
            char frameBuf[2];
            if (sscanf_s(str, "%lf%lf%1s",&manIncl, &manLAN, &frameBuf, sizeof(frameBuf)) == 3)
            {
                // We're targeting a parking orbit, defined by inclination and LAN in a given frame
                if ( manIncl > 180 || manLAN > 360 || (manIncl < 0) || (manLAN < 0) ) // sanity checks
                    return false;
				char frame = frameBuf[0];
                if ( ::tolower(frame) == 'q' ) // Equatorial frame
                    tgt = UpdateProbe( data, manIncl, manLAN, FRAME_EQU );
                else if ( ::tolower(frame) == 'c' ) // Ecliptic frame
                    tgt = UpdateProbe( data, manIncl, manLAN, FRAME_ECL );
                else
                    return false;
            }
            else if (sscanf_s(str, "%lf%lf",&manIncl, &manLAN) == 2) // Dropped
            {
                return false;
                // We're targeting a parking orbit, defined by inclination and LAN in ecliptic frame
                /*
                if (manIncl > 180 || manLAN > 360 || (manIncl < 0) || (manLAN < 0)) // sanity checks
                    return false;
                tgt = UpdateProbe( data, manIncl, manLAN, FRAME_ECL );
                */
            }
            else if (sscanf_s(str, "%lf",&manIncl) == 1)
            {
                // Targeting an inclination in equatorial frame
                if (manIncl > 180 || manIncl < 0) // sanity checks
                    return false;
                if ((manIncl) < fabs(lat * DEG)
                        || fabs(manIncl) > 180-fabs(lat*DEG))
                {
                    manIncl = fabs(lat) + SMALL_DOUBLE;
                }
                data->GetTgtParam().incl = manIncl * RAD;
                data->SetTargetStr( MANUAL, MANUAL );
                cLaunchMFD->GetSound().PlaySound(TARGET_MANUAL);
                data->tgt_set = true;
                return(true);
            }
            else
            {
                // Gave up parsing the input string
                return(false);
            }
        }
    }
    if (tgt != NULL)
    {
        // Some VESSEL target was set - either a real one or The Probe
        hTgt = tgt->GetHandle();
    }
    if ( hTgt ) // The same for all types of targets - vessels, Probes and bodies
    {
        const SpaceMathKOST::ElementsOrbitParam & elopTgt = SpaceMathKOST().GetElements(hTgt, data->hRef, FRAME_EQU);
        if ( fabs(lat) < elopTgt.el.i ) // All fine
        {
            SetTarget(data, hTgt, elopTgt.el.i, dispTarget);
            cLaunchMFD->GetSound().PlaySound(TARGET_SELECTED);
            data->m_isTgtLowInclination = false;
        }
        else // Target's inclination is lower than your latitude. We'll try to target a similar dummy
        {
            cLaunchMFD->GetSound().PlaySound(TARGET_INCL_LOWER_THAN_LAT);
            EnjoLib::GeneralMath gm;
            // Increase the absolute value of latitude, to set it as new inclination,
            // which is as close to your latitude as possible.
            double newIncl = lat + 1 * RAD * gm.sign(lat);
            tgt = UpdateProbe( data, newIncl * DEG, elopTgt.el.theta * DEG, FRAME_EQU, tgt );
            if ( ! tgt )
                return false;
            SetTarget(data, tgt->GetHandle(), newIncl, dispTarget + "-" + PROBE);
            data->m_isTgtLowInclination = true;
        }
    }
    return(true);
}

void DialogTarget::SetTarget(MFDDataLaunchMFD * data, OBJHANDLE hTgt, double incl, const std::string & dispTarget)
{
    data->tgt_set = true;
    data->GetTgtParam().incl = incl;
    const std::string & targetName = Targeting().GetName(hTgt); // Get the target's name right
    data->SetTargetStr( targetName, dispTarget );
    data->PeA = data->ApA = data->GetAutomaticAlt(data->hRef);
    data->m_useOffplaneCorrector = true;
}

VESSEL * DialogTarget::UpdateProbe( MFDDataLaunchMFD * data, double inclDeg, double LANDeg, int frame, VESSEL * copyFrom )
{
    VESSELSTATUS2 vs;
    memset( &vs, 0, sizeof(vs) );
    vs.version = 2;
    vs.rbody = data->hRef;
    if (copyFrom)
    {
        copyFrom->GetRelativePos(data->hRef, vs.rpos);
        copyFrom->GetRelativeVel(data->hRef, vs.rvel);
    }
    else
    {
        // Place a guidance probe in selected orbit
        double rad = 1.1 * oapiGetSize( data->hRef ); // 10% above radius
        double vel = sqrt(data->GetMi() / rad); // Orbital velocity at the radius
        vs.rpos = _V(rad,0,0);
        vs.rvel = _V(0,0,vel);
    }

    const std::string & probeName = GetProbeName(data->GetVessel());

    VESSEL * pProbe = Targeting().GetTargetVessel(probeName.c_str()); // Try to find it
    if ( pProbe ) // The probe was created before
        pProbe->DefSetStateEx(&vs); // just move it
    else
    {
        // Probe wasn't present. Create it.
        OBJHANDLE hProbe = oapiCreateVesselEx(probeName.c_str(), PROBE, &vs);
        if (!hProbe) return NULL;
        pProbe = Targeting().GetTargetVessel(probeName.c_str());
        if (!pProbe) return NULL;
    }
    ELEMENTS el;
    pProbe->GetElements(data->hRef, el, 0, 0, frame);
    el.i = inclDeg * RAD;
    el.theta = LANDeg * RAD;
    pProbe->SetElements(data->hRef, el, 0, 0, frame);
    return pProbe;
}

std::string DialogTarget::GetProbeName( VESSEL * v )
{
    return std::string(PROBE) + '-' + v->GetName();
}
