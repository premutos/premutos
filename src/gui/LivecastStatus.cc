#include "../lib/Log.hh"
#include "LivecastStatus.hh"
#include "LivecastResult.hh"

#include <wx/splitter.h>

#include <boost/property_tree/xml_parser.hpp>

LivecastStatus::LivecastStatus(LivecastResult * livecastResult, const std::string& windowName, boost::shared_ptr<StreamInfos> streamInfos)
  : wxFrame(livecastResult, 
            wxID_ANY, 
            windowName,
            wxDefaultPosition, 
            wxSize(600, 400)),
    checkStreamEvent(wxNewEventType()),
    streamId(streamInfos->getId()),
    livecastResult(livecastResult),
    streamInfos(streamInfos)
{ 
  wxSplitterWindow * hSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_BORDER);  
  this->infos = new wxPanel(hSplitter, wxID_ANY);
  this->status = new wxPanel(hSplitter, wxID_ANY);

  wxSplitterWindow * vSplitter = new wxSplitterWindow(this->status, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_BORDER);
  this->schema = new wxPanel(vSplitter, wxID_ANY);
  this->tree = new wxTreeCtrl(vSplitter, wxID_ANY);
  vSplitter->SplitHorizontally(this->schema, this->tree, 200);
  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(vSplitter, 1, wxEXPAND | wxALL, 10);
  
  this->status->SetSizer(box);
  {
    wxStaticText * id = new wxStaticText(this->infos, wxID_ANY, wxT("Id"));
    wxStaticText * mode = new wxStaticText(this->infos, wxID_ANY, wxT("Mode"));
    wxStaticText * srcIp = new wxStaticText(this->infos, wxID_ANY, wxT("Source IP"));
    wxStaticText * dstHost = new wxStaticText(this->infos, wxID_ANY, wxT("Destination hostname"));
    wxStaticText * dstPort = new wxStaticText(this->infos, wxID_ANY, wxT("Destination port"));
    wxStaticText * protocol = new wxStaticText(this->infos, wxID_ANY, wxT("Protocol"));
    wxStaticText * extKey = new wxStaticText(this->infos, wxID_ANY, wxT("External key"));
    wxStaticText * backlog = new wxStaticText(this->infos, wxID_ANY, wxT("Backlog"));
    wxStaticText * nbConnections = new wxStaticText(this->infos, wxID_ANY, wxT("Nb connections"));
    wxStaticText * enabled = new wxStaticText(this->infos, wxID_ANY, wxT("Enabled"));
    wxStaticText * disableFilter = new wxStaticText(this->infos, wxID_ANY, wxT("Disable filter"));

    this->idValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_ID].c_str());
    this->modeValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_MODE].c_str());
    this->srcIpValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_SRC_IP].c_str());
    this->dstHostValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_DST_HOST].c_str());
    this->dstPortValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_DST_PORT].c_str());
    this->protocolValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_PROTOCOL].c_str());
    this->extKeyValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_EXT_KEY].c_str());
    this->backlogValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_BACKLOG].c_str());
    this->nbConnectionsValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_NB_CONNECTIONS].c_str());
    this->enabledValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_ENABLED].c_str());
    this->disableFilterValue = new wxStaticText(this->infos, wxID_ANY, streamInfos->infos[StreamInfos::FIELDS_DISABLE_FILTER].c_str());

    wxGridSizer * grid = new wxGridSizer(2, 12, 10);

    grid->Add(id, 0);
    grid->Add(this->idValue, 0);
    grid->Add(mode, 0);
    grid->Add(this->modeValue, 0);
    grid->Add(srcIp, 0);
    grid->Add(this->srcIpValue, 0);
    grid->Add(dstHost, 0);
    grid->Add(this->dstHostValue, 0);
    grid->Add(dstPort, 0);
    grid->Add(this->dstPortValue, 0);
    grid->Add(protocol, 0);
    grid->Add(this->protocolValue, 0);
    grid->Add(extKey, 0);
    grid->Add(this->extKeyValue, 0);
    grid->Add(nbConnections, 0);
    grid->Add(this->nbConnectionsValue, 0);
    grid->Add(backlog, 0);
    grid->Add(this->backlogValue, 0);
    grid->Add(enabled, 0);
    grid->Add(this->enabledValue, 0);
    grid->Add(disableFilter, 0);
    grid->Add(this->disableFilterValue, 0);

    wxButton * button = new wxButton(this->infos, wxID_APPLY, wxT("Refresh"));
    grid->Add(button, 0);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LivecastStatus::onRefresh, this, wxID_APPLY);

    this->infos->SetSizer(grid);
  }
  
  hSplitter->SplitVertically(this->infos, this->status, 300);

  this->Connect(checkStreamEvent, wxCommandEventHandler(LivecastStatus::onCheckStream));
  this->Bind(wxEVT_CLOSE_WINDOW, &LivecastStatus::onCloseWindow, this, wxID_ANY);
}

LivecastStatus::~LivecastStatus()
{
}

void LivecastStatus::commitStreamList()
{
}

void LivecastStatus::commitCheckStream(unsigned int streamId)
{
  LogError::getInstance().sysLog(DEBUG, "commit check stream");
  wxCommandEvent event(checkStreamEvent);
  event.SetInt(streamId);
  this->GetEventHandler()->AddPendingEvent(event);
}

wxTreeItemId LivecastStatus::updateTree(const boost::property_tree::ptree& ptree, const wxTreeItemId id)
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

void LivecastStatus::onCheckStream(wxCommandEvent& ev)
{
  LogError::getInstance().sysLog(INFO, "get status of stream %d", ev.GetInt());
  this->tree->DeleteAllItems();
  wxTreeItemId id = this->tree->AddRoot("servers");
  this->updateTree(this->streamInfos->getResultTree(), id);
  this->tree->ExpandAll();  
}

void LivecastStatus::onRefresh(wxCommandEvent& ev)
{
  LogError::getInstance().sysLog(DEBUG, "refresh");
  ev.Skip();
}

void LivecastStatus::onCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  this->Destroy();
  this->livecastResult->removeStreamStatus(this->streamId);
}
