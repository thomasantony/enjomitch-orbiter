#ifndef BURNTIMEMFD_H_INCLUDED
#define BURNTIMEMFD_H_INCLUDED

#include <orbitersdk.h>

class BurnTimeMFD: public MFD {
public:
  BurnTimeMFD (DWORD w, DWORD h, VESSEL *vessel);
  ~BurnTimeMFD ();
  bool ConsumeKeyBuffered (DWORD key);
  void Update (HDC hDC);
  static int MsgProc (UINT msg, UINT mfinald, WPARAM wparam, LPARAM lparam);
  int ButtonMenu (const MFDBUTTONMENU **menu) const;
  char *ButtonLabel (int bt);
  bool ConsumeButton(int bt, int event);
  void CalcApses(VESSEL* vessel);
  void PrintEngUnit(HDC hDC, char* format, char* units, double mult, double value, int x, int l);
  void PrintEngUnit(HDC hDC, char* format, char* units, double value, int x, int l);
  void PrintEngUnit(HDC hDC, char* format, char* unitSI, char* unitUS, double multSI, double multUS, double value, int x, int l);
  void PrintString(HDC hDC, char* format, const char* value, int x, int l);
  void PrintString(HDC hDC, const char* format, int x, int l);
  void TimeStep(double ENow);
  void ArmAutoBurn();
  void CalcCircular();
  void CalcIBurn(VESSEL* vessel);
  double GetStackMass(VESSEL* vessel);

  double mu,a,e,IPeri,IApo,Rperi,Rapo,EArmed,IReference,mfuel;

  double mv,ms,me,isp,F,Eff;

  double IBurn,IBurn2,mul;
  int mode; //0 - IBurn-zero=IPeri
            //1 - IBurn-zero=IApo
            //2 - IBurn-zero=manual countdownm
			//3 - By Target distance

  int MySoundId;
  int Sel_eng;


protected:

  VESSEL *messel;
  int height;



private:
  //static int Sel_eng;
  void writemenu( HDC hDC );
  int line( int );
  double dvcurr;
  double ENow;
  double mdot;
  double mfinal,msfinal,msempty;
  double maxdv;
  double TTot;
};

#endif // BURNTIMEMFD_H_INCLUDED
