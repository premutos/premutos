#include "../lib/Log.hh"
#include "LivecastResult.hh"
#include "LivecastGui.hh"
#include "LivecastStatus.hh"
#include "GuiConfiguration.hh"

#include <boost/property_tree/xml_parser.hpp>

namespace livecast {
namespace gui {

static const wxColour lightYellow(255, 255, 226);
static const wxColour lightBlue(226, 255, 255);
static const wxColour orange(225, 143, 26);

enum column_id_t
{
  ID,
  STATUS,
  MODE,
  PROTOCOL,
  SRC_IP,
  BACKLOG,
};

std::ostream& operator<<(std::ostream& os, const livecast::monitor::StreamInfos::status_t status)
{
  using namespace livecast::monitor;
  switch (status)
  {
  case StreamInfos::STATUS_WAITING:      os << "WAITING";      break;
  case StreamInfos::STATUS_INITIALIZING: os << "INITIALIZING"; break;
  case StreamInfos::STATUS_RUNNING:      os << "RUNNING";      break;
  case StreamInfos::STATUS_ERROR:        os << "ERROR";        break;
  case StreamInfos::STATUS_UNKNOWN:      os << "UNKNOWN";      break;
  default:
    LogError::getInstance().sysLog(CRITICAL, "bad status %d", status);
    assert(false);
  }
  return os;
}

}
}

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

LivecastListCtrl::LivecastListCtrl(wxWindow* parent, boost::shared_ptr<LivecastMonitor> monitor)
  : wxListCtrl(parent,wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES),
    monitor(monitor)
{
}

wxString LivecastListCtrl::OnGetItemText(long item, long column) const
{
  LogError::getInstance().sysLog(DEBUG, "ask value for (%d, %d)", item, column);

  const char * value;
  std::ostringstream oss;
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->monitor->getStreams().begin();
  std::advance(it, item);
  if (it != this->monitor->getStreams().end())
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
    case MODE:     value = it->second->infos[StreamInfos::FIELDS_MODE].c_str(); break;
    case PROTOCOL: value = it->second->infos[StreamInfos::FIELDS_PROTOCOL].c_str(); break;
    case SRC_IP:   value = it->second->infos[StreamInfos::FIELDS_SRC_IP].c_str(); break;
    case BACKLOG:  value = it->second->infos[StreamInfos::FIELDS_BACKLOG].c_str(); break;
    default:
      LogError::getInstance().sysLog(ERROR, "cannot find column %d", column);
      assert(false);
    }
  }
    
  return _(value);
}

wxListItemAttr * LivecastListCtrl::OnGetItemAttr(long item) const
{
  wxColour fgColour;
  wxColour bgColour;
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->monitor->getStreams().begin();
  std::advance(it, item);
  if (it != this->monitor->getStreams().end())
  {
    switch (it->second->getStatus())
    {  
    case StreamInfos::STATUS_WAITING:      fgColour = *wxLIGHT_GREY; break;
    case StreamInfos::STATUS_INITIALIZING: fgColour = *wxBLUE;       break;
    case StreamInfos::STATUS_RUNNING:      fgColour = *wxGREEN;      break;
    case StreamInfos::STATUS_ERROR:        fgColour = *wxRED;        break;
    case StreamInfos::STATUS_UNKNOWN:      fgColour = orange; break;
    }  
    bgColour = (((item % 2) == 0) ? lightYellow : lightBlue);
  }

  wxListItemAttr * attr = new wxListItemAttr(fgColour, bgColour, wxFont());
  return attr;
}

//
//

