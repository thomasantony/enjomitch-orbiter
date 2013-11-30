#include <iostream>
#include <wx/string.h>
#include <wx/databaselayer/DatabaseLayerException.h>
#include "ProjListCtrl.h"

ProjListCtrl::ProjListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator, const wxString& name)
        : MyListCtrl(parent, id, pos, size, style, validator, name)
{
    wxListItem itemCol;
    itemCol.SetText(_T("Name"));
    itemCol.SetImage(-1);
    InsertColumn(0, itemCol);

    itemCol.SetText(_T("Deadline"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    InsertColumn(1, itemCol);

    itemCol.SetText(_T("Revenue"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    InsertColumn(2, itemCol);

    itemCol.SetText(_T("Taken by"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    InsertColumn(3, itemCol);

    itemCol.SetText(_T("Rev / worker"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    InsertColumn(4, itemCol);

    SetTextColour(*wxBLUE);
   // lProjers->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, true);
    int colWidth = 120;
    SetColumnWidth( 0, colWidth );
    SetColumnWidth( 1, colWidth );
    SetColumnWidth( 2, colWidth );
    SetColumnWidth( 3, colWidth );
    SetColumnWidth( 4, colWidth );

    SelectAll();
}

void ProjListCtrl::SelectAll()
{
  try {
    if (db->IsOpen()) // db->Open(_("localhost"), _("test"));
    {
    DatabaseResultSet * q = db->ExecuteQuery(_T("SELECT * FROM projects;"));
    ShowListFromResult(q);
    }
  }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc ProjListCtrl::SelectAll"));
    }
}

void ProjListCtrl::ShowListFromResult(DatabaseResultSet * q)
{

  Hide();
  DeleteAllItems();

  try
  {
      wxString buf;
      //int i = 0;
      wxArrayString names, dates, revenues;
      wxArrayLong pids;
      //wxSQLite3ResultSet q = m_db->ExecuteQuery("select * from workers;");
      while (q->Next())
      {
        //cout << (const char*)(q.GetString(0).mb_str()) << " | ";
        pids.Add( q->GetResultInt(1) );
        names.Add( q->GetResultString(2) ); // name
        dates.Add( q->GetResultDate(3).FormatDate() ); // date
        revenues.Add( wxString::Format(_("%d"), q->GetResultInt(4)) ); // revenue

        //i++;
      }
      db->CloseResultSet(q);

      for (unsigned i = 0; i < names.Count(); i++)
      {
        long tmp = InsertItem( i, names.Item(i), 0 ); // name
        SetItemData( tmp, pids.Item(i) ); // set ID as item data
        SetItem( tmp, 1, dates.Item(i) ); // date
        SetItem( tmp, 2, revenues.Item(i) ); // revenue



        wxString query;
        query.Printf(_T("SELECT COUNT(wid) FROM assignments WHERE pid = %d;"), pids.Item(i));

        DatabaseResultSet * q2 = db->RunQueryWithResults(query);

        if ( q2->Next() )
        {
          float frevenues = wxAtof( revenues.Item(i) );
          int takenBy = q2->GetResultInt(1);
          buf.Printf(_T("%d"), takenBy);
          SetItem(tmp, 3, buf);
          buf.Printf(_T("%.2f"), takenBy ? (frevenues / takenBy) : 0 );
          SetItem(tmp, 4, buf);

          db->CloseResultSet(q2);
        }
      }
    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc ProjListCtrl::ShowListFromResult"));
    }


  Show();

}
