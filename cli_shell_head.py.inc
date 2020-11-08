
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
