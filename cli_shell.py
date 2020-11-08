
from cmd import Cmd
import socket
import logging
import threading
import time
import signal
import threading
import time
import select
import socket
import logging
import threading
import time
import signal
import threading
import time
import select
import sys

argi = len(sys.argv) - 1

assert(argi == 2)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.close()
exit_now = False
ip_connection_ok = False
address = sys.argv[1] #'127.0.0.1'
port = int(sys.argv[2]) #50008

def print_rx_data_thread():
  global sock
  global exit_now
  global address
  global port
  global ip_connection_ok

  while not exit_now:
    if ip_connection_ok:
      ready = select.select([sock], [], [], 0.1)
      if ready[0]:
        try:
          data = sock.recv(1024)
          if not data:
            print('Empty data. closing client.')
            sock.close()
            ip_connection_ok = False
          else:
            out = '%s$> ' % data
            sys.stdout.write(out)
            sys.stdout.flush()
        except Exception as e:
          print("recv failed. Exception is %s" %  e)
          ip_connection_ok = False
          sock.close()
    else:
      try:
        print("\r\nTry to connect to %s:%d" % (address, port))
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((address, port))
        ip_connection_ok = True
        print("Connection to %s:%d is Ok" % (address, port))
      except Exception as e:
        ip_connection_ok = False
        i = 0
        while i < 20:
          if exit_now:
            print("\r\nRX data thread break")
            sys.stdout.flush()
            break
          i += 1
          time.sleep(0.1)

  exit_now = True
  if ip_connection_ok:
    ip_connection_ok = False
    sock.close()
  print("\r\nRX data thread exit")
  sys.stdout.flush()


def CliSendIp(id, cmd, args, input):
  if ip_connection_ok:
    buff = '<id="0x%x" name="%s" arglist="%s" args="%s">' % (id, cmd, args, input)
    sock.send(buff.format().strip('\n').strip(' '))
  else:
    print("cli server not connected")

class CliShellPrompt(Cmd):
    prompt = '$> '
    intro = "Welcome! Type ? or press tab to list commands"

    def emptyline(self):
        return False

    def do_exit(self, inp):
        global exit_now
        global ip_connection_ok
        exit_now = True
        task.join()
        if ip_connection_ok:
          ip_connection_ok = False
          sock.close()
        print("Cli exit")
        return True

    def do_modem_status(self, inp):
      CliSendIp(0x0b5151cd, "modem_status", "", "{}".format(inp))

    def do_modem_com_enable_async_rx_print(self, inp):
      CliSendIp(0x0bb8a2a1, "modem_com_enable_async_rx_print", "", "{}".format(inp))

    def do_modem_com_disable_async_rx_print(self, inp):
      CliSendIp(0x151b0c4d, "modem_com_disable_async_rx_print", "", "{}".format(inp))

    def do_log_list_loggers(self, inp):
      CliSendIp(0x170aa9d3, "log_list_loggers", "", "{}".format(inp))

    def do_modem_com_read_rx_data(self, inp):
      CliSendIp(0x276df8fd, "modem_com_read_rx_data", "", "{}".format(inp))

    def do_modem_hw_reset(self, inp):
      CliSendIp(0x2b17a495, "modem_hw_reset", "", "{}".format(inp))

    def do_modem_sms_delete(self, inp):
      CliSendIp(0x3598398c, "modem_sms_delete", "u:uId", "{}".format(inp))

    def do_log_status(self, inp):
      CliSendIp(0x362d73ec, "log_status", "", "{}".format(inp))

    def do_modem_sms_send(self, inp):
      CliSendIp(0x3ff673f2, "modem_sms_send", "s:sNumber s:sData", "{}".format(inp))

    def do_modem_hw_power_off(self, inp):
      CliSendIp(0x47c38d54, "modem_hw_power_off", "", "{}".format(inp))

    def do_modem_sms_status(self, inp):
      CliSendIp(0x61d0d0e1, "modem_sms_status", "", "{}".format(inp))

    def do_log_set_verbosity(self, inp):
      CliSendIp(0x66502d4d, "log_set_verbosity", "s:sVerbosity", "{}".format(inp))

    def do_modem_sms_read(self, inp):
      CliSendIp(0x7d3537d2, "modem_sms_read", "u:uId", "{}".format(inp))

    def do_modem_sms_list(self, inp):
      CliSendIp(0x8695ac23, "modem_sms_list", "", "{}".format(inp))

    def do_modem_hw_status(self, inp):
      CliSendIp(0x98e202f2, "modem_hw_status", "", "{}".format(inp))

    def do_modem_call_status(self, inp):
      CliSendIp(0xa39e4493, "modem_call_status", "", "{}".format(inp))

    def do_modem_call_dial(self, inp):
      CliSendIp(0xb868e062, "modem_call_dial", "s:sNumber", "{}".format(inp))

    def do_modem_com_write_tx_data(self, inp):
      CliSendIp(0xd0b1b299, "modem_com_write_tx_data", "s:sPort s:sData", "{}".format(inp))

    def do_cli_ping(self, inp):
      CliSendIp(0xd26fa6b4, "cli_ping", "", "{}".format(inp))

    def do_modem_call_hang(self, inp):
      CliSendIp(0xd3f2c3c1, "modem_call_hang", "", "{}".format(inp))

    def do_modem_hw_power_on(self, inp):
      CliSendIp(0xdddb4f3b, "modem_hw_power_on", "", "{}".format(inp))

    def default(self, inp):
      print('Invalid command <%s>\r\n%s' % ("{}".format(inp), self.prompt))

    do_EOF = do_exit

if __name__ == '__main__':
    def signal_handler(sig, frame):
      global exit_now
      exit_now = True
    signal.signal(signal.SIGINT, signal_handler)
    task = threading.Thread(target=print_rx_data_thread)
    task.start()
    time.sleep(0.1)
    CliShellPrompt().cmdloop()
