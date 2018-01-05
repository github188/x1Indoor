/* rtp_unix.c: RTP API types, structures, and functions specific to the
				 Windows implementation of the library

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


#ifdef __unix
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>	/* MAXHOSTNAMELEN */
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/uio.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <sys/time.h>
#endif

#ifdef WIN32
#include <time.h>
#include <winsock2.h>
#endif


#include "rtp_api.h"
#include "rtp_api_ext.h"

#include "rtp_lowlevel.h"
#include "rtp_highlevel.h"
#include "rtp_highlevel_internal.h"
#include "sysdep.h"

#include "global.h"			/* from RFC 1321 */
#include "md5.h"				 /* from RFC 1321 */

extern int IsMulticast(struct in_addr addr);
static void hl_changed_sockaddr_callback(context cid,
						 person p,
						 struct sockaddr *sa,
						 int is_rtcp);


/* High-level API functions */


rtperror RTPCreate(context *the_context)
{
	rtperror err;

	hl_context *uc;

	uc = (hl_context *) malloc(sizeof(hl_context));
	if (uc == NULL)
		return errordebug(RTP_CANT_ALLOC_MEM, "RTPCreate",
					"out of memory\n");
	err = RTPLowLevelCreate(the_context);
	if (err != RTP_OK)
		goto bailout;

	err = RTPSessionSetHighLevelInfo(*the_context, (void*)uc);
	if (err != RTP_OK)
		goto bailout;

	uc->connection_opened = FALSE;

	uc->send_addr_list = NULL;
	uc->recv_addr_list = NULL;

	uc->rtp_sourceaddr.sin_family = AF_UNSPEC;
	uc->rtcp_sourceaddr.sin_family = AF_UNSPEC;

	uc->use_encryption = _RTP_DEFAULT_ENCRYPTION;
	uc->key = NULL;

	uc->encrypt_initfunc = NULL;
	uc->encrypt_encryptfunc = NULL;
	uc->encrypt_decryptfunc = NULL;

	uc->PreventEntryIntoFlaggingFunctions = FALSE;
	uc->SendErrorCallBack = NULL;
	uc->ChangedMemberAddressCallBack = NULL;
	uc->SendFlag = 0;
	err = RTPSetChangedMemberSockaddrCallBack(*the_context,
							&hl_changed_sockaddr_callback);
	if (err != RTP_OK)
		goto bailout;

	return RTP_OK;

bailout:
	free(uc);
	return err;
}

rtperror RTPCreateExt(context *the_context)
{
	rtperror err = RTPCreate(the_context);
	if (RTP_OK!=err)
		return err;
	//置标志
	hl_context *uc;
	err = RTPSessionGetHighLevelInfo(*the_context, (void **)&uc);
	uc->SendFlag = 1;
	return RTP_OK;
}


	
rtperror RTPDestroy(context cid)
{
	rtperror err;
	address_holder_t *s, *t;

	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void **)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	/* Context exists.	Now check if connection is open.	If so,
		 close it. */
	if (uc->connection_opened){
		err = RTPCloseConnection(cid, NULL);
		if (err != RTP_OK){
			return err;
		}
	}
	
	/* Remove the receiver list (if it exists) */
	if (uc->recv_addr_list != NULL) {
		free(uc->recv_addr_list);
	}

	/* Remove the sender list (if it exists) */
	s = uc->send_addr_list;
	while (s != NULL) 
	{
		t = s->next;
		free(s);
		s = t;
	}

	free(uc);
	err = RTPLowLevelDestroy(cid);
	return err;
}

/* This function adds a destination for sending packets. They can be
	 either unicast or multicast. TTL has no meaning for unicast, and
	 may be given as any value. The library will set it to zero before storing
	 it anyway. The port is in host byte order.

	 The function also creates and connects the sockets for RTP and RTCP. 
	 If multicast, it also sets the ttl. The resulting sockets are stored
	 in the context. When it's time to send, the send function can be
	 used directly.

	 You should not call this function with zero port number. Send port numbers
	 should never be dynamic. */

int RTPSessionGetSendAddrCount(context cid)
{
	hl_context *uc;
	rtperror err;
	address_holder_t *holder;
	int count = 0;
	if (cid==-1)
		return 0;
	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return 0;

	holder = uc->send_addr_list;
	while(holder != NULL) 
	{
		if(!holder->deleteflag) 
			count++;
		holder = holder->next;
	}
	return count;
}


