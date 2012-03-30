#ifndef __GUI_CONFIGURATION_HH__
#define __GUI_CONFIGURATION_HH__

namespace livecast {
namespace gui {

class GuiConfiguration
{
public:
  GuiConfiguration();
  virtual ~GuiConfiguration();

  virtual const char * getMainWindowName() const = 0;
  virtual int getMainWinHSize() const = 0;
  virtual int getMainWinVSize() const = 0;
};

}
}

#endif
