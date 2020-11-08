#include <sys/ioctl.h>
#include <assert.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>

#include "shell_cli_console.h"

#include "string_tools.h"

struct shell_command_t;
typedef std::vector<shell_command_t> shell_command_list_t;

typedef struct shell_command_t {
  const char * name;
  shell_command_callback_t cmd_cb;
} shell_command_t;

class ShellSession {

public:
  std::vector<char> m_tx_buf;
  std::vector<wchar_t> m_line;
  std::vector<std::vector<wchar_t>> m_history;
  int m_historyPos;
  int m_xPos;
  int m_xMaxPos;
  std::string m_prompt;
  std::string m_end_of_line;
  std::string m_clear_line_and_prompt;
  bool m_end_session;

  void * m_io_handle;
  static shell_command_list_t m_cmd_list;

  ShellSession(void * io_handle) {
    m_io_handle = io_handle;
    m_tx_buf.clear();
    m_line.clear();
    m_history.clear();
    m_historyPos = 0;
    m_xPos = 0;
    m_xMaxPos = 80;
    m_end_session = false;
    //m_prompt = "\033[35m$> \033[0m";
    m_prompt = "$> ";
    m_clear_line_and_prompt =  "\033[2K\r" + m_prompt;
    m_end_of_line =  "\r\n" ;
  }

  static void AddConsoleCommand(const char * name, shell_command_callback_t cmd_cb) {
    ASSERT(name && *name);
    ASSERT(cmd_cb);
    m_cmd_list.push_back({name, cmd_cb});
  }

  static shell_command_callback_t FindConsoleCommand(const char * name) {
    ASSERT(name);

    if(!*name) {
      return nullptr;
    }

    for(shell_command_t cmd : m_cmd_list) {
      if(!strcmp(name, cmd.name)) {
        return cmd.cmd_cb;
      }
    }
    return nullptr;
  }

  static shell_command_callback_t FindConsoleCommand(std::vector<wchar_t> line) {
    std::string name = wchar_to_string(line);
    return FindConsoleCommand(name.c_str());
  }

  static std::vector<const char *> AutoCompleteCommand(const char * name) {
    ASSERT(name);
    std::vector<const char *> ret{};

    for(shell_command_t cmd : m_cmd_list) {
      if(!*name) {
        ret.push_back(cmd.name);
      }
      else if(cmd.name == strstr(cmd.name, name)) {
        ret.push_back(cmd.name);
      }
    }

    return ret;
  }

  static std::vector<const char *> AutoCompleteCommand(std::vector<wchar_t> line) {
    std::string name = wchar_to_string(line);
    return AutoCompleteCommand(name.c_str());
  }

  void append_tx_buf(const char * buf, unsigned len) {
    if(buf && len) {
      for(unsigned i = 0; i < len; i++) {
        m_tx_buf.push_back(buf[i]);
      }
    }
  }

  void append_tx_buf(const char * s) {
    while(*s) {
      m_tx_buf.push_back(*s);
      s++;
    }
  }

  void append_tx_buf(std::string& s) {
    for(char c : s) {
      m_tx_buf.push_back(c);
    }
  }

  void fix_cursor_position(void) {
    if(m_xPos < 0) {
      m_xPos = 0;
    }

    if(m_xPos > m_line.size()) {
      m_xPos = m_line.size();
    }

    if(!m_line.size()) {
      m_xPos = 0;
    }
  }

  void append_tx_buf(wchar_t c) {
    uint8_t b0 = (c >> 0) & 0xFF;
    uint8_t b1 = (c >> 8) & 0xFF;
    uint8_t b2 = (c >> 16) & 0xFF;
    uint8_t b3 = (c >> 24) & 0xFF;

    if(b0) m_tx_buf.push_back(b0);
    if(b1) m_tx_buf.push_back(b1);
    if(b2) m_tx_buf.push_back(b2);
    if(b3) m_tx_buf.push_back(b3);
  }

  void append_tx_buf(std::vector<wchar_t>& line) {
    for(wchar_t c : line) {
      append_tx_buf(c);
    }
  }

  void append_tx_buf(char c) {
    m_tx_buf.push_back(c);
  }

  void append_mline_tx_data(unsigned b, unsigned e) {
    ASSERT(e <= m_line.size());
    //ASSERT(b <= e);
    for(int pos = b; pos < e; pos++) {
      append_tx_buf(m_line[pos]);
    }
  }

