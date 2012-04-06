#ifndef __LIVECAST_EDIT_PIPELINE_HH__
#define __LIVECAST_EDIT_PIPELINE_HH__

#include "../monitor/LivecastMonitor.hh"
#include <wx/wx.h>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class LivecastEditPipeline : public wxPanel
{
public:
  LivecastEditPipeline(wxWindow * parent, 
                       boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor, 
                       unsigned int streamId);

protected:
  void onReset(wxCommandEvent & event);
  void onSave(wxCommandEvent & event);
  void onQuit(wxCommandEvent & event);

private:
  wxTextCtrl * textCtrl;

  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;
  unsigned int streamId;
};

}
}

#endif
