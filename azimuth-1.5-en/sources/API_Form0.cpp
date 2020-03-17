#include <windows.h>


#include "API_Form0.hpp"
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <iostream>
#define PL 0 //localisation define. PL=1 means Polish version

using namespace std;



extern double azimuth,equator_v,equator_radius,target_radius;
extern double true_azimuth_deg,secondary_azimuth;
extern double alt,des_inclination,base_latitude;
//extern int alt,base_latitude;
//extern double des_inclination;
extern double sidereal_day_t,mi;
extern double base_v[2],tgt_orbit_v[2],lnch_v[2],lnch_v_norm; //vectors
extern double mass;
const double pi=3.1415926535897931, G=6.67259e-11;
extern int planet;
double inclination,latitude,atm_alt;

//{{Implementation}}
extern double calculate_azimuth (double, double , double ); //double orbital_v <- old

// ---------------------------------------
// TJvDesignPanel
// ~~~~~~~~~~~
//


//BEFORE_DIALOG_PROC


BOOL CALLBACK TJvDesignPanelDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//INSIDE_DIALOG_PROC
	static HWND hCtlEdit_Sid_Day;
	static HWND hCtlEdit_Equator_Rad;
	static HWND hCtlEdit_Mass;
	static HWND hCtlEdit_Output;
	static HWND hCtlQuit;
	static HWND hCtlCalculate;
	static HWND hCtlEdit_Orbit_Alt;
	static HWND hCtlEdit_Lat;
	static HWND hCtlEdit_Inclin;
	static HWND hCtlListBox_inclin;
	static HWND hCtlListBox_Lat;
	static HWND hCtlListBox_Body;
	static HWND hCtlCalculator;
	static HWND hCtlOrbit_alt;
	static HWND hCtlBody_And_Alt;
	static HWND hCtlLatitude;
	static HWND hCtlInclination_and_alt;
	static HWND hCtlSidereal_day;
	static HWND hCtlEquator_radius;
	static HWND hCtlMass;
	switch (uMsg)
	{
		case WM_CLOSE:
		{
//INSIDE_WM_CLOSE
	
			EndDialog(hWnd, IDCANCEL); // TJvDesignPanel
		//return TRUE;
			break;
		}
//INSIDE_SWITCH_MESSAGE

		case WM_INITDIALOG:
		{
//INSIDE_WM_INITDIALOG
			hCtlEdit_Sid_Day = GetDlgItem(hWnd, IDC_EDIT_SID_DAY); // Edit_Sid_Day
			SendMessage(hCtlEdit_Sid_Day, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"86164.091");
			
			hCtlEdit_Equator_Rad = GetDlgItem(hWnd, IDC_EDIT_EQUATOR_RAD); // Edit_Equator_Rad
			SendMessage(hCtlEdit_Equator_Rad, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"6.378165e6");
			
			hCtlEdit_Mass = GetDlgItem(hWnd, IDC_EDIT_MASS); // Edit_Mass
			SendMessage(hCtlEdit_Mass, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"5.973698968e+24");
			
			hCtlSidereal_day = GetDlgItem(hWnd, IDC_SIDEREAL_DAY); // Sidereal_day
			
			hCtlEquator_radius = GetDlgItem(hWnd, IDC_EQUATOR_RADIUS); // Equator_radius
			
			hCtlMass = GetDlgItem(hWnd, IDC_MASS); // Mass
			
			hCtlEdit_Output = GetDlgItem(hWnd, IDC_EDIT_OUTPUT); // Edit_Output
#if (PL==0)
			SendMessage(hCtlEdit_Output, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"This little-o appie calculates the true azimuth at which you must launch to have a desired inclination after reaching orbit Planets' rotaion is taken into account.\r\nYou can either input data manually or make use of the listboxes. The inclination can be ranging from 0 to 180. If you want to pick a target's inc from OrbitMFD you must switch it to Equatorial Frame (Shift+F or press FRM). Input your launch site latitude with precision of 0.1 or greater (from MapMFD or Scenario Editor > 'on the ground'). It can be set from -90 to 90. Next select body and final orbital altitude in [km] which should be set higher than body's atmoshpere cutoff alt as shown in the body listbox." );
#else 
			SendMessage(hCtlEdit_Output, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"Ta malutka aplikacja oblicza rzeczywisty azymut z ktorym musisz wystartowac aby osiagnac orbite o wybranej inklinacji. Rotacje planet sa wziete pod uwage.\r\nMozesz albo wprowadzac dane wlasnorecznie albo uzyc listboxow. Inklinacja moze zawierac sie w zakresie od 0 do 180. Jesli chcesz wybrac inklinacje celu z OrbitMFD, musisz przelaczyc go na tryb rownikowy (Shift+F lub przycisk FRM). Wprowadz szerokosc gegraficzna ladowiska z dokladnoscia co do jednej dziesiatej lub wiecej. (Wartosc z N lub S z MapMFD lub ze Scenario editora -> 'on the ground'). Moze byc ustawiona od -90 do 90. Nastepnie wybierz cialo i ostateczna wysokosc orbity w [km] ktora powinna byc ustawiona powyzej wysokosci odciecia atmosfery danego ciala, jak pokazano na listboxie ciala. " );
#endif
			hCtlQuit = GetDlgItem(hWnd, IDC_QUIT); // Quit
			
			hCtlCalculate = GetDlgItem(hWnd, IDC_CALCULATE); // Calculate
			
			hCtlEdit_Orbit_Alt = GetDlgItem(hWnd, IDC_EDIT_ORBIT_ALT); // Edit_Orbit_Alt

			SendMessage(hCtlEdit_Orbit_Alt, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"250");
			
			hCtlEdit_Lat = GetDlgItem(hWnd, IDC_EDIT_LAT); // Edit_Lat
		
			SendMessage(hCtlEdit_Lat, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"28.597");
			
			hCtlEdit_Inclin = GetDlgItem(hWnd, IDC_EDIT_INCLIN); // Edit_Inclin
	
			SendMessage(hCtlEdit_Inclin, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"51.57");
			
			hCtlListBox_inclin = GetDlgItem(hWnd, IDC_LISTBOX_INCLIN); // ListBox_inclin
			SendDlgItemMessage(hWnd, IDC_LISTBOX_INCLIN, LB_ADDSTRING, 0, (LPARAM)"ISS             i 51.57  a 362");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_INCLIN, LB_ADDSTRING, 0, (LPARAM)"Luna-OB1   i 91.54  a 500");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_INCLIN, LB_ADDSTRING, 0, (LPARAM)"Mars Relay i 26.82  a 310");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_INCLIN, LB_ADDSTRING, 0, (LPARAM)"Mir              i 26.94  a 299");
			
			
			
			hCtlListBox_Lat = GetDlgItem(hWnd, IDC_LISTBOX_LAT); // ListBox_Lat
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Baikonur 45.92");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Canaveral 28.597");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Edwards 34.5");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Habana 23.00");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Kourou 5.2");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Vandenberg 34.58");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"White Sands 32.5");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Brighton 41.125");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_LAT, LB_ADDSTRING, 0, (LPARAM)"Olympus 12.74");
			
			
			hCtlListBox_Body = GetDlgItem(hWnd, IDC_LISTBOX_BODY); // ListBox_Body
			SendDlgItemMessage(hWnd, IDC_LISTBOX_BODY, LB_ADDSTRING, 0, (LPARAM)"Earth 200 km");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_BODY, LB_ADDSTRING, 0, (LPARAM)"Moon");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_BODY, LB_ADDSTRING, 0, (LPARAM)"Mars 100 km");
			SendDlgItemMessage(hWnd, IDC_LISTBOX_BODY, LB_ADDSTRING, 0, (LPARAM)"Custom");
			
			
			hCtlCalculator = GetDlgItem(hWnd, IDC_CALCULATOR); // Calculator
			
			hCtlOrbit_alt = GetDlgItem(hWnd, IDC_ORBIT_ALT); // Orbit_alt
			
			hCtlBody_And_Alt = GetDlgItem(hWnd, IDC_BODY_AND_ALT); // Body
			
			hCtlLatitude = GetDlgItem(hWnd, IDC_LATITUDE); // Latitude
			
			hCtlInclination_and_alt = GetDlgItem(hWnd, IDC_INCLINATION_AND_ALT); // Inclination_and_alt
		}
		break; //WM_INITDIALOG

