#include "Util.hh"
#include "../lib/Log.hh"
#include <sstream>

namespace livecast {
namespace gui {

const wxColour lightYellow(255, 255, 240);
const wxColour lightBlue(240, 255, 255);
const wxColour grey(150, 150, 150);
const wxColour green(16, 148, 66);
const wxColour orange(225, 143, 26);

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
