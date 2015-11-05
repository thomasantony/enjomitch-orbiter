/****************************************************************************
  This file is part of Multistage2015
  
  Multistage2015 is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Multistage2015 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Multistage2015; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA*/
// ==============================================================
//						MultiStage2015
//                  
//					       By Fred18
//                  
//
// Multistage2015.h
// ==============================================================


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define ORBITER_MODULE


#define MAXLEN 4096
#define TBOOSTER 0
#define TSTAGE 1
#define TPAYLOAD 2
#define TFAIRING 3
#define TINTERSTAGE 4
#define TLES 5

#define FXMACH 0
#define FXVENT 1
//#define ORBITER_MODULE

#define TLMSECS 7200

#include <iostream>
#include <fstream>
#include <string>
#include "OrbiterSoundSDK40.h"
#include <time.h>
#include "orbitersdk.h"
using namespace std;

struct VECTOR4{
	double x;
	double y;
	double z;
	double t;
};
struct VECTOR2{
	double x;
	double y;
};
struct CUTOFF_CONDITIONS{
	double T;
	int stage;
	int rem_stages;
	double r_T;
	double rdot_T;
	double remBT[10];
};
struct BATTS{
	bool wBatts;
	double MaxCharge;
	double CurrentCharge;
};
struct MISC{
	double COG;
	int GNC_Debug;
	bool telemetry;
	int Focus;
	bool thrustrealpos;
};
struct TEX{
	char TextureName[MAXLEN][16];
	SURFHANDLE hTex[16];
	};
struct PARTICLE{
		char ParticleName[MAXLEN][16];
		PARTICLESTREAMSPEC Pss[16];
	};
struct BOOSTER{
	//mandatory
	int N;
	char meshname[MAXLEN];
	VECTOR3 off;
	double height;
	double diameter;
	double thrust;
	double emptymass;
	double fuelmass;
	double burntime;	
	double angle;
	//unmandatory
	double burndelay;
	VECTOR3 speed;
	VECTOR3 rot_speed;
	char module[MAXLEN];
	VECTOR3 eng[4];
	double engine_phase[32];
	double engine_amp[32];
	double freq[32];
	double eng_diameter;
	char eng_tex[MAXLEN];
	char eng_pstream1[MAXLEN];
	char eng_pstream2[MAXLEN];
	VECTOR3 eng_dir;
	THGROUP_HANDLE Thg_boosters_h;
	//derived parameters
	double isp;
	double volume;
	PROPELLANT_HANDLE tank;
	THRUSTER_HANDLE th_booster_h[10];
	MESHHANDLE msh_h[10];
	int msh_idh[10];
	int nEngines;
	double currDelay;
	VECTOR3 curve[10];
	double IgnitionTime;
	bool Ignited;
	bool PSSdefined;
};
struct INTERSTAGE{
	//mandatory
	char meshname[MAXLEN];
	VECTOR3 off;
	double height;
	double diameter;
	double volume;
	double emptymass;
	//unmandatory
	double separation_delay;
	VECTOR3 speed;
	VECTOR3 rot_speed;
	char module[MAXLEN];
	MESHHANDLE msh_h;
	int msh_idh;
	double currDelay;
	};
struct ADAPTER{
	//mandatory
	char meshname[MAXLEN];
	VECTOR3 off;
	double height;
	double diameter;
	double emptymass;
	
	MESHHANDLE msh_h;
	int msh_idh;
	
	};

