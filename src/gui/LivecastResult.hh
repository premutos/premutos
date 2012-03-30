#ifndef __LIVECAST_RESULT_HH__
#define __LIVECAST_RESULT_HH__

#include "../ResultCallbackIntf.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../monitor/StreamInfos.hh"
#include "LivecastStatus.hh"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <map>
#include <list>
#include <boost/thread/mutex.hpp>

namespace livecast {
namespace gui {

class LivecastGui;

class LivecastResult : public wxPanel,
                       public livecast::monitor::ResultCallbackIntf
{
public:
  LivecastResult(LivecastGui * livecastGui, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  ~LivecastResult();

  void commitStreamList();
  void commitCheckStream(unsigned int streamId);
  std::list<unsigned int> getStreamsSelected() const;
  boost::shared_ptr<LivecastStatus> getStreamStatus(unsigned int streamId);
  int removeStreamStatus(unsigned int streamId);  
  
private:
  void onStreamListUpdate(wxCommandEvent& ev);
  void onCheckStream(wxCommandEvent& ev);
  void onRefresh(wxCommandEvent& ev);
  void onStreamListDblClicked(wxListEvent& ev);
  void OnCloseLivecastStatus(wxCloseEvent& event);
  void updateItem(livecast::monitor::MonitorConfiguration::map_streams_infos_t::const_iterator& it);

  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;

  LivecastGui * livecastGui;
  wxListCtrl * list;

  wxColour * lightYellow;
  wxColour * lightBlue;  
  wxColour * orange;  

  typedef std::map<unsigned int, boost::shared_ptr<LivecastStatus> > status_frames_t;
  status_frames_t statusFrames;

  const wxEventType streamListEvent;  
  const wxEventType checkStreamEvent;  
  
  mutable boost::mutex mutex;
};

}
}
 
#endif
