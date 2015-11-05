#include "PEGMultiFred.h"

PEGMultiFred::PEGMultiFred(const VESSEL * v)
: ves(v)
{
    //ctor
}

PEGMultiFred::~PEGMultiFred()
{
    //dtor
}

//Get Vehicle Acceleration along Velocity Vector
double PEGMultiFred::getacc(){

double acc;
VECTOR3 force_vec, acc_vec, spd_vec;
VECTOR3 weight_vec,thrust_vec,drag_vec;
// Get the vectors
ves->GetShipAirspeedVector(spd_vec);
ves->GetThrustVector(thrust_vec);
ves->GetWeightVector(weight_vec);
ves->GetDragVector(drag_vec);
force_vec.x=thrust_vec.x+weight_vec.x+drag_vec.x;
force_vec.y=thrust_vec.y+weight_vec.y+drag_vec.y;
force_vec.z=thrust_vec.z+weight_vec.z+drag_vec.z;
// Normalize the speed vector
normalise(spd_vec);
// Calculate the acceleration vector
acc_vec = force_vec / ves->GetMass();

// Take the dot product
acc = dotp(acc_vec,spd_vec);

return acc;
}

//Initialize PEG
void PEGMultiFred::InitPEG(){
oapiWriteLog("RESET PEG");
for(int i=1;i<=nStages;i++){
v_e[i]=stage[i-1].isp;
a_[i]=stage[i-1].thrust/GetMassAtStage(i-1,FALSE);
a_fin[i]=stage[i-1].thrust/GetMassAtStage(i-1,TRUE);
tau_[i]=v_e[i]/a_[i];
A[i]=0;
B[i]=0;
}
NN=nStages;
for(int i=0;i<10;i++)
{
	T[i]=0;
	OmegaS[i]=0;
	delta_rdot[i]=0;
	rdot_in[i]=0;
	delta_r[i]=0;
	r_in[i]=0;
	r_T[i]=0;
	rdot_T[i]=0;
	VthetaAtStaging[i]=0;
}


return;
}

void PEGMultiFred::CalculateTargets()
{
	if((tgtabside>tgtapo)||(tgtabside<tgtperi))
	{
		if(tgtperi>=80000)
		{
			tgtabside=tgtperi;
		}else{
			tgtabside=tgtapo;
		}
	}
	double etarget=(tgtapo+rt-tgtperi-rt)/(tgtapo+2*rt+tgtperi);
	double atarget=(tgtapo+rt)/(1+etarget);
	epsfin=-mu/(2*atarget);
	double vr_target=sqrt(mu*(2/(tgtabside+rt)-1/(0.5*(tgtapo+tgtperi+2*rt))));
	if(etarget==0)
	{tgtvv=0;}else{
		if((tgtabside==tgtperi)||(tgtabside==tgtapo)){
		tgtvv=0;}else{
	double cosTA=(atarget*(1-(etarget*etarget))-(tgtabside+rt))/((tgtabside+rt)*etarget);
	double tgtTrueAnomaly=acos(cosTA);
	tgtvv=vr_target*(etarget*sin(tgtTrueAnomaly))/(1+etarget*cos(tgtTrueAnomaly));
		}
	}
	return;
}

bool PEGMultiFred::CutoffCheck(){
	OBJHANDLE hearth=ves->GetSurfaceRef();
	ves->GetElements (hearth, el, &op, 0, 1);
	eps=-mu/(2*el.a);
	if(eps>=epsfin){
	killAP();
	runningPeg=FALSE;
	return TRUE;
	}
	return FALSE;
}

double PEGMultiFred::GetTMECO()
{double MECO=0;
for(int i=1;i<=NN;i++)
{
	MECO+=T[i];
}
MECO+=MET;
return MECO;
}
void PEGMultiFred::Navigate(){
	J=currentStage+1;
	ves->GetHorizonAirspeedVector(ShipSpeed);
	VertVel=ShipSpeed.y;
	ves->GetRelativePos(ves->GetGravityRef(),rvec);
	ves->GetRelativeVel(ves->GetGravityRef(),vvec);
	r=length(rvec);
	v=length(vvec);
	hvec=crossp(rvec,vvec);
	h=length(hvec);
	thrust=stage[currentStage].thrust;
	mass=ves->GetMass();
	a_[J]=getacc();
	omega=h/(r*r);
	if(wBoosters){a_[J]=thrust/mass;}
	v_e[J]=stage[J-1].isp;
	tau_[J]=v_e[J]/a_[J];
	NN=coc.stage+1;

	for(int i=1;i<=NN;i++)
	{
		T[i]=coc.remBT[i-1];
	}

    g=-mu/(r*r);
    cent=omega*omega*r;
    g_term=-g/a_[J];
    cent_term=-cent/a_[J];

	A[J-1]=0;
	B[J-1]=0;
	DeltaA[J-1]=0;
	DeltaB[J-1]=0;
	DeltaA[NN]=0;
	DeltaB[NN]=0;

}

