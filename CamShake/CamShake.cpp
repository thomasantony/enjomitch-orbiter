// ==============================================================
//                  ORBITER PLUGIN: CamShake
//            Copyright (C) 2005, 2006 Oliver Pieper
//
// CamShake.cpp
//
// Add VC shake effects for thrusters and athmospheric effects.
// ==============================================================

#define STRICT 1
#define ORBITER_MODULE
#include "Orbitersdk.h"
#include "DlgCtrl.h"
#include "resource.h"
#include <cstdio>
#include "perlin.h"
#include "inifile.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include "OpOrbiterVessel.h"

using namespace boost::filesystem;
using namespace boost;


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

// Global parameters
static struct {
	BOOL      th_enabled, at_enabled, gd_enabled, no_staging;
	HINSTANCE hDLL;
	HWND      hDlg;
	DWORD     dwCmd;
	VESSEL    *focus_vessel;
	VESSEL    *parent_vessel;  // ultimate attachment parent, or focus_vessel, if not attached
	VECTOR3 StartPos, ShakePos;
	double fa_thrust[4];  // freq/amp pairs for thruster effects
	double fa_atmos[4];  // freq/amp pairs for atmospheric effects
	double fa_ground[4];  // freq/amp pairs for ground effects
	double max_thrust, gd_max_speed;
} g_Param;

void OpenDlgClbk (void *context);
BOOL CALLBACK DlgProc (HWND, UINT, WPARAM, LPARAM);

// DLL initialisation
DLLCLBK void InitModule (HINSTANCE hDLL)
{
	g_Param.hDLL = hDLL;
	g_Param.focus_vessel = NULL;
	g_Param.parent_vessel = NULL;
	g_Param.th_enabled = g_Param.at_enabled = g_Param.gd_enabled = g_Param.no_staging = FALSE;
	
	g_Param.fa_thrust[0] = 50;
	g_Param.fa_thrust[1] = 0.001;
	g_Param.fa_thrust[2] = 10;
	g_Param.fa_thrust[3] = 0.004;
	
	g_Param.fa_atmos[0] = 100;
	g_Param.fa_atmos[1] = 0.001;
	g_Param.fa_atmos[2] = 50;
	g_Param.fa_atmos[3] = 0.003;
	
	g_Param.gd_max_speed = 100.0;
	g_Param.fa_ground[0] = 100;
	g_Param.fa_ground[1] = 0.001;
	g_Param.fa_ground[2] = 50;
	g_Param.fa_ground[3] = 0.003;

	// Register the custom function in Orbiter
	g_Param.dwCmd = oapiRegisterCustomCmd ("Camera Shake",
		"Simulate cockpit vibration due to thruster and atmospheric effects.",
		OpenDlgClbk, NULL);

	// Register custom dialog controls
	//oapiRegisterCustomControls (hDLL);
}

// DLL cleanup
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the custom function in Orbiter
	oapiUnregisterCustomCmd (g_Param.dwCmd);

	// Unregister custom dialog controls
	//oapiUnregisterCustomControls (hDLL);
}

DLLCLBK void opcOpenRenderViewport (HWND hWnd, DWORD w, DWORD h, BOOL fullscreen)
{
	g_Param.hDlg = NULL;
}



