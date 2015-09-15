/* NSS Module for rotd */
#include <stdio.h>
#include <nss.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define _public_ __attribute__ ((visibility("default")))
#define _hidden_ __attribute__ ((visibility("hidden")))

static inline size_t PROTO_ADDRESS_SIZE(int proto) {
  assert(proto == AF_INET || proto == AF_INET6);
  return proto == AF_INET6 ? 16 : 4;
}


#define ALIGN(a) (((a+sizeof(void*)-1)/sizeof(void*))*sizeof(void*))


/*
NOTE(varoun): Look at include/netdb.h in the glibc source for the
NSS Prototypes.
*/

enum nss_status
_nss_rotd_gethostbyname4_r(const char *name,
                                 struct gaih_addrtuple **pat,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp) _public_;

enum nss_status
_nss_rotd_gethostbyname3_r(const char *name,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp,
                                 char **canonp) _public_;

enum nss_status
_nss_rotd_gethostbyname2_r(const char *name,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop) _public_;

enum nss_status
_nss_rotd_gethostbyname_r(const char *name,
                                struct hostent *host,
                                char *buffer, size_t buflen,
                                int *errnop, int *h_errnop) _public_;

enum nss_status
_nss_rotd_gethostbyaddr2_r(const void* addr, socklen_t len,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp) _public_;

enum nss_status
_nss_rotd_gethostbyaddr_r(const void* addr, socklen_t len,
                                int af,
                                struct hostent *host,
                                char *buffer, size_t buflen,
                                int *errnop, int *h_errnop) _public_;



static enum nss_status
fill_in_hostent(const char *name, int af,
                struct hostent *result,
                char *buffer, size_t buflen,
                int *errnop, int *h_errnop,
                int32_t *ttlp,
                char **canonp) {

  size_t l, idx, ms;
  char *r_addr, *r_name, *r_aliases, *r_addr_list;

  size_t alen = PROTO_ADDRESS_SIZE(af);

  /* Make the query to rotor and get the result */
  char real[16];
  get_real(name, real);

  l = strlen(name);
  ms = ALIGN(l+1) +
    sizeof(char*)+
    ALIGN(alen)+
    2*sizeof(char*);

  if (buflen < ms) {
    *errnop = ENOMEM;
    *h_errnop = NO_RECOVERY;
    return NSS_STATUS_TRYAGAIN;
  }

  /* First fill in the hostname */
  r_name = buffer;
  memcpy(r_name, name, l+1);
  idx = ALIGN(l+1);

  /* Second - Create an empty aliases array */
  r_aliases = buffer + idx;
  *(char**) r_aliases = NULL;
  idx += sizeof(char*);

  /* Third - add the addresses */
  r_addr = buffer + idx;
  in_addr_t addr = inet_addr(real);
  memcpy(r_addr, &addr, alen);
  idx += ALIGN(alen);

  /* Fourth - Add address pointer array */

  r_addr_list = buffer + idx;
  ((char**) r_addr_list)[0] = r_addr;
  ((char**) r_addr_list)[1] = NULL;
  idx += 2*sizeof(char*);

  /* Verify that the size matches */
  assert(idx = ms);

  result->h_name = r_name;
  result->h_aliases = (char**) r_aliases;
  result->h_addrtype = af;
  result->h_length = alen;
  result->h_addr_list = (char**) r_addr_list;

  if (ttlp)
    *ttlp = 0;

  if (canonp)
    *canonp = r_name;

  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_rotd_gethostbyname3_r(const char *name,
                           int af,
                           struct hostent *host,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop,
                           int32_t *ttlp,
                           char **canonp) {
  if (af == AF_UNSPEC)
    af = AF_INET;

  if (af != AF_INET) {
    *errnop = EAFNOSUPPORT;
    *h_errnop = NO_DATA;
    return NSS_STATUS_UNAVAIL;
  }

  if (strcasecmp(name, "www.bsd.org") != 0) {
    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
  }

  return fill_in_hostent(name, af, host, buffer, buflen, errnop, h_errnop,
                         ttlp, canonp);
}





enum nss_status
_nss_rotd_gethostbyname2_r (const char *name, int af, struct hostent *host,
                            char *buffer, size_t buflen,
                            int *errnop, int *h_errnop) {

  size_t l, idx, alen;
  char *r_addr, *r_name, *r_aliases, *r_addr_list;

  if (af = AF_INET) {

    /* Step Zero - Make a query to rotor */
    char result[16];
    get_real(name, result);

    if (strcmp(result, "ns_unavail") == 0) {
      return NSS_STATUS_NOTFOUND;
    } else if (strcmp(result, "ns_tryagain") == 0) {
      return NSS_STATUS_NOTFOUND;
    } else {

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
      r_addr = buffer + idx;
      alen = 4; /* 4 for AF_INET */
      in_addr_t addr = inet_addr(result);
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
  return NSS_STATUS_NOTFOUND;
};

enum nss_status
_nss_rotd_gethostbyaddr_r (const void *addr, socklen_t addrlen, int af,	struct hostent *host,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop) {
  return NSS_STATUS_NOTFOUND;
};
