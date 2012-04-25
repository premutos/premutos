#include "../lib/Log.hh"
#include "StreamInfos.hh"
#include "LivecastConnection.hh"
#include "MonitorConfiguration.hh"
#include "ResultCallbackIntf.hh"

#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace livecast {
namespace monitor {

bool operator==(const livecast::monitor::StreamInfos::server_t& s1, const livecast::monitor::StreamInfos::server_t& s2)
{
  return (s1.host == s2.host) && (s1.port == s2.port);
}

}
}

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::lib;

StreamInfos::status_t getGlobalStatus(const std::list<std::string>& serversStatus);

//
//

StreamInfos::StreamInfos(unsigned int streamId)
  : streamId(streamId),
    globalStatus(STATUS_UNKNOWN),
    modified(false)
{
}

StreamInfos::~StreamInfos()
{
}

void StreamInfos::status(boost::shared_ptr<ResultCallbackIntf> resultCb,
                         const boost::shared_ptr<MonitorConfiguration> cfg)
{
  LogError::getInstance().sysLog(DEBUG, "start status %u", this->streamId);
  this->modified = false;
  boost::property_tree::ptree resultTmp = this->resultTree;
  this->resultTree.clear();
  boost::property_tree::ptree ptree;
  for (unsigned int i = 0; i < this->servers.size(); i++)
  {

    boost::shared_ptr<boost::property_tree::ptree> statusHost(new boost::property_tree::ptree);
    for (unsigned int j = 0; j < this->servers[i].size(); j++)
    {
      for (std::list<server_t>::iterator itServer = this->servers[i][j].begin(); itServer != this->servers[i][j].end(); ++itServer)
      {
        boost::shared_ptr<LivecastConnection> conn = cfg->getConnection((*itServer).host, (*itServer).adminPort);
        conn->status(this->streamId, statusHost);

        for (boost::property_tree::ptree::iterator itResult = statusHost->get_child("").begin();
             itResult != statusHost->get_child("").end(); ++itResult)
        {

          if (itResult->first != (*itServer).host) continue;
          
          const std::string type = itResult->second.get<std::string>("type");
          if ((type == "streamdup") && ((*itServer).adminPort != 1111)) continue;
          if ((type == "masterbox") && ((*itServer).adminPort != 2222)) continue;
          if ((type == "rtmp streamer") && ((*itServer).adminPort != 3333)) continue;
          if ((type == "hls streamer") && ((*itServer).adminPort != 4444)) continue;

          const std::string status = itResult->second.get<std::string>("result");
          LogError::getInstance().sysLog(DEBUG, "find result: status => '%s'", status.c_str());
          if (status.find("WAITING") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_WAITING;
          }
          else if (status.find("INITIALIZING") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_INITIALIZING;
          }
          else if (status.find("RUNNING") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_RUNNING;
          }
          else if (status.find("RUNNING;ENABLE") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_RUNNING;
          }
          else if (status.find("RUNNING;DISABLE") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_RUNNING;
          }
          else if  (status.find("ERROR") != std::string::npos)
          {
            (*itServer).status = StreamInfos::STATUS_ERROR;
          }

          itResult->second.put("protocol", (*itServer).protocol);
          itResult->second.put("port", (*itServer).port);
          itResult->second.put("leaf", (*itServer).leaf ? "true" : "false");

        }

        // status detail
        (*itServer).statusDetails.clear();
        boost::shared_ptr<boost::property_tree::ptree> statusDetails(new boost::property_tree::ptree);
        conn->status(this->streamId, statusDetails, true);
        for (boost::property_tree::ptree::iterator itResult = statusDetails->get_child("").begin();
             itResult != statusDetails->get_child("").end(); ++itResult)
        {
          const std::string status = itResult->second.get<std::string>("result");
          LogError::getInstance().sysLog(DEBUG, "result => \n%s", status.c_str());

          typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
          boost::char_separator<char> sep("\n");
          tokenizer tok(status, sep);
          for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
          {
            std::string streamId = (*it);
            std::string statusStr = (*it);
            std::string messageStr = (*it);
            const std::string::size_type pos = (*it).find(" ");
            if (pos != std::string::npos)
            {
              streamId = (*it).substr(0, pos);
              statusStr = (*it).substr(pos + 1);
              messageStr = (*it).substr(pos + 1);
            }
      
            unsigned int id = 0;
            try 
            {
              id = boost::lexical_cast<unsigned int>(streamId.c_str());
            }
            catch (const boost::bad_lexical_cast& ex)
            {
              continue;
            }
            
            (*itServer).statusDetails.push_back(boost::make_tuple(StreamInfos::parseStatus(*it), statusStr, messageStr));
          }
        }

      }
    }

//     std::cout << std::endl
//               << "============================="
//               << std::endl;
//     boost::property_tree::write_xml(std::cout, *statusHost);
//     std::cout << std::endl
//               << "============================="
//               << std::endl;

    const std::string wing = (i == 0) ? "primary" : "backup" ;
    ptree.put_child(wing.c_str(), *statusHost);
  }

  this->resultTree.push_back(boost::property_tree::ptree::value_type("status", ptree)); 

  LogError::getInstance().sysLog(DEBUG, "end status %u", this->streamId);

  this->parseStatus();

  this->modified = (resultTree != resultTmp);

  LogError::getInstance().sysLog(DEBUG, "post result");
  resultCb->commitCheckStream(this->streamId);
}

void StreamInfos::reinit(const boost::shared_ptr<MonitorConfiguration> cfg)
{
  LogError::getInstance().sysLog(DEBUG, "reinit %u", this->streamId);
  
  // reinit all primary streamdup
  for (unsigned int i = 0; i < this->servers.size(); i++)
  {
    for (std::list<server_t>::iterator it = this->servers[i][0].begin(); it != this->servers[i][0].end(); ++it)
    {      
      boost::shared_ptr<LivecastConnection> conn = cfg->getConnection((*it).host, (*it).adminPort);
      LogError::getInstance().sysLog(DEBUG, "reinit stream %u on %s:%u", this->streamId, (*it).host.c_str(), (*it).adminPort);
      conn->reinit(this->streamId);
    }
  }

}

void StreamInfos::loadInfos(const boost::shared_ptr<MonitorConfiguration> cfg)
{
  this->profiles.clear();
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
  this->globalStatus = wingsStatus[0].second; // primary
    
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

StreamInfos::status_t StreamInfos::parseStatus(const std::string& value)
{
  StreamInfos::status_t statusTmp = StreamInfos::STATUS_UNKNOWN;
  if (value.find("WAITING") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_WAITING;
  }
  else if (value.find("INITIALIZING") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_INITIALIZING;
  }
  else if (value.find("RUNNING") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_RUNNING;
  }
  else if (value.find("RUNNING;ENABLE") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_RUNNING;
  }
  else if (value.find("RUNNING;DISABLE") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_RUNNING;
  }
  else if  (value.find("ERROR") != std::string::npos)
  {
    statusTmp = StreamInfos::STATUS_ERROR;
  }
  return statusTmp;
}

//
// c functions

StreamInfos::status_t getGlobalStatus(const std::list<std::string>& serversStatus)
{
  StreamInfos::status_t status = StreamInfos::STATUS_UNKNOWN;

  for (std::list<std::string>::const_iterator it = serversStatus.begin(); it != serversStatus.end(); ++it)
  {
    StreamInfos::status_t statusTmp = StreamInfos::parseStatus(*it);

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
