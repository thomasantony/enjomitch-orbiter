// ==============================================================
//               ORBITER MODULE: OrbitraderMFD
//                  Part of the ORBITER SDK
// 	 Copyright (C) 2007         Simon Ender "Enjo"         - author and maintainer  ender-sz !AT! go2 pl
//	 Copyright (C) 2008	 Mohd "Computerex" Ali  - contributor
//                   All rights reserved
//
// OrbitraderMFD.cpp
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================================================


#define STRICT
#define ORBITER_MODULE
#include "windows.h"
#include <vector>
#include <numeric>
#include "OrbitraderMFD.hpp"
#include "MFDData.hpp"
#include "consts.hpp"
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <exception>
#include <pthread.h>
//#define MY_ENCODING "ISO-8859-1"
#define MY_ENCODING "UTF-8"
using namespace std;

std::vector<MFDData*> v_MFDdata; // Create a vector that holds pointers to MFDData objects
std::vector<TradingObject*> Tradingobject; // and to base/stations with its prices objects


double MFDData::money = 1000;
double MFDData::moneyDiff = 0;

const unsigned int nr_of_menu_items = 8;
const unsigned int nr_of_hidden_menu_items = 2;
const unsigned int nr_of_main_menu_items = nr_of_menu_items - nr_of_hidden_menu_items;

const unsigned int TRANSFER_MNU = 0;
const unsigned int PORT_SEL_MNU = 1;
const unsigned int CHECK_PLANETS_MNU = 2;
const unsigned int FUEL_MNU = 3;
const unsigned int CONFIG_MNU = 4;
const unsigned int ACCOUNT_MNU = 5;
const unsigned int BASES_LIST_MNU = nr_of_main_menu_items;
const unsigned int BASE_PRICES_MNU = nr_of_main_menu_items + 1;

const unsigned int OrbitraderMFD::nr_of_trading_items = 4;
const unsigned int OrbitraderMFD::nr_of_config_items = 2;
const unsigned int OrbitraderMFD::nr_of_account_items = 4;
const unsigned int OrbitraderMFD::amount_of_buttons = 6;
const unsigned int OrbitraderMFD::maximum_load = 200;
const unsigned int OrbitraderMFD::MFD_LINE_NR = 24;
const unsigned int OrbitraderMFD::MFD_CHAR_LEN = 35;

double dt = 2;
double prevT = -2;
/*
// transfer codes
const unsigned int GET_BASES_INDEX = 1;
const unsigned int GET_REMOTE_BASE = 2;
const unsigned int GET_LOCAL_BASE = 3;
const unsigned int GET_PAYLOAD = 4;
const unsigned int GET_STATUS = 5;
const unsigned int GET_VALIDATE = 6;
const unsigned int GET_RESTORE = 7;
const unsigned int POST_LOGIN = 10;
const unsigned int POST_TRANSFER = 11;
const unsigned int POST_FUEL_ORDER = 12;
const unsigned int POST_STATUS_LANDED = 13;
const unsigned int POST_STATUS_LAUNCHED = 14;
const unsigned int POST_STATUS_LEFT_ORBIT = 15;
const unsigned int POST_STATUS_ENTERED_ORBIT = 16;
const unsigned int POST_STATUS_SLINGED = 17;
const unsigned int POST_STATUS_POWER_SLINGED = 18;

// orbital statuses
const unsigned int STATUS_LEFT_ORBIT = 1;
const unsigned int STATUS_ENTERED_ORBIT = 2;
const unsigned int STATUS_SLINGED = 3;
const unsigned int STATUS_POWER_SLINGED = 4;
*/
enum xferCodes {
	GET_BASES_INDEX=1, 
	GET_REMOTE_BASE,
	GET_LOCAL_BASE,
	GET_PAYLOAD,
	GET_STATUS,
	GET_VALIDATE,
	GET_RESTORE,
	POST_LOGIN,
	POST_TRANSFER,
	POST_FUEL_ORDER,
	POST_STATUS_LANDED,
	POST_STATUS_LAUNCHED,
	POST_STATUS_LEFT_ORBIT,
	POST_STATUS_ENTERED_ORBIT,
	POST_STATUS_SLINGED,
	POST_STATUS_POWER_SLINGED
};

enum orbStatuses {
	STATUS_LEFT_ORBIT=1,
	//STATUS_REACHED_ORBIT,
	STATUS_ENTERED_ORBIT,
	STATUS_SLINGED,
	STATUS_POWER_SLINGED
};

//error codes
const unsigned int LOGIN_INCORRECT_ERR = 1;
const unsigned int INVALID_SHIP_ERR = 2;


// menu items' codes. They must be in a proper order, namely the main menu items are first
// and all the other ones are marked as "hidden" (hidden in main menu)
// and start after the last main menu item

CURL *curl;

static bool clbkUsernameDialog(void *id, char *str, void *usrdata);
static bool clbkPasswordDialog(void *id, char *str, void *usrdata);
MFDData * associateMFDData(VESSEL * vessel);
int g_MFDmode;
static float GETdeltaTime = 0.1f;
static float POSTdeltaTime = 0.1f;
static bool configRead=false;

static bool ready = true;
int error_code = 0;
int xfer_code = 0;

std::string username;
std::string password;

// CURLM *multi_handle; // not using it anymore
static FILE * gf = NULL, * xmlTest = NULL;
std::ofstream fdb;

// ==============================================================
// API interface

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "OrbitraderMFD";   // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_4;                // MFD mode selection key
	spec.msgproc = OrbitraderMFD::MsgProc;  // MFD mode callback function

	//item = new int[nr_of_trading_items]; 	memset (item,  0, nr_of_trading_items*sizeof(int));
	gf = fopen ("bledy.txt","w");
	//	fdb = new std::ofstream();
	fdb.open ("OT-debug.txt");
	DB("Debug session started (InitModule)");

	LIBXML_TEST_VERSION

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	//multi_handle = curl_multi_init();	// not using it anymore

	xmlTest = fopen("test-xml.txt","w");

	if (xmlTest) fclose(xmlTest);	
	xmlTest = fopen("test-xml2.txt","w");
	if (xmlTest) fclose(xmlTest);
	
	//chunk.memory = NULL;
	//chunk.size = 0; 

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	//DB("opcDLLExit");
	// Unregister the custom MFD mode when the module is unloaded
	int i = 0;
	for (i = 0; i < v_MFDdata.size(); i++) 
		if (v_MFDdata.at(i))  delete v_MFDdata.at(i);
	for (i = 0; i < Tradingobject.size(); i++)
		if (Tradingobject[i]) delete Tradingobject[i];

	/*for (i = 0; i < v_bases.size(); i++) 
	if (v_bases[i]) delete v_bases[i];

	for (i = 0; i < v_planets.size(); i++) 
	if (v_planets[i]) delete v_planets[i];

	for (i = 0; i < v_xmlDataExtractor.size(); i++) 
	if (v_xmlDataExtractor[i]) delete v_xmlDataExtractor[i];

	for (i = 0; i < data->v_remoteProductName.size(); i++) 
	if (data->v_remoteProductName[i]) delete data->v_remoteProductName[i];

	for (i = 0; i < data->v_localProductName.size(); i++) 
	if (data->v_localProductName[i]) delete data->v_localProductName[i];
	*/
	// curl_multi_cleanup(multi_handle);  // not using it anymore
	curl_easy_cleanup(curl);
	if (gf) fclose(gf); 
	if (fdb.is_open()) fdb.close();
	oapiUnregisterMFDMode (g_MFDmode);
}

DLLCLBK void opcOpenRenderViewport (HWND renderWnd, DWORD width, DWORD height, BOOL fullscreen) 
{
	
	DB("Debug session started (opcOpenRenderViewport)");
}

DLLCLBK void opcCloseRenderViewport (void)
{
	//
}

DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	int i,j;
	
	for (j=0; j < oapiGetVesselCount(); j++)
	{
		OBJHANDLE hVessel = oapiGetVesselByIndex(j);
		VESSEL * vessel = oapiGetVesselInterface (hVessel);
		associateMFDData(vessel);
	}
	
	std::vector<MFDData*>::iterator it = v_MFDdata.begin() + 1;
	for (int i = 0; i < v_MFDdata.size(); i++)
    {
        it = v_MFDdata.begin() + i;
        if (!oapiIsVessel(v_MFDdata.at(i)->vessel)) // if vessel does not exist, delete the vector
        {
            delete v_MFDdata.at(i);
            v_MFDdata.erase(it);
        }
    }
	
	//gi = v_MFDdata.size();
	
	
	for (i=0; i < v_MFDdata.size(); i++)
	{
	try {
		MFDData* data = v_MFDdata.at(i);
		if(!data->vessel) 
		{
		// TODO :  Here we should delete this entry from the vector (?) and clear transfer data, and reset codes.
			gi = 15;
			continue;
		}	
		
		data->vs2.version = 2;
		// We'll be ckecking for dock information and fuel information (later)
		data->vs2.flag = VS_DOCKINFOLIST | VS_FUELLIST;
		// set to NULL to allow Orbiter to allocate needed memory. We'll freeing it at the end of this callback
		data->vs2.fuel = NULL;
		data->vs2.dockinfo = NULL;
		data->vessel->GetStatusEx(&data->vs2);
			
		if (thready)
		{
		if (!logged && !username.empty() && !password.empty())
		{
			data->POSTLogin();
		}
		if (logged && !data->validationDone)
		{
			data->GETValidate();
		}
		if (data->shipValid && !data->stateRestored)
		{
			//data->GETRestore();
			data->stateRestored = true; 		 // NEVER use together ! It's just a debug instruction
		}	
		//if (logged && data->shipValid)		
		//if (data->shipValid)
		if (data->stateRestored)
		{				
			VECTOR3 rel_pos;
			
			// get relative position of our ship with a closest base, by her handle			
			if (data->vs2.base)	data->vessel->GetRelativePos(data->vs2.base, rel_pos);
			// and calculate straight line distance. We have to be 15 km from the center of the base and be landed
			if (data->vs2.base && data->vs2.status && sqrt(rel_pos.x*rel_pos.x + 
			rel_pos.y*rel_pos.y + rel_pos.z*rel_pos.z) < 15e3) 
				data->landedNearBase = true;
			
			data->hSBody = data->vessel->GetSurfaceRef();
			data->hGBody = data->vessel->GetGravityRef(); // Get gravity dominant body
			
			if (!data->vs2.status)  // if in flight
			{
				data->moneyDiff = 0;
				data->m_fuelOrder.clear();
				
				data->CheckDockStatus();
				
				switch(data->CheckOrbitStatus())
				{
					case STATUS_LEFT_ORBIT:
						data->leavingOrbitSent = false;
					break;
					case STATUS_ENTERED_ORBIT:
						data->enteringOrbitSent = false;
					break;
					case STATUS_SLINGED:
						data->slingshotSent = false;
					break;
					case STATUS_POWER_SLINGED:
						data->powerSlingshotSent = false;
					break;
					default:
					break;
				}	
				 // oapiGetSysTime () > x is a workaround of Orbiter limitation - at first the VESSELSTATUS2.status is switching
			//if (data->prevStatus && oapiGetSysTime () > 5 ) 	
			//	data->statusLaunchedSent = false;
			}	// if landed
			else if (!data->prevStatus && oapiGetSysTime () > 5 ) 	
				data->statusLandedSent = false;
			
			if (data->hSBody && data->hGBody) {
				if (!data->leavingOrbitSent)	
					data->POSTStatus(POST_STATUS_LEFT_ORBIT , &data->leavingOrbitSent);
				if (!data->enteringOrbitSent)	
					data->POSTStatus(POST_STATUS_ENTERED_ORBIT , &data->enteringOrbitSent);
				if (!data->slingshotSent)			
					data->POSTStatus(POST_STATUS_SLINGED , &data->slingshotSent);	
				if (!data->powerSlingshotSent) 	
					data->POSTStatus(POST_STATUS_POWER_SLINGED , &data->powerSlingshotSent);
				if (!data->statusLandedSent)	
					data->POSTStatus(POST_STATUS_LANDED, &data->statusLandedSent);	
				if (!data->statusLaunchedSent) 
					data->POSTStatus(POST_STATUS_LAUNCHED, &data->statusLaunchedSent);
			}
			
			//if (oapiGetSysTime() - prevT > dt)
			{
				data->RefuelBlocker();	
				prevT = oapiGetSysTime(); 
			}
			
			// Thou shalt always check thine pointers
			if (data->vs2.base && data->hSBody)
			{
				if (data->landedNearBase && !data->localBasePricesRead)	{
					data->GETLocalPrices();
				}
				
				if (data->POSTtransferRequest)	{
					data->POSTTransferRequest();
				}
				if (data->POSTfuelRequest)	{
					data->POSTFuelOrder();
				}		
			}
				
				
			if (!data->payloadRead)
			{
				data->GETPayload();
			}	
			if (!basesIndexRead)
			{
				data->GETBasesIndex();
			}
			if (!data->remoteBasePricesRead)
			{
				data->GETRemotePrices();
			}			
			else
			{
				//CheckFuelMassIncrease();
				//RefuelBlocker();
			}
			
			// get relative position of our ship with a closest base, by her handle			
			if (data->vs2.base)	data->vessel->GetRelativePos(data->vs2.base, rel_pos);
			// and calculate straight line distance. We have to be 15 km from the center of the base and be landed
				if (data->vs2.base && data->vs2.status && sqrt(rel_pos.x*rel_pos.x + 
				rel_pos.y*rel_pos.y + rel_pos.z*rel_pos.z) < 15e3) 
					data->landedNearBase = true;
				else data->landedNearBase = data->localBasePricesRead = false;
			if (data->vs2.fuel != NULL) delete data->vs2.fuel;
			if (data->vs2.dockinfo != NULL) delete data->vs2.dockinfo;				
		}
		}
	}
	catch (exception & e)
	{
		DBe("opcPreStep",e);
	}
	}
	
}

//DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd) // call every timestep

// ==============================================================
// MFD class implementation

// Constructor
OrbitraderMFD::OrbitraderMFD(DWORD w, DWORD h, VESSEL *vessel) : MFD(w, h, vessel)
{	
	DB("OrbitraderMFD constructor");
	
	this->data = associateMFDData(vessel);


	
	propellantCount = vessel->GetPropellantCount();
	



	GET_still_running = 1;
	GET_finished = 0;
	GET_started = 0;

	POST_still_running = 1;
	POST_finished = 0;
	POST_started = 0; 

	

	memset(buf,'\0',sizeof(buf));
	memset(buf2,'\0',sizeof(buf2));


	//chunk.memory = NULL;
	//chunk.size = 0; 

	
	hSun = NULL;
	hLocalBody = NULL;
	/* 
	price = new float[nr_of_trading_items]; 
	for (int i=0; i<nr_of_trading_items; i++) {   
	price[i] = (nr_of_trading_items - i) * 10 ; // some dumb coding of initial price
	}
	*/ 
	std::string line;
	std::ifstream accountReader;
	accountReader.open("Config/Orbitrader/account.txt");
	if (accountReader.is_open())
	{
		while (! accountReader.eof() )
		{
			getline (accountReader,line);
			if (!line.compare(0,9,"username:") && line.size() > 10)	
				username.assign(line,10, line.size()-10);
			if (!line.compare(0,9,"password:") && line.size() > 10)	
				password.assign(line,10, line.size()-10);
		}
		accountReader.close();
	}
	

	// reading an offline database for singleplayer version
	if (!configRead)
	{
		FILE * f = fopen("Config/OrbitraderDB.cfg","rt");

		if (f)
		{	
			bool bodyFound=false, baseFound=false, priceFound=false;
			char buffer[512];

			OBJHANDLE hgbody=NULL, hbase=NULL;
			while (!feof(f)) 		// a loop for reading
			{
				memset(buffer, '\0', sizeof(buffer));
				fgets (buffer,512,f); 			// read next line
				buffer[strlen(buffer)-1] = 0; 	// terminating a string which has a new line char at the end

				if (!strncmp(buffer,"END_BASES",9))  // if found base 
					break;
				else if (!strncmp(buffer,"BODY_NAME",9)) {
					// get body handle. buffer+10 because first 10 chars are "BODY_NAME "
					hgbody = oapiGetGbodyByName(buffer+10);		
				}
				else if (hgbody && !strncmp(buffer,"BASE_NAME",9)) {	 // if handle is valid and base entry is found
					hbase = oapiGetBaseByName(hgbody, buffer+10);		 // get base handle
				}
				else if (hbase && !strncmp(buffer,"PRICES",6)) {		// if base handle is valid and PRICES entry found
					// a temporary variable to be passed as an argument to TradingObject (a base) constructor
					float prices[nr_of_trading_items];

					// A SENSITIVE LINE!! We can have a varying number of articles. A check is performed
					if (sscanf (buffer+7, "%f%f%f%f", &prices[0], &prices[1], &prices[2], &prices[3]) == nr_of_trading_items)   
					{

						// Create a pointer to a new object with given parameters
						TradingObject * dat = new TradingObject(prices,nr_of_trading_items);
						dat->hCurrentBase=hbase; 		// pass the base handle just read to that Object 
						Tradingobject.push_back(dat); 	// Add that pointer to a vector
						// tell this MFD's instance that it will be exchanging with this base (may be wrong. A scan is performed in Update anyway)
						this->exchangeBase=dat;			
						//this->exchangeBase->pPrices = new float[nr_of_trading_items];
					}		
				}	

			} // file reading loop
			fclose(f);


		}
		configRead = true;		// don't read again (configRead is a global var)
	}  



	mHeight = h; 	// store constructor's info about MFD's height and width in class' variables
	mWidth = w;
	linespacing = mHeight / MFD_LINE_NR;
	charwidth = mWidth / MFD_CHAR_LEN;

	// store constructor's info about a vessel in which the MFD was enabled in class' variables
	m_vessel = vessel;

	DB("Constructor code end");		
}

OrbitraderMFD::~OrbitraderMFD ()
{
	DB("OrbitraderMFD destructor");
	//if (data->portUsed != NULL)  delete [] data->portUsed;
	//POST_data.clear();
	//if(chunk.memory)   free(chunk.memory);
	//chunk.memory = NULL;
	//chunk.size = 0;
	//xfer_code = 0;
	DB("destructor code end");
}


