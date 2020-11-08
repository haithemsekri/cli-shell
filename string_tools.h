#ifndef __STRING_TOOLS_INCLUDES__
#define __STRING_TOOLS_INCLUDES__ "5ec1b937-fccf-4b98-b9bb-2d7389b3ad8a"

#include <cstdbool>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "defines.h"

typedef enum string_numeric_type {
  string_numeric_type_invalid,
  string_numeric_type_minus_double,
  string_numeric_type_plus_double,
  string_numeric_type_minus_int,
  string_numeric_type_plus_int,
  string_numeric_type_hex_int
} string_numeric_type;

string_numeric_type get_string_numeric_type(const char * s);

bool string_is_numeric(const char * s);
bool string_to_int(const char * s, int * p_val);
bool string_to_double(const char * s, double * p_val);
int string_to_on_off_int(const char * s);
int string_to_enable_disable_int(const char * s);
int str_to_yes_no_int(const char * s);
int addr_ptr_diff(const void * s, const void * e);


int string_find_delimiter(const char * s, uint e, uint b, char c1, char c2, int * p_len);
int string_replace_char(char * s, uint b, uint l, char c1, char c2);
int string_trim_char(char * s, uint e, char c);
std::string wchar_to_string(std::vector<wchar_t>& v);
std::vector<wchar_t> string_to_wchar(const char * s);
int string_args_tokenize(char * s, uint l, char * argv[], uint argv_l, char sep);

#endif //__STRING_TOOLS_INCLUDES__
