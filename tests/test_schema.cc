#include "gui/StatusSchema.hh"
#include "lib/Log.hh"

#include <wx/wx.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace livecast {
namespace gui {

class TestSchema : public wxApp
{
public:
  TestSchema();
  ~TestSchema();
  virtual bool OnInit();
};

class StatusSchemaLoader
{
public:
  void load1(StatusSchema * const statusSchema);
  void load2(StatusSchema * const statusSchema);
  void load3(StatusSchema * const statusSchema, const boost::shared_ptr<boost::property_tree::ptree> statusInfos);
};

}
}

using namespace livecast::gui;
using namespace livecast::lib;
namespace po = boost::program_options;
namespace pt = boost::property_tree;

IMPLEMENT_APP(TestSchema);

TestSchema::TestSchema()
{
}

TestSchema::~TestSchema()
{
}

bool TestSchema::OnInit()
{
  try
  {
    po::variables_map input_arg;
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "produce this message")
      ("file,f", po::value<std::string>(), "")
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
    
    if (input_arg.count("file"))
    {
      LogError::getInstance().sysLog(DEBUG, "parse file: '%s'", input_arg["file"].as<std::string>().c_str());
      std::ifstream fin(input_arg["file"].as<std::string>().c_str());
      boost::shared_ptr<pt::ptree> schemaInfos(new pt::ptree);
      pt::read_xml(fin, *schemaInfos);
      fin.close();

//       std::cout << std::endl << "========" << std::endl;
//       pt::write_xml(std::cout, *schemaInfos);    
//       std::cout << std::endl << "========" << std::endl;

      wxFrame * frame = new wxFrame(NULL, wxID_ANY, "test schema");
      StatusSchema * statusSchema = new StatusSchema(frame);
      StatusSchemaLoader statusSchemaLoader;
      statusSchemaLoader.load3(statusSchema, schemaInfos);
      wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
      box->Add(statusSchema, 1, wxEXPAND, 10);
      frame->SetSizer(box);
      frame->Show(true);
    }
    else
    {
      wxFrame * frame = new wxFrame(NULL, wxID_ANY, "test schema");
      StatusSchema * statusSchema = new StatusSchema(frame);
      StatusSchemaLoader statusSchemaLoader;
      statusSchemaLoader.load1(statusSchema);
      wxBoxSizer * box = new wxBoxSizer(wxHORIZONTAL);
      box->Add(statusSchema, 1, wxEXPAND, 10);
      frame->SetSizer(box);
      frame->Show(true);

    }

  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return false;
  }
  
  return true;
}

