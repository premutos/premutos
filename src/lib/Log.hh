/***************************************************************************
                            log.h  -  description
                             -------------------
    begin                : Jeu nov 7 2002
    copyright            : (C) 2002 by Sebastien Petit
    email                : spe@selectbourse.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Includes C
 */
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

class LogError
{
public:

  static LogError& getInstance();
  static LogError& getInstance(const char * ident, int mode, bool force);

  ~LogError();

  void openFile(const char *);
  void setOutput(char);
  char getOutput(void);
  void setIdentity(const char *);

  void setLockMode(bool value)      { this->lockMode = value; }
  void setColorMode(bool value)     { this->colorLog = value; }
  void setDateMode(bool value)      { this->dateMode = value; }
  void setPidMode(bool value)       { this->pidMode = value; }
  void setLocalFile(int _localFile) { this->localFile = _localFile; }
  void setMode(char _mode)          { this->mode = _mode; }
  void setLevel(int _level)         { this->level = _level; }
  void setLogOpt(int _logOpt)       { this->logOpt = _logOpt; }
  void setIdent(const char *_ident) { 
    ::bzero(this->ident, sizeof(ident)); 
    ::strncpy(this->ident, _ident, sizeof(this->ident)); 
  }
  void setStatsIdent(const char * _statsIdent)  { 
    ::bzero(this->statsIdent, sizeof(this->statsIdent)); 
    ::strncpy(this->statsIdent, _statsIdent, sizeof(this->statsIdent)); 
  }
  
  void setHttpIdent(const char * _httpIdent)  { 
    ::bzero(this->httpIdent, sizeof(this->httpIdent)); 
    ::strncpy(this->httpIdent, _httpIdent, sizeof(this->httpIdent)); 
  }

  int          getLocalFile(void)    { return this->localFile; }
  char         getMode(void)         { return this->mode; }
  int          getLogOpt(void)       { return this->logOpt; }
  char *       getIdent(void)        { return this->ident; }
  const char * getStatsIdent() const { return this->statsIdent; }
  const char * getHttpIdent() const  { return this->httpIdent; }
  
  void sysLog(const char * identity, int facility, const char * forma, ...);
  void sysLog(const char * file, const char * function, unsigned int line, int facility, const char * format, ...);

private:  
  LogError(void);
  LogError(const char * ident, int mode);

  void printWithColor(const char * buf, int facility);
  void closeFile(void);
  void initMutexes(void);

private:
  int level;
  int localFile;
  int logOpt;
  char mode;
  char ident[64];
  char statsIdent[64];
  char httpIdent[64];
  pthread_rwlock_t locksyslog;
  pthread_rwlock_t lockclose;
  bool colorLog;
  bool lockMode;
  bool dateMode;
  bool pidMode;
};

extern LogError *systemLog;

namespace chainsaw
{
void hexdump(void * data, unsigned int len);
}

#define CRITICAL __FILE__, __FUNCTION__, __LINE__, LOG_CRIT
#define ERROR __FILE__, __FUNCTION__, __LINE__, LOG_ERR
#define WARNING __FILE__, __FUNCTION__, __LINE__, LOG_WARNING
#define NOTICE __FILE__, __FUNCTION__, __LINE__, LOG_NOTICE
#define INFO __FILE__, __FUNCTION__, __LINE__, LOG_INFO
#define DEBUG __FILE__, __FUNCTION__, __LINE__, LOG_DEBUG

#endif
