#ifndef __SHELL_CLI_CONSOLE_HH__
#define __SHELL_CLI_CONSOLE_HH__ 1

typedef int (*shell_command_callback_t) (void * handle, int argc, const char * argv[]);

void cli_console_add_cmd_cb(const char * name, shell_command_callback_t cb);

void cli_console_run(void * io_handle);

int cli_console_data_read (void * handle, void * b, unsigned l);
int cli_console_data_write (void * handle, const void * b, unsigned l);
int cli_console_fmt_write (void * handle, const char * fmt, ...);


void AddTestCommand (void);
void cli_console_term (void);

#endif