#ifndef _WORKASSPROJDIALOG_H
#define _WORKASSPROJDIALOG_H

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/databaselayer/MysqlDatabaseLayer.h>
#include "AssDialog.h"

using namespace std;

class WorkAssProjDialog : public AssDialog
{
public:
  WorkAssProjDialog(wxWindow * parent, const wxString& title, MysqlDatabaseLayer* db, long wid);

  private:
    void InitCheckListbox();
    //static const long ID_ASSIGNMENT_LISTBOX;
};

#endif