// Store the settings for the current vessel class
void StoreVesselParameters()
{
	path config_file("Config/CamShake/CamShake.cfg");

	// create IniFile, if necessary
	if (!exists(config_file))	CIniFile::Create(config_file.string());

	string SectionName(g_Param.parent_vessel->GetClassName());

	// write stage independent parameters
	CIniFile::SetValue("NoStagingDetection", lexical_cast<std::string>(g_Param.no_staging), SectionName, config_file.string());
	
	if (g_Param.no_staging)
		SectionName += "_NOSTAGING";
	else
		SectionName += "_" + lexical_cast<std::string>(int(GetMaxThrust0(g_Param.parent_vessel)));
	
	// store Thrust effect settings
	CIniFile::SetValue("ThrustEffectEnabled", lexical_cast<std::string>(g_Param.th_enabled), SectionName, config_file.string());
	CIniFile::SetValue("ThrustFrequency1", lexical_cast<std::string>(g_Param.fa_thrust[0]), SectionName, config_file.string());
	CIniFile::SetValue("ThrustAmplitude1", lexical_cast<std::string>(g_Param.fa_thrust[1]), SectionName, config_file.string()); 
	CIniFile::SetValue("ThrustFrequency2", lexical_cast<std::string>(g_Param.fa_thrust[2]), SectionName, config_file.string());
	CIniFile::SetValue("ThrustAmplitude2", lexical_cast<std::string>(g_Param.fa_thrust[3]), SectionName, config_file.string()); 

	// store atmospheric effect settings
	CIniFile::SetValue("AtmosEffectEnabled", lexical_cast<std::string>(g_Param.at_enabled), SectionName, config_file.string());
	CIniFile::SetValue("AtmosFrequency1", lexical_cast<std::string>(g_Param.fa_atmos[0]), SectionName, config_file.string());
	CIniFile::SetValue("AtmosAmplitude1", lexical_cast<std::string>(g_Param.fa_atmos[1]), SectionName, config_file.string()); 
	CIniFile::SetValue("AtmosFrequency2", lexical_cast<std::string>(g_Param.fa_atmos[2]), SectionName, config_file.string());
	CIniFile::SetValue("AtmosAmplitude2", lexical_cast<std::string>(g_Param.fa_atmos[3]), SectionName, config_file.string()); 

	// store ground effect settings
	CIniFile::SetValue("GroundEffectEnabled", lexical_cast<std::string>(g_Param.gd_enabled), SectionName, config_file.string());
	CIniFile::SetValue("GroundFrequency1", lexical_cast<std::string>(g_Param.fa_ground[0]), SectionName, config_file.string());
	CIniFile::SetValue("GroundAmplitude1", lexical_cast<std::string>(g_Param.fa_ground[1]), SectionName, config_file.string()); 
	CIniFile::SetValue("GroundFrequency2", lexical_cast<std::string>(g_Param.fa_ground[2]), SectionName, config_file.string());
	CIniFile::SetValue("GroundAmplitude2", lexical_cast<std::string>(g_Param.fa_ground[3]), SectionName, config_file.string()); 
	CIniFile::SetValue("GroundMaxSpeed", lexical_cast<std::string>(g_Param.gd_max_speed), SectionName, config_file.string()); 
}

// Read the settings for the current vessel class (or use default settings)
void ReadVesselParameters()
{
	path config_file("Config/CamShake/CamShake.cfg");

	// create IniFile, if necessary
	if (!exists(config_file))	CIniFile::Create(config_file.string());

	string SectionName(g_Param.parent_vessel->GetClassName());
	
	// read stage independent parameters
	// check, if staging detection for this vessel is disabled
	g_Param.no_staging = lexical_cast<BOOL>(CIniFile::GetValueDef("NoStagingDetection", SectionName, config_file.string(), "0"));

	if (g_Param.no_staging)
		SectionName += "_NOSTAGING";
	else
		SectionName += "_" + lexical_cast<std::string>(int(GetMaxThrust0(g_Param.parent_vessel)));

	
	// read Thrust effect settings
	g_Param.th_enabled = lexical_cast<BOOL>(CIniFile::GetValueDef("ThrustEffectEnabled", SectionName, config_file.string(), "0"));
	g_Param.fa_thrust[0] = lexical_cast<double>(CIniFile::GetValueDef("ThrustFrequency1", SectionName, config_file.string(), "50"));
	g_Param.fa_thrust[1] = lexical_cast<double>(CIniFile::GetValueDef("ThrustAmplitude1", SectionName, config_file.string(), "0.001"));
	g_Param.fa_thrust[2] = lexical_cast<double>(CIniFile::GetValueDef("ThrustFrequency2", SectionName, config_file.string(), "10"));
	g_Param.fa_thrust[3] = lexical_cast<double>(CIniFile::GetValueDef("ThrustAmplitude2", SectionName, config_file.string(), "0.004"));

	// read atmospheric effect settings
	g_Param.at_enabled = lexical_cast<BOOL>(CIniFile::GetValueDef("AtmosEffectEnabled", SectionName, config_file.string(), "0"));
	g_Param.fa_atmos[0] = lexical_cast<double>(CIniFile::GetValueDef("AtmosFrequency1", SectionName, config_file.string(), "100"));
	g_Param.fa_atmos[1] = lexical_cast<double>(CIniFile::GetValueDef("AtmosAmplitude1", SectionName, config_file.string(), "0.001"));
	g_Param.fa_atmos[2] = lexical_cast<double>(CIniFile::GetValueDef("AtmosFrequency2", SectionName, config_file.string(), "50"));
	g_Param.fa_atmos[3] = lexical_cast<double>(CIniFile::GetValueDef("AtmosAmplitude2", SectionName, config_file.string(), "0.003"));

	// read ground effect settings
	g_Param.gd_enabled = lexical_cast<BOOL>(CIniFile::GetValueDef("GroundEffectEnabled", SectionName, config_file.string(), "0"));
	g_Param.fa_ground[0] = lexical_cast<double>(CIniFile::GetValueDef("GroundFrequency1", SectionName, config_file.string(), "100"));
	g_Param.fa_ground[1] = lexical_cast<double>(CIniFile::GetValueDef("GroundAmplitude1", SectionName, config_file.string(), "0.001"));
	g_Param.fa_ground[2] = lexical_cast<double>(CIniFile::GetValueDef("GroundFrequency2", SectionName, config_file.string(), "50"));
	g_Param.fa_ground[3] = lexical_cast<double>(CIniFile::GetValueDef("GroundAmplitude2", SectionName, config_file.string(), "0.003"));
	g_Param.gd_max_speed = lexical_cast<double>(CIniFile::GetValueDef("GroundMaxSpeed", SectionName, config_file.string(), "100"));
}