// Define reactions on control keys. Called when L/R Shift + other key is pressed
bool OrbitraderMFD::ConsumeKeyBuffered(DWORD key)
{
	DB("OrbitraderMFD::ConsumeKeyBuffered");
	int i = 0;
	// FIXME: check pointers
	switch (key)
	{
	case OAPI_KEY_EQUALS:		// + buying / taking cargo, taking passenger

/*	v_payloadName;
	
v_payloadId;
	localBasePricesRead;
	data->v_localProductName;
data->v_localProductPrice;
data->v_localProductId;
*/
		switch (data->menu_nr)
		{
		case TRANSFER_MNU:
			if (data->landedNearBase && data->localBasePricesRead && data->payloadRead && data->item_index < nr_of_trading_items2)
			{
				data->sel_unit_id = data->v_localProductId.at(data->item_index);
				// increase item's amount pointed by item_index by multiplier
				data->m_order[data->sel_unit_id] += data->multiplier;
				data->money -= data->multiplier * data->v_localProductPrice.at(data->item_index);
				
				if (data->money < 0)
				{
					data->m_order[data->sel_unit_id] -= data->multiplier;
					data->money += data->multiplier * data->v_localProductPrice.at(data->item_index);
				}
				/*for (i = 0; i < data->v_payloadId.size(); i++)
				{
					if (data->v_localProductId.at(data->item_index) == data->v_payloadId.at(i))
					{
						data->v_payloadAmount.at(i) += data->multiplier;
					}
				}*/
				// decrease money by multiplier (amount of bought items) * price 
				//in a contacted base of a given item pointed by item_index
				//data->money -= data->multiplier * exchangeBase->pPrices[data->item_index];
				// if we're overloaded or ran out of cash, roll everything back
				/*if (VesselsLoad(data->item) > maximum_load || data->money < 0)
				{
					data->item[data->item_index] -= data->multiplier;
					data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
				}*/
				break;
			} 
			/*else if (data->portUsed[data->selectedPort])
			{
				// add some cargo to our ship
				data->cargoLoad += data->multiplier;
				data->item[data->item_index] += data->multiplier;
				// and decrease the same amount from the ship or station that we're data->docked with
				data->exchangePartner->item[data->item_index] -= data->multiplier;
				//data->money -= data->multiplier * exchangePartner->pPrices[data->item_index];  
				// TODO: Add spport for trading with orbital stations
				// if we're overloaded or our partner's supplies have been depleted
				// or we're bankrupt, roll everythng back
				if (VesselsLoad(data->item) > maximum_load || data->exchangePartner->item[data->item_index] < 0 || data->money < 0)
				{
					data->item[data->item_index] -= data->multiplier;
					data->exchangePartner->item[data->item_index] += data->multiplier;
					//data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
				}
				break;
			}*/


			break;
			// FIXME freaks out when RCS ordered to be tanked to full as well as mains but you haven't got enough funds for the latter
		case FUEL_MNU:
			if (data->landedNearBase && data->localBasePricesRead && data->item_index < propellantCount)
			{
			
				// increase item's amount pointed by item_index by multiplier
				precentageRatio = data->v_fuelMaxMass.at(data->item_index) * data->multiplier /100;
				double maxPossibleFuelOrder = data->v_fuelMaxMass.at(data->item_index) - data->v_fuelMass.at(data->item_index);
				double maxAffordableFuelOrder = 0;
				// this is money which you'd pay for fuel of the currently selected tank (or something like that :P )
				double currOrderValue = data->m_fuelOrder[data->item_index] / 10;
				
				if (maxPossibleFuelOrder / 10 > data->money + data->moneyDiff + currOrderValue)	
				{
					maxAffordableFuelOrder = (data->money + data->moneyDiff + currOrderValue) * 10;
				}

				// if you want to order more than you can hold
				if (data->m_fuelOrder[data->item_index] + precentageRatio >
				//if (data->m_fuelOrder[data->item_index] >
					maxPossibleFuelOrder)// && data->money + data->moneyDiff > 0)
				{		
					// if you want to order more than you can afford
					if (maxAffordableFuelOrder && data->m_fuelOrder[data->item_index] 
							+ precentageRatio > maxAffordableFuelOrder)
					{
						//sprintf(oapiDebugString(),"1, %f", maxAffordableFuelOrder);
						// remove the difference of the entire previous order
						data->moneyDiff += data->m_fuelOrder[data->item_index] / 10;
						// order as much as you can afford
						data->m_fuelOrder[data->item_index] = maxAffordableFuelOrder;
						data->moneyDiff -= data->m_fuelOrder[data->item_index] / 10;
					}
					else
					{
						if (data->money + data->moneyDiff > 0)
						{
						//sprintf(oapiDebugString(),"2, %f", maxAffordableFuelOrder);
						// remove the difference of the entire previous order
						data->moneyDiff += data->m_fuelOrder[data->item_index] / 10;
						// order so much that you have 100 %						
						data->m_fuelOrder[data->item_index] = maxPossibleFuelOrder;
						data->moneyDiff -= data->m_fuelOrder[data->item_index] / 10;
						/*
							sprintf(oapiDebugString(),"3, %f", maxAffordableFuelOrder);
							data->moneyDiff -= data->m_fuelOrder[data->item_index] / 10;
							//data->m_fuelOrder[data->item_index] -= maxPossibleFuelOrder;
							data->m_fuelOrder[data->item_index] += data->moneyDiff * 10;
						*/
						}
						
					}
				} 
				else 
				if (data->money + data->moneyDiff > 0)
				{
					//sprintf(oapiDebugString(),"4, %f", maxAffordableFuelOrder);
					data->m_fuelOrder[data->item_index] += precentageRatio;
					data->moneyDiff -= precentageRatio / 10;
					if (maxAffordableFuelOrder && data->m_fuelOrder[data->item_index] 
							+ precentageRatio > maxAffordableFuelOrder)
					{
						//sprintf(oapiDebugString(),"1, %f", maxAffordableFuelOrder);
						// remove the difference of the entire previous order
						data->moneyDiff += data->m_fuelOrder[data->item_index] / 10;
						// order as much as you can afford
						data->m_fuelOrder[data->item_index] = maxAffordableFuelOrder;
						data->moneyDiff -= data->m_fuelOrder[data->item_index] / 10;
					}
					else
					if (data->money + data->moneyDiff < 0)
					{
						data->m_fuelOrder[data->item_index] -= precentageRatio;
						data->moneyDiff += precentageRatio / 10;
					}
				}

			}
		break;
		case CONFIG_MNU:  // configuration menu
			if (data->item_index == 0) // GET interval
			{
				GETdeltaTime += 0.1f;
				if (GETdeltaTime > 1) GETdeltaTime -= 0.1f;
				break;
			}
			else if (data->item_index == 1) // POST interval
			{
				POSTdeltaTime += 0.1f;
				if (POSTdeltaTime > 1) POSTdeltaTime -= 0.1f;
				break;
			}
			break;

		default:
			break;
		}	
		break;
		/*
		// if landed, sanity check, and if we're in byuing / exchanging menu, numbered 0
		if (data->landedNearBase && data->item_index < nr_of_trading_items && data->menu_nr == 0) 
		{
		// increase item's amount pointed by item_index by multiplier
		data->item[data->item_index] += data->multiplier;
		// decrease money by multiplier (amount of bought items) * price in a contacted base of a given item pointed by item_index
		data->money -= data->multiplier * exchangeBase->pPrices[data->item_index];
		// if we're overloaded or ran out of cash, roll everything back
		if (VesselsLoad(data->item) > maximum_load || data->money < 0)
		{
		data->item[data->item_index] -= data->multiplier;
		data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
		}
		} 
		// if we're not landed but a selected port is used (we're data->docked), sanity check, and menu number check
		else if (data->portUsed[data->selectedPort] && data->item_index < nr_of_trading_items && data->menu_nr == 0)
		{
		// add some cargo to our ship
		data->item[data->item_index] += data->multiplier;
		// and decrease the same amount from the ship or station that we're data->docked with
		exchangePartner->item[data->item_index] -= data->multiplier;
		//data->money -= data->multiplier * exchangePartner->pPrices[data->item_index];  // TODO: Add spport for trading with orbital stations
		// if we're overloaded or our partner's supplies have been depleted or we're bankrupt, roll everythng back
		if (VesselsLoad(data->item) > maximum_load || exchangePartner->item[data->item_index] < 0 || data->money < 0)
		{
		data->item[data->item_index] -= data->multiplier;
		exchangePartner->item[data->item_index] += data->multiplier;
		//data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
		}
		}
		*/

	case OAPI_KEY_MINUS:  	// selling / leaving cargo. Inverse from the above
		/*if (data->landedNearBase && data->item_index < nr_of_trading_items && data->menu_nr == 0)
		{
			data->item[data->item_index] -= data->multiplier;
			if (data->item[data->item_index] < 0 ) data->item[data->item_index] += data->multiplier;
			else data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
		} else if (data->portUsed[data->selectedPort] && data->item_index < nr_of_trading_items && data->menu_nr == 0)
		{
			data->item[data->item_index] -= data->multiplier;
			exchangePartner->item[data->item_index] += data->multiplier;
			if (data->item[data->item_index] < 0 || VesselsLoad(exchangePartner->item) > maximum_load) 
			{
				data->item[data->item_index] += data->multiplier;
				exchangePartner->item[data->item_index] -= data->multiplier;
			}
			//else data->money += data->multiplier * exchangeBase->pPrices[data->item_index];

		}
		*/
		switch (data->menu_nr)
		{
		case TRANSFER_MNU:
			if (data->landedNearBase && data->localBasePricesRead && data->payloadRead && data->item_index < nr_of_trading_items2)
			{
				// increase item's amount pointed by item_index by multiplier
				data->sel_unit_id = data->v_localProductId.at(data->item_index);
				// increase item's amount pointed by item_index by multiplier
				
				data->m_order[data->sel_unit_id] -= data->multiplier;
				data->money += data->multiplier * data->v_localProductPrice.at(data->item_index);
				if (data->m_order[data->sel_unit_id] < 0)
				{
					int i = 0;
					bool found = false;
					for (i=0; i < data->v_payloadId.size(); i++)
						if (data->sel_unit_id == data->v_payloadId.at(i))
						{
							found = true;
							break;
						}				
					if(!found)
					{
						data->m_order[data->sel_unit_id] += data->multiplier;
						data->money -= data->multiplier * data->v_localProductPrice.at(data->item_index);
					}
					else if (-data->m_order[data->sel_unit_id] > data->v_payloadAmount.at(i))
					{
						data->m_order[data->sel_unit_id] += data->multiplier;
						data->money -= data->multiplier * data->v_localProductPrice.at(data->item_index);
					}
					
				}
				// decrease money by multiplier (amount of bought items) * price 
				//in a contacted base of a given item pointed by item_index
				//data->money -= data->multiplier * exchangeBase->pPrices[data->item_index];
				// if we're overloaded or ran out of cash, roll everything back
				/*if (VesselsLoad(data->item) > maximum_load || data->money < 0)
				{
					data->item[data->item_index] -= data->multiplier;
					data->money += data->multiplier * exchangeBase->pPrices[data->item_index];
				}*/
				
			} 
		break;
		case FUEL_MNU:
			if (data->landedNearBase && data->localBasePricesRead && data->item_index < propellantCount)
			{
				// increase item's amount pointed by item_index by multiplier
				precentageRatio = data->v_fuelMaxMass.at(data->item_index) * data->multiplier /100;
				// if the current order is less than all you can sell
				if (data->m_fuelOrder[data->item_index] > - data->v_fuelMass.at(data->item_index))
				{
					// if you want to sell less than you can
					if (data->m_fuelOrder[data->item_index] - precentageRatio  < 
							- data->v_fuelMass.at(data->item_index)) 
					{
						//sprintf(oapiDebugString(),"3");
						// remove the difference of the previous order
						data->moneyDiff += data->m_fuelOrder[data->item_index] / 10;
						// sell so much that you have exactly 0
						if (data->m_fuelOrder[data->item_index] != - data->v_fuelMass.at(data->item_index))
							data->moneyDiff += (data->v_fuelMass.at(data->item_index) )/ 10;
						data->m_fuelOrder[data->item_index] = - data->v_fuelMass.at(data->item_index);
					} 
					else
					{
						//sprintf(oapiDebugString(),"4");
						data->m_fuelOrder[data->item_index] -= precentageRatio;
						data->moneyDiff += precentageRatio / 10;
					}	
				}
			}			
			break;
		case CONFIG_MNU:  // configuration menu
			if (data->item_index == 0) // GET interval
			{
				GETdeltaTime -= 0.1f;
				if (GETdeltaTime < 0.1f) GETdeltaTime += 0.1f;
			}
			else if (data->item_index == 1) // POST interval
			{
				POSTdeltaTime -= 0.1f;
				if (POSTdeltaTime < 0.1f) POSTdeltaTime += 0.1f;
			}


			break;

		default:
			break;
		}
		break;
	case OAPI_KEY_RBRACKET: 	// navigate through the menus (next item)
		data->item_index++;		// increment item index
		switch (data->menu_nr)
		{
		case nr_of_menu_items: // main menu
			// automatically reset index when we exceed the number of items in this menu
			// -1 because there's no main menu selector in the main menu,
			//  and the last item in every menu is the main menu selector
			if (data->item_index > nr_of_main_menu_items - 1) data->item_index=0;
			break;
		case TRANSFER_MNU: // trading post
			if (data->item_index > nr_of_trading_items2) data->item_index=0;
			break;
		case PORT_SEL_MNU: // docking port selection
			if (data->item_index > nr_of_docking_ports) data->item_index=0;
			break;
		case CHECK_PLANETS_MNU: // check planets of other stocks
			if (data->item_index > data->nr_of_planets) data->item_index = 0;
			break;
		case BASES_LIST_MNU: // check other stocks. 
			if (data->item_index > data->v_basesAt1Planet.size() + 1) data->item_index = 0;
			break;
		case BASE_PRICES_MNU:  // check a particular base
			if (data->item_index > 1) data->item_index=0;
			break;
		case FUEL_MNU:
			if (data->item_index > propellantCount) data->item_index = 0;
		case CONFIG_MNU:  // configuration menu
			if (data->item_index > nr_of_config_items) data->item_index=0;
			break;
		case ACCOUNT_MNU:
			if (data->item_index > nr_of_account_items) data->item_index=0;
			break;
		default:
			break;
		}
		break;
	case OAPI_KEY_LBRACKET: // previous item
		data->item_index--; 	// decrement item index
		switch (data->menu_nr)
		{
		case nr_of_menu_items: // main menu
			if (data->item_index < 0) data->item_index = nr_of_main_menu_items - 1;
			break;
		case TRANSFER_MNU: // trading post/exchange
			if (data->item_index < 0) data->item_index = nr_of_trading_items2;
			break;
		case PORT_SEL_MNU: // docking port selection
			if (data->item_index < 0) data->item_index = nr_of_docking_ports;
			break;
		case CHECK_PLANETS_MNU: // check planets of other stocks
			if (data->item_index < 0) data->item_index = data->nr_of_planets;
			break;
			// exception: There's an additional choice: Go up, hence + 1
		case BASES_LIST_MNU: // check other stocks
			if (data->item_index < 0) data->item_index = data->v_basesAt1Planet.size() + 1;
			break;
		case BASE_PRICES_MNU:  // check a particular base
			// exception: we have little choice here - 
			// there are only Main Menu and Go up here, hence 2 items
			if (data->item_index < 0) data->item_index = 1; 
			break;
		case FUEL_MNU:
			if (data->item_index < 0) data->item_index = propellantCount;
		break;
		case CONFIG_MNU:  // configuration menu
			if (data->item_index < 0) data->item_index = nr_of_config_items;
			break;
		case ACCOUNT_MNU:
			if (data->item_index < 0) data->item_index = nr_of_account_items;
		default:
			break;
		}

		break;
	case OAPI_KEY_S: // select item. Actions depending on selected menu item should be added here
		switch (data->menu_nr)
		{
		case nr_of_menu_items: 	// main menu is the last item in a Main Menu list
			if (data->shipValid || data->item_index == nr_of_main_menu_items - 1)	{
				data->menu_nr = data->item_index;
				data->item_index = 0;
			}
			//if (data->menu_nr == TRANSFER_MNU) localBasePricesRead = false;
			break;
		case TRANSFER_MNU: // trading post/exchange
			if (data->item_index == nr_of_trading_items2) // if main menu selected
			{
				data->item_index = TRANSFER_MNU;
				data->menu_nr = nr_of_menu_items; // return to main menu
				//data->item_index = 0;
				break;
			}
			else if (data->landedNearBase)
			{
				data->POSTtransferRequest = true;
				break;
			}
			else if (data->portUsed[data->selectedPort])
			{
				//data->POSTdockTransfer = true;
			}
			break;
		case PORT_SEL_MNU: 	// port selection currently
			if (data->item_index == nr_of_docking_ports) // if main menu selected
			{
				data->item_index = data->menu_nr;
				data->menu_nr = nr_of_menu_items; // return to main menu
				//data->item_index = 0;
				break;
			}
			// define a selected port by the selected item and check if it's used
			data->selectedPort = data->item_index;
			if (data->portUsed.at(data->selectedPort)) // if we're actually data->docked to something
			{	
				data->menu_nr = TRANSFER_MNU; // jump to trade/exchange menu
				//data->item_index = 0;
			}
			break;
		case CHECK_PLANETS_MNU: // check planets of other stocks
			if (data->item_index == data->nr_of_planets) // if main menu selected
			{
				data->item_index = data->menu_nr;
				data->menu_nr = nr_of_menu_items; // return to main menu
				//data->item_index = 0;
				break;
			}
			else
			{
				// if selected one planet, look for all bases belonging to that planet
				data->v_basesAt1Planet.clear();
				int i=0;
				for (i=0 ; i< v_planets.size(); i++)
				{
					// data->v_uniquePlanetsInd keeps indexes of unique planets from vector v_planets
					// data->item_index is a selected planet
					if (!v_planets.at(data->v_uniquePlanetsInd.at(data->item_index)).compare(v_planets.at(i)))
						data->v_basesAt1Planet.push_back(i);
				}

				data->selected_planet = data->item_index;
				data->menu_nr = BASES_LIST_MNU;  // select check other stocks
				data->item_index = 0;
			}
			break;
		case BASES_LIST_MNU: // check other stocks
			if (data->item_index == data->v_basesAt1Planet.size()) // if main menu selected
			{
				data->item_index = data->menu_nr-1; // we want to return to planet selection
				data->menu_nr = nr_of_menu_items; // return to main menu
				//data->item_index = 0;
				break;
			}
			else if (data->item_index == data->v_basesAt1Planet.size() + 1) // if go up selected
			{
				data->menu_nr = CHECK_PLANETS_MNU;  // return to planet selection
				data->item_index = data->selected_planet;
				break;
			}
			else if (data->item_index < data->v_basesAt1Planet.size() && data->item_index >= 0)
			{
				data->remoteBasePricesRead = false;
				data->selected_base = data->item_index;
				data->item_index = 1;  // highlight Go Up
				data->menu_nr = BASE_PRICES_MNU; // jump to trade/exchange menu
				break;
			}
			break;
		case BASE_PRICES_MNU: 	// check a particular base
			if (data->item_index == 0) // if main menu selected (only two options)
			{	
				// point to the previously selected menu
				data->menu_nr = nr_of_menu_items; // return to main menu
				data->item_index = CHECK_PLANETS_MNU;
				break;
				// here you'll confirm your transfer
			} 
			else if (data->item_index == 1) // if go up selected (only two options)
			{
				data->menu_nr = BASES_LIST_MNU;
				data->item_index = data->selected_base;
				break;
			}
			break;
		case FUEL_MNU:
			if (data->item_index == propellantCount)
			{
				data->item_index = data->menu_nr;
				data->menu_nr = nr_of_menu_items;
				break;
			}
			else 
			{
				data->POSTfuelRequest = true;
			}
			
		break;
		case CONFIG_MNU:  // configuration menu
			if (data->item_index == nr_of_config_items)
			{
				data->item_index = data->menu_nr;
				data->menu_nr = nr_of_menu_items;
			}
			break;
		case ACCOUNT_MNU:  // configuration menu
			if (data->item_index == nr_of_account_items)
			{
				data->item_index = data->menu_nr;
				data->menu_nr = nr_of_menu_items;
			}
			else if (data->item_index == 0) 
			{
				oapiOpenInputBox("Username", clbkUsernameDialog, "", 20, this);	
			}
			else if (data->item_index == 1) { break;}
			// store account info
			else if (data->item_index == 2) {
				std::ofstream accountWriter;
				accountWriter.open("Config/Orbitrader/account.txt",ios::trunc);
				if (accountWriter.is_open())
				{
					accountWriter << "username: "<< username << endl;
					accountWriter << "password: "<< password;
					accountWriter.close();
				}
			}
			// destroy account info
			else if (data->item_index == 3) {
				std::ofstream accountWriter;
				accountWriter.open("Config/Orbitrader/account.txt",ios::trunc);
				if (accountWriter.is_open()) 
				{
					accountWriter << "username: "<< endl;
					accountWriter << "password: ";
					accountWriter.close();
				}
			}			
		break;

		default:
			break;
		}
		break;
	case OAPI_KEY_APOSTROPHE:
		// multiplier as a number of items traded at once
		data->multiplier *= 10;
		// reset the multipier
		if (data->multiplier > 100) data->multiplier = 1;
		break;
	default:
		return(false);
		break;
	};

	return(true);
}

