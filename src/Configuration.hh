#ifndef __CONFIGURATION_HH__
#define __CONFIGURATION_HH__

#include "gui/GuiConfiguration.hh"
#include "monitor/MonitorConfiguration.hh"
#include "monitor/LivecastMonitor.hh"
#include <sstream>

class Configuration : public MonitorConfiguration,
                      public GuiConfiguration
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

    ProgramOption();
    int parseMainWinSize();
  };

public:
  Configuration();
  void parseOpts(int argc, char**argv);
  void load(std::istream& is);

  inline void setMonitor(boost::shared_ptr<LivecastMonitor> monitor) { this->monitor = monitor; }
  inline const boost::shared_ptr<ProgramOption> getOpts() const { return this->opts; }

  // Monitor configuration interface
  virtual const char * getDbUser() const { return this->opts->dbUser.c_str(); }
  virtual const char * getDbPass() const { return this->opts->dbPass.c_str(); }
  virtual const char * getDbHost() const { return this->opts->dbHost.c_str(); }
  virtual const char * getDbName() const { return this->opts->dbName.c_str(); }
  inline const char * getQueryStream() const { return this->opts->dbQueryStream.c_str(); }
  inline const char * getQueryStreamList() const { return this->opts->dbQueryStreamList.c_str(); }
  inline const char * getQueryStreamProfile() const { return this->opts->dbQueryStreamProfile.c_str(); }
  inline unsigned int getConnectionTimeout() const { return this->opts->connectionTimeout; }

  // Gui configuration interface
  inline const char * getMainWindowName() const { return this->opts->mainWinName.c_str(); }
  inline int getMainWinHSize() const { return this->opts->hSize; }
  inline int getMainWinVSize() const { return this->opts->vSize; }

protected:
  boost::shared_ptr<LivecastConnection> createConnection(const std::string& host, uint16_t port) const;

private:
  boost::shared_ptr<LivecastMonitor> monitor;  
  boost::shared_ptr<ProgramOption> opts;
};

#endif