double PEGMultiFred::b0(int j,double t_j){
	return -v_e[j]*log(1-t_j/tau_[j]);
}

double PEGMultiFred::b_(int n, int j , double t_j){
	if(n==0) return b0(j,t_j);
	return b_(n-1,j,t_j)*tau_[j]-v_e[j]*pow(t_j,n)/n;
}

double PEGMultiFred::c0(int j, double t_j){
	return b0(j,t_j)*t_j-b_(1,j,t_j);
}

double PEGMultiFred::c_(int n, int j, double t_j){
	if(n==0) return c0(j,t_j);
	return c_(n-1,j,t_j)*tau_[j]-v_e[j]*pow(t_j,n+1)/(n*(n+1));
}


double PEGMultiFred::a(int j, double t_j){
return a_[j]/(1-t_j/tau_[j]);
}

void PEGMultiFred::FEstimate()
{

	for(int i=0;i<10;i++)
	{
		coc.remBT[i]=0;
	}
	double T_MECO=0;
	double RemDvToDo=finalv(tgtabside,tgtapo,tgtperi)-GetOS();
	double RemDv=RemDvToDo;
	int q=0;

	while((RemDv>StageRemDv(currentStage+q))&&(currentStage+q<nStages-1))
	{
		RemDv-=StageRemDv(currentStage+q);
		coc.remBT[currentStage+q]=RemBurnTime(currentStage+q,1);
		T_MECO+=RemBurnTime(currentStage+q,1);
		q++;
	}

	T_MECO+=RemDv*ves->GetPropellantMass(stage[currentStage+q].tank)/(stage[currentStage+q].thrust*log((GetMassAtStage(currentStage+q,TRUE)+ves->GetPropellantMass(stage[currentStage+q].tank))/GetMassAtStage(currentStage+q,TRUE)));
	coc.remBT[currentStage+q]=RemDv*ves->GetPropellantMass(stage[currentStage+q].tank)/(stage[currentStage+q].thrust*log((GetMassAtStage(currentStage+q,TRUE)+ves->GetPropellantMass(stage[currentStage+q].tank))/GetMassAtStage(currentStage+q,TRUE)));

	coc.T=T_MECO;
	coc.stage=currentStage+q;
	coc.rem_stages=q;

	return;
}

void PEGMultiFred::FStaging()
{
r_T[J]=r+VertVel*T[J]+c0(1,T[J])*A[J]+c_(1,1,T[J])*B[J];
rdot_T[J]=VertVel+b0(1,T[J])*A[J]+b_(1,1,T[J])*B[J];

r_in[J]=r;
rdot_in[J]=VertVel;

for(int i=J+1;i<NN;i++)
{
	delta_rdot[i]=b0(i,T[i])*A[i]+b_(1,i,T[i])*B[i];
	delta_r[i]=rdot_in[i]*T[i]+c0(i,T[i])*A[i]+c_(1,i,T[i])*B[i];
}

for(int i=J+1;i<NN;i++)
{
	rdot_in[i]=rdot_in[J];
	r_in[i]=r_in[J];
	for(int j=J+1;j<=i-1;j++)
	{
		rdot_in[i]+=delta_rdot[j];
		r_in[i]+=delta_r[j];
	}
}

for(int j=J+1;j<NN;j++)
{
	rdot_T[j]=rdot_in[j];
	r_T[j]=r_in[j];
}

	r_T[NN]=tgtabside+rt;//tgtapo+rt;
	rdot_T[NN]=tgtvv;//0;

	VthetaAtStaging[J]=GetOS()+StageRemDv(J-1);
	for(int i=J+1;i<=NN;i++)
	{
		VthetaAtStaging[i]=VthetaAtStaging[i-1]+StageRemDv(i-1);
	}
	for(int i=J;i<=NN;i++)
	{
		OmegaS[i]=VthetaAtStaging[i]/r_T[i];//FredEstimateStaging().r_T[i];
	}

return;
}

