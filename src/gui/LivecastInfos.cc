#include "Util.hh"
#include "LivecastInfos.hh"
#include "LivecastListCtrl.hh"
#include "LivecastTreeListCtrl.hh"
#include "LivecastStatus.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace gui;
using namespace monitor;
using namespace livecast::lib;

LivecastInfos::LivecastInfos(wxWindow * parent)
  : wxPanel(parent, wxID_ANY),
    currentSelectionPage(0)
{

  // prepare infos
  {
    this->infos = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
    // this->infos->SetScrollbars(10, 10, 100, 100);

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
    this->noteBook->InsertPage(0, this->profiles, "profiles", true);
    this->profiles->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &LivecastInfos::onProfilesListDblClicked, this, wxID_ANY);

    // servers
    this->servers = new LivecastTreeListCtrl(this->noteBook);
    this->servers->AddColumn("row");
    this->servers->AddColumn("type");
    this->servers->AddColumn("hostname");
    this->servers->AddColumn("ip");
    this->servers->AddColumn("port");
    this->servers->AddColumn("status");
    this->servers->AddColumn("details");
    this->servers->AddRoot("");
    this->noteBook->InsertPage(1, this->servers, "servers", false);

    // status schema
    // to be added in setInfos method

    this->noteBook->Bind(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, &LivecastInfos::onTabMiddleUp, this, wxID_ANY);
  }

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->infos, 0, wxALIGN_LEFT | wxALL, 10);
  box->Add(this->noteBook, 1, wxEXPAND | wxALL, 10);
  this->SetSizer(box);
}

