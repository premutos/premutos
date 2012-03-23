#ifndef __MONITOR_CONFIGURATION_HH__
#define __MONITOR_CONFIGURATION_HH__

#include <map>
#include <string>

class MonitorConfiguration
{
public:
  typedef std::map<unsigned int, const std::string> port_to_server_t;  

public:
  MonitorConfiguration();
  
  virtual unsigned int getConnectionTimeout() const = 0;
  port_to_server_t::mapped_type& getServerFromPort(port_to_server_t::key_type port) const;

protected:
  int load(const char * dbHost, const char * dbUser, const char * dbPass, const char * dbName, const char * query);

private:
  port_to_server_t portsToServers;
};

#endif
