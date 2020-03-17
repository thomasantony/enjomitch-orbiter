#include <windows.h>

#include "API_Form0.hpp"
#include <cmath>

double true_azimuth_deg,secondary_azimuth;
double azimuth,equator_v,equator_radius,target_radius;
double alt,des_inclination,base_latitude;
//int alt,base_latitude;
//double des_inclination;
double sidereal_day_t,mi;
double base_v[2],tgt_orbit_v[2],lnch_v[2],lnch_v_norm; //vectors
double mass;
const double pi=3.1415926535897931, G=6.67259e-11;
int planet;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{

 DialogBoxParam(hInstance,MAKEINTRESOURCE(IDC_TJVDESIGNPANEL),0,TJvDesignPanelDialogProc,0);

}

double calculate_azimuth (double des_inclination, double base_latitude, double alt) //double orbital_v <- old
{
	int i;
	double true_azimuth;
	azimuth= asin( cos(des_inclination*pi/180) / cos(base_latitude*pi/180));  // this equ doesn't take rotation into accout
	equator_v= equator_radius * (2*pi/sidereal_day_t);   //equator velocity
	base_v[1]= equator_v * cos (base_latitude*pi/180);   // base velocity vector item (east)
	target_radius=equator_radius+alt;
	mi=G*mass;  //gravitational parameter
	tgt_orbit_v[0]=sqrt(mi/target_radius)*cos(azimuth); // northern velocity
	tgt_orbit_v[1]=sqrt(mi/target_radius)*sin(azimuth); // eastern velocity
	for(i=0; i<2; i++) lnch_v[i]=tgt_orbit_v[i]-base_v[i]; // taking launch site rotation into accout
	lnch_v_norm=sqrt(lnch_v[0]*lnch_v[0] + lnch_v[1]*lnch_v[1]); // normalizing launch vector
#if (PL==0)
	//cout <<endl<<"Normalized launch vector = "<<lnch_v_norm<<" m/s"<<endl;
#else 
	//cout <<endl<<"Znormalizowany wektor startu = "<<lnch_v_norm<<" m/s"<<endl;
#endif
	if (lnch_v[0]==0) lnch_v[0]=0.01; //div by zero protection	
	true_azimuth= atan(lnch_v[1]/lnch_v[0]); // tan(azimuth) = eastern_vel / northern_vel
	//true_azimuth= atan( tan(azimuth) - base_v / (orbital_v * cos(azimuth) ) );  <-- old
	//return azimuth;
	return true_azimuth;
	
}


