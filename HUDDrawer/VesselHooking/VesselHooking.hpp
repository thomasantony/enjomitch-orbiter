// ==============================================================
//                ORBITER AUX LIBRARY: HUDDrawer
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Allows modules (MFDs or plain modules) to draw on HUD.
// WARNING! This library is a HACK and it may stop working
// in future versions of Orbiter API if the library isn't
// maintained anymore. Note however that it's very easy to do so,
// by updating the VesselHooking class
//
// Copyright (C) 2008 Steve "agentgonzo" Arch - VESSEL virtual table hijacking
// Copyright (C) 2012-2013 Szymon "Enjo" Ender - created a framework
//
//                         All rights reserved
//
// HUDDrawer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// HUDDrawer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HUDDrawer.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#ifndef HUDDrawer_HPP
#define HUDDrawer_HPP

#include "Orbitersdk.h"
#include "IDrawsHUD.hpp"
#include <vector>
#include <string>

namespace EnjoLib
{
/// Lets module to draw on HUD
/**
    Implements Vessel virtual methods hooking capabilities, so that it's possible to use the vessel's
    capability of drawing on HUD from other context, for example from an MFD.
    This class is by no means to be used by you. Only by the HUDDrawer module internally.
*/
class VesselHooking : public
#ifdef ORB2006
VESSEL2
#else
VESSEL3
#endif
{
public:
	/// Called internally from HUDDrawer module
	/**
        Never call this method or CTDs will occur on switching vessels
	*/
	static __declspec(dllexport) void FocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus);
    /// Ctor
    /** Used only internally  */
    VesselHooking(OBJHANDLE, int);
    /// Dtor
	~VesselHooking();

	struct Drawer;
	static __declspec(dllexport) std::vector<VesselHooking::Drawer> GetDrawersList();
	static __declspec(dllexport) void UpdateDrawer( const VesselHooking::Drawer & drawer );
	static __declspec(dllexport) void EnableAllDrawers();
	static __declspec(dllexport) void DisableAllDrawers();

private:


