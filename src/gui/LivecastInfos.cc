#include "Util.hh"
#include "LivecastInfos.hh"
#include "LivecastListCtrl.hh"
#include "../lib/Log.hh"

using namespace livecast;
using namespace gui;
using namespace monitor;

LivecastInfos::LivecastInfos(wxWindow * parent)
  : wxPanel(parent, wxID_ANY)
{
  
  this->noteBook = new wxNotebook(this, wxID_ANY);

  wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
  box->Add(this->noteBook, 1, wxEXPAND | wxALL, 10);
  this->SetSizer(box);

  // prepare infos
  {
    this->infos = new wxPanel(this->noteBook, wxID_ANY);

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

    this->idValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->modeValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->srcIpValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->dstHostValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->dstPortValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->protocolValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->extKeyValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->backlogValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->nbConnectionsValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->enabledValue = new wxStaticText(this->infos, wxID_ANY, "not set");
    this->disableFilterValue = new wxStaticText(this->infos, wxID_ANY, "not set");

    wxGridSizer * grid = new wxGridSizer(2, 11, 10);

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
  
    this->infos->SetSizer(grid);
    this->noteBook->InsertPage(0, this->infos, "details infos", true);
  }

  {
    this->servers = new LivecastListCtrl(this->noteBook);
    
    unsigned int index = 0;
    this->servers->InsertColumn(index++, "row", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "type", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "hostname", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "ip", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "port", wxLIST_FORMAT_LEFT);
    this->servers->InsertColumn(index++, "status", wxLIST_FORMAT_LEFT);

    this->noteBook->InsertPage(1, this->servers, "servers", false);
  }

}

void LivecastInfos::setInfos(const boost::shared_ptr<livecast::monitor::StreamInfos> streamInfos)
{

  // infos
  this->idValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_ID].c_str());
  this->modeValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_MODE].c_str());
  this->srcIpValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_SRC_IP].c_str());
  this->dstHostValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_DST_HOST].c_str());
  this->dstPortValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_DST_PORT].c_str());
  this->protocolValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_PROTOCOL].c_str());
  this->extKeyValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_EXT_KEY].c_str());
  this->backlogValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_BACKLOG].c_str());
  this->nbConnectionsValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_NB_CONNECTIONS].c_str());
  this->enabledValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_ENABLED].c_str());
  this->disableFilterValue->SetLabel(streamInfos->infos[StreamInfos::FIELDS_DISABLE_FILTER].c_str());

  // servers
  unsigned int n = 0;
  this->servers->DeleteAllItems();
  const boost::array<boost::array<std::list<StreamInfos::server_t>, 3>, 2>& serv = streamInfos->getServers();
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

//         switch ((*it).status)
//         {  
//         case StreamInfos::STATUS_WAITING:      this->servers->SetItemTextColour(n, wxColour(wxColour(*wxLIGHT_GREY))); break;
//         case StreamInfos::STATUS_INITIALIZING: this->servers->SetItemTextColour(n, wxColour(wxColour(*wxBLUE)));       break;
//         case StreamInfos::STATUS_RUNNING:      this->servers->SetItemTextColour(n, wxColour(wxColour(*wxGREEN)));      break;
//         case StreamInfos::STATUS_ERROR:        this->servers->SetItemTextColour(n, wxColour(wxColour(*wxRED)));        break;
//         case StreamInfos::STATUS_UNKNOWN:      this->servers->SetItemTextColour(n, wxColour(wxColour(orange)));        break;
//         }  
        this->servers->SetItemTextColour(n, (*it).status);
        this->servers->SetItemBackgroundColour(n, wxColour(wxColour(((n % 2) == 0) ? lightYellow : lightBlue)));
        n++;
      }
    }
  }
}