struct LES{
	char meshname[MAXLEN];
	VECTOR3 off;
	double height;
	double diameter;
	double emptymass;
	VECTOR3 speed;
	VECTOR3 rot_speed;
	char module[MAXLEN];
	MESHHANDLE msh_h;
	int msh_idh;
	double volume;
};
struct ULLAGE{
	bool wUllage;
	bool ignited;
	double thrust;
	double anticipation;
	double overlap;
	int N;
	double angle;
	VECTOR3 pos;
	VECTOR3 dir;
	double length;
	double diameter;
	char tex[MAXLEN];
	THRUSTER_HANDLE th_ullage;
	double rectfactor;
};
struct STAGE{
	//mandatory
	char meshname[MAXLEN];
	VECTOR3 off;
	double height;
	double diameter;
	double thrust;
	double emptymass;
	double fuelmass;
	double burntime;	
	//unmandatory
	BATTS batteries;
	double ignite_delay;
	VECTOR3 speed;
	VECTOR3 rot_speed;
	char module[MAXLEN];
	double pitchthrust;
	double yawthrust;
	double rollthrust;
	VECTOR3 eng[32];
	double engine_phase[32];
	double engine_amp[32];
	double freq[32];
	VECTOR4 engV4[32];
	double eng_diameter;
	char eng_tex[MAXLEN];
	char eng_pstream1[MAXLEN];
	char eng_pstream2[MAXLEN];
	VECTOR3 eng_dir;
	ULLAGE ullage;
	//derived parameters
	double isp;
	double volume;
	PROPELLANT_HANDLE tank;
	THRUSTER_HANDLE th_main_h[32],th_att_h[2];
	MESHHANDLE msh_h;
	int msh_idh;
	int nEngines;
	bool wInter;
	INTERSTAGE interstage;
	bool Ignited;
	double currDelay;
	double IgnitionTime;
	bool reignitable;
	bool wBoiloff;
};
struct PAYLOAD{
	//mandatory
	char meshname[MAXLEN];
	char meshname0[MAXLEN];
	char meshname1[MAXLEN];
	char meshname2[MAXLEN];
	char meshname3[MAXLEN];
	char meshname4[MAXLEN];
	
	VECTOR3 off[5];
	double height;
	double diameter;
	double mass;
	char module[MAXLEN];
	char name[MAXLEN];
	//unmandatory
	
	VECTOR3 speed;
	VECTOR3 rot_speed;
	
	double volume;
	int render;
	int nMeshes;
	MESHHANDLE msh_h[5];
	int msh_idh[5];
	VECTOR3 Rotation;
	bool rotated;
	bool live;
	};
struct FAIRING{
	//mandatory
	int N;
	char meshname[MAXLEN];
	VECTOR3 off;
	double angle;
	double height;
	double diameter;
	double emptymass;
	//unmandatory
	double volume;
	VECTOR3 speed;
	VECTOR3 rot_speed;
	char module[MAXLEN];
	MESHHANDLE msh_h[10];
	int msh_idh[10];
	};
enum GNC_Comand {CM_ROLL,CM_PITCH,CM_ENGINE,CM_FAIRING,CM_LES,CM_DISABLE_PITCH,CM_DISABLE_ROLL,CM_DISABLE_JETTISON,CM_PLAY,CM_JETTISON,CM_TARGET,CM_AOA,CM_ATTITUDE,CM_SPIN,CM_INVERSE,CM_ORBIT,CM_ENGINEOUT,CM_NOLINE};
struct GNC_STEP{
	double time;
	char Comand[128];
	GNC_Comand GNC_Comand;
	double val_init;
	double val_fin;
	double time_init;
	double time_fin;
	double duration;
	double trval1,trval2,trval3,trval4,trval5,trval6;
	bool executed;
	char trchar[256];
	bool wValue[6];
};
struct SSOUND{
	char MAIN_THRUST[MAXLEN];
	bool Main;
	char HOVER_THRUST[MAXLEN];
	bool Hover;
	char RCS_THRUST_ATTACK[MAXLEN];
	bool RCS_ta;
	char RCS_THRUST_SUSTAIN[MAXLEN];
	bool RCS_ts;
	char AIR_CONDITIONNING[MAXLEN];
	bool Ac;
	char COCKPIT_AMBIENCE_1[MAXLEN];
	bool Ca1;
	char COCKPIT_AMBIENCE_2[MAXLEN];
	bool Ca2;
	char COCKPIT_AMBIENCE_3[MAXLEN];
	bool Ca3;
	char COCKPIT_AMBIENCE_4[MAXLEN];
	bool Ca4;
	char COCKPIT_AMBIENCE_5[MAXLEN];
	bool Ca5;
	char COCKPIT_AMBIENCE_6[MAXLEN];
	bool Ca6;
	char COCKPIT_AMBIENCE_7[MAXLEN];
	bool Ca7;
	char COCKPIT_AMBIENCE_8[MAXLEN];
	bool Ca8;
	char COCKPIT_AMBIENCE_9[MAXLEN];
	bool Ca9;
	int GncStepSound[57];
};
struct FX_MACH{
	double mach_min;
	double mach_max;
	char pstream[MAXLEN];
	bool added;
	VECTOR3 off;
	VECTOR3 dir;
	PSTREAM_HANDLE ps_h;
};
struct FX_VENT{
	double time_fin[10];
	char pstream[MAXLEN];
	VECTOR3 off[10];
	VECTOR3 dir[10];
	PSTREAM_HANDLE ps_h[10];
	bool added[10];
};

