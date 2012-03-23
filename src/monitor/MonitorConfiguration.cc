#include "MonitorConfiguration.hh"
#include "../lib/Log.hh"
#include <sstream>
#include <cassert>
#include <cerrno>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>

MonitorConfiguration::MonitorConfiguration()
{
  this->portsToServers.insert(std::make_pair(0, "unknown"));
  this->portsToServers.insert(std::make_pair(1111, "streamdup"));
  this->portsToServers.insert(std::make_pair(2222, "masterbox"));
  this->portsToServers.insert(std::make_pair(3333, "rtmp streamer"));
  this->portsToServers.insert(std::make_pair(4444, "hls streamer"));
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

int MonitorConfiguration::load(const char * dbHost, const char * dbUser, const char * dbPass, const char * dbName, const char * query)
{
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
      LogError::getInstance().sysLog(DEBUG, "%s - %s - %s - %s", row[0], row[1], row[2], row[3]);
    }

  } while (row != 0);

	mysql_close(&connection);

  LogError::getInstance().sysLog(DEBUG, "end");

  return 0;
}
