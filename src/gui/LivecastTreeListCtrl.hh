#ifndef __LIVECAST_TREE_LIST_CTRL_HH__
#define __LIVECAST_TREE_LIST_CTRL_HH__

#include <wx/treelistctrl.hh>

namespace livecast {
namespace gui {

class LivecastTreeListCtrl : public wxTreeListCtrl
{
public:
  LivecastTreeListCtrl(wxWindow * parent);

protected:
  void onPaint(wxPaintEvent& ev);
  void onSort(wxListEvent& ev);

private:
  bool sortOrder;
};

}
}

#endif
