#ifndef __LIVECAST_GUI_HH__
#define __LIVECAST_GUI_HH__

#include "../monitor/LivecastMonitor.hh"
#include "GuiConfiguration.hh"
#include "LivecastControl.hh"
#include "LivecastResult.hh"
#include "LivecastServers.hh"
#include "LivecastTaskBarIcon.hh"
#include "ResultCallback.hh"

#include <wx/wx.h>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class LivecastGui : public wxFrame
{
public:
  LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  ~LivecastGui();

  void refresh();
  void check(unsigned int streamId = 0);
  inline boost::shared_ptr<livecast::monitor::ResultCallbackIntf> getResultCallback() { return this->resultCb; }
private:
  void onCloseWindow(wxCloseEvent& ev);
  void onShowServer(wxCommandEvent& ev);
  void onExit(wxCommandEvent& ev);

  wxNotebook * noteBook;
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
