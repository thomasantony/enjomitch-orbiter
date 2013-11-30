#include "ConnectDialog.h"
#include "proj_testMain.h"
#include <wx/databaselayer/DatabaseLayerException.h>
#include "Globals.h"

const long ConnectDialog::idtHost = wxNewId();
const long ConnectDialog::idtDb = wxNewId();
const long ConnectDialog::idtLogin = wxNewId();
const long ConnectDialog::idtPass = wxNewId();

ConnectDialog::ConnectDialog( wxWindow * parent, const wxString & title, MysqlDatabaseLayer* db )
: wxDialog ( parent, wxID_ANY, title, wxDefaultPosition, wxSize(220, 280) )
{
  m_db = db;
  m_parent = parent;

  //wxPanel * panel = new wxPanel(this);
  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);

  m_tHost = new wxTextCtrl(this, idtHost, _("localhost") );
  m_tDb = new wxTextCtrl(this, idtDb );
  m_tLogin = new wxTextCtrl(this, idtLogin );
  m_tPass = new wxTextCtrl(this, idtPass, _(""),
            wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );

  wxGridSizer * gs = new wxGridSizer(4, 2, 10, 10);
  gs->Add( new wxStaticText(this, wxID_STATIC, _("Host") ), 1,
          wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
  gs->Add( m_tHost , 1 , wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
  gs->Add( new wxStaticText(this, wxID_STATIC, _("Database") ), 1,
          wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
  gs->Add( m_tDb, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
  gs->Add( new wxStaticText(this, wxID_STATIC, _("Login") ), 1 ,
          wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
  gs->Add( m_tLogin, 1  , wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
  gs->Add( new wxStaticText(this, wxID_STATIC, _("Password") ), 1 ,
          wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
  gs->Add( m_tPass, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );


  vbox->Add(gs, 1, wxALL, 10);


  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);
  hbox->Add(new wxButton(this, wxID_CANCEL), 1, wxRIGHT, 10);
  hbox->Add(new wxButton(this, wxID_OK), 1, wxLEFT, 10);

  vbox->Add(hbox, 0 , wxEXPAND);

  this->SetSizer(vbox);

  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConnectDialog::OnOK));

  ShowModal();
  Destroy();
}

void ConnectDialog::OnOK( wxCommandEvent & WXUNUSED(event) )
{
  if ( m_tHost->IsEmpty() )
  {
    wxMessageBox(_T("Host field empty"), _T("Error"), wxICON_ERROR);
    return;
  }
  if ( m_tDb->IsEmpty() )
  {
    wxMessageBox(_T("Database name field empty"), _T("Error"), wxICON_ERROR);
    return;
  }
  try
  {
    if ( m_tLogin->IsEmpty() && m_tPass->IsEmpty() )
    {
      m_db->Open( m_tHost->GetLineText(0), m_tDb->GetLineText(0) );

    }
    else
    {
      m_db->Open( m_tHost->GetLineText(0), m_tDb->GetLineText(0),
                    m_tLogin->GetLineText(0), m_tPass->GetLineText(0) );
    }
    m_db->RunQuery(CREATE_WORKERS, true);
    m_db->RunQuery(CREATE_PROJECTS, true);
    m_db->RunQuery(CREATE_ASSIGNMENTS, true);

  }
  catch (DatabaseLayerException & e)
  {
    wxMessageBox(e.GetErrorMessage(), _("Error"), wxICON_ERROR);
#ifdef __WXMSW__
    m_parent->Close();
#endif
    return;
  }
  Destroy();
}
