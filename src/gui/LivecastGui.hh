#ifndef __LIVECAST_GUI_HH__
#define __LIVECAST_GUI_HH__

#include "../monitor/LivecastMonitor.hh"
#include "ResultCallback.hh"

#include <wx/wx.h>
#include <wx/aui/auibook.h>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class GuiConfiguration;
class LivecastControl;
class LivecastResult;
class LivecastServers;
class LivecastTaskBarIcon;
class LivecastPreferences;

class LivecastGui : public wxFrame
{
public:
  LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  ~LivecastGui();

  void addTab(wxPanel * panel, const char * title);
  void refresh();
  void check(unsigned int streamId = 0);
  inline boost::shared_ptr<livecast::monitor::ResultCallbackIntf> getResultCallback() { return this->resultCb; }
private:
  void onCloseWindow(wxCloseEvent& ev);
  void onOpenStreamInformation(wxCommandEvent& ev);
  void onOpenPreferences(wxCommandEvent& ev);
  void onClosePreferences(wxCloseEvent& ev);
  void onExit(wxCommandEvent& ev);
  void onTabMiddleUp(wxAuiNotebookEvent& ev);
  void onAddView(wxCommandEvent& ev);
  void onSwitchView(wxCommandEvent& ev);

  wxAuiNotebook * noteBook;
  wxPanel * panel;

  wxMenuBar * menubar;
  std::list<const wxMenuItem *> views;

  LivecastTaskBarIcon * taskBar;
  LivecastControl * control;
  LivecastResult * streams;
  LivecastServers * servers;
  LivecastPreferences * preferences;
  boost::shared_ptr<ResultCallback> resultCb;
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  boost::shared_ptr<GuiConfiguration> cfg;
};

}
}

#endif