rtperror RTPSessionAddSendAddr(context cid, char *addr, u_int16 port, u_int8 ttl)
{
	address_holder_t *holder;
	struct sockaddr_in saddr;
	int len, nRet;
	struct in_addr translation;
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (port == 0) {
		return errordebug(RTP_BAD_PORT, "RTPSessionAddSendAddr",
					"Port number zero not allowed");
	}
	/* If the port is odd, assume it's the RTCP port */
	//nat映射地址无法保证获得偶数端口
#if 0
	if((port & 1) == 1)
		port--;
#endif
	if((holder = (address_holder_t *) malloc(sizeof(address_holder_t))) == 0) {
		return errordebug(RTP_CANT_ALLOC_MEM, "RTPSessionAddSendAddr",
					"Cannot allocate memory");
	}


	/* Translate address */
	translation = host2ip(addr);
	if(translation.s_addr == (u_int32) -1) 
	{
		free(holder);
		return errordebug(RTP_BAD_ADDR, "RTPSessionAddSendAddr",
					"Could not resolve address");
	}


	/* Write values of address, port to context */
	holder->address = translation;
	holder->port = htons(port);
	holder->deleteflag = FALSE;
	holder->ttl = 0;
	if(IsMulticast(translation)) holder->ttl = ttl;

	//都同时指向接收socket
	if (1==uc->SendFlag)
	{
		if (uc->connection_opened)
		{
			holder->rtpsocket = uc->recv_addr_list->rtpsocket;
			holder->rtcpsocket = uc->recv_addr_list->rtcpsocket;
		}	
	}
	else
	{
	
		/* Create the RTP and RTCP sockets for this sender */

		holder->rtpsocket = _sys_create_socket(SOCK_DGRAM);

		if (holder->rtpsocket == _SYS_INVALID_SOCKET)
		{
			free(holder);
			return errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
					"couldn't get RTP socket for context %d",	(int)cid);
		}
	

		holder->rtcpsocket = _sys_create_socket(SOCK_DGRAM);
		if (holder->rtcpsocket == _SYS_INVALID_SOCKET)
		{
			_sys_close_socket(holder->rtpsocket);
			free(holder);
			return errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
						"couldn't get RTCP socket for context %d",	(int)cid);
		}

		/* Connect them, first RTP socket */
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_addr = holder->address;
		saddr.sin_port = htons(port);

		if(_sys_connect_socket(holder->rtpsocket, &saddr) == _SYS_SOCKET_ERROR)
		{
			err = errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
				"couldn't connect RTP socket for context %d",	(int)cid);
			goto bailout;
		}
		/* Now RTCP socket */
		saddr.sin_port = htons(port+1);

		if(_sys_connect_socket(holder->rtcpsocket, &saddr) == _SYS_SOCKET_ERROR)
		{
			err = errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
			"couldn't connect RTCP socket for context %d",	(int)cid);
			goto bailout;
		}
	
		if(IsMulticast(holder->address)) 
		{
			/* Set multicast TTL if needed */
			nRet = _sys_set_ttl(holder->rtpsocket, ttl);
			if(nRet == _SYS_SOCKET_ERROR) 
			{
				err = errordebug(RTP_CANT_SET_SOCKOPT, "RTPSessionAddSendAddr",
					"couldn't set RTP TTL for context %d",	(int)cid);
				goto bailout;
			}
			nRet = _sys_set_ttl(holder->rtcpsocket, ttl);
		
			if(nRet == _SYS_SOCKET_ERROR) 
			{
				err = errordebug(RTP_CANT_SET_SOCKOPT, "RTPSessionAddSendAddr",
					"couldn't set RTCP TTL for context %d",	(int)cid);
				goto bailout;
			}

			/* Determine source addresses, for loopback detection */
			/* XXX: multiple multicast destinations might have different sources */

			len = sizeof(struct sockaddr_in);

			if(_sys_get_socket_name(holder->rtpsocket, &uc->rtp_sourceaddr) == _SYS_SOCKET_ERROR) 
			{
				err = errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
					"Couldn't get RTP source address for context %d", (int)cid);
				goto bailout;
			}
			if(_sys_get_socket_name(holder->rtcpsocket, &uc->rtcp_sourceaddr) == _SYS_SOCKET_ERROR) 
			{
				err = errordebug(RTP_CANT_GET_SOCKET, "RTPSessionAddSendAddr",
					"Couldn't get RTCP source address for context %d", (int)cid);
				goto bailout;
			}
		}
	}

	/* Add address to list */

	holder->next = uc->send_addr_list;
	uc->send_addr_list = holder;

	return RTP_OK;

	bailout:

	_sys_close_socket(holder->rtpsocket);
	_sys_close_socket(holder->rtcpsocket);
	free(holder);

	return err;
}

/* This function removes addresses from the send list. The port is in host
	 byte order. The address, port, and ttl must match exactly in order to
	 remove the element from the list.

	 The function will also close the associated sockets. */

rtperror RTPSessionRemoveSendAddr(context cid, char *addr, u_int16 port, u_int8 ttl) 
{
	address_holder_t *holder;
	struct in_addr translation;
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	holder = uc->send_addr_list;

	/* If the port is odd, assume it's the RTCP port */
	//nat映射地址无法保证获得偶数端口
#if 0
	if((port & 1) == 1)
		port--;
#endif
	translation = host2ip(addr);
	if(translation.s_addr == (u_int32) -1)
	{
		return errordebug(RTP_BAD_ADDR, "RTPSessionRemoveSendAddr",
					"Could not resolve address");
	}


	/* TTL matching is only done for multicast. For unicast, all TTL's
		 are set to zero */

	if(!IsMulticast(translation)) ttl = 0;

	while(holder != NULL) 
	{
		if(!(holder->deleteflag) &&
			 (holder->address.s_addr == translation.s_addr) &&
			 (holder->port == htons(port)) &&
			 (holder->ttl == ttl)) 
			break;

		holder = holder->next;
	}

	/* Now holder is either NULL if there was no match, else it points
		 to the address which matched */

	if(holder == NULL) 
	{
		return errordebug(RTP_BAD_ADDR, "RTPSessionRemoveSendAddr",
					"No such address");
	}
	else 
	{
		holder->deleteflag = TRUE;
		return RTP_OK;
	}
}

/* This function sets the address and port that the library listens to
	 for incoming packets. Currently, you can only listen to a single
	 socket. For unicast operation, setting the address to NULL will
	 cause the library to use INADDR_ANY to bind to. Setting the port
	 to zero will cause the library to obtain a dynamic port number to
	 listen to for RTP. The RTCP port will then be bound to the port one
	 higher than this. Once the socket has been created and opened (as a
	 result of calling RTPOpenConnection, you can use
	 RTPSessionGetReceiveAddr to read the port number that was actually
	 used. For multicast, the address is the multicast group to listen
	 to.

	 Listening to a multicast address should? get you the unicast
	 packets destined for the same port.

	 The port is in host byte order.

	 You cannot call this function once OpenConnection has been called.
	 */
		
rtperror RTPSessionSetReceiveAddr(context cid, char *address, u_int16 port)
{
	address_holder_t *holder;
	struct in_addr translation;
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (uc->connection_opened)
	{
		return errordebug(RTP_FIXED_WHEN_OPEN, "RTPSessionSetLocalAddr",
					"Cannot change address during opened connection");
	}

	/* If the port is odd, assume it's the RTCP port */
	//nat映射地址无法保证获得偶数端口
#if 0
	if((port & 1) == 1)
		port--;
#endif
	translation = host2ip(address);
	if(translation.s_addr == (u_int32) -1) 
	{
		return errordebug(RTP_BAD_ADDR, "RTPSessionSetReceiveAddr",
					"Could not resolve address");
	}

	if(uc->recv_addr_list == NULL) 
	{

		/* Create new address structure */
		if((holder = (address_holder_t *) malloc(sizeof(address_holder_t))) == 0) 
		{
			return errordebug(RTP_CANT_ALLOC_MEM, "RTPSessionAddSendAddr",
			"Cannot allocate memory");
		}

		holder->address = translation;
		if (address == NULL) holder->address.s_addr = 0;
		holder->port = htons(port);
		holder->ttl = 0;

		uc->recv_addr_list = holder;
	} 
	else 
	{

		/* Modify existing values */
		holder = uc->recv_addr_list;
		holder->address = translation;
		if(address == NULL) holder->address.s_addr = 0;
		holder->port = htons(port);
		holder->ttl = 0;
	}


	return RTP_OK;
}