// Refresh of a MFD window
void OrbitraderMFD::Update (HDC hDC)
{
	//sprintf (oapiDebugString(), "gdb = %f",  gdb );
	sprintf (oapiDebugString(), "gi = %d",  gi );
	//sprintf (oapiDebugString(), "landed_nb = %d",  data->landedNearBase );
	//DB("OrbitraderMFD::Update");
	int i=0, j=0;
	vs2.version = 2;
	// We'll be ckecking for dock information and fuel information (later)
	vs2.flag = VS_DOCKINFOLIST | VS_FUELLIST;
	// set to NULL to allow Orbiter to allocate needed memory
	vs2.fuel = NULL;
	vs2.dockinfo = NULL;
	
	Title (hDC, "Orbitrader");
	
	StatusWriter(&hDC);
	//sprintf (oapiDebugString(),"bases read = %d",basesIndexRead );
	//if (chunk.memory)	sprintf (oapiDebugString(), (const char *)chunk.memory );

	// Get vessel status, as defined in Orbiter API Reference
	m_vessel->GetStatusEx(&vs2);
	
	nr_of_docking_ports = vs2.ndockinfo;
	
	if (data->validationDone && !data->shipValid) 	error_code = INVALID_SHIP_ERR;
	else if (error_code == INVALID_SHIP_ERR) error_code = 0;

	OBJHANDLE hSBody = NULL; 
	ELEMENTS el;

	int lineOffset = 0;
	memset(buf,'\0',sizeof(buf));
	const char * cptr = NULL;
	// display output to the user, depending on which menu was selected
	switch(data->menu_nr)
	{
	case nr_of_menu_items: // main menu
		// get our name
		strncpy(buf,m_vessel->GetName(),100);
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		lineOffset = 3;

		
		// highlight the selected item on grey, else leave it as yellow
		if (data->item_index == 0) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" Trading Post");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));

		if (data->item_index == 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" Port Selection");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));

		if (data->item_index == 2) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" View stocks");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == 3) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" Refueling menu");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));

		if (data->item_index == 4) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" Configuration menu");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == 5) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else SetTextColor(hDC, 0x0033FFFF);	// Yellow
		sbuf.assign(" Account menu");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		memset(buf,'\0',BUFLEN);
		sprintf(buf,"%0.1f$",data->money);
		TextOut(hDC,charwidth * 1,linespacing*(lineOffset++), buf, (int)strlen(buf));
		
		
		
		hSBody = m_vessel->GetSurfaceRef();
		if (hSBody) 	{
		m_vessel->GetElements(hSBody, el);
		}
		
		ssbuf.str(std::string());
		ssbuf<<el.e;
		TextOut(hDC,0,linespacing*lineOffset++,ssbuf.str().c_str(), (int)(ssbuf.str().size()));

		break;

	case TRANSFER_MNU:
	try {
		lineOffset = 6;
		nr_of_trading_items2 = data->v_localProductName.size();
		memset(buf,'\0',sizeof(buf));
		strncpy(buf,m_vessel->GetName(),50);
		if (data->portUsed[data->selectedPort]) 
		{
			strncat(buf,", xfer with ",BUFLEN - strlen(buf) - 1);
			if (data->exchangePartner->shipValid)
			// get name of vessel with which we're exchanging our cargo
				strncat(buf,data->exchangePartner->vessel->GetName(),BUFLEN - strlen(buf) - 1);
			else strncat(buf,"an invalid ship.",BUFLEN - strlen(buf) - 1);
		}		
		else 
		if (data->landedNearBase)
		{
			strncat(buf,", landed at: ",BUFLEN - strlen(buf) - 1);
			// get name of vessel with which we're exchanging our cargo
			memset(buf2,'\0',BUFLEN);	
			oapiGetObjectName(vs2.base, buf2, BUFLEN);
			strncat(buf,buf2,BUFLEN - strlen(buf) - 1);
		}

		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == nr_of_trading_items2) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		if (data->localBasePricesRead && data->payloadRead)
		{
			SetTextColor(hDC, 0x00FFBB22);	// Light blue
			memset(buf,'\0',BUFLEN);
			sprintf(buf,"%0.1f$, x%d",data->money,data->multiplier);
			ssbuf.str(std::string());
			ssbuf.setf(std::ios_base::fixed);
			ssbuf.precision(1);
			ssbuf << data->money <<"$ ";
			ssbuf.precision(0);
			ssbuf << data->multiplier <<"x";
			TextOut(hDC,charwidth * 1,linespacing*(lineOffset-1), ssbuf.str().c_str(), ssbuf.str().size());
			sbuf.assign("Price Stock Cargo");
			SetTextColor(hDC, 0x00FFFF);	// Yellow
			TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(lineOffset-1), sbuf.c_str(), sbuf.size());

			for (i=0; i < nr_of_trading_items2; i++)
			{
				sbuf.assign(" ");
				sbuf.append(data->v_localProductName.at(i), 0, static_cast<int>(MFD_CHAR_LEN/2 -1));
				if (data->item_index == i) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
				else 						SetTextColor(hDC, 0x00FFFF);	// Yellow
				TextOut(hDC,0,linespacing*(i+lineOffset), sbuf.c_str(), sbuf.size());
				ssbuf.str(std::string());
				ssbuf << data->v_localProductPrice.at(i) << "$";
				// adding a certain amount of whitespaces to fit the text before the next column
				sbuf2.assign("               ");
				sbuf.assign(sbuf2,0,7-ssbuf.str().size());
				//sprintf (oapiDebugString(), "size = %d",  ssbuf.str().size() );
				ssbuf << sbuf;
				ssbuf << data->v_localProductAmount.at(i);
				sbuf.assign(sbuf2,0,12-ssbuf.str().size());
				ssbuf << sbuf;
				
				for (j=0; j < data->v_payloadId.size(); j++)
				{
					if (data->v_payloadId.at(j) == data->v_localProductId.at(i))
					{
						ssbuf << data->v_payloadAmount.at(j);
					}
				}
				int amount = data->m_order[data->v_localProductId.at(i)];
				if (amount)
				{
					if (amount >= 0)
						ssbuf << "+"<<amount;			
					else 
						ssbuf << amount;
				}
				SetTextColor(hDC, 0x0000EE00);	// Green
				// print the value starting from the middle of display
				TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(i+lineOffset), ssbuf.str().c_str(), ssbuf.str().size());
				ssbuf.str(std::string());

			}
		}
		}
		catch (exception & e)
		{	DBe("Transfer Menu",e);	}

		break;

	case PORT_SEL_MNU: // Docking port selection
	try {
		//nr_of_docking_ports
		char portNum[10];
		memset(portNum, '\0', sizeof(portNum));
		memset(buf, '\0', BUFLEN);
		lineOffset = 5;

		strncpy(buf,m_vessel->GetName(),50);
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == nr_of_docking_ports) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		// display all ports
		for (i=0; i < data->portUsed.size(); i++)
		{
			memset(buf,'\0',BUFLEN);
			strcpy(buf," Dock ");
			// the ports' numbers start from 1, not 0
			sprintf(portNum, "%d",i+1);
			strcat(buf,portNum);
			if (data->portUsed.at(i)) strcat(buf,": used");
			else strcat(buf,": unused");
			if (data->item_index == i) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
			else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
			// display every port (through i) 
			TextOut(hDC,0,linespacing*(i+lineOffset), buf, (int)strlen(buf));
		}
	} 
	catch (exception & e)
	{	
		DBe("PORT SELECTION" ,e);
	}
		break;

	case CHECK_PLANETS_MNU: // Planet selection of other bases
		lineOffset = 5;

		strncpy(buf,m_vessel->GetName(),50);
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == data->nr_of_planets) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		try {
			for (i=0; i < data->nr_of_planets; i++)
			{
				memset(buf,'\0',sizeof(buf));
				strcpy(buf," Body: ");
				const char * cptr = v_planets.at(data->v_uniquePlanetsInd.at(i)).c_str();
				strncat(buf, cptr , BUFLEN - strlen(buf) - 1);
				if (data->item_index == i) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
				else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
				// display every port (through i) 
				TextOut(hDC,0,linespacing*(i+lineOffset), buf, (int)strlen(buf));
			}
		}
		catch (exception & e) {
			DBe("CHECK_PLANETS_MNU",e); }
		break;

	case BASES_LIST_MNU: // check other stocks on one planet
		lineOffset = 6;
		memset(buf,'\0',BUFLEN);
		strncpy(buf, "Checking bases on: ", BUFLEN - 1);
		cptr = v_planets.at(data->v_uniquePlanetsInd.at(data->selected_planet)).c_str();
		strncat(buf, cptr, BUFLEN - strlen(buf) - 1); 
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == data->v_basesAt1Planet.size()) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		if (data->item_index == data->v_basesAt1Planet.size() + 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*4," Go up ", 7);

		for (i=0; i < data->v_basesAt1Planet.size(); i++)
		{
			memset(buf,'\0',BUFLEN);
			strncpy(buf," ", BUFLEN - 1);
			const char * cptr = v_bases.at(data->v_basesAt1Planet.at(i)).c_str();
			strncat(buf, cptr, static_cast<int>(MFD_CHAR_LEN/2));
			if (data->item_index == i) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
			else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
			// display every base on that planet (through i) 
			TextOut(hDC,0,linespacing*(i+lineOffset), buf, (int)strlen(buf));

			//int bufPrevLen = (int)strlen(buf);
			memset(buf,'\0',BUFLEN);
			if (i + MFD_LINE_NR - lineOffset < data->v_basesAt1Planet.size())
			{
				const char * cptr = v_bases.at(data->v_basesAt1Planet.at(i + MFD_LINE_NR - lineOffset)).c_str();
				strncpy(buf, cptr, 
					static_cast<int>(MFD_CHAR_LEN/2));

				if (data->item_index == i + MFD_LINE_NR - lineOffset) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
				else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
				// display every base on that planet (through i) 
				TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 + 1),linespacing*(i+lineOffset), buf, (int)strlen(buf));		
			}
		}
		break;

	case BASE_PRICES_MNU: // trading post
	
		lineOffset = 6;
		nr_of_trading_items2 = data->v_remoteProductName.size();
		memset(buf,'\0',sizeof(buf));
		//strncpy(buf,m_vessel->GetName(),50);
		//if (data->portUsed[data->selectedPort]) 
		//{
		//strcat(buf,", xfer with: ");
		// get name of vessel with which we're exchanging our cargo
		//strncpy(&buf[strlen(buf)],exchangePartner->vessel->GetName(),50);
		//}
		
		cptr = v_bases.at(data->v_basesAt1Planet.at(data->selected_base)).c_str();
		strncpy(buf, cptr, BUFLEN -1);
		strncat(buf, " on ", BUFLEN - strlen(buf) - 1); 
		cptr = v_planets.at(data->v_uniquePlanetsInd.at(data->selected_planet)).c_str();
		strncat(buf, cptr, BUFLEN - strlen(buf) - 1); 

		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == 0) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		if (data->item_index == 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*4," Go up ", 7);
	
		if (data->remoteBasePricesRead)
		{
			SetTextColor(hDC, 0x00FFBB22);	// Light blue
			ssbuf.str(std::string());
			ssbuf.setf(std::ios_base::fixed);
			ssbuf.precision(1);
			ssbuf << data->money <<"$";
			TextOut(hDC,charwidth * 1,linespacing*(lineOffset-1), ssbuf.str().c_str(), ssbuf.str().size());
			SetTextColor(hDC, 0x00FFFF);	// Yellow
			ssbuf.str(std::string());
			sbuf.assign("Price Stock Cargo");
			TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(lineOffset-1), sbuf.c_str(), sbuf.size());

			for (i=0; i < nr_of_trading_items2; i++)
			{
				sbuf.assign(" ");
				sbuf.append(data->v_remoteProductName.at(i), 0, static_cast<int>(MFD_CHAR_LEN/2 -1));
				SetTextColor(hDC, 0x00FFFF);	// Yellow
				// display every product in that base (through i) 
				TextOut(hDC,0,linespacing*(i+lineOffset), sbuf.c_str(), sbuf.size());
				ssbuf.str(std::string());
				try {
				ssbuf << data->v_remoteProductPrice.at(i) << "$"; }
				catch (exception & e)	{
				DBe("remote price",e);	}
				// adding a certain amount of whitespaces to fit the text before the next column
				sbuf2.assign("               ");
				sbuf.assign(sbuf2,0,7-ssbuf.str().size());
				//sprintf (oapiDebugString(), "size = %d",  ssbuf.str().size() );
				try {
				ssbuf << sbuf << data->v_remoteProductAmount.at(i); }
				catch (exception & e)	{
				DBe("remote amount",e);	}
				sbuf.assign(sbuf2,0,12-ssbuf.str().size());
				ssbuf << sbuf;
				SetTextColor(hDC, 0x0000EE00);	// Green
				try {
				for (j=0; j < data->v_payloadId.size(); j++)
				{
					if (data->v_payloadId.at(j) == data->v_remoteProductId.at(i))
					{
						ssbuf << data->v_payloadAmount.at(j);
					}
				} }
				catch (exception & e)	{
				DBe("remote id",e);	}
				TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(i+lineOffset), ssbuf.str().c_str(), ssbuf.str().size());
				ssbuf.str(std::string());
			} 
		} 
		
		
		break;
		
	case FUEL_MNU:
		lineOffset = 5;
		memset(buf,'\0',BUFLEN);
		strncpy(buf, "Refueling menu", BUFLEN - 1);
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));
		
		if (data->item_index == nr_of_config_items) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);
		ssbuf.str(std::string());
		ssbuf.precision(1);
		ssbuf.setf(std::ios_base::fixed);
		ssbuf <<  data->money + data->moneyDiff << "$  " << data->multiplier<< "x ";
		sbuf = ssbuf.str();
		ssbuf.str(std::string());
		
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,charwidth * 1,linespacing*(lineOffset++), sbuf.c_str(), sbuf.length());
		sbuf.assign("Status  Order");
		SetTextColor(hDC, 0x00FFFF);	// Yellow
		TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(lineOffset-1), sbuf.c_str(), sbuf.size());
		
		ssbuf.str(std::string());
		ssbuf.precision(1);
		ssbuf.setf(std::ios_base::fixed);
		ssbuf <<  data->moneyDiff << "$  " << data->multiplier<< "x ";
		sbuf = ssbuf.str();
		ssbuf.str(std::string());
		SetTextColor(hDC, 0x00FFFF);	// Yellow
		TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(10), sbuf.c_str(), sbuf.size());
		
		for (i=0; i<propellantCount; i++)
		{
			ssbuf << "Tank " << i+1 << " --------";
			if (data->item_index == i) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
			else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
			TextOut(hDC,charwidth * 1,linespacing*(lineOffset++), ssbuf.str().c_str(), ssbuf.str().length());
			ssbuf.str(std::string());
			if (vs2.fuel)
			{
				ssbuf << vs2.fuel[i].level * 100;	
				sbuf2.assign("               ");
				sbuf.assign(sbuf2,0,8-ssbuf.str().size());
				ssbuf << sbuf << data->m_fuelOrder[i]/(data->v_fuelMaxMass.at(i)+0.001) * 100;	
			}
			SetTextColor(hDC, 0x0000EE00);	// Green
			TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2 +1),linespacing*(lineOffset-1), ssbuf.str().c_str(), ssbuf.str().size());
			ssbuf.str(std::string());
		}
		
		break;
		
		
		
	break;

	case CONFIG_MNU:
		lineOffset = 5;
		memset(buf,'\0',BUFLEN);
		strncpy(buf, "Configuration menu", BUFLEN - 1);
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,buf, (int)strlen(buf));

		if (data->item_index == nr_of_config_items) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		if (data->item_index == 0) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		TextOut(hDC,0,linespacing*lineOffset," Download interval", 18);
		memset(buf,'\0',BUFLEN);
		sprintf(buf, "%0.1f s", GETdeltaTime);
		SetTextColor(hDC, 0x0000EE00);	// Green
		TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2+5),linespacing* lineOffset, buf, (int)strlen(buf));
		if (data->item_index == 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		TextOut(hDC,0,linespacing*(lineOffset +1)," Upload interval", 16);
		memset(buf,'\0',BUFLEN);
		sprintf(buf, "%0.1f s", POSTdeltaTime);
		SetTextColor(hDC, 0x0000EE00);	// Green
		TextOut(hDC,charwidth * static_cast<int>(MFD_CHAR_LEN/2+5),linespacing* (lineOffset +1), buf, (int)strlen(buf));
		break;
		
	case ACCOUNT_MNU:
		lineOffset = 5;
		sbuf.assign("Account menu");
		SetTextColor(hDC, 0x00FFBB22);	// Light blue
		TextOut(hDC,0,linespacing*1,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == nr_of_account_items) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);
		
		if (data->item_index == 0) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		sbuf.assign(" Login");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		sbuf.assign(" Logout");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == 2) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		sbuf.assign(" Store username and psswd (insecure)");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
		if (data->item_index == 3) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x00FFFF);	// Yellow
		sbuf.assign(" Erase local account info");
		TextOut(hDC,0,linespacing*lineOffset++,sbuf.c_str(), (int)(sbuf.size()));
		
	break;

		/*
		if (data->item_index == nr_of_trading_items) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0022FFAA);	// Lime Green
		TextOut(hDC,0,linespacing*3," Main menu ", 11);

		if (data->item_index == 0) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
		// display cargo name
		TextOut(hDC,0,linespacing*5," Gold ", 6);
		SetTextColor(hDC, 0x0000EE00);	// Green
		memset(buf,'\0',sizeof(buf));
		// get amount of that cargo
		sprintf(buf, "%d kg", data->item[0]);
		// and display it as a whole
		TextOut(hDC,charwidth*6,linespacing*5,buf, (int)strlen(buf));


		if (data->item_index == 1) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*6," Silver ", 8);
		SetTextColor(hDC, 0x0000EE00);	// Green
		memset(buf,'\0',sizeof(buf));
		sprintf(buf, "%d kg", data->item[1]);
		TextOut(hDC,charwidth*8,linespacing*6,buf, (int)strlen(buf));

		if (data->item_index == 2) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*7," Water ", 7);
		SetTextColor(hDC, 0x0000EE00);	// Green
		sprintf(buf, "%d kg", data->item[2]);
		TextOut(hDC,charwidth*7,linespacing*7,buf, (int)strlen(buf));

		if (data->item_index == 3) 	SetTextColor(hDC, 0x00E0E0E0); // Grey
		else 					SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*8," Dust ", 6);
		SetTextColor(hDC, 0x0000EE00);	// Green
		sprintf(buf, "%d kg", data->item[3]);
		TextOut(hDC,charwidth*6,linespacing*8,buf, (int)strlen(buf));

		SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*10," Money ", 7);
		SetTextColor(hDC, 0x0000EE00);	// Green
		sprintf(buf, "%.2f $", data->money);
		TextOut(hDC,charwidth*7,linespacing*10,buf, (int)strlen(buf));

		SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*11," Multiplier ", 12);
		SetTextColor(hDC, 0x0000EE00);	// Green
		sprintf(buf, "%dx", data->multiplier);
		TextOut(hDC,charwidth*12,linespacing*11,buf, (int)strlen(buf));

		SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*12," Current Load ", 14);
		SetTextColor(hDC, 0x0000EE00);	// Green
		// calculate sum of ship's items
		sprintf(buf, "%d kg", VesselsLoad(data->item));
		TextOut(hDC,charwidth*14,linespacing*12,buf, (int)strlen(buf));

		SetTextColor(hDC, 0x0033FFFF);	// Yellow
		TextOut(hDC,0,linespacing*13," Maximal Load ", 14);
		SetTextColor(hDC, 0x0000EE00);	// Green
		sprintf(buf, "%d kg", maximum_load);
		TextOut(hDC,charwidth*14,linespacing*13,buf, (int)strlen(buf));
		*/
		break;

	default:
		break;
	}

	StatusWriter(&hDC);
	data->prevStatus = vs2.status;
	// API requirement
	if (vs2.fuel != NULL) delete vs2.fuel;
	if (vs2.dockinfo != NULL) delete vs2.dockinfo;
	//DB("Update end");
}

