
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "defines.h"
#include "log.h"

static log_printer_t s_defaultLogPrinter = {LOG_PRIORITY_DEBUG, "default"};
log_printer_t * pstDefaultLogPrinter = &s_defaultLogPrinter;

static bool s_bEnableTimePrint = true;
static bool s_bEnableDatePrint = false;
static bool s_bEnableSourcePrint = false;
static bool s_bEnableProcessPrint = false;
static bool s_bEnableMethodPrint = true;

static uint s_u32SysLogMaxLevel = LOG_PRIORITY_WARN;
static uint s_u32StdOutLogMaxLevel = LOG_PRIORITY_WARN;
static uint s_u32CliLogMaxLevel = LOG_PRIORITY_WARN;

static uint s_bLogSysLogReady = false;

static char acProgramName[32];
static log_printer_t * s_acLogPrinters[64];

static void (*s_cli_log_cb_handler) (const char * msg, uint len);

void log_register_writer(log_printer_t * handle)
{
  ASSERT(handle);
  ASSERT(handle->name);

  for(uint i = 0; i < SIZEOF_ARRAY(s_acLogPrinters); i++) {
    if(!s_acLogPrinters[i]) {
      s_acLogPrinters[i] = handle;
      return ;
    }
  }

  FATAL_PRINTF("s_acLogPrinters size must be increased");
}

void log_set_cli_writer_callback(void (*cb) (const char * msg, uint len))
{
   s_cli_log_cb_handler = cb;
}

bool log_set_printer_max_prio(const char * name, log_priority_t prio)
{
  ASSERT(name);
  ASSERT(*name);

  for(uint i = 0; i < SIZEOF_ARRAY(s_acLogPrinters); i++) {
    if(s_acLogPrinters[i] && s_acLogPrinters[i]->name && !strcmp(name, s_acLogPrinters[i]->name)) {
      s_acLogPrinters[i]->maxLogLevel = prio;
      return true;
    }
  }

  return false;
}

log_printer_t * const * log_get_writer_list(uint * plen)
{
  uint i = 0;

  ASSERT(plen);

  for(; i < SIZEOF_ARRAY(s_acLogPrinters) && s_acLogPrinters[i]; i++);

  *plen = i;
  return s_acLogPrinters;
}

void log_set_process_name(const char * name) {
  ASSERT(name);
  ASSERT(*name);

  strncpy(acProgramName, name, sizeof(acProgramName) - 1);
  acProgramName[sizeof(acProgramName) - 1] = 0;
}

void log_enable_time_print(bool bEnable) {
  s_bEnableTimePrint = bEnable ? true : false;
}

void log_enable_date_print(bool bEnable) {
  s_bEnableDatePrint = bEnable ? true : false;
}

void log_enable_source_print(bool bEnable) {
  s_bEnableSourcePrint = bEnable ? true : false;
}

void log_enable_process_print(bool bEnable) {
  s_bEnableProcessPrint = bEnable ? true : false;
}

void log_enable_method_print(bool bEnable) {
  s_bEnableProcessPrint = bEnable ? true : false;
}

void log_set_syslog_max_level(log_priority_t prio) {
  ASSERT(prio <= LOG_PRIORITY_DISABLED);
  s_u32SysLogMaxLevel = prio;
}

void log_set_stdout_max_level(log_priority_t prio) {
  ASSERT(prio <= LOG_PRIORITY_DISABLED);
  s_u32StdOutLogMaxLevel = prio;
}

void log_set_cli_max_level(log_priority_t prio) {
  ASSERT(prio <= LOG_PRIORITY_DISABLED);
  s_u32CliLogMaxLevel = prio;
}

const char * log_priority_enum_to_string(uint prio)
{
  return
    (prio == LOG_PRIORITY_DEBUG) ? "DEBUG" :
    (prio == LOG_PRIORITY_INFO)  ? "INFO" :
    (prio == LOG_PRIORITY_WARN)  ? "WARNING" :
    (prio == LOG_PRIORITY_ERROR) ? "ERROR" :
    (prio == LOG_PRIORITY_FATAL) ? "FATAL" :
    (prio == LOG_PRIORITY_DISABLED) ? "DISABLED" :
    "__UNKNOWN__";
}

bool log_is_permitted(log_printer_t * handle, uint prio)
{
  ASSERT(handle);
  if(prio >= LOG_PRIORITY_DISABLED) {
    return false;
  }
  return (prio <= handle->maxLogLevel) &&
      ((s_u32SysLogMaxLevel >= prio) || (s_u32StdOutLogMaxLevel >= prio) || (s_u32CliLogMaxLevel >= prio));
}


