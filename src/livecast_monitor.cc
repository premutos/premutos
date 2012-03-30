#include "Configuration.hh"
#include "lib/Log.hh"
#include "gui/LivecastGui.hh"
#include "monitor/LivecastMonitor.hh"

#include <fstream>
#include <boost/thread.hpp>

class MyApp : public wxApp
{
public:
  MyApp();
  ~MyApp();
  virtual bool OnInit();

private:
  boost::shared_ptr<Configuration::ProgramOption> parseProgramOptions();

  boost::shared_ptr<Configuration> cfg;
  boost::shared_ptr<LivecastGui> gui;
  boost::shared_ptr<LivecastMonitor> monitor;
};

IMPLEMENT_APP(MyApp);

MyApp::MyApp()
  : cfg(new Configuration),
    monitor(new LivecastMonitor(cfg))
{
}

MyApp::~MyApp()
{
}

bool MyApp::OnInit()
{
  try
  {
    this->cfg->parseOpts(wxApp::argc, wxApp::argv);
  }
  catch (const Configuration::BadProgramOptions& ex)
  {
    return EXIT_FAILURE;
  }
  
  // this->monitor.reset(new LivecastMonitor(this->cfg));
  this->gui.reset(new LivecastGui(this->cfg, this->monitor));
  this->cfg->setMonitor(monitor);

  LogError::getInstance().sysLog(DEBUG, "%s", this->cfg->getOpts()->serversConf.c_str());
  std::ifstream f(this->cfg->getOpts()->serversConf.c_str());
  cfg->load(f);
  f.close();

  boost::thread threadMonitor = boost::thread(boost::bind(&LivecastMonitor::run, monitor));
  gui->Show(true);

  if (this->cfg->getOpts()->checkOnInit)
  {
    gui->refresh();
  }

  return true;
}
