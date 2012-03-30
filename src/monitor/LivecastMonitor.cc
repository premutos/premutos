#include "../lib/Log.hh"
#include "LivecastMonitor.hh"
#include <boost/bind.hpp>

#include <boost/thread.hpp>

using namespace livecast;
using namespace livecast::monitor;

LivecastMonitor::LivecastMonitor(boost::shared_ptr<MonitorConfiguration> cfg)
  : deadline_timer(io_service),
    cfg(cfg)
{
}

LivecastMonitor::~LivecastMonitor()
{
}

int LivecastMonitor::run()
{
  this->deadline_timer.async_wait(boost::bind(&LivecastMonitor::handleCheckTimer, this, boost::asio::placeholders::error));
  this->io_service.run();
  return 0;
}

void LivecastMonitor::refresh(boost::shared_ptr<ResultCallbackIntf> resultCb)
{
  for (MonitorConfiguration::map_streams_infos_t::const_iterator it = this->cfg->getStreamsInfos().begin(); it != this->cfg->getStreamsInfos().end(); ++it)
  {
    resultCb->commitStreamList();
    this->check(it->first, resultCb);
  }
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

const MonitorConfiguration::map_streams_infos_t& LivecastMonitor::getStreams() const
{
  return this->cfg->getStreamsInfos();
}

boost::asio::io_service& LivecastMonitor::getIOService()
{
  return this->io_service;
}

void LivecastMonitor::handleCheckTimer(const boost::system::error_code&)
{
  // never thrown
}
