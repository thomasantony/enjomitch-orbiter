#define STRICT
#define ORBITER_MODULE 
#include "orbitersdk.h"
#include "EnergyMFDmk2.hpp" 
#include <string>
#include <fstream>
#include <vector>

#define GREEN		RGB(0x00, 0xFF, 0x00)
#define BLUEGREEN	RGB(0x00, 0xDD, 0xCC)
#define YELLOW		RGB(0xDF, 0xDF, 0x00)
#define	WHITE		RGB(0xFF, 0xFF, 0xFF)
#define RED			RGB(0xFF, 0x00, 0x00)
#define GREY		RGB(0xA0, 0xA0, 0xA0)

#define MFDTextOut(hDC, x, y, colour, text, ...)	{char buf[1024]; sprintf_s(buf, 1024, text, __VA_ARGS__); \
														if(colour != 0)\
															SetTextColor(hDC, colour);\
														TextOut(hDC, x, y, buf, (int)strlen(buf));\
} 

using namespace std;

vector <double> vpe,vke,vme;
const double maxDist = 400e6;
double time = 0;
const int samplesPerPeriod = 150;
const int periods = 3;
int count = 0;

static struct {  // global data storage
	double tnext;  // time of next sample
	int   sample;  // current sample index
	float *time;   // sample time
	float *ke;    // kinetic energy data
	float *pe;  // potential energy data
	float *me;   // mechanical energy data
} g_Data;


void save();
ENERGIES CalculateEnergies(VESSEL * vessel, OBJHANDLE hRef);

enum si
{
	none,
	kilo,
	Mega,
	Giga,
	Tera
};

enum eeselection 
{
	KE = 0,
	PE,
	ME,
	//KEPE,
	//KEME,
	//PEME,
	ALL
};

enum egraphMode
{
	AUTO = 0,
	ZERO_BASED,
	ABSOL,
	MANUAL
};

enum edispMode
{
	BOTH = 0,
	GRAPHS,
	WORDS
};

short dispMode = BOTH;

short eselection = ALL;
const int ndata = 200;  // data points
double sample_dt; // data point interval
float sel_sample_dt = 0;
const double def_sample_dt = 150.0; // data point interval
const double init_sample_dt = 0.01;
//const double rt_sample_dt = 0.2;

// one state for every graph
short graphMode[3] = {AUTO, AUTO, AUTO};
bool ready = false;
OBJHANDLE hRef = NULL;
int g_MFDmode; // identifier for new MFD mode
 
DLLCLBK void InitModule (HINSTANCE hModule) // perform global module initialisation here 
{   
	static char *name = "EnergyMFDmk2";   // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_E;                // MFD mode eselection key
	spec.msgproc = EnergyMFDmk2::MsgProc;  // MFD mode callback function
	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);  
	
	g_Data.tnext  = 0.0;
	g_Data.sample = 0;
	g_Data.time = new float[ndata];   memset (g_Data.time, 0, ndata*sizeof(float));
	g_Data.ke   = new float[ndata];   memset (g_Data.ke,   0, ndata*sizeof(float));
	g_Data.pe   = new float[ndata];   memset (g_Data.pe,   0, ndata*sizeof(float));
	g_Data.me   = new float[ndata];   memset (g_Data.me,   0, ndata*sizeof(float));
} 

DLLCLBK void ExitModule (HINSTANCE hModule) // perform module cleanup here 
{ 
	oapiUnregisterMFDMode (g_MFDmode);
	delete []g_Data.time;
	delete []g_Data.ke;
	delete []g_Data.pe;
	delete []g_Data.me;
} 

