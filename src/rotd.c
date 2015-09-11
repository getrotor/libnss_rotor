/* NSS Module for rotd */
#include <stdio.h>
#include <nss.h>
#include <netdb.h>

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
