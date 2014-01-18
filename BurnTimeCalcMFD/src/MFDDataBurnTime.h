#ifndef MFDDATABURNTIME_H
#define MFDDATABURNTIME_H

#include <multipleVesselsMFD/MFDData.hpp>
#include <Orbitersdk.h>

class MFDDataBurnTime : public EnjoLib::MFDGoodies::MFDData
{
    public:
        MFDDataBurnTime(VESSEL * vessel);
        virtual ~MFDDataBurnTime();
        void Update();
        void ArmAutoBurn();
        void CalcCircular();
        void CalcApses(VESSEL* vessel);
        void CalcIBurn(VESSEL* vessel);
        double GetStackMass(VESSEL* vessel);

        static const int numEngines;
        static const THGROUP_TYPE groups[6];
        static const char group_names[6][7];

        double mu,a,e,IPeri,IApo,Rperi,Rapo,EArmed,IReference,mfuel;

      double mv,ms,me,isp,F,Eff;

      double IBurn,IBurn2,mul;
      int mode; //0 - IBurn-zero=IPeri
                //1 - IBurn-zero=IApo
                //2 - IBurn-zero=manual countdownm
                //3 - By Target distance

      int MySoundId;
      int Sel_eng;
      double dv, mextra;
      bool IsEngaged,IsArmed,IsCircular;
        double ECutoff,IManual,EReference;
        double TDist;
        double sOffset;



  int dspunit; //0 - SI
            //1 - US

            double dvcurr, dvOld;
      double ENow;
      double mdot;
      double mfinal,msfinal,msempty;
      double maxdv;
      double TTot;

      int inputmode;
        int IndexCenterObj;

    protected:
    private:


};

#endif // MFDDATABURNTIME_H