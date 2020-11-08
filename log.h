#ifndef __LOG_INCLUDE__
#define __LOG_INCLUDE__ "a8538102-ffe4-424e-99f4-89f161f7ac3a"

#include <syslog.h>

#define EOL "\r\n"

int log_write (const char * file, const char * func,
    int line, void * handle, uint prio,
    const char * s, uint l);

int log_printf (
    const char * file, const char * func,
    int line, void * handle, uint prio,
    const char * fmt, ...) __attribute__ ((format(printf, 6, 7)));

int fatal_write (
  const char * file, const char * func,
  int line, void * handle, uint prio,
  const char * s, uint l);

int fatal_printf (
  const char * file, const char * func,
  int line, void * handle, uint prio,
  const char * fmt, ...) __attribute__ ((format(printf, 6, 7)));

#define LOG_PRINTF(handle, prio, ...) \
  log_is_permitted(handle, prio) && log_printf(__FILE__, __func__, __LINE__, handle, prio, __VA_ARGS__)

#define LOG_PUTS(handle, prio, msg) \
  log_is_permitted(handle, prio) && log_printf(__FILE__, __func__, __LINE__, handle, prio, msg)

#define FATAL_PRINTF(...) \
  fatal_printf(__FILE__, __func__, __LINE__, NULL, LOG_PRIORITY_FATAL, __VA_ARGS__)

#define FATAL_PUTS(msg) \
  fatal_printf(__FILE__, __func__, __LINE__, NULL, LOG_PRIORITY_FATAL, msg)

#define ASSERT_PRINTF(cond, ...) \
  (!(cond)) && FATAL_PRINTF("Assertion failure (" #cond ")" EOL __VA_ARGS__)

#define ASSERT_PUTS(cond, msg) \
  (!(cond)) && FATAL_PRINTF("Assertion failure (" #cond ")" EOL, msg)

#ifndef ASSERT
#define ASSERT(cond) \
  (!(cond)) && FATAL_PRINTF("Assertion failure (" #cond ")" EOL)
#endif

#define LOG_MSG_MAX_SIZE 1024
#define FATAL_LOG_PATH "/tmp"

typedef struct log_printer_t {
  uint maxLogLevel;
  const char * name;
} log_printer_t;

typedef enum log_priority_t {
    LOG_PRIORITY_FATAL = LOG_EMERG,
    LOG_PRIORITY_ERROR = LOG_CRIT,
    LOG_PRIORITY_WARN = LOG_WARNING,
    LOG_PRIORITY_INFO = LOG_INFO,
    LOG_PRIORITY_DEBUG = LOG_DEBUG,
    LOG_PRIORITY_DISABLED = LOG_DEBUG + 1,
} log_priority_t;

extern log_printer_t * pstDefaultLogPrinter;
void log_set_process_name(const char * name);
void log_enable_time_print(bool bEnable);
void log_enable_date_print(bool bEnable);
void log_enable_source_print(bool bEnable);
void log_enable_process_print(bool bEnable);
void log_enable_method_print(bool bEnable);
void log_set_syslog_max_level(log_priority_t prio);
void log_set_stdout_max_level(log_priority_t prio);
void log_set_cli_max_level(log_priority_t prio);
log_printer_t * const * log_get_writer_list(uint * plen);
void log_register_writer(log_printer_t * handle);
bool log_set_printer_max_prio(const char * name, log_priority_t prio);
bool log_is_permitted(log_printer_t * handle, uint prio);
void log_set_cli_writer_callback(void (*cb) (const char * msg, uint len));
const char * log_priority_enum_to_string(uint prio);
void log_register_cli(void);

#endif
