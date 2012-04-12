#ifndef __MONITOR_CONFIGURATION_HH__
#define __MONITOR_CONFIGURATION_HH__

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace monitor {

class StreamInfos;
class LivecastConnection;

class MonitorConfiguration
{
public:
  typedef std::map<unsigned int, const std::string> port_to_server_t;
  typedef std::map<unsigned int, boost::shared_ptr<StreamInfos> > map_streams_infos_t;
  typedef std::map<std::pair<std::string, uint16_t>, boost::shared_ptr<LivecastConnection> > connections_t;

public:
  MonitorConfiguration();
  virtual ~MonitorConfiguration();
  
  virtual unsigned int getConnectionTimeout() const = 0;

  // db
  virtual void setAccessKey(const char *) = 0;
  virtual const char * getDbUser() const = 0;
  virtual const char * getDbPass() const = 0;
  virtual const char * getDbHost() const = 0;
  virtual const char * getDbName() const = 0;

  // db queries
  virtual const char * getQueryStream() const = 0;
  virtual const char * getQueryStreamList() const = 0;
  virtual const char * getQueryStreamProfile() const = 0;

  port_to_server_t::mapped_type& getServerFromPort(port_to_server_t::key_type port) const;
  inline const map_streams_infos_t& getStreamsInfos() const { return this->streamsInfos; }
  inline const connections_t& getConnections() const { return this->connections; }
  boost::shared_ptr<LivecastConnection> getConnection(const std::string& host, uint16_t port);

  virtual void load() = 0;
  int load(StreamInfos& streamInfos) const;
  std::string getPipeline(unsigned int streamId) const;
  
protected:
  virtual boost::shared_ptr<LivecastConnection> createConnection(const std::string& host, uint16_t port) const = 0;
  int loadStreamList();
  int initMysql(const char * query);

private:
  connections_t connections;
  port_to_server_t portsToServers;
  map_streams_infos_t streamsInfos;
};

}
}

#endif
