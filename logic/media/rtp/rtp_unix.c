/* rtp_unix.c: RTP API types, structures, and functions specific to the
				 Unix implementation of the library

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtp_unix.h"
#include "rtp_api.h"
#include "rtp_api_ext.h"

#include "sysdep.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <errno.h>


extern u_long md_32(char *string, int length);
socktype _sys_create_socket(int type) {
	int skt;

	skt = socket(AF_INET, type, 0);

	if(skt == -1)
		return(_SYS_INVALID_SOCKET);
	else
		return(skt);

}

int _sys_close_socket(socktype skt) {

	int err;
	err = close(skt);
	if(err == -1)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_connect_socket(socktype skt, struct sockaddr_in *sa) {

	int err;

	err = connect(skt,
		(struct sockaddr *) sa,
		sizeof(struct sockaddr_in));

	if(err == -1)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_set_ttl(socktype skt, int ttl) {

	int8 lttl;
	int nRet;

	lttl = (int8) ttl;
	
	nRet = setsockopt(skt,
				 IPPROTO_IP,
				 IP_MULTICAST_TTL,
				 (char*) &lttl,
				 sizeof(int8));

	if(nRet == -1)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_get_socket_name(socktype skt, struct sockaddr_in *sa) {

	int len;
	int ret;

	len = sizeof(struct sockaddr);
	ret = getsockname(skt, (struct sockaddr *) sa, &len);
	if(ret == -1) 
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_set_reuseaddr(socktype skt) {
	int err;
	int one;

	one = 1;
	err = setsockopt(skt,
			 SOL_SOCKET,
			 SO_REUSEADDR,
			 (char*) &one,
			 sizeof(one));

	if(err < 0)
		return(_SYS_SOCKET_ERROR);
	else return(0);
}

int _sys_set_reuseport(socktype skt) {
	
	int err,one;

	one = 1;
#ifdef SO_REUSEPORT
	err = setsockopt(skt,
			 SOL_SOCKET,
			 SO_REUSEPORT,
			 (char*) &one,
			 sizeof(one));
#else
	err = 0;
#endif

	if(err < 0)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);

}

int _sys_bind(socktype skt, struct sockaddr_in *sa) 
{

	int nRet;

	nRet = bind (skt,
		(struct sockaddr *) sa,
		sizeof(struct sockaddr));
	if(nRet == -1) 
	{
		nRet = errno;
		if(nRet == EADDRINUSE)
			return(_SYS_SOCKET_ADDRINUSE);
		else if(nRet == EADDRNOTAVAIL)
			return(_SYS_SOCKET_ADDRNOTAVAIL);
		else return(_SYS_SOCKET_ERROR);
	}
	else
		return(0);

}

int _sys_join_mcast_group(socktype rtpskt, struct sockaddr_in *sa) 
{
	struct ip_mreq mreq;
	int err;

	mreq.imr_multiaddr = sa->sin_addr;
	mreq.imr_interface.s_addr = INADDR_ANY;
		
	err = setsockopt(rtpskt,
			 IPPROTO_IP,
			 IP_ADD_MEMBERSHIP,
			 (char*) &mreq,
			 sizeof(struct ip_mreq));


	
	if(err < 0)
		return(_SYS_SOCKET_ERROR);
	else return(0);
}


int _sys_sendmsg(socktype s, struct msghdr *m, int f) {
	int count;

	count = sendmsg(s, m, f);
	return(count);
}

int _sys_sendto(socktype s, char *buf, int len, int flags, struct sockaddr *to)
{
#if 0
	struct sockaddr_in * destAddress = to;;
	printf("***sendto:%x:%d len:%d\n", destAddress->sin_addr.s_addr, htons(destAddress->sin_port), len);
#endif	
	return sendto(s, buf, len, flags, to, sizeof(struct sockaddr_in));		
}	

int _sys_send(socktype skt, char *buf, int buflen, int flags) {
	int res;

	res = send(skt, buf, buflen, flags);
	return(res);
}

int _sys_recvfrom(socktype skt, char *buf, int len, int flags, struct sockaddr *from, int *alen) {

	int res;

	res = recvfrom(skt,
		buf,
		len,
		flags,
		from,
		alen);

	return(res);
}

/* Determine if an IPv4 address is multicast.
	 Unlike IN_MULTICAST, take a struct in_addr in network byte order. */

int IsMulticast(struct in_addr addr)
{
	unsigned int haddr;

	haddr = ntohl(addr.s_addr);
	if(((haddr >> 28) & 0xf) == 0xe)
		return(1);
	else
		return(0);
}

#ifdef _RTP_SEMI_RANDOM

u_int32 random32(int type)
{
	printf("rand()\n");
	return rand();
}

#else
u_int32 random32(int type)
{
	return random();
/*	
	struct {
		int		 type;
		struct	timeval tv;
		clock_t cpu;
		pid_t	 pid;
		u_long	hid;
		uid_t	 uid;
		gid_t	 gid;
		struct	utsname name;
	} s;
	gettimeofday(&s.tv, 0); 
	uname(&s.name); 
	s.type = type;
	s.cpu = clock(); 
	s.pid = getpid();
	printf("gethostid\n");
	s.hid = gethostid(); 
	printf("gethostid\n");
	s.uid = getuid();
	s.gid = getgid(); 
	return md_32((char *)&s, sizeof(s)); 
*/	
	
}															 /* random32 */
#endif
