#include "LivecastGui.hh"
#include "LivecastServers.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

enum LIVECAST_MENU
{
  LIVECAST_MENU_QUIT = 1,
  LIVECAST_MENU_SERVERS,
};

LivecastGui::LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor)
  : wxFrame(NULL, 
            wxID_ANY, 
            cfg->getMainWindowName(), 
            wxDefaultPosition, 
            wxSize(cfg->getMainWinHSize(), cfg->getMainWinVSize())),
    monitor(monitor),
    cfg(cfg)
{
  //
  // icon
  this->SetIcon(wxIcon(wxT("res/kitd_noc_logo.png")));

  //
  // menu
  if (wxTaskBarIcon::IsAvailable())
  {
    this->taskBar = new LivecastTaskBarIcon(this);
    taskBar->SetIcon(wxIcon(wxT("res/kitd_noc_logo.png")), "noc admin");
  }
  else
  {
    LogError::getInstance().sysLog(ERROR, "system tray not available on your system");
  }

  file = new wxMenu;  
  file->Append(LIVECAST_MENU_SERVERS, wxT("Show &Servers\tCtrl-s"), wxT("Show Servers"));
  file->Append(LIVECAST_MENU_QUIT, wxT("&Quit\tCtrl-q"), wxT("Quit"));
  
  edit = new wxMenu;
  help = new wxMenu;

  menubar = new wxMenuBar;
  menubar->Append(file, wxT("&File"));
  menubar->Append(edit, wxT("&Edit"));
  menubar->Append(help, wxT("&Help"));

  this->SetMenuBar(menubar);

  //
  // 
  this->panel = new wxPanel(this, wxID_ANY);

  this->noteBook = new wxNotebook(this->panel, wxID_ANY);
  this->control = new LivecastControl(this);

  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(this->noteBook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  vbox->Add(-1, 25);
  vbox->Add(this->control, 0, wxALIGN_RIGHT | wxRIGHT, 10);
  this->panel->SetSizer(vbox);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->panel, 1, wxEXPAND, 10);
  this->SetSizer(box);

  this->streams = new LivecastResult(this->noteBook, this, this->monitor);
  this->noteBook->InsertPage(0, this->streams, "streams", true);

  this->servers = new LivecastServers(this->noteBook, this->monitor);
  this->noteBook->InsertPage(1, this->servers, "servers", false);

  this->resultCb.reset(new ResultCallback(this->streams, this->servers));

  this->Centre();  

  this->Bind(wxEVT_CLOSE_WINDOW, &LivecastGui::onCloseWindow, this, wxID_ANY);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onShowServer, this, LIVECAST_MENU_SERVERS);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onExit, this, LIVECAST_MENU_QUIT);
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
    lId = this->streams->getStreamsSelected();
  }
  else
  {
    lId.push_back(streamId);
  }

  for (std::list<unsigned int>::const_iterator it = lId.begin(); it != lId.end(); ++it)
  {
    LogError::getInstance().sysLog(DEBUG, "check %d", *it);
    boost::shared_ptr<ResultCallbackIntf> cb = this->streams->getStreamStatus(*it);
    this->monitor->check(*it, cb);
  }
}

void LivecastGui::refresh()
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
  this->monitor->refresh(this->resultCb);
}

void LivecastGui::onCloseWindow(wxCloseEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "hide main window");
  this->Hide();
}

void LivecastGui::onShowServer(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "show server");
  this->servers = new LivecastServers(this->noteBook, this->monitor);
  this->noteBook->InsertPage(1, this->servers, "servers", true);
}

void LivecastGui::onExit(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "exit");
  this->Destroy();
}
