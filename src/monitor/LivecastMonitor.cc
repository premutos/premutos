#include "LivecastMonitor.hh"
#include <boost/bind.hpp>

LivecastMonitor::LivecastMonitor(boost::shared_ptr<ResultCallbackIntf> resultCb)
  : deadline_timer(io_service),
    resultCb(resultCb)
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

void LivecastMonitor::add(LivecastConnectionPtr connection)
{
  this->connections.push_back(connection);
}

void LivecastMonitor::check()
{
  this->resultCb->clear();
  std::for_each(this->connections.begin(), 
                this->connections.end(), 
                boost::bind(&LivecastConnection::check, _1));
}

boost::asio::io_service& LivecastMonitor::getIOService()
{
  return this->io_service;
}

void LivecastMonitor::handleCheckTimer(const boost::system::error_code&)
{
  // never thrown
}
