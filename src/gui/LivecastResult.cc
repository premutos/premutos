#include "../lib/Log.hh"
#include "Util.hh"
#include "LivecastResult.hh"
#include "LivecastGui.hh"
#include "LivecastStatus.hh"
#include "LivecastInfos.hh"
#include "LivecastListCtrl.hh"
#include "LivecastEditPipeline.hh"
#include "GuiConfiguration.hh"

namespace livecast {
namespace gui {

enum popup_menu_t
{
  POPUP_INFOS = 1,
  POPUP_STATUS_DETAILS,
  POPUP_PIPELINE,
  POPUP_REINIT,
};

enum column_id_t
{
  ID,
  STATUS,
  MODE,
  PROTOCOL,
  SRC_IP,
  BACKLOG,
};

}
}

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;
using namespace livecast::lib;

class LivecastListCtrlVirtual : public LivecastListCtrl
{
public:
  LivecastListCtrlVirtual(wxWindow * parent, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor);

protected:
  wxString OnGetItemText(long item, long column) const;
  wxListItemAttr * OnGetItemAttr(long item) const;

private:
  boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor;

};

LivecastListCtrlVirtual::LivecastListCtrlVirtual(wxWindow * parent, boost::shared_ptr<LivecastMonitor> monitor)
  : LivecastListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL),
    monitor(monitor)
{
}

wxString LivecastListCtrlVirtual::OnGetItemText(long item, long column) const
{
  LogError::getInstance().sysLog(DEBUG, "ask value for (%d, %d)", item, column);

  const char * value = "";
  std::ostringstream oss;
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().begin();
  std::advance(it, item);
  if (it != cfg->getStreamsInfos().end())
  {
    switch (column)
    {
    case ID: 
      oss << it->second->getId();
      value = oss.str().c_str();
      break;
    case STATUS: 
      oss << it->second->getStatus();
      value = oss.str().c_str();
      break;
    case MODE:     value = it->second->getInfos()[StreamInfos::FIELDS_MODE].c_str(); break;
    case PROTOCOL: value = it->second->getInfos()[StreamInfos::FIELDS_PROTOCOL].c_str(); break;
    case SRC_IP:   value = it->second->getInfos()[StreamInfos::FIELDS_SRC_IP].c_str(); break;
    case BACKLOG:  value = it->second->getInfos()[StreamInfos::FIELDS_BACKLOG].c_str(); break;
    default:
      LogError::getInstance().sysLog(ERROR, "cannot find column %d", column);
      assert(false);
    }
  }
    
  return _(value);
}

wxListItemAttr * LivecastListCtrlVirtual::OnGetItemAttr(long item) const
{
  wxColour fgColour;
  wxColour bgColour;
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().begin();
  std::advance(it, item);
  if (it != cfg->getStreamsInfos().end())
  {
    switch (it->second->getStatus())
    {  
    case StreamInfos::STATUS_WAITING:      fgColour = livecast_grey;     break;
    case StreamInfos::STATUS_INITIALIZING: fgColour = livecast_yellow;   break;
    case StreamInfos::STATUS_RUNNING:      fgColour = livecast_green;    break;
    case StreamInfos::STATUS_ERROR:        fgColour = livecast_red;      break;
    case StreamInfos::STATUS_UNKNOWN:      fgColour = livecast_darkGrey; break;
    }  
    bgColour = (((item % 2) == 0) ? livecast_lightYellow : livecast_lightBlue);
  }

  wxListItemAttr * attr = new wxListItemAttr(fgColour, bgColour, wxFont());
  return attr;
}

//
//