struct Fuel_Levels{
	double Stage_Fuel_Lvls[10];
	double Boosters_Fuel_Lvls[10];
};
class Multistage2015:public VESSEL3{
public:
 Multistage2015(OBJHANDLE hObj,int fmodel);
 ~Multistage2015();
 void clbkSetClassCaps(FILEHANDLE cfg);
 int clbkConsumeBufferedKey(DWORD key,bool down,char *kstate);
 void clbkLoadStateEx(FILEHANDLE scn,void *vs);
 void clbkSaveState(FILEHANDLE scn);
 void clbkPostStep(double simt, double simdt, double mjd);
 void clbkPreStep(double simt, double simdt, double mjd);
 void clbkPostCreation();
 bool clbkDrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp);
 void clbkVisualCreated (VISHANDLE _vis, int refcount);
 void clbkVisualDestroyed (VISHANDLE _vis, int refcount);
 int MyID;
 VISHANDLE vis;
 THGROUP_HANDLE thg_h_main;
 
 //Making Public Some Vars
// virtual double GetTgtApo();
// virtual Fuel_Levels GetFuelLevels();
 //double testing;


// UINT rotateplx[5];
 //UINT rotateply[5];
 //UINT rotateplz[5];
//private:
// I think it's better to have most of these as private
 VECTOR4 _V4(double x, double y, double z, double t);
 VECTOR2 _V2(double x, double y);
 bool IsOdd(int integer);
 GNC_STEP Gnc_step[150]; //MAx 150 Steps
 void AutoJettison();
 void initGlobalVars();
 void VehicleSetup();
 void RotatePayload(int pns, int nm, VECTOR3 anglesrad);
 void LoadMeshes();
 void UpdateMass();
 void UpdatePMI();
 void UpdateOffsets();
 void CreateUllage();
 char dataparsed[MAXLEN];
 bool parseinifile(char filename[MAXLEN]);
 void parseStages(char filename[MAXLEN]);
 void parseBoosters(char filename[MAXLEN]);
 void parseInterstages(char filename[MAXLEN],int parsingstage);
 void parseAdapter(char filename[MAXLEN]);
 void parseLes(char filename[MAXLEN]);
 void parseFairing(char filename[MAXLEN]);
 void parsePayload(char filename[MAXLEN]);
 void parseMisc(char filename[MAXLEN]);
 void parseTexture(char filename[MAXLEN]);
 void parseParticle(char filename[MAXLEN]);
 void parseFXMach(char filename[MAXLEN]);
 void parseFXVent(char filename[MAXLEN]);
 void parseSound(char filename[MAXLEN]);
 VECTOR3 GetBoosterPos(int nBooster, int N);
 void ArrangePayloadMeshes(char data[MAXLEN], int pnl);
 char* GetProperPayloadMeshName(int pnl, int n);
 void ArrangePayloadOffsets(char data[MAXLEN], int pnl);
 void Jettison(int type, int current);
 void Spawn(int type, int current);
 VECTOR3 CharToVec(char charvar[MAXLEN],VECTOR3* outvec);
 VECTOR4 CharToVec4(char charvar[MAXLEN],VECTOR4* outvec);
 int Configuration;
 double CogElev;
 double VerticalAngle;
 char OrbiterRoot[MAXLEN];
 char fileini[MAXLEN];
 char guidancefile[MAXLEN];
 int nStages;
 int nBoosters;
 int nInterstages;
 int nPayloads;
 int nTextures;
 int nParticles;
 int currentStage;
 int currentBooster;
 int currentInterstage;
 int currentPayload;
 bool hasFairing;
 bool wLes;
 bool wMach;
 bool wVent;
 int wFairing;
 STAGE stage[10];
 BOOSTER booster[10];
 PAYLOAD payload[10];
 FAIRING fairing;
 MISC Misc;
 TEX tex;
 PARTICLE Particle;
 ADAPTER Adapter;
 LES Les;
 SSOUND Ssound;
 FX_MACH FX_Mach;
 FX_VENT FX_Vent;
 void CreateRCS();
 void CreateMainThruster();
 bool wBoosters;
 bool wAdapter;
 char logbuff[MAXLEN];
 char buffreset[MAXLEN];
 double MET;
 SURFHANDLE GetProperExhaustTexture(char name[MAXLEN]);
 void FLY(double simtime, double simdtime, double mjdate);
 VECTOR3 RotateVecZ(VECTOR3 input, double Angle);
 PARTICLESTREAMSPEC GetProperPS(char name[MAXLEN]);
 void InitializeDelays();
 double stage_ignition_time;
