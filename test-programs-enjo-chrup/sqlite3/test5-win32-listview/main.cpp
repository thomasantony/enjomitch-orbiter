#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <tchar.h>    //string and other mapping macros
#include <sstream>
#include <exception>
#include "../common/sqlitemodule1.h"
//#include "../common/sqlite3.h"
#include "../common/WinGetVal.h"
#include "resources.h"

//workers (surname TEXT, name TEXT, city TEXT, salary INTEGER NOT NULL, userid INTEGER PRIMARY KEY)

using namespace std;

static ustring sdefTableName = "users";

inline int ErrMsg(const ustring& s)
{
	return MessageBox(0,s.c_str(),_T("Error"),MB_OK|MB_ICONEXCLAMATION);
}

inline int Msg(const ustring& s)
{
	return MessageBox(0,s.c_str(),_T("Information"),MB_OK|MB_ICONWARNING);
}

inline int MsgOKCancel(const ustring& s)
{
	return MessageBox(0,s.c_str(),_T("Question"),MB_OKCANCEL|MB_ICONQUESTION);
}


int PrintVal(int val)
{
	char buf[512];
	sprintf(buf,"%d",val);
	return MessageBox(0, buf, "Debug", MB_OK);
}

void ListViewSelect(const HWND hwnd, const vector <string> & sresult)
{
  HWND hList = GetDlgItem(hwnd,ID_LV_ALL);
  SendMessage(hList,LVM_DELETEALLITEMS, 0, 0);
  LVITEM LvItem;
  memset(&LvItem,0,sizeof(LvItem)); // Zero struct's Members
  LvItem.mask=LVIF_TEXT;   // Text Style
  LvItem.cchTextMax = 256; // Max size of test
  char cTemp[255]={0};
  
  int i = 0, j = 0;
  //PrintVal(sresult.size());
  for (size_t vi = 0; vi < sresult.size(); vi++, j++)
  {
    if (j > 4) 
    {
      j = 0;
      i++;    
    }    
    LvItem.iItem = i;          // choose item  
    LvItem.iSubItem = j;       // Put in first coluom
    strncpy(cTemp, sresult.at(vi).c_str(), 255);
    LvItem.pszText = cTemp;
    if (j == 0)
      SendMessage(hList,LVM_INSERTITEM,0,(LPARAM)&LvItem);
    else
      SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Send info to the Listview
  }
}

void ListViewSelectAll(const HWND hwnd)
{
  vector <string> sresult; 
  do_sql_colquery("SELECT * FROM workers",&sresult);
  ListViewSelect(hwnd, sresult);
}

void ListViewQuery(const HWND hwnd, const string & query)
{
  vector <string> sresult; 
  do_sql_colquery(query,&sresult);
  ListViewSelect(hwnd, sresult);
}

/*
void insert_user(const HWND hwnd, const string & newUser)
{
  bool found = false;
  vector<string> result;
  ustring query;
  
  query = "select username from ";
  query += sdefTableName;
  query += ";";
  do_sql_colquery(query.c_str(), &result);
  
  for (size_t i = 0; i < result.size(); i++)
  {
	 if (result.at(i) == newUser) found = true;
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
  else
    SetDlgItemText(hwnd, ID_ED_OUTPUT, "User exists");
}

void delete_user(const HWND hwnd, const string & user)
{
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
*/
void InitListView(const HWND hwnd)
{

  HWND hList = GetDlgItem(hwnd,ID_LV_ALL);
  //ListView_SetExtendedListViewStyleEx(hList, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
  LVCOLUMN LvCol;
  memset(&LvCol,0,sizeof(LvCol));                  // Zero Members
  LvCol.mask=LVCF_TEXT|LVCF_WIDTH;    // Type of mask
  LvCol.cx=100;                                   // width between each coloum
  LvCol.pszText="Surname";                            // First Header Text
  LvCol.cchTextMax = 256;
  //SendMessage(hList,LVM_SETEXTENDEDLISTVIEWSTYLE,  0,LVS_EX_FULLROWSELECT); // Set style
           
           // Inserting Couloms as much as we want

 SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol); // Insert/Show the coloum
