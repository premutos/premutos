#include "MonitorConfiguration.hh"
#include "../lib/Log.hh"
#include <sstream>
#include <cassert>
#include <cerrno>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <boost/lexical_cast.hpp>

using namespace livecast;
using namespace livecast::monitor;

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
  std::string query = this->getQueryStream();

  std::string::size_type pos = query.find("<streamId>");
  if (pos != std::string::npos)
  {
    std::ostringstream oss;
    oss << streamInfos.getId();
    query.replace(pos, 10, oss.str());
  }

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
      streamInfos.infos[StreamInfos::FIELDS_ID] =             row[StreamInfos::FIELDS_ID];
      streamInfos.infos[StreamInfos::FIELDS_SRC_IP] =         row[StreamInfos::FIELDS_SRC_IP];
      streamInfos.infos[StreamInfos::FIELDS_MODE] =           row[StreamInfos::FIELDS_MODE];
      streamInfos.infos[StreamInfos::FIELDS_PROTOCOL] =       row[StreamInfos::FIELDS_PROTOCOL];
      streamInfos.infos[StreamInfos::FIELDS_DST_HOST] =       row[StreamInfos::FIELDS_DST_HOST];
      streamInfos.infos[StreamInfos::FIELDS_DST_PORT] =       row[StreamInfos::FIELDS_DST_PORT];
      streamInfos.infos[StreamInfos::FIELDS_EXT_KEY] =        row[StreamInfos::FIELDS_EXT_KEY];
      streamInfos.infos[StreamInfos::FIELDS_BACKLOG] =        row[StreamInfos::FIELDS_BACKLOG];
      streamInfos.infos[StreamInfos::FIELDS_NB_CONNECTIONS] = row[StreamInfos::FIELDS_NB_CONNECTIONS];
      streamInfos.infos[StreamInfos::FIELDS_ENABLED] =        row[StreamInfos::FIELDS_ENABLED];
      streamInfos.infos[StreamInfos::FIELDS_DISABLE_FILTER] = row[StreamInfos::FIELDS_DISABLE_FILTER];
    }

  } while (row != 0);

	mysql_close(&connection);

  LogError::getInstance().sysLog(DEBUG, "end");

  return 0;
}

int MonitorConfiguration::loadStreamList()
{
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
      LogError::getInstance().sysLog(DEBUG, "%s - %s - %s - %s - %s", row[0], row[1], row[2], row[3], row[4]);
      unsigned int streamId = boost::lexical_cast<unsigned int>(row[0]);
      map_streams_infos_t::iterator it = this->streamsInfos.find(streamId);
      if (it == this->streamsInfos.end())
      {
        boost::shared_ptr<StreamInfos> si(new StreamInfos(streamId));
        it = this->streamsInfos.insert(std::make_pair(streamId, si)).first;
      }

      std::list<connections_t::key_type> keys;
      keys.push_back(std::make_pair(row[2], 1111));
      keys.push_back(std::make_pair(row[3], 2222));
      if (row[4] != NULL)
      {
        keys.push_back(std::make_pair(row[4], 3333));
        keys.push_back(std::make_pair(row[4], 4444));
      }

      connections_t::iterator itConnections;
      for (std::list<connections_t::key_type>::const_iterator itKeys = keys.begin(); itKeys != keys.end(); ++itKeys)
      {
        itConnections = this->connections.find(*itKeys);
        if (itConnections == this->connections.end())
        {
          boost::shared_ptr<LivecastConnection> connection = this->createConnection((*itKeys).first, (*itKeys).second);
          itConnections = this->connections.insert(std::make_pair(*itKeys, connection)).first;
        }
        StreamInfos::server_t server((*itKeys).first.c_str(), (*itKeys).second, 
                                     (*itKeys).second == 1111 ? StreamInfos::server_t::STREAM_DUP : 
                                     (*itKeys).second == 2222 ? StreamInfos::server_t::MASTER_BOX : 
                                     (*itKeys).second == 3333 ? StreamInfos::server_t::STREAMER_RTMP : 
                                     (*itKeys).second == 4444 ? StreamInfos::server_t::STREAMER_HLS : 
                                     StreamInfos::server_t::UNKNOWN);
        it->second->addServer(server, row[1][0] == '0');
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
