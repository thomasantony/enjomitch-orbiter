#ifndef __ENERGYMFDMK2_H
#define __ENERGYMFDMK2_H

#include "orbitersdk.h"
typedef union {      // 3 energies 
	float data[3];
	struct { float ke, pe, me; };
} ENERGIES;

class EnergyMFDmk2 : public GraphMFD 
{
public:
	EnergyMFDmk2 (DWORD w, DWORD h, VESSEL *vessel);
	~EnergyMFDmk2 ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	bool ConsumeKeyBuffered(DWORD key);
	bool ConsumeButton (int bt, int event);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	
	bool SetEnergyRange (char *rstr);
	bool SetSampleT (char *rstr);
	bool SetTarget (char *rstr);

private:
	void FindRetRange (float * data, float * min, float * max);
	void SetAbsMinMax();
	void SetYRange();
	void SetXRangePlot(HDC hDC);
	void DrawSelection(HDC hDC);
	void DrawWords(HDC hDC);
	float absmin, absmax;
	VESSEL * vessel;
	//VECTOR3 CalculateEnergies();
};

#endif // !__ENERGYMFDMK2_H