void vWriteToSysLog (uint prio, const char * msg)
{
  if(!s_bLogSysLogReady) {
    openlog (acProgramName, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    s_bLogSysLogReady = true;
  }
  if(s_bLogSysLogReady) {
    syslog (prio, "%s", msg);
  }
}

void vWriteFatalToFiles (const char * msg, uint len)
{
  ASSERT(msg);
  ASSERT(*msg);
  ASSERT(len);

  char fatalErrorPath[128];
  snprintf (fatalErrorPath, sizeof(fatalErrorPath) - 1, "%s/%s", FATAL_LOG_PATH, acProgramName);

  errno = 0;

  if(0 != CreateDirIfNotFound (fatalErrorPath)) {
    fprintf (stderr, "mkdir(%s) failed (%s)\n", fatalErrorPath, strerror(errno));
    return;
  }

  char fatalErrorFile[256];
  snprintf (fatalErrorFile, sizeof(fatalErrorFile) - 1, "%s/fatal_log", fatalErrorPath);

  int fd = -1;
  if(0 == access (fatalErrorFile, F_OK)) {
    fd = open (fatalErrorFile, O_APPEND | O_RDWR);
  }
  else {
    fd = open (fatalErrorFile, O_CREAT | O_RDWR, 0644);
  }

  if(fd < 0) {
    fprintf (stderr, "open(%s) failed (%s)\n", fatalErrorFile, strerror(errno));
    return;
  }

  (void) write (fd, msg, len);
  fsync (fd);
  close (fd);
}


uint ss_strncpy(char * d, uint l, const char * s, uint n)
{
  ASSERT(d);
  ASSERT(s);
  ASSERT(n < l);

  char * e = strncpy(d, s, n);

  ASSERT(e >= s);

  return (e - s);
}

uint append_time (char * s, uint l, uint n)
{
  ASSERT(s);
  ASSERT(l > n);

  errno = 0;
  time_t ts = time(NULL);
  struct tm * ptm = localtime(&ts);

  ASSERT(ptm);

  n += snprintf(s + n, l - n, "%02u-%02u-%02u %02u:%02u:%02u ",
      ptm->tm_year - 100,
      ptm->tm_mon + 1,
      ptm->tm_mday,
      ptm->tm_hour,
      ptm->tm_min,
      ptm->tm_sec);

  return n;
}

uint append_thread_name (char * s, uint l, uint n)
{
  ASSERT(s);
  ASSERT(l > n);

  char name[32];
  pthread_getname_np(pthread_self(), name, sizeof(name));
  n += snprintf(
      s + n, l - n,
      "%s[%04u]: %s: ", acProgramName, getpid(), name);

  return n;
}

int fatal_printf (
  const char * file, const char * func,
  int line, void * handle, uint prio,
  const char * fmt, ...)
{
  char msg[LOG_MSG_MAX_SIZE];
  int nb = 0;




  nb += snprintf(
      msg + nb, sizeof(msg) - nb,
      "%s: %d: %s(): ", file, line, func);

  int msgPos = nb;

  nb += snprintf(
      msg + nb, sizeof(msg) - nb,
      "%-8s: ", log_priority_enum_to_string(prio));

  va_list ap;
  va_start(ap, fmt);
  nb += vsnprintf(msg + nb, sizeof(msg) - nb, fmt, ap);
  va_end(ap);

  if(s_cli_log_cb_handler) {
    s_cli_log_cb_handler(msg, nb);
  }

  {
    write(STDOUT_FILENO, msg, nb);
    fsync(STDOUT_FILENO);
  }

  vWriteToSysLog(prio, &msg[msgPos]);

  vWriteFatalToFiles(msg, nb);

  (void) handle;

  sync();

  exit(0);

  return 0;
}

int log_printf (
  const char * file, const char * func,
  int line, void * handle, uint prio,
  const char * fmt, ...)
{
  char msg[LOG_MSG_MAX_SIZE];
  int nb = 0;

  if(s_bEnableDatePrint || s_bEnableTimePrint) {
    time_t ts = time(NULL);
    struct tm * ptm = localtime(&ts);

    ASSERT(ptm);

    if(s_bEnableDatePrint) {
      nb += snprintf(
          msg + nb, sizeof(msg) - nb,
          "%02u-%02u-%02u ",
          ptm->tm_year - 100,
          ptm->tm_mon + 1,
          ptm->tm_mday);
    }

    if(s_bEnableTimePrint) {
      nb += snprintf(
          msg + nb, sizeof(msg) - nb,
          "%02u:%02u:%02u ",
          ptm->tm_hour,
          ptm->tm_min,
          ptm->tm_sec);
    }
  }

  if(s_bEnableProcessPrint) {
    char taskName[32];

    pthread_getname_np(pthread_self(), taskName, sizeof(taskName));

    nb += snprintf(
        msg + nb, sizeof(msg) - nb,
        "%s[%04u]: %s: ", acProgramName, getpid(), taskName);
  }

  if(s_bEnableSourcePrint) {
    nb += snprintf(
        msg + nb, sizeof(msg) - nb,
        "%s: %d: ", file, line);
  }

  if(s_bEnableMethodPrint) {
    nb += snprintf(
        msg + nb, sizeof(msg) - nb,
        "%s(): ", func);
  }

  int msgPos = nb;

  nb += snprintf(
      msg + nb, sizeof(msg) - nb,
      "%-8s: ", log_priority_enum_to_string(prio));

  va_list ap;
  va_start(ap, fmt);
  nb += vsnprintf(msg + nb, sizeof(msg) - nb, fmt, ap);
  va_end(ap);

  if (s_u32SysLogMaxLevel >= prio) {
    vWriteToSysLog(prio, &msg[msgPos]);
  }

  if (s_u32StdOutLogMaxLevel >= prio) {
    if(prio <= LOG_PRIORITY_WARN) {
      write(STDERR_FILENO, msg, nb);
    } else {
      write(STDOUT_FILENO, msg, nb);
    }
  }

  if(s_cli_log_cb_handler) {
    s_cli_log_cb_handler(msg, nb);
  }

  (void) handle;
  return nb;
}
