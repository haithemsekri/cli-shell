#include <stdio.h>
#include <stdlib.h>

//#define ASSERT_TO_FAILURE
//#define DEV_TESTS

#define SIZEOF_ARRAY(a) ((unsigned)(sizeof(a) / sizeof(a[0])))

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#ifdef ASSERT_TO_FAILURE
  #define EXCEPTION() goto FAIL
  #define FAILURE(expr) FAIL: expr
#else
  #define EXCEPTION() abort()
  #define FAILURE(expr)
#endif

#define ASSERT_FMT(cond, ...) \
  do { if(!(cond)) { \
    fprintf (stderr, AT ": ASSERT: (" #cond "): " __VA_ARGS__); \
    EXCEPTION(); \
  }} while(0)

#define FATAL_FMT(...) \
  do { \
    fprintf (stderr, AT ": FATAL: " __VA_ARGS__); \
    EXCEPTION(); \
  } while(0)

#define ASSERT(cond) ASSERT_FMT(cond, "Failed\n")

#define L_LOG(...)     fprintf (stderr, AT ": LOG: " __VA_ARGS__)

#define L_ERR(...)     fprintf (stderr, AT ": ERR: " __VA_ARGS__)

#define L_FATAL(...)   FATAL_FMT (__VA_ARGS__)


#ifdef DEV_TESTS
#include <iostream>

#define TEST_EQ(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue == rvalue)) { \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_EQ(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_EQ(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)

#define TEST_GT(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue > rvalue)) { \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_GT(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_GT(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)

#define TEST_GE(l, r) \
  do { \
    auto lvalue = l; \
    auto rvalue = r; \
    if(!(lvalue >= rvalue)) { \
      std::cout << "Failed " << __FILE__ << ": " << __LINE__ << " ::TEST_GE(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
      exit(1); \
    } else { \
      std::cout << "OK     " << __FILE__ << ": " << __LINE__ << " ::TEST_GE(" << #l \
        << " = " << lvalue << ", " << #r << " = " << rvalue << ")." << std::endl; \
  }} while(0)
#endif


size_t strncpy_s(const char * at, char * d, size_t m, const char * s, size_t n)
{
  size_t l;

  ASSERT(d);
  ASSERT(s);
  ASSERT(m);
  ASSERT(n);

  l = 0;
  while((l < n) && (l < m) && s[l]) {d[l] =  s[l]; l++;}
  d[m - 1] = 0;
  ASSERT_FMT((l < n) && (l < m) && !s[l], "String truncanated [%s]\n", at);
  return l;
  FAILURE(return -1;)
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>

//(errno == %s)." EOL, strerror(errno));

int open_unix_server (const char * path, int n)
{
  int sfd = -1;
  struct sockaddr_un addr;

  ASSERT(path && *path);

  ASSERT_FMT((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) > 0, "%s\n", strerror(errno));

  memset(&addr, 0x00, sizeof(addr));
  addr.sun_family = AF_UNIX;
  ASSERT((size_t) -1 != strncpy_s(AT, addr.sun_path, sizeof(addr.sun_path), path, sizeof(addr.sun_path)));

  unlink(path);

  ASSERT_FMT(0 == bind(sfd, (struct sockaddr*)&addr, sizeof(addr)), "%s\n", strerror(errno));
  ASSERT_FMT(0 == listen(sfd, n), "%s\n", strerror(errno));
  ASSERT_FMT(0 == chmod(path, 0666), "%s\n", strerror(errno));

  return sfd;

  FAILURE(
    if(sfd >= 0) {
      close(sfd);
    }
    return -1;
  );
}

int forward_data(int * ifds, int * ofds, size_t olen)
{
  char buff[1024];
  uint32_t i;
  ssize_t rb;
  ssize_t wb;

  ASSERT(ifds);
  ASSERT(ofds);
  ASSERT(olen);

  errno = 0;
  rb = read(*ifds, buff, sizeof(buff));
  if(rb <= 0) {
    L_LOG("read: (fd == %d) %s\n", *ifds, strerror(errno));
    close(*ifds);
    *ifds = -1;
  } else {
    for(i = 0; i < olen; i++) {
      if(ofds[i] > 0) {
        wb = write(ofds[i], buff, rb);
        if(wb <= 0) {
          L_LOG("write: (fd == %d) %s\n", ofds[i], strerror(errno));
          close(ofds[i]);
          ofds[i] = -1;
        }
      }
    }
   }

  return 0;
  FAILURE(return -1;);
}

int wait_fds_events(int * fds, size_t fds_len, fd_set * rfds)
{
  int nfds;
  int ret;

  ASSERT(fds);
  ASSERT(fds_len > 0);
  ASSERT(rfds);

  FD_ZERO(rfds);

  nfds = -1;
  for(size_t i = 0; i < fds_len; i++) {
    if (fds[i] > 0) {
      FD_SET(fds[i], rfds);
      if (fds[i] > nfds) {
        nfds = fds[i];
      }
    }
  }

  ASSERT(nfds > 0);

  do {
    errno = 0;
    ret = select(nfds + 1, rfds, NULL, NULL, NULL);
  } while((ret < 0) && (EINTR == errno)); /// Skip EINTR

  return ret;
  FAILURE(return -1;);
}


static struct sigaction sig_action;
static bool exit_now = false;
static void (*cb_sig) (void) = NULL;

static void sig_handler(int num, siginfo_t * info, void * ctx)
{
   (void) num;
   (void) info;
   (void) ctx;

    L_LOG("sig_handler (num == %d).\r\n", num);
    exit_now = true;
    if(cb_sig) cb_sig();
    exit(EXIT_SUCCESS);
}

int install_sig_handler(void (*cb) (void))
{
  exit_now = false;
  memset(&sig_action, 0, sizeof(sig_action));
  sig_action.sa_sigaction = sig_handler;
  sig_action.sa_flags = SA_SIGINFO;

  exit_now = false;
  cb_sig = cb;

  ASSERT_FMT(0 == sigaction(SIGINT,  &sig_action, NULL), "%s\n", strerror(errno));
  ASSERT_FMT(0 == sigaction(SIGTERM, &sig_action, NULL), "%s\n", strerror(errno));
  ASSERT_FMT(0 == sigaction(SIGQUIT, &sig_action, NULL), "%s\n", strerror(errno));
  ASSERT_FMT(0 == sigaction(SIGINT,  &sig_action, NULL), "%s\n", strerror(errno));
  //ASSERT_FMT(0 == sigaction(SIGKILL, &sig_action, NULL), "%s\n", strerror(errno));

  return 0;
  FAILURE(return -1;);
}
/*
int main(int argc, char ** argv)
{
  L_LOG("Hello\n");
  L_LOG("Hello <%s>\n", "Invalid args");
  L_ERR("Hello\n");
  L_ERR("Hello <%s>\n", "Invalid args");

  int fd = open_unix_server (argv[1], 2);
  ASSERT(fd > 0);*/
/*
  char b[1];
  char s[] = "end";
  size_t n = strncpy_s(AT, b, sizeof(b), s, sizeof(s));
  printf("n = %lx, b = <%s>\n", n, b);*/
  //ASSERT_FMT(5 == 10, "Invalid argument\n");
  /*
  L_FATAL("Hello\n");
  L_FATAL("Hello <%s>\n", "Invalid args");
  ASSERT(5 == 10);

*//*
  close(fd);
  unlink(argv[1]);
  return 0;
  FAILURE(return -1);
}*/



int fds[8];
int filedes_out[2] = { -1, -1};
int filedes_in[2] = { -1, -1};
pid_t pid;

void term(void);

int init(const char * ipc, const char * path, const char * name)
{
    for(size_t i = 0; i < SIZEOF_ARRAY(fds); i++) {
        fds[i] = -1;
    }

    ASSERT_FMT(0 == pipe(filedes_out), "%s\n", strerror(errno));
    ASSERT_FMT(0 == pipe(filedes_in), "%s\n", strerror(errno));

    ASSERT_FMT(-1 != (pid = fork()), "%s\n", strerror(errno));

    if (pid == 0) {
        L_LOG("child (pid == %d).\n", getpid());
        while ((dup2(filedes_out[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
        while ((dup2(filedes_in[0], STDIN_FILENO) == -1) && (errno == EINTR)) {}
        close(filedes_out[1]);
        close(filedes_out[0]);
        close(filedes_in[1]);
        close(filedes_in[0]);
        execl(path, name, (char*)0);
        L_LOG("child exit.\n");
        exit(EXIT_SUCCESS);
    }

    install_sig_handler(term);
    fds[0] =  open_unix_server(ipc, SIZEOF_ARRAY(fds) - 2);
    ASSERT(-1 != fds[0]);

    fds[1] = filedes_out[0];
    ASSERT(-1 != fds[1]);

    return 0;
    FAILURE(
      if(fds[0] > 0) {close(fds[0]);}
      if(fds[1] > 0) {close(fds[1]);}
      fds[0] = -1;
      fds[1] = -1;
      return -1;
    );
}

void term(void)
{
  fds[0] = -1;
  fds[1] = -1;
  for(size_t i = 0; i < SIZEOF_ARRAY(fds); i++) {
      if (fds[i] > 0) {
          close(fds[i]);
          fds[i] = -1;
      }
  }

  close(filedes_out[0]);
  close(filedes_in[0]);
  close(filedes_out[1]);
  close(filedes_in[1]);

  L_LOG("killing <%d>\n", pid);
  kill(pid, 15);
  L_LOG("exiting\n\n");
  exit(EXIT_SUCCESS);
}

void accept_client(fd_set * rfds)
{
  if((fds[0] >= 0) && FD_ISSET(fds[0], rfds)) {
      int fd = accept(fds[0], NULL,NULL);
      if(fd > 0) {
          size_t i;
          for(i = 2; (i < SIZEOF_ARRAY(fds)) && (fds[i] >= 0); i++);
          if(i >= SIZEOF_ARRAY(fds)) {
              L_ERR("out of range");
              close(fd);
          } else {
              L_LOG("add client to pos <%lu> <%d>\n", i, fd);
              fds[i] = fd;
          }
      }
  }
}

void forward_client_data(fd_set * rfds)
{
  for(size_t i = 2; i < SIZEOF_ARRAY(fds); i++) {
      if ((fds[i] >= 0) && FD_ISSET(fds[i], rfds)) {
          forward_data(&fds[i], &filedes_in[1],1);
      }
  }
}

void forward_process_data(fd_set * rfds)
{
  if ((filedes_out[0] >= 0) && FD_ISSET(filedes_out[0], rfds)) {
      forward_data(&filedes_out[0], &fds[2], SIZEOF_ARRAY(fds) - 2);
  }
}

// "/tmp/bluetoothctl.sock" "/usr/bin/bluetoothctl"  "bluetoothctl"
int main(int argc, char ** argv)
{
    if(argc != 4) {
        L_ERR("expected argument sock_path bin_path process_name.\r\n");
        exit(EXIT_SUCCESS);
    }
    L_LOG("parent (pid == %d).\r\n", getpid());

    init(argv[1], argv[2], argv[3]);

    while(!exit_now && (fds[0] >= 0) && (fds[0] >= 1)) {
        fd_set rfds;
        if(wait_fds_events(fds, SIZEOF_ARRAY(fds), &rfds) > 0) {
            accept_client(&rfds);
            forward_client_data(&rfds);
            forward_process_data(&rfds);
        } else {
          break;
        }
    }

    term();
    return 0;
}