  void append_mline_tx_data(bool right) {
    int begin;
    int end;

    fix_cursor_position();

    if(right) {
      end = m_xPos + m_xMaxPos;
      if(end > m_line.size()) end = m_line.size();

      begin = end - m_xMaxPos;
      if(begin < 0) begin = 0;
    } else {
      begin = m_xPos - m_xMaxPos;
      if(begin < 0) begin = 0;

      end = begin + m_xMaxPos;
      if(end > m_line.size()) end = m_line.size();
    }

    append_tx_buf(m_clear_line_and_prompt);
    append_mline_tx_data(begin, end);
    if(m_xPos < end) {
      std::string goto_leftx = "\033[" + std::to_string(end - m_xPos) + "D";
      append_tx_buf(goto_leftx);
    }
  }

  void send_tx_buf(void) {
    if(m_tx_buf.size()) {
      cli_console_data_write(m_io_handle, m_tx_buf.data(), m_tx_buf.size());
    }
    m_tx_buf.clear();
  }

  void key_tab(void) {
    fix_cursor_position();

    std::vector<const char *> cmdList{};

    if(!m_line.size()) {
      cmdList = ShellSession::AutoCompleteCommand("\0");
      if(cmdList.size()) {
        append_tx_buf(m_end_of_line);
        for(const char * s : cmdList) {
          append_tx_buf(s);
          append_tx_buf(m_end_of_line);
        }
      }
      m_xPos = 0;
      append_mline_tx_data(true);
    } else {
      cmdList = ShellSession::AutoCompleteCommand(m_line);
      if(cmdList.size() == 1) {
        append_tx_buf(m_clear_line_and_prompt);
        append_tx_buf(cmdList.back());
        m_line = string_to_wchar(cmdList.back());
        m_xPos = m_line.size();
      } else if(cmdList.size() > 1) {
        m_xPos = m_line.size();
        append_mline_tx_data(true);
        append_tx_buf(m_end_of_line);
        for(const char * s : cmdList) {
          append_tx_buf(s);
          append_tx_buf(m_end_of_line);
        }
        append_mline_tx_data(true);
      }
    }

    send_tx_buf();
  }