//void PEGMultiFred::PEG(){
void PEGMultiFred::MajorCycle(){
	CalculateTargets();
	Navigate();
	FStaging();
	FEstimate();

	for(int i=currentStage+1;i<NN;i++)
	{
		DeltaA[i]=(mu/(r_T[i]*r_T[i])-OmegaS[i]*OmegaS[i]*r_T[i])*((1/(a_fin[i])-(1/(a_[i+1]))));
		DeltaB[i]=(mu/(r_T[i]*r_T[i])-OmegaS[i]*OmegaS[i]*r_T[i])*((1/v_e[i])-(1/v_e[i+1]))+((3*OmegaS[i]*OmegaS[i]-2*mu/(r_T[i]*r_T[i]*r_T[i]))*rdot_T[i])*((1/(a_fin[i])-(1/(a_[i+1]))));
	}



	double alpha=0;
	for(int l=J;l<=NN;l++)
	{
		alpha+=b0(l,T[l]);
	}

	double beta=0;
	for(int l=J;l<=NN;l++)
	{
		double term=0;
		for(int k=J;k<=l-1;k++)
		{
			term+=T[k];
		}
		beta+=b_(1,l,T[l])+b0(l,T[l])*term;
	}

	double gamma=0;
	for(int l=J;l<=NN;l++)
	{
		double term1=0;
		for(int k=J;k<=l-1;k++)
		{
			double term2=0;
			for(int i=J;i<=k-1;i++)
			{
				term2+=DeltaB[i];
			}
			term1+=b0(l,T[l])*DeltaA[k]+b0(l,T[l])*T[k]*term2+b_(1,l,T[l])*DeltaB[k];
		}
		gamma+=term1;
	}

	gamma=gamma*(-1);
	gamma+=tgtvv-VertVel;

	double epsilon=0;
	for(int l=J;l<=NN;l++)
	{
		double term=0;
		for(int k=J;k<=l-1;k++)
		{
			term+=b0(k,T[k]);
		}
		epsilon+=c0(l,T[l])+T[l]*term;
	}


	double zeta=0;

	for(int l=J;l<=NN;l++)
	{
		double term2=0;
		for(int k=J;k<=l-1;k++)
		{
			double term1=0;
			for(int i=J;i<=k-1;i++)
			{
				term1+=T[i];
			}
			term2+=c0(l,T[l])*T[k]+b_(1,k,T[k])*T[l]+b0(k,T[k])*T[l]*term1;
		}
		zeta+=c_(1,l,T[l])+term2;
	}

	double iota=0;
	for(int l=J;l<=NN;l++)
	{double term3=0;
		for(int k=J;k<=l-1;k++)
		{double term2=0;
			for(int i=J;i<=k-1;i++)
			{double term1=0;
				for(int m=J;m<=i-1;m++)
				{
					term1+=DeltaB[m];
				}
				term2+=b0(k,T[k])*T[l]*DeltaA[i]+b0(k,T[k])*T[i]*T[l]*term1+b_(1,k,T[k])*T[l]*DeltaB[i]+c0(l,T[l])*T[k]*DeltaB[i];
			}
			term3+=c0(l,T[l])*DeltaA[k]+c_(1,l,T[l])*DeltaB[k]+term2;
		}
		iota+=term3;
	}

	double k23=0;
	for(int l=J;l<=NN;l++)
	{
		k23+=T[l];
	}

	k23=k23*VertVel;
	double lambda=(tgtabside+rt)-k23-(ves->GetAltitude()+rt)-iota;

	A[J]=(gamma*zeta-beta*lambda)/(alpha*zeta-beta*epsilon);
	B[J]=(alpha*lambda-gamma*epsilon)/(alpha*zeta-beta*epsilon);




	double transvarA=0;
	for(int l=J;l<=NN;l++)
	{
		double term1=0;
		for(int k=J;k<l-1;k++)
		{
			term1+=DeltaB[k];
		}
		transvarA+=DeltaA[l]+B[1]*T[l]+T[l]*term1;
	}


	double transvarB=0;
	for(int l=J;l<=NN;l++)
	{
		transvarB+=DeltaB[l];
	}



	for(int l=J+1;l<=NN;l++)
	{
	A[l]=A[l-1]+transvarA;
	B[l]=B[l-1]+transvarB;
	}


	}

	void PEGMultiFred::PEG(){

	UpdatePegTimer+=oapiGetSimStep();

	if(UpdatePegTimer>=0.1)
	{
		UpdatePegTimer=0;
		MajorCycle();
	}



	double arg=0;

	if((A[J]<10)&&(A[J]>-10))
	{
		//arg=A[J]+B[J]*oapiGetSimStep()+g_term+cent_term;
		arg=A[J]+B[J]*UpdatePegTimer+g_term+cent_term;
	}else{
		InitPEG();
	}

	double pitchlimit;
	if(J==1){pitchlimit=35*RAD;}else{pitchlimit=35*RAD;} //For future refinery of peg program start

	if(abs(arg)<sin(pitchlimit))
	{
		PegDesPitch=asin(arg);
	}else{
		PegDesPitch=pitchlimit*(abs(arg)/arg);
	}
	if(abs(arg)<1)
	{
	TgtPitch=asin(arg);
	}else{
	TgtPitch=90*RAD*SignOf(arg);
	};

		// ACCOUNT FOR THRUST NOT IN INLINE WITH VEHICLE
	VECTOR3 thrustVector;
	ves->GetThrustVector(thrustVector);
	if(length(thrustVector)>0)
	{
		normalise(thrustVector);
		PegDesPitch-=asin(thrustVector.y);
		TgtPitch-=asin(thrustVector.y);
	}



	//Attitude(PegDesPitch,(0.5*(1-VinkaMode)*PI),GetProperHeading(),8,5,8);
	//sprintf(oapiDebugString(),"DesPitch: %.1f arg: %.3f T1:%.1f T2 %.1f T3:%.1f NN:%i r_T[1]:%.3f rdot_T[1]:%.3f",PegDesPitch*DEG,arg,T[1],T[2],T[3],NN,(r_T[currentStage+1]-rt)/1000,rdot_T[currentStage+1]);
	//oapiWriteLog(oapiDebugString());
}

