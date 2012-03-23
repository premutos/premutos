#include "LivecastGui.hh"
#include "../lib/Log.hh"

#include <boost/thread.hpp>

LivecastGui::LivecastGui(const wxString& title, wxSize size)
  : wxFrame(NULL, 
            wxID_ANY, 
            title, 
            wxDefaultPosition, 
            size)
{
  this->SetIcon(wxIcon(wxT("res/kitd_noc_logo.png")));
  this->panel = new wxPanel(this, wxID_ANY);

  menubar = new wxMenuBar;
  file = new wxMenu;
  edit = new wxMenu;
  help = new wxMenu;

  menubar->Append(file, wxT("&File"));
  menubar->Append(edit, wxT("&Edit"));
  menubar->Append(help, wxT("&Help"));
  SetMenuBar(menubar);

  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);

  this->control.reset(new LivecastControl(this->panel));
  this->result.reset(new LivecastResult(this->panel));

  vbox->Add(this->result.get(), 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  vbox->Add(-1, 25);
  vbox->Add(this->control.get(), 0, wxALIGN_RIGHT | wxRIGHT, 10);

  this->panel->SetSizer(vbox);
  this->Centre();
}

LivecastGui::~LivecastGui()
{
}

void LivecastGui::check()
{
  LogError::getInstance().sysLog(DEBUG, "check");
  boost::thread thread(boost::bind(&LivecastMonitorIntf::check, this->monitor));
}
