#include "Configuration.hh"
#include "monitor/LivecastMonitor.hh"
#include "monitor/LivecastConnection.hh"
#include "lib/Log.hh"

#include <wx/wx.h>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace po = boost::program_options;

using namespace livecast;

Configuration::ProgramOption::ProgramOption()
  : confFilename("/usr/local/etc/livecast-monitor.conf"),
    serversConf("/usr/local/etc/livecast-servers.conf"),
    logOutput("stdout"),
    hSize(-1),
    vSize(-1),
    connectionTimeout(200),
    logLevel(5),
    logColor(false),
    logDate(false),
    logPid(false),
    checkOnInit(false),
    useVirtualList(false)
{
}

int
Configuration::ProgramOption::parseMainWinSize()
{
  int rc = 0;
  try
  {
    std::string::size_type xpos = this->mainWinSize.find("x");
    if (xpos != std::string::npos)
    {
      this->hSize = boost::lexical_cast<int>(this->mainWinSize.substr(0, xpos));
      this->vSize = boost::lexical_cast<int>(this->mainWinSize.substr(xpos + 1));
    }
    else
    {
      LogError::getInstance().sysLog(ERROR, "bad value of main window size: missing 'x' separator");
      rc = -1;
    }
  }
  catch (const boost::bad_lexical_cast& ex)
  {
    LogError::getInstance().sysLog(ERROR, "bad value of main window size: %s", ex.what());
    rc = -1;
  }
  return rc;
}

//
//

Configuration::Configuration()
{
}

void Configuration::load(std::istream&)
{

//   try
//   {

//     boost::property_tree::ptree ptree;
//     boost::property_tree::read_xml(is, ptree);

//     for (boost::property_tree::ptree::const_iterator it = ptree.get_child("servers").begin(); it != ptree.get_child("servers").end(); ++it)
//     {
//       std::string host = it->second.get<std::string>("host");
//       uint16_t port = it->second.get<int>("port");
//       std::string user = it->second.get<std::string>("user");
//       std::string pass = it->second.get<std::string>("pass");
//       LogError::getInstance().sysLog(DEBUG, "insert connection with values : [ host => '%s' ; port => '%d' ; user => '%s' ; pass => '%s' ] ", host.c_str(), port, user.c_str(), pass.c_str());
//       LivecastConnectionPtr connection(new LivecastConnection(this->monitor->getIOService(), this->resultCb, host, port, user, pass, this));
//     }
//   }
//   catch (const std::exception& ex)
//   {
//     LogError::getInstance().sysLog(ERROR, "cannot parse servers configuration: %s", ex.what());
//   }

  if (this->opts != 0)
  {
    this->MonitorConfiguration::loadStreamList();
  }
}

void Configuration::parseOpts(int argc, char**argv)
{
  this->opts.reset(new Configuration::ProgramOption);
  bool error = false;
  try
  {
    
    po::variables_map input_arg;
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "produce this message")
      ("conf", po::value<std::string>(&this->opts->confFilename)->default_value(this->opts->confFilename), "set configuration file name")
      ("servers-conf", po::value<std::string>(&this->opts->serversConf)->default_value(this->opts->serversConf), "set configuration file name")
      ("log-output", po::value<std::string>(&this->opts->logOutput)->default_value(this->opts->logOutput), "log output (syslog, stdout)")
      ("log-ident", po::value<std::string>(&this->opts->logIdent)->default_value(argv[0]), "set log ident")
      ("log-level", po::value<unsigned int>(&this->opts->logLevel)->default_value(this->opts->logLevel), "set log level 2 to 7")
      ("log-color", po::bool_switch(&this->opts->logColor), "set color log")
      ("log-date", po::bool_switch(&this->opts->logDate), "put date in log")
      ("log-pid", po::bool_switch(&this->opts->logPid), "put pid in log")
      ("db-user", po::value<std::string>(&this->opts->dbUser)->default_value(this->opts->dbUser), "set user database")
      ("db-pass", po::value<std::string>(&this->opts->dbPass)->default_value(this->opts->dbPass), "set pass database")
      ("db-host", po::value<std::string>(&this->opts->dbHost)->default_value(this->opts->dbHost), "set host database")
      ("db-name", po::value<std::string>(&this->opts->dbName)->default_value(this->opts->dbName), "set name database")
      ("db-query-stream", po::value<std::string>(&this->opts->dbQueryStream)->default_value(this->opts->dbQueryStream), "set database query to load stream informations")
      ("db-query-stream-list", po::value<std::string>(&this->opts->dbQueryStreamList)->default_value(this->opts->dbQueryStreamList), "set database query to load stream list informations")
      ("db-query-stream-profile", po::value<std::string>(&this->opts->dbQueryStreamProfile)->default_value(this->opts->dbQueryStreamProfile), "set database query to load stream profiles informations")
      ("main-win-name", po::value<std::string>(&this->opts->mainWinName)->default_value(argv[0]), "set main window name")
      ("main-win-size", po::value<std::string>(&this->opts->mainWinSize), "set main window size")
      ("connection-timeout", po::value<unsigned int>(&this->opts->connectionTimeout)->default_value(this->opts->connectionTimeout), "server connection timeout in milliseconds")
      ("check-on-init", po::bool_switch(&this->opts->checkOnInit), "perform server check on initialisation")
      ("use-virtual-list", po::bool_switch(&this->opts->useVirtualList), "use a wx widget virtual list to display streams list (see wxWidgets Documentation for further informations)")
      ;

    po::store(po::parse_command_line(argc, argv, desc), input_arg);
    po::notify(input_arg);

    std::ifstream f(this->opts->confFilename.c_str());
    po::store(po::parse_config_file(f, desc, false), input_arg);
    f.close();
    po::notify(input_arg);

    if (input_arg.count("help"))
    {
      std::cout << desc << std::endl;      
      exit(0);
    }

    boost::algorithm::to_lower(this->opts->logOutput);
    if (this->opts->logOutput == "stdout")
    {
		  LogError::getInstance().setMode(STDOUT);
    }
    else if (this->opts->logOutput == "syslog")
    {
		  LogError::getInstance().setMode(SYSLOG);
    }
    LogError::getInstance().setLevel(this->opts->logLevel);
    LogError::getInstance().setIdent(this->opts->logIdent.c_str());
    LogError::getInstance().setLockMode(true);
    LogError::getInstance().setColorMode(this->opts->logColor);
    LogError::getInstance().setDateMode(this->opts->logDate);
    LogError::getInstance().setPidMode(this->opts->logPid);

    if (input_arg.count("main-win-size"))
    {
      if (this->opts->parseMainWinSize() < 0)
      {
        error = true;
      }
    }
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    error = true;
  }

  if (error)
  {
    throw new BadProgramOptions;
  }

}

boost::shared_ptr<monitor::LivecastConnection> Configuration::createConnection(const std::string& host, uint16_t port) const
{
  boost::shared_ptr<monitor::LivecastConnection> connection(new monitor::LivecastConnection(this->monitor->getIOService(), host, port, "tma", "k67bgt3b", this));
  return connection;
}
