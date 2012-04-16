#ifndef __LIVECAST_STATUS_DETAIL_HH__
#define __LIVECAST_STATUS_DETAIL_HH__

#include "../monitor/ResultCallbackIntf.hh"
#include "../monitor/StreamInfos.hh"
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <list>
#include <boost/property_tree/ptree.hpp>

namespace livecast {
namespace gui {

class StatusSchema;

class LivecastStatus : public wxPanel
{
public:
  LivecastStatus(wxWindow * parent, boost::shared_ptr<const livecast::monitor::StreamInfos> streamInfos, bool noTree = false, bool primary = true);
  ~LivecastStatus();
  
  void commitCheckStream(unsigned int streamId);

private:
  void onCheckStream(wxCommandEvent& ev);
  void onRefresh(wxCommandEvent & ev);
  wxTreeItemId updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id);
  StatusSchema * statusSchema;
  wxTreeCtrl * tree;

  const wxEventType checkStreamEvent;  
  unsigned int streamId;
  boost::shared_ptr<const livecast::monitor::StreamInfos> streamInfos;
  const bool noTree;
  const bool primary;
};

class LivecastStatusCallback : public livecast::monitor::ResultCallbackIntf
{
public:
  LivecastStatusCallback(LivecastStatus * status) : status(status) {}

  void commitStreamsList() {}
  void commitServersList() {}
  void commitCheckStream(unsigned int streamId) { this->status->commitCheckStream(streamId); }

private:
  LivecastStatus * status;
};

}
}

#endif
