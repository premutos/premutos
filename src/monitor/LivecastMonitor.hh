#ifndef __LIVECAST_MONITOR_HH__
#define __LIVECAST_MONITOR_HH__

#include "../ResultCallbackIntf.hh"
#include "LivecastConnection.hh"
#include "MonitorConfiguration.hh"

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace livecast {
namespace monitor {

class LivecastMonitor
{
public:
  LivecastMonitor(boost::shared_ptr<MonitorConfiguration> cfg);
  ~LivecastMonitor();  

  int run();
  boost::asio::io_service& getIOService();

  // Livecast monitor interface
  void refresh(boost::shared_ptr<ResultCallbackIntf> resultCb);
  void check(unsigned int streamId, boost::shared_ptr<ResultCallbackIntf> resultCb);
  const boost::shared_ptr<StreamInfos> getStreamInfos(unsigned int streamId);
  const MonitorConfiguration::map_streams_infos_t& getStreams() const;

protected:
  void handleCheckTimer(const boost::system::error_code&);

private:
  boost::asio::io_service io_service;
  boost::asio::deadline_timer deadline_timer;
  boost::shared_ptr<MonitorConfiguration> cfg;
};

}
}

#endif