// Return button labels
char *OrbitraderMFD::ButtonLabel (int bt)
{
	DB("OrbitraderMFD::ButtonLabel");
	// The labels for the two buttons used by our MFD mode
	static char *label[amount_of_buttons] = {"PRV", "NXT","SCT","BUY","SLL","ADJ"};
	return (bt < amount_of_buttons ? label[bt] : 0);
}

// Return button menus
int OrbitraderMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	DB("OrbitraderMFD::ButtonMenu");
	// The menu descriptions for the two buttons
	static const MFDBUTTONMENU mnu[amount_of_buttons] = {
		{"Prev. item", 0, '['},
		{"Next item", 0, ']'},
		{"Select option", 0, 'S'},
		{"Buy / Take", 0, '+'},
		{"Sell / Leave", 0, '-'},
		{"Adj. multiplier", 0, '"'}
	};
	if (menu) *menu = mnu;
	return amount_of_buttons; // return the number of buttons used
}

bool OrbitraderMFD::ConsumeButton (int bt, int event)
{
	// check if mouse pressed on a button
	if (event & PANEL_MOUSE_LBDOWN) {
		switch(bt) {
			case 0:
				ConsumeKeyBuffered(OAPI_KEY_LBRACKET);
				break;
			case 1:
				ConsumeKeyBuffered(OAPI_KEY_RBRACKET);
				break;
			case 2:
				ConsumeKeyBuffered(OAPI_KEY_S);
				break;
			case 3:
				ConsumeKeyBuffered(OAPI_KEY_EQUALS);
				break;
			case 4:
				ConsumeKeyBuffered(OAPI_KEY_MINUS);
				break;
			case 5:
				ConsumeKeyBuffered(OAPI_KEY_APOSTROPHE);
				break;
			default:
				return(false);
				break;
		};
	}

	return(true);
}



int OrbitraderMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	DB("OrbitraderMFD::MsgProc");
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new OrbitraderMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

// FIXME - nr_of_trading_items2 ?
int OrbitraderMFD::VesselsLoad(int * item) // Just calculate whole load (sum of all items)
{
	DB("OrbitraderMFD::VesselsLoad");
	int sum=0, i=0;
	for (i=0; i<nr_of_trading_items2; i++)
	{
		sum += item[i];
	}
	return sum;
}





static bool clbkUsernameDialog(void *id, char *str, void *usrdata)
{
	OrbitraderMFD * cOrbitraderMFD = (OrbitraderMFD*) usrdata;
	// Use the provided string as username
	cOrbitraderMFD->AssignUsername(str);
	// now invoke the dialog for password
	oapiOpenInputBox("Password", clbkPasswordDialog, "", 20, usrdata);
	// but don't hide all InputBoxes just yet
	return false;
}

static bool clbkPasswordDialog(void *id, char *str, void *usrdata)
{
	OrbitraderMFD * cOrbitraderMFD = (OrbitraderMFD*) usrdata;
	cOrbitraderMFD->AssignPassword(str);
	// both strings assigned, we're done here. Hide the InputBox and check the strings' correctness later
	return true;
}

void OrbitraderMFD::AssignUsername(char * str)
{
	username.assign(str);
}
void OrbitraderMFD::AssignPassword(char * str)
{
	password.assign(str);
}


