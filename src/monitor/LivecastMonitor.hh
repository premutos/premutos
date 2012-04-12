#ifndef __LIVECAST_MONITOR_HH__
#define __LIVECAST_MONITOR_HH__

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace livecast {
namespace monitor {

class MonitorConfiguration;
class ResultCallbackIntf;
class StreamInfos;

class LivecastMonitor
{
public:
  LivecastMonitor(boost::shared_ptr<MonitorConfiguration> cfg);
  ~LivecastMonitor();  

  int run();
  boost::asio::io_service& getIOService();

  void refresh(boost::shared_ptr<ResultCallbackIntf> resultCb);
  void check(unsigned int streamId, boost::shared_ptr<ResultCallbackIntf> resultCb);
  void reinitStream(unsigned int streamId);
  const boost::shared_ptr<StreamInfos> getStreamInfos(unsigned int streamId);
  const boost::shared_ptr<MonitorConfiguration> getConfiguration() const;

  inline void setRefreshPeriod(boost::posix_time::milliseconds refreshPeriod) { this->refreshPeriod = refreshPeriod; }
  inline boost::posix_time::milliseconds getRefreshPeriod() const { return this->refreshPeriod; }

protected:
  void handleRefresh(const boost::system::error_code&);
  void handleCheckTimer(const boost::system::error_code&);

private:
  boost::asio::io_service io_service;
  boost::posix_time::milliseconds refreshPeriod;
  boost::asio::deadline_timer refresh_timer;
  boost::asio::deadline_timer deadline_timer;
  boost::shared_ptr<MonitorConfiguration> cfg;
  boost::shared_ptr<ResultCallbackIntf> resultCallback;
};

}
}

#endif
