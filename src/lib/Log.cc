#include "Log.hh"

#include <config.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>

#define BRIGHT 1

// Foreground colors
#define FG_BLACK           30    
#define FG_RED             31    
#define FG_GREEN           32    
#define FG_YELLOW          33    
#define FG_BLUE            34    
#define FG_MAGENTA         35    
#define FG_CYAN            36    
#define FG_WHITE           37    

//    Background colors
#define BG_BLACK           40    
#define BG_RED             41    
#define BG_GREEN           42    
#define BG_YELLOW          43    
#define BG_BLUE            44    
#define BG_MAGENTA         45    
#define BG_CYAN            46    
#define BG_WHITE           47    

LogError * systemLog = 0;

LogError& LogError::getInstance()
{
  if (systemLog == 0)
  {
    systemLog = new LogError();
  }
  return *systemLog;
}

LogError& LogError::getInstance(const char * ident, int mode, bool force)
{
  if (systemLog == 0 || force)
  {
    if (systemLog != 0)
    {
      delete systemLog;
    }
    systemLog = new LogError(ident, mode);
  }
  return *systemLog;
}

LogError::LogError()
  : level(-1), // no level,
    localFile(0),
    logOpt(0),
    mode(STDOUT),
    colorLog(false),
    lockMode(false),
    dateMode(false),
    pidMode(false)
{
	::strcpy(this->ident, "UserLog");
	initMutexes();
}

LogError::LogError(const char *_ident, int _mode) 
  : level(-1),
    localFile(0),
		logOpt(0),
    mode(_mode),
    colorLog(false),
    lockMode(false),
    dateMode(false),
    pidMode(false)
{
	this->initMutexes();
	if (this->mode & SYSLOG) 
  {
		::strncpy(this->ident, _ident, sizeof(this->ident)-1);
		ident[sizeof(this->ident)-1] = '\0';
		::openlog(this->ident, LOG_NDELAY, LOG_USER);
	}
	else if (this->mode & LOCALFILE)
  {
		this->openFile(_ident);
  }
}

LogError::~LogError()
{
	::bzero(this->ident, sizeof(this->ident));
	if (this->mode & SYSLOG)
    ::closelog();
	if (this->mode & LOCALFILE)
		this->closeFile();
	::pthread_rwlock_destroy(&locksyslog);
	::pthread_rwlock_destroy(&lockclose);
}

void LogError::initMutexes(void)
{
	pthread_rwlock_init(&locksyslog, NULL);
	pthread_rwlock_init(&lockclose, NULL);
}

void LogError::sysLog(const char *identity, int facility, const char *format, ...) 
{
	va_list ap;
	time_t currentTime;
	struct tm *localTime;
	char date[128];
	char buf1[LOGBUFFER];
  ::bzero(buf1, LOGBUFFER);

  if (this->level != -1 && (this->level < facility))
  {
    // do not log
    return ;
  }

	va_start(ap, format);
	vsnprintf(buf1, sizeof(buf1), format, ap);
	va_end(ap);
	if (! buf1[0]) 
  {
		this->sysLog(ERROR, "cannot log a NULL string");
		return;
	}

  if (this->dateMode && ((this->mode & STDOUT) || (this->mode & LOCALFILE)))
  {
    currentTime = ::time(NULL);
    localTime = ::localtime(&currentTime);
    ::strftime(date, sizeof(date), "%d/%b/%Y:%T %z", localTime);
  }

  if (this->dateMode)
  {
    char buf2[LOGBUFFER];
    ::bzero(buf2, LOGBUFFER);
    ::snprintf(buf2, sizeof(buf2), "[%s] - %s", date, buf1);
    ::memcpy(buf1, buf2, LOGBUFFER);
  }

  if (this->pidMode)
  {
    char buf2[LOGBUFFER];
    ::bzero(buf2, LOGBUFFER);
    ::snprintf(buf2, LOGBUFFER, "[%d] - %s", ::getpid(), buf1);
    ::memcpy(buf1, buf2, LOGBUFFER);
  }

	if (this->mode & SYSLOG) 
  {
    bool sameIdent = ::strcmp(identity, this->ident) != 0;
		::pthread_rwlock_wrlock(&locksyslog);
		if (!sameIdent)
    {
      ::openlog(identity, LOG_NDELAY, LOG_USER);
    }
		::syslog(facility, "%s", buf1);
    if (!sameIdent)
    {
      ::closelog();
      ::openlog(this->ident, LOG_NDELAY, LOG_USER);
    }
		::pthread_rwlock_unlock(&locksyslog);
	}

	if (this->mode & LOCALFILE) 
  {
		::pthread_rwlock_wrlock(&locksyslog);
		::write(getLocalFile(), buf1, strlen(buf1));
		::pthread_rwlock_unlock(&locksyslog);
	}

	if (this->mode & STDOUT)
  {
    if (this->lockMode) ::pthread_rwlock_wrlock(&locksyslog);
    if (this->colorLog)
    {
      this->printWithColor(buf1, facility);
    }
    else
    {
      ::fprintf(stderr, "%s\n", buf1);
    }
    ::fflush(stderr);
    if (this->lockMode) ::pthread_rwlock_unlock(&locksyslog);
  }

}

void LogError::sysLog(const char *file, const char *function, unsigned int line, int facility, const char *format, ...)
{
	va_list ap;
	char buf1[LOGBUFFER];
 	char buf2[LOGBUFFER];
  ::bzero(buf1, LOGBUFFER);
  ::bzero(buf2, LOGBUFFER);

  if (this->level != -1 && (this->level < facility))
  {
    // do not log
    return ;
  }

	va_start(ap, format);
	vsnprintf(buf1, sizeof(buf1), format, ap);
	va_end(ap);
	if (! buf1[0]) 
  {
		this->sysLog(ERROR, "cannot log a NULL string");
		return;
	}

  ::snprintf(buf2, sizeof(buf2), "%s - %s:%d:%s", file, function, line, buf1);
  this->sysLog(this->ident, facility, "%s", buf2);
}

void LogError::printWithColor(const char * buf, int facility)
{
  switch (facility)
  {
  case LOG_CRIT:
    ::fprintf(stderr, "%c[%d;%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_YELLOW, BG_RED, buf);
    break;
  case LOG_ERR:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_RED, buf);
    break;
  case LOG_WARNING:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_YELLOW, buf);
    break;
  case LOG_NOTICE:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_GREEN, buf);
    break;
  case LOG_INFO:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_CYAN, buf);
    break;
  case LOG_DEBUG:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_MAGENTA, buf);
    break;
  default:
    ::fprintf(stderr, "%c[%d;%dm%s\033[0m\n", 0x1B, BRIGHT, FG_BLACK, buf);
  }
}

void LogError::openFile(const char *name)
{
	setLocalFile(open(name, O_APPEND | O_WRONLY | O_CREAT, 0644));
}

void LogError::closeFile(void)
{	
	::pthread_rwlock_wrlock(&this->lockclose);
	close(getLocalFile());
	::pthread_rwlock_unlock(&this->lockclose);
}

namespace chainsaw
{

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
void hexdump(void * data, unsigned int len)
{
  hexdump(data, len, NULL, 0);
  fflush(stdout);
}
#else
void hexdump(void*, unsigned int)
{
  // TODO
}
#endif

}