void OrbitraderMFD::StatusWriter(HDC * hDC)
{
// Status writer. If something is happening...
	if (xfer_code != 0) 
	{
		SetTextColor(*hDC, 0x0055AAEE);	// Light Red
		switch (xfer_code)
		{
		case GET_BASES_INDEX:
			TextOut(*hDC,0,linespacing*2," Downloading bases list", 23);
			break;
		case GET_LOCAL_BASE:
			TextOut(*hDC,0,linespacing*2," Downloading local prices list", 30);
			break;
		case GET_REMOTE_BASE:
			TextOut(*hDC,0,linespacing*2," Downloading remote prices list", 31);
			break;
		case GET_PAYLOAD:
			TextOut(*hDC,0,linespacing*2," Downloading payload", 20);
			break;
		case GET_VALIDATE:
			TextOut(*hDC,0,linespacing*2," Validating ship", 16);
			break;
			
		case POST_LOGIN:
			TextOut(*hDC,0,linespacing*2," Logging in", 11);
			break;
		case POST_TRANSFER:
			TextOut(*hDC,0,linespacing*2," Uploading transfer request", 27);
			break;
		case POST_FUEL_ORDER:
			TextOut(*hDC,0,linespacing*2," Uploading fuel request", 23);
			break;
		case POST_STATUS_LANDED:
		case POST_STATUS_LAUNCHED:
		case POST_STATUS_LEFT_ORBIT:
		case POST_STATUS_ENTERED_ORBIT:
		case POST_STATUS_SLINGED:
		case POST_STATUS_POWER_SLINGED:
			TextOut(*hDC,0,linespacing*2," Uploading ship's status", 24);
		break;
		default:
			break;
		}
	} else if (error_code != 0) {
		SetTextColor(*hDC, 0x000000FF);	// Red
		switch (error_code)
		{
			case LOGIN_INCORRECT_ERR:		
				TextOut(*hDC,0,linespacing*2," Login incorrect!", 17);
			break;
			case INVALID_SHIP_ERR:
				TextOut(*hDC,0,linespacing*2," Invalid ship!", 14);
			break;
			default:
			break;
		}
	}
}

