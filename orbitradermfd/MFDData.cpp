// ==============================================================
//               ORBITER MODULE: OrbitraderMFD
//                  Part of the ORBITER SDK
// 	 Copyright (C) 2007         Simon Ender "Enjo"         - author and maintainer  ender-sz !AT! go2 pl
//	 Copyright (C) 2008	 Mohd "Computerex" Ali  - contributor
//                   All rights reserved
//
// MFDData.cpp
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


#include "MFDData.hpp"

std::vector<std::string> v_bases;
std::vector<std::string> v_planets;
std::vector<std::string> v_xmlDataExtractor;

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


struct MemoryStruct {
char *memory;
size_t size;
};
MemoryStruct chunk;

std::string POST_data;

pthread_t tid;
bool thready = true;
bool thjoined = true;
char * tempbuf;

bool logged = false;
//bool stateRestored = false;
map <VESSEL *, int>  m_xfer_code;
int therror = 0;
bool basesIndexRead = false;

int gi = 0;
double gdb = 0;

MFDData::MFDData (VESSEL * vess) : item_index(0),multiplier(1),selectedPort(0),portChkFlag(0),menu_nr(nr_of_menu_items)
{
	int i;
	vessel = NULL;
	hSBody = hGBody = NULL;

	vessel = vess;
	
	if (vessel != NULL)
	{
		VESSELSTATUS2 vs2;
		memset(&vs2, 0, sizeof(VESSELSTATUS2));
		vs2.version = 2;
		vs2.flag = VS_DOCKINFOLIST | VS_FUELLIST;
		vs2.fuel = NULL;
		vs2.dockinfo = NULL;
		vessel->GetStatusEx(&vs2); 	 	
		
		for (i=0; i < vessel->GetPropellantCount(); i++)
		{
			PROPELLANT_HANDLE propellHandle = vessel->GetPropellantHandleByIndex (i);
			if (propellHandle)	
			{
				v_fuelMass.push_back(vessel->GetPropellantMass(propellHandle));
				v_prevFuelMass.push_back(vessel->GetPropellantMass(propellHandle));
				v_fuelMaxMass.push_back(vessel->GetPropellantMaxMass(propellHandle));		
			}
		}
		
		OBJHANDLE hSBody = NULL, hGBody = NULL; 
		ELEMENTS el,elG;
		ORBITPARAM prmG;
		memset (&el, 0,sizeof(el));
		memset (&elG, 0,sizeof(elG));
		memset (&prmG, 0,sizeof(prmG));

		hSBody = vessel->GetSurfaceRef();
		hGBody = vessel->GetGravityRef();
		
		if (hSBody) 	{
			vessel->GetElements(hSBody, el);
			prevEcc = el.e;
			vessel->GetPeDist(prevPeD);
		} 	else {	prevEcc = 0; prevPeD = 0; }
		if (hGBody) {
			vessel->GetElements(hGBody, elG, &prmG);
			prevEccG = elG.e;
			prevPeTG = prmG.PeT;	
		} else { prevEccG = 0; prevPeTG = 0; }

		prevStatus = vs2.status;
	
	
		// at first set all ports' flags as undocked
		portUsed.assign(vs2.ndockinfo, false);
		
		localBasePricesRead = false;	
		remoteBasePricesRead = true; // false only if selected
		POSTtransferRequest = false;
		POSTfuelRequest = false;
		statusLandedSent = statusLaunchedSent = leavingOrbitSent = true;
		enteringOrbitSent =  slingshotSent = powerSlingshotSent = true;
		
		leavingOrbit = enteringOrbit = reachingOrbit = powerSlinging = false;
		
		validationDone = shipValid = stateRestored = false;
		
		landedNearBase = false;		// just initialise. A scan is performed in Update
		prevTime = -5;
		
		payloadRead = shipValid = false;
	}
}

MFDData::~MFDData () {
		//if (item != NULL) delete [] item;
		//if (portUsed != NULL) delete [] portUsed;
		 }


int MFDData::XMLwriterTransfer(const std::string& planet, const std::string& basename)
{
	DB("MFDData::XMLwriterTransfer");
	if (!vessel) return -2;
	int rc = 0, result = 0, i=0;
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	// xmlChar *tmp;  // for encoding
	//FILE *fp;

	/* Create a new XML buffer, to which the XML document will be
	* written */
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {
		DB("Error creating the xml buffer\n");
		return -1;
	}

	/* Create a new XmlWriter for memory, with no compression.
	* Remark: there is no compression for this kind of xmlTextWriter */
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {
		DB("Error creating the xml writer\n");
		xmlBufferFree(xmlbuf);
		return -1;
	}

	/* Start the document with the xml default for the version,
	* encoding ISO 8859-1 and the default for the standalone
	* declaration. */
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n"); result = -1;}

	/* Start an element named "transfer". Since thist is the first
	* element, this will be the root element of the document. */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "transfer");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}


	/* Start an element named "vessel" as child of "transfer". */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "vessel");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}

	/* Write an element named "name" as child of "vessel". */
	
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "name",
		BAD_CAST vessel->GetName());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

	/* Write an element named "class" as child of "vessel". */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "class",
		BAD_CAST vessel->GetClassName());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

	/* Write an element named "load" as child of "vessel". */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "load",
		"%d", 111);
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

	/* Close the element named "vessel". */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n"); result = -1;}

	rc = xmlTextWriterStartElement(writer, BAD_CAST "location");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "planet",	BAD_CAST planet.c_str());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "basename",	BAD_CAST basename.c_str());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
	rc = xmlTextWriterEndElement(writer); // end location
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}

	/* Write an element named "products" as child of "transfer". */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "products");
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
	for( std::map<int, int>::iterator iter = m_order.begin(); iter != m_order.end(); iter++ ) 
	{
		/* Write an element named "product" as child of "products". */
		rc = xmlTextWriterStartElement(writer, BAD_CAST "product");
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

		/* Write an element named "id" as child of "product". */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "id",
			"%d", (*iter).first);
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

		/* Write an element named "amount" as child of "product". */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "amount","%d", (*iter).second);		 if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}

		/* Close the element named "product". */
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) {DB("Error at xmlTextWriterEndElement\n"); result = -1;}
	} // for


	/* Here we could close the elements "products" and "transfer" using the
	* function xmlTextWriterEndElement, but since we do not want to
	* write any other elements, we simply call xmlTextWriterEndDocument,
	* which will do all the work. */
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}

	xmlFreeTextWriter(writer);

	POST_data.assign((const char *) xmlbuf->content);

	xmlBufferFree(xmlbuf);
	return result;
}

int MFDData::XMLwriterFuel(const std::string& planet, const std::string& basename)
{
	DB("MFDData::XMLwriterFuel");
	if (!vessel) return -2;
	int rc, result = 0, i=0;
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	// xmlChar *tmp;  // for encoding
	//FILE *fp;

	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterFuel: Error creating the xml buffer\n"); return -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); return -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "fuel_transfer");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	
	rc = XMLwriterComponentVessel(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVessel\n");	result = -1;}
	rc = XMLwriterComponentLocation(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentLocation\n");	result = -1;}
	

	rc = xmlTextWriterStartElement(writer, BAD_CAST "order");
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
	for( std::map<int,double>::iterator iter = m_fuelOrder.begin(); iter != m_fuelOrder.end(); iter++ ) 
	{ 
		rc = xmlTextWriterStartElement(writer, BAD_CAST "tank");
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "normal");
		if (rc < 0) {DB("Error at xmlTextWriterStartAttribute\n"); result = -1;}

		ssbuf.str(std::string());
		ssbuf << (*iter).first+1;
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "tank_id", BAD_CAST ssbuf.str().c_str()) ;
		ssbuf.str(std::string());
		if (rc < 0) {DB("Error at xmlTextWriterStartAttribute\n"); result = -1;}
		
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tank_id","%d", (*iter).first+1);
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "amount","%f", (*iter).second);
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n"); result = -1;}
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) {DB("Error at xmlTextWriterEndElement\n"); result = -1;}
	} // for

	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}

	xmlFreeTextWriter(writer);

	POST_data.assign((const char *) xmlbuf->content);
	
	xmlBufferFree(xmlbuf);
	return result;
}

