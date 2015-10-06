/* NSS Module for rotor */
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
_nss_rotor_gethostbyname4_r(const char *name,
                                 struct gaih_addrtuple **pat,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp) _public_;

enum nss_status
_nss_rotor_gethostbyname3_r(const char *name,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp,
                                 char **canonp) _public_;

enum nss_status
_nss_rotor_gethostbyname2_r(const char *name,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop) _public_;

enum nss_status
_nss_rotor_gethostbyname_r(const char *name,
                                struct hostent *host,
                                char *buffer, size_t buflen,
                                int *errnop, int *h_errnop) _public_;

enum nss_status
_nss_rotor_gethostbyaddr2_r(const void* addr, socklen_t len,
                                 int af,
                                 struct hostent *host,
                                 char *buffer, size_t buflen,
                                 int *errnop, int *h_errnop,
                                 int32_t *ttlp) _public_;

enum nss_status
_nss_rotor_gethostbyaddr_r(const void* addr, socklen_t len,
                                int af,
                                struct hostent *host,
                                char *buffer, size_t buflen,
                                int *errnop, int *h_errnop) _public_;

enum nss_status
_nss_rotor_gethostbyname4_r(const char *name,
                           struct gaih_addrtuple **pat,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop,
                           int32_t *ttlp)
{

  size_t l, idx, ms;
  char *r_name;
  struct gaih_addrtuple *r_tuple, *r_tuple_prev = NULL;

  /* Check to make sure the name is in our rotation. */
  /* TODO(varoun): Do a proper rotation check here */
  /* if (strcasecmp(name, "www.bsd.org") != 0) { */
  /*   *errnop = ENOENT; */
  /*   *h_errnop = HOST_NOT_FOUND; */
  /*   return NSS_STATUS_NOTFOUND; */
  /* } */

  l = strlen(name);
  ms = ALIGN(l+1)+ALIGN(sizeof(struct gaih_addrtuple));
  if (buflen < ms) {
    *errnop = ENOMEM;
    *h_errnop = NO_RECOVERY;
    return NSS_STATUS_TRYAGAIN;
  }

  /* First, Fill in the hostname */
  r_name = buffer;
  memcpy(r_name, name, l+1);
  idx = ALIGN(l+1);

  /* Make the query to rotor and get the result */
  char real[16];

  /* TODO(varoun): Why is this memset needed? Without it we seem to get
  ** incorrect results. Ex. 255.255.255.255 as the response!
  */

  memset(real, 0, sizeof(real));
  if (get_real(name, real)!= 0) {
   *errnop = ENOENT;
   *h_errnop = HOST_NOT_FOUND;
   return NSS_STATUS_NOTFOUND;
  }

  if((strcasecmp(real, "ns_unavail") == 0) ||
     (strcasecmp(real, "ns_tryagain") == 0)) {
    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
  }

  /* Fill in the address */
  in_addr_t ipv4 = inet_addr(real);
  r_tuple = (struct gaih_addrtuple*) (buffer + idx);
  r_tuple->next = r_tuple_prev;
  r_tuple->name = r_name;
  r_tuple->family = AF_INET;
  r_tuple->scopeid = 0;
  memcpy(r_tuple->addr, &ipv4, 4);
  r_tuple_prev = r_tuple;

  idx += ALIGN(sizeof(struct  gaih_addrtuple));

  /* Verify that size matches */
  assert(idx == ms);

  *pat = r_tuple_prev;

  if (ttlp)
    *ttlp = 0;

  return NSS_STATUS_SUCCESS;

  };


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
  /* Make the query to rotor and get the result */
  char real[16];

  /* TODO(varoun): Why is this memset needed? Without it we seem to get
  ** incorrect results. Ex. 255.255.255.255 as the response!
  */

  memset(real, 0, sizeof(real));
  if (get_real(name, real) != 0) {
    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
  }

  if((strcasecmp(real, "ns_unavail") == 0) ||
     (strcasecmp(real, "ns_tryagain") == 0)) {
    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
  }

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
_nss_rotor_gethostbyname3_r(const char *name,
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

 /* TODO(varoun): Check for valid rotations here ? */
  /* if (strcasecmp(name, "www.bsd.org") != 0) { */
  /*   *errnop = ENOENT; */
  /*   *h_errnop = HOST_NOT_FOUND; */
  /*   return NSS_STATUS_NOTFOUND; */
  /* } */

  return fill_in_hostent(name, af, host, buffer, buflen, errnop, h_errnop,
                         ttlp, canonp);
}

enum nss_status
_nss_rotor_gethostbyname2_r (const char *name, int af, struct hostent *host,
                            char *buffer, size_t buflen,
                            int *errnop, int *h_errnop) {

  return _nss_rotor_gethostbyname3_r(name, af, host,
                                    buffer, buflen,
                                    errnop, h_errnop,
                                    NULL, NULL);
};


enum nss_status
_nss_rotor_gethostbyname_r (const char *name, struct hostent *host,
                           char *buffer, size_t buflen,
                           int *errnop, int *h_errnop) {
  return _nss_rotor_gethostbyname3_r(name, AF_UNSPEC, host,
                                    buffer, buflen,
                                    errnop, h_errnop,
                                    NULL, NULL);
};
