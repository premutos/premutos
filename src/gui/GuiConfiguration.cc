#include "GuiConfiguration.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::gui;

static boost::shared_ptr<GuiConfiguration> instance;

const boost::shared_ptr<GuiConfiguration> GuiConfiguration::getInstance()
{
  if (instance.get() == 0)
  {
    LogError::getInstance().sysLog(CRITICAL, "configuration not set");
  }
  return instance;
}

void GuiConfiguration::setInstance(const boost::shared_ptr<GuiConfiguration> cfg)
{
  instance = cfg;
}

GuiConfiguration::GuiConfiguration()
{
}

GuiConfiguration::~GuiConfiguration()
{
}
