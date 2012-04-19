#include "MonitorConfiguration.hh"
#include "StreamInfos.hh"
#include "../lib/Log.hh"
#include <sstream>
#include <cassert>
#include <cerrno>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <boost/lexical_cast.hpp>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::lib;

enum query_list_rows_t
{
  QLR_STREAM_ID,
  QLR_ROW,
  QLR_STREAMDUP_HOSTNAME, 
  QLR_STREAMDUP_MODE, 
  QLR_STREAMDUP_PROTOCOL, 
  QLR_STREAMDUP_PORT, 
  QLR_MASTERBOX_HOSTNAME, 
  QLR_MASTERBOX_PORT, 
  QLR_MASTERBOX_LEAF,
  QLR_STREAMER_HOSTNAME,
};

MonitorConfiguration::MonitorConfiguration()
{
  this->portsToServers.insert(std::make_pair(0, "unknown"));
  this->portsToServers.insert(std::make_pair(1111, "streamdup"));
  this->portsToServers.insert(std::make_pair(2222, "masterbox"));
  this->portsToServers.insert(std::make_pair(3333, "rtmp streamer"));
  this->portsToServers.insert(std::make_pair(4444, "hls streamer"));
}

MonitorConfiguration::~MonitorConfiguration()
{
}

boost::shared_ptr<LivecastConnection> MonitorConfiguration::getConnection(const std::string& host, uint16_t port)
{
  connections_t::key_type key = std::make_pair(host, port);
  connections_t::iterator it;
  it = this->connections.find(key);
  if (it == this->connections.end())
  {
    LogError::getInstance().sysLog(CRITICAL, "cannot find connection to %s:%u", host.c_str(), port);
  }
  assert (it != this->connections.end());
  return it->second;
}

MonitorConfiguration::port_to_server_t::mapped_type&
MonitorConfiguration::getServerFromPort(MonitorConfiguration::port_to_server_t::key_type port) const
{
  MonitorConfiguration::port_to_server_t::const_iterator it = this->portsToServers.find(port);
  if (it != this->portsToServers.end())
  {
    return it->second;
  }
  else
  {
    MonitorConfiguration::port_to_server_t::const_iterator it = this->portsToServers.find(0);
    assert(it != this->portsToServers.end());
    return it->second;
  }
}

int MonitorConfiguration::load(StreamInfos& streamInfos) const
{
  const char * dbHost = this->getDbHost();
  const char * dbUser = this->getDbUser();
  const char * dbPass = this->getDbPass();
  const char * dbName = this->getDbName();
  std::string query = "";

  MYSQL connection;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int rc = 0;

	mysql_init(&connection);
	if (!mysql_real_connect(&connection, dbHost, dbUser, dbPass, dbName, 0, NULL, 0))
  {
    LogError::getInstance().sysLog(ERROR, "connection to database failed: %s\n", mysql_error(&connection));
		return EINVAL;
	}

  //
  // load stream infos
  query = this->getQueryStream();

  std::string::size_type pos = query.find("<streamId>");
  if (pos != std::string::npos)
  {
    std::ostringstream oss;
    oss << streamInfos.getId();
    query.replace(pos, 10, oss.str());
  }

  rc = mysql_query(&connection, query.c_str());
  if (rc < 0)
  {
    LogError::getInstance().sysLog(ERROR, "query failed: %s\n", mysql_error(&connection));
		return rc;
  }

  if (!mysql_field_count(&connection))
  {
    LogError::getInstance().sysLog(DEBUG, "no result from query: %s\n", query.c_str());
    return EINVAL;
  }

  LogError::getInstance().sysLog(DEBUG, "query: '%s'\n", query.c_str());

  result = mysql_use_result(&connection);
  if (!result) 
  {
    LogError::getInstance().sysLog(ERROR, "cannot use result on the mysql connection: (%d) %s\n", mysql_errno(&connection), mysql_error(&connection));
    return EINVAL;
  }

  
  do
  {
    row = mysql_fetch_row(result);

    if (row != 0)
    {
      for (unsigned int i = 0; i < StreamInfos::FIELDS_LAST_FIELD; i++)
      {
        streamInfos.infos[i] = row[i];
      }
    }

  } while (row != 0);

  //
  // load profiles
  query = this->getQueryStreamProfile();

  pos = query.find("<streamId>");
  if (pos != std::string::npos)
  {
    std::ostringstream oss;
    oss << streamInfos.getId();
    query.replace(pos, 10, oss.str());
  }

  rc = mysql_query(&connection, query.c_str());
  if (rc < 0)
  {
    LogError::getInstance().sysLog(ERROR, "query failed: %s\n", mysql_error(&connection));
		return rc;
  }

  if (!mysql_field_count(&connection))
  {
    LogError::getInstance().sysLog(DEBUG, "no result from query: %s\n", query.c_str());
    return EINVAL;
  }

  LogError::getInstance().sysLog(DEBUG, "query: '%s'\n", query.c_str());

  result = mysql_use_result(&connection);
  if (!result) 
  {
    LogError::getInstance().sysLog(ERROR, "cannot use result on the mysql connection: (%d) %s\n", mysql_errno(&connection), mysql_error(&connection));
    return EINVAL;
  }
  
  do
  {
    row = mysql_fetch_row(result);

    if (row != 0)
    {
      StreamInfos::profile_infos_t pi;
      for (unsigned int i = 0; i < StreamInfos::PROFILE_LAST; i++)
      {
        pi[i] = row[i];
      }
      streamInfos.profiles.push_back(pi);
    }

  } while (row != 0);

	mysql_close(&connection);

  LogError::getInstance().sysLog(DEBUG, "end");

  return 0;
}

