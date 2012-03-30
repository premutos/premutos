#ifndef __LIVECAST_GUI_HH__
#define __LIVECAST_GUI_HH__

#include "../monitor/LivecastMonitor.hh"
#include "GuiConfiguration.hh"
#include "LivecastControl.hh"
#include "LivecastResult.hh"
#include "LivecastTaskBarIcon.hh"

#include <wx/wx.h>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class LivecastGui;
typedef boost::shared_ptr<LivecastGui> LivecastGuiPtr;

class LivecastGui : public wxFrame
{
public:
  LivecastGui(boost::shared_ptr<GuiConfiguration> cfg, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  ~LivecastGui();

  void refresh();
  void check(unsigned int streamId = 0);
  inline boost::shared_ptr<livecast::monitor::ResultCallbackIntf> getResultCallback() { return this->result; }
private:
  void onCloseWindow(wxCloseEvent& ev);
  void onTaskBarLeftClick(wxTaskBarIconEvent& ev);
  void onTaskBarRightClick(wxTaskBarIconEvent& ev);

  wxPanel * panel;

  wxMenuBar *menubar;
  wxMenu *file;
  wxMenu *edit;
  wxMenu *help;

  LivecastTaskBarIcon * taskBar;
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  boost::shared_ptr<GuiConfiguration> cfg;
  boost::shared_ptr<LivecastControl> control;
  boost::shared_ptr<LivecastResult> result;
};

}
}

#endif
