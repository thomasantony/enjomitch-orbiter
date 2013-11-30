#include <wx/string.h>
#include <wx/databaselayer/DatabaseLayerException.h>
#include "WorkListCtrl.h"

WorkListCtrl::WorkListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator, const wxString& name)
        : MyListCtrl(parent, id, pos, size, style, validator, name)
{
    wxListItem itemCol;
    itemCol.SetText(_T("Surname"));
    itemCol.SetImage(-1);
    InsertColumn(0, itemCol);

    itemCol.SetText(_T("Name"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    InsertColumn(1, itemCol);

    itemCol.SetText(_T("City"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    InsertColumn(2, itemCol);

    itemCol.SetText(_T("Salary"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    InsertColumn(3, itemCol);

    SetTextColour(*wxBLUE);
   // lWorkers->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, true);
    int colWidth = 150;
    SetColumnWidth( 0, colWidth );
    SetColumnWidth( 1, colWidth );
    SetColumnWidth( 2, colWidth );
    SetColumnWidth( 3, colWidth );

    SelectAll();
}

void WorkListCtrl::SelectAll()
{
  try {
    if (db->IsOpen()) // db->Open(_("localhost"), _("test"));
    {
    DatabaseResultSet * q = db->ExecuteQuery(_T("SELECT * FROM workers;"));
    ShowListFromResult(q);
    }
  }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc WorkListCtrl::SelectAll"));
    }
}

void WorkListCtrl::ShowListFromResult(DatabaseResultSet * q)
{

  Hide();
  DeleteAllItems();

  try
  {
        wxArrayString surnames, names, cities;
        wxArrayLong wids;
        //int i = 0;
        while (q->Next())
        {
          wxString buf;
          wids.Add( q->GetResultInt(1) );
          surnames.Add( q->GetResultString(2) );
          names.Add( q->GetResultString(3) );
          cities.Add( q->GetResultString(4) );

          //buf.Printf(_T("%d"), q.GetInt(3));
          //SetItem(tmp, 3, buf);
          //i++;
        }
        db->CloseResultSet(q);

        for (unsigned i = 0; i < wids.Count(); i++)
        {
          long tmp = InsertItem(i, surnames.Item(i), 0); // surname
          SetItemData(tmp, wids.Item(i)); // set ID as item data
          SetItem(tmp, 1, names.Item(i)); // name
          SetItem(tmp, 2, cities.Item(i)); // city
          //wxString query = wxString::Format( _("SELECT SUM(revenue) FROM projects WHERE pid = "
          //           "ANY (SELECT pid FROM assignments WHERE wid = %d);"), wids.Item(i) );
          wxString query = wxString::Format( _("SELECT pid FROM assignments WHERE "
                      "wid = %d;"), wids.Item(i) );
          DatabaseResultSet * q2 = db->RunQueryWithResults(query);
          wxArrayInt pids;
          while ( q2->Next() )
          {
            pids.Add( q2->GetResultInt(1) );
          }
          db->CloseResultSet( q2 );
          float salary = 0, revenue = 0;
          for (unsigned j = 0; j < pids.Count(); j++)
          {
           // query = wxString::Format( _("SELECT revenue / "
               //     "(SELECT COUNT(wid) FROM assignments WHERE "
               //     "pid = %d) FROM projects WHERE pid = %d;)"), pids.Item(j), pids.Item(j) );
            query = wxString::Format( _("SELECT revenue FROM projects WHERE pid = %d;"), pids.Item(j) );
            DatabaseResultSet * q3 = db->RunQueryWithResults(query);
            if ( q3->Next() )
            {
              revenue = q3->GetResultInt(1);
              db->CloseResultSet( q3 );
            }
            query = wxString::Format( _("SELECT COUNT(wid) FROM assignments WHERE pid = %d;"), pids.Item(j) );
            DatabaseResultSet * q4 = db->RunQueryWithResults(query);
            if ( q4->Next() )
            {
              // revenue per worker
              revenue /= q4->GetResultInt(1);
              db->CloseResultSet( q4 );
            }
            salary += revenue;
          }

          //
          SetItem(tmp, 3, wxString::Format( _("%.2f"), salary ) ); // salary
        }


    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc WorkListCtrl::ShowListFromResult"));
    }


  Show();

}