std::string MonitorConfiguration::getPipeline(unsigned int streamId) const
{
  const char * dbHost = this->getDbHost();
  const char * dbUser = this->getDbUser();
  const char * dbPass = this->getDbPass();
  const char * dbName = this->getDbName();
  std::ostringstream query;
  query << "select gstreamerPipeline from streamOptions where streamSrcId = '" << streamId << "'";

  MYSQL connection;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int rc = 0;

	mysql_init(&connection);
	if (!mysql_real_connect(&connection, dbHost, dbUser, dbPass, dbName, 0, NULL, 0))
  {
    LogError::getInstance().sysLog(ERROR, "connection to database failed: %s\n", mysql_error(&connection));
		// return EINVAL;
    return "cannot load pipeline";
	}

  rc = mysql_query(&connection, query.str().c_str());
  if (rc < 0)
  {
    LogError::getInstance().sysLog(ERROR, "query failed: %s\n", mysql_error(&connection));
		// return rc;
    return "cannot load pipeline";
  }

  if (!mysql_field_count(&connection))
  {
    LogError::getInstance().sysLog(DEBUG, "no result from query: %s\n", query.str().c_str());
    // return EINVAL;
    return "";
  }

  LogError::getInstance().sysLog(DEBUG, "query: '%s'\n", query.str().c_str());

  result = mysql_use_result(&connection);
  if (!result) 
  {
    LogError::getInstance().sysLog(ERROR, "cannot use result on the mysql connection: (%d) %s\n", mysql_errno(&connection), mysql_error(&connection));
    // return EINVAL;
    return "cannot load pipeline";
  }

  
  row = mysql_fetch_row(result);
  
  std::string pipeline;
  if (row != 0)
  {
    pipeline = row[0];
  }
  else 
  {
    pipeline = "cannot load pipeline";
  }

	mysql_close(&connection);

  return pipeline;
}