// Open the dialog box
void OpenDlgClbk (void *context)
{
	char cbuf[10];
	HWND hDlg = oapiOpenDialog (g_Param.hDLL, IDD_INTERFACE, DlgProc);
	if (hDlg) {
		g_Param.hDlg = hDlg;

		CheckDlgButton(hDlg, IDC_CHK_NOSTAGING, g_Param.no_staging);

		CheckDlgButton(hDlg, IDC_CHK_THRUST_ENABLED, g_Param.th_enabled);
		sprintf(cbuf, "%#.1f", g_Param.fa_thrust[0]);
		SetDlgItemText(hDlg, IDC_EDIT_TH_FREQ1, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_thrust[1] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_TH_AMP1, cbuf);
		sprintf(cbuf, "%#.1f", g_Param.fa_thrust[2]);
		SetDlgItemText(hDlg, IDC_EDIT_TH_FREQ2, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_thrust[3] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_TH_AMP2, cbuf);
		
		CheckDlgButton(hDlg, IDC_CHK_ATMO_ENABLED, g_Param.at_enabled);
		sprintf(cbuf, "%#.1f", g_Param.fa_atmos[0]);
		SetDlgItemText(hDlg, IDC_EDIT_AT_FREQ1, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_atmos[1] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_AT_AMP1, cbuf);
		sprintf(cbuf, "%#.1f", g_Param.fa_atmos[2]);
		SetDlgItemText(hDlg, IDC_EDIT_AT_FREQ2, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_atmos[3] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_AT_AMP2, cbuf);

		CheckDlgButton(hDlg, IDC_CHK_GROUND_ENABLED, g_Param.gd_enabled);
		sprintf(cbuf, "%#.1f", g_Param.fa_ground[0]);
		SetDlgItemText(hDlg, IDC_EDIT_GD_FREQ1, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_ground[1] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_GD_AMP1, cbuf);
		sprintf(cbuf, "%#.1f", g_Param.fa_ground[2]);
		SetDlgItemText(hDlg, IDC_EDIT_GD_FREQ2, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.fa_ground[3] * 100);
		SetDlgItemText(hDlg, IDC_EDIT_GD_AMP2, cbuf);
		sprintf(cbuf, "%#.2f", g_Param.gd_max_speed);
		SetDlgItemText(hDlg, IDC_EDIT_GD_MAXSPD, cbuf);
	}
}

