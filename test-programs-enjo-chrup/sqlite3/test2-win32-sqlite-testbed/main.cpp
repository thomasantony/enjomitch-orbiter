#include <iostream>
#include <windows.h>
#include <string>
#include <tchar.h>    //string and other mapping macros
#include <sstream>
#include "../common/sqlitemodule1.h"
//#include "../common/sqlite3.h"
#include "../common/WinGetVal.h"
#include "resources.h"

using namespace std;

static ustring sdefTableName = "users";

inline int ErrMsg(const ustring& s)
{
	return MessageBox(0,s.c_str(),_T("Error"),MB_OK|MB_ICONEXCLAMATION);
}

void insert_user(const HWND hwnd, const string & newUser)
{
  bool found = false;
  vector<string> result;
  ustring query;
  
  query = "select username from ";
  query += sdefTableName;
  query += ";";
  do_sql_colquery(query.c_str(), &result);
  
//  for (size_t i = 0; i < result.size(); i++)
  {
//	 if (result.at(i) == newUser) found = true;
  }
  if (!found)
  {
    query = "INSERT INTO ";
    query += sdefTableName;
    query += " VALUES (NULL, \"";
    query += newUser;
    query += "\");";
    //cout << "query = " << query << endl;
    //do_sql_colquery(query.c_str(), &result);
    SetDlgItemText(hwnd, ID_ED_QUERY, query.c_str());
  }
 // else
 //   SetDlgItemText(hwnd, ID_ED_OUTPUT, "User exists");
}

void delete_user(const HWND hwnd, const string & user)
{
/*
  bool found = false;
  vector<string> result;
  do_sql_colquery("select username from users;", &result);
  
  for (size_t i = 0; i < result.size(); i++)
  {
	 if (result.at(i) == user) found = true;
  }
  if (found)
  */
  {
  
    ustring query = "DELETE FROM ";
    query += sdefTableName;
    query += " WHERE username=\"";
    query += user;
    query += "\";";
    //cout << "query = " << query << endl;
    //do_sql_colquery(query.c_str(), &result);
    SetDlgItemText(hwnd, ID_ED_QUERY, query.c_str());
  }
}

void ConvertToOutput(const vector <string> & in, string * out)
{
 // if (in == NULL) return;
  for (size_t i = 0; i < in.size(); i++)
  {
    *out += in.at(i);
    *out += "\r\n";
  }
}
void OnCommandOK(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval, soutput, sret;
  vector <string> sresult;
    
  sval = wgv.GetValString(hwnd, ID_ED_QUERY);
  
  sret = do_sql_colquery(sval,&sresult);
  if (sret.empty()) // no error
  {
	  ConvertToOutput(sresult, &soutput);
	  SetDlgItemText(hwnd,ID_ED_OUTPUT,soutput.c_str());
  }
  else 
	  SetDlgItemText(hwnd,ID_ED_OUTPUT,sret.c_str());
  
  
}

void OnCommandInitDB(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval;
  
  sval = wgv.GetValString(hwnd, ID_ED_DB);
  if (sval.empty()) return;
  
  close_sqlite();
  initialise_sqlite(sval.c_str());
}

void OnCommandSetTable(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval;
  
  sval = wgv.GetValString(hwnd, ID_ED_TAB);
  if (sval.empty()) return;
  sdefTableName = sval;
}

void OnCommandGenTable(const HWND hwnd)
{
  ustring query = "CREATE TABLE ";
  query += sdefTableName;
  query += " (userid INTEGER PRIMARY KEY, username TEXT UNIQUE);";
  SetDlgItemText(hwnd, ID_ED_QUERY, query.c_str());
}

void OnCommandSelectAll(const HWND hwnd)
{
  ustring query = "SELECT * FROM ";
  query += sdefTableName;
  query += ";";
  SetDlgItemText(hwnd, ID_ED_QUERY, query.c_str());
}

