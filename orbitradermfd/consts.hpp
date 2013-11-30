// ==============================================================
//               ORBITER MODULE: OrbitraderMFD
//                  Part of the ORBITER SDK
// 	 Copyright (C) 2007         Simon Ender "Enjo"         - author and maintainer  ender-sz !AT! go2 pl
//	 Copyright (C) 2008	 Mohd "Computerex" Ali  - contributor
//                   All rights reserved
//
// consts.hpp
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


#define BASE_SERVER_ADDRESS "http://shedalier.net/zabawki/cake/rest/"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <pthread.h>
#include <curl/curl.h>
#define BUFLEN 512

using namespace std;

class MFDData;

//error codes
extern const unsigned int LOGIN_INCORRECT_ERR;
extern const unsigned int INVALID_SHIP_ERR;


/*
// transfer codes
extern const unsigned int GET_BASES_INDEX;
extern const unsigned int GET_REMOTE_BASE;
extern const unsigned int GET_LOCAL_BASE;
extern const unsigned int GET_PAYLOAD;
extern const unsigned int GET_STATUS;
extern const unsigned int GET_VALIDATE;
extern const unsigned int GET_RESTORE;
extern const unsigned int POST_LOGIN;
extern const unsigned int POST_TRANSFER;
extern const unsigned int POST_FUEL_ORDER;
extern const unsigned int POST_STATUS_LANDED;
extern const unsigned int POST_STATUS_LAUNCHED;
extern const unsigned int POST_STATUS_LEFT_ORBIT;
extern const unsigned int POST_STATUS_ENTERED_ORBIT;
extern const unsigned int POST_STATUS_SLINGED;
extern const unsigned int POST_STATUS_POWER_SLINGED;

// orbital statuses
extern const unsigned int STATUS_LEFT_ORBIT;
extern const unsigned int STATUS_ENTERED_ORBIT;
extern const unsigned int STATUS_SLINGED;
extern const unsigned int STATUS_POWER_SLINGED;
*/
/*
extern enum xferCodes {
	GET_BASES_INDEX, 
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
extern enum orbStatuses {
	STATUS_LEFT_ORBIT,
	STATUS_ENTERED_ORBIT,
	STATUS_SLINGED,
	STATUS_POWER_SLINGED
};
*/
extern std::vector<MFDData*> v_MFDdata;

extern const unsigned int nr_of_menu_items;
extern const unsigned int nr_of_hidden_menu_items;
extern const unsigned int nr_of_main_menu_items;

extern CURL *curl;
extern std::string username;
extern std::string password;
extern bool stateRestored;

extern pthread_t tid;
extern bool thready;
extern bool thjoined;
extern char * tempbuf;

extern bool logged;
//extern bool stateRestored;
extern int xfer_code;
extern int error_code;
extern bool basesIndexRead;

extern std::vector<std::string> v_bases;
extern std::vector<std::string> v_planets;
extern std::vector<std::string> v_xmlDataExtractor;

extern int gi;
extern double gdb;

extern MFDData * associateMFDData(VESSEL * vessel);

extern std::ofstream fdb; 
extern void inline DB(const string&);
extern void inline DBe(const char* s, exception & e);