int MFDData::XMLwriterLogin(const std::string& username,const std::string& password)
{
DB("MFDData::XMLwriterLogin");
	int rc=0, result = 0;
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	//sbuf.assign("foo");
	//sbuf2.assign("bar");
	
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterLogin: Error creating the xml buffer\n"); result = -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); result = -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "login");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "username",	BAD_CAST username.c_str());
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	result = -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "password", BAD_CAST password.c_str());
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n"); result = -1;}
	rc = xmlTextWriterEndElement(writer); // end vessel
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}
	
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}
	
	xmlFreeTextWriter(writer);

	POST_data.assign((const char *) xmlbuf->content);
	
	xmlBufferFree(xmlbuf);
	return result;
}





int MFDData::XMLwriterStatusLanded()
{
	DB("MFDData::XMLwriterStatusLanded");
	if (!vessel || !hSBody || !hGBody) return -2;	
	int rc=0, result = 0, i=0;	
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterFuel: Error creating the xml buffer\n"); return -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); return -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "status");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mjd","%0.4f", oapiGetSimMJD());
	ssbuf.str(std::string());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");	result = -1;}
	
	rc = XMLwriterComponentVessel(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVessel\n");	result = -1;}
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "parameters");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "landed");
	if (rc < 0) {DB("Error at xmlTextWriterWriteAttribute\n");	result = -1;}
	rc = XMLwriterComponentLocation(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentLocation\n");	result = -1;}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}
	
	rc = XMLwriterComponentFuelReserves(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentFuelReserves\n");	result = -1;}
	
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}
	
	xmlFreeTextWriter(writer);
	POST_data.assign((const char *) xmlbuf->content);
	xmlBufferFree(xmlbuf);	
	return result;
}

int MFDData::XMLwriterStatusLaunched()
{
	DB("MFDData::XMLwriterStatusLaunched");
	if (!vessel || !hSBody || !hGBody) return -2;	
	int rc=0, result = 0, i=0;	
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterFuel: Error creating the xml buffer\n"); return -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); return -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "status");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mjd","%0.4f", oapiGetSimMJD());
	ssbuf.str(std::string());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");	result = -1;}
	
	rc = XMLwriterComponentVessel(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVessel\n");	result = -1;}
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "parameters");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "launched");
	if (rc < 0) {DB("Error at xmlTextWriterWriteAttribute\n");	result = -1;}
	rc = XMLwriterComponentLocation(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentLocation\n");	result = -1;}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}
	
	rc = XMLwriterComponentFuelReserves(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentFuelReserves\n");	result = -1;}
	
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}
	
	xmlFreeTextWriter(writer);
	POST_data.assign((const char *) xmlbuf->content);
	xmlBufferFree(xmlbuf);	
	return result;
}


int MFDData::XMLwriterStatusLeftOrbit(int type)
{
	DB("MFDData::XMLwriterStatusLeftOrbit");
	if (!vessel || !hSBody || !hGBody) return -2;	
	int rc=0, result = 0, i=0;	
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	ELEMENTS el; 
	ORBITPARAM prm;
	
	OBJHANDLE hSun = oapiGetGbodyByIndex(0);
	if (hSun == NULL) return -3;
	vessel->GetElements(hSun, el, &prm);
	
	
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterFuel: Error creating the xml buffer\n"); return -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); return -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "status");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mjd","%0.4f", oapiGetSimMJD());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");	result = -1;}
	
	rc = XMLwriterComponentVessel(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVessel\n");	result = -1;}
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "parameters");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}
	memset(buf, 0, BUFLEN);
	switch (type)
	{
		case POST_STATUS_LEFT_ORBIT:
			oapiGetObjectName(hSBody, buf, BUFLEN);
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "left_orbit");
		break;
		case POST_STATUS_SLINGED:
			oapiGetObjectName(hGBody, buf, BUFLEN);
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "slinged");
		break;
		case POST_STATUS_POWER_SLINGED:
			oapiGetObjectName(hGBody, buf, BUFLEN);
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "power_slinged");
		break;
		default:
		break;
	}
	if (rc < 0) {DB("Error at xmlTextWriterWriteAttribute\n");	result = -1;}	
	
	
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "planet",BAD_CAST buf);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	result = -1;}
	rc = XMLwriterComponentVectors(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVectors\n");	result = -1;}	
	rc = XMLwriterComponentElemens(writer, el, prm);
	if (rc < 0) {DB("Error at XMLwriterComponentElemens\n");	result = -1;}	
	rc = xmlTextWriterEndElement(writer); // end parameters
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}
	
	rc = XMLwriterComponentFuelReserves(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentFuelReserves\n");	result = -1;}
	
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}
	
	xmlFreeTextWriter(writer);
	POST_data.assign((const char *) xmlbuf->content);
	xmlBufferFree(xmlbuf);		
	return result;
}

int MFDData::XMLwriterStatusEnteredOrbit()
{
	DB("MFDData::XMLwriterStatusEnteredOrbit");
	if (!vessel || !hSBody || !hGBody) return -2;	
	int rc=0, result = 0, i=0;	
	xmlTextWriterPtr writer;
	xmlBufferPtr xmlbuf;
	ELEMENTS el;
	ORBITPARAM prm;
	
	OBJHANDLE hLocalBody = vessel->GetSurfaceRef();
	if (hLocalBody == NULL) return -3;
	vessel->GetElements(hLocalBody, el, &prm);
	
	xmlbuf = xmlBufferCreate();
	if (xmlbuf == NULL) {DB("XMLwriterFuel: Error creating the xml buffer\n"); return -1;}
	writer = xmlNewTextWriterMemory(xmlbuf, 0);
	if (writer == NULL) {DB("Error creating the xml writer\n"); xmlBufferFree(xmlbuf); return -1;}
	rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
	if (rc < 0) {DB("Error at xmlTextWriterStartDocument\n");	result = -1;}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "status");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mjd","%0.4f", oapiGetSimMJD());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");	result = -1;}
	
	rc = XMLwriterComponentVessel(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVessel\n");	result = -1;}
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "parameters");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	result = -1;}	
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "entered_orbit");
	if (rc < 0) {DB("Error at xmlTextWriterWriteAttribute\n");	result = -1;}
	memset(buf,'/0',BUFLEN);
	oapiGetObjectName(hSBody, buf, BUFLEN);
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "planet",BAD_CAST buf);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	result = -1;}
	rc = XMLwriterComponentVectors(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentVectors\n");	result = -1;}
	rc = XMLwriterComponentElemens(writer, el, prm);
	if (rc < 0) {DB("Error at XMLwriterComponentElemens\n");	result = -1;}
	rc = xmlTextWriterEndElement(writer); // end parameters
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	result = -1;}
	
	rc = XMLwriterComponentFuelReserves(writer);
	if (rc < 0) {DB("Error at XMLwriterComponentFuelReserves\n");	result = -1;}
	
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndDocument\n"); result = -1;}
	
	xmlFreeTextWriter(writer);
	POST_data.assign((const char *) xmlbuf->content);
	xmlBufferFree(xmlbuf);		
	return result;
}