LvCol.pszText="Name";                            // Next coloum
SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol); // ...
LvCol.pszText="City";                            //
SendMessage(hList,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol); //
LvCol.pszText="Salary";                            //
SendMessage(hList,LVM_INSERTCOLUMN,3,(LPARAM)&LvCol); //
LvCol.pszText="ID";                            //
SendMessage(hList,LVM_INSERTCOLUMN,4,(LPARAM)&LvCol); //
ShowWindow(hList, SW_SHOW);

  ListViewSelectAll(hwnd);
//  Setting properties Of members:

/*
char Temp[255]="";

LvItem.pszText="Item 0"; // Text to display (can be from a char variable) (Items)

SendMessage(hList,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send info to the Listview

for(int i=1;i<=4;i++) // Add SubItems in a loop
{
   LvItem.iSubItem=i;
   sprintf(Temp,"SubItem %d",i);
   LvItem.pszText=Temp;
   SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems
}

LvItem.iItem=1;          // choose item  
LvItem.iSubItem=0;       // Put in first coluom
SendMessage(hList,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send info to the Listview
for(int i=1;i<=4;i++) // Add SubItems in a loop
{
   LvItem.iSubItem=i;
   sprintf(Temp,"SubItem %d",i);
   LvItem.pszText=Temp;
   SendMessage(hList,LVM_SETITEM,0,(LPARAM)&LvItem); // Enter text to SubItems
}
*/
}

