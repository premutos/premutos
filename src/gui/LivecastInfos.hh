#ifndef __LIVECAST_INFOS_HH__
#define __LIVECAST_INFOS_HH__

#include <wx/wx.h>
#include <wx/aui/auibook.h>
#include <wx/listctrl.h>

#include "../monitor/ResultCallbackIntf.hh"
#include "../monitor/StreamInfos.hh"

namespace livecast {
namespace gui {

class LivecastListCtrl;
class LivecastStatus;

class LivecastInfos : public wxPanel
{
public:
  LivecastInfos(wxWindow* parent);

  std::list<boost::shared_ptr<monitor::ResultCallbackIntf> > setInfos(boost::shared_ptr<const livecast::monitor::StreamInfos> infos);
  void refresh();

protected:
  void onTabMiddleUp(wxAuiNotebookEvent& event);

private:
  wxAuiNotebook * noteBook;
  wxPanel * infos;
  LivecastListCtrl * profiles;
  LivecastListCtrl * servers;
  LivecastStatus * primaryStatusSchema;
  LivecastStatus * backupStatusSchema;

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

  boost::shared_ptr<const livecast::monitor::StreamInfos> streamInfos;

  unsigned int currentSelectionPage;
};

}
}

#endif
