#ifndef __LIVECAST_GUI_HH__
#define __LIVECAST_GUI_HH__

#include "../LivecastMonitorIntf.hh"
#include "LivecastControl.hh"
#include "LivecastResult.hh"

#include <wx/wx.h>
#include <boost/shared_ptr.hpp>

class LivecastGui;
typedef boost::shared_ptr<LivecastGui> LivecastGuiPtr;

class LivecastGui : public wxFrame
{
public:
  LivecastGui(const wxString& title, wxSize size);
  ~LivecastGui();

  void check();
  inline void setMonitor(LivecastMonitorIntfPtr monitor) { this->monitor = monitor; }
  inline boost::shared_ptr<ResultCallbackIntf> getResultCallback() { return this->result; }
private:
  wxPanel * panel;

  wxMenuBar *menubar;
  wxMenu *file;
  wxMenu *edit;
  wxMenu *help;

  LivecastMonitorIntfPtr monitor;
  boost::shared_ptr<LivecastControl> control;
  boost::shared_ptr<LivecastResult> result;
};

#endif
