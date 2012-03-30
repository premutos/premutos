#include "LivecastGui.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

LivecastGui::LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor)
  : wxFrame(NULL, 
            wxID_ANY, 
            cfg->getMainWindowName(), 
            wxDefaultPosition, 
            wxSize(cfg->getMainWinHSize(), cfg->getMainWinVSize())),
    monitor(monitor),
    cfg(cfg)
{
  this->SetIcon(wxIcon(wxT("res/kitd_noc_logo.png")));
  this->panel = new wxPanel(this, wxID_ANY);

  if (wxTaskBarIcon::IsAvailable())
  {
    this->taskBar = new LivecastTaskBarIcon(this);
    taskBar->SetIcon(wxIcon(wxT("res/kitd_noc_logo.png")), "noc admin");
  }
  else
  {
    LogError::getInstance().sysLog(ERROR, "system tray not available on your system");
  }

  menubar = new wxMenuBar;
  file = new wxMenu;
  edit = new wxMenu;
  help = new wxMenu;

  menubar->Append(file, wxT("&File"));
  menubar->Append(edit, wxT("&Edit"));
  menubar->Append(help, wxT("&Help"));
  SetMenuBar(menubar);

  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);

  this->control.reset(new LivecastControl(this));
  this->result.reset(new LivecastResult(this, this->monitor));

  vbox->Add(this->result.get(), 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  vbox->Add(-1, 25);
  vbox->Add(this->control.get(), 0, wxALIGN_RIGHT | wxRIGHT, 10);

  this->panel->SetSizer(vbox);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->panel, 1, wxEXPAND, 10);
  this->SetSizer(box);

  this->Centre();

  this->Bind(wxEVT_CLOSE_WINDOW, &LivecastGui::onCloseWindow, this, wxID_ANY);
  this->taskBar->Bind(wxEVT_TASKBAR_LEFT_DCLICK, &LivecastTaskBarIcon::OnLeftButtonDClick, this->taskBar, wxID_ANY);
}

LivecastGui::~LivecastGui()
{
  this->taskBar->Destroy();
}

void LivecastGui::check(unsigned int streamId)
{
  LogError::getInstance().sysLog(DEBUG, "check");
  std::list<unsigned int> lId;
  if (streamId == 0)
  {
    lId = this->result->getStreamsSelected();
  }
  else
  {
    lId.push_back(streamId);
  }

  for (std::list<unsigned int>::const_iterator it = lId.begin(); it != lId.end(); ++it)
  {
    LogError::getInstance().sysLog(DEBUG, "check %d", *it);
    boost::shared_ptr<ResultCallbackIntf> cb = this->result->getStreamStatus(*it);
    this->monitor->check(*it, cb);
  }
}

void LivecastGui::refresh()
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
  this->monitor->refresh(this->result);
}

void LivecastGui::onCloseWindow(wxCloseEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "hide main window");
  this->Hide();
}
