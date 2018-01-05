/* rtp_api_internal.h : Default settings, internal structures, and
                        various functionality required to support the
                        functionality in rtp_api.h and rtp_api.c

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include "rtp_mlist_internal.h"

#ifndef _RTP_API_INTERNAL_H
#define _RTP_API_INTERNAL_H

#include "rtp_api.h"
#include "rtp_api_ext.h"

#include "rtp_lowlevel.h"



/***************
 * Global Defs *
 ***************/

/* Maximum number of CSRC */
#define _RTP_MAX_CSRC 15

/* How many contexts we initially make available (i.e. how many
   contexts can be created before the ContextList which indexes
   the contexts calls realloc() */
#define _RTP_INIT_CONTEXTS_AVAIL 256

/* Protocol version */

#define _RTP_VERSION 2


/* The increment by which we increase the ContextList when we need
   to realloc */
#define _RTP_CONTEXT_INC 10


/* Maximum storage for BYE packet reason */
#define _RTP_MAX_BYE_SIZE 1024


/* Maximum # of packets that can appear in a compound RTCP packet
   (used to store pointers to the start of each packet.  Since
   this doesn't use much space, we overestimate this value) */
#define _RTP_MAX_PKTS_IN_COMPOUND 200



/* # of Payload types available. */
#define _RTP_MAX_PAYLOAD_TYPES 128



/* Default bandwidth in Kb / sec */
#define _RTP_DEFAULT_BANDWIDTH 120



/* RTCP bandwidth fraction (default is 5%)*/
#define _RTP_DEFAULT_RTCP_FRAC .05;



/* Fraction of RTCP bandwidth to be shared among active senders */
#define _RTP_DEFAULT_SENDER_BW_FRAC 0.25


/* Maximum number of stored unique ID remaps.  An ID is remapped
   when a collision is resolved and the toolkit realizes that two
   of its members are actually the same member.  The larger ID
   gets mapped to the smaller.  If we overrun the number of remaps
   (which is very doubtful), earlier remaps get dropped (i.e.
   forgotten), and any attempt to access the larger ID for a member
   will not locate the member. */
#define _RTP_MAX_UNIQUE_ID_REMAPS 2000


/* The time to a hard timeout is a fixed interval of time that is
   a multiple of the time to soft timeout.  _RTP_HARD_MEMBER_TIMEOUT
   holds the value of that multiple. */
#define _RTP_HARD_MEMBER_TIMEOUT 3


/* The time until a member has a "soft" timeout, or the time that a
   member who has not been "validated" i.e. does not have a CNAME or is
   involved in a collision will be terminated earlier than the typical
   timeout above.  The termination is in terms of the time of the
   current RTCP receiver interval. */
#define _RTP_SOFT_MEMBER_TIMEOUT 5



/* Number of sender intervals before a sender times out */
#define _RTP_SENDER_TIMEOUT 1

/* Number of sender intervals before a purported sender has their information
   deleted */

#define _RTP_PURPORTED_SENDER_TIMEOUT 5


/* Maximum time until a sender timeout */
#define _RTP_SENDER_MAX_TIMEOUT 3600
#define _RTP_PURPORTED_SENDER_MAX_TIMEOUT 18000


/* The minimum time in which an RTCP packet is allowed to be sent 
   (note: this is the value before being multiplied by the random
   factor) */
#define _RTCP_MIN_TIME 5



/* Used to calculate the average size of an RTCP packet, this represents
   the fraction by which the most recent RTCP packet influences the
   average value */
#define _RTCP_SIZE_GAIN (1.0 / 16.0)


/* ... XXX move to sys-dep */
/* Number of seconds between 1-Jan-1900 and 1-jan-1970 */
#define _GETTIMEOFDAY_TO_NTP_OFFSET 2208988800U

/* When we pick a new SSRC, we keep trying until we find one thats
not already used. This is the maximum number of times we'll try
before giving up. This is a problem when the user tries to set
the SSRC with a mask of all ones,so that it effectively cannot
be redrawn */

#define _RTP_MAXIMUM_SSRC_REDRAWS  50


#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif



/* Contributor type
 *
 * This is used to hold information about a CSRC contributor
 */

typedef struct contributor {
  u_int32 ssrc;                  /* SSRC for this contributor */
  struct contributor *next;      /* Next contributor */
} contributor_t;

/******************************************************************
 * _RTP_CONTEXT                                                   *
 * The following structure holds information that pertains to the *
 * context.  This holds or points to all information relevant to  *
 * the current session                                            *
 ******************************************************************/


