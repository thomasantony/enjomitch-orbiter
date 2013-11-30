#include "projListCtrl.h"
#include "test.h"
#include "globals.h"
#include "projectdialog.h"

ProjListCtrl::ProjListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,  const wxSize& size, long style, wxSQLite3Database * db)
  : wxListCtrl(parent, id, pos, size, style) 
{
  m_parent = parent;
  m_projid = 0;
  //m_db = db;
  m_db = new wxSQLite3Database; 
  try {
    m_db->Open(DBFILE);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::ProjListCtrl() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  
  Connect(ID_PROJ_LISTBOX, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(ProjListCtrl::OnSelected));
  Connect(ID_PROJ_LISTBOX, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(ProjListCtrl::OnActivated));
  
}

ProjListCtrl::~ProjListCtrl()
{
  m_db->Close();
  delete m_db;
}

void ProjListCtrl::OnSelected(wxListEvent& event)
{
  
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  //wxBell();

  m_projid = GetItemData(event.m_itemIndex);
  wxListItem info;
  info.m_itemId = event.m_itemIndex;
  info.m_col = 0;
  info.m_mask = wxLIST_MASK_TEXT;
  
  
  //ident->m_text->SetLabel(wxString::Format(wxT("%d"), count));

  if ( GetItem(info) )
  {
    ident->m_textProjName->Clear();
    ident->m_textProjName->WriteText(info.m_text);
  }
  
  info.m_col ++;
  if ( GetItem(info) )
  {
    ident->m_textProjRev->Clear();
    ident->m_textProjRev->WriteText(info.m_text);
  }
  info.m_col ++;
}


void ProjListCtrl::OnActivated(wxListEvent& event)
{
  ProjectDialog *project = new ProjectDialog(this, wxT("Assignments"), m_db, m_projid);
  project->Show(true);
}

void ProjListCtrl::Add()
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString buf;
  
//"INSERT INTO workers VALUES ('Bjoork', 'Jurgen','Valhalla',99, NULL);"
  wxString query = _T("INSERT INTO projects VALUES (NULL, '");
  query += ident->m_textProjName->GetLineText(0);
  query += _T("', ");
  query += ident->m_textProjRev->GetLineText(0);
  query += _T(");");
  
  ident->m_textProjName->Clear();
  ident->m_textProjRev->Clear();
    
  UpdateQuery(query);
  SelectAll();
}

void ProjListCtrl::Update()
{
  Ident * ident = (Ident *) m_parent->GetParent()->GetParent();
  wxASSERT(ident);
  if (ident == NULL) return;
  
  wxString buf;
  wxString query = _T("UPDATE projects SET projectname='");
  
  query += ident->m_textProjName->GetLineText(0);
  query += _T("', revenue=");
  query += ident->m_textProjRev->GetLineText(0);
  query += _T(" WHERE projectid=");
  buf.Printf(_T("%d"), m_projid);
  query += buf + _T(";");
  
  //cout << (const char*)(query.mb_str()) << endl;
  UpdateQuery(query);
  SelectAll();
}

void ProjListCtrl::Delete()
{
  wxString buf, question;
  wxSQLite3ResultSet q;
  //DELETE FROM users WHERE userid=1;
  
  wxString query = _T("SELECT * FROM projects WHERE projectid=");
  buf.Printf(_T("%d"), m_projid);
  query += buf + _T(";");
  try 
  {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    q = m_db->ExecuteQuery(query);
    if (!q.NextRow()) return;
    
    question = _T("Are you sure you want to delete project\r\n");
    question += q.GetString(1) + _T("?");
    
    int result = wxMessageBox(question, _T("question"), wxYES_NO); 
    if (result == wxNO) return;
      
    query = _T("DELETE FROM projects WHERE projectid=");
    buf.Printf(_T("%d"), m_projid);
    query += buf + _T(";");  
    UpdateQuery(query);
    
    query = _T("DELETE FROM assignments WHERE projectid=");
    buf.Printf(_T("%d"), m_projid);
    query += buf + _T(";"); 
    UpdateQuery(query);
    
    SelectAll();
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::Delete() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void ProjListCtrl::UpdateQuery(const wxString & query)
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    m_db->ExecuteUpdate(query);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::UpdateQuery() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void ProjListCtrl::SelectQuery(const wxString & query)
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    wxSQLite3ResultSet q = m_db->ExecuteQuery(query);  
    ShowListFromResult(q);
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::SelectQuery() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void ProjListCtrl::SelectAll()
{
  try { 
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    wxSQLite3ResultSet q = m_db->ExecuteQuery(_T("SELECT projectid, projectname, revenue FROM projects;"));    
    ShowListFromResult(q); 
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::SelectAll() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void ProjListCtrl::ShowListFromResult(wxSQLite3ResultSet & q)
{
  
  Hide();
  DeleteAllItems();
  
  try {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    int i = 0;
    //wxSQLite3ResultSet q = m_db->ExecuteQuery("select * from workers;");
    while (q.NextRow())
    {
      //cout << (const char*)(q.GetString(0).mb_str()) << " | ";
      wxString query, buf;
      long tmp = InsertItem(i, q.GetString(1), 0);
      SetItemData(tmp, q.GetInt(0)); // set ID as item data
      SetItem(tmp, 1, q.GetString(2));
      
      query.Printf(_T("SELECT COUNT(userid) FROM assignments WHERE projectid = %d;"), q.GetInt(0));
      int takenBy = m_db->ExecuteScalar(query);
      buf.Printf(_T("%d"), takenBy);
      SetItem(tmp, 2, buf);
      
      
      buf.Printf(_T("%.2f"), takenBy ? (q.GetDouble(2) / takenBy) : 0 );
      SetItem(tmp, 3, buf);
      i++;
    }
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At ProjListCtrl::SelectAll " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
    
  Show();
}
