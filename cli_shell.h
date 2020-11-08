

/**********************************************************************/
/****************** Genrated File avoid to modify *********************/
/**********************************************************************/

#include <stdint.h>
#include <string.h>


typedef struct {
  const uint32_t u32Id;
  int32_t (*pftCliCb) (void * handle, int32_t argc, const char * argv[]);
  uint32_t argc;
  const char * cmd;
  const char * argv;
} CliMethodStructType;

static CliMethodStructType stCliCommandsList[] = {
    {0x0b5151cd, NULL, 0, "modem_status", NULL},
    {0x0bb8a2a1, NULL, 0, "modem_com_enable_async_rx_print", NULL},
    {0x151b0c4d, NULL, 0, "modem_com_disable_async_rx_print", NULL},
    {0x170aa9d3, NULL, 0, "log_list_loggers", NULL},
    {0x276df8fd, NULL, 0, "modem_com_read_rx_data", NULL},
    {0x2b17a495, NULL, 0, "modem_hw_reset", NULL},
    {0x3598398c, NULL, 1, "modem_sms_delete", "u:uId"},
    {0x362d73ec, NULL, 0, "log_status", NULL},
    {0x3ff673f2, NULL, 2, "modem_sms_send", "s:sNumber s:sData"},
    {0x47c38d54, NULL, 0, "modem_hw_power_off", NULL},
    {0x61d0d0e1, NULL, 0, "modem_sms_status", NULL},
    {0x66502d4d, NULL, 1, "log_set_verbosity", "s:sVerbosity"},
    {0x7d3537d2, NULL, 1, "modem_sms_read", "u:uId"},
    {0x8695ac23, NULL, 0, "modem_sms_list", NULL},
    {0x98e202f2, NULL, 0, "modem_hw_status", NULL},
    {0xa39e4493, NULL, 0, "modem_call_status", NULL},
    {0xb868e062, NULL, 1, "modem_call_dial", "s:sNumber"},
    {0xd0b1b299, NULL, 2, "modem_com_write_tx_data", "s:sPort s:sData"},
    {0xd26fa6b4, NULL, 0, "cli_ping", NULL},
    {0xd3f2c3c1, NULL, 0, "modem_call_hang", NULL},
    {0xdddb4f3b, NULL, 0, "modem_hw_power_on", NULL},
};

