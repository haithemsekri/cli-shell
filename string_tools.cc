
#include "string_tools.h"

bool char_is_digit(char c) {
  return (c >= '0') &&  (c <= '9');
}

bool string_is_digit(const char * s) {
  if(!s)
    return false;
  if(!*s)
    return false;

  while(*s) {
    if(!char_is_digit(*s++)) {
      return false;
    }
  }

  return true;
}

bool string_is_double(const char * s) {
  if(!s)
    return false;
  if(!*s)
    return false;

  unsigned points = 0;
  unsigned digits_right = 0;
  unsigned digits_left = 0;
  while(*s) {
    if(char_is_digit(*s)) {
      digits_left++;
      if(points) digits_right++;
    } else if(*s == '.') {
      if(!digits_left) {
        return false;
      }
      points++;
      if(points > 1) {
        return false;
      }
    } else {
      return false;
    }
    s++;
  }

  if(!digits_right) {
    return false;
  }

  return true;
}

bool char_is_alphanum(char c) {
  return char_is_digit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') &&  (c <= 'f'));
}

bool string_is_alphanum(const char * s) {
  if(!s)
    return false;
  if(!*s)
    return false;

  while(*s) {
    if(!char_is_alphanum(*s++)) {
      return false;
    }
  }

  return true;
}

string_numeric_type get_string_numeric_type(const char * s)
{
  if(!s)
    return string_numeric_type_invalid;
  if(!*s)
    return string_numeric_type_invalid;

  if(s[0] == '-') {
    if(string_is_digit(s + 1)) {
      return string_numeric_type_minus_int;
    } else if(string_is_double(s + 1)) {
      return string_numeric_type_minus_double;
    } else {
      return string_numeric_type_invalid;
    }
  } else if((s[0] == '0') && (s[1] == 'x')) {
    if(string_is_alphanum(s + 2)) {
      return string_numeric_type_hex_int;
    } else {
      return string_numeric_type_invalid;
    }
  } else {
    if(string_is_digit(s)) {
      return string_numeric_type_plus_int;
    } else if(string_is_double(s)) {
      return string_numeric_type_plus_double;
    } else {
      return string_numeric_type_invalid;
    }
  }

  return string_numeric_type_invalid;
}

bool string_is_numeric(const char * s)
{
  return get_string_numeric_type(s) != string_numeric_type_invalid;
}

bool string_to_int(const char * s, int * p_val)
{
  string_numeric_type ret;

  ASSERT(s);
  ASSERT(p_val);

  ret = get_string_numeric_type(s);
  if(ret == string_numeric_type_minus_int) {
    *p_val = strtol(s, NULL, 10);
    return true;
  } else if(ret == string_numeric_type_plus_int) {
    *p_val = strtoul(s, NULL, 10);
    return true;
  } else if(ret == string_numeric_type_hex_int) {
    *p_val = strtoul(s, NULL, 16);
    return true;
  } else {
    return false;
  }

  FAILURE(return false);
}

bool string_to_double(const char * s, double * p_val)
{
  string_numeric_type ret;
  ASSERT(s);
  ASSERT(p_val);

  ret = get_string_numeric_type(s);
  if(ret == string_numeric_type_plus_double) {
    *p_val = atof(s);
    return true;
  } else if(ret == string_numeric_type_minus_double) {
    *p_val = atof(s);
    return true;
  } else if(ret == string_numeric_type_plus_int) {
    *p_val = atof(s);
    return true;
  } else if(ret == string_numeric_type_minus_int) {
    *p_val = atof(s);
    return true;
  } else {
    return false;
  }

  FAILURE(return false);
}

int string_to_on_off_int(const char * s)
{
  if(!s)
    return -1;
  if(!*s)
    return -1;

  return !strcmp(s, "on") ? 1 :
    !strcmp(s, "off") ? 0 : -1;
}

int string_to_enable_disable_int(const char * s)
{
  if(!s)
    return -1;
  if(!*s)
    return -1;

  return !strcmp(s, "enable") ? 1 :
    !strcmp(s, "disable") ? 0 : -1;
}

