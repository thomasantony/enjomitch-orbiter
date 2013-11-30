#include "myListCtrl.h"
#include "globals.h"
#include "test.h"
#include "customdialog.h"


using namespace std;


MyListCtrl::MyListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,  const wxSize& size, long style, wxSQLite3Database * db)
  : wxListCtrl(parent, id, pos, size, style) 
{
  m_parent = parent;
  m_userid = 0;
  m_db = new wxSQLite3Database; 
  try {
    m_db->Open(DBFILE);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::ProjListCtrl() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  
  Connect(ID_LISTBOX, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(MyListCtrl::OnSelected));
  Connect(ID_LISTBOX, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(MyListCtrl::OnActivated));
}

MyListCtrl::~MyListCtrl()
{
  m_db->Close();
  delete m_db;
}

void MyListCtrl::OnSelected(wxListEvent& event)
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
 // puts("On Selected");
  wxASSERT(ident);
  if (ident == NULL) return;
  
  //wxBell();

  m_userid = GetItemData(event.m_itemIndex);
  wxListItem info;
  info.m_itemId = event.m_itemIndex;
  info.m_col = 0;
  info.m_mask = wxLIST_MASK_TEXT;
  
  
  //ident->m_text->SetLabel(wxString::Format(wxT("%d"), count));

  if ( GetItem(info) )
  {
    ident->m_textSurname->Clear();
    ident->m_textSurname->WriteText(info.m_text);
  }
  
  info.m_col ++;
  if ( GetItem(info) )
  {
    ident->m_textName->Clear();
    ident->m_textName->WriteText(info.m_text);
  }
  info.m_col ++;
  if ( GetItem(info) )
  {
    ident->m_textCity->Clear();
    ident->m_textCity->WriteText(info.m_text);
  }
  info.m_col ++;
  if ( GetItem(info) )
  {
    ident->m_textSalary->Clear();
    ident->m_textSalary->WriteText(info.m_text);
  }
  

}


void MyListCtrl::OnActivated(wxListEvent& event)
{

  CustomDialog *custom = new CustomDialog(this, wxT("Assignments"), m_db, m_userid);
  custom->Show(true);
}

void MyListCtrl::Add()
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString buf;
  
//"INSERT INTO workers VALUES ('Bjoork', 'Jurgen','Valhalla',99, NULL);"
  wxString query = _T("INSERT INTO workers VALUES ('");
  query += ident->m_textSurname->GetLineText(0);
  query += _T("', '");
  query += ident->m_textName->GetLineText(0);
  query += _T("', '");
  query += ident->m_textCity->GetLineText(0);
  query += _T("', ");
  query += ident->m_textSalary->GetLineText(0);
  query += _T(", NULL);");
  
  ident->m_textSurname->Clear();
  ident->m_textName->Clear();
  ident->m_textCity->Clear();
  ident->m_textSalary->Clear();
  
  UpdateQuery(query);
  SelectAll();
}

void MyListCtrl::Update()
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString buf;
  wxString query = _T("UPDATE workers SET surname='");
  
  query += ident->m_textSurname->GetLineText(0);
  query += _T("', name='");
  query += ident->m_textName->GetLineText(0);
  query += _T("', city='");
  query += ident->m_textCity->GetLineText(0);
  query += _T("', salary=");
  query += ident->m_textSalary->GetLineText(0);
  query += _T(" WHERE userid=");
  buf.Printf(_T("%d"), m_userid);
  query += buf + _T(";");
  
  //cout << (const char*)(query.mb_str()) << endl;
  UpdateQuery(query);
  SelectAll();
}

