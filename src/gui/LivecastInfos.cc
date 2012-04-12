#include "Util.hh"
#include "LivecastInfos.hh"
#include "LivecastListCtrl.hh"
#include "LivecastStatus.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace gui;
using namespace monitor;

LivecastInfos::LivecastInfos(wxWindow * parent)
  : wxPanel(parent, wxID_ANY),
    currentSelectionPage(0)
{

  // prepare infos
  {
    this->infos = new wxPanel(this, wxID_ANY);
    wxPanel * labelPanel = new wxPanel(this->infos, wxID_ANY);
    wxPanel * valuePanel = new wxPanel(this->infos, wxID_ANY);

    wxStaticText * id = new wxStaticText(labelPanel, wxID_ANY, wxT("Id:"));
    wxStaticText * mode = new wxStaticText(labelPanel, wxID_ANY, wxT("Mode:"));
    wxStaticText * srcIp = new wxStaticText(labelPanel, wxID_ANY, wxT("Source IP:"));
    wxStaticText * dstHost = new wxStaticText(labelPanel, wxID_ANY, wxT("Destination hostname:"));
    wxStaticText * dstPort = new wxStaticText(labelPanel, wxID_ANY, wxT("Destination port:"));
    wxStaticText * protocol = new wxStaticText(labelPanel, wxID_ANY, wxT("Protocol:"));
    wxStaticText * extKey = new wxStaticText(labelPanel, wxID_ANY, wxT("External key:"));
    wxStaticText * backlog = new wxStaticText(labelPanel, wxID_ANY, wxT("Backlog:"));
    wxStaticText * nbConnections = new wxStaticText(labelPanel, wxID_ANY, wxT("Nb connections:"));
    wxStaticText * enabled = new wxStaticText(labelPanel, wxID_ANY, wxT("Enabled:"));
    wxStaticText * disableFilter = new wxStaticText(labelPanel, wxID_ANY, wxT("Disable filter:"));

    id->SetFont(this->GetFont().Underlined());
    mode->SetFont(this->GetFont().Underlined());
    srcIp->SetFont(this->GetFont().Underlined());
    dstHost->SetFont(this->GetFont().Underlined());
    dstPort->SetFont(this->GetFont().Underlined());
    protocol->SetFont(this->GetFont().Underlined());
    extKey->SetFont(this->GetFont().Underlined());
    backlog->SetFont(this->GetFont().Underlined());
    nbConnections->SetFont(this->GetFont().Underlined());
    enabled->SetFont(this->GetFont().Underlined());
    disableFilter->SetFont(this->GetFont().Underlined());

    this->idValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->modeValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->srcIpValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->dstHostValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->dstPortValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->protocolValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->extKeyValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->backlogValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->nbConnectionsValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->enabledValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
    this->disableFilterValue = new wxStaticText(valuePanel, wxID_ANY, "not set");

    this->idValue->SetFont(this->GetFont().Bold());
    this->modeValue->SetFont(this->GetFont().Bold());
    this->srcIpValue->SetFont(this->GetFont().Bold());
    this->dstHostValue->SetFont(this->GetFont().Bold());
    this->dstPortValue->SetFont(this->GetFont().Bold());
    this->protocolValue->SetFont(this->GetFont().Bold());
    this->extKeyValue->SetFont(this->GetFont().Bold());
    this->backlogValue->SetFont(this->GetFont().Bold());
    this->nbConnectionsValue->SetFont(this->GetFont().Bold());
    this->enabledValue->SetFont(this->GetFont().Bold());
    this->disableFilterValue->SetFont(this->GetFont().Bold());

    wxStaticBox * box = new wxStaticBox(this->infos, wxID_ANY, "global infos");
    wxStaticBoxSizer * sizer = new wxStaticBoxSizer(box, wxHORIZONTAL);
    wxBoxSizer * sizerLabel = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * sizerValue = new wxBoxSizer(wxVERTICAL);

    sizerLabel->Add(id, 0, wxALL, 10);
    sizerLabel->Add(mode, 0, wxALL, 10);
    sizerLabel->Add(srcIp, 0, wxALL, 10);
    sizerLabel->Add(dstHost, 0, wxALL, 10);
    sizerLabel->Add(dstPort, 0, wxALL, 10);
    sizerLabel->Add(protocol, 0, wxALL, 10);
    sizerLabel->Add(extKey, 0, wxALL, 10);
    sizerLabel->Add(nbConnections, 0, wxALL, 10);
    sizerLabel->Add(backlog, 0, wxALL, 10);
    sizerLabel->Add(enabled, 0, wxALL, 10);
    sizerLabel->Add(disableFilter, 0, wxALL, 10);
  
    sizerValue->Add(this->idValue, 0, wxALL, 10);
    sizerValue->Add(this->modeValue, 0, wxALL, 10);
    sizerValue->Add(this->srcIpValue, 0, wxALL, 10);
    sizerValue->Add(this->dstHostValue, 0, wxALL, 10);
    sizerValue->Add(this->dstPortValue, 0, wxALL, 10);
    sizerValue->Add(this->protocolValue, 0, wxALL, 10);
    sizerValue->Add(this->extKeyValue, 0, wxALL, 10);
    sizerValue->Add(this->nbConnectionsValue, 0, wxALL, 10);
    sizerValue->Add(this->backlogValue, 0, wxALL, 10);
    sizerValue->Add(this->enabledValue, 0, wxALL, 10);
    sizerValue->Add(this->disableFilterValue, 0, wxALL, 10);

    labelPanel->SetSizer(sizerLabel);
    valuePanel->SetSizer(sizerValue);

    sizer->Add(labelPanel, 1);
    sizer->Add(valuePanel, 1);

    this->infos->SetSizer(sizer);
  }

  // servers, profiles, status schema
  {
    this->noteBook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);

    // profiles
    this->profiles = new LivecastListCtrl(this->noteBook);
    unsigned int index = 0;
    this->profiles->InsertColumn(index++, "Id", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Width", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Height", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Video Bitrate", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Audio Bitrate", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Deinterlace", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Framerate", wxLIST_FORMAT_LEFT);
    this->profiles->InsertColumn(index++, "Protocols", wxLIST_FORMAT_LEFT);
    this->noteBook->InsertPage(1, this->profiles, "profiles", true);

    // servers
    this->servers = new LivecastListCtrl(this->noteBook);
    index = 0;
    this->servers->InsertColumn(index++, "row", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "type", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "hostname", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "ip", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "port", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "status", wxLIST_FORMAT_LEFT);
    this->noteBook->InsertPage(2, this->servers, "servers", false);

    // status schema
    // to be added in setInfos method

    this->noteBook->Bind(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, &LivecastInfos::onTabMiddleUp, this, wxID_ANY);
  }

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->infos, 0, wxALIGN_LEFT | wxALL, 10);
  box->Add(this->noteBook, 1, wxEXPAND | wxALL, 10);
  this->SetSizer(box);
}

