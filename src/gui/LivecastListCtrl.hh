#ifndef __LIVECAST_LIST_CTRL_HH__
#define __LIVECAST_LIST_CTRL_HH__

#include "../monitor/StreamInfos.hh"

#include <wx/listctrl.h>
#include <boost/shared_ptr.hpp>

namespace livecast {

namespace gui {

class LivecastListCtrl : public wxListView
{
public:
  LivecastListCtrl(wxWindow * parent);

  void SetItemTextColour(long item, livecast::monitor::StreamInfos::status_t status);

protected:
  LivecastListCtrl(wxWindow * parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style);
  void onPaint(wxPaintEvent& ev);
};

}
}

#endif
