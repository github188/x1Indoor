/* rtp_lowlevel.h : RTP API types, structures, and functions necessary
   to implement the network-dependent parts of an interface
   based on the low-level (network-independent) interface.
   Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#ifndef _RTP_LOWLEVEL_H
#define _RTP_LOWLEVEL_H

/* Types for low-level functions */

/* Which sub-parts of an RTCP packet we want to send. */
typedef enum {
  RTCP_SUBPARTS_ALL,
  RTCP_SUBPARTS_RRSR,
  RTCP_SUBPARTS_SDES
} rtcp_subparts_t;

/* Event types used by RTPSetTimer. */
#define RTP_TIMER_SEND_RTCP            1
#define RTP_TIMER_SEND_BYE_ALL         2
#define RTP_TIMER_SEND_BYE_CONTRIBUTOR 3
#define RTP_TIMER_SEND_BYE_COLLISION   4

/* Debugging variables */

/* The most recent error raised -- accessed by RTPDebugStr() */
/* Not thread-safe */
extern rtperror _RTP_cur_err; 

#ifdef _RTP_DEBUG
/* A comment about the error mentioned above */
/* Also not thread-safe */
extern char _RTP_err_msg[256];
#endif

/* Functions the low level provides */

rtperror RTPLowLevelCreate(context *the_context);
rtperror RTPLowLevelDestroy(context cid);

rtperror RTPSessionSetHighLevelInfo(context cid, void *info);
rtperror RTPSessionGetHighLevelInfo(context cid, void **info);

rtperror RTPStartSession(context cid);
rtperror RTPStopSession(context cid, char *reason);

rtperror RTPBuildByePacket(context cid, int one_csrc, u_int32 ssrc,
			   char *reason, int padding_block, char *buffer,
			   int *length);
rtperror RTPBuildRTCPPacket(context cid, rtcp_subparts_t subparts,
			    int padding_block, char *buffer, int *length);

rtperror RTPGetRTPHeaderLength(context cid, int *length);
rtperror RTPBuildRTPHeader(context cid, int32 tsinc, int8 marker,
			   int16 pti, int8 padding, int payload_len,
			   char *buffer, int *buflen);

rtperror RTPPacketReceived(context cid, char *rtp_pkt_stream, int len,
			   struct sockaddr from_addr, int fromaddrlen,
			   int isRTCP, int possible_loopback);

rtperror RTPSessionLowLevelRemoveFromContributorList(context cid,
						     u_int32 ssrc,
						     char *reason);

rtperror RTPSetChangedMemberSockaddrCallBack(context cid,
	void (*f)(context, person, struct sockaddr*, int));


rtperror RTPMemberInfoGetRTPSockaddr(context cid, person p,
				     struct sockaddr *addr);
rtperror RTPMemberInfoGetRTCPSockaddr(context cid, person p,
				      struct sockaddr *addr);


rtperror RTPMostRecentSockaddr(context cid, struct sockaddr *addr);



/* Helper functions the low level provides */

int TimeExpired(struct timeval *init_time, struct timeval *cur_time,
		struct timeval *interval);

/* Functions the high-level must provide */

void RTPSetTimer(context cid, int32 timer_type, u_int32 data,
		 char *str, struct timeval *tp);

int FromDifferentSource(struct sockaddr *addr1, struct sockaddr *addr2,
			int complen);

/* InitRandom initializes random number generators with a random seed. */
void InitRandom(void); 

u_int32 random32(int type);


/* Debugging */
#ifdef _RTP_DEBUG
rtperror errordebug(rtperror err, char *func, char *msg, ...)
#define _RTP_DEFINE_EDEBUG
#ifdef __GNUC__
__attribute__ ((format (printf, 3, 4)))
#endif
;
#else
#ifdef __GNUC__
#define errordebug(err, extra...)		(_RTP_cur_err = (err))
#else
rtperror errordebug(rtperror err, char *func, char *msg, ...);
#define _RTP_DEFINE_EDEBUG
#endif
#endif


#endif /* _RTP_LOWLEVEL_H */
