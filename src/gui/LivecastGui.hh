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
  void onShowServer(wxCommandEvent& ev);
  void onExit(wxCommandEvent& ev);
  void onTabMiddleUp(wxAuiNotebookEvent& ev);

  wxAuiNotebook * noteBook;
  wxPanel * panel;

  wxMenuBar *menubar;
  wxMenu *file;
  wxMenu *edit;
  wxMenu *help;

  LivecastTaskBarIcon * taskBar;
  LivecastControl * control;
  LivecastResult * streams;
  LivecastServers * servers;
  boost::shared_ptr<ResultCallback> resultCb;
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  boost::shared_ptr<GuiConfiguration> cfg;
};

}
}

#endif