LivecastResult::LivecastResult(LivecastGui * livecastGui, boost::shared_ptr<LivecastMonitor> monitor)
  : wxPanel(livecastGui, 
            wxID_ANY, 
            wxDefaultPosition, 
            wxDefaultSize, 
            wxBORDER_NONE),
    monitor(monitor),
    livecastGui(livecastGui),
    streamListEvent(wxNewEventType()),
    checkStreamEvent(wxNewEventType())
{
  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);

  if (GuiConfiguration::getInstance()->useVirtualStreamList())
  {
    this->list = new LivecastListCtrl(this, monitor);
  }
  else
  {
    this->list =  new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
  }
  this->list->InsertColumn(ID, "Stream ID", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(STATUS, "Status", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(MODE, "Mode", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(PROTOCOL, "Protocol", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(SRC_IP, "Source IP", wxLIST_FORMAT_LEFT);
  this->list->InsertColumn(BACKLOG, "Backlog", wxLIST_FORMAT_LEFT);

  hbox->Add(this->list, 1, wxEXPAND | wxALL, 10);
  this->SetSizer(hbox);

  this->Bind(wxEVT_PAINT, &LivecastResult::OnPaint, this, wxID_ANY);
  this->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastResult::onStreamListDblClicked, this, wxID_ANY);
  this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastResult::onRefresh, this, wxID_APPLY);
  this->Connect(streamListEvent, wxCommandEventHandler(LivecastResult::onStreamListUpdate));
  this->Connect(checkStreamEvent, wxCommandEventHandler(LivecastResult::onCheckStream));
}

LivecastResult::~LivecastResult()
{
}

void LivecastResult::commitStreamList()
{
  LogError::getInstance().sysLog(DEBUG, "commit stream list");
  wxCommandEvent event(streamListEvent);
  this->GetEventHandler()->AddPendingEvent(event);
}

void LivecastResult::commitCheckStream(unsigned int streamId)
{
  LogError::getInstance().sysLog(DEBUG, "commit check stream %d", streamId);
  wxCommandEvent event(checkStreamEvent);
  event.SetInt(streamId);
  this->GetEventHandler()->AddPendingEvent(event);
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

boost::shared_ptr<LivecastStatus> LivecastResult::getStreamStatus(unsigned int streamId)
{
  status_frames_t::iterator it = this->statusFrames.find(streamId);
  try
  {
    if (it == this->statusFrames.end())
    {
      LogError::getInstance().sysLog(DEBUG, "create status frame %d", streamId);
      std::ostringstream oss;
      oss << "status of stream " << streamId;
      boost::shared_ptr<StreamInfos> streamInfos = this->monitor->getStreamInfos(streamId);
      boost::shared_ptr<LivecastStatus> statusFrame(new LivecastStatus(this, oss.str(), streamInfos));
      statusFrame->Centre();
      statusFrame->Show();
      std::pair<status_frames_t::iterator, bool> res = this->statusFrames.insert(std::make_pair(streamId, statusFrame));
      if (res.second)
      {
        it = res.first;
      }
      else
      {
        assert(false);
        LogError::getInstance().sysLog(CRITICAL, "cannot find informations for stream id %u", streamId);
        exit(-1);
      }
    }
  }
  catch (const StreamInfosException& ex)
  {
    LogError::getInstance().sysLog(ERROR, "cannot load information for stream id %u", streamId);
  }
  // fixme
  return it->second;
}

int LivecastResult::removeStreamStatus(unsigned int streamId)
{
  int rc = 0;
  status_frames_t::iterator it = this->statusFrames.find(streamId);
  if (it != this->statusFrames.end())
  {
    this->statusFrames.erase(it);
  }
  else
  {
    rc = -1;
    LogError::getInstance().sysLog(ERROR, "cannot find status for stream id %d", streamId);
  }
  return rc;
}

void LivecastResult::onStreamListUpdate(wxCommandEvent& WXUNUSED(event))
{
  if (this->list->IsVirtual())
  {
    LogError::getInstance().sysLog(ERROR, "stream list update [size : %d]", this->monitor->getStreams().size());
    this->list->SetItemCount(this->monitor->getStreams().size());
  }
  else
  {
    this->list->DeleteAllItems();
    for (MonitorConfiguration::map_streams_infos_t::const_iterator it = this->monitor->getStreams().begin(); it != this->monitor->getStreams().end(); ++it)
    {
      unsigned int n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(this->monitor->getStreams().begin()), it);
      wxListItem item;
      std::cout << n << " ";
      item.SetId(n);
      item.SetData(it->second->getId());
      unsigned int n2 = this->list->InsertItem(item);
      assert(n == n2);
      this->updateItem(it);
    }
    std::cout << std::endl;
  }
}

void LivecastResult::onStreamListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(ERROR, "double click on item %d with data %d", event.GetIndex(), event.GetItem().GetData());

  if (this->list->IsVirtual())
  {
    MonitorConfiguration::map_streams_infos_t::const_iterator it = this->monitor->getStreams().begin();
    std::advance(it, event.GetIndex());
    if (it != this->monitor->getStreams().end())
    {
      this->livecastGui->check(it->second->getId());
    }
  }
  else
  {
    this->livecastGui->check(event.GetItem().GetData());
  }
}

