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

static int32_t s_i32FindCmdIdxByName(const char * name)
{
  ASSERT(name);
  ASSERT(*name);

  int32_t i32Id;
  int32_t maxIdx = SIZEOF_ARRAY(stCliCommandsList);

  for(i32Id = 0; i32Id < maxIdx; i32Id++) {
    if(!strcmp(name, stCliCommandsList[i32Id].cmd)) {
      break;
    }
  }

  if(i32Id >= maxIdx) {
    i32Id = -1;
  }

  return i32Id;
}

static int32_t s_i32FindCmdIdxById(uint32_t u32Id)
{
  int32_t i32Id = -1;
  int32_t minIdx = 0;
  int32_t maxIdx = SIZEOF_ARRAY(stCliCommandsList) - 1;

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

void vCli_RegisterCommandByName(const char * name, int32_t (*cb) (void * handle, int32_t argc, const char * argv[]))
{
  ASSERT(name);
  ASSERT(*name);
  ASSERT(cb);

  int32_t i32Id = s_i32FindCmdIdxByName(name);
  ASSERT(i32Id != -1);

  stCliCommandsList[i32Id].pftCliCb = cb;
}

int32_t i32AddrDiff(const void * start, const void * end)
{
  ASSERT(start);
  ASSERT(end);
  ASSERT(end >= start);
  return ((const char *) end) - ((const char *) start);
}

int32_t findCharDelimetersPos(const char * s, uint32_t u32sLen, uint32_t u32Pos0, char c1, char c2, int32_t * pi32c2Pos)
{
  ASSERT(s);
  ASSERT(pi32c2Pos);
  ASSERT(u32sLen);

  if(u32Pos0 >= u32sLen) {
    return -1;
  }

  const char * p0 = strchr(&s[u32Pos0], c1);
  if(!p0) {
    return -1;
  }

  const char * p1 = strchr(p0 + 1, c2);
  if(!p1) {
    return -1;
  }

  *pi32c2Pos = i32AddrDiff(s, p1);
  return i32AddrDiff(s, p0);
}

int32_t findStrDelimetersPos(const char * s, uint32_t u32sLen, uint32_t u32Pos0, const char * s1, const char * s2, int32_t * pi32s2Pos)
{
  ASSERT(s);
  ASSERT(s1);
  ASSERT(s2);
  ASSERT(pi32s2Pos);
  ASSERT(u32sLen);

  if(u32Pos0 >= u32sLen) {
    return -1;
  }

  const char * p0 = strstr(&s[u32Pos0], s1);
  if(!p0) {
    return -1;
  }

  if(i32AddrDiff(s, p0) > u32sLen) {
    return -1;
  }

  const char * p1 = strstr(p0 + 1, s2);
  if(!p1) {
    return -1;
  }

  if(i32AddrDiff(s, p1) > u32sLen) {
    return -1;
  }

  *pi32s2Pos = i32AddrDiff(s, p1);
  return i32AddrDiff(s, p0);
}

int32_t CharCount(char * s, uint32_t u32Pos0, uint32_t u32Pos1, char c1)
{
  ASSERT(s);

  int32_t count = 0;

  if(u32Pos0 > u32Pos1) {
    return 0;
  }

  for(uint32_t pos = u32Pos0; pos <= u32Pos1; pos++) {
    if(s[pos] == c1) {
      count++;
    }
  }

  return count;
}

int32_t findCharAndReplace(char * s, uint32_t u32Pos0, uint32_t u32Pos1, char c1, char c2)
{
  ASSERT(s);

  int32_t count = 0;

  if(u32Pos0 > u32Pos1) {
    return 0;
  }

  for(uint32_t pos = u32Pos0; pos <= u32Pos1; pos++) {
    if(s[pos] == c1) {
      count++;
      s[pos] = c2;
    }
  }

  return count;
}

uint32_t TokenizeArguments(char * s, uint32_t u32Len, char * argv[], uint32_t u32ArgvMax)
{
  ASSERT(s);
  ASSERT(u32Len);
  ASSERT(argv);
  ASSERT(u32ArgvMax);

  uint32_t argc = 0;
  uint32_t pos = 0;

  while(pos <= u32Len) {
    int32_t pos1 = -1;
    int32_t pos0 = findCharDelimetersPos(s, u32Len, pos, '"', '"', &pos1);
    if((pos0 == -1) || (pos1 == -1)) {
      findCharAndReplace(s, pos, u32Len, ' ', 0);
      break;
    } else {
      findCharAndReplace(s, pos, pos0, ' ', 0);
      s[pos0] = 0;
      s[pos1] = 0;
      pos = pos1+ 1;
    }
  }

  pos = 0;
  while((pos <= u32Len) && (argc < u32ArgvMax)) {
    if(s[pos]) {
      int32_t len = strlen(&s[pos]);
      argv[argc] = &s[pos];
      argc++;
      pos += len + 1;
    } else {
      pos++;
    }
  }

  return argc;
}

int32_t i32Cli_ClientWriteRawData(void * handle, const void * buf, uint32_t len)
{
  ASSERT(handle);
  ASSERT(buf);
  ASSERT(len);

  int32_t i32Ret = write(STDOUT_FILENO, buf, len);
  fsync(STDOUT_FILENO);
  return i32Ret;
}

int32_t i32Cli_ClientWriteFmtData(void * handle, const char * fmt, ...)
{
  ASSERT(handle);
  ASSERT(fmt);

  va_list vaList;
  char buf[512];

  va_start(vaList, fmt);
  ssize_t nb = vsnprintf(buf, sizeof(buf) - 1, fmt, vaList);
  va_end(vaList);
  buf[sizeof(buf) - 1] = 0;
  return i32Cli_ClientWriteRawData(handle, buf, nb);
}

void vRunCliCmd(void * handle, uint32_t u32Id, const char * pcArgs)
{
  ASSERT(handle);
  ASSERT(u32Id);
  ASSERT(pcArgs);
  ASSERT(*pcArgs);

  int32_t i32Id = s_i32FindCmdIdxById(u32Id);
  if(i32Id == -1) {
    i32Cli_ClientWriteFmtData(handle, "0x%08x: command not found\r\n", u32Id);
    return ;
  }
  if(stCliCommandsList[i32Id].pftCliCb == NULL) {
    i32Cli_ClientWriteFmtData(handle, "%s@0x%08x: command not registered\r\n", stCliCommandsList[i32Id].cmd, u32Id);
    return ;
  }

  const uint32_t u32ArgcMax = 32;
  uint32_t len  = strlen(pcArgs);
  char pcArgsTmp[len + 1];
  strncpy(pcArgsTmp, pcArgs, len);
  pcArgsTmp[len] = 0;

  char * argv[u32ArgcMax];
  uint32_t argc = TokenizeArguments(pcArgsTmp, len, argv, u32ArgcMax);
  int32_t i32Ret = stCliCommandsList[i32Id].pftCliCb(handle, argc, (const char **) argv);
  (void) i32Ret;
}

int32_t i32Cli_KeyWordValuePos(char * s, uint32_t len, const char * pcKeyWord)
{
  ASSERT(s);
  ASSERT(*s);
  ASSERT(len);
  ASSERT(pcKeyWord);
  ASSERT(*pcKeyWord);

  char * p = strstr(s, pcKeyWord);
  if(!p) {
    return -1;
  }

  p += strlen(pcKeyWord);

  int32_t i32Pos = i32AddrDiff(s, p);

  if(i32Pos > len) {
    return -1;
  }

  return i32Pos;
}

int32_t i32Cli_ParseRxData(char * data, uint32_t len, uint32_t * pi32Id, char * pcArgs)
{
  ASSERT(data);
  ASSERT(len);
 // ASSERT(pi32Id);
  //ASSERT(pcArgs);

  int32_t i32S2Pos = -1;
  int32_t i32S1Pos = findStrDelimetersPos(data, len, 0, "<cmd ", "</cmd>", &i32S2Pos);
  printf("i32S1Pos == %d\n", i32S1Pos);
  printf("i32S2Pos == %d\n", i32S2Pos);
  if(i32S1Pos == -1) {
    return -1;
  }
  if(i32S2Pos == -1) {
    return -1;
  }

  i32S1Pos += sizeof("<cmd ") - 1;
  i32S2Pos -= sizeof("</cmd>");

  int32_t idPos = i32Cli_KeyWordValuePos(data + i32S1Pos, i32S2Pos - i32S1Pos + 1, "id=\"");
  printf("idPos == %d\n", idPos);
  if(idPos == -1) {
    return -1;
  }
  idPos += i32S1Pos;

  int32_t arglistPos = i32Cli_KeyWordValuePos(data + i32S1Pos, i32S2Pos - i32S1Pos + 1, "arglist=\"");
  if(arglistPos == -1) {
    return -1;
  }
  arglistPos += i32S1Pos;

  printf("arglistPos == %d\n", arglistPos);
  //*pi32Id = &data[idPos];
  //*pcArgs = &data[arglistPos];
  return 0;
}

//<cmd id="0x3ff673f2" name="modem_sms_send" arglist="s:sNumber s:sData" args="+464589666252 "hello data 1, hello data 2""></cmd>


int32_t cli_modem_com_write_tx_data (void * handle, int32_t argc, const char * argv[])
{
  (void) (argv);
  i32Cli_ClientWriteFmtData(handle, "%s: received <argc == %d>\r\n", __func__, argc);
  for (int32_t argi = 0; argi < argc; argi++) {
    i32Cli_ClientWriteFmtData(handle, "    argv[%u] == <%s>\r\n", argi, argv[argi]);
  }
  return 0;
}

int32_t i32Cli_DivideRxData(char * data, uint32_t len)
{
  ASSERT(data);
  ASSERT(len);

  int32_t counter = 0;
  int32_t pos = 0;

  for(;pos < len;) {

    char * begin =
  }
  return counter;
}

int main(void)
{
  if(1) {
  char  cmdline[]  = "  id=\"0x3ff673f2\" name=\"modem_sms_send\" arglist=\"s:sNumber s:sData\" args=\"+464589666252 \"hello data 1, hello data 2\"\"   ";
  uint32_t * pi32Id = NULL;
  char * pcArgs = NULL;
  int32_t i32Ret = i32Cli_ParseRxData(cmdline, sizeof(cmdline) - 1,  pi32Id,  pcArgs);

  }
  return 0;
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