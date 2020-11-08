#include <sys/ioctl.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>

#include "log.h"
#include "shell_cli_console.h"

#define SIZEOF_ARRAY(a) ((unsigned)(sizeof(a) / sizeof(a[0])))

#define ASSERT assert

typedef struct shell_session_t {
  int ifd;
  int ofd;
} shell_session_t;

static struct termios stdin_config;
static int sfd = -1;
static int cfd = -1;
static pthread_t tty_thread_handle;
static pthread_t tcp_thread_handle;

static void * shell_console_run_stdio(void * args)
{
  (void) args;

  struct shell_session_t shell_session = {STDIN_FILENO, STDOUT_FILENO};

  LOG_PRINTF(pstDefaultLogPrinter, LOG_PRIORITY_INFO, "stdio cli console started\r\n");

  fsync(STDOUT_FILENO);
  cli_console_run(&shell_session);

  LOG_PRINTF(pstDefaultLogPrinter, LOG_PRIORITY_INFO, "stdio cli console exited\r\n");

  fsync(STDOUT_FILENO);
  tcsetattr(STDIN_FILENO, TCSANOW, &stdin_config);

  return NULL;
  //pthread_exit(0);
}

static void * shell_console_run_tcp(void * args)
{
  (void) args;

  for(;;) {
    if(cfd < 0) {
      struct sockaddr_in sAddrIn;
      socklen_t sLen = sizeof(sAddrIn);
      cfd = accept(sfd, (struct sockaddr*) &sAddrIn, &sLen);
    } else {
      struct shell_session_t shell_session = {cfd, cfd};
      LOG_PRINTF(pstDefaultLogPrinter, LOG_PRIORITY_INFO, "tcp cli console started\r\n");
      fsync(STDOUT_FILENO);
      cli_console_run(&shell_session);
      LOG_PRINTF(pstDefaultLogPrinter, LOG_PRIORITY_INFO, "tcp cli console exited\r\n");
      fsync(STDOUT_FILENO);
      close(cfd);
      cfd = -1;
    }
  }

  pthread_exit(0);
}

void shell_console_device_init(void)
{
  struct termios config;

  ASSERT(0 == tcgetattr(STDIN_FILENO, &stdin_config));
  config = stdin_config;
  config.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON);
  config.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  config.c_cflag &= ~(CSIZE | PARENB);

  ASSERT(0 == tcsetattr(STDIN_FILENO, TCSANOW, &config));

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  ASSERT(sfd > 0);

  int option = 1;
  int ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, (char*)&option, sizeof(option));
  ASSERT(ret == 0);

  option = 1;
  ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
  ASSERT(ret == 0);

  struct sockaddr_in sAddr;
  bzero(&sAddr, sizeof(sAddr));

  sAddr.sin_family = AF_INET;
  sAddr.sin_addr.s_addr = INADDR_ANY;
  sAddr.sin_port = htons(2015);

  ret = bind(sfd, (struct sockaddr*) &sAddr, sizeof(sAddr));
  ASSERT(ret == 0);

  ret = listen(sfd, 1);
  ASSERT(ret == 0);

}

void shell_console_device_start(void)
{
  pthread_create(&tcp_thread_handle, NULL, shell_console_run_tcp, NULL);
  if(0) {
    pthread_create(&tty_thread_handle, NULL, shell_console_run_stdio, NULL);
  }
}

void shell_console_device_stop(void)
{
  pthread_cancel(tcp_thread_handle);
  if(0) {
    pthread_cancel(tty_thread_handle);
  }
}

void shell_console_device_terminate(void)
{
  if(cfd > 0) {
    close(cfd);
    cfd = -1;
  }
  if(sfd > 0) {
    close(sfd);
    sfd = -1;
  }
  (void) tcsetattr(STDIN_FILENO, TCSANOW, &stdin_config);
}

int cli_console_data_read (void * handle, void * b, unsigned l)
{
  return read (((shell_session_t*) handle)->ifd, b, l);
}

int cli_console_data_write (void * handle, const void * b, unsigned l)
{
  return write (((shell_session_t*) handle)->ofd, b, l);
}

int cli_console_fmt_write (void * handle, const char * fmt, ...)
{
  char msg[1024];
  int nb = 0;

  va_list ap;
  va_start(ap, fmt);
  nb = vsnprintf(msg, sizeof(msg) - 1, fmt, ap);
  va_end(ap);

  return cli_console_data_write (handle, msg, nb);
}

static void cli_writer_handler (const char * msg, unsigned len)
{
  if(1) {
    struct shell_session_t shell_session = {STDIN_FILENO, STDOUT_FILENO};
    cli_console_data_write (&shell_session, msg, len);
  }

  if(cfd > 0) {
    struct shell_session_t shell_session = {cfd, cfd};
    cli_console_data_write (&shell_session, msg, len);
  }
}

int main(void)
{
  log_set_process_name("test_log");

  log_register_writer(pstDefaultLogPrinter);

  log_set_cli_writer_callback(cli_writer_handler);

  log_register_cli();

  log_set_syslog_max_level(LOG_PRIORITY_WARN);
  log_set_stdout_max_level(LOG_PRIORITY_FATAL);
  log_set_cli_max_level(LOG_PRIORITY_DEBUG);

  log_printer_t s_mainLogPrinter = {LOG_PRIORITY_WARN, "MainProgram"};
  s_mainLogPrinter.maxLogLevel = LOG_PRIORITY_DEBUG;
  log_register_writer(&s_mainLogPrinter);

  shell_console_device_init();
  shell_console_device_start();
  shell_console_run_stdio(NULL);
  shell_console_device_stop();
  shell_console_device_terminate();
  cli_console_term();
  return 0;
}

/*
 g++ shell_cli_console.cc shell_cli_console_device.cc  -o /tmp/shell_key_handler.cc.bin -Wall -Wno-sign-compare -lpthread -Wextra
socat $(tty),raw,echo=0 TCP:127.0.0.1:2015
modem_status g++ shell_key_handler.cc -o "-Wall  -Wno-sign-compare" /tmp/shell_key_handler.cc.bin -Wall  -Wno-sign-compare
  modem_power_on g++ shell_key_handler.cc -o "-Wall  -Wno-sign-compare" /tmp/shell_key_handler.cc.bin -Wall  -Wno-sign-compare
*/
