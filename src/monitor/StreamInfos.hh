#ifndef __STREAM_INFOS_HH__
#define __STREAM_INFOS_HH__

#include "ResultCallbackIntf.hh"
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

namespace livecast {
namespace monitor {

class MonitorConfiguration;
class LivecastConnection;

class StreamInfosException
{
};

class StreamInfos
{
public:
  enum status_t
  {
    STATUS_WAITING,
    STATUS_INITIALIZING,
    STATUS_RUNNING,
    STATUS_ERROR,
    STATUS_UNKNOWN,
  };

  struct server_t
  {
    enum type_t
    {
      STREAM_DUP,
      MASTER_BOX,
      STREAMER_RTMP,
      STREAMER_HLS,
      UNKNOWN,
    };
    const std::string host;
    const uint16_t port;
    const type_t type;
    status_t status;
    server_t(const char * host,
             const uint16_t& port,
             const type_t& type)
      : host(host), port(port), type(type), status(STATUS_UNKNOWN)
      {
      }
  };

public: 
  StreamInfos(unsigned int streamId);
  ~StreamInfos();
  
  void check(boost::shared_ptr<ResultCallbackIntf> resultCb, const boost::shared_ptr<MonitorConfiguration> cfg);
  void loadInfos(const boost::shared_ptr<MonitorConfiguration> cfg);
  void parseStatus();
  void addServer(const server_t& server, bool primary);

  inline unsigned int getId() const { return this->streamId; }
  inline status_t getStatus() const { return this->status; }
  inline const boost::property_tree::ptree& getResultTree() const { return this->resultTree; }
  inline const boost::array<boost::array<std::list<server_t>, 3>, 2>& getServers() const { return this->servers; }

  static status_t parseStatus(const std::string& value);

  enum fields_t
  {
    FIELDS_ID,
    FIELDS_SRC_IP,
    FIELDS_MODE,
    FIELDS_PROTOCOL,
    FIELDS_DST_HOST,
    FIELDS_DST_PORT,
    FIELDS_EXT_KEY,
    FIELDS_BACKLOG,
    FIELDS_NB_CONNECTIONS,
    FIELDS_ENABLED,
    FIELDS_DISABLE_FILTER,
    FIELDS_LAST_FIELD
  };
  boost::array<std::string, FIELDS_LAST_FIELD> infos;

private:
  unsigned int streamId;
  status_t status;
  boost::property_tree::ptree resultTree;
  boost::array<boost::array<std::list<server_t>, 3>, 2> servers;
};

}
}

#endif
