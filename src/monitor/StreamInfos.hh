#ifndef __STREAM_INFOS_HH__
#define __STREAM_INFOS_HH__

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/property_tree/ptree.hpp>

namespace livecast {
namespace monitor {

class ResultCallbackIntf;
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

  enum profile_t
  {
    PROFILE_ID,
    PROFILE_WIDTH,
    PROFILE_HEIGHT,
    PROFILE_VIDEOBITRATE,
    PROFILE_AUDIOBITRATE,
    PROFILE_DEINTERLACE,
    PROFILE_FRAMERATE,
    PROFILE_PROTOCOLS,
    PROFILE_PRIMARY_CDN_URL,
    PROFILE_BACKUP_CDN_URL,
    PROFILE_PLAYBACK_URL,
    PROFILE_CDN_USERNAME,
    PROFILE_CDN_PASSWORD,
    PROFILE_CDN_LIVENAME,
    PROFILE_LAST,
  };
  typedef boost::array<std::string, PROFILE_LAST> profile_infos_t;
  typedef std::list<profile_infos_t> profiles_t;

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
  typedef boost::array<std::string, FIELDS_LAST_FIELD> infos_t;

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
    const uint16_t adminPort;
    const std::string protocol;
    const uint16_t port;
    const type_t type;
    status_t status;
    std::list<boost::tuple<status_t, std::string, std::string> > statusDetails;
    bool leaf;
    server_t(const char * host,
             const uint16_t adminPort,
             const char * protocol,
             const uint16_t& port,
             const type_t& type,
             const bool leaf)
      : host(host), adminPort(adminPort), protocol(protocol), port(port), type(type), status(STATUS_UNKNOWN), leaf(leaf)
      {
      }
  };
  typedef boost::array<boost::array<std::list<server_t>, 3>, 2> servers_t;

public: 
  StreamInfos(unsigned int streamId);
  ~StreamInfos();
  
  void status(boost::shared_ptr<ResultCallbackIntf> resultCb, const boost::shared_ptr<MonitorConfiguration> cfg);
  void reinit(const boost::shared_ptr<MonitorConfiguration> cfg);
  void loadInfos(const boost::shared_ptr<MonitorConfiguration> cfg);
  void parseStatus();
  void addServer(const server_t& server, bool primary);

  inline unsigned int getId() const { return this->streamId; }
  inline status_t getStatus() const { return this->globalStatus; }
  inline const boost::property_tree::ptree& getResultTree() const { return this->resultTree; }
  inline const infos_t& getInfos() const { return this->infos; }
  inline const servers_t& getServers() const { return this->servers; }
  inline const profiles_t& getProfiles() const { return this->profiles; }
  inline bool isModified() const { return this->modified; }

  static status_t parseStatus(const std::string& value);

private:
  unsigned int streamId;
  status_t globalStatus;
  boost::property_tree::ptree resultTree;
  infos_t infos;
  profiles_t profiles;
  servers_t servers;
  bool modified;

  friend class MonitorConfiguration;
};

}
}

#endif