LivecastResult::LivecastResult(wxWindow * parent, LivecastGui * livecastGui, boost::shared_ptr<LivecastMonitor> monitor)
  : wxPanel(parent, 
            wxID_ANY, 
            wxDefaultPosition, 
            wxDefaultSize, 
            wxBORDER_NONE),
    streamsListEvent(wxNewEventType()),
    checkStreamEvent(wxNewEventType()),
    monitor(monitor),
    livecastGui(livecastGui)
{
  this->splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH);

  if (GuiConfiguration::getInstance()->useVirtualStreamList())
  {
    this->list = new LivecastListCtrlVirtual(this->splitter, monitor);
  }
  else
  {
    this->list =  new LivecastListCtrl(this->splitter);
  }

  this->list->InsertColumn(ID, "Stream ID", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(STATUS, "Status", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(MODE, "Mode", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(PROTOCOL, "Protocol", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(SRC_IP, "Source IP", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(BACKLOG, "Backlog", wxLIST_FORMAT_LEFT);

  this->infos = new LivecastInfos(this->splitter);
  this->infos->Show(false);

  this->splitter->Initialize(this->list);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->splitter, 1, wxEXPAND | wxALL, 0);
  this->SetSizer(box);
  
  this->list->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastResult::onStreamListDblClicked, this, wxID_ANY);
  this->list->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastResult::onStreamListItemActivated, this, wxID_ANY);
  this->list->Bind(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, &LivecastResult::onStatusListRightClicked, this, wxID_ANY);
  this->Connect(streamsListEvent, wxCommandEventHandler(LivecastResult::onStreamListUpdate));
  this->Connect(checkStreamEvent, wxCommandEventHandler(LivecastResult::onCheckStream));
}

LivecastResult::~LivecastResult()
{
}

std::list<unsigned int> LivecastResult::getStreamsSelected() const
{
  std::list<unsigned int> lId;
  long itemIndex = -1;
  for (;;) 
  {
    itemIndex = this->list->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
    if (itemIndex == -1) break; 
    lId.push_back(this->list->GetItemData(itemIndex));
  }
  return lId;
}

void LivecastResult::onStreamListUpdate(wxCommandEvent& WXUNUSED(event))
{
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  if (this->list->IsVirtual())
  {
    LogError::getInstance().sysLog(ERROR, "stream list update [size : %d]", cfg->getStreamsInfos().size());
    this->list->SetItemCount(cfg->getStreamsInfos().size());
  }
  else
  {
    unsigned int n = 0;
    for (MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().begin(); it != cfg->getStreamsInfos().end(); ++it)
    {
      n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(cfg->getStreamsInfos().begin()), it);
      if ((int)n >= this->list->GetItemCount())
      {
        wxListItem item;
        item.SetId(n);
        item.SetData(it->second->getId());
#ifndef NDEBUG        
        unsigned int n2 = 
#endif
          this->list->InsertItem(item);
        assert(n == n2);
      }
      else
      {
        this->list->SetItemData(n, it->second->getId());
      }
      this->updateItem(it);
    }
    for (int i = n + 1; i < this->list->GetItemCount(); i++)
    {
      this->list->DeleteItem(i);
    }
  }
  this->infos->refresh();
}

void LivecastResult::onStreamListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "double click on item %d with data %d", event.GetIndex(), event.GetItem().GetData());

  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  if (this->list->IsVirtual())
  {
    MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().begin();
    std::advance(it, event.GetIndex());
    if (it != cfg->getStreamsInfos().end())
    {
      this->livecastGui->check(it->second->getId());
    }
  }
  else
  {
    this->livecastGui->check(event.GetItem().GetData());
  }
}

void LivecastResult::onStreamListItemActivated(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "item %d with data %d is selected", event.GetIndex(), event.GetItem().GetData());
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().begin();
  std::advance(it, event.GetIndex());
  assert(it != cfg->getStreamsInfos().end());
  std::list<boost::shared_ptr<ResultCallbackIntf> > cbs = this->infos->setInfos(it->second);
  std::for_each(cbs.begin(), cbs.end(), boost::bind(&LivecastMonitor::check, this->monitor, it->second->getId(), _1));
  this->infos->Show(true);
  this->splitter->SplitHorizontally(this->list, this->infos, 0);
}

void LivecastResult::onCheckStream(wxCommandEvent& event)
{
  unsigned int streamId = (unsigned int)event.GetInt();
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().find(streamId);
  assert(it != cfg->getStreamsInfos().end());

  if (this->list->IsVirtual())
  {
    unsigned int n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(cfg->getStreamsInfos().begin()), it);
    this->list->RefreshItem(n);
  }
  else
  {
    this->updateItem(it);
  }
}