/* This function returns the receive address and port number, in host
	 order. They must have been previously set with RTPSessionSetReceiveAddr
	 in order for this to work. */

rtperror RTPSessionGetReceiveAddr(context cid, char *addr, u_int16 *port)
{
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if(uc->recv_addr_list == NULL) {
		return errordebug(RTP_BAD_ADDR, "RTPSessionGetReceiveAddr",
					"Address not yet set");
	}

	strcpy(addr, inet_ntoa(uc->recv_addr_list->address));
	*port = ntohs(uc->recv_addr_list->port);

	return RTP_OK;
}
		

rtperror RTPOpenConnection(context cid)
{
	struct sockaddr_in saddr;
	int dynamic_ports,bind_count,problem, nRet;
	socktype rtpskt, rtcpskt;
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (uc->connection_opened)
		RTPCloseConnection(cid,NULL);
	

	/* First check if the user has set the local address */
	if(uc->recv_addr_list == NULL) 
		return errordebug(RTP_BAD_ADDR, "RTPOpenConnection","Address not yet set");
	

	/* Set a flag for dynamic ports */

	if(uc->recv_addr_list->port == 0) 
		dynamic_ports = 1;
	else
		dynamic_ports = 0;

	/* For dynamic ports, we choose a port randomly, and try
		 and bind to it, plus the one one higher for RTCP. If either
		 fail, we iterate _BIND_COUNTER times, and then give up */

	bind_count = 0;
	while(bind_count < _BIND_COUNTER) 
	{
		
		bind_count++;

		/* We only use ports in the dynamic range - 49152 - 65535 */

		if(dynamic_ports == 1) 
			uc->recv_addr_list->port = htons(_UDP_PORT_BASE + 2 * ((u_int16) (drand48() * _UDP_PORT_RANGE)));

		/* Create the RTP and RTCP sockets */
		uc->recv_addr_list->rtpsocket = _sys_create_socket(SOCK_DGRAM);
		rtpskt = uc->recv_addr_list->rtpsocket;
		if (uc->recv_addr_list->rtpsocket == _SYS_INVALID_SOCKET)
		{
			return errordebug(RTP_CANT_GET_SOCKET, "RTPOpenConnection",
			"couldn't get RTP socket for context %d",	(int)cid);
		}
		uc->recv_addr_list->rtcpsocket = _sys_create_socket(SOCK_DGRAM);
		rtcpskt = uc->recv_addr_list->rtcpsocket;
		if (uc->recv_addr_list->rtcpsocket == _SYS_INVALID_SOCKET)
		{
			_sys_close_socket(rtpskt);
			return errordebug(RTP_CANT_GET_SOCKET, "RTPOpenConnection",
			"couldn't get RTCP socket for context %d",	(int)cid);
		}
		//alter by zhengk
		
		if(_sys_set_reuseaddr(rtpskt) == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket (rtpskt);
			_sys_close_socket (rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
			"couldn't reuse RTP address for context %d", (int)cid);
		}
		if(_sys_set_reuseaddr(rtcpskt) == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
			"couldn't reuse RTCP address for context %d", (int)cid);
		} 

		if(_sys_set_reuseport(rtpskt) == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
			"couldn't reuse RTP port for context %d",	(int)cid);
		}
		if (_sys_set_reuseport(rtcpskt) == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
			"couldn't reuse RTCP port for context %d",	(int)cid);
		} 
		
		/* bind sockets */
		
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_addr = uc->recv_addr_list->address;
		saddr.sin_port = uc->recv_addr_list->port;
		//printf("uc->recv_addr_list->port:%d dynamic_ports:%d\n", ntohs(uc->recv_addr_list->port), dynamic_ports);

		/* If the address is multicast or null, bind to INADDR_ANY */

		if((uc->recv_addr_list->address.s_addr == 0) || IsMulticast(saddr.sin_addr))
			saddr.sin_addr.s_addr = INADDR_ANY;

		/* RTP port bind */
		problem = 0;
		if((problem = _sys_bind(rtpskt, &saddr)) == _SYS_SOCKET_ADDRNOTAVAIL)
		{
			saddr.sin_addr.s_addr = INADDR_ANY;
			problem = _sys_bind(rtpskt, &saddr);
		}

		/* Address in use, try another port if we're doing dynamic ports */
		if((problem == _SYS_SOCKET_ADDRINUSE) && (dynamic_ports == 1)) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			continue;
		}
		else if(problem != 0) 
		{
			return errordebug(RTP_CANT_BIND_SOCKET, "RTPOpenConnection",
			"couldn't bind RTP address for context %d",	(int)cid);
		}

		/* No error! */

		saddr.sin_port = htons(ntohs(uc->recv_addr_list->port) + 1);

		/* Bind to RTCP port */

		problem = 0;
		if((problem = _sys_bind(rtcpskt, &saddr)) == _SYS_SOCKET_ADDRNOTAVAIL)
		{
			/* The user specified a nonlocal address - probably they want to
				 send to this as a unicast address, so try INADDR_ANY */
			saddr.sin_addr.s_addr = INADDR_ANY;
			problem = _sys_bind(rtpskt, &saddr);

		}

		/* Address in use, try another port if we're doing dynamic ports */

		if((problem == _SYS_SOCKET_ADDRINUSE) &&(dynamic_ports == 1)) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			continue;
		} 
		else if(problem != 0)
		{
			return errordebug(RTP_CANT_BIND_SOCKET, "RTPOpenConnection",
			"couldn't bind RTCP address for context %d", (int)cid);
		}
		break;
	}

	/* Now we are here either because of success, or looping
		 too much */

	if(bind_count == _BIND_COUNTER) 
	{
		return errordebug(RTP_CANT_BIND_SOCKET, "RTPOpenConnection",
					"couldn't bind dynamic address for context %d", (int)cid);
	}


	/* Allow reuse of the address and port	*/

	if (IsMulticast(uc->recv_addr_list->address))
	{ /* Multicast */
	
		/* Every member of the session is a member of the multicast session */
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_addr = uc->recv_addr_list->address;
		saddr.sin_port = uc->recv_addr_list->port;
		nRet = _sys_join_mcast_group(rtpskt, &saddr);
		if(nRet == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
				"couldn't join RTP multicast group for context %d",	(int)cid);
		}
		nRet = _sys_join_mcast_group(rtcpskt, &saddr);
		if(nRet == _SYS_SOCKET_ERROR) 
		{
			_sys_close_socket(rtpskt);
			_sys_close_socket(rtcpskt);
			return errordebug(RTP_CANT_SET_SOCKOPT, "RTPOpenConnection",
				"couldn't join RTCP multicast group for context %d",	(int)cid);
		}

	}

	/* Schedule the first rtcp packet, and initialize some data structures */
	err = RTPStartSession(cid);
	if (err != RTP_OK)
		return err;

	uc->connection_opened = TRUE;
	//add by zhengk
	if (1==uc->SendFlag && uc->send_addr_list!=NULL)
	{
		address_holder_t *holder = uc->send_addr_list;
		while (holder)
		{
			holder->rtpsocket = uc->recv_addr_list->rtpsocket;
			holder->rtcpsocket = uc->recv_addr_list->rtcpsocket;
			holder = holder->next;
		}	
	}
	return RTP_OK;
}


