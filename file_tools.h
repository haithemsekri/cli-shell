#ifndef __FILE_TOOLS_INCLUDES__
#define __FILE_TOOLS_INCLUDES__ "1a6525dc-96c5-4362-b41d-c785480c6812"

#include <inttypes.h>
#include "defines.h"

int dir_exist(const char * path);
int dirf_exist(const char * fmt, ...) ASSERT_FMT(1, 2);
int dir_create(const char * path);
int dirf_create(const char * fmt, ...) ASSERT_FMT(1, 2);
int dir_create_recursive(char * path, uint n);
int dirf_create_recursive(const char * fmt, ...) ASSERT_FMT(1, 2);
int dir_delete(const char * path);
int dirf_delete(const char * fmt, ...) ASSERT_FMT(1, 2);
int dir_list_nodes(char * b, uint * l, int type, const char * path);
int dirf_list_nodes(char * b, uint * l, int t, const char * fmt, ...) ASSERT_FMT(4, 5);
int dir_delete_recursive(const char * path);
int dirf_delete_recursive(const char * fmt, ...) ASSERT_FMT(1, 2);

int file_exist(const char * path);
int filef_exist(const char * fmt, ...) ASSERT_FMT(1, 2);
int file_create(const char * path);
int filef_create(const char * fmt, ...) ASSERT_FMT(1, 2);
int filef_open_rw(const char * fmt, ...) ASSERT_FMT(1, 2);
int file_open_ro(const char * path);
int filef_open_ro(const char * fmt, ...) ASSERT_FMT(1, 2);
int file_printf(int fd, const char * fmt, ...) ASSERT_FMT(2, 3);
int file_delete(const char * path);
int filef_delete(const char * fmt, ...) ASSERT_FMT(1, 2);

int path_base_pos(const char * path);
int pathf_base_pos(const char * fmt, ...) ASSERT_FMT(1, 2);

#endif //__FILE_TOOLS_INCLUDES__
