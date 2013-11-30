// ==============================================================
//               ORBITER MODULE: OrbitraderMFD
//                  Part of the ORBITER SDK
// 	 Copyright (C) 2007         Simon Ender "Enjo"         - author and maintainer  ender-sz !AT! go2 pl
//	 Copyright (C) 2008	 Mohd "Computerex" Ali  - contributor
//                   All rights reserved
//
// MFDData.hpp
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
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <numeric>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include "consts.hpp"
//#define MY_ENCODING "ISO-8859-1"
#define MY_ENCODING "UTF-8"
#define BUFLEN 512

using namespace std;

class MFDData
{
public:
	MFDData (VESSEL *); 
	~MFDData ();
		 
	double CheckFuelMassIncrease();
	void RefuelBlocker();
	int CheckOrbitStatus();
	void CheckDockStatus();
	int XMLwriterTransfer(const std::string& planet, const std::string& basename);
	int XMLwriterFuel(const std::string& planet, const std::string& basename);
	int XMLwriterLogin(const std::string& username,const std::string& password);
	int XMLwriterStatusLanded();
	int XMLwriterStatusLaunched();
	int XMLwriterStatusLeftOrbit(int type);
	int XMLwriterStatusEnteredOrbit();
	int XMLwriterComponentVessel(xmlTextWriterPtr & writer);
	int XMLwriterComponentLocation(xmlTextWriterPtr & writer);
	int XMLwriterComponentFuelReserves(xmlTextWriterPtr & writer);
	int XMLwriterComponentElemens(xmlTextWriterPtr & writer, const ELEMENTS & el, const ORBITPARAM & prm);
	int XMLwriterComponentVectors(xmlTextWriterPtr & writer);
	xmlChar * ConvertInput(const char *in, const char *encoding);
	
	int ExtractXMLData (xmlChar * xpath, vector<string> * v_XMLData);
	int ExtractXMLData (xmlChar * xpath, vector<double> * v_XMLData);
	int ExtractXMLData (xmlChar * xpath, std::string * s_XMLData);
	int ExtractXMLData (xmlChar * xpath, double * d_XMLData);
	int PopulateStock(std::vector<std::string> * v_productName, std::vector<int> * v_productPrice, std::vector<int> * v_productId, std::vector<int> * v_ProductAmount);
	int PopulatePayload(std::vector<std::string> * v_productName, std::vector<int> * v_productAmount, std::vector<int> * v_productId);
	xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath);
	
	
	//void RefuelBlocker();
	//int CheckOrbitStatus();
	
	void GETLocalPrices();
	void GETBasesIndex();
	void GETPayload();
	void GETRemotePrices();
	void GETLoginStatus();
	void GETValidate();
	void GETRestore();
	void POSTLogin();
	void POSTTransferRequest();
	void POSTFuelOrder();
	
	void POSTStatus(int current_xfer_code, bool * statusFlag);
	
	unsigned int multiplier;
	int menu_nr; //0=main, 1=trade 2=other_markets 3=fuel 4=passenger 5=messages
	int item_index;
	int sel_unit_id;
	int selected_planet;
	int selected_base;
	int selected_tank;
	int cargoLoad;
	static double money;
	static double moneyDiff;
	
	bool shipValid;
	bool validationDone;
	bool payloadRead;
	bool stateRestored;
	
	bool localBasePricesRead;
	bool remoteBasePricesRead;
	bool POSTtransferRequest;
	bool POSTfuelRequest;
	
	bool docked;
	bool landedNearBase;
	bool statusLandedSent;
	bool statusLaunchedSent;
	bool leavingOrbit;
	bool enteringOrbit;
	bool reachingOrbit;
	bool powerSlinging;
	bool leavingOrbitSent;
	bool enteringOrbitSent;
	bool slingshotSent;
	bool powerSlingshotSent;
	
	double prevTime;
	double prevEcc;
	double prevEccG;
	double prevPeTG;
	double prevPeD;
	int prevStatus;
	
	int nr_of_planets;
	
	std::string s_loginStatus;
	
	std::string s_shipName;
	std::string s_shipClass;

	
	char buf[BUFLEN],buf2[BUFLEN];
	std::stringstream ssbuf;
	std::string sbuf, sbuf2;
	
	
	std::vector<double> v_fuelMaxMass;
	std::vector<double> v_fuelMass;
	std::vector <double> v_prevFuelMass;

	
	// FIXME : bases list should be in a singleton
	std::vector<int>  v_basesAt1Planet;
	std::vector<std::string> v_remoteProductName;
	std::vector<int> v_remoteProductPrice;
	std::vector<int> v_remoteProductId;
	std::vector<int> v_remoteProductAmount;
	std::vector<std::string> v_localProductName;
	std::vector<int> v_localProductPrice;
	std::vector<int> v_localProductId;
	std::vector<int> v_localProductAmount;
	std::vector<int> v_uniquePlanetsInd;
	std::map<int,int> m_order;
	std::map<int,double> m_fuelOrder;
	
	std::vector<std::string> v_payloadName;
	std::vector<int> v_payloadAmount;
	std::vector<int> v_payloadId;
	
	std::vector<bool> portUsed;
	
	//float * pPrices; TODO
	int selectedPort;
	bool portChkFlag;
	
	MFDData * exchangePartner;
	VESSEL * vessel;
	VESSELSTATUS2 vs2;
	OBJHANDLE hSBody,hGBody;
};