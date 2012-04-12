#ifndef __CONFIGURATION_HH__
#define __CONFIGURATION_HH__

#include "gui/GuiConfiguration.hh"
#include "monitor/MonitorConfiguration.hh"
#include <sstream>
#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>

namespace livecast {

namespace monitor {
class LivecastMonitor;
}

class Configuration : public monitor::MonitorConfiguration,
                      public gui::GuiConfiguration
{
public:

  class BadProgramOptions
  {
  };

  struct ProgramOption
  {
    std::string confFilename;
    std::string serversConf;
    std::string mainWinName;
    std::string mainWinSize;
    std::string logOutput;
    std::string logIdent;
    std::string dbUser;
    std::string dbPass;
    std::string dbHost;
    std::string dbName;
    std::string dbAccessFilename;
    std::string accessKey;
    std::string dbQueryStream;
    std::string dbQueryStreamList;
    std::string dbQueryStreamProfile;
    int hSize;
    int vSize;
    unsigned int refreshPeriod;
    unsigned int connectionTimeout;
    unsigned int logLevel;
    bool logColor;
    bool logDate;
    bool logPid;
    bool checkOnInit;
    bool useVirtualList;

    ProgramOption();
    int parseMainWinSize();
  };

  typedef std::map<std::string, boost::tuple<std::string, std::string, std::string, std::string> > access_t;

public:
  Configuration();
  void parseOpts(int argc, char**argv);
  void load();
  void loadAccess(std::istream& is);

  inline void setMonitor(boost::shared_ptr<monitor::LivecastMonitor> monitor) { this->monitor = monitor; }
  inline const boost::shared_ptr<ProgramOption> getOpts() const { return this->opts; }

  // Monitor configuration interface
  void setAccessKey(const char * key) { this->accessKey = key; }

  const char * getDbUser() const { 
    access_t::const_iterator it = this->access.find(this->accessKey);
    return it->second.get<2>().c_str(); 
  }
  const char * getDbPass() const { 
    access_t::const_iterator it = this->access.find(this->accessKey);
    return it->second.get<3>().c_str(); 
  }
  const char * getDbHost() const { 
    access_t::const_iterator it = this->access.find(this->accessKey);
    return it->second.get<0>().c_str(); 
  }
  const char * getDbName() const { 
    access_t::const_iterator it = this->access.find(this->accessKey);
    return it->second.get<1>().c_str(); 
  }

  inline const char * getQueryStream() const { return this->opts->dbQueryStream.c_str(); }
  inline const char * getQueryStreamList() const { return this->opts->dbQueryStreamList.c_str(); }
  inline const char * getQueryStreamProfile() const { return this->opts->dbQueryStreamProfile.c_str(); }
  inline unsigned int getConnectionTimeout() const { return this->opts->connectionTimeout; }

  // Gui configuration interface
  inline const char * getMainWindowName() const { return this->opts->mainWinName.c_str(); }
  inline int getMainWinHSize() const { return this->opts->hSize; }
  inline int getMainWinVSize() const { return this->opts->vSize; }
  inline bool useVirtualStreamList() const { return this->opts->useVirtualList; }
  const std::list<std::string> getViews() const;

protected:
  boost::shared_ptr<monitor::LivecastConnection> createConnection(const std::string& host, uint16_t port) const;

private:
  boost::shared_ptr<monitor::LivecastMonitor> monitor;  
  boost::shared_ptr<ProgramOption> opts;
  access_t access;
  std::string accessKey;
};

}

#endif