int MFDData::XMLwriterComponentVessel(xmlTextWriterPtr & writer)
{
	DB("MFDData::XMLwriterComponentVessel");
	int rc = 0;
	if (!vessel) return -2;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "vessel");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "name",	BAD_CAST vessel->GetName());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");	return -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "class", BAD_CAST vessel->GetClassName());
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");return -1;}
	rc = xmlTextWriterEndElement(writer); // end vessel
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	return -1;}	
	return 0;
}

int MFDData::XMLwriterComponentLocation(xmlTextWriterPtr & writer)
{
	DB("MFDData::XMLwriterComponentLocation");
	if (!hSBody || !vessel) return -2;
	int rc = 0;
	memset(buf, '\0', BUFLEN);	
	// get current reference planet's name
	oapiGetObjectName(hSBody, buf, BUFLEN -1);
	
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "location");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
	if (landedNearBase) 
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "base");
	else 
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "void");
	if (rc < 0) {DB("Error at xmlTextWriterStartAttribute\n"); return -1;}
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "planet",	BAD_CAST buf);
	if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");return -1;}
	if (landedNearBase && vs2.base) {
		memset(buf2, '\0', BUFLEN);
		// get current reference base's name
		oapiGetObjectName(vs2.base, buf2, BUFLEN -1);
		rc = xmlTextWriterWriteElement(writer, BAD_CAST "basename",	BAD_CAST buf2);
		if (rc < 0) {DB("Error at xmlTextWriterWriteFormatElement\n");return -1;}
	}
	//else 
	{ 
		rc = xmlTextWriterStartElement(writer, BAD_CAST "equpos");
		if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "lng","%0.6f", vs2.surf_lng);
		if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "lat","%0.6f", vs2.surf_lat);
		if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "hdg","%0.3f", vs2.surf_hdg);
		if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
		rc = xmlTextWriterEndElement(writer); // end equpos
		if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	return -1;}	
	//
	}
	

	rc = xmlTextWriterEndElement(writer); // end location
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");	return -1;}
	return 0;
}

int MFDData::XMLwriterComponentFuelReserves(xmlTextWriterPtr & writer)
{
	DB("MFDData::XMLwriterComponentFuelReserves");
	if (!vessel) return -2;
	int rc = 0, i = 0;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "fuel");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");return -1;}
	for (i=0; i < vessel->GetPropellantCount(); i++)
	{
		
		rc = xmlTextWriterStartElement(writer, BAD_CAST "tank");
		if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
		rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "tank_id", "%d", i+1) ;
		if (rc < 0) {DB("Error at xmlTextWriterWriteAttribute\n");return -1;}
				
		PROPELLANT_HANDLE propellHandle = vessel->GetPropellantHandleByIndex (i);
		if (!propellHandle)	return -3;
		
		rc = xmlTextWriterWriteFormatString(writer, "%.2lf", vessel->GetPropellantMass(propellHandle));
				
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");return -1;}	
	} // for	
	return 0;
}

int MFDData::XMLwriterComponentElemens(xmlTextWriterPtr & writer, const ELEMENTS & el, const ORBITPARAM & prm)
{
	DB("MFDData::XMLwriterComponentElemens");
	int rc = 0;
	// Keplerian orbital elements (standard)
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "sma","%0.6f", el.a);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ecc","%0.6f", el.e);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "inc","%0.6f", el.i);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "lan","%0.6f", el.theta);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "lpe","%0.6f", el.omegab);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	// derrived elements
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ped","%0.0f", prm.PeD);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "apd","%0.0f", prm.ApD);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "pet","%0.0f", prm.PeT);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "apt","%0.0f", prm.ApT);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	return 0;
}

int MFDData::XMLwriterComponentVectors(xmlTextWriterPtr & writer)
{
	DB("MFDData::XMLwriterComponentVectors");
	if (!hSBody || !hGBody) return -2;	
	int rc = 0;
		
	rc = xmlTextWriterStartElement(writer, BAD_CAST "rpos");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "x","%0.6f", vs2.rpos.data[0]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "y","%0.6f", vs2.rpos.data[1]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "z","%0.6f", vs2.rpos.data[2]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");return -1;}	
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "rvel");
	if (rc < 0) {DB("Error at xmlTextWriterStartElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dx","%0.6f", vs2.rvel.data[0]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dy","%0.6f", vs2.rvel.data[1]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dz","%0.6f", vs2.rvel.data[2]);
	if (rc < 0) {DB("Error at xmlTextWriterWriteElement\n");	return -1;}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {DB("Error at xmlTextWriterEndElement\n");return -1;}	
	
	return 0;
}
	
xmlChar * MFDData::ConvertInput(const char *in, const char *encoding)
{
	DB("MFDData::ConvertInput");
	xmlChar *out;
	int ret;
	int size;
	int out_size;
	int temp;
	xmlCharEncodingHandlerPtr handler;

	if (in == 0)
		return 0;

	handler = xmlFindCharEncodingHandler(encoding);

	if (!handler) {
		printf("ConvertInput: no encoding handler found for '%s'\n",
			encoding ? encoding : "");
		return 0;
	}

	size = (int) strlen(in) + 1;
	out_size = size * 2 - 1;
	out = (unsigned char *) xmlMalloc((size_t) out_size);

	if (out != 0) {
		temp = size - 1;
		ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
		if ((ret < 0) || (temp - size + 1)) {
			if (ret < 0) {
				printf("ConvertInput: conversion wasn't successful.\n");
			} else {
				printf
					("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
					temp);
			}

			xmlFree(out);
			out = 0;
		} else {
			out = (unsigned char *) xmlRealloc(out, out_size + 1);
			out[out_size] = 0;  /*null terminating out */
		}
	} else {
		printf("ConvertInput: no mem\n");
	}

	return out;
}



