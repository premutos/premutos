#ifndef __LIVECAST_STATUS_DETAIL_HH__
#define __LIVECAST_STATUS_DETAIL_HH__

#include "../monitor/ResultCallbackIntf.hh"
#include "../monitor/StreamInfos.hh"
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <boost/property_tree/ptree.hpp>

namespace livecast {
namespace gui {

class LivecastResult;

class LivecastStatus : public wxFrame,
                       public livecast::monitor::ResultCallbackIntf
{
public:
  LivecastStatus(LivecastResult * livecastResult, const std::string& windowName, boost::shared_ptr<livecast::monitor::StreamInfos> streamInfos);
  ~LivecastStatus();
  
  void commitStreamList();
  void commitCheckStream(unsigned int streamId);

private:
  void onCheckStream(wxCommandEvent& ev);
  void onRefresh(wxCommandEvent & ev);
  void onCloseWindow(wxCloseEvent& ev);
  wxTreeItemId updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id);
  wxPanel * infos;
  wxPanel * status;
  wxPanel * schema;
  wxTreeCtrl * tree;

  wxStaticText * idValue;
  wxStaticText * modeValue;
  wxStaticText * srcIpValue;
  wxStaticText * dstHostValue;
  wxStaticText * dstPortValue;
  wxStaticText * protocolValue;
  wxStaticText * extKeyValue;
  wxStaticText * backlogValue;
  wxStaticText * nbConnectionsValue;
  wxStaticText * enabledValue;
  wxStaticText * disableFilterValue;

  const wxEventType checkStreamEvent;  
  unsigned int streamId;
  LivecastResult * const livecastResult;
  boost::shared_ptr<livecast::monitor::StreamInfos> streamInfos;
};

}
}

#endif