rtperror RTPCloseConnection(context cid, char *reason)
{
	address_holder_t *s;
	hl_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (!uc->connection_opened){
		return RTP_OK; /* Connection is already closed */
	}

	err = RTPStopSession(cid, reason);
	if (err != RTP_OK)
		return err;


	if (_sys_close_socket(uc->recv_addr_list->rtpsocket) < 0){
		return errordebug(RTP_CANT_CLOSE_SESSION, "RTPCloseConnection",
					"context %d couldn't close RTP session", (int)cid);
	}
	if (_sys_close_socket(uc->recv_addr_list->rtcpsocket) < 0){
		return errordebug(RTP_CANT_CLOSE_SESSION, "RTPCloseConnection",
					"context %d couldn't close RTCP session", (int)cid);
	}
	if (0==uc->SendFlag)
	{
		s = uc->send_addr_list;
		while(s != NULL) 
		{
			if (_sys_close_socket(s->rtpsocket) < 0)
			{
				return errordebug(RTP_CANT_CLOSE_SESSION, "RTPCloseConnection",
					"context %d couldn't close RTP session", (int)cid);
			}
			if (_sys_close_socket(s->rtcpsocket) < 0)
			{
				return errordebug(RTP_CANT_CLOSE_SESSION, "RTPCloseConnection",
					"context %d couldn't close RTCP session", (int)cid);
			}
			s = s->next;
		}
	}
	uc->connection_opened = FALSE;
	return(err);
}

rtperror RTPSessionGetRTPSocket(context cid, socktype *value)
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (!uc->connection_opened){
		return errordebug(RTP_NOSOCKET, "RTPSessionGetRTPSocket",
					"context %d, connection not yet opened.", (int)cid);
	}
	*value = uc->recv_addr_list->rtpsocket;
	return RTP_OK;
}

rtperror RTPSessionGetRTCPSocket(context cid, socktype *value)
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (!uc->connection_opened){
		return errordebug(RTP_NOSOCKET, "RTPSessionGetRTCPSocket",
					"context %d, connection not yet opened.", (int)cid);
	}
	*value = uc->recv_addr_list->rtcpsocket;
	return RTP_OK;
}







rtperror RTPSendVector(context cid, int32 tsinc, int8 marker,
					 int16 pti, struct iovec *payload, int vec_count){
	/* Our packet will be composed of vec_count + 2 buffers:
		 buffer 0 is the header
		 buffers 1 .. n-1 are the payload
		 buffer n is the payload padding */
	struct iovec *pktpart;
	int pktlen;
	int buflen;
	struct msghdr the_message;
	int errchk, i, data_len, rundelete;
	address_holder_t *s, *prevs;
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (uc->PreventEntryIntoFlaggingFunctions)
	{
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPSendVector",
					"context %d, cannot be called now",
					(int)cid);
	}

	if(uc->send_addr_list == NULL) 
	{
		return errordebug(RTP_BAD_ADDR, "RTPSendVector",
					"context %d, no send addresses",
					(int)cid);
	}

	pktlen = vec_count + 2;
	pktpart = (struct iovec *) calloc(sizeof(struct iovec), pktlen);
	if (pktpart == NULL) 
	{
		return errordebug(RTP_CANT_ALLOC_MEM, "RTPSendVector",
					"context %d, couldn't allocate iovec",
					(int)cid);
	}

	uc->PreventEntryIntoFlaggingFunctions = TRUE;

	data_len = 0;

	for (i = 0; i < vec_count; i++) 
	{
		pktpart[i+1].iov_base = payload[i].iov_base;
		pktpart[i+1].iov_len = payload[i].iov_len;
		data_len += payload[i].iov_len;
		/* octet count only measures the payload (no extension, no SSRC lists,
			 etc.) */
	}

	buflen = _RTP_MAX_PKT_SIZE;

	err = RTPBuildRTPHeader(cid, tsinc, marker, pti, FALSE /* ... XXX encrypt */,
				data_len, uc->packet_buffer, &buflen);
	if (err)
		goto cleanup;

	pktpart[0].iov_base = uc->packet_buffer;
	pktpart[0].iov_len = buflen;

	/* Add padding to the payload if necessary */
	/* XXX encryption */
	pktpart[pktlen-1].iov_base = NULL;
	pktpart[pktlen-1].iov_len = 0;

	/* XXX do encryption here */

	/* Packet is not encrypted.	Then it is ready to be sent.	*/
	memset((char*) &the_message, 0, sizeof(the_message));
	the_message.msg_name = (caddr_t) NULL;
	the_message.msg_namelen = 0;
	the_message.msg_iov = pktpart;
	the_message.msg_iovlen = pktlen;

	s = uc->send_addr_list;

	err = RTP_OK;
	rundelete = FALSE;
	while(s != NULL) 
	{
		if(s->deleteflag == FALSE) 
		{
			errchk = _sys_sendmsg(s->rtpsocket, (struct msghdr*)&the_message, 0);
			if (errchk < 0)
			{
				err = errordebug(RTP_SOCKET_WRITE_FAILURE, "RTPSendVector",
			 		"context %d could not write to RTP socket",	(int)cid); 
				if (uc->SendErrorCallBack != NULL) 
				{
					uc->SendErrorCallBack(cid, inet_ntoa(s->address), ntohs(s->port), s->ttl);
				}
				if (s->deleteflag == TRUE) 
				{
					rundelete = TRUE;
				}
			}
		} 
		else
		{
			rundelete = TRUE;
		}
		s = s->next;
	}

	/* Now, we clean up the send list and remove all that have been deleted.
		 We know that this needs to be done if rundelete is TRUE */

	prevs = NULL;
	if(rundelete == TRUE) 
	{
		s = uc->send_addr_list;

		while(s != NULL) 
		{
			if(s->deleteflag == TRUE) 
			{
				if(prevs == NULL) 
					uc->send_addr_list = s->next;
				else
					prevs->next = s->next;
				if (0==uc->SendFlag)
				{
					_sys_close_socket(s->rtpsocket);
					_sys_close_socket(s->rtcpsocket);
				}
				free(s);
			}
			prevs = s;
			s = s->next;
		}
	}

