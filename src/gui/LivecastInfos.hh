#ifndef __LIVECAST_INFOS_HH__
#define __LIVECAST_INFOS_HH__

#include <wx/wx.h>
#include <wx/aui/auibook.h>
#include <wx/listctrl.h>

#include "../monitor/StreamInfos.hh"

namespace livecast {
namespace gui {

class LivecastListCtrl;

class LivecastInfos : public wxPanel
{
public:
  LivecastInfos(wxWindow* parent);

  void setInfos(const boost::shared_ptr<livecast::monitor::StreamInfos> infos);

protected:
  void onTabMiddleUp(wxAuiNotebookEvent& event);

private:
  wxAuiNotebook * noteBook;
  wxPanel * infos;
  LivecastListCtrl * servers;

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

};

}
}

#endif