DLLCLBK void opcPreStep (double simt, double simdt, double mjd) // call every timestep
{
	VESSEL *vessel = oapiGetFocusInterface();
	if (vessel == NULL) return;
	//hRef = vessel->GetSurfaceRef();
	if (hRef == NULL) return;	
	if (!ready) 
	{
		//VESSELSTATUS2 vs2;
		//vessel->GetStatusEx(vs2);
	//	if 
		sample_dt = init_sample_dt;	
	}
	double alt = vessel->GetAltitude(); // launched?
	if (simt >= g_Data.tnext && alt > vessel->GetSize())
	{
		
		VECTOR3 pos;
		vessel->GetRelativePos(hRef, pos);
		if (length(pos) > maxDist) return;
		ENERGIES energ = CalculateEnergies(vessel, hRef);
		while (simt > 3.4e38)
			simt -= 3.4e38;
		g_Data.time[g_Data.sample] = (float)simt;
		g_Data.ke[g_Data.sample] = energ.ke;
		g_Data.pe[g_Data.sample] = energ.pe;
		// mechanical energy
		g_Data.me[g_Data.sample] = energ.me;
		if (!ready && g_Data.sample + 1 == ndata)
		{
			ready = true;
			sample_dt = sel_sample_dt == 0 ? def_sample_dt : sel_sample_dt;
			//g_Data.tnext  = 0.0;
	//g_Data.sample = 0;
	//memset (g_Data.time, 0, ndata*sizeof(float));
	//  memset (g_Data.ke,   0, ndata*sizeof(float));
	//  memset (g_Data.pe,   0, ndata*sizeof(float));
	//  memset (g_Data.me,   0, ndata*sizeof(float));
		}
		g_Data.sample = (g_Data.sample+1) % ndata;
		g_Data.tnext += sample_dt;	
		/*
		if (oapiGetTimeAcceleration() >= 100000)
		{
			g_Data.tnext  = 0.0;
			g_Data.sample = 0;
			memset (g_Data.time,  0, ndata*sizeof(float));
			memset (g_Data.ke,  0, ndata*sizeof(float));
			memset (g_Data.pe,   0, ndata*sizeof(float));
			memset (g_Data.me, 0, ndata*sizeof(float));
		}
			*/
	}
	
/*	
	time += simdt;
	if (time > sampling)
	{
		time = 0;
		vpe.push_back(pe);
		vke.push_back(ke);
		vme.push_back(me);
		count ++;
	}
	if (count > periods * samplesPerPeriod)
	{
		count = 0;
		
		vpe.clear();
		vke.clear();
		vme.clear();
	}
*/	
	//save();
//	sprintf (oapiDebugString(), "PE = %0.0f, KE = %0.0f, ME = %0.0f", pe / 10e6, ke / 10e6, me / 10e6);
	//sprintf (oapiDebugString(), "%d", count);
}

void save ()
{
	ofstream file("energies.sci");
	if (file.is_open())
	{
		file << "pe = [\n";
		for (unsigned int i = 0; i < vpe.size();  i++)
		{
			file << vpe.at(i) << "\n";
		}
		file << "\n];"; 
		file << "\nke = [\n";
		for (unsigned int i = 0; i < vke.size();  i++)
		{
			file << vke.at(i) << "\n";
		}
		file <<"\n];";
		file << "\nme = [\n"; 
		for (unsigned int i = 0; i < vme.size();  i++)
		{
			file << vme.at(i) << "\n";
		}
		file <<"\n];";
		file.close();
	}
}

EnergyMFDmk2::EnergyMFDmk2(DWORD w, DWORD h, VESSEL *vessel) 
: GraphMFD (w, h, vessel)
{	
	if (hRef == NULL) hRef = vessel->GetSurfaceRef();
	int g;
	g = AddGraph ();
	SetAxisTitle (g, 0, "Time: s");
	SetAxisTitle (g, 1, "KE: J");
	AddPlot (g, g_Data.time, g_Data.ke, ndata, 1, &g_Data.sample);
	
	g = AddGraph ();
	SetAxisTitle (g, 0, "Time: s");
	SetAxisTitle (g, 1, "PE: J");
	AddPlot (g, g_Data.time, g_Data.pe, ndata, 5, &g_Data.sample);
	
	g = AddGraph ();
	SetAxisTitle (g, 0, "Time: s");
	SetAxisTitle (g, 1, "ME: J");
	AddPlot (g, g_Data.time, g_Data.me, ndata, 1, &g_Data.sample);
	
	this->vessel = vessel;
}

