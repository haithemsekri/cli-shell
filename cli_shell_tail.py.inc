
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
