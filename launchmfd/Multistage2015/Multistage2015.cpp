// ==============================================================
//						MultiStage2015
//
//					       By Fred18
//
//
// Multistage2015.cpp
// ==============================================================





//############################################################################//
//#define ORBITER_MODULE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <math.h>
#include <stdio.h>
#include "orbitersdk.h"


#include "Multistage2015.h"

//Creation
Multistage2015::Multistage2015(OBJHANDLE hObj,int fmodel):VESSEL3(hObj,fmodel){}
Multistage2015::~Multistage2015(){}

VECTOR4 Multistage2015::_V4(double x, double y, double z, double t){
VECTOR4 v4;
v4.x=x;
v4.y=y;
v4.z=z;
v4.t=t;
return v4;
}

VECTOR2 Multistage2015::_V2(double x, double y)
{
	VECTOR2 v2;
	v2.x=x;
	v2.y=y;
	return v2;
}
//returns 1 if X is positive or -1 if X is negative
int Multistage2015::SignOf(double X)
{
	return X/abs(X);
}

//returns true if a number is odd

bool Multistage2015::IsOdd( int integer )
{
if ( integer % 2== 0 )
     return true;
  else
     return false;
}
//transforms char variable in v3
VECTOR3 Multistage2015::CharToVec(char charvar[MAXLEN],VECTOR3* outvec){

	double dbuff[3];
	char *cbuff;
	cbuff=NULL;
	cbuff=strtok(charvar,",");
	int k=0;
	 while (cbuff != NULL)
		{
			int cbs;
			for(cbs=0;cbs<sizeof(cbuff);cbs++){
				if(cbuff[cbs]=='(') cbuff[cbs]=' ';
				else if(cbuff[cbs]==')') cbuff[cbs]=' ';
							}


		dbuff[k]=atof(cbuff);

		switch(k){
			case 0:
			outvec->x=dbuff[k];
			break;
			case 1:
			outvec->y=dbuff[k];
			break;
			case 2:
			outvec->z=dbuff[k];
			break;
		}
		k+=1;
		if(k>2)k=0;

		cbuff = strtok (NULL, ",");

		}
	return *outvec;
}

