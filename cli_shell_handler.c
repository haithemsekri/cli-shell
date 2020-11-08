#include <stdio.h>
#include <assert.h>
#include "cli_shell.h"


static int32_t s_i32FindCmdIdxByName(const char * name)
{
  int32_t i32Id = -1;

  if(name && *name) {
    for(i32Id = 0; i32Id < (sizeof(stCliCommandsList) / sizeof(stCliCommandsList[0])); i32Id++) {
      if(!strcmp(name, stCliCommandsList[i32Id].cmd)) {
        break;
      }
    }

    if(i32Id >= (sizeof(stCliCommandsList) / sizeof(stCliCommandsList[0]))) {
      i32Id = -1;
    }
  }

  return i32Id;
}

static int32_t s_i32FindCmdIdxById(uint32_t u32Id)
{
  int32_t i32Id = -1;
  int32_t minIdx = 0;
  int32_t maxIdx = sizeof(stCliCommandsList) / sizeof(stCliCommandsList[0]) - 1;

  while ((minIdx >= 0) && (maxIdx >= 0) && (minIdx <= maxIdx)) {
    int32_t pos = (minIdx + maxIdx) / 2;
    if(u32Id == stCliCommandsList[pos].u32Id) {
      i32Id = pos;
      break;
    } else if(u32Id < stCliCommandsList[pos].u32Id) {
      maxIdx = pos - 1;
    } else if (u32Id > stCliCommandsList[pos].u32Id) {
      minIdx = pos + 1;
    } else {
      break;
    }
  }

  return i32Id;
}

void vCli_AddCallBackByName(const char * name, int32_t (*pftCliCb) (void * handle, int32_t argc, const char * argv[]))
{
  assert(name && *name);
  assert(pftCliCb);

  int32_t i32Id = s_i32FindCmdIdxByName(name);

  if(i32Id == -1) {
    fprintf(stderr, "%s: command not found in cli list", name);
  } else {
    stCliCommandsList[i32Id].pftCliCb = pftCliCb;
  }
}


int main(void)
{
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