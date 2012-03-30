#ifndef __LIVECAST_CONNECTION_HH__
#define __LIVECAST_CONNECTION_HH__

#include "MonitorConfiguration.hh"
#include "../ResultCallbackIntf.hh"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread/condition_variable.hpp>

namespace livecast {
namespace monitor {

class LivecastConnectionException
{
};

class LivecastConnection : public boost::enable_shared_from_this<LivecastConnection>
{
public:
  LivecastConnection(boost::asio::io_service& io_service,
                     const std::string& host, 
                     const boost::uint16_t port,
                     const std::string& user,
                     const std::string& pass,
                     const MonitorConfiguration * const conf);
  ~LivecastConnection();

  void check(unsigned int streamId, boost::shared_ptr<boost::property_tree::ptree> result);
protected:
  void handleConnect(const boost::system::error_code& error);
  void handleTimeout(const boost::system::error_code& error);
  void checkError(const boost::system::error_code& error) const;
  void parseResult(const char * result);
private:
  boost::asio::ip::tcp::socket socket;
  boost::asio::ip::tcp::resolver resolver;
  boost::asio::deadline_timer deadline_timer;
  boost::asio::strand strand;

  unsigned int streamId;
  boost::shared_ptr<boost::property_tree::ptree> result;

  boost::condition_variable cond;
  boost::mutex checkMut;
  boost::mutex syncMut;
  bool data_ready;

  const std::string host;
  const boost::uint16_t port;
  const std::string& user;
  const std::string& pass;

  const MonitorConfiguration * const conf;
  
  std::string resultString;
};

}
}

#endif