/// GUIDANCE - REMOVE

void PEGMultiFred::killAP(){
	ves->SetThrusterGroupLevel(THGROUP_MAIN,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_PITCHUP,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_PITCHDOWN,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_YAWLEFT,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_YAWRIGHT,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_BANKRIGHT,0);
	ves->SetThrusterGroupLevel(THGROUP_ATT_BANKLEFT,0);
	//ves->ActivateNavmode(NAVMODE_KILLROT);
	return;
}

 //Returns Any Stage Rmeaining DV
 double PEGMultiFred::StageRemDv(int dvstage)
 {
	 double srd;
	 srd=stage[dvstage].isp*log((GetMassAtStage(dvstage,TRUE)+ves->GetPropellantMass(stage[dvstage].tank))/GetMassAtStage(dvstage,TRUE));
	 return srd;
 }

 //Returns Remaining Stage Burn Time
 double PEGMultiFred::RemBurnTime(int rbtstage, double level=1){
	 double BT;
	 	 BT=stage[rbtstage].isp*ves->GetPropellantMass(stage[rbtstage].tank)/(stage[rbtstage].thrust*level);
	 return BT;
 }

 //returns Vechicle Mass at a certain stage
 double PEGMultiFred::GetMassAtStage(int MassStage,bool empty=TRUE){
	 double Mass=0;

	 int add=0;
		 if(empty==TRUE){add=1;}else{add=0;}

	 for(int i=MassStage;i<nStages;i++){
		 Mass+=stage[i].emptymass;
	 }
	 for(int j=MassStage+add;j<nStages;j++){
	 Mass+=stage[j].fuelmass;
	 }
	 for(int k=0;k<nPayloads;k++){
		 Mass+=payload[k].mass;
	 }
	 if(hasFairing){
	 Mass+=fairing.emptymass;
	 }

	 return Mass;
 }

  //returns Vehicle Orbital Speed
 double PEGMultiFred::GetOS(){
	OBJHANDLE hearth=ves->GetSurfaceRef();
	VECTOR3 relvel;
	ves->GetRelativeVel(hearth,relvel);
	double os=length(relvel);
	return os;
}

double PEGMultiFred::finalv(double Abside, double Apo,double Peri){
	return sqrt(mu*(2/(Abside+rt)-1/(0.5*(Apo+Peri+2*rt))));
}

//returns 1 if X is positive or -1 if X is negative
int PEGMultiFred::SignOf(double X)
{
	return X/abs(X);
}