cleanup:	
	free(pktpart);

	uc->PreventEntryIntoFlaggingFunctions = FALSE;
	return(err);
}





rtperror RTPSendVectorExt(context cid, int32 tsinc, int8 marker,
					 int16 pti, struct iovec *payload, int vec_count)
{
	/* Our packet will be composed of vec_count + 2 buffers:
		 buffer 0 is the header
		 buffers 1 .. n-1 are the payload
		 buffer n is the payload padding */
	//struct iovec *pktpart;
	//int pktlen;
	int buflen;
	//struct msghdr the_message;
	int errchk, i, data_len, rundelete;
	address_holder_t *s, *prevs;
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (uc->PreventEntryIntoFlaggingFunctions){
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPSendVector",
					"context %d, cannot be called now",
					(int)cid);
	}

	if(uc->send_addr_list == NULL) {
		return errordebug(RTP_BAD_ADDR, "RTPSendVector",
					"context %d, no send addresses",
					(int)cid);
	}


	uc->PreventEntryIntoFlaggingFunctions = TRUE;

	data_len = 0;
	for (i = 0; i < vec_count; i++) 
		data_len += payload[i].iov_len;

	buflen = _RTP_MAX_PKT_SIZE;

	err = RTPBuildRTPHeader(cid, tsinc, marker, pti, FALSE /* ... XXX encrypt */,
				data_len, uc->packet_buffer, &buflen);
		
	if (err)
		goto cleanup;
	struct sockaddr_in addr;
	bzero ( &addr, sizeof(addr) );
	addr.sin_family=AF_INET;
	int pos = buflen;
	for(i=0; i<vec_count; i++)
	{
		memcpy(uc->packet_buffer+pos, payload[i].iov_base, payload[i].iov_len);
		pos += payload[i].iov_len;
	}
	data_len += buflen;

	s = uc->send_addr_list;

	err = RTP_OK;
	rundelete = FALSE;
	//printf("send address:\n");
	while(s != NULL) 
	{
		if(s->deleteflag == FALSE) 
		{
			addr.sin_port=s->port;
			addr.sin_addr=s->address ;
			//printf("s->port:%d\n", htons(s->port));
			errchk = _sys_sendto(uc->recv_addr_list->rtpsocket,uc->packet_buffer, data_len, 0, &addr);
			if (errchk < 0)
			{
				printf("errchk:%d\n", errchk);
				err = errordebug(RTP_SOCKET_WRITE_FAILURE, "RTPSendVector",
			 			"context %d could not write to RTP socket",	(int)cid); 
				if (uc->SendErrorCallBack != NULL) 
					uc->SendErrorCallBack(cid,inet_ntoa(s->address),ntohs(s->port),	s->ttl);
				if (s->deleteflag == TRUE)
					rundelete = TRUE;
			}
		} 
		else 
		{
			rundelete = TRUE;
		}
		s = s->next;
	}

	/* Now, we clean up the send list and remove all that have been deleted.
		 We know that this needs to be done if rundelete is TRUE */

	prevs = NULL;
	if(rundelete == TRUE) 
	{
		s = uc->send_addr_list;

		while(s != NULL)
		{
			if(s->deleteflag == TRUE) 
			{
				if(prevs == NULL) 
					uc->send_addr_list = s->next;
				else
					prevs->next = s->next;
				if (0==uc->SendFlag)
				{
					_sys_close_socket(s->rtpsocket);
					_sys_close_socket(s->rtcpsocket);
				}
				free(s);
			}
			prevs = s;
			s = s->next;
		}
	}

cleanup:	

	uc->PreventEntryIntoFlaggingFunctions = FALSE;
	return(err);
}

rtperror RTPSend(context cid, int32 tsinc, int8 marker,
		 int16 pti, int8 *payload, int len)
{
	hl_context *uc;
	rtperror err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		return err;	
	struct iovec payload_vec[1];
	payload_vec[0].iov_base = payload;
	payload_vec[0].iov_len = len;
	if (1==uc->SendFlag)
		return RTPSendVectorExt(cid, tsinc, marker, pti, payload_vec, 1);
	else
		return RTPSendVector(cid, tsinc, marker, pti, payload_vec, 1);
}

