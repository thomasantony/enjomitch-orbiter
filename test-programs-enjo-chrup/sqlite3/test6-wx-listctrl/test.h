#ifndef _TEST_H
#define _TEST_H

#include <wx/wx.h>
#include <wx/imaglist.h>
#include <wx/bookctrl.h>
#include "myListCtrl.h"
#include "projListCtrl.h"
#include "globals.h"

class Ident : public wxFrame
{
public:
  Ident(const wxString& title);
  virtual ~Ident();
  
  void OnQuit(wxCommandEvent & event);
  void OnNew(wxCommandEvent & event);
  void OnUpdate(wxCommandEvent & event);
  void OnDelete(wxCommandEvent & event);
  void OnSearchAND(wxCommandEvent & event);
  void OnSearchOR(wxCommandEvent & event);
  void OnSelectAll(wxCommandEvent & event);
  void OnFunctions(wxCommandEvent & event);
  void OnMoreLess(wxCommandEvent & event);
  void OnBetween(wxCommandEvent & event);
  
  void OnProjNew(wxCommandEvent & event);
  void OnProjUpdate(wxCommandEvent & event);
  void OnProjDelete(wxCommandEvent & event);
  
  void OnBookChange(wxCommandEvent & event);
  
  
  void OnDefaultColumnWidth(wxCommandEvent & event);
  void OnAutoColumnWidth(wxCommandEvent & event);
  void OnToggleLines(wxCommandEvent & event);
  void OnHelp(wxCommandEvent & event);
  void OnAbout(wxCommandEvent & event);
  
  
  wxTextCtrl * m_textSurname;
  wxTextCtrl * m_textName;
  wxTextCtrl * m_textCity;
  wxTextCtrl * m_textSalary;
  
  wxTextCtrl * m_textResult; 
  wxTextCtrl * m_textLess;
  wxTextCtrl * m_textMore;
  
  wxTextCtrl * m_textProjName;
  wxTextCtrl * m_textProjRev;
  
private:
  void InitWithReportItems();
  void InitProjWithReportItems();
  
  static const int m_colWidth = 170;
  wxBookCtrl * m_book;
  wxImageList * m_imageListSmall;
  MyListCtrl * m_listCtrl;
  ProjListCtrl * m_projListCtrl;
  //wxImageList * m_imageListSmall;

};

#endif // #ifndef _TEST_H
