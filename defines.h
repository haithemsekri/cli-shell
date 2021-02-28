#ifndef __DEFINES_INCLUDES__
#define __DEFINES_INCLUDES__ "7813a91b-cfb7-443a-919c-e3e71be12fa9"

#define SIZEOF_ARRAY(a) ((unsigned)(sizeof(a) / sizeof(a[0])))
#define ASSERT_FMT(f, a) __attribute__ ((format(printf, f, a)))

#ifndef ASSERT
#ifdef ASSERT_TO_FAILURE
#define ASSERT(cond) \
  do { if(!(cond)) { \
    fprintf (stderr, "%s:%d: Assert(%s) failed\n", __FILE__, __LINE__, #cond); \
    goto FAIL; \
  }} while(0)
#define FAILURE(expr) \
  FAIL: expr
#else
#include <assert.h>
#define ASSERT assert
#define FAILURE(expr)
#endif
#endif




#ifdef DEV_TESTS
#include <iostream>

#define TEST_EQ(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue == rvalue)) { \
      auto lvalue = l; \
      auto rvalue = r; \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_EQ(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_EQ(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)

#define TEST_GT(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue > rvalue)) { \
      auto lvalue = l; \
      auto rvalue = r; \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_GT(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_GT(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)

#define TEST_GE(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue >= rvalue)) { \
      auto lvalue = l; \
      auto rvalue = r; \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_GE(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_GE(" << #l << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)
#endif

#endif //__DEFINES_INCLUDES__