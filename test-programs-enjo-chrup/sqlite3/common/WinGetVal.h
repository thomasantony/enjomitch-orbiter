#include <windows.h>
#include <string>
#include <tchar.h>

using namespace std;

typedef std::basic_string<TCHAR> ustring;
//typedef wstring ustring;

class WinGetVal
{
  public:
    int GetValInt(const HWND hwnd, int ID);
    int GetValHex(const HWND hwnd, int ID);
    ustring GetValString(const HWND hwnd, int ID);
};