/*
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
  ustring sval, soutput;
  vector <string> sresult;
    
  sval = wgv.GetValString(hwnd, ID_ED_QUERY);
  do_sql_colquery(sval,&sresult);
  
  ConvertToOutput(sresult, &soutput);
  
  SetDlgItemText(hwnd,ID_ED_OUTPUT,soutput.c_str());
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
  query += " (userid INTEGER PRIMARY KEY, username TEXT);";
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

void OnCommandAdd(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval;
  vector <string> sresult;
  
//"INSERT INTO workers VALUES ('Bjoork', 'Jurgen','Valhalla',99, NULL);"
  ustring query = "INSERT INTO workers VALUES ('";
  sval = wgv.GetValString(hwnd, ID_ED_SUR);
  query += sval;
  query += "', '";
  sval = wgv.GetValString(hwnd, ID_ED_NAM);
  query += sval;
  query += "', '";
  sval = wgv.GetValString(hwnd, ID_ED_CIT);
  query += sval;
  query += "', ";
  sval = wgv.GetValString(hwnd, ID_ED_SAL);
  query += sval;
  query += ", NULL);";
  
  // clear ID so that the user doesn't delete a different person
  SetDlgItemText(hwnd, ID_ED_ID, "");
  
  SetDlgItemText(hwnd, ID_ED_SUR, "");
  SetDlgItemText(hwnd, ID_ED_NAM, "");
  SetDlgItemText(hwnd, ID_ED_SAL, "");
  SetDlgItemText(hwnd, ID_ED_CIT, "");
  
  do_sql_colquery(query, &sresult);
  
  ListViewSelectAll(hwnd);
}

void OnCommandUpdate(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval;
  vector <string> sresult;
  
  // UPDATE workers SET city='Gory', salary=100 WHERE userid=10;
  ustring query = "UPDATE workers SET surname='";
  sval = wgv.GetValString(hwnd, ID_ED_SUR);
  query += sval;
  query += "', name='";
  sval = wgv.GetValString(hwnd, ID_ED_NAM);
  query += sval;
  query += "', city='";
  sval = wgv.GetValString(hwnd, ID_ED_CIT);
  query += sval;
  query += "', salary=";
  sval = wgv.GetValString(hwnd, ID_ED_SAL);
  query += sval;
  query += " WHERE userid=";
  sval = wgv.GetValString(hwnd, ID_ED_ID);
  query += sval;
  query += ";";
  
  do_sql_colquery(query, &sresult);
  
  ListViewSelectAll(hwnd);
}

void OnCommandDelete(const HWND hwnd)
{
  WinGetVal wgv;
  ustring sval, question;
  vector <string> sresult;
  
  //DELETE FROM users WHERE username="asd";
  
  sval = wgv.GetValString(hwnd, ID_ED_ID);
  if (sval.empty())  {
    Msg("Can't delete: ID field empty");
    return;
  }
  ustring query = "SELECT * FROM workers WHERE userid=";
  query += sval;
  query += ";";
  do_sql_colquery(query, &sresult);
  
  question = "Are you sure you want to delete worker\r\n";
  for (size_t i = 0; (i < sresult.size()) && (i < 3); i++)
  {
    question += sresult.at(i);
    question += " ";
  }
  question += "?";
  int result = MsgOKCancel(question); 
  if (result == 2) return;
    
  query = "DELETE FROM workers WHERE userid=";
  query += sval;
  query += ";";
  
  do_sql_colquery(query, &sresult);
  
  ListViewSelectAll(hwnd);
}

void OnCommandSearchFromFields(const HWND hwnd, const bool And)
{
  WinGetVal wgv;
  ustring surname, name, city, salary;
  vector <string> sresult;
  
  surname = wgv.GetValString(hwnd, ID_ED_SUR);
  name =    wgv.GetValString(hwnd, ID_ED_NAM);
  city =    wgv.GetValString(hwnd, ID_ED_CIT);
  salary =  wgv.GetValString(hwnd, ID_ED_SAL);
  //SELECT * FROM workers WHERE surname='Ender' AND name='Szymon' AND city='Oberhausen'
  ustring query = "SELECT * FROM workers WHERE ";
  
  if (surname.empty() && name.empty() && city.empty() && salary.empty())
  {
    Msg("All fields are empty");
    return;
  }
    
  if (!surname.empty()) {
    query += "surname='";
    query += surname;
    query += "'";
    if (!name.empty() || !city.empty() || !salary.empty())
    {
      if (And)
        query += " AND ";
      else
        query += " OR ";
    }
    else
      query += ";";
  }
  if (!name.empty())
  {
    query += "name='";
    query += name;
    query += "'";
    if (!city.empty() || !salary.empty())
    {
      if (And)
        query += " AND ";
      else
        query += " OR ";
    }
    else
      query += ";";
  }
  if (!city.empty())
  {
    query += "city='";
    query += city;
    query += "'";
    if (!salary.empty())
    {
      if (And)
        query += " AND ";
      else
        query += " OR ";
    }
    else
      query += ";";
  }
  if (!salary.empty())
  {
    query += "salary=";
    query += salary;
    query += ";";
  }
  //ErrMsg(query);
  
  ListViewQuery(hwnd, query);
  //do_sql_colquery(query, &sresult);
}

void OnCommandSearchALL(const HWND hwnd)
{
  ListViewSelectAll(hwnd);
}

void OnCommandSalaryFunctions(const HWND hwnd, const int id)
{
  vector <string> vsresult;
  //SELECT MAX(column_name) FROM table_name
  ustring query = "SELECT ";
  switch (id)
  {
    case ID_BT_SUM:
      query += "SUM(";
    break;
    case ID_BT_AVG:
      query += "AVG(";
    break;
    case ID_BT_CNT:
      query += "COUNT(";
    break;
    case ID_BT_MAX:
      query += "MAX(";
    break;
    case ID_BT_MIN:
      query += "MIN(";
    break;
    default:
      ErrMsg("Unknown code at OnCommandSalaryFunctions()");
      return;
    break;
  }
  
  query += "salary) FROM workers;";
  
  do_sql_colquery(query, &vsresult);
  if (!vsresult.empty())
  {
    SetDlgItemText(hwnd,ID_ED_OUT,vsresult.at(0).c_str());
    switch (id)
    {
      case ID_BT_MAX:
      case ID_BT_MIN:
      {
        string query = "SELECT * FROM workers WHERE salary=";
        query += vsresult.at(0);
        query += ";";
        ListViewQuery(hwnd, query);
      }
      break;
    }
  }
}

void OnCommandMoreLess(const HWND hwnd, const bool more)
{
  WinGetVal wgv;
  ustring sval;
  
  // SELECT * FROM workers WHERE salary>100  
  ustring query;
  if (more) {
    sval = wgv.GetValString(hwnd, ID_ED_MORE);
    query = "SELECT * FROM workers WHERE salary>";
  }
  else {
    sval = wgv.GetValString(hwnd, ID_ED_LESS);
    query = "SELECT * FROM workers WHERE salary<";
  } 
  
  if (sval.empty()) {
    Msg("The required value field is empty");
    return;
  }
  
  query += sval;
  query += ";";
  
  ListViewQuery(hwnd, query);
}

void OnCommandBetween(const HWND hwnd, const bool between)
{
  WinGetVal wgv;
  ustring svalMore, svalLess;
  
  // SELECT * FROM workers WHERE salary BETWEEN 100 AND 1000
  
  svalLess = wgv.GetValString(hwnd, ID_ED_LESS);
  svalMore = wgv.GetValString(hwnd, ID_ED_MORE);
  
  
  if (svalMore.empty() || svalLess.empty()) {
    Msg("Please supply both values");
    return;
  }
  
  ustring query;
  
  if (between)
    query = "SELECT * FROM workers WHERE salary BETWEEN ";
  else
    query = "SELECT * FROM workers WHERE salary NOT BETWEEN ";
    
  query += svalLess;
  query += " AND ";
  query += svalMore;
  query += ";";
  
  ListViewQuery(hwnd, query);
}

void OnCommand(const HWND hwnd,int id,int nCode,const HWND hCntrl)
{
	ustring s_error;

	//INT_PTR ptr_ret;
	//cout << "id = "<< id << " " << nCode << " " << hCntrl << endl;
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
    case ID_BT_ADD:
      OnCommandAdd(hwnd);
    break;
    case ID_BT_UPD:
      OnCommandUpdate(hwnd);
    break;
    case ID_BT_DEL:
      OnCommandDelete(hwnd);
    break;
    case ID_BT_SFIELDS_AND:
      OnCommandSearchFromFields(hwnd, true);
    break;
    case ID_BT_SFIELDS_OR:
      OnCommandSearchFromFields(hwnd, false);
    break;
    case ID_BT_SALL:
      OnCommandSearchALL(hwnd);
    break;
    case ID_BT_SUM:
    case ID_BT_AVG:
    case ID_BT_CNT:
    case ID_BT_MAX:
    case ID_BT_MIN:
      OnCommandSalaryFunctions(hwnd, id);
    break;
    case ID_BT_MORE:
      OnCommandMoreLess(hwnd, true);
    break;
    case ID_BT_LESS:
      OnCommandMoreLess(hwnd, false);
    break;
    case ID_BT_BTW:
      OnCommandBetween(hwnd, true);
    break;
    case ID_BT_NBTW:
      OnCommandBetween(hwnd, false);
    break;
    
    /*
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
    */
  }
}

