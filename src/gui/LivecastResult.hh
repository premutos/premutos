#ifndef __LIVECAST_RESULT_HH__
#define __LIVECAST_RESULT_HH__

#include "../ResultCallbackIntf.hh"
#include <wx/wx.h>
#include <wx/treectrl.h>

class LivecastResult : public wxPanel,
                       public ResultCallbackIntf
{
public:
  LivecastResult(wxPanel *parent);
  ~LivecastResult();

  void onResultUpdate(wxCommandEvent& ev);
  void clear();
  void commit();
  boost::property_tree::ptree& getResultTree();
private:
  wxTreeItemId updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id);

  wxTreeCtrl * tree;

  const wxEventType resultEvent;
  boost::property_tree::ptree resultTree;
};
 
#endif
