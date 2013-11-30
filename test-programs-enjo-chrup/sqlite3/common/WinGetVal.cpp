#include <sstream>
#include "WinGetVal.h"

int WinGetVal::GetValInt(const HWND hwnd, int ID)
{
	int value;
	int len = GetWindowTextLength(GetDlgItem(hwnd, ID));
	if(len > 0)
	{
		int i;
		int ibuf;
		char* buf;
		
		buf = (char*)GlobalAlloc(GPTR, len + 1);
		GetDlgItemText(hwnd, ID, buf, len + 1);
		for (i=0; i<len; i++)
		{
			if ((buf[i] < 48 || buf[i] > 57) && (buf[i] != 45 && buf[i] != 46))
			{
				GlobalFree((HANDLE)buf);
				return 0;
				//return ERR_CHAR; // -2
			}
		}
		ibuf=atoi(buf);
		GlobalFree((HANDLE)buf);
		if (ibuf <= 0) { return 0;
				// return ERR_NEG_VAL;	//-3
				} 
		else { value = ibuf; 
			//return 0; // Success
		}
	}
	else {	return 0;
		//return ERR_EMPTY; // -1
		} 
	return value;
}

int WinGetVal::GetValHex(const HWND hwnd, int ID)
{
	int i_result;
	
	int len = GetWindowTextLength(GetDlgItem(hwnd, ID));
	if(len > 0)
	{
		char* buf;
		
		buf = (char*)GlobalAlloc(GPTR, len + 1);
		GetDlgItemText(hwnd, ID, buf, len + 1);
		
		stringstream ssbuf(buf);
		for (size_t i = 0; i < ssbuf.str().size(); i++)
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
		ssbuf >> i_result;
		GlobalFree((HANDLE)buf);
		
	}
	return i_result;
}

ustring WinGetVal::GetValString(const HWND hwnd, int ID)
{
	ustring s_result;
	int len = GetWindowTextLength(GetDlgItem(hwnd, ID));
	if(len > 0)
	{
		char* buf;
		
		buf = (char*)GlobalAlloc(GPTR, len + 1);
		GetDlgItemText(hwnd, ID, buf, len + 1);

		s_result.assign(_T(buf));
		GlobalFree((HANDLE)buf);
		
	}
	return s_result;
}
