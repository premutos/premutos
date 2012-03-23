#ifndef __CONFIGURATION_HH__
#define __CONFIGURATION_HH__

#include "monitor/MonitorConfiguration.hh"
#include "ResultCallbackIntf.hh"
#include "monitor/LivecastMonitor.hh"
#include <sstream>

class Configuration : public MonitorConfiguration
{
public:

  class BadProgramOptions
  {
  };

  struct ProgramOption
  {
    std::string confFilename;
    std::string serversConf;
    std::string mainWinSize;
    std::string logOutput;
    std::string logIdent;
    std::string dbUser;
    std::string dbPass;
    std::string dbHost;
    std::string dbName;
    std::string dbQuery;
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
  inline void setResult(boost::shared_ptr<ResultCallbackIntf> resultCb) { this->resultCb = resultCb; }
  inline const boost::shared_ptr<ProgramOption> getOpts() const { return this->opts; }
  inline unsigned int getConnectionTimeout() const { return this->opts->connectionTimeout; }

private:
  boost::shared_ptr<LivecastMonitor> monitor;  
  boost::shared_ptr<ResultCallbackIntf> resultCb;
  boost::shared_ptr<ProgramOption> opts;
};

#endif
