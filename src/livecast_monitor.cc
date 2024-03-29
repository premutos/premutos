#include "Configuration.hh"
#include "lib/Log.hh"
#include "gui/LivecastGui.hh"
#include "monitor/LivecastMonitor.hh"

#include <fstream>
#include <boost/thread.hpp>

namespace livecast {

class Livecast : public wxApp
{
public:
  Livecast();
  ~Livecast();
  virtual bool OnInit();

private:
  boost::shared_ptr<Configuration::ProgramOption> parseProgramOptions();

  boost::shared_ptr<Configuration> cfg;
  boost::shared_ptr<monitor::LivecastMonitor> monitor;
  boost::shared_ptr<gui::LivecastGui> gui;
};

}

using namespace livecast;

IMPLEMENT_APP(Livecast);

Livecast::Livecast()
  : cfg(new Configuration),
    monitor(new monitor::LivecastMonitor(cfg))
{
  gui::GuiConfiguration::setInstance(this->cfg);
}

Livecast::~Livecast()
{
}

bool Livecast::OnInit()
{
  try
  {
    this->cfg->parseOpts(wxApp::argc, wxApp::argv);
  }
  catch (const Configuration::BadProgramOptions& ex)
  {
    return EXIT_FAILURE;
  }

  livecast::lib::LogError::getInstance().sysLog(DEBUG, "%s", this->cfg->getOpts()->dbAccessFilename.c_str());
  std::ifstream f(this->cfg->getOpts()->dbAccessFilename.c_str());
  cfg->loadAccess(f);
  f.close();
  
  this->gui.reset(new gui::LivecastGui(cfg, monitor));
  this->cfg->setMonitor(this->monitor);

  cfg->load();

  boost::thread threadMonitor = boost::thread(boost::bind(&monitor::LivecastMonitor::run, monitor));
  gui->Show(true);

  if (this->cfg->getOpts()->checkOnInit)
  {
    boost::thread(boost::bind(&gui::LivecastGui::refresh, gui));
  }

  return true;
}
