#ifndef __LIVECAST_CONTROL_HH__
#define __LIVECAST_CONTROL_HH__

#include <wx/wx.h>

class LivecastControl : public wxPanel
{
public:
  LivecastControl(wxPanel *parent);
  ~LivecastControl();

  wxPanel * parent;

protected:
  void OnCheck(wxCommandEvent & event);
  void OnQuit(wxCommandEvent & event);
};

#endif
