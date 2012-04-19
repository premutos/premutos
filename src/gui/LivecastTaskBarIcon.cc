#include "LivecastTaskBarIcon.hh"
#include "../lib/Log.hh"
#include "LivecastGui.hh"

#include <wx/menu.h>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;
using namespace livecast::lib;

LivecastTaskBarIcon::LivecastTaskBarIcon(LivecastGui * const livecastGui)
  : livecastGui(livecastGui)
{
}

void LivecastTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
  LogError::getInstance().sysLog(DEBUG, "show main window");
  this->livecastGui->Show();
}

void LivecastTaskBarIcon::OnMenuRestore(wxCommandEvent&)
{
  LogError::getInstance().sysLog(DEBUG, "menu restore");
  this->livecastGui->Show();
}

void LivecastTaskBarIcon::OnMenuExit(wxCommandEvent&)
{  
  LogError::getInstance().sysLog(DEBUG, "menu exit");
  this->livecastGui->Destroy();
}

void LivecastTaskBarIcon::OnMenuCheckStream(wxCommandEvent&)
{
  LogError::getInstance().sysLog(DEBUG, "menu check stream");
}

void LivecastTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
  LogError::getInstance().sysLog(DEBUG, "menu sub");
}

wxMenu * LivecastTaskBarIcon::CreatePopupMenu()
{
  wxMenu *menu = new wxMenu;
  menu->Append(wxID_ANY, wxT("&Restore main window"));
  menu->AppendSeparator();
//   menu->AppendCheckItem(wxID_ANY, wxT("&Check"));
//   menu->AppendSeparator();
//   wxMenu *submenu = new wxMenu;
//   submenu->Append(PU_SUB1, wxT("One submenu"));
//   submenu->AppendSeparator();
//   submenu->Append(PU_SUB2, wxT("Another submenu"));
//   menu->Append(PU_SUBMAIN, wxT("Submenu"), submenu);
//   menu->AppendSeparator();
  menu->Append(wxID_EXIT, wxT("&Quit"), wxT("Quit livecore admin"));
  menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastTaskBarIcon::OnMenuRestore, this, wxID_ANY);
  menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastTaskBarIcon::OnMenuExit, this, wxID_EXIT);
  return menu;
}
