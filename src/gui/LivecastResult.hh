#ifndef __LIVECAST_RESULT_HH__
#define __LIVECAST_RESULT_HH__

#include "../monitor/ResultCallbackIntf.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../monitor/StreamInfos.hh"
#include "LivecastStatus.hh"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace livecast {
namespace gui {

class LivecastGui;

//
//
class LivecastListCtrl : public wxListCtrl
{
public:
  LivecastListCtrl(wxWindow* parent, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);

protected:
  wxString OnGetItemText(long item, long column) const;
  wxListItemAttr * OnGetItemAttr(long item) const;

private:
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
};

//
//
class LivecastResult : public wxPanel,
                       public livecast::monitor::ResultCallbackIntf,
                       public boost::enable_shared_from_this<LivecastResult>
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
  void updateItem(livecast::monitor::MonitorConfiguration::map_streams_infos_t::const_iterator& it);

  void OnPaint(wxPaintEvent& ev);

  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;

  LivecastGui * livecastGui;
  wxListCtrl * list;

  typedef std::map<unsigned int, boost::shared_ptr<LivecastStatus> > status_frames_t;
  status_frames_t statusFrames;

  const wxEventType streamListEvent;  
  const wxEventType checkStreamEvent;  
  
  mutable boost::mutex mutex;
};

}
}
 
#endif
