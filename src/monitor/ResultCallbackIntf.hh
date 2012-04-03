#ifndef __RESULT_CALLBACK_INTF_HH__
#define __RESULT_CALLBACK_INTF_HH__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace livecast {
namespace monitor {

class ResultCallbackIntf
{
public:
  virtual void commitStreamsList() = 0;
  virtual void commitServersList() = 0;
  virtual void commitCheckStream(unsigned int streamId) = 0;
};

}
}

#endif
