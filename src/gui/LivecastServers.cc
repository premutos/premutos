#include "Util.hh"
#include "LivecastListCtrl.hh"
#include "LivecastServers.hh"
#include "LivecastInfos.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../monitor/LivecastConnection.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../lib/Log.hh"

#include <wx/treelistctrl.hh>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace livecast {
namespace gui {

class LivecastTreeListCtrl : public wxTreeListCtrl
{
public:
  LivecastTreeListCtrl(wxWindow * parent)
    : wxTreeListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_ROW_LINES),
      sortOrder(true)
    {
      this->Bind(wxEVT_PAINT, &LivecastTreeListCtrl::onPaint, this, wxID_ANY);
      this->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &LivecastTreeListCtrl::onSort, this, wxID_ANY);
    }

protected:
  void onPaint(wxPaintEvent& ev)
    {
      wxSize size = this->GetSize();
      unsigned int width = this->GetColumnCount();
      for (unsigned int i = 0; i < width; i++)
      {
        this->SetColumnWidth(i, size.GetWidth() / width);
      }
      ev.Skip();
    }
  void onSort(wxListEvent& ev)
    {
      livecast::lib::LogError::getInstance().sysLog(ERROR, "click on col %d", ev.GetColumn());
      this->SortChildren(this->GetRootItem(), ev.GetColumn(), this->sortOrder);
      this->sortOrder = !this->sortOrder;
    }
private:
  bool sortOrder;
};

}
}

using namespace livecast;
using namespace livecast::gui;
using namespace livecast::monitor;
using namespace livecast::lib;

enum popup_menu_t
{
  POPUP_INFOS_DETAILS = 1,
  POPUP_REINIT,
};

LivecastServers::LivecastServers(wxWindow * parent, boost::shared_ptr<livecast::monitor::LivecastMonitor> monitor)
  : wxPanel(parent, wxID_ANY),
    serversListEvent(wxNewEventType()),
    monitor(monitor)
{  
  this->splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH);

  this->servers = new LivecastListCtrl(this->splitter);

  unsigned int colIndex = 0;
  this->servers->InsertColumn(colIndex++, "type", wxLIST_FORMAT_LEFT);
  this->servers->InsertColumn(colIndex++, "hostname", wxLIST_FORMAT_LEFT); 
  this->servers->InsertColumn(colIndex++, "ip", wxLIST_FORMAT_LEFT);
  this->servers->InsertColumn(colIndex++, "admin port", wxLIST_FORMAT_LEFT);

  this->results = new wxAuiNotebook(this->splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
  
  this->splitter->Initialize(this->servers);

  wxBoxSizer * box = new wxBoxSizer(wxVERTICAL);
  box->Add(this->splitter, 1, wxEXPAND | wxALL, 0);
  this->SetSizer(box);

  // bind servers event
  this->servers->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastServers::onServersListDblClicked, this, wxID_ANY);
  this->Connect(serversListEvent, wxCommandEventHandler(LivecastServers::onServersListUpdate));

  // bind results event
  this->results->Bind(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, &LivecastServers::onTabMiddleUp, this, wxID_ANY);

  this->Show(true);
}

void LivecastServers::refresh()
{
  unsigned int index = 0;
  const MonitorConfiguration::connections_t& conns = monitor->getConfiguration()->getConnections();
  MonitorConfiguration::connections_t::const_iterator it;
  this->servers->DeleteAllItems();
  // this->statusList->DeleteAllItems();
  this->results->Show(false);
  for (it = conns.begin(); it != conns.end(); ++it)
  {
    wxListItem item;
    item.SetId(index);
    this->servers->InsertItem(item);

    std::string ip = "n/a";
    struct hostent * entry = ::gethostbyname(it->first.first.c_str());
    if (entry != 0)
    {
      ip = ::inet_ntoa(*((in_addr*)entry->h_addr));
    }

    std::ostringstream portSS;
    portSS << it->first.second;
    this->servers->SetItem(index, 0, monitor->getConfiguration()->getServerFromPort(it->first.second).c_str());
    this->servers->SetItem(index, 1, it->first.first.c_str());    
    this->servers->SetItem(index, 2, ip);
    this->servers->SetItem(index, 3, portSS.str().c_str());

    this->servers->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));
    index++;
  }
}

void LivecastServers::onServersListUpdate(wxCommandEvent& WXUNUSED(event))
{
  this->refresh();
}