rtperror RTPReceive(context cid, socktype socket, char *rtp_pkt_stream, int *len)
{
	int read_len, tot_len;
	struct sockaddr from_addr;
	int fromaddrlen;
	struct sockaddr_in *check_addr, *from_addr_in;
	int isRTCP, possible_loopback;

	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	if (uc->PreventEntryIntoFlaggingFunctions)
	{
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPReceive",
											"context %d, cannot be called now",
											(int)cid);
	}
	if (socket != uc->recv_addr_list->rtpsocket &&
			socket != uc->recv_addr_list->rtcpsocket){
		return errordebug(RTP_SOCKET_MISMATCH, "RTPReceive",
					"context %d, socket provided not RTP socket nor RTCP socket", (int)cid);
	}
	uc->PreventEntryIntoFlaggingFunctions = TRUE;

	fromaddrlen = sizeof(from_addr);
	read_len = _sys_recvfrom(socket, rtp_pkt_stream, *len, 0, &from_addr, &fromaddrlen);
	if (read_len == -1) {
		return errordebug(RTP_SOCKET_READ_FAILURE, "RTPReceive",
					"Could not read from socket %d", socket);
	}

	if (read_len == *len)
	{
		/* If we get here, then the buffer was not large enough to hold
			 the whole packet. */
		tot_len = read_len;
		/* Keep reading until we drain the buffer */
		while (read_len == *len)
		{
			read_len = _sys_recvfrom(socket, rtp_pkt_stream, *len, 0, &from_addr, &fromaddrlen);
			tot_len += read_len;
		}
		*len = tot_len;
		
		uc->PreventEntryIntoFlaggingFunctions = FALSE;
		return errordebug(RTP_INSUFFICIENT_BUFFER, "RTPReceive",
					"context %d, insufficient buffer provided to hold packet", (int)cid);
	}
	*len = read_len;

	/* XXX encryption: decrypt here */

	isRTCP = (socket == uc->recv_addr_list->rtcpsocket);

	/* If our fromaddr agrees with the appropriate source addr, mark this as a
		 possible loopback to RTPPacketReceived.
	 
		 On sensible systems where getsockname() does the right thing for
		 connected UDP sockets, we check if the addr and port match.
		 Unfortunately, on some systems (Solaris and Windows) getsockname() puts
		 INADDR_ANY for the addr; there, we can only check if the ports
		 match. */

	if (isRTCP)
		check_addr = (struct sockaddr_in *)&uc->rtcp_sourceaddr;
	else
		check_addr = (struct sockaddr_in *)&uc->rtp_sourceaddr;

	from_addr_in = (struct sockaddr_in *)&from_addr;

	possible_loopback =
		(check_addr->sin_family != AF_UNSPEC &&
		 (check_addr->sin_addr.s_addr == from_addr_in->sin_addr.s_addr ||
			check_addr->sin_addr.s_addr == INADDR_ANY) /* Solaris, Winsock */ &&
		 from_addr_in->sin_port == check_addr->sin_port);

	err = RTPPacketReceived(cid, rtp_pkt_stream, read_len,
				from_addr, fromaddrlen,
				isRTCP, possible_loopback);

	uc->PreventEntryIntoFlaggingFunctions = FALSE;
	return err;
}

rtperror RTPSessionRemoveFromContributorList(context cid, u_int32 ssrc,
							 char *reason)
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	err = RTPSessionLowLevelRemoveFromContributorList(cid, ssrc, reason);

	return err;
}

/* This is for the low-level code -- interface with RTPSchedule */

struct timer_info 
{
	context cid;
	int32 timer_type;
	u_int32 data;
	char *reason;
};

void RTPSetTimer(context cid, int32 timer_type, u_int32 data,
		 char *str, struct timeval *tp)
{
	rtperror err;
	hl_context *uc;
	struct timer_info *ti;
	struct timeval now;
	struct timeval notime = {0,0};

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return;

	/* Build a timer_info */
	ti = (struct timer_info *) malloc(sizeof(struct timer_info));
	if (ti == NULL)
		/* XXX log debug info? */
		return;

	ti->cid = cid;
	ti->timer_type = timer_type;
	ti->data = data;
	if (str != NULL)
		ti->reason = strdup(str);
	else
		ti->reason = NULL;

	/* If the time-to-send isn't in the future, and if we're not in a callback,
		 send immediately */
	gettimeofday(&now, NULL);
	if (!uc->PreventEntryIntoFlaggingFunctions &&
			TimeExpired(tp, &now, &notime)) {
		RTPExecute(cid, (rtp_opaque_t) ti);
	}
	else {
		RTPSchedule(cid, (rtp_opaque_t) ti, tp);
	}
}


