#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sysexits.h>
#include <err.h>
#include <sys/socket.h>

#define BUFLEN 128
#define TIMEOUT 20

void
sigalrm(int signo)
{
}

void
print_echo(const char *str, int sockfd)
{
  char buf[BUFLEN];
  int n;

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6789);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (sendto(sockfd, str, strlen(str), 0,
             (struct sockaddr *)&addr, sizeof(addr)) < 0)
    errx(EX_IOERR, "sendto error");

  alarm(TIMEOUT);
if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
    if (errno != EINTR)
      alarm(0);
    errx(EX_IOERR, "recv error");
  }

  alarm(0);
  write(STDOUT_FILENO, buf, n);
}

int
main(int argc, char *argv[])
{
  if (argc != 2)
    errx(EX_USAGE, "Usage: ./a.out <string>");

    int sockfd, err;
    struct sigaction sa;

    sa.sa_handler = sigalrm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) < 0)
      errx(EX_IOERR, "sigaction error");

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      err = errno;
    } else {
      print_echo(argv[1], sockfd);
      exit(0);
    }

    fprintf(stderr, "cant contact localhost echo server: %s",
            strerror(err));
    exit(1);
}

