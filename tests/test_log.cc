#include "lib/Log.hh"

using namespace livecast::lib;

int main()
{

  LogError::getInstance().setMode(STDOUT);
  LogError::getInstance().setIdent("testlog");
  LogError::getInstance().setLockMode(true);
  LogError::getInstance().setColorMode(true);
  LogError::getInstance().setDateMode(true);
  LogError::getInstance().setPidMode(true);
  LogError::getInstance().setLevel(7);

  LogError::getInstance().sysLog(DEBUG, "ERROR");
  LogError::getInstance().sysLog(INFO, "INFO");
  LogError::getInstance().sysLog(NOTICE, "NOTICE");
  LogError::getInstance().sysLog(WARNING, "WARNING");
  LogError::getInstance().sysLog(ERROR, "ERROR");
  LogError::getInstance().sysLog(CRITICAL, "CRITICAL");
  
  return 0;
}
