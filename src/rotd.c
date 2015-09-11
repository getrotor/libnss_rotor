/* NSS Module for rotd */
#include <stdio.h>
#include <nss.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

/*
NOTE(varoun): Look at include/netdb.h in the glibc source for the
NSS Prototypes.
 */

enum nss_status
_nss_rotd_sethostent (int flag) {
  printf("@ %s\n", __FUNCTION__);
  return NSS_STATUS_SUCCESS;
};

enum nss_status
_nss_rotd_endhostent (void) {
  printf("@ %s\n", __FUNCTION__);
  return NSS_STATUS_SUCCESS;
};

enum nss_status
_nss_rotd_gethostent_r (struct hostent *host,
                        char *buffer, size_t buflen,
                        int *errnop, int *h_errnop) {
  printf("@ %s\n", __FUNCTION__);
  return NSS_STATUS_NOTFOUND;
};

enum nss_status
_nss_rotd_gethostbyname2_r (const char *name, int af, struct hostent *host,
                            char *buffer, size_t buflen,
                            int *errnop, int *h_errnop) {
  printf("@ %s\n", __FUNCTION__);
  printf("ARG(name) -> %s\n", name);
  printf("ARG(buflen) -> %d\n", buflen);

  if (strcmp(name, "nsstest.varoun.com") == 0)
  {
    if (af = AF_INET) {
      printf("Custom lookup\n");

      strcpy(buffer, "nsstest.varoun.com");
      host->h_name = buffer;
      host->h_aliases = NULL;
      host->h_addrtype = AF_INET;
      host->h_length = 4;

      char *hostaddrp;
      hostaddrp = buffer + 256;
      int hostaddr = inet_addr("1.2.3.4");
      char *offset = memcpy(hostaddrp, &hostaddr, 4);
      host->h_addr_list[0] = NULL;
      return NSS_STATUS_SUCCESS;
    }
  }


  return NSS_STATUS_NOTFOUND;
};


enum nss_status
_nss_rotd_gethostbyname_r (const char *name, struct hostent *host,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop) {
  printf("@ %s\n", __FUNCTION__);
  return NSS_STATUS_NOTFOUND;
};

enum nss_status
_nss_rotd_gethostbyaddr_r (const void *addr, socklen_t addrlen, int af,	struct hostent *host,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop) {
  printf("@ %s\n", __FUNCTION__);
  return NSS_STATUS_NOTFOUND;
};
