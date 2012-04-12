#include "LivecastPreferences.hh"

using namespace livecast;
using namespace livecast::gui;

LivecastPreferences::LivecastPreferences(wxWindow * parent)
  : wxFrame(parent, wxID_ANY, "Preferences")
{

  wxPanel * dbInfos = new wxPanel(this, wxID_ANY);
  wxStaticText * host = new wxStaticText(dbInfos, wxID_ANY, wxT("host:"));
  wxStaticText * name = new wxStaticText(dbInfos, wxID_ANY, wxT("name:"));
  wxStaticText * user = new wxStaticText(dbInfos, wxID_ANY, wxT("user:"));
  wxStaticText * pass = new wxStaticText(dbInfos, wxID_ANY, wxT("pass:"));
    
  this->dbHost = new wxTextCtrl(dbInfos, wxID_ANY, wxT(""));
  this->dbName = new wxTextCtrl(dbInfos, wxID_ANY, wxT(""));
  this->dbUser = new wxTextCtrl(dbInfos, wxID_ANY, wxT(""));
  this->dbPass = new wxTextCtrl(dbInfos, wxID_ANY, wxT(""));

  wxGridSizer * sizer = new wxGridSizer(2, 10, 10);

  sizer->Add(host, 0, wxALIGN_LEFT, 0);
  sizer->Add(this->dbHost, 0, wxALIGN_LEFT, 0);
  sizer->Add(name, 0, wxALIGN_LEFT, 0);
  sizer->Add(this->dbName, 0, wxALIGN_LEFT, 0);
  sizer->Add(user, 0, wxALIGN_LEFT, 0);
  sizer->Add(this->dbUser, 0, wxALIGN_LEFT, 0);
  sizer->Add(pass, 0, wxALIGN_LEFT, 0);
  sizer->Add(this->dbPass, 0, wxALIGN_LEFT, 0);

  dbInfos->SetSizer(sizer);

  wxStaticBox * box = new wxStaticBox(this, wxID_ANY, "database informations");
  wxStaticBoxSizer * boxSizer = new wxStaticBoxSizer(box, wxHORIZONTAL);
  boxSizer->Add(dbInfos, 0, wxALIGN_CENTER, 10);
  this->SetSizer(boxSizer);
}