int str_to_yes_no_int(const char * s)
{
  if(!s)
    return -1;
  if(!*s)
    return -1;

  return !strcmp(s, "yes") ? 1 :
    !strcmp(s, "no") ? 0 : -1;
}

int addr_ptr_diff(const void * s, const void * e)
{
  ASSERT(s);
  ASSERT(e);
  ASSERT(e >= s);
  return ((const char *) e) - ((const char *) s);
  FAILURE(return -1);
}

int string_find_delimiter(const char * s, uint l, uint b, char c1, char c2, int * p_len)
{
  const char * p0;
  const char * p1;

  ASSERT(s);
  ASSERT(p_len);
  ASSERT(l);

  if(b >= l)
    return -1;

  p0 = strchr(&s[b], c1);
  if(!p0)
    return -1;

  p1 = strchr(p0 + 1, c2);
  if(!p1)
    return -1;

  *p_len = addr_ptr_diff(s, p1);

  return addr_ptr_diff(s, p0);
  FAILURE(return -1);
}

int string_replace_char(char * s, uint b, uint l, char c1, char c2)
{
  int count = 0;
  ASSERT(s);

  if(b > l)
    return 0;

  for(uint pos = b; (pos <= l) && s[pos]; pos++) {
    if(s[pos] == c1) {
      count++;
      s[pos] = c2;
    }
  }

  return count;
  FAILURE(return -1);
}

int string_trim_char(char * s, uint e, char c)
{
  ASSERT(s);
  ASSERT(e);

  while((e > 0) && (s[e - 1] == c)) s[--e] = 0;

  if(e > 0) {
    uint pos = 0;
    while((pos < e) && (s[pos] == c)) s[pos++] = 0;

    if(pos < e) {
      uint i;
      for(i = pos; i < e; i++) {
        s[i - pos] = s[i];
      }
      s[i - pos] = 0;
    }
  }

  return strlen(s);
  FAILURE(return -1);
}

int string_args_tokenize(char * s, uint l, char * argv[], uint argv_l, char sep)
{
  uint argc = 0;
  uint pos = 0;

  ASSERT(s);
  ASSERT(l);
  ASSERT(argv);
  ASSERT(argv_l);

  if(!*s)
    return 0;

  while(pos <= l) {
    int pos1 = -1;
    int pos0 = string_find_delimiter(s, l, pos, sep, sep, &pos1);
    if((pos0 == -1) || (pos1 == -1)) {
      string_replace_char(s, pos, l, ' ', 0);
      break;
    } else {
      string_replace_char(s, pos, pos0, ' ', 0);
      s[pos0] = 0;
      s[pos1] = 0;
      pos = pos1+ 1;
    }
  }

  pos = 0;
  while((pos <= l) && (argc < argv_l)) {
    if(s[pos]) {
      uint len = strlen(&s[pos]);
      int nb = string_trim_char(&s[pos], len, ' ');
      ASSERT(nb >= 0);
      if(nb) {
        argv[argc] = &s[pos];
        argc++;
      }
      pos = pos + len + 1;
    } else {
      pos++;
    }
  }

  return argc;
  FAILURE(return -1);
}

std::string wchar_to_string(std::vector<wchar_t>& v)
{
  std::string s{};

  for (wchar_t c : v) {
    u_char b0 = (c >> 0) & 0xFF;
    u_char b1 = (c >> 8) & 0xFF;
    u_char b2 = (c >> 16) & 0xFF;
    u_char b3 = (c >> 24) & 0xFF;

    if(b0) s.push_back(b0);
    if(b1) s.push_back(b1);
    if(b2) s.push_back(b2);
    if(b3) s.push_back(b3);
  }

  return s;
}

std::vector<wchar_t> string_to_wchar(const char * s)
{
  std::vector<wchar_t> v{};

  ASSERT(s);

  while(*s) {
    v.push_back(*s++);
  }
  return v;
  FAILURE(return v);
}
