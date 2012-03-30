#ifndef __LIVECAST_CONTROL_HH__
#define __LIVECAST_CONTROL_HH__

#include <wx/wx.h>

class LivecastGui;

class LivecastControl : public wxPanel
{
public:
  LivecastControl(LivecastGui * livecastGui);
  ~LivecastControl();

protected:
  void OnRefresh(wxCommandEvent & event);
  void OnCheck(wxCommandEvent & event);
  void OnQuit(wxCommandEvent & event);

private:
  LivecastGui * const livecastGui;
};

#endif