typedef struct {

  /*********************************
   * Fields that are fixed for the *
   * entire duration of a context. *
   *********************************/

  /* the context number, which is also the index of this context in
     ContextList */
  int context_num; 



  /******************************
   * Fields that can be altered *
   * during a session.          *
   ******************************/


  /*  rtpprofile profile;  Profile type has been removed */


  /* Whether or not reconsideration is to be used. */
  reconsideration_t reconsideration;


  /* prev_group_size is used by reconsideration to determine if the
     size of the group has grown or shrunk since last call */
  int32 prev_group_size;


  /* We allow the user to set certain bits in the SSRC, with
     the others drawn based on the algorithm in RTP. The static
     SSRC contains the value of the static part, and the mask
     indicates which bits are static, and which are redrawn */

  u_int32 static_ssrc;
  u_int32 ssrc_mask;

  /* The local member's SSRC value (which can change during a connection
     due to a collision. This is in network byte order */
  u_int32 my_ssrc; 


  /* # of packets sent by local member during the connection */
  u_int32 sending_pkt_count; 

  
  /* # of octets sent by local member during the connection */
  u_int32 sending_octet_count; 


  /* The comparison in rates between NTP timestamp and RTP timestamp.
     A value of n means that each increment of an RTP timestamp
     corresponds to n microseconds. */
  int32 profileRTPTimeRates[_RTP_MAX_PAYLOAD_TYPES];


  /* The number of sources that contribute locally in the next packet*/
  int32 CSRClen;

  /* The SSRCs for these locally contributing sources in the next packet*/
  u_int32 *CSRCList;

  /* List of those CSRC for which we send SDES */
  contributor_t *contriblist;

  /* Bandwidth allocated for the session */
  float session_bandwidth;


  /* Fraction of bandwidth allocated for RTCP */
  float rtcp_fraction;


  /* Fraction of (RTCP) bandwidth to be used by senders */
  float sender_bw_fraction;


  /* Data set by the user (must also be destroyed by the user) */
  void* user_data; 


  /* Data set by the network-dependent code (must also be destroyed by
     that code */
  void *nd_data;


  /* We wish to prevent our member from reading his / her own packets.
     When a packet comes in, we check if it is from our local address,
     and also check if the SSRC matches our current SSRC, and if a
     collision still has not been resolved involving the local member,
     the previous ssrc as well.  We store the previous ssrc for these
     occasions here. */
  u_int32 my_previous_ssrc;

  
  /* The value of the RTP timestamp when the session opened (chosen randomly) 
   */
  int32 init_RTP_timestamp; 


  /* current rtp timestamp offset*/
  int32 time_elapsed; 

  
  /* The initial (randomly chosen) sequence number for packets transmitted
     from the local member */
  int32 init_seq_no;

  /* The extended sequence number */
  int32 seq_no; 


  /* The header extension */
  rtp_hdr_ext *hdr_extension;

  /* By setting PreventEntryIntoFlaggingFunctions to true, those
     functions that return flags (and make callbacks) cannot be
     entered.  This prevents recursive looping of the functions
     calling callbacks which call the function, etc. */
  int PreventEntryIntoFlaggingFunctions;



  /**********************
   * RTCP interval info *
   * used to calculate  *
   * the RTCP send rate *
   **********************/

  /* NOTE: This will not behave correctly when using SDES-only
     encryption, because each compound packet is counted separately and
     the average rtcp pkt size considers the two separately, but since
     they are sent at the same time, they should be counted together */

  /* Time at which last RTCP packet was sent.  Set initially to when
     the connection was opened */
  struct timeval last_rtcp_send;

  /* initial is set to TRUE if an RTCP packet has not yet been sent */
  int initial;

  /* Holds the size of the last RTCP packet (NOTE: When the compound
     packet is split due to SDES-only encryption, this counts
     the 2 parts separately) */
  int last_pkt_size;

  /* Holds the average packet size of an RTCP packet for the session */
  double avg_rtcp_size;

  /* random factor holds the random value computed for the time
     to send the next rtcp packet */
  double random_factor;



  /*****************************
   * Callback function storage *
   *****************************/
  void (*UpdateMemberCallBack)(context,person,rtpflag,char *);
  void (*ChangedMemberInfoCallBack)(context, person, memberinfo,
				    char*, rtpflag);
  void (*ChangedMemberSockaddrCallBack)(context, person,
					struct sockaddr*, int);
  void (*CollidedMemberCallBack)(context, person, person, rtpflag);
  void (*RevertingIDCallBack)(context, person, person, void*, rtpflag);




  /*************************
   * Member tracking       *
   * The following fields  *
   * are used to perform   *
   * member management     *
   *************************/


  /* We need to store somewhere the info that gets placed in an RTCP
     packet about this member.  Might as well use a member structure
     to do this. */
  member *my_memberinfo;


  /* Unique ID counter: each time a new member joins the session,
     they get a unique ID, obtained from unique_id_counter.  The
     counter is then incremented. */
  person unique_id_counter;


  /*************************
   * Membership structures *
   *************************/


  /* RTP_SSRC_Hash is the hash table that stores members indexed by SSRC */
  hash_table RTP_SSRC_Hash;


  /* RTP_ID_Hash is the hash table that stores members indexed by unique id */
  hash_table RTP_ID_Hash;

  
  /* CNAME_Hash is the hash table that stores members indexed by CNAME */
  hash_table RTP_CNAME_Hash;


  /* RTP_MemberList is a list that stores all members that are currently
     part of the session.  The list is sorted by increasing time of
     last packet received from the member */
  membership_list RTP_MemberList;


  /* RTP_SenderList is a list that stores all members that are currently
     part of the session and are sending RTP packets (i.e. doesn't consider
     RTCP packets).  The list is sorted by increasing time of last RTP
     packet received from the member */
  membership_list RTP_SenderList;





  
/* When there is an SSRC collision and a member starts sending a new
   SSRC, there may be a period of time before the CNAME is sent with
   the new SSRC.  Thus, the member will be assigned a new unique ID.
   When the CNAME comes and the toolkit can determine which 2 members
   are actually the same, the second member can be merged with the
   original, and the member can again be accessed by the first unique
   ID.  We keep a table of remapped IDs in case the user tries
   to access the member through the second unique ID. */

  /* # of remaps that have occurred */
  int num_remaps;


  /*  ID ID_remap[i] gets remapped to ID_orig_val[i] */
  person ID_remap[_RTP_MAX_UNIQUE_ID_REMAPS];  
  person ID_orig_val[_RTP_MAX_UNIQUE_ID_REMAPS]; 


  member *most_recent_rtp_sender;
  member *most_recent_rtcp_sender;
  

  /*************************
   * Internal Buffers, etc *
   *************************/

  /* The source of the most recent packet received */
  struct sockaddr most_recent_addr;

  /* A buffer which holds BYE reasons during callbacks */
  char byereason[_RTP_MAX_BYE_SIZE];

} _RTP_CONTEXT;


