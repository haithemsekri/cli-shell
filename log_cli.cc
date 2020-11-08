
#include "log.h"
#include <string.h>
#include "shell_cli_console.h"

//log_printer_t * const * log_get_writer_list(unsigned * plen);

bool char_is_int(char c) {
  return (c >= '0') &&  (c <= '9');
}

bool char_is_hex(char c) {
  return ((c >= '0') &&  (c <= '9')) || ((c >= 'A') &&  (c <= 'F')) || ((c >= 'a') &&  (c <= 'f')) || (c == 'x');
}

bool str_is_int(const char * s) {

  if(!s)
    return false;
  if(!*s)
    return false;

  while(*s) {
    if(!char_is_int(*s++)) {
      return false;
    }
  }

  return true;
}

bool str_is_hex(const char * s) {

  if(!s)
    return false;
  if(!*s)
    return false;

  while(*s) {
    if(!char_is_hex(*s++)) {
      return false;
    }
  }

  return true;
}


int str_to_on_off(const char * s) {

  if(!s)
    return -1;
  if(!*s)
    return -1;

  return !strcmp(s, "on") ? 1 :
    !strcmp(s, "off") ? 0 : -1;
}

int str_to_yes_no(const char * s) {

  if(!s)
    return -1;
  if(!*s)
    return -1;

  return !strcmp(s, "yes") ? 1 :
    !strcmp(s, "no") ? 0 : -1;
}

static int cli_log_list_writer (void * handle, int argc, const char * argv[])
{
  (void) argc;
  (void) argv;

  cli_console_fmt_write(handle, " Printer                      |    Level   |\r\n");
  cli_console_fmt_write(handle, "------------------------------+------------+\r\n");

  unsigned l = 0;
  log_printer_t * const * list = log_get_writer_list(&l);
  for(unsigned i = 0; i < l; i++) {
    cli_console_fmt_write(handle, " %-28s | %-10s |\r\n", list[i]->name, log_priority_enum_to_string(list[i]->maxLogLevel));
  }

  cli_console_fmt_write(handle, "------------------------------+------------+\r\n");

  return 0;
}

static int cli_log_enable_time_print (void * handle, int argc, const char * argv[])
{
  if (argc != 2) {
    cli_console_fmt_write(handle, "Invalid argument\r\n");
    return 0;
  }

  cli_console_fmt_write(handle, "%s: argv[1] == %s\r\n", __func__, argv[1]);

  int status = str_to_on_off(argv[1]);
  if (status == -1) {
    cli_console_fmt_write(handle, "Invalid argument\r\n");
    return 0;
  }

  log_enable_time_print(status);

  cli_console_fmt_write(handle, "OK\r\n");
  return 0;
}

static int cli_log_enable_date_print (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_enable_source_print (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_enable_process_print (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_enable_method_print (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_set_syslog_max_level (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_set_stdout_max_level (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_set_cli_max_level (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_set_printer_max_prio (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

static int cli_log_is_permitted (void * handle, int argc, const char * argv[])
{
  cli_console_fmt_write(handle, "%s: argc == %d\r\n", __func__, argc); (void) argv;
  return 0;
}

void log_register_cli(void)
{
  cli_console_add_cmd_cb("log_list_writer", cli_log_list_writer);
  cli_console_add_cmd_cb("log_enable_time_print", cli_log_enable_time_print);
  cli_console_add_cmd_cb("log_enable_date_print", cli_log_enable_date_print);

  cli_console_add_cmd_cb("log_enable_source_print", cli_log_enable_source_print);
  cli_console_add_cmd_cb("log_enable_process_print", cli_log_enable_process_print);
  cli_console_add_cmd_cb("log_enable_method_print", cli_log_enable_method_print);


  cli_console_add_cmd_cb("log_set_syslog_max_level", cli_log_set_syslog_max_level);
  cli_console_add_cmd_cb("log_set_stdout_max_level", cli_log_set_stdout_max_level);
  cli_console_add_cmd_cb("log_set_cli_max_level", cli_log_set_cli_max_level);

  cli_console_add_cmd_cb("log_set_printer_max_prio", cli_log_set_printer_max_prio);
  cli_console_add_cmd_cb("log_is_permitted", cli_log_is_permitted);

}