//transforms a char variable in V4
VECTOR4 Multistage2015::CharToVec4(char charvar[MAXLEN],VECTOR4* outvec){

	double dbuff[4];
	char *cbuff;
	cbuff=NULL;
	cbuff=strtok(charvar,",");
	int k=0;
	 while (cbuff != NULL)
		{
			int cbs;
			for(cbs=0;cbs<sizeof(cbuff);cbs++){
				if(cbuff[cbs]=='(') cbuff[cbs]=' ';
				else if(cbuff[cbs]==')') cbuff[cbs]=' ';
							}


		dbuff[k]=atof(cbuff);

		switch(k){
			case 0:
			outvec->x=dbuff[k];
			break;
			case 1:
			outvec->y=dbuff[k];
			break;
			case 2:
			outvec->z=dbuff[k];
			break;
			case 3:
			outvec->t=dbuff[k];
			break;
		}
		k+=1;
		if(k>3)k=0;

		cbuff = strtok (NULL, ",");

		}
	return *outvec;
}
//Function to Rotate a Vector3 around Z axis of a given Angle
 VECTOR3 Multistage2015::RotateVecZ(VECTOR3 input, double Angle){
	 VECTOR3 output;
	  	output=_V(input.x*cos(Angle)-input.y*sin(Angle),input.x*sin(Angle)+input.y*cos(Angle),input.z);
	 return output;
 }

 MATRIX3 Multistage2015::RotationMatrix(VECTOR3 angles)
 {MATRIX3 m;
 	MATRIX3 RM_X,RM_Y,RM_Z;
	RM_X=_M(1,0,0,0,cos(angles.x),-sin(angles.x),0,sin(angles.x),cos(angles.x));
	RM_Y=_M(cos(angles.y),0,sin(angles.y),0,1,0,-sin(angles.y),0,cos(angles.y));
	RM_Z=_M(cos(angles.z),-sin(angles.z),0,sin(angles.z),cos(angles.z),0,0,0,1);
	m=mul(RM_Z,mul(RM_Y,RM_X));
 return m;
 }

 //Returns current Heading
 double Multistage2015::GetHeading()
 {double Heading;
 oapiGetHeading(GetHandle(),&Heading);
 return Heading;
 }
 //returns Vehicle Orbital Speed
 double Multistage2015::GetOS(){
	OBJHANDLE hearth=GetSurfaceRef();
	VECTOR3 relvel;
	GetRelativeVel(hearth,relvel);
	double os=length(relvel);
	return os;
}
 //returns Vechicle Mass at a certain stage
 double Multistage2015::GetMassAtStage(int MassStage,bool empty=TRUE){
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

 //Returns Remainign Stage DV
 double Multistage2015::StageDv(int dvstage){
	 double sdv;
	  sdv=stage[dvstage].isp*log(GetMassAtStage(dvstage,FALSE)/GetMassAtStage(dvstage,TRUE));
	 return sdv;
 }

  //Returns Remaining DV at a certain Stage
 double Multistage2015::DvAtStage(int dvatstage){
	 double rdvas=0;
	 for(int i=dvatstage;i<nStages;i++){
		 rdvas+=StageDv(i);
	 }
	 return rdvas;
 }

 //Returns Current Stage Remaining DV
 double Multistage2015::CurrentStageRemDv(){
	 double csrd;
	 csrd=stage[currentStage].isp*log((GetMassAtStage(currentStage,TRUE)+GetPropellantMass(stage[currentStage].tank))/GetMassAtStage(currentStage,TRUE));
	 return csrd;
 }

 //Returns Any Stage Rmeaining DV
 double Multistage2015::StageRemDv(int dvstage)
 {
	 double srd;
	 srd=stage[dvstage].isp*log((GetMassAtStage(dvstage,TRUE)+GetPropellantMass(stage[dvstage].tank))/GetMassAtStage(dvstage,TRUE));
	 return srd;
 }

 //Returns Remaining Stage Burn Time
 double Multistage2015::RemBurnTime(int rbtstage, double level=1){
	 double BT;
	 	 BT=stage[rbtstage].isp*GetPropellantMass(stage[rbtstage].tank)/(stage[rbtstage].thrust*level);
	 return BT;
 }
 //Returns Remaining Boosters Group Burn Time
 double Multistage2015::BoosterRemBurnTime(int rbtbooster, double level=1){
	 double BT;
	 	 BT=booster[rbtbooster].isp*GetPropellantMass(booster[rbtbooster].tank)/((booster[rbtbooster].thrust*booster[rbtbooster].N)*level);
	 return BT;
 }


 //Global Variables initialization
void Multistage2015::initGlobalVars(){
	nStages=0;
	nBoosters=0;
	nInterstages=0;
	nTextures=0;
	nParticles=0;
	currentStage=0;
	currentBooster=0;
	currentInterstage=0;
	CogElev=0;
	VerticalAngle=0;
	GetCurrentDirectory(MAXLEN,OrbiterRoot); //initialize orbiter root with the current directory
	wBoosters=FALSE;
	wFairing=0;
	wLes=FALSE;
	wAdapter=FALSE;
	wMach=FALSE;
	wVent=FALSE;
	Complex=FALSE;
	stage_ignition_time=0;
	perror=0;
	yerror=0;
	rerror=0;
	int i;
	for(i=0;i<10;i++){
	stage[i].Ignited=FALSE;
	stage[i].reignitable=TRUE;
	stage[i].batteries.wBatts=FALSE;
	}

	int ii;
	for(ii=0;ii<10;ii++){
	booster[ii].Ignited=FALSE;
	}
	MET=0;
	APstat=FALSE;
	AJdisabled=FALSE;
	rolldisabled=FALSE;
	pitchdisabled=FALSE;

	Gnc_running=0;
	spinning=FALSE;
	lvl=1;
	for(int s=0;s<57;s++){
		Ssound.GncStepSound[s]=-5;
	}

PegDesPitch=35*RAD;
DeltaUpdate=0;
GT_InitPitch=89.5*RAD;
UpdatePegTimer=0;
UpdateComplex=0;
wPeg=FALSE;
runningPeg=FALSE;
//g0=9.80655;
AttCtrl=TRUE;
 PitchCtrl=TRUE;
 YawCtrl=TRUE;
 RollCtrl=TRUE;
TgtPitch=0;
//tgtapo=200000;
//tgtperi=200000;
eps=-9000000000000;

failureProbability=-1000;
timeOfFailure=-10000000;
wFailures=FALSE;
failed=FALSE;

for(int q=0;q<150;q++)
{
	Gnc_step[q].GNC_Comand=CM_NOLINE;
}


updtlm=0;
tlmidx=0;
writetlmTimer=0;
tlmnlines=0;
wReftlm=FALSE;
for(int q=0;q<TLMSECS;q++)
{
	tlmAlt[q].x=0;
	tlmSpeed[q].x=0;
	tlmPitch[q].x=0;
	tlmThrust[q].x=0;
	tlmMass[q].x=0;
	tlmVv[q].x=0;
	tlmAcc[q].x=0;

	tlmAlt[q].y=0;
	tlmSpeed[q].y=0;
	tlmPitch[q].y=0;
	tlmThrust[q].y=0;
	tlmMass[q].y=0;
	tlmVv[q].y=0;
	tlmAcc[q].y=0;
}

updtboiloff=0;
return;
}

//Create RCS
void Multistage2015::CreateRCS(){
	if(stage[currentStage].pitchthrust==0){
	 stage[currentStage].pitchthrust=0.25*stage[currentStage].thrust*stage[currentStage].height;//Empirical Values
 }
 if(stage[currentStage].yawthrust==0){
	 stage[currentStage].yawthrust=0.25*stage[currentStage].thrust*stage[currentStage].height;//Empirical Values
 }
 if(stage[currentStage].rollthrust==0){
	 stage[currentStage].rollthrust=0.1*stage[currentStage].thrust*stage[currentStage].diameter*0.5*0.5; //Empirical Values
 }
	//pitch up
 stage[currentStage].th_att_h[0]=CreateThruster(_V(0,0,1),_V(0,1,0),stage[currentStage].pitchthrust,stage[currentStage].tank,stage[currentStage].isp*100);
 stage[currentStage].th_att_h[1]=CreateThruster(_V(0,0,-1),_V(0,-1,0),stage[currentStage].pitchthrust,stage[currentStage].tank,stage[currentStage].isp*100);
 CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_PITCHUP);

	//pitch down
	stage[currentStage].th_att_h[0]=CreateThruster(_V(0,0,1),_V(0,-1,0),stage[currentStage].pitchthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	stage[currentStage].th_att_h[1]=CreateThruster(_V(0,0,-1),_V(0,1,0),stage[currentStage].pitchthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_PITCHDOWN);

	//yaw left
	stage[currentStage].th_att_h[0]=CreateThruster(_V(0,0,1),_V(-1,0,0),stage[currentStage].yawthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	stage[currentStage].th_att_h[1]=CreateThruster(_V(0,0,-1),_V(1,0,0),stage[currentStage].yawthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_YAWLEFT);

	//yaw right
	stage[currentStage].th_att_h[0]=CreateThruster(_V(0,0,1),_V(1,0,0),stage[currentStage].yawthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	stage[currentStage].th_att_h[1]=CreateThruster(_V(0,0,-1),_V(-1,0,0),stage[currentStage].yawthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_YAWRIGHT);

	//roll left
	stage[currentStage].th_att_h[0]=CreateThruster(_V(1,0,0),_V(0,1,0),stage[currentStage].rollthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	stage[currentStage].th_att_h[1]=CreateThruster(_V(-1,0,0),_V(0,-1,0),stage[currentStage].rollthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_BANKLEFT);

	//roll right
	stage[currentStage].th_att_h[0]=CreateThruster(_V(1,0,0),_V(0,-1,0),stage[currentStage].rollthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	stage[currentStage].th_att_h[1]=CreateThruster(_V(-1,0,0),_V(0,1,0),stage[currentStage].rollthrust,stage[currentStage].tank,stage[currentStage].isp*100);
	CreateThrusterGroup(stage[currentStage].th_att_h,2,THGROUP_ATT_BANKRIGHT);

	return;
}

//Creates Main Thrusters and relevant Exhausts
void Multistage2015::CreateMainThruster(){

 if(stage[currentStage].nEngines==0){    //if no stage engines are defined there will be anyway one displayed
	 stage[currentStage].nEngines=1;
	 stage[currentStage].eng[0].x=0;
	 stage[currentStage].eng[0].y=0;
	 stage[currentStage].eng[0].z=-stage[currentStage].height*0.5;

 }


	if(Misc.thrustrealpos){
		for(int i=0;i<stage[currentStage].nEngines;i++){
			stage[currentStage].th_main_h[i]=CreateThruster(stage[currentStage].off,stage[currentStage].eng_dir,stage[currentStage].thrust/stage[currentStage].nEngines,stage[currentStage].tank,stage[currentStage].isp);
		}
		//stage[currentStage].th_main_h[0]=CreateThruster(stage[currentStage].off,stage[currentStage].eng_dir,stage[currentStage].thrust,stage[currentStage].tank,stage[currentStage].isp);
	}else{
		for(int i=0;i<stage[currentStage].nEngines;i++){
			stage[currentStage].th_main_h[i]=CreateThruster(_V(0,0,0),_V(0,0,1),stage[currentStage].thrust/stage[currentStage].nEngines,stage[currentStage].tank,stage[currentStage].isp);
				}
	}
		//stage[currentStage].th_main_h[0]=CreateThruster(_V(0,0,0),_V(0,0,1),stage[currentStage].thrust,stage[currentStage].tank,stage[currentStage].isp);}
 //thg_h_main=CreateThrusterGroup(stage[currentStage].th_main_h,1,THGROUP_MAIN);
	thg_h_main=CreateThrusterGroup(stage[currentStage].th_main_h,stage[currentStage].nEngines,THGROUP_MAIN);
 SetDefaultPropellantResource(stage[currentStage].tank);

 SURFHANDLE ChoosenTexture=NULL; //Initialization of Texture


 int i;
 for(i=0;i<stage[currentStage].nEngines;i++){

	exhaustN[currentStage][i]=AddExhaust(stage[currentStage].th_main_h[i],10*stage[currentStage].eng_diameter*stage[currentStage].engV4[i].t,stage[currentStage].eng_diameter*stage[currentStage].engV4[i].t,stage[currentStage].eng[i],operator*(stage[currentStage].eng_dir,-1),GetProperExhaustTexture(stage[currentStage].eng_tex));

		AddExhaustStream(stage[currentStage].th_main_h[i],stage[currentStage].eng[i],&GetProperPS(stage[currentStage].eng_pstream1));
		AddExhaustStream(stage[currentStage].th_main_h[i],stage[currentStage].eng[i],&GetProperPS(stage[currentStage].eng_pstream2));


	 sprintf(logbuff,"%s: Stage n. %i Engines Exhaust Added--> number of engines: %i , diameter: %.3f, position x: %.3f y: %.3f z: %.3f",GetName(),currentStage+1,stage[currentStage].nEngines,stage[currentStage].eng_diameter*stage[currentStage].engV4[i].t,stage[currentStage].eng[i].x,stage[currentStage].eng[i].y,stage[currentStage].eng[i].z);
	 oapiWriteLog(logbuff);
 }

 return;
}

//Get Boosters Position given group number and booster number inside the group
VECTOR3 Multistage2015::GetBoosterPos(int nBooster, int N){

	VECTOR3 bpos=booster[nBooster].off;
	double arg=booster[nBooster].angle*RAD+(N-1)*2*PI/booster[nBooster].N;
	VECTOR3 bposdef=_V(bpos.x*cos(arg)-bpos.y*sin(arg),bpos.x*sin(arg)+bpos.y*cos(arg),bpos.z);

return bposdef;
}

char* Multistage2015::GetProperPayloadMeshName(int pnl, int n){
	if(n==0){
		return payload[pnl].meshname0;
	}else if(n==1){
		return payload[pnl].meshname1;
	}else if(n==2){
		return payload[pnl].meshname2;
	}else if(n==3){
		return payload[pnl].meshname3;
	}else if(n==4){
		return payload[pnl].meshname4;}
}


void Multistage2015::RotatePayload(int pns, int nm,VECTOR3 anglesrad){

	VECTOR3 state=_V(anglesrad.x/(2*PI),anglesrad.y/(2*PI),anglesrad.z/(2*PI));//portion of 2*PI to rotate
	VECTOR3 reference;

	if(nm==0){reference=_V(0,0,0);}else{reference=operator-(payload[pns].off[nm]-payload[pns].off[0]);}
MGROUP_ROTATE *payloadrotatex=new MGROUP_ROTATE(payload[pns].msh_idh[nm],NULL,NULL,reference,_V(1,0,0),(float)2*PI);
UINT rotateplx=CreateAnimation(0);
ANIMATIONCOMPONENT_HANDLE anim_x=AddAnimationComponent(rotateplx,0.0f,1.0f,payloadrotatex);
SetAnimation(rotateplx,state.x);


MGROUP_ROTATE *payloadrotatey=new MGROUP_ROTATE(payload[pns].msh_idh[nm],NULL,NULL,reference,_V(0,1,0),(float)2*PI);
UINT rotateply=CreateAnimation(0);
ANIMATIONCOMPONENT_HANDLE anim_y=AddAnimationComponent(rotateply,0.0f,1.0f,payloadrotatey);
SetAnimation(rotateply,state.y);

MGROUP_ROTATE *payloadrotatez=new MGROUP_ROTATE(payload[pns].msh_idh[nm],NULL,NULL,reference,_V(0,0,1),(float)2*PI);
UINT rotateplz=CreateAnimation(0);
ANIMATIONCOMPONENT_HANDLE anim_z=AddAnimationComponent(rotateplz,0.0f,1.0f,payloadrotatez);
SetAnimation(rotateplz,state.z);


return;
}

VECTOR3 Multistage2015::RotateVector(const VECTOR3& input, double angle, const VECTOR3& rotationaxis)
{
	// To rotate a vector in 3D space we'll need to build a matrix, these are the variables treqired to do so.
	MATRIX3 rMatrix;
	double c = cos(angle);
	double s = sin(angle);
	double t = 1.0 - c;
	double x = rotationaxis.x;
	double y = rotationaxis.y;
	double z = rotationaxis.z;

	// Build rotation matrix
	rMatrix.m11 = (t * x * x + c);
	rMatrix.m12 = (t * x * y - s * z);
	rMatrix.m13 = (t * x * z + s * y);
	rMatrix.m21 = (t * x * y + s * z);
	rMatrix.m22 = (t * y * y + c);
	rMatrix.m23 = (t * y * z - s * x);
	rMatrix.m31 = (t * x * z - s * y);
	rMatrix.m32 = (t * y * z + s * x);
	rMatrix.m33 = (t * z * z + c);

	// Perform Rotation
	VECTOR3 output = mul(rMatrix, input); // multiply the input vector by our rotation matrix to get our output vector
	return output; // Return rotated vector
}

//Load Meshes
void Multistage2015::LoadMeshes(){

 int q;
 for(q=currentStage;q<nStages;q++){
	 VECTOR3 pos=stage[q].off;
	 stage[q].msh_h=oapiLoadMeshGlobal(stage[q].meshname);
	 sprintf(logbuff,"%s: Stage n.%i Mesh Preloaded: %s",GetName(),q+1,stage[q].meshname);
		oapiWriteLog(logbuff);
	stage[q].msh_idh=AddMesh(stage[q].msh_h,&pos);
	sprintf(logbuff,"%s: Stage n.%i Mesh Added Mesh: %s @ x:%.3f y:%.3f z:%.3f",GetName(),q+1,stage[q].meshname,pos.x,pos.y,pos.z);
		oapiWriteLog(logbuff);
		if(stage[q].wInter==TRUE){

	VECTOR3 inspos=stage[q].interstage.off;
		stage[q].interstage.msh_h=oapiLoadMeshGlobal(stage[q].interstage.meshname);
		sprintf(logbuff,"%s: Interstage Mesh Preloaded for Stage %i",GetName(),q+1);
		oapiWriteLog(logbuff);
		stage[q].interstage.msh_idh=AddMesh(stage[q].interstage.msh_h,&inspos);
		sprintf(logbuff,"%s: Interstage Mesh Added: %s @ x:%.3f y:%.3f z:%.3f",GetName(),stage[q].interstage.meshname,inspos.x,inspos.y,inspos.z);
		oapiWriteLog(logbuff);
		}
	}


int pns;
for(pns=currentPayload;pns<nPayloads;pns++){
	if(!payload[pns].live){
	for(int nm=0;nm<payload[pns].nMeshes;nm++){
		VECTOR3 pos=payload[pns].off[nm];
		payload[pns].msh_h[nm]=oapiLoadMeshGlobal(GetProperPayloadMeshName(pns,nm));
		sprintf(logbuff,"%s Payload Mesh Preloaded %i",GetName(),pns+1);
		oapiWriteLog(logbuff);
		payload[pns].msh_idh[nm]=AddMesh(payload[pns].msh_h[nm],&pos);
		sprintf(logbuff,"%s: Payload n.%i Mesh Added: %s @ x:%.3f y:%.3f z:%.3f",GetName(),pns+1,GetProperPayloadMeshName(pns,nm),pos.x,pos.y,pos.z);
		oapiWriteLog(logbuff);
		if((payload[pns].render==0)&&(wFairing==1)){
		SetMeshVisibilityMode(payload[pns].msh_idh[nm],MESHVIS_NEVER);
		}
		if(payload[pns].rotated){
		RotatePayload(pns,nm,payload[pns].Rotation);
			}
		}
	}else{
	VECTOR3 direction,normal;
		if(!payload[pns].rotated){direction=_V(0,0,1);normal=_V(0,1,0);}else{direction=payload[pns].Rotation;
			VECTOR3 rotation;
			rotation=payload[pns].Rotation;
			direction=mul(RotationMatrix(rotation),_V(0,0,1));
			normal=mul(RotationMatrix(rotation),_V(0,1,0));
			normalise(normal);
			normalise(direction);
				}

	live_a[pns]=CreateAttachment(false,payload[pns].off[0],direction,normal,"MS2015",false);

	}
}
 int qb;
 for(qb=currentBooster;qb<nBoosters;qb++){
 VECTOR3 bpos=booster[qb].off;
 VECTOR3 bposxy=bpos;
 bposxy.z=0;
 double bro=length(bposxy);
 int NN;
for(NN=1;NN<booster[qb].N+1;NN++){
		char boosbuff[32],boosmhname[MAXLEN];
		sprintf(boosbuff,"_%i",NN);
		strcpy(boosmhname,booster[qb].meshname);
		strcat(boosmhname,boosbuff);

		double arg=booster[qb].angle*RAD+(NN-1)*2*PI/booster[qb].N;
		VECTOR3 bposdef=_V(bpos.x*cos(arg)-bpos.y*sin(arg),bpos.x*sin(arg)+bpos.y*cos(arg),bpos.z);
		booster[qb].msh_h[NN]=oapiLoadMeshGlobal(boosmhname);
		sprintf(logbuff,"%s: Booster Mesh Preloaded: %s",GetName(),boosmhname);
		oapiWriteLog(logbuff);
		booster[qb].msh_idh[NN]=AddMesh(booster[qb].msh_h[NN],&bposdef);
		sprintf(logbuff,"%s: Booster Mesh Added Mesh: %s @ x:%.3f y:%.3f z:%.3f",GetName(),boosmhname,bposdef.x,bposdef.y,bposdef.z);
		oapiWriteLog(logbuff);
		}
	}
if(wFairing==1){
	VECTOR3 fpos=fairing.off;
	VECTOR3 fposxy=fpos;
	fposxy.z=0;
	double fro=length(fposxy);
	int NF;
	for(NF=1;NF<fairing.N+1;NF++){
		char fairbuff[32],fairmshname[MAXLEN];
		sprintf(fairbuff,"_%i",NF);
		strcpy(fairmshname,fairing.meshname);
		strcat(fairmshname,fairbuff);
		VECTOR3 fposdef=_V(fro*cos(fairing.angle*RAD+(NF-1)*2*PI/fairing.N),fro*sin(fairing.angle*RAD+(NF-1)*2*PI/fairing.N),fpos.z);
		fairing.msh_h[NF]=oapiLoadMeshGlobal(fairmshname);
		sprintf(logbuff,"%s: Fairing Mesh Preloaded: %s", GetName(),fairmshname);
		oapiWriteLog(logbuff);
		fairing.msh_idh[NF]=AddMesh(fairing.msh_h[NF],&fposdef);
		sprintf(logbuff,"%s: Fairing Mesh Added Mesh: %s @ x:%.3f y:%.3f z:%.3f",GetName(),fairmshname,fposdef.x,fposdef.y,fposdef.z);
		oapiWriteLog(logbuff);

		}

	}
if(wAdapter==TRUE){
		VECTOR3 adappos=Adapter.off;
		Adapter.msh_h=oapiLoadMeshGlobal(Adapter.meshname);
		sprintf(logbuff,"%s: Adapter Mesh Preloaded",GetName());
		oapiWriteLog(logbuff);
		Adapter.msh_idh=AddMesh(Adapter.msh_h,&adappos);
		sprintf(logbuff,"%s: Adapter Mesh Added: %s @ x:%.3f y:%.3f z:%.3f",GetName(),Adapter.meshname,adappos.x,adappos.y,adappos.z);
		oapiWriteLog(logbuff);

	}
if(wLes==TRUE)
{
	VECTOR3 LesPos=Les.off;
	Les.msh_h=oapiLoadMeshGlobal(Les.meshname);
	sprintf(logbuff,"%s: Les Mesh Preloaded",GetName());
	oapiWriteLog(logbuff);
	Les.msh_idh=AddMesh(Les.msh_h,&LesPos);
	sprintf(logbuff,"%s: Les Mesh Added %s @ x:%.3f y:%.3f z:%.3f",GetName(),Les.meshname,LesPos.x,LesPos.y,LesPos.z);
	oapiWriteLog(logbuff);
}

	return;
}

//Updates PMI, Cross Sections etc.
void Multistage2015::UpdatePMI(){
	double TotalVolume=0;
	double TotalHeight=0;
	int i;
	for(i=currentStage;i<nStages;i++){
		TotalHeight+=stage[i].height;
		stage[i].volume=stage[i].height*0.25*PI*stage[i].diameter*stage[i].diameter;
		TotalVolume+=stage[i].volume;
		stage[i].interstage.volume=stage[i].interstage.height*0.25*PI*stage[i].interstage.diameter*stage[i].interstage.diameter;
		TotalVolume+=stage[i].interstage.volume;
	}
	int q;
	for(q=currentBooster;q<nBoosters;q++){
		booster[q].volume=booster[q].N*booster[q].height*0.25*PI*booster[q].diameter*booster[q].diameter;
		TotalVolume+=booster[q].volume;
	}

	int k;
	for(k=currentPayload;k<nPayloads;k++){
		//TotalHeight+=payload[k].height; //Not Considered in Height
		payload[k].volume=payload[k].height*0.25*PI*payload[k].diameter*payload[k].diameter;
		TotalVolume+=payload[k].volume;

	}

		if(wFairing==1){
		//TotalHeight+=fairing.height; //Not Considered in Height
		fairing.volume=fairing.height*0.25*PI*fairing.diameter*fairing.diameter;
	TotalVolume+=fairing.volume;
	}
		if(wLes==TRUE){
			Les.volume=Les.height*0.25*PI*Les.diameter*Les.diameter;
			TotalVolume+=Les.volume;
		}
	double PhiEq=sqrt(4*TotalVolume/(PI*TotalHeight));

	double CSX,CSY,CSZ;
	CSX=TotalHeight*PhiEq;
	CSY=CSX;
	CSZ=PI*0.25*PhiEq*PhiEq;
	SetCrossSections(_V(CSX,CSY,CSZ));

	double IZ=(PhiEq*0.5)*(PhiEq*0.5)*0.5;
	double IX,IY;
	IX=(3*(PhiEq*0.5)*(PhiEq*0.5)+TotalHeight*TotalHeight)/12;
	IY=IX;
	SetPMI(_V(IX,IY,IZ));
    SetSize(TotalHeight);

	return;
}

//Update Mass of the Vehicle on call
void Multistage2015::UpdateMass(){

	double EM=stage[currentStage].emptymass;

	int s;
	for(s=currentStage+1;s<nStages;s++){
		EM+=stage[s].emptymass;
		if(stage[s].wInter==TRUE){
			EM+=stage[s].interstage.emptymass;
		}
			 }

	int bs;
	for(bs=currentBooster;bs<nBoosters;bs++){
	EM+=(booster[bs].emptymass*booster[bs].N);
			}

	int pns;
	for(pns=currentPayload;pns<nPayloads;pns++){
		EM+=payload[pns].mass;
	}
	if(wFairing==1){	EM+=2*fairing.emptymass;}
	if(wAdapter==TRUE){ EM+=Adapter.emptymass;}
	if(wLes==TRUE){EM+=Les.emptymass;}
	SetEmptyMass(EM);



	return;
}

//Update mesh offsets
void Multistage2015::UpdateOffsets(){
	double currentDelta=stage[currentStage].off.z;
	int i;
	for(i=currentStage;i<nStages;i++){
		stage[i].off.z-=currentDelta;
		if(stage[i].wInter==TRUE){
			stage[i].interstage.off.z-=currentDelta;
		}
	}
	int p;
	for(p=currentPayload;p<nPayloads;p++){
		for(int s=0;s<payload[p].nMeshes;s++){
		payload[p].off[s].z-=currentDelta;
		}
	}

	int z;
	for(z=currentBooster;z<nBoosters;z++){
		booster[z].off.z-=currentDelta;
	}

	if(wFairing==1){
		fairing.off.z-=currentDelta;
	}

	if(wAdapter==TRUE){
		Adapter.off.z-=currentDelta;
	}
	if(wLes==TRUE){
		Les.off.z-=currentDelta;
	}
	return;
}
void Multistage2015::UpdateLivePayloads(){

	for(int pns=currentPayload;pns<nPayloads;pns++){
		if(payload[pns].live){
		VESSELSTATUS2 vslive;
		memset(&vslive,0,sizeof(vslive));
		vslive.version=2;
		OBJHANDLE checklive=oapiGetVesselByName(payload[pns].name);
		if(oapiIsVessel(checklive)){
		ATTACHMENTHANDLE liveatt;
		VESSEL3 *livepl;
		livepl = (VESSEL3*)oapiGetVesselInterface(checklive);
			  liveatt=livepl->CreateAttachment(TRUE,_V(0,0,0),_V(0,0,-1),_V(0,1,0),"MS2015",FALSE);
			  if(payload[pns].mass<=0){
				  payload[pns].mass=livepl->GetMass();}
			  if(payload[pns].height<=0){
				payload[pns].height=livepl->GetSize();
				payload[pns].diameter=payload[pns].height*0.1;}
				}else{

		VESSEL3 *v;
		OBJHANDLE hObj;
		ATTACHMENTHANDLE liveatt;
		GetStatusEx(&vslive);
		hObj=oapiCreateVesselEx(payload[pns].name,payload[pns].module,&vslive);


		if(oapiIsVessel(hObj)){
              v = (VESSEL3*)oapiGetVesselInterface(hObj);

			  liveatt=v->CreateAttachment(TRUE,_V(0,0,0),_V(0,0,-1),_V(0,1,0),"MS2015",FALSE);

			  AttachChild(hObj,live_a[pns],liveatt);
		      if(payload[pns].mass<=0){
				  payload[pns].mass=v->GetMass();}
			  if(payload[pns].height<=0){
				payload[pns].height=v->GetSize();
				payload[pns].diameter=payload[pns].height*0.1;}
				}
			}
		}
	}
	UpdateMass();
	UpdatePMI();
return;}
//Returns the particlestream specification to use or the empty one if not found
 PARTICLESTREAMSPEC Multistage2015::GetProperPS(char name[MAXLEN]){
	 for(int z=0;z<sizeof(name);z++){
		  name[z]=tolower(name[z]);
	  }
	 char checktxt[MAXLEN];
	  int nt=0;
	  int k;
	for(nt=0;nt<16;nt++){
	  for(k=0;k<MAXLEN;k++){
		  checktxt[k]=Particle.ParticleName[k][nt];
	  }
	 for(int z=0;z<sizeof(checktxt);z++){
		  checktxt[z]=tolower(checktxt[z]);
	  }

	   if(strncmp(name,checktxt,MAXLEN-5)==0){

	return Particle.Pss[nt];
	   }

	}
return Particle.Pss[15];
 }

 //returns the texture to be used or the empty one
SURFHANDLE Multistage2015::GetProperExhaustTexture(char name[MAXLEN]){
	char checktxt[MAXLEN];
	  int nt=0;
	  int k;
	for(nt=0;nt<nTextures;nt++){
	  for(k=0;k<MAXLEN;k++){
		  checktxt[k]=tex.TextureName[k][nt];
	  }

	   if(strncmp(name,checktxt,MAXLEN-5)==0){

	 return tex.hTex[nt];
	   }

	}

return NULL;
}

//creates Ullage engine and exhausts
void Multistage2015::CreateUllage(){
	if(stage[currentStage].ullage.wUllage)
	{
		stage[currentStage].ullage.ignited=FALSE;
		stage[currentStage].ullage.th_ullage=CreateThruster(_V(0,0,0),_V(0,0,1),stage[currentStage].ullage.thrust,stage[currentStage].tank,100000);
		/*VECTOR3 *ullpos=new VECTOR3[stage[currentStage].ullage.N];
		VECTOR3 *ulldir=new VECTOR3[stage[currentStage].ullage.N];
*/
		//double angleprog=0;
		for(int i=0;i<stage[currentStage].ullage.N;i++)
		{
			double ull_angle;
			if(IsOdd((int)stage[currentStage].ullage.N))
			{
			if(i<(int)(stage[currentStage].ullage.N*0.5)){
			 ull_angle=stage[currentStage].ullage.angle*RAD+(i)*((2*PI/stage[currentStage].ullage.N)/stage[currentStage].ullage.rectfactor);

			//angleprog+=ull_angle;
			}else if(i==(int)(stage[currentStage].ullage.N*0.5)){
				ull_angle=PI+stage[currentStage].ullage.angle*RAD;
				/*sprintf(logbuff,"i: %i ull_angle: %.3f",i,ull_angle*DEG);
			 oapiWriteLog(logbuff);*/
			}else{
			 ull_angle=stage[currentStage].ullage.angle*RAD+PI+(i-(int)(stage[currentStage].ullage.N*0.5))*((2*PI/stage[currentStage].ullage.N)/stage[currentStage].ullage.rectfactor);
			 sprintf(logbuff,"i: %i ull_angle: %.3f",i,ull_angle*DEG);
			 oapiWriteLog(logbuff);
			}
			}else{
				ull_angle=stage[currentStage].ullage.angle*RAD+(i)*(2*PI/stage[currentStage].ullage.N);
			}
			VECTOR3 ulldir=RotateVecZ(stage[currentStage].ullage.dir,ull_angle);
			VECTOR3 ullpos=RotateVecZ(stage[currentStage].ullage.pos,ull_angle);
					    //VECTOR3 ull_ofs=operator+(GetBoosterPos(bi,biii),RotateVecZ(booster[bi].eng[bii],angle));

			AddExhaust(stage[currentStage].ullage.th_ullage,stage[currentStage].ullage.length,stage[currentStage].ullage.diameter,ullpos,ulldir,GetProperExhaustTexture(stage[currentStage].ullage.tex));
		}
	}
	return;
}
//Setting up of the vehicle, it's the setclass caps, but called after the ini is parsed
void Multistage2015::VehicleSetup(){

SetRotDrag(_V(0.7,0.7,0.06)); //Got from Vinka

 ///PROPELLANTS
 int bk;
 for(bk=currentBooster;bk<nBoosters;bk++){
  booster[bk].tank=CreatePropellantResource(booster[bk].fuelmass*booster[bk].N);
  sprintf(logbuff,"%s: booster n. %i Tank Added: %.3f kg",GetName(),bk+1,booster[bk].fuelmass*booster[bk].N);
  oapiWriteLog(logbuff);
 }

int k;
for(k=nStages-1;k>currentStage-1;k--){
	 stage[k].tank=CreatePropellantResource(stage[k].fuelmass);
	 sprintf(logbuff,"%s: Stage n. %i Tank Added: %.3f kg",GetName(),k+1,stage[k].fuelmass);
	 oapiWriteLog(logbuff);
 }

 ////ISP
 int r;
 for(r=currentStage;r<nStages;r++){
	 stage[r].isp=stage[r].thrust*stage[r].burntime/stage[r].fuelmass;
 }

 int br;
 for(br=currentBooster;br<nBoosters;br++){
	 booster[br].isp=(booster[br].thrust*booster[br].N)*booster[br].burntime/(booster[br].fuelmass*booster[br].N);
 }

 /////MAIN THRUSTERS
 CreateMainThruster();

 //attitude thrusters
 CreateRCS();

 //add Boosters Engines
 int bi,bii,biii;
 for(bi=currentBooster;bi<nBoosters;bi++){
	 for(int bn=0;bn<booster[bi].N;bn++)
	 {
		 VECTOR3 pos,dir;
		if(Misc.thrustrealpos){
			pos=GetBoosterPos(bi,bn);
			dir=booster[bi].eng_dir;

		}else{
			pos=_V(0,0,0);
			dir=_V(0,0,1);
		}
		//booster[bi].th_booster_h[0]=CreateThruster(pos,dir,booster[bi].N*booster[bi].thrust,booster[bi].tank,booster[bi].isp);
		booster[bi].th_booster_h[bn]=CreateThruster(pos,dir,booster[bi].thrust,booster[bi].tank,booster[bi].isp);
	 }
	booster[bi].Thg_boosters_h = CreateThrusterGroup(booster[bi].th_booster_h,booster[bi].N,THGROUP_USER);

	 Particle.Pss[13].srcsize=stage[0].diameter;
	 Particle.Pss[14].srcsize=0.5*stage[0].diameter;

	if(booster[bi].nEngines==0){
	 for(bii=0;bii<booster[bi].N;bii++){
		  booster[bi].eng[bii]=_V(0,0,-booster[bi].height*0.5);
	 VECTOR3 engofs=operator+(GetBoosterPos(bi,bii),booster[bi].eng[bii]);
	 AddExhaust(booster[bi].th_booster_h[bii],10*booster[bi].eng_diameter,booster[bi].eng_diameter,engofs,operator*(booster[bi].eng_dir,-1),GetProperExhaustTexture(booster[bi].eng_tex));
	 AddExhaustStream(booster[bi].th_booster_h[bii],engofs,&GetProperPS(booster[bi].eng_pstream1));
	 AddExhaustStream(booster[bi].th_booster_h[bii],engofs,&GetProperPS(booster[bi].eng_pstream2));

	 sprintf(logbuff,"%s: Booster Engines Exhaust Added--> Booster Group: %i number of engines: %i , diameter: %.3f, position x: %.3f y: %.3f z: %.3f",GetName(),bi+1,booster[bi].nEngines,booster[bi].eng_diameter,engofs.x,engofs.y,engofs.z);
	 oapiWriteLog(logbuff);
	 }
 }else{

 for(bii=0;bii<booster[bi].nEngines;bii++){
	for(biii=1;biii<booster[bi].N+1;biii++){
		double angle=booster[bi].angle*RAD+(biii-1)*2*PI/booster[bi].N;

	 VECTOR3 engofs=operator+(GetBoosterPos(bi,biii),RotateVecZ(booster[bi].eng[bii],angle));

	 AddExhaust(booster[bi].th_booster_h[biii-1],10*booster[bi].eng_diameter,booster[bi].eng_diameter,engofs,operator*(booster[bi].eng_dir,-1),GetProperExhaustTexture(booster[bi].eng_tex));
	 AddExhaustStream(booster[bi].th_booster_h[biii-1],engofs,&GetProperPS(booster[bi].eng_pstream1));
	 AddExhaustStream(booster[bi].th_booster_h[biii-1],engofs,&GetProperPS(booster[bi].eng_pstream2));

	 sprintf(logbuff,"%s: Booster Engines Exhaust Added--> Booster Group: %i number of engines: %i , diameter: %.3f, position x: %.3f y: %.3f z: %.3f",GetName(),bi+1,booster[bi].nEngines,booster[bi].eng_diameter,engofs.x,engofs.y,engofs.z);
	 oapiWriteLog(logbuff);
				}
			}
		}
	}

 //Ullage
 CreateUllage();


 CogElev=Misc.COG;

 VECTOR3 intdpts[3],tdpts[3];
 intdpts[0]=_V(0,100,-stage[0].height*0.5-CogElev);
 intdpts[1]=_V(86,-50,-stage[0].height*0.5-CogElev);
 intdpts[2]=_V(-86,-50,-stage[0].height*0.5-CogElev);

 for(int j=0;j<3;j++)
 {
	 tdpts[j].x=intdpts[j].x;
	 tdpts[j].y=intdpts[j].y*cos(VerticalAngle)+intdpts[j].z*sin(VerticalAngle);
	 tdpts[j].z=-intdpts[j].y*sin(VerticalAngle)+intdpts[j].z*cos(VerticalAngle);
 }
 //SetTouchdownPoints(_V(0,100,-stage[0].height*0.5-CogElev),_V(86,-50,-stage[0].height*0.5-CogElev),_V(-86,-50,-stage[0].height*0.5-CogElev));
 SetTouchdownPoints(tdpts[0],tdpts[1],tdpts[2]);


  if(currentBooster<nBoosters) wBoosters=TRUE;

 UpdateMass();
 UpdatePMI();

 return;
}

//Spawns Item
void Multistage2015::Spawn(int type, int current){

char mn[MAXLEN];
VESSELSTATUS2 vs;
memset(&vs,0,sizeof(vs));
vs.version=2;
GetStatusEx(&vs);
VECTOR3 ofs;
VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z},vrot={vs.vrot.x,vs.vrot.y,vs.vrot.z},arot={vs.arot.x,vs.arot.y,vs.arot.z};
VECTOR3 vel;

	switch(type){

	case TBOOSTER:
		int i;
		for(i=1;i<booster[current].N+1;i++){
		GetMeshOffset(booster[current].msh_idh[i],ofs);
		//double vro=length(booster[current].speed);
		//vel =_V(vro*cos(booster[current].angle*RAD+(i-1)*2*PI/booster[current].N),vro*sin(booster[current].angle*RAD+(i-1)*2*PI/booster[current].N),booster[current].speed.z);
		vel=RotateVecZ(booster[current].speed,booster[current].angle*RAD+(i-1)*2*PI/booster[current].N);
		Local2Rel (ofs, vs.rpos);
		GlobalRot(vel,rofs);
		vs.rvel.x=rvel.x+rofs.x;
		vs.rvel.y=rvel.y+rofs.y;
		vs.rvel.z=rvel.z+rofs.z;
		double arg=booster[current].angle*RAD+(i-1)*2*PI/booster[current].N;
		vs.vrot.x=vrot.x+booster[current].rot_speed.x*cos(arg)-booster[current].rot_speed.y*sin(arg);
		vs.vrot.y=vrot.y+booster[current].rot_speed.x*sin(arg)+booster[current].rot_speed.y*cos(arg);
		vs.vrot.z=vrot.z+booster[current].rot_speed.z;

		char mn2[32];
		sprintf(mn2,"_%i",i);
		strcpy(mn,booster[current].meshname);
		strcat(mn,mn2);


		oapiCreateVesselEx(mn,booster[current].module, &vs);
		sprintf(logbuff,"%s: Booster n.%i jettisoned name: %s @%.3f",GetName(),current+1,mn,MET);
		oapiWriteLog(logbuff);

		}
break;
	case TSTAGE:
		GetMeshOffset(stage[current].msh_idh,ofs);
		vel =_V(stage[current].speed.x,stage[current].speed.y,stage[current].speed.z);
		Local2Rel (ofs, vs.rpos);
		GlobalRot(vel,rofs);
		vs.rvel.x=rvel.x+rofs.x;
		vs.rvel.y=rvel.y+rofs.y;
		vs.rvel.z=rvel.z+rofs.z;
		vs.vrot.x=vrot.x+stage[current].rot_speed.x;
		vs.vrot.y=vrot.y+stage[current].rot_speed.y;
		vs.vrot.z=vrot.z+stage[current].rot_speed.z;

		strcpy(mn,stage[current].meshname);

		oapiCreateVesselEx(mn,stage[current].module, &vs);
		sprintf(logbuff,"%s: Stage n.%i jettisoned name: %s @%.3f",GetName(),current+1,mn,MET);
		oapiWriteLog(logbuff);
		stage_ignition_time=MET;
break;

	case TPAYLOAD:
		if(!payload[current].live){
			GetMeshOffset(payload[current].msh_idh[0],ofs);
			vel =_V(payload[current].speed.x,payload[current].speed.y,payload[current].speed.z);
			Local2Rel (ofs, vs.rpos);
			GlobalRot(vel,rofs);
			vs.rvel.x=rvel.x+rofs.x;
			vs.rvel.y=rvel.y+rofs.y;
			vs.rvel.z=rvel.z+rofs.z;
			vs.vrot.x=vrot.x+payload[current].rot_speed.x;
			vs.vrot.y=vrot.y+payload[current].rot_speed.y;
			vs.vrot.z=vrot.z+payload[current].rot_speed.z;

			if(payload[current].rotated){
			MATRIX3 RotMatrix,RotMatrix_Def;
			GetRotationMatrix(RotMatrix);
			VECTOR3 rotation;
			rotation=payload[current].Rotation;
			RotMatrix_Def=mul(RotMatrix,RotationMatrix(rotation));
			vs.arot.x=atan2(RotMatrix_Def.m23,RotMatrix_Def.m33);
			vs.arot.y=-asin(RotMatrix_Def.m13);
			vs.arot.z=atan2(RotMatrix_Def.m12,RotMatrix_Def.m11);
			}
			OBJHANDLE hpl;
			hpl=oapiCreateVesselEx(payload[current].name,payload[current].module, &vs);
			if(currentPayload+1==Misc.Focus){
			oapiSetFocusObject(hpl);
					}
		}else{
			if(GetAttachmentStatus(live_a[current])){
				OBJHANDLE live=GetAttachmentStatus(live_a[current]);
				VESSEL3 *v;
				v=(VESSEL3*)oapiGetVesselInterface(live);
				DetachChild(live_a[current],length(payload[current].speed));
				if(currentPayload+1==Misc.Focus){
				oapiSetFocusObject(live);
					}
				}
			}
		sprintf(logbuff,"%s: Payload n.%i jettisoned name: %s @%.3f",GetName(),current+1,payload[current].name,MET);
		oapiWriteLog(logbuff);

		break;
	case TFAIRING:

		int ii;
		for(ii=1;ii<fairing.N+1;ii++){

		GetMeshOffset(fairing.msh_idh[ii],ofs);

		double vro=length(fairing.speed);
		vel =_V(vro*cos(fairing.angle*RAD+(ii-1)*2*PI/fairing.N),vro*sin(fairing.angle*RAD+(ii-1)*2*PI/fairing.N),fairing.speed.z);
		//vel=RotateVecZ(fairing.speed,fairing.angle*RAD+(ii-1)*2*PI/fairing.N);
		Local2Rel (ofs, vs.rpos);
		GlobalRot(vel,rofs);
		vs.rvel.x=rvel.x+rofs.x;
		vs.rvel.y=rvel.y+rofs.y;
		vs.rvel.z=rvel.z+rofs.z;

		double arg=(ii-1)*2*PI/fairing.N;
		vs.vrot.x=vrot.x+fairing.rot_speed.x*cos(arg)-fairing.rot_speed.y*sin(arg);
		vs.vrot.y=vrot.y+fairing.rot_speed.x*sin(arg)+fairing.rot_speed.y*cos(arg);
		vs.vrot.z=vrot.z+fairing.rot_speed.z;

		char mn2[32];
		sprintf(mn2,"_%i",ii);
		strcpy(mn,fairing.meshname);
		strcat(mn,mn2);

		oapiCreateVesselEx(mn,fairing.module, &vs);
		sprintf(logbuff,"%s: Fairing jettisoned: name %s @%.3f",GetName(),mn,MET);
		oapiWriteLog(logbuff);
		}

		break;
case TINTERSTAGE:

	GetMeshOffset(stage[current].interstage.msh_idh,ofs);

		vel =_V(stage[current].interstage.speed.x,stage[current].interstage.speed.y,stage[current].interstage.speed.z);
		Local2Rel (ofs, vs.rpos);
		GlobalRot(vel,rofs);
		vs.rvel.x=rvel.x+rofs.x;
		vs.rvel.y=rvel.y+rofs.y;
		vs.rvel.z=rvel.z+rofs.z;
		vs.vrot.x=vrot.x+stage[current].interstage.rot_speed.x;
		vs.vrot.y=vrot.y+stage[current].interstage.rot_speed.y;
		vs.vrot.z=vrot.z+stage[current].interstage.rot_speed.z;

		strcpy(mn,stage[current].interstage.meshname);

		oapiCreateVesselEx(mn,stage[current].interstage.module, &vs);
		sprintf(logbuff,"%s: Interstage of stage %i jettisoned name: %s @%.3f",GetName(),current+1,mn,MET);
		oapiWriteLog(logbuff);

		break;
case TLES:

		GetMeshOffset(Les.msh_idh,ofs);

		vel =_V(Les.speed.x,Les.speed.y,Les.speed.z);
		Local2Rel (ofs, vs.rpos);
		GlobalRot(vel,rofs);
		vs.rvel.x=rvel.x+rofs.x;
		vs.rvel.y=rvel.y+rofs.y;
		vs.rvel.z=rvel.z+rofs.z;
		vs.vrot.x=vrot.x+Les.rot_speed.x;
		vs.vrot.y=vrot.y+Les.rot_speed.y;
		vs.vrot.z=vrot.z+Les.rot_speed.z;

		strcpy(mn,Les.meshname);

		oapiCreateVesselEx(mn,Les.module, &vs);
		sprintf(logbuff,"%s: Les jettisoned name: %s @%.3f",GetName(),mn,MET);
		oapiWriteLog(logbuff);
	break;


	}
return;
}

//Jettison item
void Multistage2015::Jettison(int type,int current){
	switch(type){
	case TBOOSTER:
		Spawn(type,current);

		int i;
		for(i=1;i<booster[current].N+1;i++){
		DelMesh(booster[current].msh_idh[i]);
		}

		//DelThruster(booster[current].th_booster_h[0]);
		DelThrusterGroup(booster[current].Thg_boosters_h,TRUE);
		DelPropellantResource(booster[current].tank);
		currentBooster+=1;

		UpdateMass();
		UpdatePMI();
		if(currentBooster>=nBoosters){wBoosters=FALSE;}
	break;

	case TSTAGE:
		Spawn(type,current);


		DelMesh(stage[current].msh_idh);
		ClearThrusterDefinitions();
		//DelThrusterGroup(thg_h_main,TRUE);
		//DelThruster(stage[current].th_main_h[0]);
		DelPropellantResource(stage[current].tank);
		currentStage+=1;
		UpdateMass();
		UpdatePMI();
		CreateUllage();
		CreateMainThruster();
		CreateRCS();
		ShiftCG(_V(0,0,(stage[current+1].off.z-stage[current].off.z)));
		SetCameraOffset(_V(0,0,0));

		break;

	case TPAYLOAD:
		Spawn(type,current);
		if(!payload[current].live){
		for(int ss=0;ss<payload[current].nMeshes;ss++){
			DelMesh(payload[current].msh_idh[ss]);}
		}
		currentPayload+=1;
		UpdateMass();
		UpdatePMI();

		break;
	case TFAIRING:
		Spawn(type,current);
		int pns;
		for(pns=currentPayload;pns<nPayloads;pns++){
			for(int s=0;s<payload[pns].nMeshes;s++){
				SetMeshVisibilityMode(payload[pns].msh_idh[s],MESHVIS_EXTERNAL);
			}
		}
		int ii;
		for(ii=1;ii<fairing.N+1;ii++){
		DelMesh(fairing.msh_idh[ii]);
		}


		wFairing=0;
		UpdateMass();
		UpdatePMI();
		break;
	case TLES:
		Spawn(type,current);
		DelMesh(Les.msh_idh);
		wLes=FALSE;
		UpdateMass();
		UpdatePMI();
		break;
case TINTERSTAGE:
	Spawn(type,current);
	DelMesh(stage[current].interstage.msh_idh);
		currentInterstage+=1;
		stage[current].wInter=FALSE;
		UpdateMass();
		UpdatePMI();
	break;

	}
return;
}

//initialize correctly the delays if flying from a scenario not starting on ground
void Multistage2015::InitializeDelays(){

	//initialize booster burn delays, only if stage is 0 and if Met is positive
	if((currentStage==0)&&(MET>0)){
		for(int kb=currentBooster;kb<nBoosters;kb++){
			if(booster[kb].currDelay>0){
				booster[kb].currDelay-=MET;
			}
		}
	}

	//initialize stages using already used by vinka STAGE_IGNITION_TIME even if it should be called "STAGE_JETTISONED_TIME"
	else if(currentStage>0){
		double delta=MET-stage_ignition_time;
		if(delta<stage[currentStage].currDelay){
		stage[currentStage].currDelay-=delta;
		}

	}
return;
}


void Multistage2015::AutoJettison(){

	if(currentBooster<nBoosters){

		if(GetPropellantMass(booster[currentBooster].tank)<=0.000001){
				Jettison(TBOOSTER,currentBooster);

		}
	}

	if(currentStage<nStages-1){
		if((currentStage==0)&&(currentBooster<nBoosters)){return;}
			else if(GetPropellantMass(stage[currentStage].tank)<=0.1){
				Jettison(TSTAGE,currentStage);

		}

	}
	if((stage[currentStage].wInter==TRUE)&&(stage[currentStage].interstage.currDelay<=0)){
		Jettison(TINTERSTAGE,currentStage);
	}
	return;
}
void Multistage2015::Guidance_Debug(){
	int step=VinkaGetStep(MET);
	double DesiredPitch;
	if(Gnc_step[step].GNC_Comand==CM_ROLL){
	DesiredPitch=(Gnc_step[step].val_init+(Gnc_step[step].val_fin-Gnc_step[step].val_init)*(MET-Gnc_step[step].time_init)/((Gnc_step[VinkaFindFirstPitch()].time-1)-Gnc_step[step].time_init))*RAD;//88*RAD;
	double heading;
	oapiGetHeading(GetHandle(),&heading);
	sprintf(oapiDebugString(),"MET: %.1f Step: %i P: %.2f (%.2f) H: %.2f (%.2f)",MET,step,GetPitch()*DEG,DesiredPitch*DEG,heading*DEG,VinkaAzimuth*DEG);
	}else if(Gnc_step[step].GNC_Comand==CM_PITCH){
	DesiredPitch=(Gnc_step[step].val_init+(Gnc_step[step].val_fin-Gnc_step[step].val_init)*(MET-Gnc_step[step].time_init)/(Gnc_step[step].time_fin-Gnc_step[step].time_init))*RAD;
	sprintf(oapiDebugString(),"MET: %.1f Step: %i P: %.2f (%.2f) Delta: %.1f",MET,step,GetPitch()*DEG,DesiredPitch*DEG,GetPitch()*DEG-DesiredPitch*DEG);
	}else{
	sprintf(oapiDebugString(),"MET: %.1f Step: %i",MET,step);
	}






	return;
}

void Multistage2015::ComplexFlight()
{
	UpdateComplex+=oapiGetSimStep();
	if(UpdateComplex>=1)
	{
		UpdateComplex=0;

		for(int i=0;i<stage[currentStage].nEngines;i++)
		{

			double newMax=(stage[currentStage].thrust/stage[currentStage].nEngines)*(1+(stage[currentStage].engine_amp[i]*sin(2*PI/stage[currentStage].freq[i]*MET+stage[currentStage].engine_phase[i])));
			SetThrusterMax0(stage[currentStage].th_main_h[i],newMax);
			/*double test=(1+stage[currentStage].engine_amp[i]*sin(2*PI/60*MET+stage[currentStage].engine_phase[i]));
			sprintf(logbuff,"MET:%.1f engine:%i TEST:%.6f",MET,i,test);
			oapiWriteLog(logbuff);*/
		}
		if(wBoosters)
		{
			for(int j=0;j<booster[currentBooster].N;j++)
			{
				double newMax=booster[currentBooster].thrust*(1+(booster[currentBooster].engine_amp[j]*sin(2*PI/booster[currentBooster].freq[j]*MET+booster[currentBooster].engine_phase[j])));
				SetThrusterMax0(booster[currentBooster].th_booster_h[j],newMax);
			}
		}
	}


	return;
}

void Multistage2015::Boiloff()
{
	updtboiloff+=oapiGetSimStep();

	if(updtboiloff>=3600)
	{
		updtboiloff=0;
		for(int i=currentStage;i<nStages;i++)
		{
			if(stage[i].wBoiloff)
			{
				double propmass=GetPropellantMass(stage[i].tank);
				propmass-=1;
				SetPropellantMass(stage[i].tank,propmass);

			}
		}
	}
	return;
}

void Multistage2015::FLY(double simtime, double simdtime, double mjdate){

	if(stage[currentStage].currDelay>0){ stage[currentStage].currDelay-=simdtime; }
	if(stage[currentStage].interstage.currDelay>0) {stage[currentStage].interstage.currDelay-=simdtime;}
	if(wBoosters){for(int nb=0;nb<nBoosters;nb++){
	if(booster[nb].currDelay>0){ booster[nb].currDelay-=simdtime;
	}}}
	if(!AJdisabled){AutoJettison();}
	double Level[10];
	double btime[10];

	if((!stage[currentStage].Ignited)&&(stage[currentStage].currDelay<=0)){
		SetThrusterGroupLevel(THGROUP_MAIN,1);
		stage[currentStage].Ignited=TRUE;
		stage[currentStage].IgnitionTime=MET;
		sprintf(logbuff,"%s Stage n: %i ignited @%.1f",GetName(),currentStage+1,stage[currentStage].IgnitionTime);
		oapiWriteLog(logbuff);
	}
	//MET+=simdtime;


	//BOOSTERS SECTION
	if((wBoosters)&&(GetThrusterGroupLevel(THGROUP_MAIN)>0.95)){

	int kb;
	for(kb=currentBooster;kb<nBoosters;kb++){
		if(booster[kb].currDelay<=0){
			if(booster[kb].Ignited==FALSE){
			booster[kb].Ignited=TRUE;
			booster[kb].IgnitionTime=MET;

			sprintf(logbuff,"%s Booster n: %i ignited @%.1f",GetName(),kb+1,booster[kb].IgnitionTime);
			oapiWriteLog(logbuff);
			}else{

			btime[kb]=MET-booster[kb].IgnitionTime;
			Level[kb]=1;
			double m,q;
			int qq;
			for(qq=0;qq<10;qq++){

			if(btime[kb]>booster[kb].curve[qq].x){
					if(qq<9){

				if(btime[kb]<booster[kb].curve[qq+1].x){

			m=(booster[kb].curve[qq+1].y-booster[kb].curve[qq].y)/(booster[kb].curve[qq+1].x-booster[kb].curve[qq].x);
			q=booster[kb].curve[qq].y-m*booster[kb].curve[qq].x;
			Level[kb]=(m*btime[kb]+q)/100;
													}

				}else{
				m=(booster[kb].curve[qq].y-booster[kb].curve[qq].y)/(booster[kb].curve[qq].x-booster[kb].curve[qq].x);
				q=booster[kb].curve[qq].y-m*booster[kb].curve[qq].x;
				Level[kb]=(m*btime[kb]+q)/100;

					}
				}
				}
				}


			//SetThrusterLevel(booster[kb].th_booster_h[0],Level[kb]);
			//SetThrusterGroupLevel(booster[kb].Thg_boosters_h,Level[kb]);
			SetThrusterGroupLevel(booster[kb].Thg_boosters_h,Level[kb]);


		}
			}
	}




	//if((APstat)&&(wVinkasGuidance)){
	if(APstat){
		//MET+=simdtime;
	/*	VinkaAutoPilot();
		if(Misc.GNC_Debug==1){Guidance_Debug();}
			if(MET>VinkaFindEndTime()){
				killAP();
				APstat=100;
				}*/

		VinkaAutoPilot();
		if(Misc.GNC_Debug==1){Guidance_Debug();}
		if(MET>VinkaFindEndTime()){
			killAP();
			APstat=FALSE;
					}

		}

	//avoid reignition of not reignitable stages by setting thurst to 0
	if((!stage[currentStage].reignitable)&&(stage[currentStage].Ignited))
	{
		if(GetThrusterGroupLevel(THGROUP_MAIN)==0){
		SetThrusterResource(stage[currentStage].th_main_h[0],NULL);
		}
	}
	if(tlmidx<TLMSECS){Telemetry();}

	if(stage[currentStage].batteries.wBatts)
	{
		stage[currentStage].batteries.CurrentCharge-=oapiGetSimStep();
		if(stage[currentStage].batteries.CurrentCharge<=0){
			stage[currentStage].batteries.CurrentCharge=0;
			ClearThrusterDefinitions();
		}
	}

	if(Complex)
	{
		ComplexFlight();
	}

	if(stage[currentStage].ullage.wUllage)
	{
		if((!stage[currentStage].ullage.ignited)&&(stage[currentStage].currDelay<stage[currentStage].ullage.anticipation))//&&(MET-stage[currentStage].IgnitionTime<stage[currentStage].ullage.overlap))
		{
			SetThrusterLevel(stage[currentStage].ullage.th_ullage,1);
			stage[currentStage].ullage.ignited=TRUE;
		}else if((stage[currentStage].ullage.ignited)&&(stage[currentStage].IgnitionTime!=0)&&(MET-stage[currentStage].IgnitionTime>stage[currentStage].ullage.overlap)){

			SetThrusterLevel(stage[currentStage].ullage.th_ullage,0);
		}
	}



//	Boiloff();

	MET+=simdtime;
return;
}


//returns hours mins and secs of a time (positive and negative)
VECTOR3 Multistage2015::hms(double time){
	VECTOR3 met;
	if(time==0){met=_V(0,0,0);}else{
	time=abs(time+0.5*(time/abs(time)-1));
	met.x=floor(time/3600)-0.5*(time/abs(time)-1);
	met.y=floor((time-met.x*3600*(time/abs(time)))/60)-0.5*(time/abs(time)-1);
	met.z=floor(time-met.x*3600*(time/abs(time))-met.y*60*(time/abs(time)));}
	return met;
}

double Multistage2015::GetProperNforCGTE(double time){
	double n;
	double Thrust=stage[0].thrust;
	double BoosterFlow=0;
	double BoosterFuelMassBurnt=0;
	for(int i=0;i<nBoosters;i++){
		if((booster[i].burndelay<time)&&(time<(booster[i].burndelay+booster[i].burntime))){
		Thrust+=booster[i].thrust*booster[i].N;
	BoosterFlow=((booster[i].fuelmass*booster[i].N)/booster[i].burntime);
	BoosterFuelMassBurnt=BoosterFlow*(time-booster[i].burndelay);
		}}
	double mass=GetMass();
	double FirstStageFlow=stage[0].fuelmass/stage[0].burntime;

	double substr=time*(FirstStageFlow)+BoosterFuelMassBurnt; //Grande approssimazione
	mass-=substr;
	n=Thrust/(mass*g0);

	//sprintf(logbuff,"Thrust %.3f fsflow %.3f boostersflow %.3f mass %.3f n %.3f",Thrust,FirstStageFlow,BoostersFlow,mass,n);
	//oapiWriteLog(logbuff);
	return n;
}
bool Multistage2015::CGTE(double psi0){
	double t0,v0,x0,y0;
  double deltaPsi0,z0,C,psi,z,v,deltaT,deltax,deltay,x,y;
	//t0=3;

	double Thrust=stage[0].thrust;
	double BoostersFlow=0;
	for(int i=0;i<nBoosters;i++){
		if(booster[i].burndelay==0){  //accounting for boosters not immediatly ignited
			Thrust+=booster[i].thrust*booster[i].N;}
	}
	 double mass=GetMassAtStage(0,FALSE);

	 x0=0;
	 //y0=v0*t0;
	 y0=altsteps[1];
	 t0=sqrt((2*altsteps[1])/((Thrust/mass)-g0));
	 v0=t0*(Thrust/mass)-g0;
//	sprintf(logbuff,"CGTE init cond v0: %.3f y0: %.3f t0: %.3f",v0,y0,t0);
	// oapiWriteLog(logbuff);
	// deltaPsi0=psi0/100;

	// while(psi0<55*RAD){
		while((t0<500)&&(psi0<80*RAD)){
		 deltaPsi0=psi0/100;
		 double n=GetProperNforCGTE(t0);
		 z0=tan(0.5*psi0);
		 C=v0/((pow(z0,n-1))*(1+z0*z0));
		 psi=psi0+deltaPsi0;
		 z=tan(0.5*psi);
		 v=C*(pow(z,n-1))*(1+z*z);
		 deltaT=C/g0*((pow(z,n-1))*(1/(n-1)+(z*z)/(n+1))-(pow(z0,n-1))*(1/(n-1)+(z0*z0)/(n+1)));
		 deltax=0.5*deltaT*(v0*sin(psi0)+v*sin(psi));
		 deltay=0.5*deltaT*(v0*cos(psi0)+v*cos(psi));
		 x=x0+deltax;
		 y=y0+deltay;
		 x0=x;
		 y0=y;
		 v0=v;
		 psi0=psi;
		 t0+=deltaT;

		/* sprintf(logbuff,"t0 %.3f psi0 %.3f v %.3f x %.3f y %.3f deltaT %.3f",t0,psi0*DEG,v,x,y,deltaT);
		 oapiWriteLog(logbuff);*/
		 if(y0>altsteps[3]){return true;}
	 }

	 return false;
}

void Multistage2015::CheckForAdditionalThrust(int pns){
	if(GetAttachmentStatus(live_a[pns])){
		OBJHANDLE live=GetAttachmentStatus(live_a[pns]);
		VESSEL3 *v;
		v=(VESSEL3*)oapiGetVesselInterface(live);
		VECTOR3 TotalThrustVecPL;;
		v->GetThrustVector(TotalThrustVecPL);
		TotalThrustVecPL=mul(RotationMatrix(payload[pns].Rotation),TotalThrustVecPL);
		AddForce(TotalThrustVecPL,payload[pns].off[0]);
		}
}

void Multistage2015::CheckForFX(int fxtype,double param)
{

	switch(fxtype){
	case FXMACH:
	if((param>FX_Mach.mach_min)&&(param<FX_Mach.mach_max))
		{
			if(!FX_Mach.added)
			{
			FX_Mach.added=TRUE;
			FX_Mach.ps_h=AddParticleStream(&GetProperPS(FX_Mach.pstream),FX_Mach.off,FX_Mach.dir,&lvl);
			}
		}else{
			if(FX_Mach.added==TRUE){
			DelExhaustStream(FX_Mach.ps_h);
			FX_Mach.added=FALSE;
			}
		}
	break;
	case FXVENT:
		for(int fv=1;fv<=nVent;fv++)
		{
			if(param<FX_Vent.time_fin[fv])
			{
				if(!FX_Vent.added[fv])
				{
					FX_Vent.ps_h[fv]=AddParticleStream(&GetProperPS(FX_Vent.pstream),FX_Vent.off[fv],FX_Vent.dir[fv],&lvl);
					FX_Vent.added[fv]=TRUE;
				}
			}else{
				if(FX_Vent.added[fv]==TRUE)
				{
					DelExhaustStream(FX_Vent.ps_h[fv]);
					FX_Vent.added[fv]=FALSE;
				}
			}
		}
		break;
	}
}

void Multistage2015::EvaluateComplexFlight()
{
	srand((unsigned)time(NULL));

	for(int i=0;i<nStages;i++)
	{
		for(int q=0;q<stage[i].nEngines;q++)
		{
			int amplitude=rand()%1500;
			stage[i].engine_amp[q]=(double)amplitude/100000;
			int transval=rand()%180;
			stage[i].engine_phase[q]=(double)transval*RAD;
			int frequency=rand()%60;
			stage[i].freq[q]=30+(double)frequency;
			sprintf(logbuff,"%s Complex Flight-> Stage %i Engine %i Amplitude %.3f phase %.3f frequency %.3f",GetName(),i+1,q+1,stage[i].engine_amp[q],stage[i].engine_phase[q],stage[i].freq[q]);
			oapiWriteLog(logbuff);
		}
	}

	for(int j=0;j<nBoosters;j++)
	{
		for(int z=0;z<booster[j].N;z++)
		{
			int amplitude=rand()%1500;
			booster[j].engine_amp[z]=(double)amplitude/100000;
			int transval=rand()%180;
			booster[j].engine_phase[z]=(double)transval*RAD;
			int frequency=rand()%60;
			booster[j].freq[z]=30+(double)frequency;
			sprintf(logbuff,"%s Complex Flight-> Booster %i Engine %i Amplitude %.3f phase %.3f frequency %.3f",GetName(),j+1,z+1,booster[j].engine_amp[z],booster[j].engine_phase[z],booster[j].freq[z]);
			oapiWriteLog(logbuff);
		}
	}


	return;
}

void Multistage2015::FailuresEvaluation()
{
	srand((unsigned)time(NULL));
	int check=rand()%1000;
	if(check<10*failureProbability)
	{
		timeOfFailure=rand()%300;
	}

	sprintf(logbuff,"%s: Failures->Probabilty: %i Number Extracted: %i Time Of Failure: %i",GetName(),failureProbability,check/10,timeOfFailure);
	oapiWriteLog(logbuff);

}

void Multistage2015::boom()
{
VESSELSTATUS2 vs;
memset(&vs,0,sizeof(vs));
vs.version=2;
GetStatusEx(&vs);

VECTOR3 ofs = _V(0,0,0);
VECTOR3 rofs, rvel = {vs.rvel.x, vs.rvel.y, vs.rvel.z};
VECTOR3 vel = {0,0,0};
Local2Rel (ofs, vs.rpos);
GlobalRot(vel,rofs);
vs.rvel.x=rvel.x+rofs.x;
vs.rvel.y=rvel.y+rofs.y;
vs.rvel.z=rvel.z+rofs.z;

OBJHANDLE hwreck;
hwreck=oapiCreateVesselEx("wreck","boom",&vs);
oapiSetFocusObject(hwreck);
oapiCameraScaleDist(20);
oapiDeleteVessel(GetHandle());
return;
}
void Multistage2015::Failure()
{
	srand((unsigned)time(NULL));
	int check=rand()%1000;
	if((currentStage==0)&&(check<250))
	//if((currentStage==0)&&(check>0))
	{
			boom();
	}else{
		//double newlevel=stage[currentStage].thrust*(1-(1/(double)stage[currentStage].nEngines));

		int engineout=rand()%stage[currentStage].nEngines;
		//SetThrusterMax0(stage[currentStage].th_main_h[engineout],newlevel);

		SetThrusterLevel(stage[currentStage].th_main_h[engineout],0);
	//	DelExhaust(exhaustN[currentStage][engineout]);
		//sprintf(logbuff,"engineout:%i exhaustN:%i oldlevel %.1f newlevel %.1f",engineout,exhaustN[currentStage][engineout],stage[currentStage].thrust,newlevel);
		//oapiWriteLog(logbuff);
	}
}
bool Multistage2015::CheckForFailure(double met)
{

	if((floor(met)==timeOfFailure)&&(!failed))
	{
		sprintf(logbuff,"%s: FAILURE FAILURE FAILURE @%.0f",GetName(),met);
		oapiWriteLog(logbuff);
		failed=TRUE;
		return TRUE;
	}else{
		return FALSE;
	}
}
int Multistage2015::WriteTelemetryFile(int initline){
FILEHANDLE TlmFile;
char filenmbuff[MAXLEN];
sprintf(filenmbuff,"%.2f_%s_MS2015_TLM.txt",oapiGetSysMJD(),GetName());
char buffer[MAXLEN];
if(initline==0){
	TlmFile=oapiOpenFile(filenmbuff,FILE_OUT,ROOT);
sprintf(buffer,"<--!Multistage 2015 Automatically Generated Telemetry File!-->");
oapiWriteLine(TlmFile,buffer);
sprintf(buffer,"MET,Altitude,Speed,Pitch,Thrust,Mass,V-Speed,Acceleration");
oapiWriteLine(TlmFile,buffer);

}else{
	TlmFile=oapiOpenFile(filenmbuff,FILE_APP,ROOT);
}


for(int i=initline;i<tlmidx;i++)
{
	sprintf(buffer,"%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f",tlmAlt[i].x,tlmAlt[i].y,tlmSpeed[i].y,tlmPitch[i].y,tlmThrust[i].y,tlmMass[i].y,tlmVv[i].y,tlmAcc[i].y);
	oapiWriteLine(TlmFile,buffer);
}
if(initline==0){oapiCloseFile(TlmFile,FILE_OUT);}else{oapiCloseFile(TlmFile,FILE_APP);}
	return tlmidx-1;
}
void Multistage2015::Telemetry()
{
	if((updtlm>=1)&&(MET<TLMSECS-1))
	{
		VECTOR3 ThrustVec;
		GetThrustVector(ThrustVec);

		tlmAlt[tlmidx].x=MET;
		tlmSpeed[tlmidx].x=MET;
		tlmPitch[tlmidx].x=MET;
		tlmThrust[tlmidx].x=MET;
		tlmMass[tlmidx].x=MET;
		tlmVv[tlmidx].x=MET;
		tlmAcc[tlmidx].x=MET;

		tlmAlt[tlmidx].y=GetAltitude();
		tlmSpeed[tlmidx].y=GetOS();
		tlmPitch[tlmidx].y=GetPitch()*DEG;
		tlmThrust[tlmidx].y=length(ThrustVec);
		tlmMass[tlmidx].y=GetMass();
		tlmVv[tlmidx].y=GetVPerp();
		tlmAcc[tlmidx].y=getacc();

		tlmidx++;
		updtlm=0;
	}
	updtlm+=oapiGetSimStep();

	if(Misc.telemetry){
	writetlmTimer+=oapiGetSimStep();
	if(writetlmTimer>=60)
	{
		writetlmTimer=0;
		tlmnlines=WriteTelemetryFile(tlmnlines);
	}
	}
	return;
}

void Multistage2015::CalculateAltSteps(double planetmass)
{
	double altref[4]={100,350,1400,35000};
	double earthmass=5.973698968*pow(10.0,24);//1000000000000
	for(int i=0;i<4;i++)
	{
		altsteps[i]=altref[i]*planetmass/earthmass;
		//sprintf(logbuff,"altstep[%i]=%.1f",i,altsteps[i]);
		//oapiWriteLog(logbuff);
	}
	return;
}

//****************************CALLBACKS******************************************************************************
void Multistage2015::clbkSetClassCaps(FILEHANDLE cfg)
{

	initGlobalVars();			// First of all let's initialize everything!




 SetSurfaceFrictionCoeff(0,0);  // to avoid bad vibrations while throttling up at launch
 SetCW(0.2,0.5,1.5,1.5);		// gotten from Vinka
 EnableTransponder(true);
 InitNavRadios(4);
 SetCameraOffset(_V(0,0,100));	//Empirical

 SetVisibilityLimit(0,0.002);
 SetAlbedoRGB(_V((float)255/255,(float)255/255,(float)255/255));
 SetGravityGradientDamping(0);

 SetMaxWheelbrakeForce(200000);

 //Default ParticleStreams Definitions:

 // DEF CONTRAIL
 char transfer[MAXLEN];
 sprintf(transfer,"Contrail");
 for(int k=0;k<MAXLEN;k++){
Particle.ParticleName[k][13]=transfer[k];}
  Particle.Pss[13].flags=0;
  Particle.Pss[13].srcsize=8;
  Particle.Pss[13].srcrate=5;
  Particle.Pss[13].v0=150;
  Particle.Pss[13].srcspread=0.3;
  Particle.Pss[13].lifetime=8;
  Particle.Pss[13].growthrate=4;
  Particle.Pss[13].atmslowdown=3;
  Particle.Pss[13].ltype=PARTICLESTREAMSPEC::EMISSIVE;
  Particle.Pss[13].levelmap=PARTICLESTREAMSPEC::LVL_PSQRT;
  Particle.Pss[13].lmin=0;
  Particle.Pss[13].lmax=0.5;
  Particle.Pss[13].atmsmap=PARTICLESTREAMSPEC::ATM_PLOG;
  Particle.Pss[13].amin=1e-6;
  Particle.Pss[13].amax=0.1;
  Particle.Pss[13].tex=oapiRegisterParticleTexture ("Contrail1");

  //DEF EXHAUST
   sprintf(transfer,"Exhaust");
 for(int q=0;q<MAXLEN;q++){
	Particle.ParticleName[q][14]=transfer[q];}
  Particle.Pss[14].flags=0;
  Particle.Pss[14].srcsize=4;
  Particle.Pss[14].srcrate=20;
  Particle.Pss[14].v0=150; //250
  Particle.Pss[14].srcspread=0.1;
  Particle.Pss[14].lifetime=0.3; //0.6
  Particle.Pss[14].growthrate=12; //20
  Particle.Pss[14].atmslowdown=2;
  Particle.Pss[14].ltype=PARTICLESTREAMSPEC::EMISSIVE;
  Particle.Pss[14].levelmap=PARTICLESTREAMSPEC::LVL_PSQRT;
  Particle.Pss[14].lmin=0;
  Particle.Pss[14].lmax=0.5;
  Particle.Pss[14].atmsmap=PARTICLESTREAMSPEC::ATM_PLOG;
  Particle.Pss[14].amin=1e-6;
  Particle.Pss[14].amax=0.1;
  Particle.Pss[14].tex=oapiRegisterParticleTexture ("Contrail3");

  //DEF EXHAUST
   sprintf(transfer,"Clear");
 for(int z=0;z<MAXLEN;z++){
	Particle.ParticleName[z][15]=transfer[z];}
  Particle.Pss[15].flags=0;
  Particle.Pss[15].srcsize=0;
  Particle.Pss[15].srcrate=0;
  Particle.Pss[15].v0=0; //250
  Particle.Pss[15].srcspread=0;
  Particle.Pss[15].lifetime=0; //0.6
  Particle.Pss[15].growthrate=0; //20
  Particle.Pss[15].atmslowdown=0;
  Particle.Pss[15].ltype=PARTICLESTREAMSPEC::DIFFUSE;
  Particle.Pss[15].levelmap=PARTICLESTREAMSPEC::LVL_FLAT;
  Particle.Pss[15].lmin=1;
  Particle.Pss[15].lmax=1;
  Particle.Pss[15].atmsmap=PARTICLESTREAMSPEC::ATM_FLAT;
  Particle.Pss[15].amin=1;
  Particle.Pss[15].amax=1;




}
void Multistage2015::clbkLoadStateEx(FILEHANDLE scn,void *vs)
{
	oapiWriteLog("Load State Started");
 char *line;
 double batt_trans=0;
 bool loadedbatts=FALSE;
// wVinkasGuidance=FALSE;



 while(oapiReadScenario_nextline(scn,line)){
	  if(!strnicmp(line,"CONFIG_FILE",11)){
		 sscanf(line+11,"%s",&fileini);

	  }else if(!strnicmp(line,"MET",3)){
		 sscanf(line+3,"%lf",&MET);
		/* if((MET<0)||(MET>1000000)){
			 MET=0;
			}*/

	  }else if(!strnicmp(line,"GNC_RUN",7)){
		  sscanf(line+7,"%i",&Gnc_running);
		  if(Gnc_running==1){APstat=TRUE;}

	  }else if(!strnicmp(line,"BATTERY",7)){
		  sscanf(line+7,"%lf",&batt_trans);
		  loadedbatts=TRUE;

	  }else if(!strnicmp(line,"STAGE_IGNITION_TIME",19)){
		  sscanf(line+19,"%lf",&stage_ignition_time);

	  }else if(!strnicmp(line,"FAILURE_PROB",12)){
		  sscanf(line+12,"%i",&failureProbability);

	  }else if(!strnicmp(line,"GNC_AUTO_JETTISON",17)){
		  int AJVal;
		  sscanf(line+17,"%i",&AJVal);
		  if(AJVal==0){AJdisabled=TRUE;}

	  }else if(!strnicmp(line,"GUIDANCE_FILE",13)){
		 sscanf(line+13,"%s",&guidancefile);
		// wVinkasGuidance=TRUE;
		 parseGuidanceFile(guidancefile);
		 if(Gnc_running!=1){
		 VinkaCheckInitialMet();
		 }

	    }else if(!strnicmp(line,"TELEMETRY_FILE",14)){
		 sscanf(line+14,"%s",&tlmfile);
		 parseTelemetryFile(tlmfile);
		 wReftlm=TRUE;

	 }else if(!strnicmp(line,"CURRENT_BOOSTER",15)){
		 sscanf(line+15,"%i",&currentBooster);
		 currentBooster-=1;

	  }else if(!strnicmp(line,"CURRENT_INTERSTAGE",18)){
		 sscanf(line+18,"%i",&currentInterstage);
		 currentInterstage-=1;


	  }else if(!strnicmp(line,"CURRENT_STAGE",13)){
		 sscanf(line+13,"%i",&currentStage);
		 currentStage-=1;
		 if(currentStage>0){currentBooster=11;}


	  }else if(!strnicmp(line,"CURRENT_PAYLOAD",15)){
		 sscanf(line+15,"%i",&currentPayload);
		 currentPayload-=1;


	  }else if(!strnicmp(line,"FAIRING",7)){
		 sscanf(line+7,"%i",&wFairing);
		 if((wFairing<0)||(wFairing>1)){wFairing=0;}

	  }else if(!strnicmp(line,"CONFIGURATION",13)){
		 sscanf(line+13,"%i",&Configuration);
		 if((Configuration<0)||(Configuration>1)){Configuration=0;}

	  }else if(!strnicmp(line,"COMPLEX",7)){
		 Complex=TRUE;


 }else{
   ParseScenarioLineEx(line,vs);
	  }

 }





 char tempFile[MAXLEN];
 strcpy(tempFile,OrbiterRoot);
 strcat(tempFile,"\\");
 strcat(tempFile,fileini);
 sprintf(logbuff,"%s: Config File: %s",GetName(),tempFile);
 oapiWriteLog(logbuff);
 parseinifile(tempFile);

 if((wFairing==1)&&(hasFairing==FALSE)){wFairing=0;}
 if(Configuration==0){			//if only configuration is defined, reset everything
	 if(hasFairing==TRUE){
		  wFairing=1;}
	 currentStage=0;
	 currentPayload=0;
	 currentBooster=0;
	 currentInterstage=0;
 }


 UpdateOffsets();
 VehicleSetup();
 LoadMeshes();

 if(Gnc_running==1){
 InitializeDelays();
 }

 if(loadedbatts)
 {
	 stage[currentStage].batteries.CurrentCharge=batt_trans;
 }

oapiWriteLog("Load State Terminated");
}
int Multistage2015::clbkConsumeBufferedKey(DWORD key,bool down,char *kstate){

	if(KEYDOWN(kstate,OAPI_KEY_J)&&(!KEYMOD_CONTROL(kstate))&&(!KEYMOD_SHIFT(kstate)&&(!KEYMOD_ALT(kstate)))){

		if(currentBooster<nBoosters){
			Jettison(TBOOSTER,currentBooster);
		}else{
		if(currentStage<nStages-1){
			if(stage[currentStage].wInter==TRUE){
				Jettison(TINTERSTAGE,currentStage);
			}else{	Jettison(TSTAGE,currentStage);}

		}else if((currentStage=nStages-1)&&(stage[currentStage].wInter)){
				Jettison(TINTERSTAGE,currentStage);
		}else{

		if((currentPayload<nPayloads)&&(wFairing==0)){
		Jettison(TPAYLOAD,currentPayload);
		}else{return 0;}
		}
		}
	}
	if(KEYDOWN(kstate,OAPI_KEY_F)&&(!KEYMOD_CONTROL(kstate))&&(!KEYMOD_SHIFT(kstate)&&(!KEYMOD_ALT(kstate)))){
		if(wLes){
			Jettison(TLES,0);
		}else{
			if(wFairing==1){Jettison(TFAIRING,0);}else{return 0;}
			}

	}

	if(KEYDOWN(kstate,OAPI_KEY_P)&&(!KEYMOD_CONTROL(kstate))&&(!KEYMOD_SHIFT(kstate)&&(!KEYMOD_ALT(kstate)))){
		if(!APstat){APstat=TRUE;return 0;}else if(APstat){APstat=FALSE; return 0;}else{

			return 0;}}
	return 0;
}
void Multistage2015::clbkSaveState(FILEHANDLE scn)
{
	char savebuff[256],savevalbuff[256];

	SaveDefaultState(scn);

	sprintf(savebuff,"CONFIG_FILE");
	sprintf(savevalbuff,fileini);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

  if(guidancefile[0]!='\0'){
	sprintf(savebuff,"GUIDANCE_FILE");
	sprintf(savevalbuff,guidancefile);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);
  }

	sprintf(savebuff,"CONFIGURATION");
	sprintf(savevalbuff,"%i",Configuration);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

  if(Complex){
  sprintf(savebuff,"COMPLEX");
  sprintf(savevalbuff,"");
  oapiWriteScenario_string(scn,savebuff,savevalbuff);
  }

	sprintf(savebuff,"CURRENT_BOOSTER");
	sprintf(savevalbuff,"%i",currentBooster+1);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

    sprintf(savebuff,"CURRENT_STAGE");
	sprintf(savevalbuff,"%i",currentStage+1);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

    sprintf(savebuff,"CURRENT_INTERSTAGE");
	sprintf(savevalbuff,"%i",currentInterstage+1);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

    sprintf(savebuff,"CURRENT_PAYLOAD");
	sprintf(savevalbuff,"%i",currentPayload+1);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

    sprintf(savebuff,"FAIRING");
	sprintf(savevalbuff,"%i",wFairing);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

    sprintf(savebuff,"MET");
	sprintf(savevalbuff,"%.3f",MET);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

  if(APstat){
	  Gnc_running=1;
    sprintf(savebuff,"GNC_RUN");
	sprintf(savevalbuff,"%i",Gnc_running);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);
  }

  if(stage[currentStage].batteries.wBatts)
  {
	  sprintf(savebuff,"BATTERY");
	  sprintf(savevalbuff,"%lf",stage[currentStage].batteries.CurrentCharge);
	  oapiWriteScenario_string(scn,savebuff,savevalbuff);
  }

  if(AJdisabled){
	  int val=1;
	sprintf(savebuff,"GNC_AUTO_JETTISON");
	sprintf(savevalbuff,"%i",val);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);
  }

  sprintf(savebuff,"STAGE_IGNITION_TIME");
  sprintf(savevalbuff,"%.6f",stage_ignition_time);
  oapiWriteScenario_string(scn,savebuff,savevalbuff);

}
void Multistage2015::clbkPreStep(double simt, double simdt, double mjd){
	if(Configuration==1) {FLY(simt,simdt,mjd);}
	if(wMach)
	{
		CheckForFX(FXMACH,GetMachNumber());
	}
	if((wVent)&&(MET<=5))
	{
		CheckForFX(FXVENT,MET);
	}
	if(wFailures)
	{
		if(CheckForFailure(MET))
		{
			Failure();
		}
	}


	return;
}
void Multistage2015::clbkPostStep(double simt, double simdt, double mjd){

//	if((APstat)&&(wVinkasGuidance)&&(Configuration==0)){//(MET<0)){
		if((APstat)&&(Configuration==0)){//(MET<0)){
		VinkaAutoPilot();
		MET+=simdt;
		stage[0].currDelay=-MET;
		if(Misc.GNC_Debug==1){Guidance_Debug();}
	}


	if((GetThrusterGroupLevel(THGROUP_MAIN)>0.95)&&(Configuration==0)) {
		Configuration=1;
		MET=0;
	}
	//if((Configuration==1)) {FLY(simt,simdt,mjd);}

	/*
	VECTOR3 hhvec,vvvec,rrvec,hloc;
	oapiGetRelativePos(GetSurfaceRef(),GetHandle(),&rrvec);
	oapiGetRelativeVel(GetSurfaceRef(),GetHandle(),&vvvec);
	hhvec=crossp(rrvec,vvvec);
	MATRIX3 m3;
	oapiGetRotationMatrix(GetSurfaceRef(),&m3);
	hloc=tmul(m3,hhvec);
	normalise(hloc);
	double INC=acos(hloc.y)*DEG-90-90;

	sprintf(oapiDebugString(),"x: %.3f y:%.3f z:%.3f INC: %.2f tgtapo:%.1f",hloc.x,hloc.y,hloc.z,INC,tgtapo);
	*/

	for(int pns=currentPayload;pns<nPayloads;pns++)
	{
		if(payload[pns].live){CheckForAdditionalThrust(pns);}
	}



return;
}
bool Multistage2015::clbkDrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp){
	VESSEL3::clbkDrawHUD (mode, hps, skp);
	int cx = hps->CX, cy = hps->CY;

				DWORD w;
				DWORD h;
				DWORD bpp;
				oapiGetViewportSize(&w,&h,&bpp);


			char NAMEbuff[512];
			memset(NAMEbuff, 0, sizeof(NAMEbuff));
			int NAMElen;
			NAMElen=sprintf_s(NAMEbuff,"%s",GetName());
			skp->Text(0.02*w,0.25*h+20,NAMEbuff,NAMElen);


			char METbuff[512];
			memset(METbuff, 0, sizeof(METbuff));
			int METlen;

			if(MET>=0){
				METlen=sprintf_s(METbuff,"MET: %03.0f:%02.0f:%02.0f",hms(MET));
			}else{
				METlen=sprintf_s(METbuff,"T-: %03.0f:%02.0f:%02.0f",hms(MET));
				}
			skp->Text(0.02*w,0.25*h+40,METbuff,METlen);

			char STAGEbuff[512];
			memset(STAGEbuff, 0, sizeof(STAGEbuff));
			int STAGElen;

			STAGElen=sprintf_s(STAGEbuff,"Stage: %i",currentStage+1);
			skp->Text(0.02*w,0.25*h+60,STAGEbuff,STAGElen);

			char BTbuff[512];
			memset(BTbuff, 0, sizeof(BTbuff));
			int BTlen;

			BTlen=sprintf_s(BTbuff,"Remaining Stage Burn Time: %02.0f:%02.0f",hms(RemBurnTime(currentStage)).y,hms(RemBurnTime(currentStage)).z);
			skp->Text(0.02*w,0.25*h+80,BTbuff,BTlen);

			if(runningPeg){
				if(GetAltitude()>35000)
				{
				char PMECObuff[512];
				memset(PMECObuff, 0, sizeof(PMECObuff));
				int PMECOlen;
				VECTOR3 PMECO=hms(GetTMECO());
				PMECOlen=sprintf_s(PMECObuff,"Predicted MECO: %02.0f:%02.0f",PMECO.y,PMECO.z);
				skp->Text(0.02*w,0.25*h+100,PMECObuff,PMECOlen);
				}
			}


			char PLNamebuff[512];
			memset(PLNamebuff, 0, sizeof(PLNamebuff));
			int PLNamelen;
			char PLWbuff[512];
			memset(PLWbuff, 0, sizeof(PLWbuff));
			int PLWlen;

			double PLtotWeight=0;
			for(int i=currentPayload;i<nPayloads;i++)
			{
				PLtotWeight+=payload[i].mass;
			}
			PLNamelen=sprintf_s(PLNamebuff,"Next Payload: %s",payload[currentPayload].name);
			skp->Text(w-400,0.25*h+20,PLNamebuff,PLNamelen);
			PLWlen=sprintf_s(PLWbuff,"Total Payload Weight: %.1f kg",PLtotWeight);
			skp->Text(w-400,0.25*h+40,PLWbuff,PLWlen);




	return true;
}
void Multistage2015::clbkPostCreation(){
	oapiWriteLog("Post Creation Started");
	UpdateLivePayloads();
	if(failureProbability>0){	wFailures=TRUE; FailuresEvaluation();}

	MyID=ConnectToOrbiterSoundDLL(GetHandle());

	if(Ssound.Main){
		ReplaceStockSound(MyID,Ssound.MAIN_THRUST,	REPLACE_MAIN_THRUST);}
	if(Ssound.Hover){
		ReplaceStockSound(MyID,Ssound.HOVER_THRUST,	REPLACE_HOVER_THRUST);}
	if(Ssound.RCS_ta){
		ReplaceStockSound(MyID,Ssound.RCS_THRUST_ATTACK, REPLACE_RCS_THRUST_ATTACK);}
	if(Ssound.RCS_ts){
		ReplaceStockSound(MyID,Ssound.RCS_THRUST_SUSTAIN, REPLACE_RCS_THRUST_SUSTAIN);}
	if(Ssound.Ac){
		ReplaceStockSound(MyID,Ssound.AIR_CONDITIONNING, REPLACE_AIR_CONDITIONNING);}
	if(Ssound.Ca1){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_1, REPLACE_COCKPIT_AMBIENCE_1);}
	if(Ssound.Ca2){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_2, REPLACE_COCKPIT_AMBIENCE_2);}
	if(Ssound.Ca3){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_3, REPLACE_COCKPIT_AMBIENCE_3);}
	if(Ssound.Ca4){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_4, REPLACE_COCKPIT_AMBIENCE_4);}
	if(Ssound.Ca5){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_5, REPLACE_COCKPIT_AMBIENCE_5);}
	if(Ssound.Ca6){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_6, REPLACE_COCKPIT_AMBIENCE_6);}
	if(Ssound.Ca7){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_7, REPLACE_COCKPIT_AMBIENCE_7);}
	if(Ssound.Ca8){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_8, REPLACE_COCKPIT_AMBIENCE_8);}
	if(Ssound.Ca9){
		ReplaceStockSound(MyID,Ssound.COCKPIT_AMBIENCE_9, REPLACE_COCKPIT_AMBIENCE_9);}

 	 for(int s=0;s<57;s++){
		 if(Ssound.GncStepSound[s]!=-5){
			 RequestLoadVesselWave(MyID,Ssound.GncStepSound[s],Gnc_step[Ssound.GncStepSound[s]].trchar,RADIO_SOUND);
		 }
		}





