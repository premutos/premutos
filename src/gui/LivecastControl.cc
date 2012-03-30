#include "LivecastControl.hh"
#include "LivecastGui.hh"
#include "../lib/Log.hh"

LivecastControl::LivecastControl(LivecastGui * livecastGui)
  : wxPanel(livecastGui, 
            wxID_ANY, 
            wxDefaultPosition, 
            wxDefaultSize, 
            wxBORDER_NONE),
    livecastGui(livecastGui)
{
  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton * refresh = new wxButton(this, wxID_OK, wxT("Refresh"));
  wxButton * check = new wxButton(this, wxID_APPLY, wxT("Check"));
  wxButton * quit = new wxButton(this, wxID_EXIT, wxT("Quit"));
  
  hbox->Add(refresh, 0, wxALIGN_RIGHT | wxRIGHT, 10);
  hbox->Add(check, 0, wxALIGN_RIGHT | wxRIGHT, 10);
  hbox->Add(quit, 0, wxALIGN_RIGHT | wxRIGHT, 0);

  this->SetSizer(hbox);

  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastControl::OnRefresh, this, wxID_OK);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastControl::OnCheck,   this, wxID_APPLY);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastControl::OnQuit,    this, wxID_EXIT);

}

LivecastControl::~LivecastControl()
{
}

void LivecastControl::OnRefresh(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
  this->livecastGui->refresh();
}

void LivecastControl::OnCheck(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "check");
  this->livecastGui->check();
}

void LivecastControl::OnQuit(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "quit");
  this->livecastGui->Destroy();
}