int MFDData::ExtractXMLData(xmlChar *xpath, vector<string> * v_XMLData)
{
	DB("MFDData::ExtractXMLData");
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr xmlResult;
	int i;
	xmlChar *keyword; // will store node's data
	
	(* v_XMLData).clear();

	doc = xmlParseMemory(chunk.memory, chunk.size);
	if (doc == NULL)  return -1; // failed to parse (erratic XML?)

	xmlResult = getnodeset (doc, xpath);
	if (xmlResult) {
		nodeset = xmlResult->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			(* v_XMLData).push_back((const char *)keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject (xmlResult);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	if (xmlResult == NULL) return -2; // couldn't find expression
	return 0;
}

int MFDData::ExtractXMLData(xmlChar *xpath, vector<double> * v_XMLData)
{
	DB("MFDData::ExtractXMLData");
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr xmlResult;
	int i;
	xmlChar *keyword; // will store node's data
	
	(* v_XMLData).clear();

	doc = xmlParseMemory(chunk.memory, chunk.size);
	if (doc == NULL)  return -1; // failed to parse (erratic XML?)

	xmlResult = getnodeset (doc, xpath);
	if (xmlResult) {
		nodeset = xmlResult->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			(* v_XMLData).push_back(atof((const char *)keyword));
			xmlFree(keyword);
		}
		xmlXPathFreeObject (xmlResult);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	if (xmlResult == NULL) return -2; // couldn't find expression
	return 0;
}

int MFDData::ExtractXMLData(xmlChar *xpath, std::string * s_XMLData)
{
	DB("MFDData::ExtractXMLData");
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr xmlResult;
	int i;
	xmlChar *keyword; // will store node's data
	
	(* s_XMLData).clear();

	doc = xmlParseMemory(chunk.memory, chunk.size);
	if (doc == NULL)  return -1; // failed to parse (erratic XML?)

	xmlResult = getnodeset (doc, xpath);
	if (xmlResult) {
		nodeset = xmlResult->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			(* s_XMLData).assign((const char *)keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject (xmlResult);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	if (xmlResult == NULL) return -2; // couldn't find expression
	return 0;
}

int MFDData::ExtractXMLData(xmlChar *xpath, double * d_XMLData)
{
	DB("MFDData::ExtractXMLData");
	xmlDocPtr doc;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr xmlResult;
	int i;
	xmlChar *keyword; // will store node's data
	

	doc = xmlParseMemory(chunk.memory, chunk.size);
	if (doc == NULL)  return -1; // failed to parse (erratic XML?)

	xmlResult = getnodeset (doc, xpath);
	if (xmlResult) {
		nodeset = xmlResult->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++) {
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			(* d_XMLData) = atof((const char *)keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject (xmlResult);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	if (xmlResult == NULL) return -2; // couldn't find expression
	return 0;
}

xmlXPathObjectPtr MFDData::getnodeset (xmlDocPtr doc, xmlChar *xpath){

	DB("MFDData::getnodeset");

	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		//fprintf(gf,"Error in xmlXPathNewContext\n");
		//fflush(gf);
		return NULL;
	}


	result = xmlXPathEvalExpression(xpath, context);

	xmlXPathFreeContext(context);

	if (result == NULL) {
		//fprintf(gf,"Error in xmlXPathEvalExpression\n");
		//fflush(gf);
		return NULL;
	}

	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		//fprintf(gf,"No result\n");
		//fflush(gf);
		return NULL;
	}

	return result;
}

/*bool MFDData::getReference (xmlDocPtr doc, xmlNodePtr cur) {
	bool result = false;
	xmlChar *uri;
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"response"))) {
		    uri = xmlGetProp(cur, "status");
		    if (!xmlStrcmp(uri, (const xmlChar *)"ok"))
				result = true;
		    xmlFree(uri);
	    }
	    cur = cur->next;
	}
	return result;
}*/

int MFDData::PopulateStock(std::vector<std::string> * v_productName, std::vector<int> * v_productPrice, std::vector<int> * v_productId, std::vector<int> * v_productAmount)
{
	DB("MFDData::PopulateStock");
	int result = 0;		// assume a global faliure
	int i=0, price = 0, product_id = 0, amount = 0;

	xmlChar *xpath1 = (xmlChar*) "//name";
	xmlChar *xpath2 = (xmlChar*) "//unitprice";
	xmlChar *xpath3 = (xmlChar*) "//product_id";
	xmlChar *xpath4 = (xmlChar*) "//amount";


	//  extract text data from "name" node
	(*v_productName).clear();
	int xmlResult = ExtractXMLData (xpath1, v_productName);

	(*v_productPrice).clear();
	//v_xmlDataExtractor.clear();
	// next extract text data from "unitprice" node
	int xmlResult2 = ExtractXMLData (xpath2, &v_xmlDataExtractor);	
	if (xmlResult2 == 0) 
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{	
			std::stringstream priceConv(v_xmlDataExtractor.at(i)); 
			// and convert "unitprice" text data to a number		
			priceConv >> price;
			(*v_productPrice).push_back(price);
		}
	}
	
	(*v_productId).clear();
	//v_xmlDataExtractor.clear();
	int xmlResult3 = ExtractXMLData (xpath3, &v_xmlDataExtractor);
	if (xmlResult3 == 0) 
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{	
			std::stringstream idConv(v_xmlDataExtractor.at(i)); 
			idConv >> product_id;
			(*v_productId).push_back(product_id);
		}
	}
	
	(*v_productAmount).clear();
	//v_xmlDataExtractor.clear();
	int xmlResult4 = ExtractXMLData (xpath4, &v_xmlDataExtractor);
	if (xmlResult4 == 0) 
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{	
			std::stringstream amountConv(v_xmlDataExtractor.at(i)); 
			amountConv >> amount;
			(*v_productAmount).push_back(amount);
		}
	}
		
	int size = (*v_productName).size();

	// if all succeded
	if ((*v_productPrice).size() == size && (*v_productName).size() == size && (*v_productId).size() == size && (*v_productAmount).size() == size)
	{
		result = 1;
		// signal global sucess
	}

	return result;
}

int MFDData::PopulatePayload(std::vector<std::string> * v_productName, std::vector<int> * v_productAmount, std::vector<int> * v_productId)
{
	DB("MFDData::PopulateStock");
	int result = 0;		// assume a global faliure
	int i=0, price = 0, product_id = 0;

	xmlChar *xpath1 = (xmlChar*) "//name";
	xmlChar *xpath2 = (xmlChar*) "//amount";
	xmlChar *xpath3 = (xmlChar*) "//product_id";

	(*v_productName).clear();
	int xmlResult = ExtractXMLData (xpath1, v_productName);
	//if (!xmlResult) return 0;
	(*v_productAmount).clear();
	v_xmlDataExtractor.clear();
	int xmlResult2 = ExtractXMLData (xpath2, &v_xmlDataExtractor);	
	if (xmlResult2 == 0) 
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{	
			// convert string into integer
			std::stringstream priceConv(v_xmlDataExtractor.at(i)); 
			priceConv >> price;
			(*v_productAmount).push_back(price);
		}
	}	
	(*v_productId).clear();
	v_xmlDataExtractor.clear();
	int xmlResult3 = ExtractXMLData (xpath3, &v_xmlDataExtractor);
	if (xmlResult3 == 0)
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{		
			std::stringstream idConv(v_xmlDataExtractor.at(i)); 
			idConv >> product_id;
			(*v_productId).push_back(product_id);
		}
	}
	
	/*int xmlResult3 = ExtractXMLData (xpath3, &v_xmlDataExtractor);
	if (xmlResult3)
	{
		for (i=0; i< v_xmlDataExtractor.size(); i++)
		{		
			std::stringstream idConv(v_xmlDataExtractor.at(i)); 
			idConv >> product_id;
			(*v_productId).push_back(product_id);
		}
	}*/
	if (((*v_productName).size() == (*v_productAmount).size()) && ((*v_productName).size() == (*v_productId).size()))
	{
		cargoLoad = accumulate((*v_productAmount).begin(),(*v_productAmount).end(), 0);
		// signal global sucess
		result = 1;
		
	}

	return result;
}

static void * myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}


static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	DB("OrbitraderMFD::WriteMemoryCallback");
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

static size_t ReadMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	DB("OrbitraderMFD::ReadMemoryCallback");
	size_t length = size*nmemb;
	strncpy((char *)ptr,(const char *)data,length);
	return length;
} 

void *CurlThreadGET(void *arg)
{
		char * suffix;
		suffix = (char *) arg;
		DB((char *)suffix);
		char url[BUFLEN];
		memset(url, '\0', BUFLEN);
		strncpy(url, BASE_SERVER_ADDRESS, BUFLEN -1);
		strncat(url, suffix, BUFLEN - strlen(url) - 1);
	
		//DB(url);
		curl_easy_setopt(curl, CURLOPT_URL, url );
		
		// Callback to be called when there's data read
		// &OrbitraderMFD::WriteMemoryCallback to satisfy some compilers
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);
		// where to write the downloaded data
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		
		if (!curl_easy_perform(curl)) /* ignores error */
			thready = true;
			
   //curl_easy_cleanup(curl);
 
   return NULL;
 }

