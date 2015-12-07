// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#ifndef __TOPOMFD_H
#define __TOPOMFD_H

#include <orbitersdk.h>

#include <vector>
#include <string>
#include <sstream>
#include <Utils/Pens.h>

#include "TopoMap.h"

class PluginTopoMapMFD;

class TopoMapMFD: public MFD2
{
public:
    TopoMapMFD (DWORD w, DWORD h, VESSEL * vessel, PluginTopoMapMFD * plugin);
    ~TopoMapMFD ();
    char *ButtonLabel (int bt);
    int ButtonMenu (const MFDBUTTONMENU **menu) const;
    bool Update ( oapi::Sketchpad * skp );
    bool ConsumeKeyBuffered(DWORD key);
    bool ConsumeKeyImmediate(char * kstate );
    bool ConsumeButton (int bt, int event);
    void WriteStatus (FILEHANDLE scn) const;
    void ReadStatus (FILEHANDLE scn);

    static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

    void SwitchButtonsPage();
    void SwitchRGBGrey();
    void RefreshIncrement();
    void RefreshDecrement();
    void DoNothing();

protected:

private:
    void DrawEllipse(oapi::Sketchpad * skp, int r, Pens::LineStyle penName);

    PluginTopoMapMFD * m_plugin;
    TopoMap m_tm;
    static bool m_rgb;
    Pens m_pens;
};

#endif // !__TOPOMFD_H