void OnCommandGenQuery(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval;
  bool add = false, oneChecked = false;
  
  if (IsDlgButtonChecked(hwnd, ID_RB_ADD) == BST_CHECKED) 
	{
    oneChecked = true;
    add = true;
  }
	if (IsDlgButtonChecked(hwnd, ID_RB_REM) == BST_CHECKED)	
	{
    oneChecked = true;
    add = false;
  }
  if (!oneChecked) return;
  
  sval = wgv.GetValString(hwnd, ID_ED_USER);
  if (sval.empty()) return;
  
  if (add)  insert_user(hwnd, sval);
  else      delete_user(hwnd, sval);
}
/*
int main ()
{
  vector<string> result;
  initialise_sqlite("dbtest.db");
  do_sql_colquery("create table users(userid integer PRIMARY KEY, username text);", &result);
  //do_sql_colquery("insert into users values (2 , \"chrupek\")",&result);
  //do_sql_colquery("delete from users where user_id = 2;",&result);
  insert_user("enjo");
  insert_user("chrupek");
 
  //do_sql_colquery("select * from users;", &result);
  do_sql_colquery("select * from users;", &result);
  cout << "size = " << result.size() << endl;
  for (size_t i = 0; i < result.size(); i++)
  {
    cout << "Result "<< i <<" = " << result.at(i) << endl; 
  }
}
*/

BOOL CALLBACK DlgProcAbout(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:

        return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hwnd, IDOK);
                break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                break;
            }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK DlgProcUsage(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:

        return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hwnd, IDOK);
                break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                break;
            }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}

void OnCommand(const HWND hwnd,int id,int nCode,const HWND hCntrl)
{
	ustring s_error;

	//INT_PTR ptr_ret;
	
	switch (id)
	{
	//case IDFEATOK:
	 	case MNU_EXIT:
			DestroyWindow(hwnd);
		break;
		case MNU_LOAD:
		
		break;
		case MNU_SAVE:
		
		break;
		case MNU_GEN_DEF:
		
		break;
    case MNU_ABOUT:
      DialogBox(GetModuleHandle(NULL), 
                MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgProcAbout);
    break;
    case MNU_USAGE:
      DialogBox(GetModuleHandle(NULL), 
                MAKEINTRESOURCE(IDD_USAGE), hwnd, DlgProcUsage);
    break;
		case IDCANCEL:
		{
			DestroyWindow(hwnd);
			break;
		}
		case IDOK:
			OnCommandOK(hwnd);
		break;
    case ID_INIT_DB:
      OnCommandInitDB(hwnd);
    break;
    case ID_SET_TAB:
      OnCommandSetTable(hwnd);
    break;
    case ID_GEN_TAB:
      OnCommandGenTable(hwnd);
    break;
    case ID_SEL_ALL:
      OnCommandSelectAll(hwnd);
    break;
    case ID_GEN_QUERY:
      OnCommandGenQuery(hwnd);
    break;
  }
}

INT_PTR OnInitDlg(const HWND hwnd,LPARAM lParam)
{
  vector <string> result;
	SendMessage(hwnd,WM_SETICON,ICON_SMALL,
            reinterpret_cast<LPARAM>(LoadImage(0,IDI_APPLICATION,IMAGE_ICON,
                                               0,0,LR_SHARED)));
	//InitCombos(hwnd);
	//InitEdits(hwnd);
 
  SetDlgItemText(hwnd,ID_ED_DB,_T(("dbtest.db")));
  SetDlgItemText(hwnd,ID_ED_TAB,_T(("users")));
  initialise_sqlite("dbtest.db");
  do_sql_colquery("CREATE TABLE users(userid INTEGER PRIMARY KEY, username TEXT UNIQUE);", &result);
	PostMessage(hwnd,WM_KEYDOWN,VK_TAB,0);
	return TRUE;
}

INT_PTR CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			OnCommand(hwnd,LOWORD(wParam),HIWORD(wParam),
			              reinterpret_cast<HWND>(lParam));
			return 0;
		}
	case WM_DESTROY:
    close_sqlite();  // NOTE: Important !
		DestroyWindow(GetDlgItem(hwnd,IDD_DIALOG1));
		PostQuitMessage(0);
		return 0;
	case WM_INITDIALOG:
	{
		return OnInitDlg(hwnd,lParam);
	}
	default:
		return 0;
	}
}


int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
	HWND hDlg=CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG1),0,DlgProc);

	if (!hDlg)
	{
		ErrMsg(_T("CreateDialog failed."));
		return 0;
	}
	
	MSG msg;
	while (GetMessage(&msg,0,0,0)>0)
	{
		if (!IsWindow(hDlg) || !IsDialogMessage(hDlg,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}   
	}
	return static_cast<int>(msg.wParam);
}
