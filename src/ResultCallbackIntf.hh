#ifndef __RESULT_CALLBACK_INTF_HH__
#define __RESULT_CALLBACK_INTF_HH__

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

class ResultCallbackIntf
{
public:
  virtual void clear() = 0;
  virtual void commit() = 0;
  virtual boost::property_tree::ptree& getResultTree() = 0;
};

#endif
