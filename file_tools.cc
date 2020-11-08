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
#include <dirent.h>

#include "defines.h"
#include "file_tools.h"

#define FMT_TO_STRING(fmt, sz) \
  char fmt_buf[sz]; \
  va_list ap; \
  uint fmt_buf_nb; \
  va_start(ap, fmt); \
  fmt_buf_nb = vsnprintf(fmt_buf, sz - 1, fmt, ap); \
  va_end(ap); \
  fmt_buf[fmt_buf_nb] = 0

#define PATH_TO_STRING(fmt, sz) \
  char fmt_buf[sz]; \
  va_list ap; \
  uint fmt_buf_nb; \
  ASSERT(fmt); \
  ASSERT(*fmt == '/'); \
  va_start(ap, fmt); \
  fmt_buf_nb = vsnprintf(fmt_buf, sz - 1, fmt, ap); \
  va_end(ap); \
  fmt_buf[fmt_buf_nb] = 0


int dir_exist(const char * path)
{
  ASSERT(path);
  ASSERT(*path);
  struct stat st;
  return ((stat(path, &st) == 0) && (st.st_mode & S_IFDIR)) ? 1 : 0;

  FAILURE(return -1);
}

int dirf_exist(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_exist(fmt_buf);

  FAILURE(return -1);
}

int file_exist(const char * path)
{
  ASSERT(path);
  ASSERT(*path);
  return (0 == access(path, F_OK)) ? 1 : 0;

  FAILURE(return -1);
}

int filef_exist(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return file_exist(fmt_buf);

  FAILURE(return -1);
}

int dir_create(const char * path)
{
  ASSERT(path);
  ASSERT(*path);
  if(dir_exist(path) > 0) {
    return 0;
  }

  if(mkdir(path, 0755) != 0) {
    return -1;
  }

  return 0;

  FAILURE(return -1);
}

int dirf_create(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_create(fmt_buf);

  FAILURE(return -1);
}

int dir_create_recursive(char * path, uint n)
{
  ASSERT(path);
  ASSERT(n >= 2);
  ASSERT(*path == '/');

  for(uint i = 1; i < n; i++) {
    if(path[i] == '/') {
      path[i] = 0;
      ASSERT(dir_create(path) == 0);
      path[i] = '/';
    }
  }

  ASSERT(dir_create(path) == 0);
  return 0;
  FAILURE(return -1);
}

int dirf_create_recursive(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_create_recursive(fmt_buf, fmt_buf_nb);
  FAILURE(return -1);
}

int file_create(const char * path)
{
  ASSERT(path);
  ASSERT(*path);

  if(0 == access(path, F_OK)) {
    return open(path, O_RDWR);
  } else {
    return open(path, O_CREAT | O_RDWR, 0644);
  }

  FAILURE(return -1);
}

int filef_create(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return file_create(fmt_buf);

  FAILURE(return -1);
}

int file_open_rw(const char * path)
{
  ASSERT(path);
  ASSERT(*path);
  return open(path, O_RDWR);

  FAILURE(return -1);
}

int filef_open_rw(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return file_open_rw(fmt_buf);

  FAILURE(return -1);
}

int file_open_ro(const char * path)
{
  ASSERT(path);
  ASSERT(*path);
  return open(path, O_RDONLY);
  FAILURE(return -1);
}

int filef_open_ro(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return file_open_ro(fmt_buf);
  FAILURE(return -1);
}

int file_printf(int fd, const char * fmt, ...)
{
  FMT_TO_STRING(fmt, 1024);
  return write(fd, fmt_buf, fmt_buf_nb);
}

int path_base_pos(const char * path)
{
  uint l;
  ASSERT(path);
  ASSERT(*path == '/');

  l = strlen(path) - 1;
  while(l > 0) {
    if(path[l] == '/') {
      break;
    }
    l--;
  }

  return l;
  FAILURE(return -1);
}

int pathf_base_pos(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return path_base_pos(fmt_buf);
  FAILURE(return -1);
}

int file_delete(const char * path)
{
  ASSERT(path);
  ASSERT(*path == '/');

  if(file_exist(path) > 0) {
    ASSERT(unlink(path) == 0);
  }

  return 0;
  FAILURE(return -1);
}

int filef_delete(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return file_delete(fmt_buf);
  FAILURE(return -1);
}

int dir_delete(const char * path)
{
  ASSERT(path);
  ASSERT(*path == '/');

  if(dir_exist(path) == 0) {
    return 0;
  }

  ASSERT(rmdir(path) == 0);
  return 0;
  FAILURE(return -1);
}

int dirf_delete(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_delete(fmt_buf);
  FAILURE(return -1);
}

int dir_list_nodes(char * b, uint * l, int type, const char * path)
{
  DIR * dir;
  uint node_i;
  uint nb;
  struct dirent * dir_e;
  int ret;

  ASSERT(path);
  ASSERT(*path == '/');
  ASSERT(b);
  ASSERT(l);
  ASSERT(*l);

  ret = dir_exist(path);
  ASSERT(ret != -1);
  if(ret == 0)
    return 0;

  dir = opendir(path);
  ASSERT(dir);

  node_i = 0;
  nb = 0;
  dir_e = NULL;

  while ((dir_e = readdir(dir))) {

    if(*(dir_e->d_name) == '.')
      continue;

    if(type == dir_e->d_type) {
      int name_len = strlen(dir_e->d_name);
      if(!name_len)
        continue;

      ASSERT((name_len + nb + 1) < *l);
      strncpy(b + nb, dir_e->d_name, name_len);
      nb += name_len;
      b[nb++] = 0;
      node_i++;
    }
  }

  *l = nb;
  closedir(dir);
  return node_i;

  FAILURE(
    if(dir) {
      closedir(dir);
    }
    return -1);
}

int dirf_list_nodes(char * b, uint * l, int t, const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_list_nodes(b, l, t, fmt_buf);
  FAILURE(return -1);
}

int dir_delete_recursive(const char * path)
{
  DIR * dir = NULL;
  struct dirent * dir_e = NULL;
  int ret;

  ASSERT(path);
  ASSERT(*path == '/');

  ret = dir_exist(path);
  ASSERT(ret != -1);
  if(!ret)
    return 0;

  dir = opendir(path);
  ASSERT(dir);

  while ((dir_e = readdir(dir))) {

    if(*(dir_e->d_name) == '.')
      continue;

    char buf[512];
    snprintf(buf, sizeof(buf) - 1, "%s/%s", path, dir_e->d_name);

    if(DT_REG == dir_e->d_type) {
      ASSERT(0 == unlink(buf));
    } else if(DT_DIR == dir_e->d_type) {
      ASSERT(0 == dir_delete_recursive(buf));
    }
  }

  closedir(dir);
  dir = NULL;
  ASSERT(0 == dir_delete(path));
  return 0;

  FAILURE(
    if(dir) {
      closedir(dir);
    }
    return -1);
}

int dirf_delete_recursive(const char * fmt, ...)
{
  PATH_TO_STRING(fmt, 256);
  return dir_delete_recursive(fmt_buf);
  FAILURE(return -1);
}