void OnNotify(const HWND hwnd,int id, int nCode, const HWND hCntrl)
{
  //cout << "nCode = " << nCode << ", NM_RETURN = " << NM_RETURN << endl;
  switch (id)
  {
    case ID_LV_ALL: 
    
      switch (nCode)
      {
        // NM_RETURN only works when the mouse button is pressed. Weird
        case NM_RETURN:
        case NM_CLICK:
        
          HWND hList = GetDlgItem(hwnd,ID_LV_ALL);
          //SetFocus(GetDlgItem(hwnd,IDFEATOK)); 
          SetFocus(hList); 
          int iSlected=SendMessage(hList,LVM_GETNEXTITEM,(WPARAM)-1,LVNI_FOCUSED);
          //cout << "iSlected = " <<iSlected<< endl;

          if(iSlected==-1)
          {
            // MessageBox(hwnd,"No Items in ListView",  "Error",MB_OK|MB_ICONINFORMATION);
            break;
          }
          LVITEM LvItem;
          char Text[255]={0};
          //char Temp[255]={0};
          //char Temp1[255]={0};
          int j=0;

          memset(&LvItem,0,sizeof(LvItem));
          LvItem.mask=LVIF_TEXT;
          LvItem.iSubItem=0;
          LvItem.pszText=Text;
          LvItem.cchTextMax=256;
          LvItem.iItem=iSlected;

          SendMessage(hList,LVM_GETITEMTEXT, 
              iSlected, (LPARAM)&LvItem); 

          //sprintf(Temp1,Text);
          SetDlgItemText(hwnd, ID_ED_SUR, Text);

          for(j = 1; j < 5; j++)
          {
            LvItem.iSubItem=j;
            SendMessage(hList,LVM_GETITEMTEXT, iSlected, (LPARAM)&LvItem);
            //sprintf(Temp," %s",Text);
            //lstrcat(Temp1,Temp);
            switch (j)
            {
              case 1:
                SetDlgItemText(hwnd, ID_ED_NAM, Text);
              break;
              case 2:
                SetDlgItemText(hwnd, ID_ED_CIT, Text);
              break;
              case 3:
                SetDlgItemText(hwnd, ID_ED_SAL, Text);
              break;
              case 4:
                SetDlgItemText(hwnd, ID_ED_ID, Text);
              break;
              default:
              break;
            }

          }
        break;
      }
    break;
  }
}

