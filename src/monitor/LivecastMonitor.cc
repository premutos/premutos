#include "LivecastMonitor.hh"
#include "LivecastConnection.hh"
#include "MonitorConfiguration.hh"
#include "ResultCallbackIntf.hh"
#include "StreamInfos.hh"

#include "../lib/Log.hh"

#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace livecast;
using namespace livecast::monitor;

LivecastMonitor::LivecastMonitor(boost::shared_ptr<MonitorConfiguration> cfg)
  : refreshPeriod(boost::posix_time::milliseconds(1000)),
    refresh_timer(io_service),
    deadline_timer(io_service),
    cfg(cfg)
{
}

LivecastMonitor::~LivecastMonitor()
{
}

int LivecastMonitor::run()
{
  this->refresh_timer.expires_from_now(boost::posix_time::seconds(2));
  this->refresh_timer.async_wait(boost::bind(&LivecastMonitor::handleRefresh, this, boost::asio::placeholders::error));
  this->deadline_timer.async_wait(boost::bind(&LivecastMonitor::handleCheckTimer, this, boost::asio::placeholders::error));
  this->io_service.run();
  return 0;
}

void LivecastMonitor::refresh(boost::shared_ptr<ResultCallbackIntf> resultCb)
{
  if (!this->resultCallback)
  {
    this->resultCallback = resultCb;
  }
  resultCb->commitStreamsList();
  resultCb->commitServersList();
  for (MonitorConfiguration::map_streams_infos_t::const_iterator it = this->cfg->getStreamsInfos().begin(); it != this->cfg->getStreamsInfos().end(); ++it)
  {
    this->check(it->first, resultCb);
  }
  resultCb->commitStreamsList();
  resultCb->commitServersList();
}

void LivecastMonitor::check(unsigned int streamId,
                            boost::shared_ptr<ResultCallbackIntf> resultCb)
{
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->cfg->getStreamsInfos().find(streamId);
  if (it != this->cfg->getStreamsInfos().end())
  {
    it->second->loadInfos(this->cfg);
    boost::thread thread(boost::bind(&StreamInfos::check, it->second, resultCb, this->cfg));
  }
}

void LivecastMonitor::reinitStream(unsigned int streamId)
{
  LogError::getInstance().sysLog(DEBUG, "reinit %u", streamId);
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->cfg->getStreamsInfos().find(streamId);
  if (it != this->cfg->getStreamsInfos().end())
  {
    it->second->reinit(this->cfg);
  }
}

const boost::shared_ptr<StreamInfos> LivecastMonitor::getStreamInfos(unsigned int streamId)
{
  MonitorConfiguration::map_streams_infos_t::const_iterator it = this->cfg->getStreamsInfos().find(streamId);
  if (it != this->cfg->getStreamsInfos().end())
  {
    it->second->loadInfos(this->cfg);
    return it->second;
  }
  else
  {
    throw new StreamInfosException();
  }
  // never reach
}

const boost::shared_ptr<MonitorConfiguration> LivecastMonitor::getConfiguration() const 
{
  return this->cfg; 
}

boost::asio::io_service& LivecastMonitor::getIOService()
{
  return this->io_service;
}

void LivecastMonitor::handleRefresh(const boost::system::error_code& error)
{
  if (!error)
  {
    LogError::getInstance().sysLog(DEBUG, "refresh timer");
    if (this->resultCallback)
    {
      LogError::getInstance().sysLog(DEBUG, "===> refresh");
      this->refresh(this->resultCallback);
    }
    this->refresh_timer.expires_from_now(boost::posix_time::seconds(2));
    this->refresh_timer.async_wait(boost::bind(&LivecastMonitor::handleRefresh, this, boost::asio::placeholders::error));
  }
  else
  {
    LogError::getInstance().sysLog(ERROR, "%s", error.message().c_str());
  }
}

void LivecastMonitor::handleCheckTimer(const boost::system::error_code&)
{
  // never thrown
}
