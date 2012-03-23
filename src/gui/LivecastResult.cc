#include "../lib/Log.hh"
#include "LivecastResult.hh"

LivecastResult::LivecastResult(wxPanel * parent)
  : wxPanel(parent, 
            wxID_ANY, 
            wxDefaultPosition, 
            wxSize(-1, -1), 
            wxBORDER_NONE),
    resultEvent(wxNewEventType())
{
  wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);
  this->tree = new wxTreeCtrl(this, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1));
  hbox->Add(this->tree, 1, wxEXPAND, 10);
  this->SetSizer(hbox);
  wxTreeItemId id = this->tree->AddRoot("servers");
  this->tree->ExpandAll(id);
  Connect(resultEvent, wxCommandEventHandler(LivecastResult::onResultUpdate));
}

LivecastResult::~LivecastResult()
{
}

void LivecastResult::onResultUpdate(wxCommandEvent&)
{
  this->tree->DeleteAllItems();
  wxTreeItemId id = this->tree->AddRoot("servers");
  this->updateTree(this->resultTree, id);
  this->tree->ExpandAll(id);
}

void LivecastResult::clear()
{
  this->tree->DeleteAllItems();
  this->resultTree = boost::property_tree::ptree();
}

void LivecastResult::commit()
{
  LogError::getInstance().sysLog(DEBUG, "commit");
  wxCommandEvent event(resultEvent);
  this->GetEventHandler()->AddPendingEvent(event);
}

boost::property_tree::ptree& LivecastResult::getResultTree()
{
  return this->resultTree;
}

wxTreeItemId LivecastResult::updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id)
{
  wxTreeItemId idReturn = id;
  if (ptree.size() > 0)
  {    
    boost::property_tree::ptree::const_iterator it;
    for (it = ptree.begin(); it != ptree.end(); ++it)
    {    
      idReturn = this->updateTree(it->second, this->tree->AppendItem(id, it->first));
    }
  }
  else
  {
    this->tree->AppendItem(id, ptree.data());
  }
  return idReturn;
}