void StatusSchemaLoader::load1(StatusSchema * const statusSchema)
{
  //
  // streamdup
  boost::shared_ptr<StatusSchema::server_t> sd0(new StatusSchema::server_t);
  sd0->id = 0;
  sd0->hostname = "streamdup0.l3.kewego.int";
  sd0->type = StatusSchema::SERVER_STREAMDUP;
  sd0->protocol = "udp multicast 224.10.10.10";
  sd0->port = 5555;
  sd0->status = StatusSchema::STATUS_RUNNING;
  std::ostringstream streamdupStatusDetail;
  streamdupStatusDetail << "1 RUNNING;masterbox0.1.l3.kewego.int"
                        << "1 RUNNING;masterbox1.1.l3.kewego.int"
                        << "1 RUNNING;masterbox2.1.l3.kewego.int"
                        << "1 RUNNING;masterbox3.1.l3.kewego.int" ;
  sd0->statusDetail = streamdupStatusDetail.str();
  statusSchema->addServer(sd0);

  //
  // masterboxes
  boost::shared_ptr<StatusSchema::server_t> mb1(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> mb2(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> mb3(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> mb4(new StatusSchema::server_t);

  mb1->id = 1;
  mb2->id = 2;
  mb3->id = 3;
  mb4->id = 4;

  mb1->hostname = "masterbox0.l3.kewego.int";
  mb2->hostname = "masterbox1.l3.kewego.int";
  mb3->hostname = "masterbox2.l3.kewego.int";
  mb4->hostname = "masterbox3.l3.kewego.int";

  mb1->type = StatusSchema::SERVER_MASTERBOX;
  mb2->type = StatusSchema::SERVER_MASTERBOX;
  mb3->type = StatusSchema::SERVER_MASTERBOX;
  mb4->type = StatusSchema::SERVER_MASTERBOX;

  mb1->protocol = "tcp";
  mb2->protocol = "tcp";
  mb3->protocol = "tcp";
  mb4->protocol = "tcp";

  mb1->port = 2991;
  mb2->port = 2991;
  mb3->port = 2991;
  mb4->port = 2991;

  mb1->status = StatusSchema::STATUS_WAITING;
  mb2->status = StatusSchema::STATUS_INITIALIZING;
  mb3->status = StatusSchema::STATUS_RUNNING;
  mb4->status = StatusSchema::STATUS_ERROR;

  std::ostringstream mb1StatusDetail;
  mb1StatusDetail << "1 WAITING" ;
  mb1->statusDetail = mb1StatusDetail.str();

  std::ostringstream mb2StatusDetail;
  mb2StatusDetail << "1 INITIALIZING;1"
                  << "1 INITIALIZING;2"
                  << "1 INITIALIZING;3"
                  << "1 INITIALIZING;4" ;
  mb2->statusDetail = mb2StatusDetail.str();

  std::ostringstream mb3StatusDetail;
  mb3StatusDetail << "1 RUNNING;1"
                  << "1 RUNNING;2"
                  << "1 RUNNING;3"
                  << "1 RUNNING;4" ;
  mb3->statusDetail = mb3StatusDetail.str();

  std::ostringstream mb4StatusDetail;
  mb4StatusDetail << "1 ERROR;1"
                  << "1 ERROR;2"
                  << "1 ERROR;3"
                  << "1 ERROR;4" ;
  mb4->statusDetail = mb4StatusDetail.str();

  statusSchema->addServer(mb1);
  statusSchema->addServer(mb2);
  statusSchema->addServer(mb3);
  statusSchema->addServer(mb4);
  
  //
  // streamers rtmp
  boost::shared_ptr<StatusSchema::server_t> sr5(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sr6(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sr7(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sr8(new StatusSchema::server_t);

  sr5->id = 5;
  sr6->id = 6;
  sr7->id = 7;
  sr8->id = 8;

  sr5->hostname = "streamer0.l3.kewego.int";
  sr6->hostname = "streamer1.l3.kewego.int";
  sr7->hostname = "streamer2.l3.kewego.int";
  sr8->hostname = "streamer3.l3.kewego.int";

  sr5->type = StatusSchema::SERVER_STREAMER_RTMP;
  sr6->type = StatusSchema::SERVER_STREAMER_RTMP;
  sr7->type = StatusSchema::SERVER_STREAMER_RTMP;
  sr8->type = StatusSchema::SERVER_STREAMER_RTMP;

  sr5->protocol = "tcp";
  sr6->protocol = "tcp";
  sr7->protocol = "tcp";
  sr8->protocol = "tcp";

  sr5->port = 1936;
  sr6->port = 1936;
  sr7->port = 1936;
  sr8->port = 1936;

  sr5->status = StatusSchema::STATUS_WAITING;
  sr6->status = StatusSchema::STATUS_INITIALIZING;
  sr7->status = StatusSchema::STATUS_RUNNING;
  sr8->status = StatusSchema::STATUS_ERROR;

  std::ostringstream sr5StatusDetail;
  sr5StatusDetail << "1 WAITING" ;
  sr5->statusDetail = sr5StatusDetail.str();

  std::ostringstream sr6StatusDetail;
  sr6StatusDetail << "1 INITIALIZING;1"
                  << "1 INITIALIZING;2"
                  << "1 INITIALIZING;3"
                  << "1 INITIALIZING;4" ;
  sr6->statusDetail = sr6StatusDetail.str();

  std::ostringstream sr7StatusDetail;
  sr7StatusDetail << "1 RUNNING;1"
                  << "1 RUNNING;2"
                  << "1 RUNNING;3"
                  << "1 RUNNING;4" ;
  sr7->statusDetail = sr7StatusDetail.str();

  std::ostringstream sr8StatusDetail;
  sr8StatusDetail << "1 ERROR;1"
                  << "1 ERROR;2"
                  << "1 ERROR;3"
                  << "1 ERROR;4" ;
  sr8->statusDetail = sr8StatusDetail.str();

  statusSchema->addServer(sr5);
  statusSchema->addServer(sr6);
  statusSchema->addServer(sr7);
  statusSchema->addServer(sr8);

  //
  // streamers hls
  boost::shared_ptr<StatusSchema::server_t> sh09(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sh10(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sh11(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sh12(new StatusSchema::server_t);

  sh09->id = 9;
  sh10->id = 10;
  sh11->id = 11;
  sh12->id = 12;

  sh09->hostname = "streamer0.l3.kewego.int";
  sh10->hostname = "streamer1.l3.kewego.int";
  sh11->hostname = "streamer2.l3.kewego.int";
  sh12->hostname = "streamer3.l3.kewego.int";

  sh09->type = StatusSchema::SERVER_STREAMER_HLS;
  sh10->type = StatusSchema::SERVER_STREAMER_HLS;
  sh11->type = StatusSchema::SERVER_STREAMER_HLS;
  sh12->type = StatusSchema::SERVER_STREAMER_HLS;

  sh09->protocol = "http";
  sh10->protocol = "http";
  sh11->protocol = "http";
  sh12->protocol = "http";

  sh09->port = 80;
  sh10->port = 80;
  sh11->port = 80;
  sh12->port = 80;

  sh09->status = StatusSchema::STATUS_WAITING;
  sh10->status = StatusSchema::STATUS_INITIALIZING;
  sh11->status = StatusSchema::STATUS_RUNNING;
  sh12->status = StatusSchema::STATUS_ERROR;

  std::ostringstream sh09StatusDetail;
  sh09StatusDetail << "1 WAITING" ;
  sh09->statusDetail = sh09StatusDetail.str();

  std::ostringstream sh10StatusDetail;
  sh10StatusDetail << "1 INITIALIZING;1"
                   << "1 INITIALIZING;2"
                   << "1 INITIALIZING;3"
                   << "1 INITIALIZING;4" ;
  sh10->statusDetail = sh10StatusDetail.str();

  std::ostringstream sh11StatusDetail;
  sh11StatusDetail << "1 RUNNING;1"
                   << "1 RUNNING;2"
                   << "1 RUNNING;3"
                   << "1 RUNNING;4" ;
  sh11->statusDetail = sh11StatusDetail.str();

  std::ostringstream sh12StatusDetail;
  sh12StatusDetail << "1 ERROR;1"
                   << "1 ERROR;2"
                   << "1 ERROR;3"
                   << "1 ERROR;4" ;
  sh12->statusDetail = sh12StatusDetail.str();

  statusSchema->addServer(sh09);
  statusSchema->addServer(sh10);
  statusSchema->addServer(sh11);
  statusSchema->addServer(sh12);

  //
  // link streamdup to masterboxes
  StatusSchema::link_t l1;
  
  l1.src = sd0;

  mb1->srcs.push_back(sd0);
  mb2->srcs.push_back(sd0);
  mb3->srcs.push_back(sd0);
  mb4->srcs.push_back(sd0);

  l1.dsts.push_back(mb1);
  l1.dsts.push_back(mb2);
  l1.dsts.push_back(mb3);
  l1.dsts.push_back(mb4);

  statusSchema->addLink(l1);

  //
  // link masterboxes to rtmp/hls streamers
  for (unsigned int i = 0; i < 4; i++)
  {
    StatusSchema::link_t l1;
    
    boost::shared_ptr<StatusSchema::server_t> src = (i == 0) ? mb1 : (i == 1) ? mb2 : (i == 2) ? mb3 : mb4;
    l1.src = src;

    sr5->srcs.push_back(src);
    sr6->srcs.push_back(src);
    sr7->srcs.push_back(src);
    sr8->srcs.push_back(src);

    l1.dsts.push_back(sr5);
    l1.dsts.push_back(sr6);
    l1.dsts.push_back(sr7);
    l1.dsts.push_back(sr8);

    //
    sh09->srcs.push_back(src);
    sh10->srcs.push_back(src);
    sh11->srcs.push_back(src);
    sh12->srcs.push_back(src);

    l1.dsts.push_back(sh09);
    l1.dsts.push_back(sh10);
    l1.dsts.push_back(sh11);
    l1.dsts.push_back(sh12);
    //

    statusSchema->addLink(l1);
  }

}

void StatusSchemaLoader::load2(StatusSchema * const statusSchema)
{
  //
  // streamdup
  boost::shared_ptr<StatusSchema::server_t> sd0(new StatusSchema::server_t);
  sd0->id = 0;
  sd0->hostname = "streamdup0.l3.kewego.int";
  sd0->type = StatusSchema::SERVER_STREAMDUP;
  sd0->protocol = "udp multicast 224.10.10.10";
  sd0->port = 5555;
  sd0->status = StatusSchema::STATUS_RUNNING;
  statusSchema->addServer(sd0);

  //
  // masterboxes
  boost::shared_ptr<StatusSchema::server_t> mb1(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> mb2(new StatusSchema::server_t);

  mb1->id = 1;
  mb2->id = 2;

  mb1->hostname = "masterbox0.l3.kewego.int";
  mb2->hostname = "masterbox2.l3.kewego.int";

  mb1->type = StatusSchema::SERVER_MASTERBOX;
  mb2->type = StatusSchema::SERVER_MASTERBOX;

  mb1->protocol = "tcp";
  mb2->protocol = "tcp";

  mb1->port = 2991;
  mb2->port = 2991;

  mb1->status = StatusSchema::STATUS_WAITING;
  mb2->status = StatusSchema::STATUS_INITIALIZING;

  statusSchema->addServer(mb1);
  statusSchema->addServer(mb2);
  
  //
  // streamers rtmp
  boost::shared_ptr<StatusSchema::server_t> sr5(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sr6(new StatusSchema::server_t);

  sr5->id = 5;
  sr6->id = 6;

  sr5->hostname = "streamer0.l3.kewego.int";
  sr6->hostname = "streamer1.l3.kewego.int";

  sr5->type = StatusSchema::SERVER_STREAMER_RTMP;
  sr6->type = StatusSchema::SERVER_STREAMER_RTMP;

  sr5->protocol = "tcp";
  sr6->protocol = "tcp";

  sr5->port = 1936;
  sr6->port = 1936;

  sr5->status = StatusSchema::STATUS_WAITING;
  sr6->status = StatusSchema::STATUS_INITIALIZING;

  statusSchema->addServer(sr5);
  statusSchema->addServer(sr6);

  //
  // streamers hls
  boost::shared_ptr<StatusSchema::server_t> sh09(new StatusSchema::server_t);
  boost::shared_ptr<StatusSchema::server_t> sh10(new StatusSchema::server_t);

  sh09->id = 9;
  sh10->id = 10;

  sh09->hostname = "streamer0.l3.kewego.int";
  sh10->hostname = "streamer1.l3.kewego.int";

  sh09->type = StatusSchema::SERVER_STREAMER_HLS;
  sh10->type = StatusSchema::SERVER_STREAMER_HLS;

  sh09->protocol = "http";
  sh10->protocol = "http";

  sh09->port = 80;
  sh10->port = 80;

  sh09->status = StatusSchema::STATUS_WAITING;
  sh10->status = StatusSchema::STATUS_INITIALIZING;

  statusSchema->addServer(sh09);
  statusSchema->addServer(sh10);
  
  //
  // link client to streamdup
  StatusSchema::link_t l0;
  l0.dsts.push_back(sd0);
  statusSchema->addLink(l0);

  //
  // link streamdup to masterboxes
  StatusSchema::link_t l1;
  
  l1.src = sd0;

  mb1->srcs.push_back(sd0);
  mb2->srcs.push_back(sd0);

  l1.dsts.push_back(mb1);
  l1.dsts.push_back(mb2);

  statusSchema->addLink(l1);

  //
  // link masterboxes to rtmp streamers
  for (unsigned int i = 0; i < 2; i++)
  {
    StatusSchema::link_t l1;
    
    boost::shared_ptr<StatusSchema::server_t> src = (i == 0) ? mb1 : mb2 ;
    l1.src = src;

    sr5->srcs.push_back(src);
    sr6->srcs.push_back(src);

    l1.dsts.push_back(sr5);
    l1.dsts.push_back(sr6);

    sh09->srcs.push_back(src);
    sh10->srcs.push_back(src);

    l1.dsts.push_back(sh09);
    l1.dsts.push_back(sh10);

    statusSchema->addLink(l1);
  }

  //
  // link internet to streamer
  StatusSchema::link_t l2;
  StatusSchema::link_t l3;
  StatusSchema::link_t l4;
  StatusSchema::link_t l5;

  l2.dsts.push_back(sr5);
  l3.dsts.push_back(sr6);
  l4.dsts.push_back(sh09);
  l5.dsts.push_back(sh10);
  
  statusSchema->addLink(l2);
  statusSchema->addLink(l3);
  statusSchema->addLink(l4);
  statusSchema->addLink(l5);

}

void StatusSchemaLoader::load3(StatusSchema * const statusSchema, const boost::shared_ptr<boost::property_tree::ptree> statusInfos)
{  
  unsigned int id = 0;
  for (pt::ptree::const_iterator it = statusInfos->get_child("status.primary").begin(); it != statusInfos->get_child("status.primary").end(); ++it)
  {
    const std::string typeStr = it->second.get<std::string>("type");
    const std::string statusStr = it->second.get<std::string>("result");

    StatusSchema::type_t type = ((typeStr == "streamdup") ? StatusSchema::SERVER_STREAMDUP :
                                 (typeStr == "masterbox") ? StatusSchema::SERVER_MASTERBOX :
                                 (typeStr == "rtmp streamer") ? StatusSchema::SERVER_STREAMER_RTMP :
                                 (typeStr == "hls streamer") ? StatusSchema::SERVER_STREAMER_HLS :
                                 StatusSchema::SERVER_UNKNOWN) ;
    StatusSchema::status_t status = ((statusStr.find("WAITING") != std::string::npos) ? StatusSchema::STATUS_WAITING :
                                     (statusStr.find("INITIALIZING") != std::string::npos) ? StatusSchema::STATUS_INITIALIZING :
                                     (statusStr.find("RUNNING") != std::string::npos) ? StatusSchema::STATUS_RUNNING :
                                     (statusStr.find("ERROR") != std::string::npos) ? StatusSchema::STATUS_ERROR :
                                     StatusSchema::STATUS_UNKNOWN) ;

    boost::shared_ptr<StatusSchema::server_t> server(new StatusSchema::server_t);
    server->id = id++;
    server->hostname = it->first;
    server->type = type;
    server->status = status;
    server->protocol = it->second.get<std::string>("protocol");
    server->port = it->second.get<unsigned int>("port");
    server->leaf = it->second.get<bool>("leaf");
   
    LogError::getInstance().sysLog(ERROR, "add server : [id:%d] [host:%s] [type:%d] [status:%d] [protocol:%s] [port:%u] [leaf:%d]",
                                   server->id, server->hostname.c_str(), server->type, server->status, server->protocol.c_str(), server->port, server->leaf);
    statusSchema->addServer(server);
  }
  statusSchema->linkAllServers();
}