void *CurlThreadPOST(void *suffix)
{
  //CURL *curl;
		//if (chunk.memory) 	free(chunk.memory);
		//chunk.memory = NULL;
		//chunk.size=0;
char url[BUFLEN];
		memset(url, '\0', BUFLEN);
		strncpy(url, BASE_SERVER_ADDRESS, BUFLEN -1);
		strncat(url, (char *)suffix, BUFLEN - strlen(url) - 1);
		
		struct curl_slist *headers = NULL;
		// Callback to be called when there's data read. 
		// &OrbitraderMFD::WriteMemoryCallback to satisfy some compilers
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);
		// where to write the downloaded data
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		headers = curl_slist_append(headers, "Content-Type: text/xml");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);		
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POST_data.c_str());
		//curl_multi_add_handle(multi_handle, curl);
		//while(CURLM_CALL_MULTI_PERFORM == curl_multi_perform(multi_handle, &POST_still_running));
//		POST_started = 1; // setup transfer only once
  
		curl_easy_setopt(curl, CURLOPT_URL, url );
  
 if (!curl_easy_perform(curl)) /* ignores error */
			{ thready = true; POST_data.clear(); }
   //curl_easy_cleanup(curl);
 
   return NULL;
 }

void MFDData::GETLocalPrices()
{
	DB("MFDData::GETLocalPrices");
	int result = 1, error = 0;
	//char buf[BUFLEN],buf2[BUFLEN];
	//if (xfer_code == 0 || xfer_code == GET_LOCAL_BASE)
	if (m_xfer_code[vessel] == 0)
	{
		char * url = NULL;
		memset(buf, '\0', BUFLEN);
		memset(buf2, '\0', BUFLEN);
		strcpy(buf,"bases/view/");
		// get current reference body's name
		oapiGetObjectName(hSBody, buf2, BUFLEN);
		// convert spaces in name into %20, etc.
		url = curl_easy_escape(curl , buf2 , strlen(buf2));
		strcat(buf,url);
		// free memory after using it
		if (url != NULL) curl_free(url);
		strcat(buf,"/");
		memset(buf2, '\0', BUFLEN);
		// get current reference base's name
		oapiGetObjectName(vs2.base, buf2, BUFLEN);
		url = curl_easy_escape(curl , buf2 , strlen(buf2));
		strcat(buf,url);
		if (url != NULL) curl_free(url);		
		
		tempbuf = (char *) malloc (sizeof(char) * (strlen(buf)+1));
		memset(tempbuf,'\0',sizeof(tempbuf));
		strncpy(tempbuf,buf,strlen(buf));
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)tempbuf);
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		// lock other transfers
		xfer_code = GET_LOCAL_BASE;
		m_xfer_code[vessel] = GET_LOCAL_BASE;
		
	}
	// if data transfer succeeded
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_LOCAL_BASE)
	{	
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); free(tempbuf); thjoined = true; } 
		else DB("Error terminating thread;");
		
		int populateResult = 0;
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;
		// a function that gets data from XML 
		// and populates given vectors with this data
		populateResult = PopulateStock(&v_localProductName, &v_localProductPrice, &v_localProductId, &v_localProductAmount);
		// if everything suceeded, don't download again
		if (populateResult) localBasePricesRead = true;
		// and free the memory used in transfer
		if (chunk.memory) 	free(chunk.memory);
		// and prepare it for later use 
		chunk.memory = NULL;
		chunk.size=0;				
	}


}

void MFDData::GETBasesIndex()
{
	
	// simulate still_running
	int result = 1;
	int error = 0;
	// check if we have a go for a transfer
	//if (xfer_code == 0 || xfer_code == GET_BASES_INDEX)
	if (m_xfer_code[vessel] == 0)
	{
		DB("MFDData::GETBasesIndex");
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)"bases/index");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = GET_BASES_INDEX;	
		m_xfer_code[vessel] = GET_BASES_INDEX;
	}
	// if libCURL finished
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_BASES_INDEX)
	{ 
		DB("MFDData::GETBasesIndex");
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		xmlChar *xpath1 = (xmlChar*) "//basename";
		xmlChar *xpath2 = (xmlChar*) "//planet";

		int xmlResult = ExtractXMLData (xpath1, &v_bases);
		int xmlResult2 = ExtractXMLData (xpath2, &v_planets);
		// if extracting succeeded
		if (xmlResult == 0 && xmlResult2 == 0 && v_bases.size() == v_planets.size())
		{
			xfer_code = 0; // unlock other transfers
			m_xfer_code[vessel] = 0;
			// stores vector indices of unique planets
			// we have at least one planet and it's at index 1
			v_uniquePlanetsInd.assign(1,1);
			int i = 0;
			for (i=1; i < v_planets.size(); i++)
			{
				bool uniquePlanet = false;
				// if current is different then the previous
				if (v_planets.at(i).compare(v_planets.at(i-1)))
				{
					// then check if perhaps it's the same as one already stored (redundancy check)
					int j = 0;
					for (j=0; j < v_uniquePlanetsInd.size(); j++)
					{
						// if at least one the same planet is found in memory
						if (!v_planets.at(i).compare(v_planets.at(v_uniquePlanetsInd.at(j))))
						{
							uniquePlanet = false;
							break;
						}
						// found a unique planet
						else	 uniquePlanet = true;			
					}
				}
				if (uniquePlanet)
				{
					// store v_bases index of a newly found unique planet
					v_uniquePlanetsInd.push_back(i);
				}

			}
			nr_of_planets = v_uniquePlanetsInd.size();				
		}
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
		basesIndexRead = true; // read only once in the simulation		
	}	

}

void MFDData::GETRemotePrices()
{
	DB("MFDData::GETRemotePrices");
	int result = 1, error = 0;
	char buf[BUFLEN],buf2[BUFLEN];
	//if (xfer_code == 0 || xfer_code == GET_REMOTE_BASE)
	if (m_xfer_code[vessel] == 0)
	{
		char * url = NULL;
		memset(buf, '\0', BUFLEN);
		memset(buf2, '\0', BUFLEN);
		strncpy(buf,"bases/view/", BUFLEN -1);
		// get selected body's name
		const char * cptr = v_planets.at(v_basesAt1Planet.at(selected_base)).c_str();
		strncpy(buf2, cptr, BUFLEN -1);
		// convert spaces in name into %20, etc.
		url = curl_easy_escape(curl , buf2 , strlen(buf2));
		strncat(buf,url, BUFLEN -1 - strlen(buf));
		// free memory after using it
		if (url != NULL) curl_free(url);
		strcat(buf,"/");
		memset(buf2, '\0', BUFLEN);
		// get selected base's name
		const char * cptr2 = v_bases.at(v_basesAt1Planet.at(selected_base)).c_str();
		strncpy(buf2, cptr2, BUFLEN -1);
		url = curl_easy_escape(curl , buf2 , strlen(buf2));
		strncat(buf,url, BUFLEN -1 - strlen(buf));
		if (url != NULL) curl_free(url);
		//strcat(buf,"/");		
		/*string sbuf("bases/view/");
		v_planets.at(v_basesAt1Planet.at(selected_base)).c_str();
		sbuf += */
		tempbuf = (char *) malloc (sizeof(char) * (strlen(buf)+1));
		memset(tempbuf,'\0',sizeof(tempbuf));
		strncpy(tempbuf,buf,strlen(buf));
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)tempbuf);
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		
		xfer_code = GET_REMOTE_BASE;
		m_xfer_code[vessel] = GET_REMOTE_BASE;
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_REMOTE_BASE)
	{		
		//DB(chunk.memory);
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); free(tempbuf); thjoined = true; } 
		else DB("Error terminating thread;");
		
		int populateResult = 0;
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;

		populateResult = PopulateStock(&v_remoteProductName, &v_remoteProductPrice, &v_remoteProductId, &v_remoteProductAmount);

		if (populateResult) remoteBasePricesRead = true;

		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
	}
}

