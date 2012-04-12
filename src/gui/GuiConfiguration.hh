#ifndef __GUI_CONFIGURATION_HH__
#define __GUI_CONFIGURATION_HH__

#include <list>
#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class GuiConfiguration
{
public:
  static const boost::shared_ptr<GuiConfiguration> getInstance();  
  static void setInstance(const boost::shared_ptr<GuiConfiguration> cfg);

  virtual ~GuiConfiguration();

  virtual const char * getMainWindowName() const = 0;
  virtual int getMainWinHSize() const = 0;
  virtual int getMainWinVSize() const = 0;
  virtual bool useVirtualStreamList() const = 0;
  virtual const std::list<std::string> getViews() const = 0;

protected:
  GuiConfiguration();
};

}
}

#endif