void LivecastResult::onCheckStream(wxCommandEvent& event)
{
  unsigned int streamId = (unsigned int)event.GetInt();
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->monitor->getStreams().find(streamId);
  assert(it != this->monitor->getStreams().end());

  if (this->list->IsVirtual())
  {
    unsigned int n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(this->monitor->getStreams().begin()), it);
    this->list->RefreshItem(n);
  }
  else
  {
    this->updateItem(it);
  }
}

void LivecastResult::onRefresh(wxCommandEvent& WXUNUSED(event))
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
}

void LivecastResult::updateItem(MonitorConfiguration::map_streams_infos_t::const_iterator& it)
{
  unsigned int n = std::distance(MonitorConfiguration::map_streams_infos_t::const_iterator(this->monitor->getStreams().begin()), it);
  
  switch (it->second->getStatus())
  {  
  case StreamInfos::STATUS_WAITING:      this->list->SetItemTextColour(n, wxColour(wxColour(*wxLIGHT_GREY))); break;
  case StreamInfos::STATUS_INITIALIZING: this->list->SetItemTextColour(n, wxColour(wxColour(*wxBLUE)));       break;
  case StreamInfos::STATUS_RUNNING:      this->list->SetItemTextColour(n, wxColour(wxColour(*wxGREEN)));      break;
  case StreamInfos::STATUS_ERROR:        this->list->SetItemTextColour(n, wxColour(wxColour(*wxRED)));        break;
  case StreamInfos::STATUS_UNKNOWN:      this->list->SetItemTextColour(n, wxColour(wxColour(orange))); break;
  }  
  this->list->SetItemBackgroundColour(n, wxColour(wxColour(((n % 2) == 0) ? lightYellow : lightBlue)));
  
  std::ostringstream ossStreamId;
  std::ostringstream ossStreamStatus;
  ossStreamId << it->second->getId();
  ossStreamStatus << it->second->getStatus();
  
  LogError::getInstance().sysLog(DEBUG, "item %u : %s => %s", n, ossStreamId.str().c_str(), ossStreamStatus.str().c_str());
  this->list->SetItem(n, ID, ossStreamId.str());
  this->list->SetItem(n, STATUS, ossStreamStatus.str());
  this->list->SetItem(n, MODE, it->second->infos[StreamInfos::FIELDS_MODE]);
  this->list->SetItem(n, PROTOCOL, it->second->infos[StreamInfos::FIELDS_PROTOCOL]);
  this->list->SetItem(n, SRC_IP, it->second->infos[StreamInfos::FIELDS_SRC_IP]);
  this->list->SetItem(n, BACKLOG, it->second->infos[StreamInfos::FIELDS_BACKLOG]);
}

void LivecastResult::OnPaint(wxPaintEvent& ev)
{
  wxSize size = this->list->GetSize();
  unsigned int width = (BACKLOG + 1);
  for (unsigned int i = 0; i < width; i++)
  {
    this->list->SetColumnWidth(i, size.GetWidth() / width);
  }
  ev.Skip();
}
