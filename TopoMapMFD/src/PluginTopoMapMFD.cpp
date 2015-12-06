#include "PluginTopoMapMFD.h"
#include "TopoMapMFD.h"

PluginTopoMapMFD::PluginTopoMapMFD(HINSTANCE hDLL)
: oapi::Module(hDLL)
{
    //ctor
}

PluginTopoMapMFD::~PluginTopoMapMFD()
{
    //dtor
}

PluginTopoMapMFD * plugin;
static int g_MFDmode; // identifier for new MFD mode
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
    plugin = new PluginTopoMapMFD( hDLL );
    oapiRegisterModule (plugin);
}

// Called on module exit
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
    // Unregister the custom MFD mode when the module is unloaded
    oapiUnregisterMFDMode (g_MFDmode);
}
