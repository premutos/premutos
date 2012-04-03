#ifndef __RESULT_CALLBACK_HH__
#define __RESULT_CALLBACK_HH__

#include "../monitor/ResultCallbackIntf.hh"

namespace livecast {
namespace gui {

class LivecastResult;
class LivecastServers;

class ResultCallback : public livecast::monitor::ResultCallbackIntf
{
public:
  ResultCallback(LivecastResult * const streams, LivecastServers * const servers);
  void commitStreamsList();
  void commitServersList();
  void commitCheckStream(unsigned int streamId);
private:
  LivecastResult * const streams;
  LivecastServers * const servers;
};

}
}

#endif