INT_PTR OnInitDlg(const HWND hwnd,LPARAM lParam)
{
  vector <string> vsresult;
	SendMessage(hwnd,WM_SETICON,ICON_SMALL,
            reinterpret_cast<LPARAM>(LoadImage(0,IDI_APPLICATION,IMAGE_ICON,
                                               0,0,LR_SHARED)));
  initialise_sqlite("lview.db");
  do_sql_colquery("CREATE TABLE workers (surname TEXT, name TEXT, city TEXT, salary INTEGER NOT NULL, userid INTEGER PRIMARY KEY;", &vsresult);
	//InitCombos(hwnd);
	//InitEdits(hwnd);
  InitListView(hwnd);
  //SetDlgItemText(hwnd,ID_ED_DB,_T(("dbtest.db")));
  //SetDlgItemText(hwnd,ID_ED_TAB,_T(("users2")));
  
  
	PostMessage(hwnd,WM_KEYDOWN,VK_TAB,0);
  
	return TRUE;
}

INT_PTR CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
    
  //if (Msg == WM_GETDLGCODE)
		//return DLGC_WANTALLKEYS;

    case WM_NOTIFY:
    {
      OnNotify(hwnd, ((LPNMHDR)lParam)->idFrom, ((LPNMHDR)lParam)->code,
                reinterpret_cast<HWND>(lParam));
      
      /*
      switch (((LPNMHDR)lParam)->code)
      {
          case NM_CLICK:
          
            if (((LPNMHDR)lParam)->idFrom == ID_LV_ALL)
            {
              
               return TRUE;
            }
            break;
          case NM_KEYDOWN:
            cout << "Keydown" << endl;
          break;
          case NM_RETURN:
            cout << "here" << endl;
          break;         
      }  
      */
      return 0;
    }
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
  // NOTE: MUST be called before CreateDialog()
  InitCommonControls();
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