void MyListCtrl::Delete()
{
  wxString buf, question;
  wxSQLite3ResultSet q;
  //DELETE FROM users WHERE userid=1;
  
  wxString query = _T("SELECT * FROM workers WHERE userid=");
  buf.Printf(_T("%d"), m_userid);
  query += buf + _T(";");
  try 
  {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    q = m_db->ExecuteQuery(query);
    if (!q.NextRow()) return;
    
    question = _T("Are you sure you want to delete worker\r\n");
    for (int i = 0; i < NUM_COLUMNS - 1; i++)
    {
      question += q.GetString(i) + _T(" ");
    }
    question += _T("?");
    int result = wxMessageBox(question, _T("question"), wxYES_NO); 
    if (result == wxNO) return;
      
    query = _T("DELETE FROM workers WHERE userid=");
    buf.Printf(_T("%d"), m_userid);
    query += buf + _T(";");
    UpdateQuery(query);
    
    query = _T("DELETE FROM assignments WHERE userid=");
    buf.Printf(_T("%d"), m_userid);
    query += buf + _T(";");
    UpdateQuery(query);
    
    SelectAll();
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::Delete() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void MyListCtrl::SearchFromFields(const bool And)
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;

  wxString surname, name, city, salary;
  
  surname = ident->m_textSurname->GetLineText(0);
  name =    ident->m_textName->GetLineText(0);
  city =    ident->m_textCity->GetLineText(0);
  salary =  ident->m_textSalary->GetLineText(0);
  //SELECT * FROM workers WHERE surname='Ender' AND name='Szymon' AND city='Oberhausen'
  wxString query = _T("SELECT * FROM workers WHERE ");
  
  if (surname.empty() && name.empty() && city.empty() && salary.empty())
  {
    wxMessageBox(_T("All fields are empty"));
    return;
  }
    
  if (!surname.empty()) {
    query += _T("surname='");
    query += surname;
    query += _T("'");
    if (!name.empty() || !city.empty() || !salary.empty())
    {
      if (And)
        query += _T(" AND ");
      else
        query += _T(" OR ");
    }
    else
      query += _T(";");
  }
  if (!name.empty())
  {
    query += _T("name='");
    query += name;
    query += _T("'");
    if (!city.empty() || !salary.empty())
    {
      if (And)
        query += _T(" AND ");
      else
        query += _T(" OR ");
    }
    else
      query += _T(";");
  }
  if (!city.empty())
  {
    query += _T("city='");
    query += city;
    query += _T("'");
    if (!salary.empty())
    {
      if (And)
        query += _T(" AND ");
      else
        query += _T(" OR ");
    }
    else
      query += _T(";");
  }
  if (!salary.empty())
  {
    query += _T("salary=");
    query += salary;
    query += _T(";");
  }
  //ErrMsg(query);
  
  SelectQuery(query);
  //do_sql_colquery(query, &sresult);
}

void MyListCtrl::Functions(wxCommandEvent & event)
{
  try {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    
    //SELECT MAX(column_name) FROM table_name
    wxString query = _T("SELECT ");
    switch (event.GetId())
    {
      case ID_SUM:
        query += _T("SUM(");
        break;
      case ID_AVERAGE:
        query += _T("AVG(");
        break;
      case ID_COUNT:
        query += _T("COUNT(");
        break;
      case ID_MAX:
        query += _T("MAX(");
        break;
      case ID_MIN:
        query += _T("MIN(");
        break;
      default:
        wxMessageBox(_T("Unknown code at OnCommandSalaryFunctions()"));
        return;
        break;
    }
    
    query += _T("salary) FROM workers;");
    
    
    int iresult = m_db->ExecuteScalar(query);
    
    Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
    wxASSERT(ident);
    if (ident == NULL) return;
    
    wxString buf;
    buf.Printf(_T("%d"),iresult);
    ident->m_textResult->Clear();
    ident->m_textResult->WriteText(buf);
    
    switch (event.GetId())
    {
      case ID_MAX:
      case ID_MIN:
      {
        wxString query = _T("SELECT * FROM workers WHERE salary=");
        query += buf + _T(";");
        SelectQuery(query);
      }
      break;
      default:
        break;
      }
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::Functions() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void MyListCtrl::MoreLess(wxCommandEvent & event)
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString buf;
  
  // SELECT * FROM workers WHERE salary>100  
  wxString query;
  switch (event.GetId())
  {
    case ID_LESS:
      buf = ident->m_textLess->GetLineText(0);
      query = _T("SELECT * FROM workers WHERE salary<");
    break;
    case ID_MORE:
      buf = ident->m_textMore->GetLineText(0);
      query = _T("SELECT * FROM workers WHERE salary>");
    break;
    default:
      break;
  }
  
  if (buf.empty()) {
    wxMessageBox(_T("The required value field is empty"));
    return;
  }
  
  query += buf + _T(";");
  
  SelectQuery(query);
}

void MyListCtrl::Between(wxCommandEvent & event)
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString svalMore, svalLess;
  
  // SELECT * FROM workers WHERE salary BETWEEN 100 AND 1000
  
  svalLess = ident->m_textLess->GetLineText(0);
  svalMore = ident->m_textMore->GetLineText(0);
  
  if (svalMore.empty() || svalLess.empty()) {
    wxMessageBox(_T("Please supply both values"));
    return;
  }
  
  wxString query;
  
  switch (event.GetId())
  {
    case ID_BETWEEN: 
      query = _T("SELECT * FROM workers WHERE salary BETWEEN ");
    break;
    case ID_NOT_BETWEEN:
      query = _T("SELECT * FROM workers WHERE salary NOT BETWEEN ");
    break;
    default:
      break;
  }
  
  query += svalLess + _T(" AND ") + svalMore + _T(";");
  
  SelectQuery(query);
}


void MyListCtrl::UpdateQuery(const wxString & query)
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    m_db->ExecuteUpdate(query);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::UpdateQuery() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void MyListCtrl::SelectQuery(const wxString & query)
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    wxSQLite3ResultSet q = m_db->ExecuteQuery(query);  
    ShowListFromResult(q);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::SelectQuery() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void MyListCtrl::SelectAll()
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    wxSQLite3ResultSet q = m_db->ExecuteQuery(_T("SELECT * FROM workers;"));    
    ShowListFromResult(q); 
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::SelectAll() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void MyListCtrl::ShowListFromResult(wxSQLite3ResultSet & q)
{
  
  Hide();
  DeleteAllItems();
  
  try {
    int i = 0;
    //wxSQLite3ResultSet q = m_db->ExecuteQuery("select * from workers;");
    while (q.NextRow())
    {
      //cout << (const char*)(q.GetString(0).mb_str()) << " | ";
      wxString buf;
      long tmp = InsertItem(i, q.GetString(0), 0);
      SetItemData(tmp, q.GetInt(4)); // set ID as item data
      SetItem(tmp, 1, q.GetString(1));
      SetItem(tmp, 2, q.GetString(2));
  
      
      buf.Printf(_T("%d"), q.GetInt(3));
      SetItem(tmp, 3, buf);
      i++;
    }
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At MyListCtrl::SelectAll " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }

  Show();
  
}




