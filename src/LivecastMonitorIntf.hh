#ifndef __MONITOR_INTF_HH__
#define __MONITOR_INTF_HH__

#include "ResultCallbackIntf.hh"
#include <boost/shared_ptr.hpp>

class LivecastMonitorIntf;
typedef boost::shared_ptr<LivecastMonitorIntf> LivecastMonitorIntfPtr;

class LivecastMonitorIntf
{
public:
  virtual void refresh(boost::shared_ptr<ResultCallbackIntf> resultCb) = 0;
  virtual void check(unsigned int streamId, boost::shared_ptr<ResultCallbackIntf> resultCb) = 0;
};

#endif