//mu=398600441800000;
//rt=6371010;
double planetmass=oapiGetMass(GetGravityRef());
mu=GGRAV*planetmass;//oapiGetMass(GetGravityRef());
rt=oapiGetSize(GetGravityRef());
g0=mu/(rt*rt);
//sprintf(logbuff,"g0=%.3f",g0);
//oapiWriteLog(logbuff);
CalculateAltSteps(planetmass);
	 //////COMPUTATIONAL GRAVITY TURN EVALUATION
	 if(Configuration==0)
	 {
	 double init_psi=21;
		while((!CGTE(init_psi*RAD))&&(init_psi>0))
		{	 init_psi-=1; }
	 double safety=init_psi*0.2;
	 init_psi-=safety;
	 if(init_psi<1){init_psi=1;}
	 //double gtip;
	 //GT_InitPitch=(90-init_psi)*RAD;
	 GT_IP_Calculated=(90-init_psi)*RAD;
	 sprintf(logbuff,"%s: Gravity Turn Initial Pitch by user: %.3f Calculated:%.3f",GetName(),GT_InitPitch*DEG,GT_IP_Calculated*DEG);
	 oapiWriteLog(logbuff);

	 if(GT_InitPitch==0) { GT_InitPitch=GT_IP_Calculated; }

	 }


	 InitPEG();

	 if(Complex)
	 {
		 EvaluateComplexFlight();
	 }

	 oapiWriteLog("Post Creation Terminated");
	return;
}
void Multistage2015::clbkVisualCreated (VISHANDLE _vis, int refcount)
{
  if (refcount > 1) return;
  vis = _vis;


}
void Multistage2015::clbkVisualDestroyed (VISHANDLE _vis, int refcount)
{
  if (vis == _vis) vis = NULL;
}

//Creation
//DLLCLBK void InitModule(HINSTANCE hModule){}
//DLLCLBK void ExitModule(HINSTANCE hModule){}
//DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel,int flightmodel){return new Multistage2015(hvessel,flightmodel);}
//DLLCLBK void ovcExit(VESSEL *vessel){if(vessel)delete(Multistage2015*)vessel;}