//bool wVinkasGuidance;
 virtual VECTOR3 hms(double time);
 virtual double RemBurnTime(int rbtstage, double level);
 virtual double BoosterRemBurnTime(int rbtbooster, double level);
 double StageDv(int dvstage);
 double DvAtStage(int dvatstage);
 double StageRemDv(int dvstage);
 double CurrentStageRemDv();
 VECTOR3 RotateVector(const VECTOR3& input, double angle, const VECTOR3& rotationaxis);
 ATTACHMENTHANDLE live_a[10];
 void UpdateLivePayloads();
 MATRIX3 RotationMatrix(VECTOR3 angles);
 void CheckForAdditionalThrust(int pns);
 void CheckForFX(int fxtype, double param);
 int nVent;
 double lvl;
 UINT exhaustN[10][32];
 virtual void WriteGNCFile();
 //GUIDANCE
 void parseGuidanceFile(char filename[MAXLEN]);
 int nsteps;
 void VinkaComposeGNCSteps();
 GNC_STEP VinkaComposeSpecificGNCSteps(GNC_STEP gnc);
 virtual void VinkaDeleteStep(int q);
 virtual void VinkaRearrangeSteps();
 virtual void VinkaAddStep(char input[MAXLEN]);
 int VinkaCountSteps();
 double perror;
 double yerror;
 double rerror;
 void Attitude(double pitch,double roll, double heading,double pitchrate, double rollrate, double yawrate);
 double VinkaAzimuth;
 double VinkaMode;
 virtual int VinkaGetStep(double met);
 void VinkaPitch(int step);
 void VinkaRoll(int step);
 void VinkaEngine(int step);
 void VinkaConsumeStep(int step);
 void VinkaAutoPilot();
 void VinkaCheckInitialMet();
 virtual double GetProperHeading();
 virtual double GetHeading();
 double GetProperRoll(double RequestedRoll);
 double initlong,initlat;
 int VinkaFindFirstPitch();
 int VinkaFindRoll();
 void VinkaUpdateRollTime();
 bool APstat;
 double finalv(double Abside, double Apo,double Peri);
 bool AJdisabled;
 bool rolldisabled;
 bool pitchdisabled;
 void killAP();
 double VinkaFindEndTime();
 int Gnc_running;
 void Guidance_Debug();
  string value1,value2,value3,value4,value5,value6;
  bool spinning;
  bool AttCtrl;
  bool PitchCtrl;
  bool YawCtrl;
  bool RollCtrl;
  virtual void ToggleAttCtrl(bool Pitch, bool Yaw, bool Roll);
  virtual void ToggleAP();
 double DeltaUpdate;
 bool wPeg;
 double TgtPitch;
 double GetVPerp();
 double altsteps[4];
 bool Complex;
 void EvaluateComplexFlight();
 void ComplexFlight();
 double UpdateComplex;
 void CalculateAltSteps(double planetmass);
  ////COMPUTATIONAL GRAVITY TURN EVALUATION
  
  double GetProperNforCGTE(double time);
  bool CGTE(double psi0);
  double GT_InitPitch;
  double GT_IP_Calculated;


  //Some More Utilities
  double GetMassAtStage(int MassStage,bool empty);
  double CalcAzimuth();
  double getabsacc();
  double getacc();
  double GetOS();
  int SignOf(double X);

  void Boiloff();
	  


	  
	  
  virtual double GetTMECO();
  
	////PEG
  bool runningPeg;
	bool CutoffCheck();
	ORBITPARAM op;
	ELEMENTS el;
	double epsfin;
	double eps;
	CUTOFF_CONDITIONS coc;
  void FEstimate();
  void FStaging();

  double r_T[10];
  double rdot_T[10];
  double T[10];
  double DeltaA[10];
  double DeltaB[10];
  double A[10];
  double B[10];
  double delta_rdot[10];
  double delta_r[10];
  double r_in[10];
  double rdot_in[10];
  double VthetaAtStaging[10];
  double OmegaS[10];
  double a_fin[10];
  

  double tgtapo;
  double tgtperi;
  double tgtinc;
  double tgtabside;
  double tgtvv;
  double Azimuth;
