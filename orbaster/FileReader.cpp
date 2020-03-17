#include <windows.h>
#include "FileReader.h"

void FILEREADER::LoadInt(char *inifile, char *inisection, char *inikey, int &inivalue)
{
	inivalue = GetPrivateProfileInt((LPCTSTR)inisection,(LPCTSTR)inikey,inivalue,(LPCTSTR)inifile);
}
void FILEREADER::LoadDouble(char *inifile, char *inisection, char *inikey, double &inivalue)
{
	const DWORD IniSize = 255;
	DWORD IniReturn;
	char IniBuffer[IniSize+1];
	char dblBuffer[20];

	IniReturn = GetPrivateProfileString((LPCTSTR)inisection,(LPCTSTR)inikey,(LPCTSTR)"",(LPWSTR)IniBuffer,IniSize,(LPCTSTR)inifile);
	if (IniReturn > 0)
	{
		sscanf(IniBuffer,"%s",&dblBuffer);
		inivalue = atof(dblBuffer);
	}
}
void FILEREADER::LoadVector(char *inifile, char *inisection, char *inikey, VECTOR3 &inivalue)
{
	const DWORD IniSize = 255;
	DWORD IniReturn;
	char IniBuffer[IniSize+1];
	char dblBufferIni[60];
	char dblBufferX[20];
	char dblBufferY[20];
	char dblBufferZ[20];

	try
	{
		sprintf(dblBufferIni,"(%.8E,%.8E,%.8E)", inivalue.x,inivalue.y,inivalue.z);
	}
	catch (...)
	{
		dblBufferIni[0] = 0;
	}

	IniReturn = GetPrivateProfileString((LPCTSTR)inisection,(LPCTSTR)inikey,(LPCTSTR)dblBufferIni,(LPWSTR)IniBuffer,IniSize,(LPCTSTR)inifile);
	sscanf(IniBuffer,"(%[^','],%[^','],%[^')']",&dblBufferX,&dblBufferY,&dblBufferZ);
	inivalue.x = atof(dblBufferX);
	inivalue.y = atof(dblBufferY);
	inivalue.z = atof(dblBufferZ);
}

void FILEREADER::LoadString(char *inifile, char *inisection, char *inikey, char *inivalue)
{
	const DWORD IniSize = 255;
	DWORD IniReturn;
	char IniBuffer[IniSize+1];

	IniReturn = GetPrivateProfileString((LPCTSTR)inisection,(LPCTSTR)inikey,(LPCTSTR)inivalue,(LPWSTR)IniBuffer,IniSize,(LPCTSTR)inifile);
	strcpy(inivalue,IniBuffer);
}