// Vessel focus change
DLLCLBK void opcFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
	// restore initial camera position of previous vessel, if available
	if (old_focus)
		oapiGetVesselInterface(old_focus)->SetCameraOffset(g_Param.StartPos);

	g_Param.focus_vessel = oapiGetVesselInterface(new_focus);
	g_Param.parent_vessel = GetUltimateParent(g_Param.focus_vessel, g_Param.focus_vessel);
	ReadVesselParameters();
	g_Param.focus_vessel->GetCameraOffset(g_Param.StartPos);	// save initial camera position
	g_Param.focus_vessel->GetCameraOffset(g_Param.ShakePos);	// starting position for shake
	g_Param.max_thrust = GetMaxThrust0(g_Param.parent_vessel);

	if (g_Param.hDlg)
		EnableWindow(GetDlgItem(g_Param.hDlg, IDC_BTN_OK), false);
}


// Frame update
DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd)
{	
	// If vessel is attachment: Has the parent vessel changed?
	// Sadly, there is no callback function for this.
	if (g_Param.parent_vessel != GetUltimateParent(g_Param.focus_vessel, g_Param.focus_vessel))
	{
		g_Param.focus_vessel->SetCameraOffset(g_Param.StartPos);	
		g_Param.parent_vessel = GetUltimateParent(g_Param.focus_vessel, g_Param.focus_vessel);
		g_Param.max_thrust = GetMaxThrust0(g_Param.parent_vessel);
		ReadVesselParameters();
		if (g_Param.hDlg) 
			EnableWindow(GetDlgItem(g_Param.hDlg, IDC_BTN_OK), false); // disable OK button
	}

	// Has the max thrust changed? This indicates the staging of a rocket which
	// cannot be detected by relying on opcFocusChanged.
	if (!g_Param.no_staging && GetMaxThrust0(g_Param.parent_vessel) != g_Param.max_thrust) {
		g_Param.focus_vessel->SetCameraOffset(g_Param.StartPos);	
		g_Param.max_thrust = GetMaxThrust0(g_Param.parent_vessel);
		ReadVesselParameters();
		if (g_Param.hDlg) 
			EnableWindow(GetDlgItem(g_Param.hDlg, IDC_BTN_OK), false); // disable OK button
	}

	// display parameters on hDlg, if visible
	if (g_Param.hDlg) {
		string aux(g_Param.parent_vessel->GetClassName());
		if (g_Param.parent_vessel != g_Param.focus_vessel)
			aux = aux + " (attachment parent)";
		SetDlgItemText(g_Param.hDlg, IDC_STATIC_CLASSNAME, aux.c_str());
		aux = lexical_cast<std::string>(int(GetMaxThrust0(g_Param.parent_vessel)));
		SetDlgItemText(g_Param.hDlg, IDC_STATIC_MAXTHRUST, aux.c_str());
	}

	// shaking the camera only works in internal modes
	if (!oapiCameraInternal()) return;

	char cbuf[128];

	// has the camera been moved outside of this plugin?
	VECTOR3 v3Aux;
	g_Param.focus_vessel->GetCameraOffset(v3Aux);
	if (v3Aux.x != g_Param.ShakePos.x || v3Aux.y != g_Param.ShakePos.y || v3Aux.z != g_Param.ShakePos.z)
		g_Param.focus_vessel->GetCameraOffset(g_Param.StartPos);

	
  	double fa_test[4];  // freq/amp pairs for thruster effects
	// has a test of the thrust shake been requested?
	if (g_Param.hDlg && (SendDlgItemMessage(g_Param.hDlg, IDC_BTN_TEST_TH, BM_GETSTATE, 0, 0) & BST_PUSHED))
	{
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_FREQ1, cbuf, 128);
		fa_test[0] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_AMP1, cbuf, 128);
		fa_test[1] = atof(cbuf)/100.0;
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_FREQ2, cbuf, 128);
		fa_test[2] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_AMP2, cbuf, 128);
		fa_test[3] = atof(cbuf)/100.0;
		if (!fa_test[0]) fa_test[1] = 0;  // no frequency, no amplitude!
		if (!fa_test[2]) fa_test[3] = 0;  // no frequency, no amplitude!
		g_Param.ShakePos = g_Param.StartPos;
		g_Param.ShakePos.x = g_Param.ShakePos.x + Perlin(SimT, fa_test, 2);
		g_Param.ShakePos.y = g_Param.ShakePos.y + Perlin(1000 + SimT, fa_test, 2);
		g_Param.ShakePos.z = g_Param.ShakePos.z + Perlin(2000 + SimT, fa_test, 2);
		g_Param.focus_vessel->SetCameraOffset(g_Param.ShakePos);
		return;		
	}
	// or has a test of the atmospheric shake been requested?
	else if (g_Param.hDlg && (SendDlgItemMessage(g_Param.hDlg, IDC_BTN_TEST_AT, BM_GETSTATE, 0, 0) & BST_PUSHED))
	{
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_FREQ1, cbuf, 128);
		fa_test[0] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_AMP1, cbuf, 128);
		fa_test[1] = atof(cbuf)/100.0;
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_FREQ2, cbuf, 128);
		fa_test[2] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_AMP2, cbuf, 128);
		fa_test[3] = atof(cbuf)/100.0;
		if (!fa_test[0]) fa_test[1] = 0;  // no frequency, no amplitude!
		if (!fa_test[2]) fa_test[3] = 0;  // no frequency, no amplitude!
		g_Param.ShakePos = g_Param.StartPos;
		g_Param.ShakePos.x = g_Param.ShakePos.x + Perlin(SimT, fa_test, 2);
		g_Param.ShakePos.y = g_Param.ShakePos.y + Perlin(1000 + SimT, fa_test, 2);
		g_Param.ShakePos.z = g_Param.ShakePos.z + Perlin(2000 + SimT, fa_test, 2);
		g_Param.focus_vessel->SetCameraOffset(g_Param.ShakePos);
		return;		
	}
	// or has a test of the ground shake been requested?
	else if (g_Param.hDlg && (SendDlgItemMessage(g_Param.hDlg, IDC_BTN_TEST_GD, BM_GETSTATE, 0, 0) & BST_PUSHED))
	{
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_GD_FREQ1, cbuf, 128);
		fa_test[0] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_GD_AMP1, cbuf, 128);
		fa_test[1] = atof(cbuf)/100.0;
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_GD_FREQ2, cbuf, 128);
		fa_test[2] = atof(cbuf);
		GetDlgItemText(g_Param.hDlg, IDC_EDIT_GD_AMP2, cbuf, 128);
		fa_test[3] = atof(cbuf)/100.0;
		if (!fa_test[0]) fa_test[1] = 0;  // no frequency, no amplitude!
		if (!fa_test[2]) fa_test[3] = 0;  // no frequency, no amplitude!
		g_Param.ShakePos = g_Param.StartPos;
		g_Param.ShakePos.x = g_Param.ShakePos.x + Perlin(SimT, fa_test, 2);
		g_Param.ShakePos.y = g_Param.ShakePos.y + Perlin(1000 + SimT, fa_test, 2);
		g_Param.ShakePos.z = g_Param.ShakePos.z + Perlin(2000 + SimT, fa_test, 2);
		g_Param.focus_vessel->SetCameraOffset(g_Param.ShakePos);
		return;		
	}


	// do not continue if no shake effect is enabled
	if (!g_Param.th_enabled & !g_Param.at_enabled & !g_Param.gd_enabled) return;

	double lvl_thrust = GetCurrentThrust0(g_Param.parent_vessel) / g_Param.max_thrust;
	double flamep = g_Param.parent_vessel->GetDynPressure() * g_Param.parent_vessel->GetAirspeed();
	double lvl_atmos = min(1.0, max(0, flamep - 40000000) / 100000000);
	double lvl_ground = min(g_Param.parent_vessel->GetAirspeed() / g_Param.gd_max_speed, 1.0);
	if (!g_Param.th_enabled) lvl_thrust = 0.0;
	if (!g_Param.at_enabled) lvl_atmos = 0.0;
	if (!g_Param.gd_enabled || !g_Param.parent_vessel->GroundContact()) lvl_ground = 0.0;

	g_Param.ShakePos = g_Param.StartPos;
	g_Param.ShakePos.x = g_Param.ShakePos.x + lvl_thrust * Perlin(SimT, g_Param.fa_thrust, 2);
	g_Param.ShakePos.y = g_Param.ShakePos.y + lvl_thrust * Perlin(1000 + SimT, g_Param.fa_thrust, 2);
	g_Param.ShakePos.z = g_Param.ShakePos.z + lvl_thrust * Perlin(2000 + SimT, g_Param.fa_thrust, 2);
	g_Param.ShakePos.x = g_Param.ShakePos.x + lvl_atmos * Perlin(3000 + SimT, g_Param.fa_atmos, 2);
	g_Param.ShakePos.y = g_Param.ShakePos.y + lvl_atmos * Perlin(4000 + SimT, g_Param.fa_atmos, 2);
	g_Param.ShakePos.z = g_Param.ShakePos.z + lvl_atmos * Perlin(5000 + SimT, g_Param.fa_atmos, 2);
	g_Param.ShakePos.x = g_Param.ShakePos.x + lvl_ground * Perlin(6000 + lvl_ground * SimT, g_Param.fa_ground, 2);
	g_Param.ShakePos.y = g_Param.ShakePos.y + lvl_ground * Perlin(7000 + lvl_ground * SimT, g_Param.fa_ground, 2);
	g_Param.ShakePos.z = g_Param.ShakePos.z + lvl_ground * Perlin(8000 + lvl_ground * SimT, g_Param.fa_ground, 2);
	g_Param.focus_vessel->SetCameraOffset(g_Param.ShakePos);

}

