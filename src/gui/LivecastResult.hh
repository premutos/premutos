#ifndef __LIVECAST_RESULT_HH__
#define __LIVECAST_RESULT_HH__

#include "../monitor/ResultCallbackIntf.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../monitor/StreamInfos.hh"
#include "LivecastStatus.hh"
#include "LivecastInfos.hh"
#include "LivecastListCtrl.hh"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>

#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace livecast {
namespace gui {

class LivecastGui;

class LivecastResult : public wxPanel,
                       public boost::enable_shared_from_this<LivecastResult>
{
public:
  LivecastResult(wxWindow * parent, LivecastGui * livecastGui, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);
  ~LivecastResult();

  std::list<unsigned int> getStreamsSelected() const;
  boost::shared_ptr<LivecastStatus> getStreamStatus(unsigned int streamId);
  int removeStreamStatus(unsigned int streamId);  

  const wxEventType streamsListEvent;  
  const wxEventType checkStreamEvent;  
  
private:
  void onStreamListUpdate(wxCommandEvent& ev);
  void onCheckStream(wxCommandEvent& ev);
  void onRefresh(wxCommandEvent& ev);
  void onStreamListDblClicked(wxListEvent& ev);
  void onStreamListItemSelected(wxListEvent& ev);
  void updateItem(livecast::monitor::MonitorConfiguration::map_streams_infos_t::const_iterator& it);

  void onPaint(wxPaintEvent& ev);

  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;

  LivecastGui * livecastGui;

  wxSplitterWindow * splitter;
  LivecastListCtrl * list;
  LivecastInfos * infos;

  typedef std::map<unsigned int, boost::shared_ptr<LivecastStatus> > status_frames_t;
  status_frames_t statusFrames;
  
  mutable boost::mutex mutex;
};

}
}
 
#endif
