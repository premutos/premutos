#ifndef __LIVECAST_TASKBAR_ICON_HH__
#define __LIVECAST_TASKBAR_ICON_HH__

#include <wx/taskbar.h>

class LivecastGui;

class LivecastTaskBarIcon : public wxTaskBarIcon
{
public:
  LivecastTaskBarIcon(LivecastGui * livecastGui);

  void OnLeftButtonDClick(wxTaskBarIconEvent&);
  void OnMenuRestore(wxCommandEvent&);
  void OnMenuExit(wxCommandEvent&);
  void OnMenuCheckStream(wxCommandEvent&);
  void OnMenuSub(wxCommandEvent&);
  virtual wxMenu *CreatePopupMenu();

private:
  LivecastGui * const livecastGui;
};

#endif
