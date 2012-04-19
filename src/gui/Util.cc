#include "Util.hh"
#include "../lib/Log.hh"
#include <sstream>

namespace livecast {
namespace gui {

const wxColour livecast_lightYellow(255, 255, 240);
const wxColour livecast_lightBlue(240, 255, 255);
const wxColour livecast_grey(200, 200, 200);
const wxColour livecast_yellow(222, 214, 0);
const wxColour livecast_green(0, 100, 0);
const wxColour livecast_red(180, 0, 0);
const wxColour livecast_darkGrey(100, 100, 100);

std::ostream& operator<<(std::ostream& os, const livecast::monitor::StreamInfos::status_t status)
{
  using namespace livecast::monitor;
  switch (status)
  {
  case StreamInfos::STATUS_WAITING:      os << "WAITING";      break;
  case StreamInfos::STATUS_INITIALIZING: os << "INITIALIZING"; break;
  case StreamInfos::STATUS_RUNNING:      os << "RUNNING";      break;
  case StreamInfos::STATUS_ERROR:        os << "ERROR";        break;
  case StreamInfos::STATUS_UNKNOWN:      os << "UNKNOWN";      break;
  default:
    livecast::lib::LogError::getInstance().sysLog(CRITICAL, "bad status %d", status);
    assert(false);
  }
  return os;
}

}
}
