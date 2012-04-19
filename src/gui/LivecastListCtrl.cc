#include "Util.hh"
#include "LivecastListCtrl.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;
using namespace livecast::lib;

static int wxCALLBACK ItemCmpFunc(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
  LogError::getInstance().sysLog(ERROR, "%x %x", item1, item2);
  LivecastListCtrl * ctrl = reinterpret_cast<LivecastListCtrl*>(sortData); 
  wxString a, b; 
  a = ctrl->GetItemText(item1, ctrl->sortCol); 
  b = ctrl->GetItemText(item2, ctrl->sortCol); 
  LogError::getInstance().sysLog(ERROR, "%s <? %s", a.ToStdString().c_str(), b.ToStdString().c_str());
  return a.CmpNoCase(b); 
}

LivecastListCtrl::LivecastListCtrl(wxWindow * parent, bool sortable)
  : wxListView(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL | wxBORDER_NONE),
    sortable(sortable)
{
  this->Bind(wxEVT_PAINT, &LivecastListCtrl::onPaint, this, wxID_ANY);
  this->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &LivecastListCtrl::onSort, this, wxID_ANY);
}

LivecastListCtrl::LivecastListCtrl(wxWindow * parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
  : wxListView(parent, id, pos, size, style),
    sortable(false)
{
}

void LivecastListCtrl::SetItemTextColour(long item, StreamInfos::status_t status)
{
  switch (status)
  {  
  case StreamInfos::STATUS_WAITING:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(livecast_grey)));     break;
  case StreamInfos::STATUS_INITIALIZING: this->wxListView::SetItemTextColour(item, wxColour(wxColour(livecast_yellow)));   break;
  case StreamInfos::STATUS_RUNNING:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(livecast_green)));    break;
  case StreamInfos::STATUS_ERROR:        this->wxListView::SetItemTextColour(item, wxColour(wxColour(livecast_red)));      break;
  case StreamInfos::STATUS_UNKNOWN:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(livecast_darkGrey))); break;
  }  
}

void LivecastListCtrl::onPaint(wxPaintEvent& ev)
{
  wxSize size = this->GetSize();
  unsigned int width = this->GetColumnCount();
  for (unsigned int i = 0; i < width; i++)
  {
    this->SetColumnWidth(i, size.GetWidth() / width);
  }
  ev.Skip();
}

void LivecastListCtrl::onSort(wxListEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "click on col %d", ev.GetColumn());
  if (sortable)
  {
    this->sortCol = ev.GetColumn();
    this->wxListCtrl::SortItems(ItemCmpFunc, (wxIntPtr)this);
  }
}
