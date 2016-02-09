/*
 * %CopyrightBegin%
 *
 * Copyright (c) 2015-2016, Varoun. P <contact@varoun.com>.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * %CopyrightEnd%
 *
 */

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

#define BUFLEN 16
#define TIMEOUT 1

void
sigalrm(int signo)
{
}

int
make_request(int sockfd, const char *name, char *result)
{
  char buf[BUFLEN];
  int n;

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6789);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (sendto(sockfd, name, strlen(name), 0,
             (struct sockaddr *)&addr, sizeof(addr)) < 0)
    errx(EX_IOERR, "sendto error");

  alarm(TIMEOUT);
if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
    if (errno != EINTR)
      alarm(0);
    return(-1);
  }

  alarm(0);
  memcpy(result, buf, n);
  return(0);
}

int
get_real(char *name, char *result)
{

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
      if (make_request(sockfd, name, result) != 0)
        return(-1);
      return(0);
    }

    fprintf(stderr, "cant contact localhost rotdserver: %s",
            strerror(err));
    return(-1);
}
