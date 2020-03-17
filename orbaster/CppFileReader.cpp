#include <exception>
#include <iostream>
#include <string>
#include <cstring>
#include "CppFileReader.h"

using namespace std;

CppFileReader::CppFileReader(const char * inifile)
{
	string line;
	ifstream ini;
	ini.open(inifile);
	if (ini.is_open())
	{
		v_file.clear();
		while (!ini.eof())
		{
			getline(ini, line);
			v_file.push_back(line);
		}
		ini.close();	
		
	}
	// FIXME	else 	load default vales
}

bool CppFileReader::LoadFile(const char * inifile)
{
	string line;
	ifstream ini;
	ini.open(inifile);
	if (ini.is_open())
	{
		v_file.clear();
		while (!ini.eof())
		{
			getline(ini, line);
			v_file.push_back(line);
		}
		ini.close();
		return true;
	}
	else
		return false;
}



double CppFileReader::LoadDouble(const char *inisection, const char *inikey)
{
	double inivalue = 0;
	stringstream ssbuf(Search(inisection, inikey));
	ssbuf >> inivalue;
	return inivalue;
}

int CppFileReader::LoadInt(const char *inisection, const char *inikey)
{
	int inivalue = 0;
	stringstream ssbuf(Search(inisection, inikey));
	for (int i = 0; i < ssbuf.str().size(); i++)
	{
		if (ssbuf.str().at(i) > '9' ||  ssbuf.str().at(i) < '0')
		return 0;
	}
	ssbuf >> inivalue;
	return inivalue;
}

int CppFileReader::LoadHex(const char *inisection, const char *inikey)
{
	int inivalue = 0;
	stringstream ssbuf(Search(inisection, inikey));
	for (int i = 0; i < ssbuf.str().size(); i++)
	{
		if (i == 0)
		{
			if (ssbuf.str().at(i) != '0')
			{
				return 0;
			}
		}
		else 
		if (i == 1)
		{
			if (ssbuf.str().at(i) != 'x' && ssbuf.str().at(i) != 'X')
			{
				return 0;
			}
		}		
		else
		if (i > 1) 
		{
			if (!(ssbuf.str().at(i) <= '9' &&  ssbuf.str().at(i) >= '0') &&
				!(ssbuf.str().at(i) <= 'F' &&  ssbuf.str().at(i) >= 'A') &&
				!(ssbuf.str().at(i) <= 'f' &&  ssbuf.str().at(i) > 'a'))
			{
				return 0;
			}
		}
		
	}
	ssbuf << hex;
	ssbuf >> inivalue;
	return inivalue;
}

bool CppFileReader::LoadBool(const char *inisection, const char *inikey)
{
	bool inivalue = 0;
	stringstream ssbuf(Search(inisection, inikey));
	ssbuf >> inivalue;
	return inivalue;
}


VECTOR3 CppFileReader::LoadVector(const char *inisection, const char *inikey)
{
	VECTOR3 v3;
	memset (&v3, 0, sizeof(VECTOR3));
	string retString = Search(inisection, inikey);
	if (retString.length() == 0) return v3;	
	else
		sscanf_s(retString.c_str(),"(%lf,%lf,%lf)",&v3.x,&v3.y,&v3.z);
	return v3;		
}

string CppFileReader::LoadString(const char *inisection, const char *inikey)
{
	string retString = Search(inisection, inikey);
	return retString;
}

string CppFileReader::Search(const char *inisection, const char *inikey)
{
	string sectionString, retString, line;
	sectionString = "[";
	sectionString += inisection;
	sectionString += "]";
	size_t keylen = strlen(inikey);
	for (int i=0; i < static_cast<int> (v_file.size()); i++)
	{
		try 
		{
			// if a comment found, skip to next line
			if (!v_file.at(i).compare(0,1, "#")) continue;
			// if section found
			if (!v_file.at(i).compare(sectionString))
			{
				for (int j=i+1; j < static_cast<int> (v_file.size()); j++)
				{
					// if section key found
					if (!v_file.at(j).compare(0, keylen , inikey))
					{
						// assign the value to a string. + 1 because we have "="
						retString.assign(v_file.at(j), keylen + 1, 
							line.length() - (keylen + 1));
						return retString;
					}
					 //if we're at next section, then it means that the inikey was not found
					if (line.find_first_of("[") == 0 
						&& line.find_last_of("]") == line.length())
						return retString;
				}
			}
		}
		catch (exception & e)
		{
			//sprintf(oapiDebugString(), "caught exception at Search: %s",e);
			//sprintf(oapiDebugString(), "v_filesize = %d",v_file.size());
			return retString;
		}
	}
	return retString;
}
