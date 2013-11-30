#ifndef _PROJASSWORKDIALOG_H
#define _PROJASSWORKDIALOG_H

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/databaselayer/MysqlDatabaseLayer.h>
#include "AssDialog.h"


class ProjAssWorkDialog : public AssDialog
{
public:
  ProjAssWorkDialog(wxWindow * parent, const wxString& title, MysqlDatabaseLayer* db, long pid);

  private:
    void InitCheckListbox();

};

#endif
