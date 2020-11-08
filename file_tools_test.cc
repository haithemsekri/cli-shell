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

int main(void)
{
  TEST_EQ(1, dir_exist("/tmp/"));
  TEST_EQ(0, dir_exist("/tmp1/"));
  TEST_EQ(-1, dir_exist(nullptr));


  TEST_EQ(0, dirf_delete_recursive("/tmp/test_dir"));

  TEST_EQ(0, dirf_exist("/tmp/test_dir/%s%d", "test_dir_lv", 1));
  TEST_EQ(-1, dir_create("/tmp/test_dir/test_dir_lv1"));
  TEST_EQ(0, file_exist("/tmp/test_dir/test_dir_lv1/file1"));
  TEST_EQ(0, dirf_create_recursive("/tmp/test_dir/%s%d", "test_dir_lv", 1));

  int fd = file_create("/tmp/test_dir/test_dir_lv1/file1");
  TEST_GT(fd, 0);
  TEST_GT(file_printf(fd, "hello world\n"), 0);
  fsync(fd);
  close(fd);

  TEST_EQ(1, file_exist("/tmp/test_dir/test_dir_lv1/file1"));

  char buff[1024];
  uint l = sizeof(buff);
  TEST_EQ(0, dirf_list_nodes(buff, &l, DT_DIR, "/tmp/test_dir/%s", "test_dir_lv1"));
  l = sizeof(buff);
  TEST_EQ(0, dir_list_nodes(buff, &l, DT_DIR, "/tmp/test_dir/test_dir_lv1/file1"));
  TEST_EQ(1, dir_list_nodes(buff, &l, DT_REG, "/tmp/test_dir/test_dir_lv1"));
  TEST_EQ(0, filef_delete("/tmp/test_dir/%s", "test_dir_lv1/file1"));
  TEST_EQ(26, pathf_base_pos("/tmp/test_dir/%s", "test_dir_lv1/file1"));

  return 0;
}