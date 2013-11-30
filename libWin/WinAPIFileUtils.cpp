#include "WinAPIFileUtils.hpp"
#include <Windows.h>

using namespace EnjoLib;

void WinAPIFileUtils::CreateDirs(const std::string & Path)
{
 char DirName[256];
 const char* p = Path.c_str();
 char* q = DirName;
 while(*p)
 {
   if (('\\' == *p) || ('/' == *p))
   {
     if (':' != *(p-1))
     {
        CreateDirectory(DirName, NULL);
     }
   }
   *q++ = *p++;
   *q = '\0';
 }
 CreateDirectory(DirName, NULL);
}