case WM_COMMAND:
	switch(LOWORD(wParam))
	{
		case IDC_LISTBOX_INCLIN:
                // It's our listbox, check the notification code
			switch(HIWORD(wParam))
			{
				case LBN_SELCHANGE:
                        // Selection changed, do stuff here.
					HWND hList = GetDlgItem(hWnd, IDC_LISTBOX_INCLIN);
					int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
					switch(index)
					{
						case (0): //ISS
							inclination=51.57;
							break;
						case (1): //Luna-OB1
							inclination=91.54;
							break;
						case (2): //Mars Relay
							inclination=26.82;
							break;	
						case (3): //Mir
							inclination=26.94;
							break;
					}
										
					
				char inclin[100];
			
				sprintf(inclin,"%0.2f",inclination);
				SetDlgItemText(hWnd, IDC_EDIT_INCLIN, inclin);
			break;
			}
		break;
		case IDC_LISTBOX_LAT:
                // It's our listbox, check the notification code
			switch(HIWORD(wParam))
			{
				case LBN_SELCHANGE:
                        // Selection changed, do stuff here.
					HWND hList = GetDlgItem(hWnd, IDC_LISTBOX_LAT);
					int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
					switch(index)
					{
						case (0): //Baikonur
							latitude=45.92;
						break;
						case (1): //Canaveral
							latitude=28.597;
							break;
						case (2): //Edwards
							latitude=34.5;
							break;	
						case (3): //Habana
							latitude=23.00;
						break;
						case (4): //Kourou 
							latitude=5.2;
							break;
						case (5): //Vandenberg 
							latitude=34.58;
						break;
						case (6): //White Sands 
							latitude=32.5;
						break;
						case (7): //Brighton 
							latitude=41.125;
							break;
						case (8): //Olympus  
							latitude=12.74;
							break;
						}	

					char lat[100];
			
					sprintf(lat,"%0.3f",latitude);
					SetDlgItemText(hWnd, IDC_EDIT_LAT, lat);
					break;
			}
			break;
		case IDC_LISTBOX_BODY:
                // It's our listbox, check the notification code
			switch(HIWORD(wParam))
			{
				case LBN_SELCHANGE:
                        // Selection changed, do stuff here.
					HWND hList = GetDlgItem(hWnd, IDC_LISTBOX_BODY);
					int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
					switch(index)
					{
						case (0): //Earth
							mass=5.973698968e+24;	
							equator_radius=6.378165e6;
							sidereal_day_t=86164.091;
							atm_alt=200;
							break;
							case (1): //Moon
								mass=7.347664e+22;
								equator_radius=1.738e6;
								sidereal_day_t=2360591.597;
								atm_alt=0;
								break;
								case (2): //Mars
									mass=6.418542e+23;
									equator_radius=3.38992e6;
									sidereal_day_t=88642.6632;
									atm_alt=100;
									break;
									case (3): //Custom
										mass=0;
										equator_radius=0;
										sidereal_day_t=0;
										atm_alt=100;
										atm_alt=0;
#if (PL==0)
										SendMessage(hCtlEdit_Output, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"You must enter your planet's physical parameters. All of them are available in the planet's configuration file in ..\\Orbiter\\Config\\.. \r\nAcceptable formats are eg. 6378165, 6.378165e6 or 6.378165e+6 Enter planet's Mass available under === Physical Parameters === (a bit of precision required here). \r\nEnter equator radius (\"Size\" entry).\r\nEnter planet's sidreal day, keeping it's sign if it's a retrograde planet (eg Venus) (\"SidRotPeriod\" entry under === Rotation Elements ===).\r\nAnd finally enter orbit's altitude in kilometers. See your planet's cutoff altitide parameter under === Atmospheric Parameters === (AtmAltLimit)" );
#else
SendMessage(hCtlEdit_Output, WM_SETTEXT, 0, (LPARAM) (LPCTSTR)"Musisz wprowadzic parametry fizyczne planety. Wszystkie one sa dostepne w pliku konfiguracyjnym planety w ..\\Orbiter\\Config\\..\r\nDopuszczalnymi formatami sa np: 6378165, 6.378165e6 lub 6.378165e+6.\r\nWprowadz mase planety dostepna pod === Physical Parameters ===  (wpis \"Mass\").\r\nWprowadz promien rownika (\"Size\" ).\r\nWprowadz okres obrotowy (wpis \"SidRotPeriod\" pod === Rotation Elements ===). Zachowaj znak okresu, jesli planeta obraca sie wstecznie (np Wenus).");
#endif
										break;
					}
					char params[100];
					sprintf(params,"%g",mass);
					SetDlgItemText(hWnd, IDC_EDIT_MASS, params);				
					sprintf(params,"%g",equator_radius);
					SetDlgItemText(hWnd, IDC_EDIT_EQUATOR_RAD, params);
					sprintf(params,"%g",sidereal_day_t);
					SetDlgItemText(hWnd, IDC_EDIT_SID_DAY, params);
					/*char inclin[100];
			
					sprintf(inclin,"%0.2f",inclination);
					SetDlgItemText(hWnd, IDC_EDIT_INCLIN, inclin);*/
					break;
			}
			break;
		case IDC_CALCULATE:
		{
			// When somebody clicks the Add button, first we get the number of
			// they entered

			BOOL bSuccess,too_big;
			int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_INCLIN));
			if(len > 0)
			{
				int i;
				char* buf;

				buf = (char*)GlobalAlloc(GPTR, len + 1);
				GetDlgItemText(hWnd, IDC_EDIT_INCLIN, buf, len + 1);
				des_inclination=atof(buf);
				if (des_inclination!=0 || buf[0]=='0') bSuccess=true; else bSuccess=false;
				if ((des_inclination>180) || (des_inclination<0)) 
				{
					too_big=true;
#if (PL==0)
					MessageBox(hWnd, "Inclination must be between 0 and 180 degrees!", "Warning", MB_OK);
#else
MessageBox(hWnd, "Inklinacja musi sie zawierac pomiedzy 0 a 180 stopniami!", "Ostrzezenie", MB_OK);
#endif
				}
				else too_big=false;

				GlobalFree((HANDLE)buf);
			}
			if((bSuccess) && (too_big==0)) 
			{

				BOOL bSuccess2,too_big2;
				int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_LAT));
				if(len > 0)
				{
					int i;
					char* buf;

					buf = (char*)GlobalAlloc(GPTR, len + 1);
					GetDlgItemText(hWnd, IDC_EDIT_LAT, buf, len + 1);
					base_latitude=atof(buf);
					if ((base_latitude!=0) || buf[0]=='0' ) bSuccess2=true; else bSuccess2=false;
					if ((base_latitude>90) || (base_latitude<-90)) 
					{
						too_big2=true;
#if (PL==0)
						MessageBox(hWnd, "Latitude must be between -90 and 90 degrees!", "Warning", MB_OK);
#else 
					MessageBox(hWnd, "Szerokosc gogr. powinna zawierac sie w przedziale od -90 do 90 stopni!", "Ostrzezenie", MB_OK);
#endif
					}
					else too_big2=false;

					GlobalFree((HANDLE)buf);
			}
			if((bSuccess2) && (too_big2==false))
				{
				if (cos(base_latitude*pi/180)==0) base_latitude=base_latitude+0.001; //div by zero protection		
				//if ( fabs(cos(des_inclination*pi/180) / cos(base_latitude*pi/180)) > 1)
				bool lesser=false;
				if ((fabs(base_latitude)>des_inclination) || (fabs(base_latitude)>180-des_inclination))
					{
						if (des_inclination<=90)
						{
#if (PL==0)
							char lesser_char[]="There is no way for this equation to help you if you choose inclination lesser than your latitude. If you want to have this inclination anyway, launch with azimuth = 90 when Tn of your target in Align MFD is minimal, and make a plane change at orbits' intersection (during orbit insertion or in orbit)";
#else
char lesser_char[]="Te rownania w zaden sposob Ci nie pomoga, jesli wybierzesz inklinacje mniejsza niz szerokosc geogr. Jesli chcesz miec ta inklinacje mimo wszystko, wystartuj z azymutem = 90 gdy Tn Twojego celu na Align MFD bedzie minimalny, i wykonaj wyrownanie orbit w ich przecieciu (podczas wchodzenia na orbite lub na orbicie).";
#endif
						SetDlgItemText(hWnd, IDC_EDIT_OUTPUT,lesser_char);
						}
						if (des_inclination>90)
						{	
							char base_latitude_char[200];
							char glue_1[100];
							char base_latitude_char_180[100];
#if (PL==0)
							char lesser_char[]="There is no way for this equation to help you if you choose inclination lesser than your latitude. To know your highest possible retrograde inclination, substract your latitude from 180 i.e. your maximal retrograde inclination = 180 - ";
#else
char lesser_char[]="Te rownania w zaden sposob Ci nie pomoga, jesli wybierzesz inklinacje mniejsza niz szerokosc geogr. Aby poznac najwieksza mozliwa inklinacje wsteczna, odejmij swoja szerokosc grg od 180 tzn twoja maksymalna inklinacja wsteczna = 180 - ";
#endif
							char equal[]=" = ";
#if (PL==0)
							char retro[]=". \r\nIf you want to have this inclination anyway, launch with azimuth = 270 when Tn of your target in Align MFD is minimal, and make a plane change at orbits' intersection (during launch or in orbit)";
#else
char retro[]=". \r\nJesli chcesz miec ta inklinacje mimo wszystko,wystartuj z azymutem = 270 dla gdy Tn Twojego celu na Align MFD bedzie minimalny, i wykonaj wyrownanie orbit w ich przecieciu (podczas wchodzenia na orbite lub na orbicie).";
#endif
							bool negative_lat=false;
							if (base_latitude<0)
							{
								negative_lat=true;
								sprintf(base_latitude_char, "%0.3f",fabs(base_latitude));
							}
							else
								sprintf(base_latitude_char, "%0.3f",base_latitude);
							if (negative_lat)
								sprintf(base_latitude_char_180, "%0.3f",(180-fabs(base_latitude)));
							else sprintf(base_latitude_char_180, "%0.3f",180-base_latitude);
							strcat(lesser_char,base_latitude_char);
							strcat(lesser_char,equal);
							strcat(lesser_char,base_latitude_char_180);
							strcat(lesser_char,retro);							
							SetDlgItemText(hWnd, IDC_EDIT_OUTPUT, lesser_char);
						}

					}

				else
				{
					BOOL bSuccess3;	
					alt = GetDlgItemInt(hWnd, IDC_EDIT_ORBIT_ALT, &bSuccess3, FALSE);
					int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_ORBIT_ALT));
					if(len > 0)
					{
						int i;
						char* buf;

						buf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemText(hWnd,  IDC_EDIT_ORBIT_ALT, buf, len + 1);
						alt=atof(buf);
						if (alt>0 || buf[0]=='0') bSuccess3=true; else bSuccess3=false;

						GlobalFree((HANDLE)buf);
					}
					if(bSuccess3) 
					{
						BOOL bSuccess4;	
						int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_MASS));
						if(len > 0)
						{
							int i;
							char* buf;

							buf = (char*)GlobalAlloc(GPTR, len + 1);
							GetDlgItemText(hWnd,  IDC_EDIT_MASS, buf, len + 1);
							mass=atof(buf);
							if (mass>0) bSuccess4=true; else bSuccess4=false;

							GlobalFree((HANDLE)buf);
						}
						BOOL bSuccess5;	
						len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_EQUATOR_RAD));
						if(len > 0)
						{
							int i;
							char* buf;

							buf = (char*)GlobalAlloc(GPTR, len + 1);
							GetDlgItemText(hWnd,  IDC_EDIT_EQUATOR_RAD, buf, len + 1);
							equator_radius=atof(buf);
							if (equator_radius>0) bSuccess5=true; else bSuccess5=false;

							GlobalFree((HANDLE)buf);
						}
						BOOL bSuccess6;	
						len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_SID_DAY));
						if(len > 0)
						{
							int i;
							char* buf;

							buf = (char*)GlobalAlloc(GPTR, len + 1);
							GetDlgItemText(hWnd,  IDC_EDIT_SID_DAY, buf, len + 1);
							sidereal_day_t=atof(buf);
							if (sidereal_day_t!=0) bSuccess6=true; else bSuccess6=false;

							GlobalFree((HANDLE)buf);
						}
						if (alt<atm_alt)
