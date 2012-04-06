#include "LivecastEditPipeline.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::gui;
using namespace livecast::monitor;

LivecastEditPipeline::LivecastEditPipeline(wxWindow * parent, 
                                           boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor, 
                                           unsigned int streamId)
  : wxPanel(parent, wxID_ANY),
    monitor(monitor),
    streamId(streamId)
{
  this->textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

  wxPanel * control = new wxPanel(this, wxID_ANY);
  wxButton * reset = new wxButton(control, wxID_CANCEL, "Reset");
  wxButton * save = new wxButton(control, wxID_APPLY, "Save");
 
  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);  
  hbox->Add(reset, 0, wxALIGN_CENTRE | wxRIGHT, 10);
  hbox->Add(save, 0, wxALIGN_CENTRE | wxLEFT, 10);
  control->SetSizer(hbox);
  
  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(this->textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  vbox->Add(-1, 25);
  vbox->Add(control, 0, wxALIGN_CENTER | wxBOTTOM, 10);
  this->SetSizer(vbox);
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  *this->textCtrl << cfg->getPipeline(this->streamId);

  reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastEditPipeline::onReset, this, wxID_ANY);
  save->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastEditPipeline::onSave, this, wxID_ANY);
}

void LivecastEditPipeline::onReset(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "reset");
}

void LivecastEditPipeline::onSave(wxCommandEvent & WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "save");
}