void MFDData::GETPayload()
{
	DB("MFDData::GETPayload");
	int result = 1, error = 0;
	//if (xfer_code == 0 || xfer_code == GET_PAYLOAD)
	if (m_xfer_code[vessel] == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)"payloads");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = GET_PAYLOAD;
		m_xfer_code[vessel] = GET_PAYLOAD;
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_PAYLOAD)
	{		
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		int populateResult = 0;
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;
		
		populateResult = PopulatePayload(&v_payloadName, &v_payloadAmount, &v_payloadId);

		if (populateResult) payloadRead = true;

		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
	}
}

void MFDData::GETLoginStatus()
{
	DB("MFDData::GETLoginStatus");
	int result = 1, error = 0;
	//if (xfer_code == 0 || xfer_code == GET_STATUS)
	if (m_xfer_code[vessel] == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)"users/status");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = GET_STATUS;
		m_xfer_code[vessel] = GET_STATUS;
		
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_STATUS)
	{
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;
		DB("Status:");
		DB(chunk.memory);
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
	}
}

void MFDData::GETValidate()
{
	DB("MFDData::GETValidate");
	int result = 1, error = 0;
	//if (xfer_code == 0 || xfer_code == GET_VALIDATE)
	if (m_xfer_code[vessel] == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)"ships/owned");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
      	xfer_code = GET_VALIDATE;
		m_xfer_code[vessel] = GET_VALIDATE;
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_VALIDATE)
	{	
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		xmlChar *xpath1 = (xmlChar*) "//name";
		xmlChar *xpath2 = (xmlChar*) "//class";
		int xmlResult = ExtractXMLData (xpath1, &s_shipName);
		int xmlResult2 = ExtractXMLData (xpath2, &s_shipClass);
		// if parsing succeeded
		if (xmlResult == 0 && xmlResult2 == 0 && vessel)	{
			if (!s_shipName.compare(vessel->GetName()) && !s_shipClass.compare(vessel->GetClassName()) ) 	{
			// FIXME !!!!
			//if (!s_shipName.compare(vessel->GetName())) 	
			{
				shipValid = true; 
				validationDone = true;
				if (error_code == INVALID_SHIP_ERR)  error_code = 0;
			}
			/*
			else	
			{	
				shipValid = false; 
				validationDone = true;
				error_code = INVALID_SHIP_ERR;
			}
			*/
			xfer_code = 0; // unlock other transfers
			m_xfer_code[vessel] = 0;
			}
		}
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
	}
}

void MFDData::GETRestore()
{
	DB("MFDData::GETRestore");
	int result = 1;
	//if (xfer_code == 0 || xfer_code == GET_RESTORE)
	if (m_xfer_code[vessel] == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadGET, (void *)"reports/last");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = GET_RESTORE;
		m_xfer_code[vessel] = GET_RESTORE;
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == GET_RESTORE)
	{	
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		//DB(chunk.memory);
		
		xmlChar *xptype = (xmlChar*) "/status/parameters/@type";
		xmlChar *xpmjd = (xmlChar*) "/status/mjd";	
		xmlChar *xpfuel = (xmlChar*) "/status/fuel/tank";
		
		string s_status, s_planet, s_base, s_locType;
		double db_data = 0;
		vector <double> v_fuel,v_rpos,v_rvel;
		int i = 0;
		OBJHANDLE h_planet = NULL;
		
		int xmlResult = ExtractXMLData (xptype, &s_status);
		if (xmlResult == 0) {
			xmlResult = ExtractXMLData (xpmjd, &db_data); // restore date
			if (xmlResult == 0) { oapiSetSimMJD(db_data); }
			
			xmlResult = ExtractXMLData (xpfuel, &v_fuel); // restore fuel reserves
			if (xmlResult == 0) {
				for (i=0; i<vs2.nfuel && i < v_fuel.size() && i < v_fuelMaxMass.size(); i++)	
					vs2.fuel[i].level = v_fuel.at(i) / v_fuelMaxMass.at(i);
			}
		
			// FIXME w left orbit ref to slonce !
			if (!s_status.compare("entered_orbit") || !s_status.compare("left_orbit")
				|| !s_status.compare("slinged") || !s_status.compare("power_slinged")) 
			{
				xmlChar *xpplanet = (xmlChar*) "/status/parameters/planet";
				xmlChar *xpx = (xmlChar*) "/status/parameters/rpos/x";
				xmlChar *xpy = (xmlChar*) "/status/parameters/rpos/y";
				xmlChar *xpz = (xmlChar*) "/status/parameters/rpos/z";
				xmlChar *xpdx = (xmlChar*) "/status/parameters/rvel/dx";
				xmlChar *xpdy = (xmlChar*) "/status/parameters/rvel/dy";
				xmlChar *xpdz = (xmlChar*) "/status/parameters/rvel/dz";
				int xmlResult = 0;
				
				//if (!s_status.compare("entered_orbit"))
				{
					xmlResult = ExtractXMLData (xpplanet, &s_planet); // restore reference planet
					if (xmlResult == 0) { 
						memset(buf,'\0',BUFLEN);
						strncpy(buf,s_planet.c_str(),BUFLEN);
						h_planet = oapiGetGbodyByName(buf);
						if (h_planet != NULL) 	vs2.rbody = h_planet;
					}
				}
				//else
				{
				//	OBJHANDLE hStar = oapiGetGbodyByIndex(0);
				//	if (hStar != NULL) 	vs2.rbody = hStar;
				}
				
				xmlResult = ExtractXMLData (xpx, &db_data); // restore state vectors
				if (xmlResult == 0) {	v_rpos.push_back(db_data);	}
				xmlResult = ExtractXMLData (xpy, &db_data);
				if (xmlResult == 0) {	v_rpos.push_back(db_data);	}
				xmlResult = ExtractXMLData (xpz, &db_data);
				if (xmlResult == 0) {	v_rpos.push_back(db_data);	}
				xmlResult = ExtractXMLData (xpdx, &db_data);
				if (xmlResult == 0) {	v_rvel.push_back(db_data);	}
				xmlResult = ExtractXMLData (xpdy, &db_data);
				if (xmlResult == 0) {	v_rvel.push_back(db_data);	}
				xmlResult = ExtractXMLData (xpdz, &db_data);	
				if (xmlResult == 0) {	v_rvel.push_back(db_data);	}
				try {
					vs2.rpos = _V(v_rpos.at(0),v_rpos.at(1),v_rpos.at(2));
					vs2.rvel = _V(v_rvel.at(0),v_rvel.at(1),v_rvel.at(2));
				}
				catch (exception & e) {	
					DBe("MFDData::GETRestore",e);
				}	
				
				vs2.status = 0;		// set active (in flight)
			}
			
			else if (!s_status.compare("landed") || !s_status.compare("launched")) 
			{
				
				xmlChar *xplocType = (xmlChar*) "/status/parameters/location/@type";
				xmlChar *xpplanetl = (xmlChar*) "/status/parameters/location/planet";
				xmlChar *xpbasel = (xmlChar*) "/status/parameters/location/basename";
				
				xmlResult = ExtractXMLData (xpplanetl, &s_planet); // restore reference planet
				if (xmlResult == 0) { 
				
					memset(buf,'\0',BUFLEN);
					strncpy(buf,s_planet.c_str(),BUFLEN);
					h_planet = oapiGetGbodyByName(buf);
					if (h_planet != NULL) 	 vs2.rbody = h_planet; 
				}
				
				xmlResult = ExtractXMLData (xplocType, &s_locType); // restore reference planet
				if (xmlResult == 0) 
				{ 
						xmlChar *xplng = (xmlChar*) "/status/parameters/location/equpos/lng";
						xmlChar *xplat = (xmlChar*) "/status/parameters/location/equpos/lat";
						xmlChar *xphdg = (xmlChar*) "/status/parameters/location/equpos/hdg";
						xmlResult = ExtractXMLData (xplng, &db_data); // restore state vectors
						if (xmlResult == 0) {	vs2.surf_lng = db_data;}
						xmlResult = ExtractXMLData (xplat, &db_data); // restore state vectors
						if (xmlResult == 0) {	vs2.surf_lat = db_data;}
						xmlResult = ExtractXMLData (xphdg, &db_data); // restore state vectors
						if (xmlResult == 0) {	vs2.surf_hdg = db_data;}
	
						if (!s_locType.compare("base"))
						{
							xmlResult = ExtractXMLData (xpbasel, &s_base); // restore reference base
							if (xmlResult == 0) {
								memset(buf,'\0',BUFLEN);
								strncpy(buf,s_base.c_str(),BUFLEN);
								if (h_planet != NULL) {
									OBJHANDLE h_base = oapiGetBaseByName(h_planet, buf);
									if (h_base != NULL) 	vs2.base = h_base;
								}
							}
						}
						else vs2.base = NULL;

				}
				
				vs2.status = 1;		// set inactive (landed)
			}
			
			vessel->DefSetStateEx(&vs2);
		
			xfer_code = 0; // unlock other transfers
			m_xfer_code[vessel] = 0;
			stateRestored = true;
		}

		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size=0;
	}
}

