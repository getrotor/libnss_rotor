/* NSS Module for rotd */
#include <stdio.h>
#include <nss.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define ALIGN(a) (((a+sizeof(void*)-1)/sizeof(void*))*sizeof(void*))
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

  size_t l, idx, alen;
  char *r_addr, *r_name, *r_aliases, *r_addr_list;

  if (strcmp(name, "nsstest.varoun.com") == 0)
  {
    if (af = AF_INET) {
      printf("Custom lookup\n");

      /* First fill in the hostname */
      l = strlen(name);
      r_name = buffer;
      memcpy(r_name, name, l+1);
      idx = ALIGN(l+1);

      /* Second, create  an empty aliases array */
      r_aliases = buffer + idx;
      *(char**) r_aliases = NULL;
      idx += sizeof(char*);

      /* Third, add the address */
      in_addr_t addr = inet_addr("1.2.3.4");
      alen = 4; /* 4 for AF_INET */

      memcpy(r_addr, &addr, alen);
        idx += ALIGN(alen);

      /* Fourth, add address pointer array */
      r_addr_list = buffer + idx;
      ((char**) r_addr_list)[0] = r_addr;
      ((char**) r_addr_list)[1] = NULL;


      host->h_name = r_name;
      host->h_aliases = (char**) r_aliases;
      host->h_addrtype = af;
      host->h_length = alen;
      host->h_addr_list = (char**) r_addr_list;

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
