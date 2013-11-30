#include "ProjAssWorkDialog.h"
#include <wx/databaselayer/DatabaseLayerException.h>




ProjAssWorkDialog::ProjAssWorkDialog(wxWindow * parent, const wxString & title, MysqlDatabaseLayer* db, long id)
  : AssDialog(parent, title, db, id)
{
  m_str1stId = _T(" pid ");
  m_str2ndId = _T(" wid ");

  InitCheckListbox();

  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(ProjAssWorkDialog::OnOK));
  Centre();

  ShowModal();

  Destroy();

}


void ProjAssWorkDialog::InitCheckListbox()
{
  DatabaseResultSet * q;

  try
  {
    // Search for workers assigned to this project
    DatabaseResultSet * qAss;

    wxString query;
    query.Printf(_T("SELECT wid FROM assignments WHERE pid=%d;"), m_id);
    qAss = m_db->RunQueryWithResults(query);
    m_arrAssProj.Clear();

    while (qAss->Next())
    {
      m_arrAssProj.Add(qAss->GetResultInt(1));
    }



    q = m_db->RunQueryWithResults(_T("SELECT wid, surname, name FROM workers;"));

    m_arrIds.Clear();
    wxArrayString items;

    while (q->Next())
    {
      m_arrIds.Add(q->GetResultInt(1));
      items.Add(q->GetResultString(2) + _T(" ") + q->GetResultString(3));
    }
    m_db->CloseResultSet( q );
    m_asslbox->InsertItems(items, 0);

    for (size_t i = 0; i < m_arrIds.Count(); i++)
    {
      bool found = false;
      for (size_t j = 0; j < m_arrAssProj.Count(); j++)
      {
        // if this project from the whole list has already been assigned to this worker, check the item in checklistbox
        if (m_arrIds.Item(i) == m_arrAssProj.Item(j))
        {
          found = true;
          break;
        }
      }
      if (found)
        m_asslbox->Check(i);
    }
  }
  catch (DatabaseLayerException & e)
  {
      wxMessageBox(e.GetErrorMessage(), _("Exc ProjAssWorkDialog::InitCheckListbox()"));
  }

}
