
#include "defines.h"
#include "string_tools.h"

int main(void)
{
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type(NULL));
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type(""));
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type("invalid"));
  TEST_EQ(string_numeric_type_plus_int , get_string_numeric_type("0"));
  TEST_EQ(string_numeric_type_plus_int , get_string_numeric_type("10"));
  TEST_EQ(string_numeric_type_minus_int , get_string_numeric_type("-1"));
  TEST_EQ(string_numeric_type_minus_int , get_string_numeric_type("-10"));
  TEST_EQ(string_numeric_type_plus_double , get_string_numeric_type("0.1"));
  TEST_EQ(string_numeric_type_minus_double , get_string_numeric_type("-0.1"));
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type("-0.1A"));
  TEST_EQ(string_numeric_type_hex_int , get_string_numeric_type("0x1A"));
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type("0x.1A"));
  TEST_EQ(string_numeric_type_invalid , get_string_numeric_type("0x1W"));


  int i_val;

  TEST_EQ(false , string_to_int("", &i_val));
  TEST_EQ(false , string_to_int("-0.5", &i_val));
  TEST_EQ(true , string_to_int("-10", &i_val)); TEST_EQ(-10 , i_val);
  TEST_EQ(true , string_to_int("-1", &i_val)); TEST_EQ(-1 , i_val);
  TEST_EQ(true , string_to_int("-0", &i_val)); TEST_EQ(0 , i_val);
  TEST_EQ(true , string_to_int("0x00", &i_val)); TEST_EQ(0 , i_val);
  TEST_EQ(true , string_to_int("0xFAFA", &i_val)); TEST_EQ(0xFAFA , i_val);

  double d_val;
  TEST_EQ(false , string_to_double("", &d_val));
  TEST_EQ(true , string_to_double("-0", &d_val)); TEST_EQ(0 , d_val);
  TEST_EQ(true , string_to_double("-5", &d_val)); TEST_EQ(-5 , d_val);
  TEST_EQ(true , string_to_double("100", &d_val)); TEST_EQ(100 , d_val);
  TEST_EQ(true , string_to_double("-10.0", &d_val)); TEST_EQ(-10.0 , d_val);
  TEST_EQ(false , string_to_double("-1.", &d_val));
  TEST_EQ(false , string_to_double("-.1.", &d_val));
  TEST_EQ(true , string_to_double("-0.01", &d_val)); TEST_EQ(-0.01 , d_val);

  TEST_EQ(1 , string_to_on_off_int("on"));
  TEST_EQ(0 , string_to_on_off_int("off"));
  TEST_EQ(-1 , string_to_on_off_int("Off"));

  TEST_EQ(1 , string_to_enable_disable_int("enable"));
  TEST_EQ(0 , string_to_enable_disable_int("disable"));
  TEST_EQ(-1 , string_to_enable_disable_int("Enable"));

  TEST_EQ(1 , str_to_yes_no_int("yes"));
  TEST_EQ(0 , str_to_yes_no_int("no"));
  TEST_EQ(-1 , str_to_yes_no_int("No"));

  int argc;
  char * argv[32];

  {
    char str[] = " g++ string_tools.cc -o    /tmp/string_tools.cc.bin -Wall -Wextra -pedantic ";
    argc = string_args_tokenize(str, strlen(str), argv, SIZEOF_ARRAY(argv), '"');
    TEST_EQ(7 , argc);
    TEST_EQ(false, strcmp(argv[0], "g++"));
    TEST_EQ(false, strcmp(argv[1], "string_tools.cc"));
    TEST_EQ(false, strcmp(argv[2], "-o"));
    TEST_EQ(false, strcmp(argv[3], "/tmp/string_tools.cc.bin"));
    TEST_EQ(false, strcmp(argv[4], "-Wall"));
    TEST_EQ(false, strcmp(argv[5], "-Wextra"));
    TEST_EQ(false, strcmp(argv[6], "-pedantic"));
  }

  {
    char str[] = " g++ string_tools.cc -o    /tmp/string_tools.cc.bin -Wall -Wextra -pedantic ";
    argc = string_args_tokenize(str, strlen(str), argv, SIZEOF_ARRAY(argv), '"');
    TEST_EQ(7 , argc);
    TEST_EQ(false, strcmp(argv[0], "g++"));
    TEST_EQ(false, strcmp(argv[1], "string_tools.cc"));
    TEST_EQ(false, strcmp(argv[2], "-o"));
    TEST_EQ(false, strcmp(argv[3], "/tmp/string_tools.cc.bin"));
    TEST_EQ(false, strcmp(argv[4], "-Wall"));
    TEST_EQ(false, strcmp(argv[5], "-Wextra"));
    TEST_EQ(false, strcmp(argv[6], "-pedantic"));
  }

  {
    char str[] = " g++ string_tools.cc \"-o    /tmp/string_tools.cc.bin\" -Wall -Wextra -pedantic ";
    argc = string_args_tokenize(str, strlen(str), argv, SIZEOF_ARRAY(argv), '"');
    TEST_EQ(6 , argc);
    TEST_EQ(false, strcmp(argv[0], "g++"));
    TEST_EQ(false, strcmp(argv[1], "string_tools.cc"));
    TEST_EQ(false, strcmp(argv[2], "-o    /tmp/string_tools.cc.bin"));
    TEST_EQ(false, strcmp(argv[3], "-Wall"));
    TEST_EQ(false, strcmp(argv[4], "-Wextra"));
    TEST_EQ(false, strcmp(argv[5], "-pedantic"));
  }

  return 0;
}