#if (PL==0) 
							MessageBox(hWnd, "Your final orbit altitude is set in body's atmosphere!", "Warning", MB_OK);
#else
	MessageBox(hWnd, "Twoja ostateczna wysokosc orbity jest ustawiona w atmosferze ciala!", "Ostrzezenie", MB_OK);
#endif
						alt=alt*1000;
						if (bSuccess4 && bSuccess5 && bSuccess6)
						{
						true_azimuth_deg=calculate_azimuth(des_inclination,base_latitude,alt)*180/pi; // actual algorithm function call with given parameters
						if (true_azimuth_deg<0) true_azimuth_deg=360+true_azimuth_deg; // quadrant checking logic
						if (true_azimuth_deg>=270) secondary_azimuth=180+(360-true_azimuth_deg); // specific descending azimuth for retrograde orbits calculation
						else secondary_azimuth=(180-true_azimuth_deg);                        //standard case
						char result[100];
						sprintf(result,"%0.2f",lnch_v_norm);
#if (PL==0)
						char launch[]="Normalized launch vector = ";
						strcat(launch,result);
						strcat(launch," m/s\r\n\r\n");
						strcat(launch,"Launch with azimuth = ");
						sprintf(result,"%0.4f",true_azimuth_deg);
						strcat(launch,result);
						strcat(launch," (ascending), or ");
						sprintf(result,"%0.4f", secondary_azimuth);
						strcat(launch,result);
						strcat(launch, " (descending) degrees.");
#else
char launch[]="Znormalizowany wektor startu = ";

						strcat(launch,result);
						strcat(launch," m/s\r\n\r\n");
						strcat(launch,"Wystartuj z azymutem = ");
						sprintf(result,"%0.4f",true_azimuth_deg);
						strcat(launch,result);
						strcat(launch," (wstepujacy), lub ");
						sprintf(result,"%0.4f", secondary_azimuth);
						strcat(launch,result);
						strcat(launch, " (zstepujacy) stopnia.");
#endif						 
						SetDlgItemText(hWnd,IDC_EDIT_OUTPUT, launch);
						}
						else 
