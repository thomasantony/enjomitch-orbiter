#ifndef __TOPOMFD_H
#define __TOPOMFD_H

#include <orbitersdk.h>

#include <vector>
#include <string>
#include <sstream>

class TopoMapMFD: public MFD2
{
public:
    TopoMapMFD (DWORD w, DWORD h, VESSEL * vessel);
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
    void DoNothing();

protected:

private:
    bool m_grey;
};

#endif // !__TOPOMFD_H
