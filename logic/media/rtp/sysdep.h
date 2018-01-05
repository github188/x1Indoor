/* System-dependent header files */

#ifndef _SYSDEP_H
#define _SYSDEP_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
 
#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
# include <memory.h>
#endif
#if HAVE_STDLIB_H
# include <stdlib.h>
#elif HAVE_MALLOC_H
# include <malloc.h>
/* Where does the rand48 family live? */
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <math.h>
#include <stdarg.h>

#ifdef __unix
#include <sys/socket.h>
/* endianness converters (ntohl, etc) */
#include <netinet/in.h>

#endif

#ifdef WIN32
#include <winsock2.h>
extern void gettimeofday(struct timeval *tp, void *unused);
double drand48();
void srand48(long sv);
#endif

#ifdef __GNUC__
#define ARG_UNUSED(t, v) t v __attribute__((unused))
#else
/* Define other compilers' warning-suppression code here */
#define ARG_UNUSED(t, v) t v
#endif

/* the library sets sockaddr.sa_family to _RTP_ADDRESS_NOT_YET_KNOWN
   if the relevant address is not yet known. This should be some
   address family value known to be invalid. */
#define _RTP_ADDRESS_NOT_YET_KNOWN (AF_MAX + 1)



#endif /* _SYSDEP_H */
