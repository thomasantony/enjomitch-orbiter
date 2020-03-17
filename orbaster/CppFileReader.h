#ifndef _H_ENJO_CPPFILEREADER
#define _H_ENJO_CPPFILEREADER

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "orbitersdk.h"

class CppFileReader
{
public:
	CppFileReader(const char * inifile);
	bool LoadFile(const char * inifile);
	double LoadDouble(const char *inisection, const char *inikey);
	int LoadInt(const char *inisection, const char *inikey);
	int LoadHex(const char *inisection, const char *inikey);
	bool LoadBool(const char *inisection, const char *inikey);
	VECTOR3 LoadVector(char *inisection, char *inikey);
	std::string LoadString(const char *inisection, const char *inikey);
private:
	std::string Search(const char *inisection, const char *inikey);
	std::vector <std::string> v_file;
};

#endif