int MonitorConfiguration::loadStreamList()
{
  this->streamsInfos.clear();
  this->connections.clear();

  const char * dbHost = this->getDbHost();
  const char * dbUser = this->getDbUser();
  const char * dbPass = this->getDbPass();
  const char * dbName = this->getDbName();
  const char * query = this->getQueryStreamList();

  MYSQL connection;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int rc = 0;

	mysql_init(&connection);
	if (!mysql_real_connect(&connection, dbHost, dbUser, dbPass, dbName, 0, NULL, 0))
  {
    LogError::getInstance().sysLog(ERROR, "connection to database failed: %s\n", mysql_error(&connection));
		return EINVAL;
	}

  rc = mysql_query(&connection, query);
  if (rc < 0)
  {
    LogError::getInstance().sysLog(ERROR, "query failed: %s\n", mysql_error(&connection));
		return rc;
  }

  if (!mysql_field_count(&connection))
  {
    LogError::getInstance().sysLog(DEBUG, "no result from query: %s\n", query);
    return EINVAL;
  }

  LogError::getInstance().sysLog(DEBUG, "query: '%s'\n", query);

  result = mysql_use_result(&connection);
  if (!result) 
  {
    LogError::getInstance().sysLog(ERROR, "cannot use result on the mysql connection: (%d) %s\n", mysql_errno(&connection), mysql_error(&connection));
    return EINVAL;
  }

  
  do
  {
    row = mysql_fetch_row(result);

    if (row != 0)
    {
      LogError::getInstance().sysLog(DEBUG, "%s - %s - %s - %s - %s - %s - %s - %s - %s - %s", 
                                     row[QLR_STREAM_ID], 
                                     row[QLR_ROW],
                                     row[QLR_STREAMDUP_HOSTNAME],
                                     row[QLR_STREAMDUP_MODE],
                                     row[QLR_STREAMDUP_PROTOCOL],
                                     row[QLR_STREAMDUP_PORT],
                                     row[QLR_MASTERBOX_HOSTNAME],
                                     row[QLR_MASTERBOX_PORT],
                                     row[QLR_MASTERBOX_LEAF],
                                     row[QLR_STREAMER_HOSTNAME]);
      unsigned int streamId = boost::lexical_cast<unsigned int>(row[QLR_STREAM_ID]);
      map_streams_infos_t::iterator it = this->streamsInfos.find(streamId);
      if (it == this->streamsInfos.end())
      {
        boost::shared_ptr<StreamInfos> si(new StreamInfos(streamId));
        it = this->streamsInfos.insert(std::make_pair(streamId, si)).first;
      }
      
      //
      // add servers
      uint16_t sdPort, mbPort;
      const std::string sdProtocol = std::string(row[QLR_STREAMDUP_MODE]) + " " + std::string(row[QLR_STREAMDUP_PROTOCOL]);
      sdPort = boost::lexical_cast<uint16_t>(row[QLR_STREAMDUP_PORT]);
      mbPort = boost::lexical_cast<uint16_t>(row[QLR_MASTERBOX_PORT]);
      StreamInfos::server_t streamdup(row[QLR_STREAMDUP_HOSTNAME], 1111, sdProtocol.c_str(), sdPort, StreamInfos::server_t::STREAM_DUP, false);
      StreamInfos::server_t masterbox(row[QLR_MASTERBOX_HOSTNAME], 2222, "tcp", mbPort, StreamInfos::server_t::MASTER_BOX, row[QLR_MASTERBOX_LEAF][0] == '0');

      it->second->addServer(streamdup, row[QLR_ROW][0] == '0');
      it->second->addServer(masterbox, row[QLR_ROW][0] == '0');

      if (row[QLR_STREAMER_HOSTNAME] != NULL)
      {
        StreamInfos::server_t streamerRtmp(row[QLR_STREAMER_HOSTNAME], 3333, "flv/tcp", 1936, StreamInfos::server_t::STREAMER_RTMP, false); 
        StreamInfos::server_t streamerHls(row[QLR_STREAMER_HOSTNAME], 4444, "http", 80, StreamInfos::server_t::STREAMER_HLS, false);
        it->second->addServer(streamerRtmp, row[QLR_ROW][0] == '0');
        it->second->addServer(streamerHls, row[QLR_ROW][0] == '0');
      }

      //
      // prepare connections
      std::list<connections_t::key_type> keys;
      keys.push_back(std::make_pair(row[QLR_STREAMDUP_HOSTNAME], 1111));
      keys.push_back(std::make_pair(row[QLR_MASTERBOX_HOSTNAME], 2222));
      if (row[QLR_STREAMER_HOSTNAME] != NULL)
      {
        keys.push_back(std::make_pair(row[QLR_STREAMER_HOSTNAME], 3333));
        keys.push_back(std::make_pair(row[QLR_STREAMER_HOSTNAME], 4444));
      }

      connections_t::iterator itConnections;
      for (std::list<connections_t::key_type>::const_iterator itKeys = keys.begin(); itKeys != keys.end(); ++itKeys)
      {
        itConnections = this->connections.find(*itKeys);
        if (itConnections == this->connections.end())
        {
          LogError::getInstance().sysLog(DEBUG, "create connection to %s:%u", (*itKeys).first.c_str(), (*itKeys).second);
          boost::shared_ptr<LivecastConnection> connection = this->createConnection((*itKeys).first, (*itKeys).second);
          itConnections = this->connections.insert(std::make_pair(*itKeys, connection)).first;
        }
      }

    }

  } while (row != 0);

	mysql_close(&connection);

  LogError::getInstance().sysLog(DEBUG, "end");

  return 0;
}

int MonitorConfiguration::initMysql(const char * query)
{
  (void)query;
  return 0;
}
