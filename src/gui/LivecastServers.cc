#include "Util.hh"
#include "LivecastServers.hh"
#include "LivecastInfos.hh"
#include "../monitor/LivecastMonitor.hh"
#include "../lib/Log.hh"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace livecast;
using namespace livecast::gui;
using namespace livecast::monitor;

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

  this->results = new wxNotebook(this->splitter, wxID_ANY);
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
  box->Add(this->splitter, 1, wxEXPAND | wxALL, 10);
  this->SetSizer(box);

  this->servers->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastServers::onServersListDblClicked, this, wxID_ANY);
  this->results->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastServers::onResultsListDblClicked, this, wxID_ANY);
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

    std::ostringstream portSS;
    portSS << it->first.second;
    this->servers->SetItem(index, 0, monitor->getConfiguration()->getServerFromPort(it->first.second).c_str());
    this->servers->SetItem(index, 1, it->first.first.c_str());
    this->servers->SetItem(index, 2, "n/a");
    this->servers->SetItem(index, 3, portSS.str().c_str());

    this->servers->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
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

  // clean
  this->statusList->DeleteAllItems();
  this->statusDetails.clear();
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

    if (this->statusDetails.find(streamId) != this->statusDetails.end())
    {
      return;
    }

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
    this->statusDetails.insert(std::make_pair(streamId, list));
  }
  catch (boost::bad_lexical_cast& ex)
  {
    LogError::getInstance().sysLog(ERROR, "bad stream id: %s", ex.what());
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
      wxListItem item;
      item.SetId(index);
      list->InsertItem(item);
      boost::char_separator<char> sep2(" ");
      tokenizer tok2(*it, sep2);

      list->SetItem(index, 0, this->host.c_str());
      list->SetItem(index, 1, this->type.c_str());
      unsigned int col = 2;
      for (tokenizer::iterator it2 = tok2.begin(); (col < 4) && (it2 != tok2.end()); ++it2)
      {
        list->SetItem(index, col++, (*it2).c_str());
      }

      StreamInfos::status_t status = StreamInfos::parseStatus(*it);
      list->SetItemTextColour(index, status);
      list->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
      index++;
    }
  }
}
