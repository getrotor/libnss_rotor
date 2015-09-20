/* UDP client for Rotor */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define BUFLEN 128
#define TIMEOUT 1 /* Timeout in secs */
#define PORT 6789
#define HOSTADDR "127.0.0.1"


int
get_real(char *name, char *result)
{
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    return(-1);



  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = inet_addr(HOSTADDR);

  if (sendto(sockfd, name, strlen(name), 0,
             (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return(-1);

  struct timeval tv = {TIMEOUT,0};
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    return(-1);

  char buf[BUFLEN];
  int n;
  if((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
    return(-1);
  } else {
    memcpy(result, buf, n);
    return(0);
  }
}
