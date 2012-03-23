#ifndef __MONITOR_INTF_HH__
#define __MONITOR_INTF_HH__

#include <boost/shared_ptr.hpp>

class LivecastMonitorIntf;
typedef boost::shared_ptr<LivecastMonitorIntf> LivecastMonitorIntfPtr;

class LivecastMonitorIntf
{
public:
  virtual void check() = 0;
};

#endif