// Dialog message callback function
BOOL CALLBACK DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char cbuf[128];

	switch (uMsg) {
	case WM_DESTROY:
		g_Param.hDlg = NULL;
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTN_OK:
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_FREQ1, cbuf, 128);
			g_Param.fa_thrust[0] = atof(cbuf);
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_AMP1, cbuf, 128);
			g_Param.fa_thrust[1] = atof(cbuf)/100.0;
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_FREQ2, cbuf, 128);
			g_Param.fa_thrust[2] = atof(cbuf);
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_TH_AMP2, cbuf, 128);
			g_Param.fa_thrust[3] = atof(cbuf)/100.0;
			if (!g_Param.fa_thrust[0]) g_Param.fa_thrust[1] = 0;  // no frequency, no amplitude!
			if (!g_Param.fa_thrust[2]) g_Param.fa_thrust[3] = 0;  // no frequency, no amplitude!
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_FREQ1, cbuf, 128);
			g_Param.fa_atmos[0] = atof(cbuf);
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_AMP1, cbuf, 128);
			g_Param.fa_atmos[1] = atof(cbuf)/100.0;
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_FREQ2, cbuf, 128);
			g_Param.fa_atmos[2] = atof(cbuf);
			GetDlgItemText(g_Param.hDlg, IDC_EDIT_AT_AMP2, cbuf, 128);
			g_Param.fa_atmos[3] = atof(cbuf)/100.0;
			if (!g_Param.fa_atmos[0]) g_Param.fa_atmos[1] = 0;  // no frequency, no amplitude!
			if (!g_Param.fa_atmos[2]) g_Param.fa_atmos[3] = 0;  // no frequency, no amplitude!
			StoreVesselParameters();
		case IDC_BTN_CANCEL:
		case IDCANCEL:
			oapiCloseDialog (hWnd);
			return TRUE;
		case IDC_CHK_NOSTAGING:
			g_Param.no_staging = IsDlgButtonChecked(hWnd, IDC_CHK_NOSTAGING) == BST_CHECKED;
			return TRUE;
		case IDC_CHK_THRUST_ENABLED:
			g_Param.th_enabled = IsDlgButtonChecked(hWnd, IDC_CHK_THRUST_ENABLED) == BST_CHECKED;
			return TRUE;
		case IDC_CHK_ATMO_ENABLED:
			g_Param.at_enabled = IsDlgButtonChecked(hWnd, IDC_CHK_ATMO_ENABLED) == BST_CHECKED;
			return TRUE;
		case IDC_CHK_GROUND_ENABLED:
			g_Param.gd_enabled = IsDlgButtonChecked(hWnd, IDC_CHK_GROUND_ENABLED) == BST_CHECKED;
			return TRUE;
		}
		break;
	}
	return oapiDefDialogProc (hWnd, uMsg, wParam, lParam);
}

