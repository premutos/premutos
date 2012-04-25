#include "LivecastTreeListCtrl.hh"
#include "../lib/Log.hh"

using namespace livecast::gui;
using namespace livecast::lib;

LivecastTreeListCtrl::LivecastTreeListCtrl(wxWindow * parent)
  : wxTreeListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_ROW_LINES),
    sortOrder(true)
{
  this->Bind(wxEVT_PAINT, &LivecastTreeListCtrl::onPaint, this, wxID_ANY);
  this->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &LivecastTreeListCtrl::onSort, this, wxID_ANY);
}

void LivecastTreeListCtrl::onPaint(wxPaintEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "paint");
  wxSize size = this->GetSize();
  unsigned int width = this->GetColumnCount();
  for (unsigned int i = 0; i < width; i++)
  {
    this->SetColumnWidth(i, size.GetWidth() / width);
  }
  ev.Skip();
}

void LivecastTreeListCtrl::onSort(wxListEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "click on col %d", ev.GetColumn());
  this->SortChildren(this->GetRootItem(), ev.GetColumn(), this->sortOrder);
  this->sortOrder = !this->sortOrder;
}
