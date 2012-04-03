#include "ResultCallback.hh"
#include "LivecastResult.hh"
#include "LivecastServers.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::gui;

ResultCallback::ResultCallback(LivecastResult * const streams, LivecastServers * const servers) 
  : streams(streams),
    servers(servers)
{
}

void ResultCallback::commitStreamsList() 
{ 
  LogError::getInstance().sysLog(DEBUG, "commit streams list");
  wxCommandEvent event(this->streams->streamsListEvent);
  this->streams->GetEventHandler()->AddPendingEvent(event);
}

void ResultCallback::commitServersList() 
{ 
  LogError::getInstance().sysLog(DEBUG, "commit servers list");
  wxCommandEvent event(this->servers->serversListEvent);
  this->servers->GetEventHandler()->AddPendingEvent(event);
}

void ResultCallback::commitCheckStream(unsigned int streamId) 
{ 
  LogError::getInstance().sysLog(DEBUG, "commit check stream %d", streamId);
  wxCommandEvent event(this->streams->checkStreamEvent);
  event.SetInt(streamId);
  this->streams->GetEventHandler()->AddPendingEvent(event);
}