	// Callback functions
        virtual void clbkSetClassCaps (FILEHANDLE cfg) {};
        virtual void clbkSaveState (FILEHANDLE scn) {};
        virtual void clbkLoadStateEx (FILEHANDLE scn, void *status) {};
        virtual void clbkSetStateEx (const void *status) {};
        virtual void clbkPostCreation () {};
        virtual void clbkFocusChanged (bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel) {};
        virtual void clbkPreStep (double simt, double simdt, double mjd) {};
        virtual void clbkPostStep (double simt, double simdt, double mjd) {};
        virtual bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event) {return 0;};
        virtual void clbkVisualCreated (VISHANDLE vis, int refcount) {};
        virtual void clbkVisualDestroyed (VISHANDLE vis, int refcount) {};
        ///
        virtual	void clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
        ///
        virtual void clbkRCSMode (int mode) {};
        virtual void clbkADCtrlMode (DWORD mode) {};
        virtual void clbkHUDMode (int mode) {};
        virtual void clbkMFDMode (int mfd, int mode) {};
        virtual void clbkNavMode (int mode, bool active) {};
        virtual void clbkDockEvent (int dock, OBJHANDLE mate) {};
        virtual void clbkAnimate (double simt) {};
        virtual int  clbkConsumeDirectKey (char *keystate) {return 0;};
        virtual int  clbkConsumeBufferedKey (DWORD key, bool down, char *keystate) {return 0;};
        virtual bool clbkLoadGenericCockpit () {return 0;};
        virtual bool clbkLoadPanel (int id) {return 0;};
        virtual bool clbkPanelMouseEvent (int id, int event, int mx, int my) {return 0;};
        virtual bool clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf) {return 0;};
        virtual bool clbkLoadVC (int id) {return 0;};
        virtual bool clbkVCMouseEvent (int id, int event, VECTOR3 &p) {return 0;};
        virtual bool clbkVCRedrawEvent (int id, int event, SURFHANDLE surf) {return 0;};
    #ifndef ORB2006
        virtual bool clbkPanelMouseEvent (int id, int event, int mx, int my, void *context) {return true;}
        virtual bool clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf, void *context) {return true;}
        virtual int  clbkGeneric (int msgid=0, int prm=0, void *context=NULL) {return 0;}
        virtual bool clbkLoadPanel2D (int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) {return true;}
        ///
        virtual bool clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp );
        ///
        virtual void clbkRenderHUD (int mode, const HUDPAINTSPEC *hps, SURFHANDLE hDefaultTex) {}
        virtual void clbkGetRadiationForce (const VECTOR3 &mflux, VECTOR3 &F, VECTOR3 &pos) {}
    #endif
        virtual void OldclbkSetClassCaps (FILEHANDLE cfg) {};
        virtual void OldclbkSaveState (FILEHANDLE scn) {};
        virtual void OldclbkLoadStateEx (FILEHANDLE scn, void *status) {};
        virtual void OldclbkSetStateEx (const void *status)  {};
        virtual void OldclbkPostCreation () {};
        virtual void OldclbkFocusChanged (bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel)  {};
        virtual void OldclbkPreStep (double simt, double simdt, double mjd)  {};
        virtual void OldclbkPostStep (double simt, double simdt, double mjd)  {};
        virtual bool OldclbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event) {return 0;};
        virtual void OldclbkVisualCreated (VISHANDLE vis, int refcount) {};
        virtual void OldclbkVisualDestroyed (VISHANDLE vis, int refcount)  {};
        virtual void OldclbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
        virtual void OldclbkRCSMode (int mode) {};
        virtual void OldclbkADCtrlMode (DWORD mode) {};
        virtual void OldclbkHUDMode (int mode) {};
        virtual void OldclbkMFDMode (int mfd, int mode) {};
        virtual void OldclbkNavMode (int mode, bool active) {};
        virtual void OldclbkDockEvent (int dock, OBJHANDLE mate) {};
        virtual void OldclbkAnimate (double simt)  {};
        virtual int  OldclbkConsumeDirectKey (char *keystate) {return 0;};
        virtual int  OldclbkConsumeBufferedKey (DWORD key, bool down, char *keystate) {return 0;};
        virtual bool OldclbkLoadGenericCockpit () {return 0;};
        virtual bool OldclbkLoadPanel (int id) {return 0;};
        virtual bool OldclbkPanelMouseEvent (int id, int event, int mx, int my) {return 0;};
        virtual bool OldclbkPanelRedrawEvent (int id, int event, SURFHANDLE surf) {return 0;};
        virtual bool OldclbkLoadVC (int id) {return 0;};
        virtual bool OldclbkVCMouseEvent (int id, int event, VECTOR3 &p) {return 0;};
        virtual bool OldclbkVCRedrawEvent (int id, int event, SURFHANDLE surf) {return 0;};

    #ifndef ORB2006
        virtual bool OldclbkPanelMouseEvent (int id, int event, int mx, int my, void *context) {return true;}
        virtual bool OldclbkPanelRedrawEvent (int id, int event, SURFHANDLE surf, void *context) {return true;}
        virtual int  OldclbkGeneric (int msgid=0, int prm=0, void *context=NULL) {return 0;}
        virtual bool OldclbkLoadPanel2D (int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) {return true;}
        virtual bool OldclbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp );
        virtual void OldclbkRenderHUD (int mode, const HUDPAINTSPEC *hps, SURFHANDLE hDefaultTex) {}
        virtual void OldclbkGetRadiationForce (const VECTOR3 &mflux, VECTOR3 &F, VECTOR3 &pos) {}
	#endif

	private:
		void Hook();
        void UnHook();

        friend class IDrawsHUD;
        static void OnMFDConstruction( IDrawsHUD * mfd );
        static void OnMFDDestruction( IDrawsHUD * mfd );
        static bool CanDrawHUD( IDrawsHUD * mfd );
        static bool IsVesselCompatible( VESSEL * v );

        static void TryHook( OBJHANDLE old_focus );
        static void TryUnHook( OBJHANDLE new_focus );

        typedef std::vector<IDrawsHUD *> VEC_HUD_DRAWERS;
        static VEC_HUD_DRAWERS vecHUDrawers;
        static std::vector<std::string> moduleBlackList;
};

struct VesselHooking::Drawer
{
    Drawer(const std::string & id, bool isActive)
    {
        this->isActive = isActive;
        this->id = id;
    }
    bool isActive;
    std::string id;
};
}

#endif // HUDDrawer_HPP
