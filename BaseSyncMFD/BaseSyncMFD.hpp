//  ==============================================================================================================================================
//	Copyright (C) 2002 - 2015 Jarmo Nikkanen
//                2016        Andrew Stokes
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>
//
//  ==============================================================================================================================================


#ifndef __BASESYNC_H
#define __BASESYNC_H

#include "BSOrbit.hpp"
#include "BaseSyncExports.hpp"
#include "EnjoLib\ModuleMessagingExtPut.hpp"
#include "EnjoLib\ModuleMessagingExt.hpp"
#include "GlideslopeExports.hpp"

#define IMFD_DEBUG 0 

extern HBRUSH green_brush,dgreen_brush;
extern HBRUSH yellow_brush;
extern HBRUSH black_brush,grey_brush;
extern HPEN dash_pen,solid_pen,solid_pen_y,dash_pen_y,solid_pen_grey;
extern HPEN dash_pen_dgrey,solid_pen_dgrey,solid_pen_dgreen, solid_pen_white;
extern HBRUSH white_brush,red_brush,dgrey_brush,no_brush;

int		MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

extern char     LineBuffer[4096];
extern VESSEL   *ship;
static int		ID;
extern bool     error_flag;


class SyncMFD : public EnjoLib::ModuleMessagingExtPut {

public:
			SyncMFD(class GeoSyncMFD *);
			~SyncMFD();

	const char * ModuleMessagingGetModuleName() const { return "BaseSyncMFD"; }
	void	Update(HDC);	
	void	TimeStep();
	void    Write(FILEHANDLE);
	void    Read(FILEHANDLE);
	void    ChangeMode();
	void    Direct();
	void    EncMode();
	void    Deorbit();
	void    LineAdjust(double);

	double  Trl2Time(double trl, Orbit *src);

	VECTOR3 ComputeDeOrbit(double mu, double rea, VECTOR3 _entry, VECTOR3 _cp);
	bool    ComputeDeOrbit(double mu, double rad, double refalt, double rea, double *pre, double *dV);
	bool    InterpolateClosestPassage(OBJHANDLE ref, Orbit *src, double lon,double lat,double orbit,
		    double *diff,double *tim, double *head,double *longitude);

	class	GeoSyncMFD *MFD;

//	char    Reference[32], Target[32];

	bool	MMPut_done;
	struct	BaseSyncExportTgtStruct bstrgt;
	struct	BaseSyncExportTgtStruct *trgt;
  const   struct  GlideslopeExportTgtStruct *gs2trgt;
  bool    usingGS2;
	struct	BaseSyncExportModeStruct mode;
	struct	BaseSyncExportSolStruct sol;
	struct	BaseSyncExportDeorbitStruct deo;
	struct	BaseSyncExportBurnStruct burn;

	double  time_to_int, sync_time, sync_trl, sync_line;
	double  sync_est, sync_rea, sync_opt;
	int     sync_min, sync_sel, display_texts, sync_num, sync_dispmin;
	bool    vector_display;
};





class GeoSyncMFD: public MFD {

public:

			GeoSyncMFD(DWORD w, DWORD h, VESSEL *vessel, int mfd);
			~GeoSyncMFD();

    bool	ConsumeKeyBuffered(DWORD key);
    bool	ConsumeButton(int bt, int event);
    char *	ButtonLabel(int bt);
    int		ButtonMenu(const MFDBUTTONMENU **menu) const;
    void	Update(HDC hDC);
    void	WriteStatus(FILEHANDLE scn) const;
    void	ReadStatus(FILEHANDLE scn);
    void	StoreStatus(void) const;
    void	RecallStatus(void);
			
	class	SyncMFD  *Sync;
	
	int		LineHeight;
	int		ButtonSpacing;
	int		FirstButton;					
	int		MFD_ID;
	int		width, height; 				
};

#endif 