double mu;
double rt;
double g0;

//Navigation vars
double r;
double v;
double z;
VECTOR3 hvec;
VECTOR3 rvec;
VECTOR3 vvec;
double h;
VECTOR3 rhat;
VECTOR3 hhat;
VECTOR3 thetahat;
VECTOR3 fhat;
double thrust;
double mass;
double a_[10];
double v_e[10];
double tau_[10];
double omega;
double rdot;
double vtheta;
VECTOR3 ShipSpeed;
double VertVel;
int NN;


//feedback vars
double g;
double cent;
double g_term;
double cent_term;
int J;


void InitPEG();
void Navigate();
double b0(int j, double t_j);
double b_(int n, int j, double t_j);
double c0(int j, double t_j);
double c_(int n, int j , double t_j);
double a(int j, double t_j);

void CalculateTargets();
void MajorCycle();
double UpdatePegTimer;
double PegDesPitch;

void PEG();

//FAILURES
int failureProbability;
int timeOfFailure;
bool wFailures;
bool failed;
void Failure();
void FailuresEvaluation();
bool CheckForFailure(double met);
void boom();


//TELEMETRY
char tlmfile[MAXLEN];
VECTOR2 tlmAlt[TLMSECS];
VECTOR2 tlmSpeed[TLMSECS];
VECTOR2 tlmPitch[TLMSECS];
VECTOR2 tlmThrust[TLMSECS];
VECTOR2 tlmMass[TLMSECS];
VECTOR2 tlmVv[TLMSECS];
VECTOR2 tlmAcc[TLMSECS];
VECTOR2 ReftlmAlt[TLMSECS];
VECTOR2 ReftlmSpeed[TLMSECS];
VECTOR2 ReftlmPitch[TLMSECS];
VECTOR2 ReftlmThrust[TLMSECS];
VECTOR2 ReftlmMass[TLMSECS];
VECTOR2 ReftlmVv[TLMSECS];
VECTOR2 ReftlmAcc[TLMSECS];

double updtlm;
double writetlmTimer;
int tlmidx;
void Telemetry();
int tlmnlines;
int loadedtlmlines;
virtual int WriteTelemetryFile(int initline);
virtual void parseTelemetryFile(char filename[MAXLEN]);
double updtboiloff;
bool wReftlm;
};



