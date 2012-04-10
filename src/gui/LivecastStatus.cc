#include "../lib/Log.hh"
#include "LivecastStatus.hh"
#include "LivecastResult.hh"
#include "StatusSchema.hh"

#include <wx/splitter.h>

#include <boost/property_tree/xml_parser.hpp>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

LivecastStatus::LivecastStatus(wxWindow * parent, boost::shared_ptr<StreamInfos> streamInfos)
  : wxPanel(parent, wxID_ANY),
    checkStreamEvent(wxNewEventType()),
    streamId(streamInfos->getId()),
    streamInfos(streamInfos)
{ 
  wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_BORDER);
  this->statusSchema = new StatusSchema(splitter);
  this->tree = new wxTreeCtrl(splitter, wxID_ANY);
  splitter->SplitVertically(this->tree, this->statusSchema, this->GetParent()->GetSize().GetWidth() / 2);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(splitter, 1, wxEXPAND | wxALL, 0);
  this->SetSizer(box);

  this->Connect(checkStreamEvent, wxCommandEventHandler(LivecastStatus::onCheckStream));
}

LivecastStatus::~LivecastStatus()
{
}

void LivecastStatus::commitCheckStream(unsigned int streamId)
{
  LogError::getInstance().sysLog(DEBUG, "commit check stream");
  wxCommandEvent event(checkStreamEvent);
  event.SetInt(streamId);
  this->GetEventHandler()->AddPendingEvent(event);
}

wxTreeItemId LivecastStatus::updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id)
{
  wxTreeItemId idReturn = id;
  if (ptree.size() > 0)
  {    
    boost::property_tree::ptree::const_iterator it;
    for (it = ptree.begin(); it != ptree.end(); ++it)
    {    
      idReturn = this->updateTree(it->second, this->tree->AppendItem(id, it->first));
    }
  }
  else
  {
    this->tree->AppendItem(id, ptree.data());
  }
  return idReturn;
}

void LivecastStatus::onCheckStream(wxCommandEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "get status of stream %d", ev.GetInt());
  this->tree->DeleteAllItems();
  wxTreeItemId idTree = this->tree->AddRoot("servers");
  this->updateTree(this->streamInfos->getResultTree(), idTree);
  this->tree->ExpandAll();  

  //
  const boost::property_tree::ptree& statusInfos = this->streamInfos->getResultTree();
  unsigned int id = 0;
  for (boost::property_tree::ptree::const_iterator it = statusInfos.get_child("status.primary").begin(); it != statusInfos.get_child("status.primary").end(); ++it)
  {
    const boost::optional<std::string> typeStr = it->second.get_optional<std::string>("type");
    const boost::optional<std::string> statusStr = it->second.get_optional<std::string>("result");

    StatusSchema::type_t type = ((*typeStr == "streamdup") ? StatusSchema::SERVER_STREAMDUP :
                                 (*typeStr == "masterbox") ? StatusSchema::SERVER_MASTERBOX :
                                 (*typeStr == "rtmp streamer") ? StatusSchema::SERVER_STREAMER_RTMP :
                                 (*typeStr == "hls streamer") ? StatusSchema::SERVER_STREAMER_HLS :
                                 StatusSchema::SERVER_UNKNOWN) ;
    StatusSchema::status_t status = (((*statusStr).find("WAITING") != std::string::npos) ? StatusSchema::STATUS_WAITING :
                                     ((*statusStr).find("INITIALIZING") != std::string::npos) ? StatusSchema::STATUS_INITIALIZING :
                                     ((*statusStr).find("RUNNING") != std::string::npos) ? StatusSchema::STATUS_RUNNING :
                                     ((*statusStr).find("ERROR") != std::string::npos) ? StatusSchema::STATUS_ERROR :
                                     StatusSchema::STATUS_UNKNOWN) ;

    boost::shared_ptr<StatusSchema::server_t> server(new StatusSchema::server_t);
    server->id = id++;
    server->hostname = it->first;
    server->type = type;
    server->status = status;
    server->protocol = *(it->second.get_optional<std::string>("protocol"));
    server->port = *(it->second.get_optional<unsigned int>("port"));
    server->leaf = *(it->second.get_optional<bool>("leaf"));
   
    LogError::getInstance().sysLog(ERROR, "add server : [id:%d] [host:%s] [type:%d] [status:%d] [protocol:%s] [port:%u] [leaf:%d]",
                                   server->id, server->hostname.c_str(), server->type, server->status, server->protocol.c_str(), server->port, server->leaf);

//     LogError::getInstance().sysLog(DEBUG, "add server : [id:%d] [host:%s] [type:%d] [status:%d]",
//                                    server->id, server->hostname.c_str(), server->type, server->status);

    this->statusSchema->addServer(server);
  }
  this->statusSchema->linkAllServers();
  wxPaintEvent event;
  this->statusSchema->GetEventHandler()->AddPendingEvent(event);
}

void LivecastStatus::onRefresh(wxCommandEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
  ev.Skip();
}
