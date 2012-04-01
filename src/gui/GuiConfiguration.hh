#ifndef __GUI_CONFIGURATION_HH__
#define __GUI_CONFIGURATION_HH__

#include <boost/shared_ptr.hpp>

namespace livecast {
namespace gui {

class GuiConfiguration
{
public:
  // static const GuiConfiguration * const getInstance();  
  // static void setInstance(const GuiConfiguration * const cfg);

  static const boost::shared_ptr<GuiConfiguration> getInstance();  
  static void setInstance(const boost::shared_ptr<GuiConfiguration> cfg);

  virtual ~GuiConfiguration();

  virtual const char * getMainWindowName() const = 0;
  virtual int getMainWinHSize() const = 0;
  virtual int getMainWinVSize() const = 0;
  virtual bool useVirtualStreamList() const = 0;

protected:
  GuiConfiguration();
};

}
}

#endif