rtperror RTPExecute(context cid, rtp_opaque_t opaque)
{
	rtperror err;
	hl_context *uc;
	struct timer_info *ti;
	int errchk, rundelete, buflen;
	address_holder_t *s, *prevs;
	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	ti = (struct timer_info *) opaque;

	if (ti->cid != cid) 
	{
		return errordebug(RTP_UNKNOWN_CONTEXT, "RTPExecute",
					"context %d in arg != context %d in opaque",
					(int)cid, (int)ti->cid);
	}	

	if (uc->PreventEntryIntoFlaggingFunctions){
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPExecute",
					"context %d, cannot be called now",
					(int)cid);
	}
	uc->PreventEntryIntoFlaggingFunctions = TRUE;
	/* XXX encryption: block sizes, sub-parts */
	buflen = _RTP_MAX_PKT_SIZE;
	switch(ti->timer_type) 
	{
		case RTP_TIMER_SEND_RTCP:
			//printf("send RTP_TIMER_SEND_RTCP\n");
			err = RTPBuildRTCPPacket(cid, RTCP_SUBPARTS_ALL, 0,
					 uc->packet_buffer, &buflen);
			break;
		case RTP_TIMER_SEND_BYE_ALL:
			//printf("send RTP_TIMER_SEND_BYE_ALL\n");
			err = RTPBuildByePacket(cid, FALSE, 0, ti->reason, 0,
					uc->packet_buffer, &buflen);
			break;
		case RTP_TIMER_SEND_BYE_CONTRIBUTOR:
			//printf("send RTP_TIMER_SEND_BYE_CONTRIBUTOR\n");
			err = RTPBuildByePacket(cid, TRUE, ti->data, ti->reason, 0,
					uc->packet_buffer, &buflen);
			break;
		case RTP_TIMER_SEND_BYE_COLLISION:
			//printf("send RTP_TIMER_SEND_BYE_COLLISION\n");
			err = RTPBuildByePacket(cid, TRUE, ti->data, ti->reason, 0,
					uc->packet_buffer, &buflen);
			break;
		default:
			goto cleanup;
	}

	if (err != RTP_OK) 
	{
		if (err == RTP_DONT_SEND_NOW) 
		{
			/* This is a legitimate "error" message and should be suppressed */
			/* XXX: should any other return codes be suppressed? */
			err = RTP_OK;
		}
		goto cleanup;
	}

	/* XXX encryption: encrypt here */

	s = uc->send_addr_list;

	err = RTP_OK;
	rundelete = FALSE;
	
	//发送地址
	struct sockaddr_in addr;
	bzero ( &addr, sizeof(addr));
	addr.sin_family=AF_INET;
	
	while(s != NULL) 
	{
		if(s->deleteflag == FALSE) 
		{
			if (1==uc->SendFlag)
			{
				addr.sin_port = htons(ntohs(s->port)+1);
				addr.sin_addr = s->address ;
				errchk = _sys_sendto(s->rtcpsocket,uc->packet_buffer, buflen, 0, &addr);
			}	
			else
			{
				errchk = _sys_send(s->rtcpsocket, uc->packet_buffer, buflen, 0);
			}	
			if (errchk < 0)
			{
				if (uc->SendErrorCallBack != NULL) 
				{
					uc->SendErrorCallBack(cid,inet_ntoa(s->address),ntohs(s->port),	s->ttl);
				}
				if (s->deleteflag == TRUE) 
				{
					rundelete = TRUE;
				}
			}
		}
		else
		{
			rundelete = TRUE;
		}
		s = s->next;
	}

	/* Now, we clean up the send list and remove all that have been deleted.
		 We know that this needs to be done if rundelete is TRUE */

	prevs = NULL;
	if (rundelete == TRUE) 
	{
		s = uc->send_addr_list;

		while(s != NULL) 
		{
			if(s->deleteflag == TRUE) 
			{
				if(prevs == NULL) 
					uc->send_addr_list = s->next;
				else
					prevs->next = s->next;
				if (0==uc->SendFlag)
				{
					_sys_close_socket(s->rtpsocket);
					_sys_close_socket(s->rtcpsocket);
				}
				free(s);
			}
			prevs = s;
			s = s->next;
		}
	}

cleanup:
	if (ti->reason != NULL) {
		free(ti->reason);
	}
	free(ti);
	uc->PreventEntryIntoFlaggingFunctions = FALSE;
	return err;
}

rtperror RTPSessionSetEncryption(context cid, encryption_t value){
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	uc->use_encryption = value;
	return RTP_OK;
}

rtperror RTPSessionGetEncryption(context cid, encryption_t *value){

	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	*value =	uc->use_encryption;
	return RTP_OK;
}


rtperror RTPSessionSetEncryptionFuncs(context cid, 
							void (*init)(context, void*),
							void (*encrypt)(context, char*,
									int, void*),
							void (*decrypt)(context, char*,
									int, void*)){
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	uc->encrypt_initfunc = init;
	uc->encrypt_encryptfunc = encrypt;
	uc->encrypt_decryptfunc = decrypt;
	return RTP_OK;
}
	
rtperror RTPSessionSetKey(context cid, void* value){
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;


	uc->key = value;
	if (uc->encrypt_initfunc != NULL){
		uc->encrypt_initfunc(cid, value);
	}
	return RTP_OK;
}

rtperror RTPSessionGetKey(context cid, void **value)
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	*value = uc->key;
	return RTP_OK;
}


rtperror RTPSetSendErrorCallBack(context cid,
	void (*f)(context, char *, u_int16, u_int8)) 
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	uc->SendErrorCallBack = f;
	return RTP_OK;
}

static void hl_changed_sockaddr_callback(context cid,
						 person p,
						 struct sockaddr *sa,
						 int is_rtcp)
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return;

	if (uc->ChangedMemberAddressCallBack != NULL) 
	{
		char portstr[_RTP_MAX_PORT_STR_SIZE];
		struct sockaddr_in *si = (struct sockaddr_in *) sa;

		sprintf(portstr, "%hu", ntohs(si->sin_port));
		uc->ChangedMemberAddressCallBack(cid, p,
						 inet_ntoa(si->sin_addr),
						 portstr,
						 is_rtcp);
	}
}	

rtperror RTPSetChangedMemberAddressCallBack(context cid,
	void (*f)(context, person, char*, char*, int))
{
	rtperror err;
	hl_context *uc;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

	uc->ChangedMemberAddressCallBack = f;
	return RTP_OK;
}

rtperror RTPMostRecentAddr(context cid, char *addr, char *port) 
{
	struct sockaddr_in si;
	rtperror err;

	err = RTPMostRecentSockaddr(cid, (struct sockaddr *)&si);
	if (err != RTP_OK)
		return err;

	strcpy(addr, inet_ntoa(si.sin_addr));
	sprintf(port, "%hu", ntohs(si.sin_port));

	return RTP_OK;
}


rtperror RTPMemberInfoGetRTPAddr(context cid, person p,
				 char *addr, char *port) 
{
	struct sockaddr_in si;
	rtperror err;

	err = RTPMemberInfoGetRTPSockaddr(cid, p, (struct sockaddr *)&si);
	if (err != RTP_OK)
		return err;

	strcpy(addr, inet_ntoa(si.sin_addr));
	sprintf(port, "%hu", ntohs(si.sin_port));

	return RTP_OK;
}

rtperror RTPMemberInfoGetRTCPAddr(context cid, person p,
					char *addr, char *port) 
{
	struct sockaddr_in si;
	rtperror err;

	err = RTPMemberInfoGetRTCPSockaddr(cid, p, (struct sockaddr *)&si);
	if (err != RTP_OK)
		return err;

	strcpy(addr, inet_ntoa(si.sin_addr));
	sprintf(port, "%hu", ntohs(si.sin_port));

	return RTP_OK;
}



