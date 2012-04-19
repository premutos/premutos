#ifndef __LIVECAST_GUI_UTIL_HH__
#define __LIVECAST_GUI_UTIL_HH__

#include "../monitor/StreamInfos.hh"
#include <wx/colour.h>

namespace livecast {
namespace gui {

extern const wxColour livecast_lightYellow;
extern const wxColour livecast_lightBlue;
extern const wxColour livecast_grey;
extern const wxColour livecast_yellow;
extern const wxColour livecast_green;
extern const wxColour livecast_red;
extern const wxColour livecast_darkGrey;

std::ostream& operator<<(std::ostream& os, const livecast::monitor::StreamInfos::status_t status);

}
}

#endif
