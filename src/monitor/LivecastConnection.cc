#include "../lib/Log.hh"
#include "LivecastConnection.hh"
#include "MonitorConfiguration.hh"
#include "ResultCallbackIntf.hh"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/algorithm/string/erase.hpp>

using namespace livecast;
using namespace livecast::monitor;
using namespace livecast::lib;

LivecastConnection::LivecastConnection(boost::asio::io_service& io_service,
                                       const std::string& host, 
                                       const boost::uint16_t port,
                                       const std::string& user,
                                       const std::string& pass,
                                       const MonitorConfiguration * const conf)
  : socket(io_service),
    resolver(io_service),
    deadline_timer(io_service),
    strand(io_service),
    streamId(0),
    data_ready(false),
    host(host),
    port(port),
    user(user),
    pass(pass),
    conf(conf)
{
}

LivecastConnection::~LivecastConnection()
{
}

void LivecastConnection::status(unsigned int streamId, boost::shared_ptr<boost::property_tree::ptree> result, bool details)
{
  boost::unique_lock<boost::mutex> lockCheck(this->checkMut);
  boost::unique_lock<boost::mutex> lockSync(this->syncMut);
  this->streamId = streamId;
  this->result = result;
  if (details)
  {
    this->op = STATUS_DETAIL;
  }
  else
  {
    this->op = STATUS;
  }

  this->perform();

  // wait for result
  while(!this->data_ready)
  {
    this->cond.wait(lockSync);
  }
}

void LivecastConnection::reinit(unsigned int streamId)
{
  boost::unique_lock<boost::mutex> lockCheck(this->checkMut);
  boost::unique_lock<boost::mutex> lockSync(this->syncMut);
  this->streamId = streamId;
  this->op = REINIT;

  this->perform();

  // wait for result
  while(!this->data_ready)
  {
    this->cond.wait(lockSync);
  }
}

void LivecastConnection::perform()
{
  try
  {
    std::ostringstream os;
    os << this->port; // ugly
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), this->host, os.str());
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    if (iterator != boost::asio::ip::tcp::resolver::iterator())
    {
      this->data_ready = false;      
      LogError::getInstance().sysLog(DEBUG, "start operation on stream id %d on %s:%d", streamId, this->host.c_str(), this->port);
      this->socket.async_connect(iterator->endpoint(), this->strand.wrap(boost::bind(&LivecastConnection::handleConnect, shared_from_this(), boost::asio::placeholders::error)));
      this->deadline_timer.expires_from_now(boost::posix_time::milliseconds(this->conf->getConnectionTimeout()));
      this->deadline_timer.async_wait(this->strand.wrap(boost::bind(&LivecastConnection::handleTimeout, shared_from_this(), boost::asio::placeholders::error)));
    }
  }
  catch (const LivecastConnectionException& ex)
  {
    std::ostringstream oss;
    oss << "cannot get status information on " << this->host.c_str() << ":" << this->port ;
    this->parseResult(oss.str().c_str());
    LogError::getInstance().sysLog(WARNING, "%s", oss.str().c_str());
  } 
  catch (const std::exception& ex)
  {
    std::ostringstream oss;
    oss << "cannot get status information on " << this->host.c_str() << ":" << this->port << " => " << ex.what();
    this->parseResult(oss.str().c_str());
    LogError::getInstance().sysLog(WARNING, "%s", oss.str().c_str());
  } 
}
  
void LivecastConnection::handleConnect(const boost::system::error_code& err)
{
  if (err)
  {
    LogError::getInstance().sysLog(WARNING, "%s on %s:%d (streamId %d)", err.message().c_str(), this->host.c_str(), this->port, this->streamId);
    std::ostringstream oss;
    oss << "cannot connect to host " << this->host << ":" << this->port << std::endl;
    this->resultString = oss.str();
    this->parseResult(oss.str().c_str());
    // post result
    {
      boost::lock_guard<boost::mutex> lock(this->syncMut);
      this->data_ready = true;
    }
    this->cond.notify_one();
    return;
  }

  try
  {
    this->deadline_timer.cancel();
    LogError::getInstance().sysLog(DEBUG, "connect to %s:%d", this->host.c_str(), this->port);

    boost::system::error_code error;

    // waiting for server presentation
    boost::array<char, 2048> buf;  
    size_t len;
    len = socket.read_some(boost::asio::buffer(buf), error);
    this->checkError(error);
    buf[len] = 0;
    LogError::getInstance().sysLog(DEBUG, buf.data());
    
    // authentification
    boost::asio::write(socket, boost::asio::buffer("auth admin;a02sg32\r\n"), boost::asio::transfer_all(), error);
    this->checkError(error);

    // check auth successfull 
    len = socket.read_some(boost::asio::buffer(buf), error);
    this->checkError(error);
    buf[len] = 0;
    LogError::getInstance().sysLog(DEBUG, buf.data());

    // perform operation    
    std::ostringstream oss;
    switch (this->op)
    {
    case STATUS:
      oss << "status " << this->streamId << "\r\n";
      break;
    case STATUS_DETAIL:
      oss << "status_detail " << this->streamId << "\r\n";
      break;
    case REINIT:
      oss << "reinit " << this->streamId << "\r\n";
    }
    boost::asio::write(socket, boost::asio::buffer(oss.str()), boost::asio::transfer_all(), error);
    this->checkError(error);

    // get response
    len = socket.read_some(boost::asio::buffer(buf), error);
    this->checkError(error);
    buf[len] = 0;
    LogError::getInstance().sysLog(DEBUG, buf.data());
 
    // todo : parse response
    this->resultString = buf.data();
    this->parseResult(buf.data());

    this->socket.close();

    // post result
    {
      boost::lock_guard<boost::mutex> lock(this->syncMut);
      this->data_ready = true;
    }
    this->cond.notify_one();

  }
  catch (const LivecastConnectionException& ex)
  {
    LogError::getInstance().sysLog(WARNING, "cannot get status information for streamId %d on %s:%d", this->streamId, this->host.c_str(), this->port);
  } 
}

void LivecastConnection::handleTimeout(const boost::system::error_code& error)
{
  if (!error)
  {
    LogError::getInstance().sysLog(WARNING, "timeout reached : unabled to reach %s:%d for streamId %d", this->host.c_str(), this->port, this->streamId);
    this->socket.close();
  }
}

void LivecastConnection::checkError(const boost::system::error_code& error) const
{
  if (error != 0)
  {
    throw LivecastConnectionException();
  }
}

void LivecastConnection::parseResult(const char * result_str)
{
  boost::property_tree::ptree ptree;
  ptree.put("type", this->conf->getServerFromPort(this->port).c_str());
  std::string result_ss(result_str);
  boost::algorithm::erase_last(result_ss, "\n");
  std::string::size_type pos = result_ss.find_last_of("\n");
  if (pos != std::string::npos)
  {
    result_ss = result_ss.substr(0, pos);
  }
  ptree.put("result", result_ss);
  this->result->push_back(boost::property_tree::ptree::value_type(this->host, ptree));
}
