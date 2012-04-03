#ifndef __LIVECAST_GUI_UTIL_HH__
#define __LIVECAST_GUI_UTIL_HH__

#include "../monitor/StreamInfos.hh"
#include <wx/colour.h>

namespace livecast {
namespace gui {

extern const wxColour lightYellow;
extern const wxColour lightBlue;
extern const wxColour orange;

std::ostream& operator<<(std::ostream& os, const livecast::monitor::StreamInfos::status_t status);

}
}

#endif