MFDData * associateMFDData(VESSEL * vessel)
{
	int i = 0;
	// Code and comments below provided by Mohd Ali (Computerex) and modified by Simon Ender (Enjo)
	bool Found = false;

	// Scan through the v_MFDdata vector
	for (i = 0; i < v_MFDdata.size(); i++)
	{
		// if there is a null pointer, break, just to be safe
		if (!v_MFDdata[i]->vessel)
			continue;

		// if the handle of the vessel this MFD is linked to is the same as the handle of the vessel inside the MFDData object that is currently being scanned
		//if (vessel->GetHandle() == v_MFDdata[i]->vessel->GetHandle())
		if (vessel == v_MFDdata[i]->vessel)
		{
			//this->data = v_MFDdata[i];
			Found=true;
			return v_MFDdata.at(i);
			break;
		}
	}
	// if no match is found, let's create one from scratch
	if (!Found)
	{
		// Create a new MFDData object
		MFDData * dat = new MFDData(vessel);
		//  Set the MFDData object's vessel hook to the vessel pointer this MFD is linked to
		//dat->vessel = vessel;
		// store this MFDData object inside the v_MFDdata vector
		v_MFDdata.push_back(dat);
		// set our data member of our myMFD class to the new MFDData object we just created
		//this->data=dat;
		// Code and comments above provided by Mohd Ali (Computerex) and modified by Simon Ender (Enjo)
		return dat;
		
		// tymczasowe - utworzenie...
		//this->data->item = new int[nr_of_trading_items];
		// ... i zainicjalizowanie artykulow jakas wartoscia
		//for (i=0; i<nr_of_trading_items; i++) {
		//	this->data->item[i] = 10;
		//}
	}
	return NULL;
}

void inline DB(const string& s) {
	if (fdb) {
		fdb.precision(6);
		fdb << oapiGetSysTime () << "  " << s << std::endl;
		fdb.flush();
	}
}

void inline DBe(const char* s, exception & e) {
	if (fdb) {
		fdb << oapiGetSysTime () << "  Standard exception in: "<< s << " " << e.what() << endl;
		fdb.flush();
	}
}

// Obsoleted code

// Update
// get relative position of our ship with a closest base, by its handle
	//if (vs2.base) 
	//	m_vessel->GetRelativePos(vs2.base, rel_pos);

	// and calculate straight line distance. We have to be 15 km from the center of the base and be landed
	//if (vs2.base && vs2.status && sqrt(rel_pos.x*rel_pos.x + rel_pos.y*rel_pos.y + rel_pos.z*rel_pos.z) < 15e3) 
	//data->landedNearBase = true;
	/*{
		if  (!data->landedNearBase)
		{
			// same logic as in MFD instances creation
			data->landedNearBase = true;
			bool Found = false;
			for (i = 0 ; i < Tradingobject.size(); i++)
			{
				// but check by handles. Bases don't use inteligent pointers
				if (!Tradingobject[i]->hCurrentBase)
					break;

				if (vs2.base == Tradingobject[i]->hCurrentBase)
				{
					// exchangePartner equivallent, meaning a base which with we'll be trading
					this->exchangeBase = Tradingobject[i];
					Found=true;
					break;
				}
			}
			// not found means it was not defined by config, nor XML
			if (!Found)
			{
				TradingObject * dat = new TradingObject(nr_of_trading_items);
				dat->hCurrentBase = vs2.base;
				// store base's handle in a vector
				Tradingobject.push_back(dat);
				this->exchangeBase = dat;
			}

		}
	}*/
	//else data->landedNearBase = data->localBasePricesRead = false;

/*
	nr_of_docking_ports = data->vs2.ndockinfo;

	// sanity check
	if (data->selectedPort >= data->vs2.ndockinfo) data->selectedPort=0;

	// if something is data->docked to our selected port
	if (data->vs2.dockinfo[data->selectedPort].rvessel != NULL)
	{

		//if (!data->portChkFlag)
		{
			data->portChkFlag = true;
			bool Found = false;
			for (j = 0; j < v_MFDdata.size(); j++)
			{
				if (!v_MFDdata[j]->vessel)
					break;
				if (data->vs2.dockinfo[data->selectedPort].rvessel == v_MFDdata[j]->vessel->GetHandle())
				{
					this->exchangePartner = v_MFDdata[j];
					Found=true;
					//petla = true;
					break;
				}
			}
			if (!Found)
			{
				MFDData * dat = new MFDData();
				// set that new MFDData instance, an inteligent pointer (Vessel Interface) by 
				// handle of vessel data->docked to the selected port 
				dat->vessel = oapiGetVesselInterface(data->vs2.dockinfo[data->selectedPort].rvessel);
				v_MFDdata.push_back(dat);
				// and set that vessel as our exchange partner
				this->exchangePartner = dat;
				this->exchangePartner->item = new int[nr_of_trading_items];
				for (i=0; i<nr_of_trading_items; i++) {
					// tymczasowe - daj statkowi do ktorego zadokowalismy, a nie zostal znaleziony,
					// po 45 jednostek wszystkiego.
					this->exchangePartner->item[i] = 45;
				}

				VESSELSTATUS2 vs2_2;
				vs2_2.version = 2;
				vs2_2.flag = VS_DOCKINFOLIST | VS_FUELLIST;
				vs2_2.fuel = NULL;
				vs2_2.dockinfo = NULL;

				// get status of our partner
				this->exchangePartner->vessel->GetStatusEx(&vs2_2);

				// a flag telling if sombody data->docked to one of our ports
				exchangePartner->portUsed = new bool [vs2_2.ndockinfo]; 
				// just initialise
				for (i=0; i<vs2_2.ndockinfo; i++) {
					exchangePartner->portUsed[i] = false; }

				// API requirement
				if (vs2_2.fuel != NULL) delete vs2_2.fuel;
				if (vs2_2.dockinfo != NULL) delete vs2_2.dockinfo;
			}
		}
	} else data->portChkFlag = false;

	// Docking ports scanning
	for (i=0; i < data->vs2.ndockinfo; i++)
	{
		if (data->vs2.dockinfo[i].rvessel != NULL)
			data->portUsed[i] = true;
		else
		{
			data->portUsed[i] = false;
			if (i == data->selectedPort) data->portChkFlag = false;
		}
	}
	
	
		int i = 0;
		// Code and comments below provided by Mohd Ali (Computerex) and modified by Simon Ender (Enjo)
		bool Found = false;
		// Scan through the v_MFDdata vector
		for (i = 0; i < v_MFDdata.size(); i++)
		{
			// if there is a null pointer, break, just to be safe
			if (!v_MFDdata[i]->vessel)
				continue;

			// if the handle of the vessel this MFD is linked to is the same as the handle of the vessel inside the MFDData object that is currently being scanned
			if (vessel->GetHandle() == v_MFDdata[i]->vessel->GetHandle())
			{
				//this->data = v_MFDdata[i];
				Found=true;
				break;
			}
		}
		// if no match is found, let's create one from scratch
		if (!Found)
		{
			// Create a new MFDData object
			MFDData * dat = new MFDData(vessel);
			//  Set the MFDData object's vessel hook to the vessel pointer this MFD is linked to
			//dat->vessel = vessel;
			// store this MFDData object inside the v_MFDdata vector
			v_MFDdata.push_back(dat);
			// set our data member of our myMFD class to the new MFDData object we just created
			//this->data=dat;
			// Code and comments above provided by Mohd Ali (Computerex) and modified by Simon Ender (Enjo)
		}

*/