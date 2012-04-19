#ifndef LOG_H
#define LOG_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <pthread.h>

#include <boost/cstdint.hpp>

#define LOG_FILE  -1

#define SYSLOG		0x01
#define LOCALFILE	0x02
#define STDOUT		0x04

#define LOGBUFFER 8192

#define CRITICAL __FILE__, __FUNCTION__, __LINE__, LOG_CRIT
#define ERROR __FILE__, __FUNCTION__, __LINE__, LOG_ERR
#define WARNING __FILE__, __FUNCTION__, __LINE__, LOG_WARNING
#define NOTICE __FILE__, __FUNCTION__, __LINE__, LOG_NOTICE
#define INFO __FILE__, __FUNCTION__, __LINE__, LOG_INFO
#define DEBUG __FILE__, __FUNCTION__, __LINE__, LOG_DEBUG

namespace livecast {
namespace lib {

class LogError
{
public:

  static LogError& getInstance();
  static LogError& getInstance(const char * ident, int mode, bool force);

  virtual ~LogError();

  void setLockMode(bool value)      { this->lockMode = value; }
  void setColorMode(bool value)     { this->colorLog = value; }
  void setDateMode(bool value)      { this->dateMode = value; }
  void setPidMode(bool value)       { this->pidMode = value; }
  void setLocalFile(int _localFile) { this->localFile = _localFile; }
  void setMode(char _mode)          { this->mode = _mode; }
  void setLevel(int _level)         { this->level = _level; }
  void setIdent(const char *_ident) { 
    ::bzero(this->ident, sizeof(ident)); 
    ::strncpy(this->ident, _ident, sizeof(this->ident)); 
  }

  int          getLocalFile(void)    { return this->localFile; }
  char         getMode(void)         { return this->mode; }
  char *       getIdent(void)        { return this->ident; }
  
  void sysLog(const char * identity, int facility, const char * forma, ...);
  void sysLog(const char * file, const char * function, unsigned int line, int facility, const char * format, ...);

private:  
  LogError(void);
  LogError(const char * ident, int mode);

  void printWithColor(const char * buf, int facility);
  void openFile(const char *);
  void closeFile(void);
  void initMutexes(void);

private:
  int level;
  int localFile;
  char mode;
  char ident[64];
  pthread_rwlock_t locksyslog;
  pthread_rwlock_t lockclose;
  bool colorLog;
  bool lockMode;
  bool dateMode;
  bool pidMode;
};

// extern LogError * systemLog;

}
}

#endif