#if (PL==0)
							MessageBox(hWnd, "You didn't enter one or more body's parameters!", "Warning", MB_OK);


					}
					else MessageBox(hWnd, "You didn't enter altitude!", "Warning", MB_OK);
				//}
				}			
				}
				else 
				{
					if (too_big2==false)
					MessageBox(hWnd, "You didn't enter latitude!", "Warning", MB_OK);
			}
			
			}
			else 
			{
				if (too_big==false)
				MessageBox(hWnd, "You didn't enter inclination!", "Warning", MB_OK);
#else				
				MessageBox(hWnd, "Nie wprowadziles jednego lub wiecej parametrow ciala!", "Ostrzezenie", MB_OK);
				
			}
			else MessageBox(hWnd, "Nie wprowadziles wysokosci!", "Ostrzezenie", MB_OK);
				//}
		}			
	}
			else 
			{
		if (too_big2==false)
			MessageBox(hWnd, "Nie wprowadziles szerokosci geograficznej!", "Ostrzezenie", MB_OK);
			}
			
	}
		else 
		{
		if (too_big==false)
			MessageBox(hWnd, "Nie wprowadziles inklinacji!", "Ostrzezenie", MB_OK);
#endif
		}
		break;
		}

	 //WM_CLOSE
	case IDC_QUIT:
	{
		EndDialog(hWnd, IDCANCEL); // TJvDesignPanel
	}
		break;

		
};

}; // switch (uMsg)
//OUT_OF_SWITCH_MESSAGE
	return FALSE;

}; // DIALOG_PROC
//OUT_OF_DIALOG_PROC

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"A Bitmap Program",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

