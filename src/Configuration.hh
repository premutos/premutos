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

  enum access_t
  {
    ACCESS_LOCAL,
    ACCESS_DEV,
    ACCESS_HOM,
    ACCESS_PROD,
    ACCESS_LAST,
  };

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
    std::string defaultAccess;
    std::string dbQueryStream;
    std::string dbQueryStreamList;
    std::string dbQueryStreamProfile;
    int hSize;
    int vSize;
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

public:
  Configuration();
  void parseOpts(int argc, char**argv);
  void load();
  void loadAccess(std::istream& is);

  inline void setMonitor(boost::shared_ptr<monitor::LivecastMonitor> monitor) { this->monitor = monitor; }
  inline const boost::shared_ptr<ProgramOption> getOpts() const { return this->opts; }

  // Monitor configuration interface
  void setAccess(unsigned int i) { if (i < this->access.size()) { this->accessIndex = i; } }
  const char * getDbUser() const { return this->access[this->accessIndex].get<2>().c_str(); }
  const char * getDbPass() const { return this->access[this->accessIndex].get<3>().c_str(); }
  const char * getDbHost() const { return this->access[this->accessIndex].get<0>().c_str(); }
  const char * getDbName() const { return this->access[this->accessIndex].get<1>().c_str(); }
  inline const char * getQueryStream() const { return this->opts->dbQueryStream.c_str(); }
  inline const char * getQueryStreamList() const { return this->opts->dbQueryStreamList.c_str(); }
  inline const char * getQueryStreamProfile() const { return this->opts->dbQueryStreamProfile.c_str(); }
  inline unsigned int getConnectionTimeout() const { return this->opts->connectionTimeout; }

  // Gui configuration interface
  inline const char * getMainWindowName() const { return this->opts->mainWinName.c_str(); }
  inline int getMainWinHSize() const { return this->opts->hSize; }
  inline int getMainWinVSize() const { return this->opts->vSize; }
  inline bool useVirtualStreamList() const { return this->opts->useVirtualList; }

protected:
  boost::shared_ptr<monitor::LivecastConnection> createConnection(const std::string& host, uint16_t port) const;

private:
  boost::shared_ptr<monitor::LivecastMonitor> monitor;  
  boost::shared_ptr<ProgramOption> opts;
  boost::array<boost::tuple<std::string, std::string, std::string, std::string>, ACCESS_LAST> access;
  unsigned int accessIndex;
};

}

#endif