EnergyMFDmk2::~EnergyMFDmk2 ()
{

}

int EnergyMFDmk2::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		return (int)(new EnergyMFDmk2 (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

bool EnergyMFDmk2::ConsumeKeyBuffered (DWORD key)
{
	bool EnergyInput (void *id, char *str, void *data);
	bool SampleTInput (void *id, char *str, void *data);
	bool TargetInput (void *id, char *str, void *data);
	
	//bool VtanInput (void *id, char *str, void *data);

	switch (key) {
	case OAPI_KEY_G:
		hRef = vessel->GetGravityRef();
	return true;
	case OAPI_KEY_S:
		hRef = vessel->GetSurfaceRef();
	return true;
	case OAPI_KEY_T:
		oapiOpenInputBox ("Enter target", TargetInput, 0, 20, (void*)this);	
	return true;
	case OAPI_KEY_A:
		oapiOpenInputBox ("Sampling time (s) ['d' for default = 150 s]:", SampleTInput, 0, 20, (void*)this);	
	return true;
	case OAPI_KEY_L: // graph seLection
		// if we reached the last eselection, reset to zero
		if (eselection == ALL) eselection = 0;
		else eselection++;
	return true;
	case OAPI_KEY_E:
		oapiOpenInputBox ("Energy range (J) [min max (ex. -11e12 23e11) -or- 'a' for auto, 'b' for absolute, 'z' for zero based]", EnergyInput, 0, 20, (void*)this);
	return true;
	case OAPI_KEY_M: //disp mode selection
		// if we reached the last eselection, reset to zero
		if (dispMode == WORDS) dispMode = 0;
		else dispMode++;
	return true;
	case OAPI_KEY_R:
		ready = false;
		g_Data.tnext  = oapiGetSimTime();
		g_Data.sample = 0;
		memset (g_Data.time,  0, ndata*sizeof(float));
		memset (g_Data.ke,  0, ndata*sizeof(float));
		memset (g_Data.pe,   0, ndata*sizeof(float));
		memset (g_Data.me, 0, ndata*sizeof(float));
	return true;

	
	//case OAPI_KEY_P:
	//	page = (page+1) % 2;
	//	return true;
	//case OAPI_KEY_R:
//		oapiOpenInputBox ("Vrad range (km/s) [min max, or 'a' for auto]:", VradInput, 0, 20, (void*)this);
	//	return true;
	//case OAPI_KEY_T:
//		oapiOpenInputBox ("Vtan range (km/s) [min max, or 'a' for auto]:", VtanInput, 0, 20, (void*)this);
	//	return true;
	
	default:
		return (false);
	}
	return false;
}

bool EnergyMFDmk2::ConsumeButton (int bt, int event)
{
	if (event & PANEL_MOUSE_LBDOWN) {
		switch(bt) {
			case 0:	ConsumeKeyBuffered(OAPI_KEY_G);			break;
			case 1:	ConsumeKeyBuffered(OAPI_KEY_S);			break;
			case 2: ConsumeKeyBuffered(OAPI_KEY_T);			break;
			case 3:	ConsumeKeyBuffered(OAPI_KEY_A);			break;	
			case 4: ConsumeKeyBuffered(OAPI_KEY_L);			break;
			case 5: ConsumeKeyBuffered(OAPI_KEY_E);			break;
			case 6: ConsumeKeyBuffered(OAPI_KEY_R);			break;
			case 7: ConsumeKeyBuffered(OAPI_KEY_M);			break;

			default: return(false);	break;
		};
	}

	return(true);
}

char *EnergyMFDmk2::ButtonLabel (int bt)
{
	// The labels for the two buttons used by our MFD mode
	//static char *label[] = {"I +", "I -","Iad","A +","A -","Aad","TGT","AUT","SND","MOD","CLC","VW"};
	static char *label[] = {"Gr","Sr","Tgt","sAm","seL","Er","Rst","Mod"};
	return (bt < sizeof(label) / sizeof(char*) ? label[bt] : 0);
}

// Return button menus
int EnergyMFDmk2::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the two buttons
	static const MFDBUTTONMENU mnu[] = {
		{"get Gravity ref", 0, 'G'},
		{"get Surface ref", 0, 'S'},
		{"enter Target", 0, 'S'},
		{"sAmple time", 0, 'A'},	
		{"seLect graph", 0, 'L'},
		{"set Energy range", 0, 'E'},
		{"Reset graphs", 0, 'R'},
		{"display Mode", 0, 'M'}
	};
	if (menu) *menu = mnu;
	return sizeof(mnu) / sizeof(MFDBUTTONMENU); // return the number of buttons used
}

void EnergyMFDmk2::Update (HDC hDC)
{
	Title (hDC, "Energy MFD mk2");
	
	//if (ready)
	{
	switch (dispMode)
	{
		case BOTH:
		//	SetYRange();
		//	SetXRangePlot(hDC);
		//	DrawSelection(hDC);
		//	DrawWords(hDC);
		//break;
		case GRAPHS:
			SetYRange();
			SetXRangePlot(hDC);
			DrawSelection(hDC);
			DrawWords(hDC);
		break;
		case WORDS:
			DrawWords(hDC);
		break;
	}
		
		//int g = 0;
		
		
			//sprintf (oapiDebugString(), "absmin %0.2f,  absmax %0.2f", absmin, absmax);
			//sprintf (oapiDebugString(), "KE %d,  ME %d", graphMode[KE], graphMode[ME]);
			

		
		//g = 0;
		
		
		
	}

	

	
}

void EnergyMFDmk2::FindRetRange (float * data, float * min, float * max)
{
	float tmin = data[0], tmax = data[0], tmp;
	//FindRange (data, ndata, tmin, tmax);
	for (int i=0; i < ndata; i++)
	{
		if (tmin > data[i])// && data[i] != 0)
			tmin = data[i];
		if (tmax < data[i])// && data[i] != 0)
			tmax = data[i];
	}

	if (tmin > tmax)
		tmp = tmin, tmin = tmax, tmax = tmp;
	if (tmin == tmax)
		tmin -= 0.5, tmax += 0.5;
	//if (abs(tmax - tmin) < 0.1 * abs(tmax))
	//	tmin -= float(0.1) * tmin, tmax += float(0.1) * tmax;
	//if (tmin == 0) tmin = tmax - (float)(0.1) * tmax;
	*min = tmin;
	*max = tmax;
}

bool EnergyMFDmk2::SetEnergyRange (char *rstr)
{
	float emin, emax, etemp;
	if (rstr[0] == 'a' || rstr[0] == 'A') 
	{
		switch (eselection)
		{
			case ALL:
				graphMode[KE] = AUTO;
				graphMode[PE] = AUTO;
				graphMode[ME] = AUTO;
			return true;
			case KE:
				graphMode[KE] = AUTO;
			return true;
			case PE:
				graphMode[PE] = AUTO;
			return true;
			case ME:
				graphMode[ME] = AUTO;
			return true;
			/*
			case KEPE:
				graphMode[KE] = AUTO;
				graphMode[PE] = AUTO;
			return true;
			case KEME:
				graphMode[KE] = AUTO;
				graphMode[ME] = AUTO;
			return true;
			case PEME:
				graphMode[PE] = AUTO;
				graphMode[ME] = AUTO;
			return true;
			*/
			default:
			return true;
			
		}
	} 
	else if (rstr[0] == 'z' || rstr[0] == 'Z')
	{
    switch (eselection)
		{
			case ALL:
				graphMode[KE] = ZERO_BASED;
				graphMode[PE] = ZERO_BASED;
				graphMode[ME] = ZERO_BASED;
			return true;
			case KE:
				graphMode[KE] = ZERO_BASED;
			return true;
			case PE:
				graphMode[PE] = ZERO_BASED;
			return true;
			case ME:
				graphMode[ME] = ZERO_BASED;
			return true;
			/*
			case KEPE:
				graphMode[KE] = ZERO_BASED;
				graphMode[PE] = ZERO_BASED;
			return true;
			case KEME:
				graphMode[KE] = ZERO_BASED;
				graphMode[ME] = ZERO_BASED;
			return true;
			case PEME:
				graphMode[PE] = ZERO_BASED;
				graphMode[ME] = ZERO_BASED;
			return true;
			*/
			default:
			return true;
		}
		return true;
	}
	else if (rstr[0] == 'b' || rstr[0] == 'B')
	{
     switch (eselection)
		{
			case ALL:
				graphMode[KE] = ABSOL;
				graphMode[PE] = ABSOL;
				graphMode[ME] = ABSOL;
			return true;
			case KE:
				graphMode[KE] = ABSOL;
			return true;
			case PE:
				graphMode[PE] = ABSOL;
			return true;
			case ME:
				graphMode[ME] = ABSOL;
			return true;
			/*
			case KEPE:
				graphMode[KE] = ABSOL;
				graphMode[PE] = ABSOL;
			return true;
			case KEME:
				graphMode[KE] = ABSOL;
				graphMode[ME] = ABSOL;
			return true;
			case PEME:
				graphMode[PE] = ABSOL;
				graphMode[ME] = ABSOL;
			return true;
			*/
			default:
			return true;
		}
		return true;
	}
	else if (sscanf (rstr, "%f%f", &emin, &emax) == 2) {
		if (emax < emin)
		{
			etemp = emax;
			emax = emin;
			emin = etemp;
		}
		switch (eselection)
		{
		case ALL:
			graphMode[KE] = MANUAL;
			graphMode[PE] = MANUAL;
			graphMode[ME] = MANUAL;
			SetRange (KE, 1, emin, emax);
			SetRange (PE, 1, emin, emax);
			SetRange (ME, 1, emin, emax);
		return true;
		case KE:
			graphMode[KE] = MANUAL;
			SetRange (KE, 1, emin, emax);
		return true;
		case PE:
			graphMode[PE] = MANUAL;
			SetRange (PE, 1, emin, emax);
		return true;
		case ME:
			graphMode[ME] = MANUAL;
			SetRange (ME, 1, emin, emax);
		return true;
		/*
		case KEPE:
			graphMode[KE] = MANUAL;
			graphMode[PE] = MANUAL;
			SetRange (KE, 1, emin, emax);
			SetRange (PE, 1, emin, emax);
		return true;
		case KEME:
			graphMode[KE] = MANUAL;
			graphMode[ME] = MANUAL;
			SetRange (KE, 1, emin, emax);
			SetRange (ME, 1, emin, emax);
		return true;
		case PEME:
			graphMode[PE] = MANUAL;
			graphMode[ME] = MANUAL;
			SetRange (PE, 1, emin, emax);
			SetRange (ME, 1, emin, emax);
		return true;
		*/
		default:
		return true;
		}
		return true;	
	}
	return false;
}

bool EnergyInput (void *id, char *str, void *data)
{
	return ((EnergyMFDmk2*)data)->SetEnergyRange (str);
}

bool TargetInput (void *id, char *str, void *data)
{
	return ((EnergyMFDmk2*)data)->SetTarget (str);
}

bool EnergyMFDmk2::SetSampleT (char *rstr)
{
	if (rstr[0] == 'd' || rstr[0] == 'D') {
		sel_sample_dt = (float)def_sample_dt;
		sample_dt = sel_sample_dt;
		return true;
	} else if (sscanf (rstr, "%f", &sel_sample_dt) == 1) {
		sample_dt = sel_sample_dt >= 1 ? sel_sample_dt : 1;
		return true;
	}
	return false;
}

bool EnergyMFDmk2::SetTarget (char *rstr)
{
	//OBJHANDLE thRef = oapiGetGbodyByName(rstr);
	OBJHANDLE thRef = oapiGetObjectByName(rstr);
	if (thRef) {
		hRef = thRef;
		return true;
	}
	return false;
}

bool SampleTInput (void *id, char *str, void *data)
{
	return ((EnergyMFDmk2*)data)->SetSampleT (str);
}

void EnergyMFDmk2::SetAbsMinMax()
{
	float min,max;
	absmin = absmax = 0;
	if (graphMode[KE] == ABSOL || graphMode[PE] == ABSOL || graphMode[ME] == ABSOL)
	{
		FindRetRange(g_Data.ke, &min, &max);
		absmin = min(absmin, min);
		absmax = max(absmax, max);
		FindRetRange(g_Data.pe, &min, &max);
		absmin = min(absmin, min);
		absmax = max(absmax, max);
		FindRetRange(g_Data.me, &min, &max);
		absmin = min(absmin, min);
		absmax = max(absmax, max);
	}
}

void EnergyMFDmk2::SetYRange()
{
	SetAbsMinMax();
	float min, max;
	switch (graphMode[KE])
	{
		case AUTO:
			FindRetRange(g_Data.ke, &min, &max);
			SetRange (KE, 1, min, max);
		break;
		case ZERO_BASED:
			FindRetRange(g_Data.ke, &min, &max);
			SetRange (KE, 1, 0, max);
		break;
		case ABSOL:
			//FindRetRange(g_Data.ke, &min, &max);
			SetRange (KE, 1, absmin, absmax);
		break;
		default:
		break;
	}
	
	switch (graphMode[PE])
	{
		case AUTO:
			FindRetRange(g_Data.pe, &min, &max);
			SetRange (PE, 1, min, max);
		break;
		case ZERO_BASED:
			FindRetRange(g_Data.pe, &min, &max);
			SetRange (PE, 1, min, 0);
		break;
		case ABSOL:
			//FindRetRange(g_Data.ke, &min, &max);
			SetRange (PE, 1, absmin, absmax);
		break;
		default:
		break;
	}
	
	switch (graphMode[ME])
	{
		// this mode hangs Orbiter
		case AUTO:
		//	FindRetRange(g_Data.me, &min, &max);
		//	SetRange (ME, 1, min, max);	
		//break;
		case ZERO_BASED:
			FindRetRange(g_Data.me, &min, &max);	
			// whithout these two lines orbiter hangs
			min = min - (float)(0.05) * abs(min);
			max = max + (float)(0.05) * abs(max);
			SetRange (ME, 1, min, max);
		break;
		case ABSOL:
			//FindRetRange(g_Data.ke, &min, &max);
			SetRange (ME, 1, absmin, absmax);
		break;
		default:
		break;
	}
}

void EnergyMFDmk2::SetXRangePlot(HDC hDC)
{
	SetAutoRange (KE, 0);
	Plot (hDC, KE, ch, (H+ch)/3, "Kinetic energy");
	
	SetAutoRange (PE, 0);
	Plot (hDC, PE, (H+ch)/3, (H+ch)*2/3, "Potential energy");
	
	SetAutoRange (ME, 0);
	Plot (hDC, ME, (H+ch)*2/3, H, "Mechanical energy");
}

void EnergyMFDmk2::DrawSelection(HDC hDC)
{
	switch (eselection)
	{
		case ALL:
			MoveToEx (hDC,W-3,ch, NULL);
			LineTo (hDC,W-3,(H+ch)/3);
			MoveToEx (hDC,W-3,(H+ch)/3, NULL);
			LineTo (hDC,W-3,(H+ch)*2/3);
			MoveToEx (hDC,W-3,(H+ch)*2/3, NULL);
			LineTo (hDC,W-3,(H+ch));
		break;
		case KE:
			MoveToEx (hDC,W-3,0, NULL);
			LineTo (hDC,W-3,(H+ch)/3);
		break;
		case PE:
			MoveToEx (hDC,W-3,(H+ch)/3, NULL);
			LineTo (hDC,W-3,(H+ch)*2/3);
		break;
		case ME:
			MoveToEx (hDC,W-3,(H+ch)*2/3, NULL);
			LineTo (hDC,W-3,(H+ch));
		break;
		/*
		case KEPE:
			MoveToEx (hDC,W-1,ch, NULL);
			LineTo (hDC,W-1,(H+ch)*2/3);
		break;
		case KEME:
			MoveToEx (hDC,W-1,0, NULL);
			LineTo (hDC,W-1,(H+ch)/3);
			MoveToEx (hDC,W-1,(H+ch)*2/3, NULL);
			LineTo (hDC,W-1,(H+ch));
		break;
		case PEME:
			MoveToEx (hDC,W-1,(H+ch)/3, NULL);
			LineTo (hDC,W-1,(H+ch));
		break;
		*/
		default:
		break;
	}
}

void EnergyMFDmk2::DrawWords(HDC hDC)
{
	char * cKE, *cPE, * cME;
	char * cKEMode, * cPEMode, * cMEMode;
	char * cSampleT = "Sampling time %0.0lf s";
	char * cRefFar = "Ref. body too far away";
	char cRef[512] = "Ref. body: ";
	char buf[256];
	oapiGetObjectName(hRef, buf, 256);
	strcat(cRef, buf);
	ENERGIES energ = CalculateEnergies(vessel, hRef);
	//sprintf (oapiDebugString(), "ke %0.2f",  g_Data.ke[g_Data.sample]);
	//sprintf (oapiDebugString(), "samle %d",  g_Data.sample);
	
	short multipl = 0;
	while (abs(energ.ke) > 1000)
	{	
		energ.ke /= 1000;
		multipl ++;
	}
	switch (multipl)
	{
		case none:
			cKE = "KE = %.2f";
		break;
		case kilo:
			cKE = "KE = %.2f k";
		break;
		case Mega:
			cKE = "KE = %.2f M";
		break;
		case Giga:
			cKE = "KE = %.2f G";
		break;
		case Tera:
			cKE = "KE = %.2f T";
		break;
		default:
			cKE = "KE = %.2f ?";
		break;
	}
	
	multipl = 0;
	while (abs(energ.pe) > 1000)
	{	
		energ.pe /= 1000;
		multipl ++;
	}
	switch (multipl)
	{
		case none:
			cPE= "PE = %.2f";
		break;
		case kilo:
			cPE = "PE = %.2f k";
		break;
		case Mega:
			cPE = "PE = %.2f M";
		break;
		case Giga:
			cPE = "PE = %.2f G";
		break;
		case Tera:
			cPE = "PE = %.2f T";
		break;
		default:
			cPE = "PE = %.2f ?";
		break;
	}
	
	multipl = 0;
	while (abs(energ.me) > 1000)
	{	
		energ.me /= 1000;
		multipl ++;
	}
	switch (multipl)
	{
		case none:
			cME = "ME = %.2f";
		break;
		case kilo:
			cME = "ME = %.2f k";
		break;
		case Mega:
			cME = "ME = %.2f M";
		break;
		case Giga:
			cME = "ME = %.2f G";
		break;
		case Tera:
			cME = "ME = %.2f T";
		break;
		default:
			cME = "ME = %.2f ?";
		break;
	}

	switch (graphMode[KE])
	{
		case AUTO:
			cKEMode = "Mode - automatic";
		break;
		case ZERO_BASED:
			cKEMode = "Mode - zero based";
		break;
		case ABSOL:
			cKEMode = "Mode - absolute";
		break;
		case MANUAL:
			cKEMode = "Mode - manual";
		break;
		default:
			cKEMode = "Mode - unknown";
		break;
	}
	switch (graphMode[PE])
	{
		case AUTO:
			cPEMode = "Mode - automatic";
		break;
		case ZERO_BASED:
			cPEMode = "Mode - zero based";
		break;
		case ABSOL:
			cPEMode = "Mode - absolute";
		break;
		case MANUAL:
			cPEMode = "Mode - manual";
		break;
		default:
			cPEMode = "Mode - unknown";
		break;
	}
	switch (graphMode[ME])
	{
		case AUTO:
			cMEMode = "Mode - automatic";
		break;
		case ZERO_BASED:
			cMEMode = "Mode - zero based";
		break;
		case ABSOL:
			cMEMode = "Mode - absolute";
		break;
		case MANUAL:
			cMEMode = "Mode - manual";
		break;
		default:
			cMEMode = "Mode - unknown";
		break;
	}
	VECTOR3 pos;
	vessel->GetRelativePos(hRef, pos);
	DWORD xOffsetEn = 0, xOffsetSet = 0;
	switch (dispMode)
	{
		case GRAPHS:
		case BOTH:
			xOffsetEn = cw*32;
			xOffsetSet = cw*10;
		break;
		case WORDS:
			xOffsetEn = xOffsetSet = cw*3;
		break;
		default:
		break;
	}
	
	if (dispMode == BOTH || dispMode == WORDS)
	{
		MFDTextOut(hDC, xOffsetSet, ch*3, 	GREY, cSampleT, sample_dt);
		MFDTextOut(hDC, xOffsetSet, ch*5, 	GREY, cKEMode);
		MFDTextOut(hDC, xOffsetSet, ch*10,	GREY, cRef);
		MFDTextOut(hDC, xOffsetSet, ch*12,	GREY, cPEMode);
		if (length(pos) > maxDist)
		MFDTextOut(hDC, xOffsetSet, ch*17,	RED, cRefFar);
		MFDTextOut(hDC, xOffsetSet, ch*19,	GREY, cMEMode);
	}
	MFDTextOut(hDC, xOffsetEn, ch*7, 	BLUEGREEN, cKE, energ.ke);	
	MFDTextOut(hDC, xOffsetEn, ch*15,	BLUEGREEN, cPE, energ.pe);	
	MFDTextOut(hDC, xOffsetEn, ch*21,	BLUEGREEN, cME, energ.me);
	

	
	
	//MFDTextOut(hDC, (int)(strlen(cE) * cw + cw*5), (int)(line_offset++* ch), GREEN, cMultipl, ke);
	//MFDTextOut(hDC, 0, (line_offset)* linespacing, YELLOW, SOUTHERN);	
}

ENERGIES CalculateEnergies(VESSEL * vessel, OBJHANDLE hRef)
{
	ENERGIES energ;
	memset (&energ, 0, sizeof(energ));
	if (vessel == NULL || hRef == NULL)
		return energ;
	VECTOR3 pos;
	
	vessel->GetRelativePos(hRef, pos);
	double rad = length(pos);
	VECTOR3 relVel;
	vessel->GetRelativeVel(hRef, relVel);
	double shipVel = length(relVel);
	double shipMass = vessel->GetMass();
	// kinetic energy
	energ.ke = static_cast<float>(shipMass * shipVel * shipVel / 2); 
	//vessel->GetElements(hRef, e, &op);
	//double sampling = op.T / samplesPerPeriod;		
	
	double bodyMass = oapiGetMass(hRef);
	
	// potential energy
	energ.pe = static_cast<float>(- GGRAV * shipMass * bodyMass / rad);
	energ.me = energ.ke + energ.pe;
	return energ;
}