std::list<boost::shared_ptr<ResultCallbackIntf> > LivecastInfos::setInfos(boost::shared_ptr<const livecast::monitor::StreamInfos> streamInfos)
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
    for (unsigned int i = 0; i < StreamInfos::PROFILE_PRIMARY_CDN_URL; i++)
    {
      this->profiles->SetItem(index, i, (*it)[i]);
    }
    this->profiles->SetItemBackgroundColour(index, wxColour(wxColour(((index % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));
    n++;
  }

  // servers
  n = 0;
  wxTreeItemId root = this->servers->GetRootItem();
  this->servers->DeleteChildren(root);
  const StreamInfos::servers_t& serv = streamInfos->getServers();
  for (unsigned int r = 0; r < 2; r++)
  {
    const char * row = ((r == 0) ? "primary" : "backup");
    wxTreeItemId rowItem = this->servers->AppendItem (root, row);
    this->servers->SetItemBackgroundColour(rowItem, livecast_grey);
    for (unsigned int t = 0; t < 3; t++)
    {
      for (std::list<StreamInfos::server_t>::const_iterator it = serv[r][t].begin(); it != serv[r][t].end(); ++it)
      {
        const char * type = (((*it).type == StreamInfos::server_t::STREAM_DUP) ? "streamdup" : 
                             ((*it).type == StreamInfos::server_t::MASTER_BOX) ? "masterbox" : 
                             ((*it).type == StreamInfos::server_t::STREAMER_RTMP) ? "streamer rtmp" :
                             ((*it).type == StreamInfos::server_t::STREAMER_HLS) ? "streamer hls" :
                             "unknown");

        wxTreeItemId item = this->servers->AppendItem (rowItem, row);

        std::ostringstream portSS;
        std::ostringstream statusSS;
        portSS << (*it).port;
        statusSS << (*it).status;

        unsigned int col = 1;
        this->servers->SetItemText(item, col++, type);
        this->servers->SetItemText(item, col++, (*it).host.c_str());
        this->servers->SetItemText(item, col++, "n/a");
        this->servers->SetItemText(item, col++, portSS.str().c_str());
        this->servers->SetItemText(item, col++, statusSS.str().c_str());
        this->servers->SetItemText(item, col++, "n/a");

        switch ((*it).status)
        {  
        case StreamInfos::STATUS_WAITING:      this->servers->SetItemTextColour(item, wxColour(wxColour(livecast_grey)));     break;
        case StreamInfos::STATUS_INITIALIZING: this->servers->SetItemTextColour(item, wxColour(wxColour(livecast_yellow)));   break;
        case StreamInfos::STATUS_RUNNING:      this->servers->SetItemTextColour(item, wxColour(wxColour(livecast_green)));    break;
        case StreamInfos::STATUS_ERROR:        this->servers->SetItemTextColour(item, wxColour(wxColour(livecast_red)));      break;
        case StreamInfos::STATUS_UNKNOWN:      this->servers->SetItemTextColour(item, wxColour(wxColour(livecast_darkGrey))); break;
        }  

        this->servers->SetItemBackgroundColour(item, wxColour(wxColour(((n % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));

        // add details
        unsigned int nDetails = n + 1;
        for (std::list<boost::tuple<StreamInfos::status_t, std::string, std::string> >::const_iterator itDetails = (*it).statusDetails.begin(); itDetails != (*it).statusDetails.end(); ++itDetails)
        {
          wxTreeItemId itemDetails = this->servers->AppendItem (item, row);
          col = 1;
          this->servers->SetItemText(itemDetails, col++, type);
          this->servers->SetItemText(itemDetails, col++, (*it).host.c_str());
          this->servers->SetItemText(itemDetails, col++, "n/a");
          this->servers->SetItemText(itemDetails, col++, portSS.str().c_str());
          this->servers->SetItemText(itemDetails, col++, (*itDetails).get<1>().c_str());
          this->servers->SetItemText(itemDetails, col++, (*itDetails).get<2>().c_str());
          
          switch ((*itDetails).get<0>())
          {  
          case StreamInfos::STATUS_WAITING:      this->servers->SetItemTextColour(itemDetails, wxColour(wxColour(livecast_grey)));     break;
          case StreamInfos::STATUS_INITIALIZING: this->servers->SetItemTextColour(itemDetails, wxColour(wxColour(livecast_yellow)));   break;
          case StreamInfos::STATUS_RUNNING:      this->servers->SetItemTextColour(itemDetails, wxColour(wxColour(livecast_green)));    break;
          case StreamInfos::STATUS_ERROR:        this->servers->SetItemTextColour(itemDetails, wxColour(wxColour(livecast_red)));      break;
          case StreamInfos::STATUS_UNKNOWN:      this->servers->SetItemTextColour(itemDetails, wxColour(wxColour(livecast_darkGrey))); break;
          }  
          
          this->servers->SetItemBackgroundColour(itemDetails, wxColour(wxColour(((nDetails % 2) == 0) ? livecast_lightYellow : livecast_lightBlue)));
          nDetails++;
        }

        n++;
      }
    }
    
    // expand row
    this->servers->Expand(rowItem);

  }

  // clean schema
  for (size_t i = 2; i <= this->noteBook->GetPageCount() + 1; i++)
  {
    LogError::getInstance().sysLog(DEBUG, "remove tab %d", i);
    this->noteBook->RemovePage(i);
    this->noteBook->DeletePage(i);
  }

  // primary status schema
  this->primaryStatusSchema = new LivecastStatus(this->noteBook, streamInfos);
  this->noteBook->InsertPage(2, this->primaryStatusSchema, "primary schema", true);

  // backup status schema
  this->backupStatusSchema = new LivecastStatus(this->noteBook, streamInfos, false);
  this->noteBook->InsertPage(3, this->backupStatusSchema, "backup schema", true);

  // set selection
  this->noteBook->SetSelection((this->currentSelectionPage < this->noteBook->GetPageCount()) ? this->currentSelectionPage : 0);

  boost::shared_ptr<LivecastStatusCallback> cb1(new LivecastStatusCallback(this->primaryStatusSchema));
  boost::shared_ptr<LivecastStatusCallback> cb2(new LivecastStatusCallback(this->backupStatusSchema));
  std::list<boost::shared_ptr<ResultCallbackIntf> > cbs;
  cbs.push_back(cb1);
  cbs.push_back(cb2);
  return cbs;
}

void LivecastInfos::refresh()
{
  if (this->streamInfos && this->streamInfos->isModified())
  {
    this->setInfos(this->streamInfos);
    this->servers->Refresh();
  }
}

void LivecastInfos::onTabMiddleUp(wxAuiNotebookEvent& event)
{
  if (event.GetSelection() > 3)
  {
    this->noteBook->DeletePage(event.GetSelection());
  }
}

void LivecastInfos::onProfilesListDblClicked(wxListEvent& event)
{
  LogError::getInstance().sysLog(DEBUG, "click on profiles list item %d", event.GetIndex());

  StreamInfos::profiles_t::const_iterator it = streamInfos->getProfiles().begin();
  std::advance(it, event.GetIndex());

  LogError::getInstance().sysLog(DEBUG, "id '%s'", (*it)[StreamInfos::PROFILE_ID].c_str());
  LogError::getInstance().sysLog(DEBUG, "width '%s'", (*it)[StreamInfos::PROFILE_WIDTH].c_str());
  LogError::getInstance().sysLog(DEBUG, "height '%s'", (*it)[StreamInfos::PROFILE_HEIGHT].c_str());
  LogError::getInstance().sysLog(DEBUG, "video bitrate '%s'", (*it)[StreamInfos::PROFILE_VIDEOBITRATE].c_str());
  LogError::getInstance().sysLog(DEBUG, "audio bitrate '%s'", (*it)[StreamInfos::PROFILE_AUDIOBITRATE].c_str());
  LogError::getInstance().sysLog(DEBUG, "deinterlace '%s'", (*it)[StreamInfos::PROFILE_DEINTERLACE].c_str());
  LogError::getInstance().sysLog(DEBUG, "framerate '%s'", (*it)[StreamInfos::PROFILE_FRAMERATE].c_str());
  LogError::getInstance().sysLog(DEBUG, "protocols '%s'", (*it)[StreamInfos::PROFILE_PROTOCOLS].c_str());
  LogError::getInstance().sysLog(DEBUG, "primary cdn url '%s'", (*it)[StreamInfos::PROFILE_PRIMARY_CDN_URL].c_str());
  LogError::getInstance().sysLog(DEBUG, "backup cdn url '%s'", (*it)[StreamInfos::PROFILE_BACKUP_CDN_URL].c_str());
  LogError::getInstance().sysLog(DEBUG, "playback url '%s'", (*it)[StreamInfos::PROFILE_PLAYBACK_URL].c_str());
  LogError::getInstance().sysLog(DEBUG, "cdn username '%s'", (*it)[StreamInfos::PROFILE_CDN_USERNAME].c_str());
  LogError::getInstance().sysLog(DEBUG, "cdn password '%s'", (*it)[StreamInfos::PROFILE_CDN_PASSWORD].c_str());
  LogError::getInstance().sysLog(DEBUG, "cdn livename '%s'", (*it)[StreamInfos::PROFILE_CDN_LIVENAME].c_str());
  
  wxPanel * profileDetails = new wxScrolledWindow(this->noteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);

  wxPanel * labelPanel = new wxPanel(profileDetails, wxID_ANY);
  wxPanel * valuePanel = new wxPanel(profileDetails, wxID_ANY);

  wxStaticText * id = new wxStaticText(labelPanel, wxID_ANY, wxT("id:"));
  wxStaticText * width = new wxStaticText(labelPanel, wxID_ANY, wxT("width:"));
  wxStaticText * height = new wxStaticText(labelPanel, wxID_ANY, wxT("height:"));
  wxStaticText * videobitrate = new wxStaticText(labelPanel, wxID_ANY, wxT("videobitrate:"));
  wxStaticText * audiobitrate = new wxStaticText(labelPanel, wxID_ANY, wxT("audiobitrate:"));
  wxStaticText * deinterlace = new wxStaticText(labelPanel, wxID_ANY, wxT("deinterlace:"));
  wxStaticText * framerate = new wxStaticText(labelPanel, wxID_ANY, wxT("framerate:"));
  wxStaticText * protocols = new wxStaticText(labelPanel, wxID_ANY, wxT("protocols:"));
  wxStaticText * primary_cdn_url = new wxStaticText(labelPanel, wxID_ANY, wxT("primary_cdn_url:"));
  wxStaticText * backup_cdn_url = new wxStaticText(labelPanel, wxID_ANY, wxT("backup_cdn_url:"));
  wxStaticText * playback_url = new wxStaticText(labelPanel, wxID_ANY, wxT("playback_url:"));
  wxStaticText * cdn_username = new wxStaticText(labelPanel, wxID_ANY, wxT("cdn_username:"));
  wxStaticText * cdn_password = new wxStaticText(labelPanel, wxID_ANY, wxT("cdn_password:"));
  wxStaticText * cdn_livename = new wxStaticText(labelPanel, wxID_ANY, wxT("cdn_livename:"));

  id->SetFont(this->GetFont().Underlined());
  width->SetFont(this->GetFont().Underlined());
  height->SetFont(this->GetFont().Underlined());
  videobitrate->SetFont(this->GetFont().Underlined());
  audiobitrate->SetFont(this->GetFont().Underlined());
  deinterlace->SetFont(this->GetFont().Underlined());
  framerate->SetFont(this->GetFont().Underlined());
  protocols->SetFont(this->GetFont().Underlined());
  primary_cdn_url->SetFont(this->GetFont().Underlined());
  backup_cdn_url->SetFont(this->GetFont().Underlined());
  playback_url->SetFont(this->GetFont().Underlined());
  cdn_username->SetFont(this->GetFont().Underlined());
  cdn_password->SetFont(this->GetFont().Underlined());
  cdn_livename->SetFont(this->GetFont().Underlined());

  std::ostringstream defaultPlaybackUrl;
  defaultPlaybackUrl << "rtmp://" << (*it)[StreamInfos::PROFILE_ID] << "p.livecast.kewego.com/media/stream=" << this->streamInfos->getId() << "_" << (*it)[StreamInfos::PROFILE_ID];

  wxStaticText * idValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_ID]);
  wxStaticText * widthValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_WIDTH]);
  wxStaticText * heightValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_HEIGHT]);
  wxStaticText * videobitrateValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_VIDEOBITRATE]);
  wxStaticText * audiobitrateValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_AUDIOBITRATE]);
  wxStaticText * deinterlaceValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_DEINTERLACE]);
  wxStaticText * framerateValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_FRAMERATE]);
  wxStaticText * protocolsValue = new wxStaticText(valuePanel, wxID_ANY, (*it)[StreamInfos::PROFILE_PROTOCOLS]);
  wxStaticText * primary_cdn_urlValue = new wxStaticText(valuePanel, wxID_ANY, ((*it)[StreamInfos::PROFILE_PRIMARY_CDN_URL] != "") ? (*it)[StreamInfos::PROFILE_PRIMARY_CDN_URL] : "n/a");
  wxStaticText * backup_cdn_urlValue = new wxStaticText(valuePanel, wxID_ANY, ((*it)[StreamInfos::PROFILE_BACKUP_CDN_URL] != "") ? (*it)[StreamInfos::PROFILE_BACKUP_CDN_URL] : "n/a");
  wxStaticText * playback_urlValue = new wxStaticText(valuePanel, wxID_ANY, (((*it)[StreamInfos::PROFILE_PLAYBACK_URL]) != "") ? (*it)[StreamInfos::PROFILE_PLAYBACK_URL] : defaultPlaybackUrl.str());
  wxStaticText * cdn_usernameValue = new wxStaticText(valuePanel, wxID_ANY, ((*it)[StreamInfos::PROFILE_CDN_USERNAME] != "") ? (*it)[StreamInfos::PROFILE_CDN_USERNAME] : "n/a");
  wxStaticText * cdn_passwordValue = new wxStaticText(valuePanel, wxID_ANY, ((*it)[StreamInfos::PROFILE_CDN_PASSWORD] != "") ? (*it)[StreamInfos::PROFILE_CDN_PASSWORD] : "n/a");
  wxStaticText * cdn_livenameValue = new wxStaticText(valuePanel, wxID_ANY, ((*it)[StreamInfos::PROFILE_CDN_LIVENAME] != "") ? (*it)[StreamInfos::PROFILE_CDN_LIVENAME] : "n/a");

  idValue->SetFont(this->GetFont().Bold());
  widthValue->SetFont(this->GetFont().Bold());
  heightValue->SetFont(this->GetFont().Bold());
  videobitrateValue->SetFont(this->GetFont().Bold());
  audiobitrateValue->SetFont(this->GetFont().Bold());
  deinterlaceValue->SetFont(this->GetFont().Bold());
  framerateValue->SetFont(this->GetFont().Bold());
  protocolsValue->SetFont(this->GetFont().Bold());
  primary_cdn_urlValue->SetFont(this->GetFont().Bold());
  backup_cdn_urlValue->SetFont(this->GetFont().Bold());
  playback_urlValue->SetFont(this->GetFont().Bold());
  cdn_usernameValue->SetFont(this->GetFont().Bold());
  cdn_passwordValue->SetFont(this->GetFont().Bold());
  cdn_livenameValue->SetFont(this->GetFont().Bold());

  wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * sizerLabel = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * sizerValue = new wxBoxSizer(wxVERTICAL);

  sizerLabel->Add(id, 0, wxALL, 2);
  sizerLabel->Add(width, 0, wxALL, 2);
  sizerLabel->Add(height, 0, wxALL, 2);
  sizerLabel->Add(videobitrate, 0, wxALL, 2);
  sizerLabel->Add(audiobitrate, 0, wxALL, 2);
  sizerLabel->Add(deinterlace, 0, wxALL, 2);
  sizerLabel->Add(framerate, 0, wxALL, 2);
  sizerLabel->Add(protocols, 0, wxALL, 2);
  sizerLabel->Add(primary_cdn_url, 0, wxALL, 2);
  sizerLabel->Add(backup_cdn_url, 0, wxALL, 2);
  sizerLabel->Add(playback_url, 0, wxALL, 2);
  sizerLabel->Add(cdn_username, 0, wxALL, 2);
  sizerLabel->Add(cdn_password, 0, wxALL, 2);
  sizerLabel->Add(cdn_livename, 0, wxALL, 2);
  
  sizerValue->Add(idValue, 0, wxALL, 2);
  sizerValue->Add(widthValue, 0, wxALL, 2);
  sizerValue->Add(heightValue, 0, wxALL, 2);
  sizerValue->Add(videobitrateValue, 0, wxALL, 2);
  sizerValue->Add(audiobitrateValue, 0, wxALL, 2);
  sizerValue->Add(deinterlaceValue, 0, wxALL, 2);
  sizerValue->Add(framerateValue, 0, wxALL, 2);
  sizerValue->Add(protocolsValue, 0, wxALL, 2);
  sizerValue->Add(primary_cdn_urlValue, 0, wxALL, 2);
  sizerValue->Add(backup_cdn_urlValue, 0, wxALL, 2);
  sizerValue->Add(playback_urlValue, 0, wxALL, 2);
  sizerValue->Add(cdn_usernameValue, 0, wxALL, 2);
  sizerValue->Add(cdn_passwordValue, 0, wxALL, 2);
  sizerValue->Add(cdn_livenameValue, 0, wxALL, 2);

  labelPanel->SetSizer(sizerLabel);
  valuePanel->SetSizer(sizerValue);

  sizer->Add(labelPanel, 1);
  sizer->Add(valuePanel, 1);

  profileDetails->SetSizer(sizer);

  std::ostringstream title;
  title << "profile " << (*it)[StreamInfos::PROFILE_ID] << " details";
  this->noteBook->InsertPage(this->noteBook->GetPageCount(), profileDetails, title.str(), true);
}
