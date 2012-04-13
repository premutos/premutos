#ifndef __LIVECAST_SERVERS_HH__
#define __LIVECAST_SERVERS_HH__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/aui/auibook.h>
#include <wx/treelistctrl.hh>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace livecast {


namespace monitor {
class LivecastMonitor;
class LivecastConnection;
}

namespace gui {

class LivecastListCtrl;

class LivecastServers : public wxPanel
{
public:
  LivecastServers(wxWindow * parent, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  const wxEventType serversListEvent;  

protected:
  void refresh();
  void onServersListUpdate(wxCommandEvent& event);
  void onServersListDblClicked(wxListEvent& event);
  void onTabMiddleUp(wxAuiNotebookEvent& event);
  void fillStatus(wxTreeListCtrl * status, boost::property_tree::ptree& result) const;

private:
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  boost::shared_ptr<livecast::monitor::LivecastConnection> connectionTmp;
  wxSplitterWindow * splitter;
  LivecastListCtrl * servers;
  wxAuiNotebook * results;
};

}
}

#endif
