#include "../lib/Log.hh"
#include "Util.hh"
#include "LivecastStatus.hh"
#include "LivecastResult.hh"
#include "StatusSchema.hh"

#include <wx/splitter.h>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;
using namespace livecast::lib;

LivecastStatus::LivecastStatus(wxWindow * parent, boost::shared_ptr<const StreamInfos> streamInfos, bool primary)
  : wxPanel(parent, wxID_ANY),
    checkStreamEvent(wxNewEventType()),
    streamId(streamInfos->getId()),
    streamInfos(streamInfos),
    primary(primary)
{ 
  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  this->statusSchema = new StatusSchema(this, 4);
  box->Add(this->statusSchema, 1, wxEXPAND | wxALL, 0);
  this->SetSizer(box);

  this->Connect(checkStreamEvent, wxCommandEventHandler(LivecastStatus::onCheckStream));

  LogError::getInstance().sysLog(DEBUG, "commit check stream");
  wxCommandEvent event(checkStreamEvent);
  event.SetInt(streamId);
  this->GetEventHandler()->AddPendingEvent(event);
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

void LivecastStatus::onCheckStream(wxCommandEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "get status of stream %d", ev.GetInt());

  this->statusSchema->reset();
  unsigned int id = 0;
  unsigned int row = this->primary ? 0 : 1;
  const StreamInfos::servers_t& servers = this->streamInfos->getServers();
  for (StreamInfos::servers_row_list_t::const_iterator itType = servers[row].begin(); itType != servers[row].end(); ++itType)
  {
    for (StreamInfos::servers_list_t::const_iterator itServer = (*itType).begin(); itServer != (*itType).end(); ++itServer)
    {
      boost::shared_ptr<StatusSchema::server_t> server(new StatusSchema::server_t);
      server->id = id++;
      server->hostname = (*itServer).host;
      server->type = (((*itServer).type == StreamInfos::server_t::STREAM_DUP) ? "STREAMDUP" :
                      ((*itServer).type == StreamInfos::server_t::MASTER_BOX) ? "MASTERBOX" :
                      ((*itServer).type == StreamInfos::server_t::STREAMER_RTMP) ? "STREAMER_RTMP" :
                      ((*itServer).type == StreamInfos::server_t::STREAMER_HLS) ? "STREAMER_HLS" :
                      "UNKNOWN");
      server->column = (((*itServer).type == StreamInfos::server_t::STREAM_DUP) ? 0 :
                        ((*itServer).type == StreamInfos::server_t::MASTER_BOX) ? 1 :
                        ((*itServer).type == StreamInfos::server_t::STREAMER_RTMP) ? 2 :
                        ((*itServer).type == StreamInfos::server_t::STREAMER_HLS) ? 3 : 
                        4);
      server->colour = (((*itServer).status == StreamInfos::STATUS_WAITING) ? livecast_grey:
                        ((*itServer).status == StreamInfos::STATUS_INITIALIZING) ? livecast_yellow:
                        ((*itServer).status == StreamInfos::STATUS_RUNNING) ? livecast_green:
                        ((*itServer).status == StreamInfos::STATUS_ERROR) ? livecast_red:
                        livecast_darkGrey);
      server->protocol = (*itServer).protocol;
      server->port = (*itServer).port;
      server->leaf = (*itServer).leaf;

      server->statusDetail = (*itServer).host;
      for (std::list<boost::tuple<StreamInfos::status_t, std::string, std::string> >::const_iterator itDetail = (*itServer).statusDetails.begin();
           itDetail != (*itServer).statusDetails.end(); ++itDetail)
      {
        server->statusDetail += "\n";
        server->statusDetail += (*itDetail).get<1>();
      }
   
      LogError::getInstance().sysLog(DEBUG, "add server : [id:%d] [host:%s]", server->id, server->hostname.c_str(), server->type.c_str());
      LogError::getInstance().sysLog(DEBUG, "status details : \n%s", server->statusDetail.c_str());
      
      this->statusSchema->addServer(server);
    }
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
