#ifndef __LIVECAST_MONITOR_HH__
#define __LIVECAST_MONITOR_HH__

#include "../ResultCallbackIntf.hh"
#include "../LivecastMonitorIntf.hh"
#include "LivecastConnection.hh"

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

class LivecastMonitor : public LivecastMonitorIntf
{
public:
  LivecastMonitor(boost::shared_ptr<ResultCallbackIntf> resultCb);
  ~LivecastMonitor();  

  int run();
  void add(LivecastConnectionPtr connection);
  void check();
  boost::asio::io_service& getIOService();

protected:
  void handleCheckTimer(const boost::system::error_code&);

private:
  boost::asio::io_service io_service;
  boost::asio::deadline_timer deadline_timer;
  std::list<LivecastConnectionPtr> connections;
  boost::shared_ptr<ResultCallbackIntf> resultCb;
};

#endif
