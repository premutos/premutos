#ifndef __LIVECAST_PREFERENCES_HH__
#define __LIVECAST_PREFERENCES_HH__

#include <wx/wx.h>

namespace livecast {
namespace gui {

class LivecastPreferences : public wxFrame
{
public:
  LivecastPreferences(wxWindow * parent);
public:
  wxTextCtrl * dbHost;
  wxTextCtrl * dbName;
  wxTextCtrl * dbUser;
  wxTextCtrl * dbPass;
  wxTextCtrl * apiUrl;
};

}
}

#endif
