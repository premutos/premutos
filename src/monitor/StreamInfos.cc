#include "../lib/Log.hh"
#include "StreamInfos.hh"
#include "LivecastConnection.hh"
#include "MonitorConfiguration.hh"

#include <iostream>
#include <boost/property_tree/xml_parser.hpp>

bool operator==(const StreamInfos::server_t& s1, const StreamInfos::server_t& s2)
{
  return (s1.host == s2.host) && (s1.port == s2.port);
}

StreamInfos::status_t getGlobalStatus(const std::list<std::string>& serversStatus);

//
//

StreamInfos::StreamInfos(unsigned int streamId)
  : streamId(streamId),
    status(STATUS_UNKNOWN)
{
}

StreamInfos::~StreamInfos()
{
}

void StreamInfos::check(boost::shared_ptr<ResultCallbackIntf> resultCb,
                        const boost::shared_ptr<MonitorConfiguration> cfg)
{
  LogError::getInstance().sysLog(DEBUG, "start checking %u", this->streamId);
  this->resultTree.clear();
  boost::property_tree::ptree ptree;
  for (int i = 0; i < 2; i++)
  {
    boost::shared_ptr<boost::property_tree::ptree> statusHost(new boost::property_tree::ptree);
    for (int j = 0; j < 3; j++)
    {
      for (std::list<server_t>::iterator it = this->servers[i][j].begin(); it != this->servers[i][j].end(); ++it)
      {
        boost::shared_ptr<LivecastConnection> conn = cfg->getConnection((*it).host, (*it).port);
        conn->check(this->streamId, statusHost);
      }
    }
    const std::string wing = (i == 0) ? "primary" : "backup" ;
    ptree.put_child(wing.c_str(), *statusHost);
  }

  this->resultTree.push_back(boost::property_tree::ptree::value_type("status", ptree)); 

  LogError::getInstance().sysLog(DEBUG, "end checking %u", this->streamId);

  this->parseStatus();

  LogError::getInstance().sysLog(DEBUG, "post result");
  resultCb->commitCheckStream(this->streamId);
}

void StreamInfos::loadInfos(const boost::shared_ptr<MonitorConfiguration> cfg)
{
  cfg->load(*this);
}

void StreamInfos::addServer(const server_t& server, bool primary)
{
  int i = primary ? 0 : 1 ;
  int j = ((server.type == server_t::STREAM_DUP) ? 0 :
           (server.type == server_t::MASTER_BOX) ? 1 :
           (server.type == server_t::STREAMER_RTMP) ? 2 :
           (server.type == server_t::STREAMER_HLS) ? 2 :
           -1);
  if (j == -1)
  {
    LogError::getInstance().sysLog(ERROR, "bad server type for stream id %d", this->streamId);
    return;
  }
  else
  {
    if (std::find_if(this->servers[i][j].begin(), this->servers[i][j].end(), 
                     boost::bind(std::equal_to<server_t>(), _1, server)) == this->servers[i][j].end())
    {
      LogError::getInstance().sysLog(DEBUG, "stream id %u insert server %s:%d on wing %d", this->streamId, server.host.c_str(), server.port, i);
      this->servers[i][j].push_back(server);
    }
    else
    {
      LogError::getInstance().sysLog(DEBUG, "stream id %u already have server %s:%d on wing %d", this->streamId, server.host.c_str(), server.port, i);
    }
  }
}

void StreamInfos::parseStatus()
{
//   std::cout << std::endl
//             << "============================="
//             << std::endl;
//   boost::property_tree::write_xml(std::cout, this->resultTree);
//   std::cout << std::endl
//             << "============================="
//             << std::endl;

  boost::array<std::pair<std::string, status_t>, 2> wingsStatus = { { std::make_pair("primary", STATUS_UNKNOWN), std::make_pair("backup", STATUS_UNKNOWN) } };

  for (boost::array<std::pair<std::string, status_t>, 2>::iterator itWing = wingsStatus.begin(); itWing != wingsStatus.end(); ++itWing)
  {

    std::list<std::string> streamdupStatus;
    std::list<std::string> masterboxStatus;
    std::list<std::string> streamerStatus;

    // load primary server into list
    for (boost::property_tree::ptree::const_iterator itServer = this->resultTree.get_child("status." + (*itWing).first).begin(); 
         itServer != this->resultTree.get_child("status." + (*itWing).first).end(); ++itServer)
    {
      const std::string server = itServer->first;
      const std::string type = itServer->second.get<std::string>("type");
      std::string status = itServer->second.get<std::string>("result");
      std::string::size_type pos = status.find(" ");
      if (pos != std::string::npos)
      {
        status = status.substr(pos+1);
      }
      LogError::getInstance().sysLog(DEBUG, "status of %s %s is %s", type.c_str(), server.c_str(), status.c_str());
      if (type == "streamdup")
      {
        streamdupStatus.push_back(status);
      }
      else if (type == "masterbox")
      {
        masterboxStatus.push_back(status);
      }
      else if ((type == "rtmp streamer") || ("hls streamer"))
      {
        streamerStatus.push_back(status);
      }
    }
    (*itWing).second = getGlobalStatus(streamerStatus);
    if ((*itWing).second == STATUS_UNKNOWN)
    {
      (*itWing).second = getGlobalStatus(masterboxStatus);
    }
  }    
  
  // merge in a global status
  this->status = wingsStatus[0].second; // primary
    
//   switch (this->status)
//   {  
//   case STATUS_WAITING:      
//     LogError::getInstance().sysLog(ERROR, "parse status STATUS_WAITING from id %d", this->streamId);
//     break;
//   case STATUS_INITIALIZING: 
//     LogError::getInstance().sysLog(ERROR, "parse status STATUS_INITIALIZING from id %d", this->streamId);
//     break;
//   case STATUS_RUNNING:      
//     LogError::getInstance().sysLog(ERROR, "parse status STATUS_RUNNING from id %d", this->streamId);
//     break;
//   case STATUS_ERROR:        
//     LogError::getInstance().sysLog(ERROR, "parse status STATUS_ERROR from id %d", this->streamId);
//     break;
//   case STATUS_UNKNOWN:      
//     LogError::getInstance().sysLog(ERROR, "parse status STATUS_UNKNOWN from id %d", this->streamId);
//     break;
//   default:
//     LogError::getInstance().sysLog(CRITICAL, "bad status %d from id %d", this->status, this->streamId);
//     assert(false);
//   }  

}

//
// c functions

StreamInfos::status_t getGlobalStatus(const std::list<std::string>& serversStatus)
{
  StreamInfos::status_t status = StreamInfos::STATUS_UNKNOWN;

  for (std::list<std::string>::const_iterator it = serversStatus.begin(); it != serversStatus.end(); ++it)
  {
    StreamInfos::status_t statusTmp = StreamInfos::STATUS_UNKNOWN;
    if ((*it).find("WAITING") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_WAITING;
    }
    else if ((*it).find("INITIALIZING") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_INITIALIZING;
    }
    else if ((*it).find("RUNNING") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_RUNNING;
    }
    else if ((*it).find("RUNNING;ENABLE") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_RUNNING;
    }
    else if ((*it).find("RUNNING;DISABLE") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_RUNNING;
    }
    else if  ((*it).find("ERROR") != std::string::npos)
    {
      statusTmp = StreamInfos::STATUS_ERROR;
    }

    if (status == StreamInfos::STATUS_UNKNOWN)
    {
      status = statusTmp;
    }
    else if (status != statusTmp)
    {
      status = StreamInfos::STATUS_ERROR;
    }
  }

  return status;
}
