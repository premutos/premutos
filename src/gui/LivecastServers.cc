#include "Util.hh"
#include "LivecastListCtrl.hh"
#include "LivecastServers.hh"
#include "LivecastInfos.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../monitor/LivecastConnection.hh"
#include "../monitor/MonitorConfiguration.hh"
#include "../lib/Log.hh"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace livecast;
using namespace livecast::gui;
using namespace livecast::monitor;

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
  this->statusList = new LivecastListCtrl(this->results);
  colIndex = 0;
  this->statusList->InsertColumn(colIndex++, "host", wxLIST_FORMAT_LEFT);
  this->statusList->InsertColumn(colIndex++, "type", wxLIST_FORMAT_LEFT);
  this->statusList->InsertColumn(colIndex++, "id", wxLIST_FORMAT_LEFT);
  this->statusList->InsertColumn(colIndex++, "status", wxLIST_FORMAT_LEFT);
  this->results->InsertPage(0, this->statusList, "status list", true);
  this->results->Show(false);
  
  this->splitter->Initialize(this->servers);

  wxBoxSizer * box = new wxBoxSizer(wxVERTICAL);
  box->Add(this->splitter, 1, wxEXPAND | wxALL, 0);
  this->SetSizer(box);

  this->servers->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastServers::onServersListDblClicked, this, wxID_ANY);
  this->statusList->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastServers::onResultsListDblClicked, this, wxID_ANY);
  this->statusList->Bind(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, &LivecastServers::onStatusListRightClicked, this, wxID_ANY);
  this->results->Bind(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, &LivecastServers::onTabMiddleUp, this, wxID_ANY);
  this->Connect(serversListEvent, wxCommandEventHandler(LivecastServers::onServersListUpdate));

  this->Show(true);
}

void LivecastServers::refresh()
{
  unsigned int index = 0;
  const MonitorConfiguration::connections_t& conns = monitor->getConfiguration()->getConnections();
  MonitorConfiguration::connections_t::const_iterator it;
  this->servers->DeleteAllItems();
  this->statusList->DeleteAllItems();
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

    this->servers->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
    index++;
  }
}

void LivecastServers::onServersListUpdate(wxCommandEvent& WXUNUSED(event))
{
  this->refresh();
}

void LivecastServers::onStatusListRightClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "right click on stream %u", event.GetItem().GetData());
  void *data = reinterpret_cast<void *>(event.GetItem().GetData());
	wxMenu menu;
  menu.SetClientData(data);
	menu.Append(POPUP_INFOS_DETAILS, "Get More Details");
	menu.Append(POPUP_REINIT, "Reinit Stream");
  menu.Bind(wxEVT_COMMAND_MENU_SELECTED, &LivecastServers::onPopupClick, this, wxID_ANY);
	this->PopupMenu(&menu);
}

void LivecastServers::onPopupClick(wxCommandEvent& event)
{
  unsigned int streamId = reinterpret_cast<long>(static_cast<wxMenu *>(event.GetEventObject())->GetClientData());
	switch(event.GetId()) 
  {
		case POPUP_INFOS_DETAILS:
      LogError::getInstance().sysLog(ERROR, "popup infos details %u", streamId);
			break;
		case POPUP_REINIT:
      LogError::getInstance().sysLog(ERROR, "popup reinit %u", streamId);
			break;
	}
}

void LivecastServers::onTabMiddleUp(wxAuiNotebookEvent& event)
{
  if (event.GetSelection() > 0)
  {
    this->results->DeletePage(event.GetSelection());
  }
}

void LivecastServers::onServersListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "double click on server item %d", event.GetIndex());
  const MonitorConfiguration::connections_t& conns = this->monitor->getConfiguration()->getConnections();
  MonitorConfiguration::connections_t::const_iterator it = conns.begin();
  std::advance(it, event.GetIndex());
  assert(it != conns.end());

  // clean
  this->statusList->DeleteAllItems();
  for (unsigned int p = this->results->GetPageCount() - 1; p > 0; p--)
  {
    this->results->DeletePage(p);
  }

  if (it != conns.end())
  {
    LogError::getInstance().sysLog(DEBUG, "[host:port] => [%s:%u]", it->first.first.c_str(), it->first.second);
    boost::shared_ptr<boost::property_tree::ptree> result(new boost::property_tree::ptree);
    this->host = it->first.first;
    this->type = monitor->getConfiguration()->getServerFromPort(it->first.second);
    this->connectionTmp = it->second;
    this->connectionTmp->check(0, result);
    this->fillList(this->statusList, *result);
    this->results->Show(true);
    this->splitter->SplitHorizontally(this->servers, this->results, 0);
  }
}

void LivecastServers::onResultsListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "double click on result item %d", event.GetIndex());

  try
  {
    unsigned int streamId = boost::lexical_cast<unsigned int>(this->statusList->GetItemText(event.GetIndex(), 2));
    bool detail = true;
    boost::shared_ptr<boost::property_tree::ptree> result(new boost::property_tree::ptree);  
    this->connectionTmp->check(streamId, result, detail);      
    LivecastListCtrl * list = new LivecastListCtrl(this->results);
    unsigned int colIndex = 0;
    list->InsertColumn(colIndex++, "host", wxLIST_FORMAT_LEFT);
    list->InsertColumn(colIndex++, "type", wxLIST_FORMAT_LEFT);
    list->InsertColumn(colIndex++, "id", wxLIST_FORMAT_LEFT);
    list->InsertColumn(colIndex++, "status", wxLIST_FORMAT_LEFT);    
    this->fillList(list, *result);
    std::ostringstream title;
    title << "status details " << streamId;
    this->results->InsertPage(this->results->GetPageCount(), list, title.str(), true);
  }
  catch (const boost::bad_lexical_cast& ex)
  {
    LogError::getInstance().sysLog(ERROR, "bad stream id: %s", ex.what());
  }
  catch (const std::exception& ex)
  {
    LogError::getInstance().sysLog(ERROR, "bad stream id: %s", ex.what());
  }
  catch (...)
  {
    LogError::getInstance().sysLog(ERROR, "unkwnow error");
  }
}

void LivecastServers::fillList(LivecastListCtrl * list, boost::property_tree::ptree& result) const
{
  for (boost::property_tree::ptree::const_iterator itResult = result.get_child("").begin(); 
       itResult != result.get_child("").end(); ++itResult)
  {
    const std::string status = itResult->second.get<std::string>("result");
    LogError::getInstance().sysLog(DEBUG, "result => \n%s", status.c_str());

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("\n");
    tokenizer tok(status, sep);
    unsigned int index = 0;
    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
    {
      std::string streamId = (*it);
      std::string statusStr = (*it);
      const std::string::size_type pos = (*it).find(" ");
      if (pos != std::string::npos)
      {
        streamId = (*it).substr(0, pos);
        statusStr = (*it).substr(pos + 1);
      }
      
      unsigned int id = 0;
      try 
      {
        id = boost::lexical_cast<unsigned int>(streamId.c_str());
      }
      catch (const boost::bad_lexical_cast& ex)
      {
        streamId = "n/a";
      }
      
      wxListItem item;
      item.SetId(index);
      item.SetData(id);
      list->InsertItem(item);

      list->SetItem(index, 0, this->host.c_str());
      list->SetItem(index, 1, this->type.c_str());
      list->SetItem(index, 2, streamId.c_str());
      list->SetItem(index, 3, statusStr.c_str());

      StreamInfos::status_t status = StreamInfos::parseStatus(*it);
      list->SetItemTextColour(index, status);
      list->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
      index++;
    }
  }
}

