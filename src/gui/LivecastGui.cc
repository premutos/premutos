#include "LivecastGui.hh"
#include "GuiConfiguration.hh"
#include "LivecastControl.hh"
#include "LivecastResult.hh"
#include "LivecastStatus.hh"
#include "LivecastServers.hh"
#include "LivecastTaskBarIcon.hh"
#include "LivecastInfos.hh"
#include "LivecastPreferences.hh"
#include "../lib/Log.hh"
#include "../Configuration.hh"
#include <boost/lexical_cast.hpp>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

enum livecast_menu_t
{
  LIVECAST_MENU_FILE_STREAM = 1,
  LIVECAST_MENU_FILE_QUIT,
  LIVECAST_MENU_EDIT_PREFERENCES,
  LIVECAST_MENU_VIEW_LOCAL,
  LIVECAST_MENU_VIEW_DEV,
  LIVECAST_MENU_VIEW_HOM,
  LIVECAST_MENU_VIEW_PROD,
};

LivecastGui::LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor)
  : wxFrame(NULL, 
            wxID_ANY, 
            cfg->getMainWindowName(), 
            wxDefaultPosition, 
            wxSize(cfg->getMainWinHSize(), cfg->getMainWinVSize())),
    preferences(0),
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

  // file menu
  wxMenu * file = new wxMenu;  
  file->Append(LIVECAST_MENU_FILE_STREAM, wxT("Show &Stream\tCtrl-s"), wxT("Show Stream"));
  file->Append(LIVECAST_MENU_FILE_QUIT, wxT("&Quit\tCtrl-q"), wxT("Quit"));

  // edit menu
  wxMenu * edit = new wxMenu;
  edit->Append(LIVECAST_MENU_EDIT_PREFERENCES, wxT("&Preferences\tCtrl-p"), wxT("Preferences"));

  // view menu
  wxMenu * view = new wxMenu;
  view->AppendRadioItem(LIVECAST_MENU_VIEW_LOCAL, wxT("Local"), wxT(""));
  view->AppendRadioItem(LIVECAST_MENU_VIEW_DEV, wxT("Developpement"), wxT(""));
  view->AppendRadioItem(LIVECAST_MENU_VIEW_HOM, wxT("Homologation"), wxT(""));
  view->AppendRadioItem(LIVECAST_MENU_VIEW_PROD, wxT("Production"), wxT(""));

  // help menu
  wxMenu * help = new wxMenu;

  // menu bar
  menubar = new wxMenuBar;
  menubar->Append(file, wxT("&File"));
  menubar->Append(edit, wxT("&Edit"));
  menubar->Append(view, wxT("&View"));
  menubar->Append(help, wxT("&Help"));
  this->SetMenuBar(menubar);

  // bind menu action
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onOpenStreamInformation, this, LIVECAST_MENU_FILE_STREAM);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onExit, this, LIVECAST_MENU_FILE_QUIT);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onOpenPreferences, this, LIVECAST_MENU_EDIT_PREFERENCES);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onLocal, this, LIVECAST_MENU_VIEW_LOCAL);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onDev, this, LIVECAST_MENU_VIEW_DEV);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onHom, this, LIVECAST_MENU_VIEW_HOM);
  this->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastGui::onProd, this, LIVECAST_MENU_VIEW_PROD);

  //
  // 
  this->panel = new wxPanel(this, wxID_ANY);

  this->noteBook = new wxAuiNotebook(this->panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxBORDER_NONE);
  this->control = new LivecastControl(this);

  wxBoxSizer * vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(this->noteBook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 0);
  vbox->Add(this->control, 0, wxALIGN_RIGHT | wxLEFT | wxTOP, 5);
  this->panel->SetSizer(vbox);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->panel, 1, wxEXPAND, 0);
  this->SetSizer(box);

  this->streams = new LivecastResult(this->noteBook, this, this->monitor);
  this->noteBook->InsertPage(0, this->streams, "streams", true);

  this->servers = new LivecastServers(this->noteBook, this->monitor);
  this->noteBook->InsertPage(1, this->servers, "servers", false);

  this->resultCb.reset(new ResultCallback(this->streams, this->servers));

  this->Centre();  

  this->Bind(wxEVT_CLOSE_WINDOW, &LivecastGui::onCloseWindow, this, wxID_ANY);
  this->taskBar->Bind(wxEVT_TASKBAR_LEFT_DCLICK, &LivecastTaskBarIcon::OnLeftButtonDClick, this->taskBar, wxID_ANY);
  this->noteBook->Bind(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, &LivecastGui::onTabMiddleUp, this, wxID_ANY);
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

    LivecastStatus * statusFrame;
    try
    {
      LogError::getInstance().sysLog(DEBUG, "create status frame %d", *it);
      boost::shared_ptr<StreamInfos> streamInfos = this->monitor->getStreamInfos(*it);
      statusFrame = new LivecastStatus(this->noteBook, streamInfos);
    }
    catch (const StreamInfosException& ex)
    {
      LogError::getInstance().sysLog(ERROR, "cannot load information for stream id %u", streamId);
    }

    std::ostringstream title;
    title << "status of stream " << *it;
    this->addTab(statusFrame, title.str().c_str());

    boost::shared_ptr<LivecastStatusCallback> cb(new LivecastStatusCallback(statusFrame));
    this->monitor->check(*it, cb);
  }
}

