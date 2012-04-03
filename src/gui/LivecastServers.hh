#ifndef __LIVECAST_SERVERS_HH__
#define __LIVECAST_SERVERS_HH__

#include "LivecastListCtrl.hh"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace livecast {


namespace monitor {
class LivecastMonitor;
class LivecastConnection;
}

namespace gui {

class LivecastServers : public wxPanel
{
public:
  LivecastServers(wxWindow * parent, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  const wxEventType serversListEvent;  

protected:
  void refresh();
  void onServersListUpdate(wxCommandEvent& event);
  void onServersListDblClicked(wxListEvent& event);
  void onResultsListDblClicked(wxListEvent& event);
  void fillList(LivecastListCtrl * list, boost::property_tree::ptree& result) const;
private:
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  boost::shared_ptr<livecast::monitor::LivecastConnection> connectionTmp;
  std::string host;
  std::string type;
  wxSplitterWindow * splitter;
  LivecastListCtrl * servers;
  wxNotebook * results;
  LivecastListCtrl * statusList;
  std::map<unsigned int, LivecastListCtrl *> statusDetails;
};

}
}

#endif
