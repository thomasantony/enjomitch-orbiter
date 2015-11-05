#ifndef PEGMULTIFRED_H
#define PEGMULTIFRED_H

#include <orbitersdk.h>
#include "../Multistage2015/Multistage2015.h"

class PEGMultiFred
{
    public:
        PEGMultiFred( const VESSEL * v );
        virtual ~PEGMultiFred();
    protected:
    private:

        const VESSEL * ves;


        //Some More Utilities
  double GetMassAtStage(int MassStage,bool empty);
  double CalcAzimuth();
  double getabsacc();
  double getacc();
  double GetOS();
  int SignOf(double X);

  void Boiloff();


    /// GUIDANCE - REMOVE
    void killAP();



  virtual double GetTMECO();

    /// NEW to PEG
  int nStages;
   STAGE stage[10];
   double MET;
   int currentStage;
   bool wBoosters;
   double finalv(double Abside, double Apo,double Peri);
   double StageRemDv(int dvstage);
   virtual double RemBurnTime(int rbtstage, double level);
   double TgtPitch;
   double VinkaMode;
   int nPayloads;
   PAYLOAD payload[10];
   bool hasFairing;
   FAIRING fairing;
   //void Attitude(double pitch,double roll, double heading,double pitchrate, double rollrate, double yawrate);
   //virtual double GetProperHeading();

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
};

#endif // PEGMULTIFRED_H