void LivecastGui::addTab(wxPanel * panel, const char * title)
{
  this->noteBook->InsertPage(this->noteBook->GetPageCount(), panel, title, true);
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

void LivecastGui::onOpenStreamInformation(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "show stream");
  wxTextEntryDialog * entry = new wxTextEntryDialog(this, "stream id", "Enter the stream id to display informations");
  if (entry->ShowModal() == wxID_OK)
  {
    try
    {
      const std::string idStr = entry->GetValue().ToStdString();
      const unsigned int id = boost::lexical_cast<unsigned int>(idStr.c_str());
      LogError::getInstance().sysLog(DEBUG, "show stream %u", id);
      wxFrame * frame = new wxFrame(this, wxID_ANY, "full stream infos " + idStr);

      LivecastInfos * livecastInfos = new LivecastInfos(frame);
      boost::shared_ptr<ResultCallbackIntf> cb = livecastInfos->setInfos(this->monitor->getStreamInfos(id));
      this->monitor->check(id, cb);
      wxBoxSizer * box = new wxBoxSizer(wxVERTICAL);
      box->Add(livecastInfos, 1, wxEXPAND | wxALL, 0);
      frame->SetSizer(box);

      frame->SetSizer(box);
      frame->Show();
    }
    catch (const boost::bad_lexical_cast& ex)
    {
      LogError::getInstance().sysLog(DEBUG, "bad stream id");
      wxMessageDialog * error = new wxMessageDialog(this, "stream id must be an unsigned integer");
      error->ShowModal();
      error->Destroy();
    }
  }
  else
  {
    // cancel
  }
  entry->Destroy();
}

void LivecastGui::onOpenPreferences(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "onpen preferences");
  if (this->preferences == 0)
  {
    this->preferences = new LivecastPreferences(this);
  }
  this->preferences->Bind(wxEVT_CLOSE_WINDOW, &LivecastGui::onClosePreferences, this, wxID_ANY);
  this->preferences->Show(true);
}

void LivecastGui::onClosePreferences(wxCloseEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "close preferences");
  this->preferences->Show(false);
}

void LivecastGui::onExit(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "exit");
  this->Destroy();
}

void LivecastGui::onLocal(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "change view to local");
  this->SetTitle(std::string(cfg->getMainWindowName()) + " LOCAL");
  this->monitor->getConfiguration()->setAccess(Configuration::ACCESS_LOCAL);
  this->monitor->getConfiguration()->load();
  this->monitor->refresh(this->resultCb);
}

void LivecastGui::onDev(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "change view to developpement");
  this->SetTitle(std::string(cfg->getMainWindowName()) + " DEVELOPPEMENT");
  this->monitor->getConfiguration()->setAccess(Configuration::ACCESS_DEV);
  this->monitor->getConfiguration()->load();
  this->monitor->refresh(this->resultCb);
}

void LivecastGui::onHom(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(DEBUG, "change view to homologation");
  this->SetTitle(std::string(cfg->getMainWindowName()) + " HOMOLOGATION");
  this->monitor->getConfiguration()->setAccess(Configuration::ACCESS_HOM);
  this->monitor->getConfiguration()->load();
  this->monitor->refresh(this->resultCb);
}

void LivecastGui::onProd(wxCommandEvent& WXUNUSED(ev))
{
  LogError::getInstance().sysLog(ERROR, "change view to production");
  this->SetTitle(std::string(cfg->getMainWindowName()) + " PRODUCTION");
  this->monitor->getConfiguration()->setAccess(Configuration::ACCESS_PROD);
  this->monitor->getConfiguration()->load();
  this->monitor->refresh(this->resultCb);
}

void LivecastGui::onTabMiddleUp(wxAuiNotebookEvent& event)
{
  if (event.GetSelection() > 1)
  {
    this->noteBook->DeletePage(event.GetSelection());
  }
}

