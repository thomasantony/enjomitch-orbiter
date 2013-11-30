// ==============================================================
//               ORBITER MODULE: OrbitraderMFD
//                  Part of the ORBITER SDK
// 	 Copyright (C) 2007         Simon Ender "Enjo"         - author and maintainer  ender-sz !AT! go2 pl
//	 Copyright (C) 2008	 Mohd "Computerex" Ali  - contributor
//                   All rights reserved
//
// Orbitrader.hpp
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


#include <Orbitersdk.h>
#include <string>
#include <map>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#define BUFLEN 512

using namespace std;

class TradingObject
{
public:
	//TradingObject(OBJHANDLE postHandle, int type=10);
	TradingObject(const int n);
	TradingObject(float * data, const int n);
	~TradingObject() { delete [] pPrices; }
	OBJHANDLE hCurrentBase;	
	float * pPrices;
private:
	char name[MAX_PATH];
	int amount_of_items;

	//int index;
	int tradingObjectType;
};

class MFDData;


class OrbitraderMFD: public MFD {
public:
	OrbitraderMFD (DWORD w, DWORD h, VESSEL *vessel);
	~OrbitraderMFD ();
	bool ConsumeKeyBuffered (DWORD key);
	bool ConsumeButton (int bt, int event);
	char *ButtonLabel (int bt);
	int  ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	/*
	bool SetAltRange (char *rstr);
	bool SetVradRange (char *rstr);
	bool SetVtanRange (char *rstr);
	*/
	void WriteStatus (FILEHANDLE scn) const {};
	void ReadStatus (FILEHANDLE scn) {};	
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	
	//static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
	//static size_t ReadMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
	static void *myrealloc(void *ptr, size_t size);
	//int ParseXML (std::vector<xmlChar *> v_XPathArgs, std::vector<char*> * v_XMLData);
	
	void AssignUsername(char * str);
	void AssignPassword(char * str);
	
	int VesselsLoad(int * item);
	int CurlGET(const char * suffix);
	int CurlPOST(const char * suffix);
	void StatusWriter(HDC * hDC);
	
	
	static const unsigned int nr_of_trading_items;
	static const unsigned int nr_of_config_items;
	static const unsigned int nr_of_account_items;
	
	
	static const unsigned int MFD_LINE_NR;
	static const unsigned int MFD_CHAR_LEN; 
	
	static const unsigned int amount_of_buttons;	
	static const unsigned int maximum_load;	
	

			
private:
	int mHeight;
	int mWidth;	 
	int linespacing;
	int charwidth;
	

	


	
	char buf[BUFLEN], buf2[BUFLEN];
	std::stringstream ssbuf;
	std::string sbuf, sbuf2;
 

	float * price;
	
	
	
	int propellantCount;
	double precentageRatio;
	
	int tradingObjectType;
	//bool * portUsed;
	unsigned int nr_of_docking_ports;

	VESSEL * m_vessel;
	VESSELSTATUS2 vs2;
	PROPELLANT_HANDLE propellHandle;
	
	TradingObject * exchangeBase;
	MFDData * data; // A pointer to an MFDData object
	

	
	FILE * file;
	

		
	
	//std::string POST_data;
		
	unsigned int nr_of_trading_items2;
	
	int GET_still_running;
	int GET_finished;
	int GET_started;
	
	int POST_still_running;
	int POST_finished;
	int POST_started;
	
	OBJHANDLE hSun;
	OBJHANDLE hLocalBody;
};



TradingObject::TradingObject(float * data, const int n):amount_of_items(n) 
{
	int i=0;
	pPrices = new float [amount_of_items];
	for (i=0 ;i<amount_of_items; i++)
	{
		pPrices[i] = data[i];
	}
}

	// set these prices in a base if it's not found in config nor XML (default prices)
TradingObject::TradingObject(const int n):amount_of_items(n) 
{
	int i=0;
	pPrices = new float [amount_of_items];
	for (i=0 ;i<amount_of_items; i++)
	{
		if (i==0) pPrices[i] = 100.0; 
		else if (i==1) pPrices[i] = 80.0;
		else if (i==2) pPrices[i] = 10.0;
		else if (i==3) pPrices[i] = 1.0; 
		else pPrices[i] = 50;
	}
}
//TradingObject::TradingObject(OBJHANDLE postHandle, float * data, const unsigned int n): pPrices(data), index(n)
/*TradingObject::TradingObject()
{
	switch (tradingObjectType)
	{
		case 0: // a base

				
		break;
			
		case 1: // a ship
			
			bool Found = false;
				// Scan through the MFDdata vector
			for (int i = 0; i < MFDdata.size(); i++)
			{
						// if there is a null pointer, break, just to be safe
				if (!MFDdata[i]->vessel)
					break;
		
						// if the handle of the vessel this MFD is linked to is the same as the handle of the vessel inside the MFDData object that is currently being scanned
				if (postHandle == MFDdata[i]->vessel->GetHandle())
				{
					this->exchangePartner = MFDdata[i];
					FoundShip=true;
					pPrices = new [] float[OrbitraderMFD::number_of_items];
					for (int i = 0; i < OrbitraderMFD::number_of_items; i++)
					{
						pPrices[i] = 0.0;
					}
					break;
				}
			}
				
			if (!Found)
			{
						// Create a new MFDData object
				MFDData * dat = new MFDData();
					   //  Set the MFDData object's vessel hook to the vessel pointer this MFD is linked to
				dat->vessel=vessel;
					   // store this MFDData object inside the MFDdata vector
				MFDdata.push_back(dat);
					   // set our data member of our myMFD class to the new MFDData object we just created
				this->exchangePartner=dat;
				this->exchangePartner->item = new int[number_of_items];
				pPrices = new [] float[OrbitraderMFD::number_of_items];
				for (int i=0; i<OrbitraderMFD::number_of_items; i++) {
					this->exchangePartner->item[i] = 50; // FIXME
					pPrices[i] = 0.0;
				}
			}
			break;
		default:
			break;
	}
}*/



	


