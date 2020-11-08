#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include "cli_shell.h"

#define ASSERT(cond) do { if (!(cond)) { \
  fprintf(stderr, "%s: %d: %s: Assertion failure (%s).\n", \
  __FILE__, __LINE__, __func__, #cond); \
  abort();} } while(0)

#define ASSERT_MSG(cond, msg) do { if (!(cond)) { \
  fprintf(stderr, "%s: %d: %s: Assertion failure (%s): %s.\n", \
  __FILE__, __LINE__, __func__, #cond, msg); \
  abort();} } while(0)

#define ASSERT_RETURN(cond, r) do { if (!(cond)) { \
  fprintf(stderr, "%s: %d: %s: Assertion failure (%s).\n", \
  __FILE__, __LINE__, __func__, #cond); \
  return r;} } while(0)

#define ASSERT_RETURN_VOID(cond) do { if (!(cond)) do { if (!(cond)) { \
  fprintf(stderr, "%s: %d: %s: Assertion failure (%s).\n", \
  __FILE__, __LINE__, __func__, #cond); \
  return ;} } while(0)

#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

int32_t i32Cli_ClientWriteRawData(void * handle, const void * buf, uint32_t len)
{
  ASSERT(handle);
  ASSERT(buf);
  ASSERT(len);

  int32_t i32Ret = write(STDOUT_FILENO, buf, len);
  fsync(STDOUT_FILENO);
  return i32Ret;
}


int main(void)
{
  const char  * pcArgs  = "+464589666252 \"hello data 1\" 45  \"hello data 2\"";
  char * handle = NULL;

  vRunCliCmd(&handle, 0xd0b1b299, pcArgs);

  vCli_RegisterCommandByName("modem_com_write_tx_data", cli_modem_com_write_tx_data);

  vRunCliCmd(&handle, 0xd0b1b299, pcArgs);
  return 0;

  uint32_t u32Id = 0;
  uint32_t listSize = sizeof(stCliCommandsList) / sizeof(stCliCommandsList[0]);
  printf("listSize = %d\n", listSize);
  u32Id = 0; printf("s_i32FindCmdIdxById(%08x) == %d\n", u32Id, s_i32FindCmdIdxById(u32Id));
  u32Id = -1; printf("s_i32FindCmdIdxById(%08x) == %d\n", u32Id, s_i32FindCmdIdxById(u32Id));
  u32Id = stCliCommandsList[0].u32Id; printf("s_i32FindCmdIdxById(%08x) == %d\n", u32Id, s_i32FindCmdIdxById(u32Id));
  u32Id = stCliCommandsList[listSize - 1].u32Id; printf("s_i32FindCmdIdxById(%08x) == %d\n", u32Id, s_i32FindCmdIdxById(u32Id));
  u32Id = stCliCommandsList[listSize / 2].u32Id; printf("s_i32FindCmdIdxById(%08x) == %d\n", u32Id, s_i32FindCmdIdxById(u32Id));

  int32_t i32Id = s_i32FindCmdIdxByName("modem_sms_send");
  printf("s_i32FindCmdIdxByName() == %d\n", i32Id);
  printf("stCliCommandsList[i32Id].cmd == %s\n", stCliCommandsList[i32Id].cmd);
  return 0;
}