  void key_up(void) {
    fix_cursor_position();
    if(!m_history.size()) { return ;}

    m_historyPos--;

    if(m_historyPos <= 0) {
      m_historyPos = 0;
    }

    if(m_historyPos >= m_history.size()) {
      m_historyPos = m_history.size() - 1;
    }

    m_line = m_history[m_historyPos];
    m_xPos = m_line.size();
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_down(void) {
    fix_cursor_position();
    if(!m_history.size()) { return ; }

    m_historyPos++;

    if(m_historyPos <= 0) {
      m_historyPos = 0;
    }

    if(m_historyPos >= m_history.size()) {
      m_historyPos = m_history.size() - 1;
    }

    m_line = m_history[m_historyPos];
    m_xPos = m_line.size();
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_back(void) {
    fix_cursor_position();
    if(m_xPos<= 0) { return ; }
    if(!m_line.size()) { return ; }

    m_line.erase(m_line.begin() + m_xPos - 1);
    m_xPos--;
    append_mline_tx_data(false);
    send_tx_buf();
  }

  void key_del() {
    fix_cursor_position();
    if(!m_line.size()) { return ; }
    if(m_xPos == m_line.size()) { return ;}

    m_line.erase(m_line.begin() + m_xPos);
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_right(void) {
    fix_cursor_position();
    if(m_xPos < m_line.size()) {
      m_xPos++;
      append_mline_tx_data(true);
      send_tx_buf();
    }
  }

  void key_left(void) {
    fix_cursor_position();
    if(m_xPos > 0) {
      m_xPos--;
      append_mline_tx_data(false);
      send_tx_buf();
    }
  }

  void key_ctle(void) {
    m_xPos= m_line.size();
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_ctla(void) {
    m_xPos= 0;
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_ctlu(void) {
    fix_cursor_position();

    if(m_xPos >= m_line.size()) {
      m_line.clear();
    } else {
      m_line.erase(m_line.begin(), m_line.begin() + m_xPos);
    }

    m_xPos= 0;
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_enter(void) {
    fix_cursor_position();

    append_tx_buf(m_end_of_line);
    send_tx_buf();

    if(m_line.size()) {
      std::string cmdStr = wchar_to_string(m_line);

      m_history.push_back(m_line);
      m_historyPos = m_history.size();
      m_line.clear();
      m_xPos = 0;

      char * pstLine = strdup(cmdStr.c_str());
      ASSERT(pstLine);

      char * argv[32];
      int argc = string_args_tokenize(pstLine, cmdStr.length(), argv, SIZEOF_ARRAY(argv), '"');

      shell_command_callback_t cb = ShellSession::FindConsoleCommand(argv[0]);
      if(!cb) {
        append_tx_buf("error: command not found: ");
        append_tx_buf(cmdStr);
        append_tx_buf(m_end_of_line);
      }
      else {
        cb(m_io_handle, argc, (const char**) argv);
      }

      free(pstLine);
    }

    append_tx_buf(m_prompt);
    send_tx_buf();
  }

  void key_quote(void) {
    char c = 0x27;
    append_tx_buf(c);

    m_xPos++;
    if((m_xPos + m_xMaxPos) >= m_line.size()) {
      append_mline_tx_data(true);
    } else {
      append_mline_tx_data(false);
    }
    send_tx_buf();
  }

  void key_handle(wchar_t key) {
    if(key == 0x0d) {
      key_enter();
      return ;
    }

    fix_cursor_position();

    if(m_xPos == m_line.size()) {
      m_line.push_back(key);
    }
    else {
      m_line.insert(m_line.begin() + m_xPos, key);
    }

    m_xPos++;

    if((m_xPos + m_xMaxPos) >= m_line.size()) {
      append_mline_tx_data(true);
    } else {
      append_mline_tx_data(false);
    }
    send_tx_buf();
  }

  void key_ctlc(void) {
    std::string out = m_end_of_line + "CTRL+C" + m_end_of_line;
    append_tx_buf(out);
    send_tx_buf();
    m_end_session = true;
  }

  void key_ctlx(void) {
    std::string out = m_end_of_line + "CTRL+X" + m_end_of_line;
    append_tx_buf(out);
    send_tx_buf();
    m_end_session = true;
  }

  void key_ctlz(void) {
    std::string out = m_end_of_line + "CTRL+Z" + m_end_of_line;
    append_tx_buf(out);
    send_tx_buf();
    m_end_session = true;
  }

  void key_ctld(void) {
    std::string out = m_end_of_line + "CTRL+D" + m_end_of_line;
    append_tx_buf(out);
    send_tx_buf();
    m_end_session = true;
  }

  void key_ctlb(void) {
    m_xPos= m_line.size() / 2;
    append_mline_tx_data(true);
    send_tx_buf();
  }

  void key_handle(wchar_t key, unsigned len) {
    if(!key || !len) {
      return;
    }

    //dprintf(STDOUT_FILENO, "0x%08x\n", key);
    switch (key)
    {
      case 0x0000000d: key_enter(); return;
      case 0x00000002: key_ctlb(); return;
      case 0x00000003: key_ctlc(); return;
      case 0x00000009: key_tab(); return;
      case 0x0000001a: key_ctlz(); return;
      case 0x00000004: key_ctld(); return;
      case 0x00000018: key_ctlx(); return;

      case 0x0000007f: key_back(); return;

      case 0x00415b1b: key_up(); return;
      case 0x00425b1b: key_down(); return;
      case 0x00435b1b: key_right(); return;
      case 0x00445b1b: key_left(); return;

      case 0x7e335b1b: key_del(); return;

      case 0x00000001: key_ctla(); return;
      case 0x00000005: key_ctle(); return;

      case 0x00000015: key_ctlu(); return;
      case 0x009980e2: key_quote(); return;

      default: break;
    }

    //return;

    uint8_t b0 = (key >> 0) & 0xFF;
    uint8_t b1 = (key >> 8) & 0xFF;
    uint8_t b2 = (key >> 16) & 0xFF;
    uint8_t b3 = (key >> 24) & 0xFF;

    if((b0 >= 128) || (b1 >= 128) || (b2 >= 128) || (b3 >= 128)) {
      key_handle(key);
    } else {

      //dprintf(STDOUT_FILENO, "b0,b1,b2,b3 0x%02x, 0x%02x, 0x%02x, 0x%02x,\n", b0, b1, b2, b3);
      if(b0) key_handle(b0);

      if(b1) key_handle(b1);

      if(b2) key_handle(b2);

      if(b3) key_handle(b3);
    }
  }

  static void shell_navigate(void * io_handle) {
    ShellSession session(io_handle);

    session.key_enter();

    while(!session.m_end_session) {
      wchar_t key = 0;
      int len = cli_console_data_read(io_handle, &key, sizeof(key));

      if(len <= 0) {
        break;
      }

      session.key_handle(key, len);
    }

    session.m_tx_buf.clear();
    session.m_line.clear();
    session.m_history.clear();
    cli_console_fmt_write (io_handle, "\r\nBye\r\n");
  }
};

shell_command_list_t ShellSession::m_cmd_list;

void cli_console_add_cmd_cb (const char * name, shell_command_callback_t cb)
{
  ASSERT(cb);
  ASSERT(name && *name);
  ShellSession::AddConsoleCommand(name, cb);
}

void cli_console_run (void * io_handle)
{
  ASSERT(io_handle);
  ShellSession::shell_navigate(io_handle);
}

void cli_console_term (void)
{
  ShellSession::m_cmd_list.clear();
}
