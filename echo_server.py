# Echo server program
import socket

#!/usr/bin/env python2

import socket
import logging
import threading
import time
import signal
import threading
import time
import select


stop_threads = False

def main_func():
  def signal_handler(sig, frame):
    global stop_threads
    stop_threads = True

  global stop_threads
  address = '127.0.0.1'
  port = 50008

  signal.signal(signal.SIGINT, signal_handler)
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  try:
    print('Server setup')
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((address, port))
    sock.listen(1)

    while not stop_threads:
      try:
        conn, addr = sock.accept()
        print 'Connected by', addr
        while not stop_threads:
          try:
            data = conn.recv(1024)
            if not data: break
            out = '\r%s\r\nOK\r\n' % data
            conn.sendall(out)
            print("recv-send %s" %  data)
          except Exception as e:
            print("Failed to send back data. Exception is %s" %  e)
            break
        print 'Closing client', addr
        conn.close()
      except Exception as e:
        print("Failed to accept. Exception is %s" % e)


  except Exception as e:
    print("Failed to setup server %s:%d. Exception is %s" % (address, port, e))

  print('main exiting.')
  sock.close()


if __name__ == '__main__':
    main_func()
