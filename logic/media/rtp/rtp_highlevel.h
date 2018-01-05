/* rtp_highlevel.h : RTP API types, structures, and functions necessary
                    exclusive to the high-level (network-dependent) parts of
		    the interface.

		    Include rtp_api.h before this file.	    

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#ifndef _RTP_HIGHLEVEL_H
#define _RTP_HIGHLEVEL_H

/* Types */

typedef void* rtp_opaque_t;

#ifdef WIN32
typedef SOCKET socktype;
#endif

#ifdef __unix
#define SOCKTYPE_DEFINED
typedef int socktype;
#endif


/* Functions the high-level API provides */

rtperror RTPCreate(context *the_context);

rtperror RTPCreateExt(context *the_context);


rtperror RTPDestroy(context cid);


rtperror RTPOpenConnection(context cid);

rtperror RTPCloseConnection(context cid, char *reason);


rtperror RTPSend(context cid, int32 tsinc, int8 marker,
   int16 pti, int8 *payload, int len);

rtperror RTPSendVector(context cid, int32 tsinc, int8 marker,
   int16 pti, struct iovec *payload, int vec_count);

rtperror RTPReceive(context cid, socktype socket,
                    char *rtp_pkt_stream, int *len);


rtperror RTPSessionAddSendAddr(context cid, char *addr, u_int16 port, u_int8 ttl);
rtperror RTPSessionRemoveSendAddr(context cid, char *addr, u_int16 port, u_int8 ttl);

rtperror RTPSessionSetReceiveAddr(context cid, char *addr, u_int16 port);
rtperror RTPSessionGetReceiveAddr(context cid, char *addr, u_int16 *port); 



rtperror RTPSessionRemoveFromContributorList(context cid, u_int32 ssrc,
					     char *reason);

rtperror RTPSessionGetRTPSocket(context cid, socktype *value);
rtperror RTPSessionGetRTCPSocket(context cid, socktype *value);

rtperror RTPExecute(context cid, rtp_opaque_t opaque);

rtperror RTPSetChangedMemberAddressCallBack(context cid,
	void (*f)(context, person, char*, char*, int));
rtperror RTPSetSendErrorCallBack(context cid,
	void (*f)(context, char *, u_int16, u_int8));


rtperror RTPMostRecentAddr(context cid, char *addr, char *port);


rtperror RTPMemberInfoGetRTPAddr(context cid, person p,
				 char *addr, char *port);
rtperror RTPMemberInfoGetRTCPAddr(context cid, person p,
				 char *addr, char *port);


/* Functions you must provide for the high-level API */

void RTPSchedule(context cid, rtp_opaque_t opaque, struct timeval *tp);



/* Encryption funcs (side-level?) */
rtperror RTPSessionSetEncryption(context cid, encryption_t value);
rtperror RTPSessionGetEncryption(context cid, encryption_t *value);
rtperror RTPSessionSetEncryptionFuncs(context cid, 
				 void (*init)(context, void*),
				 void (*encrypt)(context, char*, int, void*),
				 void (*decrypt)(context, char*, int, void*));


#endif /* _RTP_HIGHLEVEL_H */
