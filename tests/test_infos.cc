#include <gui/LivecastInfos.hh>
#include <lib/Log.hh>
#include <wx/wx.h>
#include <iostream>
#include <boost/program_options.hpp>

namespace livecast {
namespace gui {

class TestInfos : public wxApp
{
public:
  TestInfos();
  ~TestInfos();
  virtual bool OnInit();
};

class Infos : public wxPanel
{
public:
  Infos(wxWindow * parent);
};

}
}

using namespace livecast::gui;
namespace po = boost::program_options;

IMPLEMENT_APP(TestInfos);

TestInfos::TestInfos()
{
}

TestInfos::~TestInfos()
{
}

bool TestInfos::OnInit()
{
  try
  {
    po::variables_map input_arg;
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "produce this message")
      ;

    po::positional_options_description p;
    p.add("file", -1);

    po::store(po::command_line_parser(wxApp::argc, wxApp::argv).options(desc).positional(p).run(), input_arg);
    po::notify(input_arg);

    if (input_arg.count("help"))
    {
      std::cout << desc << std::endl;      
      return false;
    }

    LogError::getInstance().setMode(STDOUT);
    LogError::getInstance().setIdent(wxApp::argv[0]);
    LogError::getInstance().setLockMode(true);
    LogError::getInstance().setColorMode(true);
    LogError::getInstance().setDateMode(true);
    LogError::getInstance().setPidMode(true);
    LogError::getInstance().setLevel(6);

    wxFrame * frame = new wxFrame(NULL, wxID_ANY, "test infos");
    // Infos * infos = new Infos(frame);
    LivecastInfos * infos = new LivecastInfos(frame);
    wxBoxSizer * box = new wxBoxSizer(wxVERTICAL);
    box->Add(infos, 1, wxEXPAND | wxALL, 0);
    frame->SetSizer(box);
    frame->Show(true);

  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return false;
  }
  
  return true;
}

Infos::Infos(wxWindow * parent)
  : wxPanel(parent, wxID_ANY)
{
  wxPanel * infos = this;
  wxPanel * labelPanel = new wxPanel(infos, wxID_ANY);
  wxPanel * valuePanel = new wxPanel(infos, wxID_ANY);

  wxStaticText * id = new wxStaticText(labelPanel, wxID_ANY, wxT("Id"));
  wxStaticText * mode = new wxStaticText(labelPanel, wxID_ANY, wxT("Mode"));
  wxStaticText * srcIp = new wxStaticText(labelPanel, wxID_ANY, wxT("Source IP"));
  wxStaticText * dstHost = new wxStaticText(labelPanel, wxID_ANY, wxT("Destination hostname"));
  wxStaticText * dstPort = new wxStaticText(labelPanel, wxID_ANY, wxT("Destination port"));
  wxStaticText * protocol = new wxStaticText(labelPanel, wxID_ANY, wxT("Protocol"));
  wxStaticText * extKey = new wxStaticText(labelPanel, wxID_ANY, wxT("External key"));
  wxStaticText * backlog = new wxStaticText(labelPanel, wxID_ANY, wxT("Backlog"));
  wxStaticText * nbConnections = new wxStaticText(labelPanel, wxID_ANY, wxT("Nb connections"));
  wxStaticText * enabled = new wxStaticText(labelPanel, wxID_ANY, wxT("Enabled"));
  wxStaticText * disableFilter = new wxStaticText(labelPanel, wxID_ANY, wxT("Disable filter"));

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

  wxStaticText * idValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * modeValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * srcIpValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * dstHostValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * dstPortValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * protocolValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * extKeyValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * backlogValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * nbConnectionsValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * enabledValue = new wxStaticText(valuePanel, wxID_ANY, "not set");
  wxStaticText * disableFilterValue = new wxStaticText(valuePanel, wxID_ANY, "not set");

  idValue->SetFont(this->GetFont().Bold());
  modeValue->SetFont(this->GetFont().Bold());
  srcIpValue->SetFont(this->GetFont().Bold());
  dstHostValue->SetFont(this->GetFont().Bold());
  dstPortValue->SetFont(this->GetFont().Bold());
  protocolValue->SetFont(this->GetFont().Bold());
  extKeyValue->SetFont(this->GetFont().Bold());
  backlogValue->SetFont(this->GetFont().Bold());
  nbConnectionsValue->SetFont(this->GetFont().Bold());
  enabledValue->SetFont(this->GetFont().Bold());
  disableFilterValue->SetFont(this->GetFont().Bold());

  wxStaticBox * box = new wxStaticBox(infos, wxID_ANY, "global infos");
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
  
  sizerValue->Add(idValue, 0, wxALL, 10);
  sizerValue->Add(modeValue, 0, wxALL, 10);
  sizerValue->Add(srcIpValue, 0, wxALL, 10);
  sizerValue->Add(dstHostValue, 0, wxALL, 10);
  sizerValue->Add(dstPortValue, 0, wxALL, 10);
  sizerValue->Add(protocolValue, 0, wxALL, 10);
  sizerValue->Add(extKeyValue, 0, wxALL, 10);
  sizerValue->Add(nbConnectionsValue, 0, wxALL, 10);
  sizerValue->Add(backlogValue, 0, wxALL, 10);
  sizerValue->Add(enabledValue, 0, wxALL, 10);
  sizerValue->Add(disableFilterValue, 0, wxALL, 10);

  labelPanel->SetSizer(sizerLabel);
  valuePanel->SetSizer(sizerValue);

  sizer->Add(labelPanel, 0);
  sizer->Add(valuePanel, 0);

  infos->SetSizer(sizer);
}
