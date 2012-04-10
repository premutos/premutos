#include "Util.hh"
#include "LivecastListCtrl.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::gui;

LivecastListCtrl::LivecastListCtrl(wxWindow * parent)
  : wxListView(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL | wxBORDER_NONE)
{
  this->Bind(wxEVT_PAINT, &LivecastListCtrl::onPaint, this, wxID_ANY);
}

LivecastListCtrl::LivecastListCtrl(wxWindow * parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
  : wxListView(parent, id, pos, size, style)
{
}

void LivecastListCtrl::SetItemTextColour(long item, StreamInfos::status_t status)
{
  switch (status)
  {  
  case StreamInfos::STATUS_WAITING:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(*wxLIGHT_GREY))); break;
  case StreamInfos::STATUS_INITIALIZING: this->wxListView::SetItemTextColour(item, wxColour(wxColour(*wxBLUE)));       break;
  case StreamInfos::STATUS_RUNNING:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(*wxGREEN)));      break;
  case StreamInfos::STATUS_ERROR:        this->wxListView::SetItemTextColour(item, wxColour(wxColour(*wxRED)));        break;
  case StreamInfos::STATUS_UNKNOWN:      this->wxListView::SetItemTextColour(item, wxColour(wxColour(orange)));        break;
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
