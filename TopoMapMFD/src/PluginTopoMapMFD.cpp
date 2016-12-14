// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#include "PluginTopoMapMFD.h"
#include "TopoMapMFD.h"
//#include "OGCImy.h"

PluginTopoMapMFD * gp_plugin;
static int g_MFDmode; // identifier for new MFD mode

PluginTopoMapMFD::PluginTopoMapMFD(HINSTANCE hDLL)
: oapi::Module(hDLL)
, m_tm(NULL)
{
}

PluginTopoMapMFD::~PluginTopoMapMFD()
{
    //dtor
}

void PluginTopoMapMFD::clbkPreStep (double simt, double simdt, double mjd)
{
    if (!m_tm)
        return;
    m_tm->UpdateMap();
}

// ==============================================================
// API interface

// Called on module init
DLLCLBK void InitModule (HINSTANCE hDLL)
{
    static char *name = "TopoMapMFD";   // MFD mode name
    MFDMODESPECEX spec;
    // Set MFDMODESPEC's fields
    spec.name = name;
    spec.key = OAPI_KEY_P;                // MFD mode selection key
    spec.msgproc = TopoMapMFD::MsgProc;  // MFD mode callback function
    spec.context = NULL;
    // Register the new MFD mode with Orbiter
    g_MFDmode = oapiRegisterMFDMode (spec);
    gp_plugin = new PluginTopoMapMFD( hDLL );
    oapiRegisterModule (gp_plugin);
}

// Called on module exit
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
    // Unregister the custom MFD mode when the module is unloaded
    oapiUnregisterMFDMode (g_MFDmode);
}