#ifndef _RTP_API_INTERNAL_C



/* ContextList will be an array of type *_RTP_CONTEXT, and will
   hold all contexts run by a single process */

extern _RTP_CONTEXT **ContextList;

/* One larger than the largest context used thusfar */
extern long _RTP_context_above_used;

/* The number of contexts that are currently in use. */
extern long _RTP_contexts_in_use;

#endif



void SetDefaultPayloadRates(_RTP_CONTEXT *the_context);

rtperror _ValidRTPContext(context cid, char *calling_func);

rtperror _GetMemberForContext(context cid, person p, member **the_member_p,
			      char *calling_func);
#ifdef _RTP_DEBUG
#define ValidRTPContext(c, f)		_ValidRTPContext(c, f)
#define GetMemberForContext(c, p, m, f)	_GetMemberForContext(c, p, m, f)
#else
#define ValidRTPContext(c, f)		_ValidRTPContext(c, 0)
#define GetMemberForContext(c, p, m, f)	_GetMemberForContext(c, p, m, 0)
#endif

struct timeval AddTimes(struct timeval *time1, struct timeval *time2);

struct timeval ConvertDoubleToTime(double interval);

double RTPTimeDiff(_RTP_CONTEXT *the_context, 
		   struct timeval *later_time,
		   struct timeval *earlier_time,
		   int8 payload_type);

double ComputeRTCPSendDelay(_RTP_CONTEXT *the_context);

double ComputeSenderRTCPSendDelay(_RTP_CONTEXT *the_context);

double ComputeReceiverRTCPSendDelay(_RTP_CONTEXT *the_context);

int DoRTCPReconsideration(_RTP_CONTEXT *the_context);

void ComputeBlockInfo(_RTP_CONTEXT *the_context, member *the_member,
		      rtcp_report_block *cur_block, u_int32 *expected_ptr);

char *Build_SR_Packet(_RTP_CONTEXT *the_context,
		      char *buffer, struct link** first_member);

char *Build_RR_Packet(_RTP_CONTEXT *the_context,
		      char *buffer, struct link** first_member,
		      int build_empty);

char *Build_RRSR_Packets(_RTP_CONTEXT *the_context, char *buffer);

char *Build_SDES_Packet(_RTP_CONTEXT *the_context, char *buffer,
			char *buffer_top, int padding_block);

char *BuildIndividualByePacket(_RTP_CONTEXT *the_context, char *buffer,
			       char *buffer_top, int padding_block,
			       char *reason, u_int32 ssrc); 


char *BuildCompoundByePacket(_RTP_CONTEXT *the_context, char *buffer,
			     char *buffer_top, int padding_block,
			     char *reason);

rtperror RemapPerson(_RTP_CONTEXT *the_context, person *p);

int SplitAndHostOrderLengthCompoundRTCP(char *rtcppacket, char *indpkts[],
					int len);

#ifndef WORDS_BIGENDIAN

void FlipRTPByteOrdering(char *the_packet, int pktlen);

void FlipRTCPByteOrdering(char *the_packet);

void Flip24(char *the_24bit_thing);

#endif /* WORDS_BIGENDIAN */

#endif /* _RTP_API_INTERNAL_H */