boost::shared_ptr<ResultCallbackIntf> LivecastInfos::setInfos(boost::shared_ptr<const livecast::monitor::StreamInfos> streamInfos)
{

  this->currentSelectionPage = this->noteBook->GetSelection();
  this->streamInfos = streamInfos;

  // infos
  this->idValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_ID].c_str());
  this->modeValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_MODE].c_str());
  this->srcIpValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_SRC_IP].c_str());
  this->dstHostValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_DST_HOST].c_str());
  this->dstPortValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_DST_PORT].c_str());
  this->protocolValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_PROTOCOL].c_str());
  this->extKeyValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_EXT_KEY].c_str());
  this->backlogValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_BACKLOG].c_str());
  this->nbConnectionsValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_NB_CONNECTIONS].c_str());
  this->enabledValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_ENABLED].c_str());
  this->disableFilterValue->SetLabel(streamInfos->getInfos()[StreamInfos::FIELDS_DISABLE_FILTER].c_str());

  unsigned int n = 0;

  // profiles
  n = 0;
  this->profiles->DeleteAllItems();
  for (StreamInfos::profiles_t::const_iterator it = streamInfos->getProfiles().begin();
       it != streamInfos->getProfiles().end(); ++it)
  {
    wxListItem item;
    item.SetId(n);
    unsigned int index = this->profiles->InsertItem(item);
    assert(index == n);
    for (unsigned int i = 0; i < StreamInfos::PROFILE_LAST; i++)
    {
      this->profiles->SetItem(index, i, (*it)[i]);
    }
    this->profiles->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
    n++;
  }

  // servers
  n = 0;
  this->servers->DeleteAllItems();
  const StreamInfos::servers_t& serv = streamInfos->getServers();
  for (unsigned int r = 0; r < 2; r++)
  {
    for (unsigned int t = 0; t < 3; t++)
    {
      const char * row = ((r == 0) ? "primary" : "backup");
      for (std::list<StreamInfos::server_t>::const_iterator it = serv[r][t].begin(); it != serv[r][t].end(); ++it)
      {
        const char * type = (((*it).type == StreamInfos::server_t::STREAM_DUP) ? "streamdup" : 
                             ((*it).type == StreamInfos::server_t::MASTER_BOX) ? "masterbox" : 
                             ((*it).type == StreamInfos::server_t::STREAMER_RTMP) ? "streamer rtmp" :
                             ((*it).type == StreamInfos::server_t::STREAMER_HLS) ? "streamer hls" :
                             "unknown");
        wxListItem item;
        item.SetId(n);
        unsigned int index = this->servers->InsertItem(item);

        std::ostringstream portSS;
        std::ostringstream statusSS;
        portSS << (*it).port;
        statusSS << (*it).status;
        this->servers->SetItem(index, 0, row);
        this->servers->SetItem(index, 1, type);
        this->servers->SetItem(index, 2, (*it).host.c_str());
        this->servers->SetItem(index, 3, "n/a");
        this->servers->SetItem(index, 4, portSS.str().c_str());
        this->servers->SetItem(index, 5, statusSS.str().c_str());
        this->servers->SetItemTextColour(index, (*it).status);
        this->servers->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? lightYellow : lightBlue)));
        n++;
      }
    }
  }

  // status schema
  for (size_t i = 2; i < this->noteBook->GetPageCount(); i++)
  {
    this->noteBook->RemovePage(i);
  }
  this->statusSchema = new LivecastStatus(this->noteBook, streamInfos, true);
  this->noteBook->InsertPage(3, this->statusSchema, "status schema", true);

  // set selection on servers list
  this->noteBook->SetSelection((this->currentSelectionPage < this->noteBook->GetPageCount()) ? this->currentSelectionPage : 0);

  boost::shared_ptr<LivecastStatusCallback> cb(new LivecastStatusCallback(this->statusSchema));
  return cb;
}

void LivecastInfos::refresh()
{
  if (this->streamInfos && this->streamInfos->isModified())
  {
    this->setInfos(this->streamInfos);
  }
}

void LivecastInfos::onTabMiddleUp(wxAuiNotebookEvent& event)
{
  if (event.GetSelection() > 2)
  {
    this->noteBook->DeletePage(event.GetSelection());
  }
}