void MFDData::POSTLogin()
{
	DB("MFDData::POSTLogin");
	int xmlResult = 0, error = 0;
	// Only when we get an upload request, we'll create an XML but only once
	if (m_xfer_code[vessel] == 0)
	{
		xmlResult = XMLwriterLogin(username, password);
	}

	int result = 1;
	//if ( (xfer_code == 0 && xmlResult == 0) || xfer_code == POST_LOGIN)
	if (m_xfer_code[vessel] == 0 && xmlResult == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadPOST, (void *)"/users/login");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = POST_LOGIN;
		m_xfer_code[vessel] = POST_LOGIN;
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == POST_LOGIN)
	{	
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
	
		xmlChar *xpath1 = (xmlChar*) "//response/@status";
		s_loginStatus.clear();
		int xmlResult = ExtractXMLData (xpath1, &s_loginStatus);
		// if extracting succeeded
		if (xmlResult == 0)
		{	
			
			if (!s_loginStatus.compare("ok")) {
				logged = true; 
				error_code = 0;
			} else {
				logged = false;
				error_code = LOGIN_INCORRECT_ERR;
			}
		}
		POST_data.clear();
		// clean up the allocated data
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size = 0;
		xfer_code = 0; // unlock other transfers	
		m_xfer_code[vessel] = 0;
	}
}

void MFDData::POSTTransferRequest()
{
	DB("MFDData::POSTTransferRequest");
	// Only when we get an upload request, we'll create an XML but only once
	int xmlResult = 0;
	if (m_xfer_code[vessel] == 0)
	{
		memset(buf, '\0', BUFLEN);
		// get current reference planet's name
		oapiGetObjectName(hSBody, buf, BUFLEN -1);
		memset(buf2, '\0', BUFLEN);
		// get current reference base's name
		oapiGetObjectName(vs2.base, buf2, BUFLEN -1);

		//convert them into strings
		sbuf.assign(buf);
		sbuf2.assign(buf2);
		// and pass them to the XML writer
		xmlResult = XMLwriterTransfer(sbuf,sbuf2);
	}

	int result = 1;
	//if ( (xfer_code == 0 && xmlResult == 0) || xfer_code == POST_TRANSFER)
	if (m_xfer_code[vessel] == 0 && xmlResult == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadPOST, (void *)"/payloads/buy");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		xfer_code = POST_TRANSFER;
		m_xfer_code[vessel] = POST_TRANSFER;
		
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == POST_TRANSFER)
	{		
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;

		POST_data.clear();

		//temporary code - updates Stock FIXME
		for( std::map<int, int>::iterator iter = m_order.begin(); iter != m_order.end(); iter++ ) 
		{
			int i = 0;
			for (i=0; i < v_localProductId.size(); i++)
			{
				if (v_localProductId.at(i) == (*iter).first)
				{
					v_localProductAmount.at(i) -= (*iter).second;
				}
			}	
		}
		//temporary code
		m_order.clear();
		
		int populateResult = PopulatePayload(&v_payloadName, &v_payloadAmount, &v_payloadId);
		// free and clean up the allocated data
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size = 0;
		POSTtransferRequest = false;
	}
}

// TODO: make it real, and not just in-game
void MFDData::POSTFuelOrder()
{
	DB("MFDData::POSTFuelOrder");
	int xmlResult = 0;
	if (m_xfer_code[vessel] == 0)
	{
		memset(buf, '\0', BUFLEN);
		// get current reference planet's name
		oapiGetObjectName(hSBody, buf, BUFLEN -1);
		memset(buf2, '\0', BUFLEN);
		// get current reference base's name
		oapiGetObjectName(vs2.base, buf2, BUFLEN -1);

		//convert them into strings
		sbuf.assign(buf);
		sbuf2.assign(buf2);
		// and pass them to the XML writer
		xmlResult = XMLwriterFuel(sbuf,sbuf2);
		
		v_fuelMass.clear();
		int i=0;
		// update current fuel reserves
		for (i=0; i < vessel->GetPropellantCount(); i++)
		{
			PROPELLANT_HANDLE propellHandle = vessel->GetPropellantHandleByIndex (i);
			if (propellHandle) 
			{
				v_fuelMass.push_back(vessel->GetPropellantMass(propellHandle));
				vessel->SetPropellantMass(propellHandle, m_fuelOrder[i] + v_fuelMass.at(i));
				v_fuelMass.at(i) = (vessel->GetPropellantMass(propellHandle));
			}
		}
		POSTfuelRequest = false;
		m_fuelOrder.clear();
		money += moneyDiff;
		moneyDiff = 0;
	}
}

void MFDData::POSTStatus(int current_xfer_code, bool * statusFlag)
{
	DB("MFDData::POSTStatus");
	int xmlResult = 0, error = 0;
	if (m_xfer_code[vessel] == 0)
	{
		switch(current_xfer_code)
		{
			case POST_STATUS_LANDED:
				xmlResult = XMLwriterStatusLanded();
			break;
			case POST_STATUS_LAUNCHED:
				xmlResult = XMLwriterStatusLaunched();
			break;
			case POST_STATUS_LEFT_ORBIT:
				xmlResult = XMLwriterStatusLeftOrbit(POST_STATUS_LEFT_ORBIT);
			break;
			case POST_STATUS_ENTERED_ORBIT:
				xmlResult = XMLwriterStatusEnteredOrbit();
			break;
			case POST_STATUS_SLINGED:
				xmlResult = XMLwriterStatusLeftOrbit(POST_STATUS_SLINGED);
			break;
			case POST_STATUS_POWER_SLINGED:
				xmlResult = XMLwriterStatusLeftOrbit(POST_STATUS_POWER_SLINGED);
			break;
			default:
			break;		
		}
		DB(POST_data.c_str());
	}
	int result = 1;
	
	//if ((xfer_code == 0 && xmlResult == 0) || xfer_code == current_xfer_code )
	if (m_xfer_code[vessel] == 0 && xmlResult == 0)
	{
		thready = thjoined = false;
		therror = pthread_create(&tid, NULL, CurlThreadPOST, (void *)"/reports/file");
		if (therror) DB("Couldn't run thread;");
		else DB("Thread started");
		
		xfer_code = current_xfer_code;
		m_xfer_code[vessel] = current_xfer_code;
		
		//sprintf(oapiDebugString(), "result = %d",result);
	}	
	//if (result == 0) 
	if (thready && m_xfer_code[vessel] == current_xfer_code)
	{	
		therror = pthread_join(tid, NULL);
		if (!therror) { DB("Thread terminated"); thjoined = true; } 
		else DB("Error terminating thread;");
		
		xfer_code = 0; // unlock other transfers
		m_xfer_code[vessel] = 0;
		
		POST_data.clear();

		// free and clean up the allocated data
		if (chunk.memory) 	free(chunk.memory);
		chunk.memory = NULL;
		chunk.size = 0;
		*statusFlag = true;
	}	
}