void LivecastServers::onServersListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "double click on server item %d", event.GetIndex());
  const MonitorConfiguration::connections_t& conns = this->monitor->getConfiguration()->getConnections();
  MonitorConfiguration::connections_t::const_iterator it = conns.begin();
  std::advance(it, event.GetIndex());
  assert(it != conns.end());

  if (it != conns.end())
  {
    LogError::getInstance().sysLog(DEBUG, "[host:port] => [%s:%u]", it->first.first.c_str(), it->first.second);
    boost::shared_ptr<boost::property_tree::ptree> result(new boost::property_tree::ptree);
    this->connectionTmp = it->second;
    this->connectionTmp->status(0, result, false);
    this->connectionTmp->status(0, result, true);

    std::ostringstream title;
    title << "status on " << " " << it->first.first << " [" << monitor->getConfiguration()->getServerFromPort(it->first.second) << "]";
    wxTreeListCtrl * status = new LivecastTreeListCtrl(this->results);
    this->fillStatus(status, *result);    
    this->results->InsertPage(this->results->GetPageCount(), status, title.str(), true);
    this->results->Show(true);

    // resize columns
    wxSize size = status->GetSize();
    unsigned int width = status->GetColumnCount();
    for (unsigned int i = 0; i < width; i++)
    {
      status->SetColumnWidth(i, size.GetWidth() / width);
    }

    this->splitter->SplitHorizontally(this->servers, this->results, 0);
  }
}

void LivecastServers::onTabMiddleUp(wxAuiNotebookEvent& event)
{
  this->results->DeletePage(event.GetSelection());
}

void LivecastServers::fillStatus(wxTreeListCtrl * statusTree, boost::property_tree::ptree& result) const
{

//   std::cout << std::endl 
//             << "============================================"
//             << std::endl;
//   boost::property_tree::write_xml(std::cout, result);
//   std::cout << std::endl 
//             << "============================================"
//             << std::endl;

  statusTree->SetBackgroundColour(wxColour(240,240,240));

  statusTree->AddColumn ("Stream ID");
  statusTree->AddColumn ("status");
  statusTree->AddColumn ("message");

  wxTreeItemId root = statusTree->AddRoot ("");

  unsigned int n = 0;
  wxTreeItemId parent;
  wxTreeItemId item;
  std::map<unsigned int, wxTreeItemId> streamIds;
  for (boost::property_tree::ptree::const_iterator itResult = result.get_child("").begin(); 
       itResult != result.get_child("").end(); ++itResult)
  {
    const std::string status = itResult->second.get<std::string>("result");
    LogError::getInstance().sysLog(DEBUG, "result => \n%s", status.c_str());

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("\n");
    tokenizer tok(status, sep);
    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
    {
      std::string streamId = (*it);
      std::string statusStr = (*it);
      std::string messageStr = (*it);
      const std::string::size_type pos = (*it).find(" ");
      if (pos != std::string::npos)
      {
        streamId = (*it).substr(0, pos);
        statusStr = (*it).substr(pos + 1);
        messageStr = (*it).substr(pos + 1);
      }
      
      unsigned int id = 0;
      try 
      {
        id = boost::lexical_cast<unsigned int>(streamId.c_str());
      }
      catch (const boost::bad_lexical_cast& ex)
      {
        continue;
      }

      std::map<unsigned int, wxTreeItemId>::const_iterator itStream = streamIds.find(id);
      if (itStream != streamIds.end())
      {
        // add status details
        item = statusTree->AppendItem (itStream->second, streamId.c_str());
      }
      else
      {
        // add entry for this stream
        item = statusTree->AppendItem (root, streamId.c_str());
        streamIds.insert(std::make_pair(id, item));
      }
      
      unsigned int col = 1;
      statusTree->SetItemText(item, col++, statusStr.c_str());
      statusTree->SetItemText(item, col++, messageStr.c_str());

      StreamInfos::status_t status = StreamInfos::parseStatus(*it);

      switch (status)
      {  
      case StreamInfos::STATUS_WAITING:      statusTree->SetItemTextColour(item, wxColour(wxColour(livecast_grey)));     break;
      case StreamInfos::STATUS_INITIALIZING: statusTree->SetItemTextColour(item, wxColour(wxColour(livecast_yellow)));   break;
      case StreamInfos::STATUS_RUNNING:      statusTree->SetItemTextColour(item, wxColour(wxColour(livecast_green)));    break;
      case StreamInfos::STATUS_ERROR:        statusTree->SetItemTextColour(item, wxColour(wxColour(livecast_red)));      break;
      case StreamInfos::STATUS_UNKNOWN:      statusTree->SetItemTextColour(item, wxColour(wxColour(livecast_darkGrey))); break;
      }  

      statusTree->SetItemBackgroundColour(item, wxColour(wxColour(((n % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));
      n++;

    }
  }
  
  statusTree->Expand(root);
}

