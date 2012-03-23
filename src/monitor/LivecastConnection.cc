#include "../lib/Log.hh"
#include "LivecastConnection.hh"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/algorithm/string/erase.hpp>

LivecastConnection::LivecastConnection(boost::asio::io_service& io_service,
                                       boost::shared_ptr<ResultCallbackIntf> resultCb,
                                       const std::string& host, 
                                       const boost::uint16_t port,
                                       const std::string& user,
                                       const std::string& pass,
                                       const MonitorConfiguration * const conf)
  : socket(io_service),
    resolver(io_service),
    deadline_timer(io_service),
    strand(io_service),
    resultCb(resultCb),
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

void LivecastConnection::check()
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
      
      LogError::getInstance().sysLog(INFO, "check %s:%d", this->host.c_str(), this->port);
      this->socket.async_connect(iterator->endpoint(), this->strand.wrap(boost::bind(&LivecastConnection::handleConnect, shared_from_this(), boost::asio::placeholders::error)));
      this->deadline_timer.expires_from_now(boost::posix_time::milliseconds(this->conf->getConnectionTimeout()));
      this->deadline_timer.async_wait(this->strand.wrap(boost::bind(&LivecastConnection::handleTimeout, shared_from_this(), boost::asio::placeholders::error)));

      // wait for result
      boost::unique_lock<boost::mutex> lock(this->mut);
      while(!this->data_ready)
      {
        this->cond.wait(lock);
      }
      
    }
  }
  catch (const LivecastConnectionException& ex)
  {
    std::ostringstream oss;
    oss << "cannot get status information on " << this->host.c_str() << ":" << this->port ;
    this->parseResult(oss.str().c_str());
    LogError::getInstance().sysLog(ERROR, "%s", oss.str().c_str());
  } 
  catch (const std::exception& ex)
  {
    std::ostringstream oss;
    oss << "cannot get status information on " << this->host.c_str() << ":" << this->port << " => " << ex.what();
    this->parseResult(oss.str().c_str());
    LogError::getInstance().sysLog(ERROR, "%s", oss.str().c_str());
  } 

  // post result
  LogError::getInstance().sysLog(DEBUG, "post result");
  this->resultCb->commit();
}
  
void LivecastConnection::handleConnect(const boost::system::error_code& err)
{
  if (err)
  {
    LogError::getInstance().sysLog(ERROR, "%s", err.message().c_str());
    std::ostringstream oss;
    oss << "cannot connect to host " << this->host << ":" << this->port << std::endl;
    this->resultString = oss.str();
    this->parseResult(oss.str().c_str());
    // post result
    {
      boost::lock_guard<boost::mutex> lock(this->mut);
      this->data_ready = true;
    }
    this->cond.notify_one();
    return;
  }

  try
  {
    this->deadline_timer.cancel();
    LogError::getInstance().sysLog(DEBUG, "connect to %s", this->host.c_str());

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

    // ask for status
    boost::asio::write(socket, boost::asio::buffer("status\r\n"), boost::asio::transfer_all(), error);
    this->checkError(error);

    // get status and print it
    len = socket.read_some(boost::asio::buffer(buf), error);
    this->checkError(error);
    buf[len] = 0;
    LogError::getInstance().sysLog(DEBUG, buf.data());
 
    // todo : parse status 
    this->resultString = buf.data();
    this->parseResult(buf.data());

    // post result
    {
      boost::lock_guard<boost::mutex> lock(this->mut);
      this->data_ready = true;
    }
    this->cond.notify_one();

    this->socket.close();
  }
  catch (const LivecastConnectionException& ex)
  {
    LogError::getInstance().sysLog(ERROR, "cannot get status information on %s:%d", this->host.c_str(), this->port);
  } 

}

void LivecastConnection::handleTimeout(const boost::system::error_code& error)
{
  if (!error)
  {
    LogError::getInstance().sysLog(ERROR, "timeout reached : unabled to reach %s:%d", this->host.c_str(), this->port);
    this->socket.close();
  }
}

void LivecastConnection::checkError(const boost::system::error_code& error) const
{
  if (error != 0)
  {
    throw new LivecastConnectionException;
  }
}

void LivecastConnection::parseResult(const char * result)
{
  boost::property_tree::ptree ptree;
  ptree.put("type", this->conf->getServerFromPort(this->port).c_str());
  std::string result_ss(result);
  boost::algorithm::erase_last(result_ss, "\n");
  ptree.put("result", result_ss);
  this->resultCb->getResultTree().push_back(boost::property_tree::ptree::value_type(this->host, ptree));
}
