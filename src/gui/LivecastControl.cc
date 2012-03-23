#include "LivecastControl.hh"
#include "LivecastGui.hh"
#include "../lib/Log.hh"

LivecastControl::LivecastControl(wxPanel * parent)
  : wxPanel(parent, 
            wxID_ANY, 
            wxPoint(-1, -1), 
            wxSize(-1, -1), 
            wxBORDER_NONE),
    parent(parent)
{
  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton * check = new wxButton(this, wxID_OK, wxT("Check"));
  wxButton * quit = new wxButton(this, wxID_EXIT, wxT("Quit"));
  
  hbox->Add(check, 0, wxALIGN_RIGHT | wxRIGHT, 10);
  hbox->Add(quit, 0, wxALIGN_RIGHT | wxRIGHT, 0);

  this->SetSizer(hbox);

  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LivecastControl::OnCheck));
  Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(LivecastControl::OnQuit));
}

LivecastControl::~LivecastControl()
{
}

void LivecastControl::OnCheck(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "check");
  LivecastGui * gui = (LivecastGui*) this->parent->GetParent();
  gui->check();
}

void LivecastControl::OnQuit(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "quit");
  LivecastGui * gui = (LivecastGui*) this->parent->GetParent();
  gui->Close(true);
}