void LivecastResult::onStatusListRightClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "right click on stream %u", event.GetItem().GetData());
  void *data = reinterpret_cast<void *>(event.GetItem().GetData());
	wxMenu menu;
  menu.SetClientData(data);
	menu.Append(POPUP_INFOS, "Get Infos");
	menu.Append(POPUP_STATUS_DETAILS, "Get Stream Status Details");
	menu.Append(POPUP_PIPELINE, "Edit Stream Pipeline");
	menu.Append(POPUP_REINIT, "Reinit Stream");
  menu.Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastResult::onPopupClick, this, wxID_ANY);
	this->PopupMenu(&menu);
}

void LivecastResult::onPopupClick(wxCommandEvent& event)
{
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  unsigned int streamId = reinterpret_cast<long>(static_cast<wxMenu *>(event.GetEventObject())->GetClientData());
	switch(event.GetId()) 
  {
  case POPUP_INFOS:
  {
    LogError::getInstance().sysLog(DEBUG, "popup infos %u", streamId);
    MonitorConfiguration::map_streams_infos_t::const_iterator it = cfg->getStreamsInfos().find(streamId);
    assert(it != cfg->getStreamsInfos().end());
    std::list<boost::shared_ptr<ResultCallbackIntf> > cbs = this->infos->setInfos(it->second);
    std::for_each(cbs.begin(), cbs.end(), boost::bind(&livecast::monitor::LivecastMonitor::check, this->monitor, it->second->getId(), _1));
    this->infos->Show(true);
    this->splitter->SplitHorizontally(this->list, this->infos, 0);
  }
  break;
  case POPUP_STATUS_DETAILS:
  {
    this->livecastGui->check(streamId);
  }
  break;
  case POPUP_PIPELINE:
  {
    LogError::getInstance().sysLog(DEBUG, "popup edit pipeline %u", streamId);
    std::ostringstream title;
    title << "Edit pipeline of stream " << streamId;
    LivecastEditPipeline * editPipeline = new LivecastEditPipeline(this->livecastGui, this->monitor, streamId);
    this->livecastGui->addTab(editPipeline, title.str().c_str());
  }
  break;
  case POPUP_REINIT:
  {
    LogError::getInstance().sysLog(DEBUG, "popup reinit %u", streamId);
    std::ostringstream title;
    title << "you are going to reinit stream " << streamId;
    wxDialog * dlg = new wxMessageDialog(this, title.str(), title.str(), wxOK | wxCANCEL | wxCENTRE);
    if (dlg->ShowModal() == wxID_OK)
    {
      LogError::getInstance().sysLog(DEBUG, "reinit %u", streamId);
      this->monitor->reinitStream(streamId);
    }
    else
    {
      LogError::getInstance().sysLog(ERROR, "do not reinit %u", streamId);
    }
    dlg->Destroy();
  }
  break;
	}
}

void LivecastResult::updateItem(MonitorConfiguration::map_streams_infos_t::const_iterator& it)
{
  boost::shared_ptr<MonitorConfiguration> cfg = this->monitor->getConfiguration();
  unsigned int n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(cfg->getStreamsInfos().begin()), it);
  
  this->list->SetItemTextColour(n, it->second->getStatus());
  this->list->SetItemBackgroundColour(n, wxColour(wxColour(((n % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));
  
  std::ostringstream ossStreamId;
  std::ostringstream ossStreamStatus;
  ossStreamId << it->second->getId();
  ossStreamStatus << it->second->getStatus();
  
  LogError::getInstance().sysLog(DEBUG, "item %u : %s => %s", n, ossStreamId.str().c_str(), ossStreamStatus.str().c_str());

  this->list->UpdateItem(n, ID, ossStreamId.str());
  this->list->UpdateItem(n, STATUS, ossStreamStatus.str());
  this->list->UpdateItem(n, MODE, it->second->getInfos()[StreamInfos::FIELDS_MODE]);
  this->list->UpdateItem(n, PROTOCOL, it->second->getInfos()[StreamInfos::FIELDS_PROTOCOL]);
  this->list->UpdateItem(n, SRC_IP, it->second->getInfos()[StreamInfos::FIELDS_SRC_IP]);
  this->list->UpdateItem(n, BACKLOG, it->second->getInfos()[StreamInfos::FIELDS_BACKLOG]);

}