/* Initialize random number generators with a random seed. */
/* Compile with -D_RTP_SEMI_RANDOM to get repeatable behavior
 * for testing.
 */

void InitRandom()
{
	struct timeval curtime;
#ifdef _RTP_SEMI_RANDOM
	return;	/* shuts off seeding random generators */
#endif
	gettimeofday(&curtime, NULL);
	srand48((int) curtime.tv_usec);
}

/* random32: generate a 32-bit random number.
 * Without _RTP_SEMI_RANDOM, this is (hopefully) a cryptographically-secure
 * hash of non-externally-predictable values.
 */



/***************************
 * Random # generator code *
 * Straight from RFC 1889	*
 ***************************/

/*
 * Generate a random 32-bit quantity.
 */

#define MD_CTX MD5_CTX 
#define MDInit MD5Init 
#define MDUpdate MD5Update 
#define MDFinal MD5Final 

u_long md_32(char *string, int length) 
{
	MD_CTX context;
	union {
		char	 c[16];
		u_long x[4];
	} digest;
	u_long r;
	int i;

	MDInit (&context); 
	MDUpdate (&context, string, length); 
	MDFinal ((unsigned char *)&digest, &context); 
	r = 0; 
	for (i = 0; i < 3; i++) { 
		r ^= digest.x[i]; 
	} 
	return r; 
	
}															 /* md_32 */

/*
 * Return random unsigned 32-bit quantity. Use 'type' argument if you
 * need to generate several different values in close succession.
 */


/********************************
 * Code from RFC 1889 ends here *
 ********************************/







/* rtp_encrypt.c */

#if 0 /* XXX encryption */
rtperror DoEncryption(context cid, struct iovec *pktpart, int pktlen,
					int IsRTP)
{
	/* Encrypts and sends packet */
	/* NOTE: Can't use _RTP_Bufferspace to hold encrypted packet because
		 the RTCP packet is already in _RTP_Bufferspace */
	char encryptbuf[_RTP_MAX_PKT_SIZE];
	int32 random_header = random32(cid);
	int tot_len, rundelete;
	int i, errchk;
	int use_socket;
	address_holder_t *s, *prevs;
	unix_context *uc;
	rtperror err;

	err = RTPSessionGetHighLevelInfo(cid, (void**)&uc);
	if (err != RTP_OK)
		/* The cid is bogus */
		return err;

#ifdef _RTP_DEBUG
	printf("Encrypting ");
	if (IsRTP){
		printf("RTP\n");
	}
	else printf("RTCP\n");
#endif


	err = RTP_OK;
	if (uc->encrypt_encryptfunc == NULL){
		return errordebug(RTP_CANT_USE_ENCRYPTION, "DoEncryption",
					"context %d encryption function not set",
					(int)cid);
		return RTP_CANT_USE_ENCRYPTION;
	}
	/* Now copy the data into the buffer where DES can then be performed */
	memcpy(encryptbuf, (char*) &random_header, sizeof(random_header));
	tot_len = sizeof(random_header);
	for (i=0; i < pktlen; i++){	 
		memcpy(&encryptbuf[tot_len], (char*)	pktpart[i].iov_base, 
		 pktpart[i].iov_len);
		tot_len += pktpart[i].iov_len;
	}


	/* NOTE: Here is where we want to call the encryption algorithm */

	uc->encrypt_encryptfunc(cid, encryptbuf,
					(int) tot_len, 
					uc->key);

	/* Now send the packet */
	s = uc->send_addr_list;
	rundelete = FALSE;
	while(s != NULL) 
	{
		if(IsRTP)
			use_socket = s->rtpsocket;
		else
			use_socket = s->rtcpsocket;

		if(s->deleteflag == FALSE) 
		{
			errchk = send(use_socket, encryptbuf, tot_len, 0);
		
			if (errchk < 0)
			{
				err = errordebug(RTP_SOCKET_WRITE_FAILURE, "DoEncryption",
			 			"context %d couldn't send encrypted packet",(int)cid);
				/* XXX need prevent entry flag set */
				if (uc->SendErrorCallBack != NULL) 
				{
					uc->SendErrorCallBack(cid,inet_ntoa(s->address),ntohs(s->port),	s->ttl);
				}
			}
		} 
		else 
		{
			rundelete = TRUE;
		}

		s = s->next;
	}

	prevs = NULL;
	if(rundelete == TRUE) {
		s = uc->send_addr_list;

		while(s != NULL) {
			if(s->deleteflag == TRUE) {

	if(prevs == NULL) 
		uc->send_addr_list = s->next;
	else
		prevs->next = s->next;

	close(s->rtpsocket);
	close(s->rtcpsocket);
	free(s);
			}
			prevs = s;
			s = s->next;
		}
	}


	return err;
}
#endif


/* Returns TRUE if the addresses don't match:
	 1) If of type AF_INET: addresses must match and ports must match
	 or the odd port must be one greater than the even port.
	 (XXX: keep this?)
	 2) If of another type, the first complen bytes must match exactly.

	 The low-level code makes sure that neither address is of family
	 _RTP_ADDRESS_NOT_YET_KNOWN before calling this function */
int FromDifferentSource(struct sockaddr *addr1, struct sockaddr *addr2,	int complen)
{
	struct sockaddr_in *inetaddr1, *inetaddr2;

	if (addr1->sa_family != addr2->sa_family)
	{
		return TRUE;
	}

	if (addr1->sa_family == AF_INET)
	{
		inetaddr1 = (struct sockaddr_in*) addr1;
		inetaddr2 = (struct sockaddr_in*) addr2;

		return ((inetaddr1->sin_addr.s_addr != inetaddr2->sin_addr.s_addr) ||
			((ntohs(inetaddr1->sin_port) & (~1)) !=
			 (ntohs(inetaddr2->sin_port) & (~1))));
	}
	
	/* Not of type internet: compare full network address */
	return (memcmp(addr1, addr2, complen) != 0);
}