/*double MFDData::CheckFuelMassIncrease()
{
	double v_fuelMassSum = 0, currFuelMassSum = 0, fuelDifference = 0;
	v_fuelMassSum = accumulate(v_fuelMass.begin(), v_fuelMass.end(), static_cast<double>(0));
	v_fuelMass.clear();
	int i=0;
	for (i=0; i < v_fuelMass.size(); i++)
	{
		PROPELLANT_HANDLE propellHandle = vessel->GetPropellantHandleByIndex (i);
		if (propellHandle) v_fuelMass.push_back(vessel->GetPropellantMass(propellHandle));
	}
	currFuelMassSum = accumulate(v_fuelMass.begin(), v_fuelMass.end(), static_cast<double>(0));
	fuelDifference = currFuelMassSum - v_fuelMassSum;
	//if (fuelDifference > 0) data->money -= fuelDifference / 10;
	return fuelDifference;
}*/

// FIXME Should detect total mass change as well after all, because someone could be performing cross-feed
void MFDData::RefuelBlocker()
{
	//if (CheckFuelMassIncrease()) gi = 1;
	//gdb = CheckFuelMassIncrease();
	//	(CheckFuelMassIncrease()<0) return;
	if (!vessel) return;
	PROPELLANT_HANDLE propellHandle;
	double prevFuelMassSum = 0, currFuelMassSum = 0, fuelDifference = 0;
	prevFuelMassSum = accumulate(v_prevFuelMass.begin(), v_prevFuelMass.end(), static_cast<double>(0));
	v_fuelMass.clear();
	int i=0;
	for (i=0; i < vessel->GetPropellantCount(); i++)
	{
		propellHandle = vessel->GetPropellantHandleByIndex (i);
		if (propellHandle) 	v_fuelMass.push_back(vessel->GetPropellantMass(propellHandle));
	}
	currFuelMassSum = accumulate(v_fuelMass.begin(), v_fuelMass.end(), static_cast<double>(0));
	fuelDifference = currFuelMassSum - prevFuelMassSum;
	
	if (fuelDifference > 0) // thou hast cheated !
	{
		for (i=0; i < v_fuelMass.size(); i++)
		{
			propellHandle = vessel->GetPropellantHandleByIndex (i);
			if (propellHandle) 
				vessel->SetPropellantMass(propellHandle, v_prevFuelMass.at(i));
		}	
	} 
	else	v_prevFuelMass.assign(v_fuelMass.begin(), v_fuelMass.end());
}

int MFDData::CheckOrbitStatus()
{
	ELEMENTS el, elG;
	ORBITPARAM prmG;
	const ATMCONST * atm;
	memset (&el, 0,sizeof(el));
	memset (&elG, 0,sizeof(elG));
	memset (&prmG, 0,sizeof(prmG));
	double PeD = 0, radlimit = 0;
	THRUSTER_HANDLE th = NULL;
	int status = 0;
	if (vessel == NULL) return -1;
	//hSBody = vessel->GetSurfaceRef();
	//hGBody = vessel->GetGravityRef(); // Get gravity dominant body
	if (hSBody == NULL || hGBody == NULL) return -2;
	vessel->GetElements(hSBody, el);			// elelemts w.r.t. surface reference body
	vessel->GetElements(hGBody, elG, &prmG);	// elelemts w.r.t. gravitational dominant reference body
	
	
	vessel->GetPeDist(PeD);
	atm = oapiGetPlanetAtmConstants(hSBody);
	
	if (atm == NULL) 	radlimit = oapiGetSize(hSBody);
	else 				radlimit = atm->radlimit;
	if (prevPeD <= radlimit)
	{
		if (PeD > radlimit)
			reachingOrbit = true;
	}
	
	if (reachingOrbit)
	{
		if (!vessel->GetThrusterGroupLevel(THGROUP_MAIN))
		{
			reachingOrbit = false;
			status = STATUS_ENTERED_ORBIT;
		}
	}
	
	if (prevEcc < 1)  // if haven't escaped orbit before (we were in an elliptic one)
	{
		if (el.e >= 1) 	// then ckeck if we started leaving it now
			leavingOrbit = true;
	} 
	else // if we're in a parabolic or hyberbolic orbit
	{
		if (el.e < 1)  // then ckeck if we started circulising it
			enteringOrbit = true;
	}

		//sprintf(oapiDebugString(),"Nought");
	
	if (leavingOrbit) 
	{
		// what if we use other? Ckeck which thrusters are fireing and then check if they stopped
		// if vessel stopped burning 
		if (!vessel->GetThrusterGroupLevel(THGROUP_MAIN))
		{
			leavingOrbit = false;
			status = STATUS_LEFT_ORBIT;
		}
	}
	else 
	{
		if (enteringOrbit)
		{
			// if vessel stopped burning 
			if (!vessel->GetThrusterGroupLevel(THGROUP_MAIN))
			{
				enteringOrbit = false;
				status = STATUS_ENTERED_ORBIT;
			}
		}	
		if (elG.e >= 1) { // if we're on in a parabolic or hyberbolic orbit 	
		if (prevPeTG > 0 && prmG.PeT < 0 ) { // if we've just passed periapsis
			if (!vessel->GetThrusterGroupLevel(THGROUP_MAIN)) // unpowered slingshot
				{ status = STATUS_SLINGED; }
			else if (prevEccG < elG.e) 	 // if our eccentricity is rising
				{ powerSlinging = true; }// powered 	slingshot 
			}				
		}		
		if (powerSlinging)
		{
			if (!vessel->GetThrusterGroupLevel(THGROUP_MAIN))
			{
				powerSlinging = false;
				status = STATUS_POWER_SLINGED;
			}
		}
	}
	
	prevEcc = el.e; 
	prevEccG = elG.e; 
	prevPeTG = prmG.PeT;
	prevPeD = PeD;
	return status;
}


void MFDData::CheckDockStatus()
{
	VESSEL * p_exchangeVessel = NULL;
	exchangePartner = NULL;
	// sanity check
	if (selectedPort >= vs2.ndockinfo) selectedPort = 0; 
	// if something is docked to our selected port
	if (vs2.dockinfo[selectedPort].rvessel != NULL)
	{
		p_exchangeVessel = oapiGetVesselInterface (vs2.dockinfo[selectedPort].rvessel);
		if (p_exchangeVessel != NULL) 
			exchangePartner = associateMFDData(p_exchangeVessel);
	}
	//data->portChkFlag = false;
	int i;
	// Docking ports scanning
	for (i=0; i < portUsed.size(); i++)
	{
		if (vs2.dockinfo[i].rvessel != NULL)
			portUsed.at(i) = true;
		else
		{
			portUsed.at(i) = false;
			if (i == selectedPort) portChkFlag = false;
		}
	}
}

