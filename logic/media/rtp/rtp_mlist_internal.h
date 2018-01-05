/* rtp_mlist_internal.h : types and functionality used by functions 
                          to support the RTP library (but mainly more directly
		          to those pertaining to membership lists).

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/


#ifndef _MLIST_INTERNAL_H
#define _MLIST_INTERNAL_H

#include "tclHash.h"
#include "sysdep.h"

/* Defining _RTP_DEBUG will trigger the printing of certain internal
   messages to stdout/stderr */

/*#define _RTP_DEBUG  */



/* Defining _RTP_WATCH_ALLOCATION will trigger printing of messages
   whenever memory is allocated or freed. */

/*#define _RTP_WATCH_ALLOCATION*/


/* if _RTP_SEMI_RANDOM is defined, then things will not be truly random
   to allow testing of SSRC collisions */
/* #define _RTP_SEMI_RANDOM */


/* Number of different SDES fields we support. */
#define _RTP_NUM_SDES_TYPES 12


/* The maximum length of an IPv4-address represented as a string */
/* This has to go here because the 'member' structure uses it. */
#define _RTP_MAX_IP_ADDR_SIZE 16

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "rtp_api.h"
#include "rtp_api_ext.h"



/* rtp_sndr_local contains information that is needed locally
   for each sender.  This structure only needs to be created
   for those members that are senders.

   Much of the information is used to calculate jitter. 
   Calc'ing jitter works as follows:  this structure should
   be created upon receipt of an RTP packet.  The prev_delay
   field should be set and jitter set to -1.

   Thus: when it's time to calculate jitter:
         if the local_info field in the member is NULL, then
	 malloc local info, set jitter to -1 and set 
	 the initial delay to the delay of the first
	 RTP packet.

	 else if jitter is -1, then set the jitter field
	 to the current jitter

	 else set the jitter field to 15/16 of its current
	 value + 1/16 of it's current jitter. */

typedef struct {

  /* The current jitter average */
  double jitter;


  /* RTP time last packet was sent */
  u_int32 prev_sndtime; 

  
  /* gettimeofday() time last packet was received */
  struct timeval prev_rcvtime; 


  /* # of RTP pkts recd from this sender since last SR packet */
  u_int32 num_pkts_recd_since_last;


  /* Total # of RTP pkts rec'd from this sender since session began */
  u_int32 num_pkts_recd;


  /* Sequence # of 1st packet received from this sender */
  u_int32 first_seqno; 

  
  /* Highest sequence # received from this sender so far */
  u_int32 highest_seqno;


  /* expected size of delay interval when last calculated */
  u_int32 expected_prior; 
} rtp_sndr_local;

struct link;

typedef struct mmbr {

  /* The (local) unique ID for this member.  Only 1 member per ID,
     and this ID is made available to the user to access the member.
     The ID is only valid locally.  The local ID for the local member
     is always 0. */
  person unique_id;
  
  /* The member's SSRC (which might change during the session if
     an SSRC collision occurs */
  u_int32 ssrc;

  /* Time that we last heard from a particular member.  last_update_time
     is the most recent time that we received an RTP or RTCP packet from
     the member.  last_rtp_send_time is the most recent time that
     we received an rtp packet, and last_rtcp_send_time is the most
     recent time that we received an RTCP packet from this member.
     last_send_basis_time is the last time we received an RTP packet
     if the sender is confirmed, else the last time we received either
     and RTP or SR, or RR about the user, if they're purported. This
     is used in sender timeouts */

  struct timeval last_update_time;  
  struct timeval last_rtp_send_time;   
  struct timeval last_rtcp_send_time;
  struct timeval last_send_basis_time;

  /* status of the member, i.e. pending, confirmed, or expired */
  memberstatus status;

  /* status of a sender */

  senderstatus sendstatus;


  /* Time that an SR was last received from this member 
     (used to calculate LSR) */
  struct timeval last_sr_receipt_time; 


  /* Data set by the user (must also be destroyed by the user) */
  void* user_data; 


  /* colliding is set to TRUE if the ssrc being used is detected as
     part of a collision.  When a BYE packet is received for a member
     with the colliding flag set, the BYE is not flagged to the user.
     If the member actually does leave, it will be noticed by the
     user upon timeout. */
  int colliding;

  /* Form a linked list with other members that collide with this
     member.  */
  struct mmbr *collides_with_next;
  struct mmbr *collides_with_prev;

  
  /* The internet address this member's most recent RTP/RTCP packet came
     from.  0 is RTP, 1 is RTCP. */
  struct sockaddr my_addr[2];


  /* SDES info (sdes_info[i] points to the sdes info of type i+1 e.g.
     CNAME is sdes_info[0]) */
  char *sdes_info[_RTP_NUM_SDES_TYPES]; 



  /* Sender only fields.  The following fields are only used when this
     member is a sender.  The information is obtained from the sender's
     SR packets and the information applies to its sending RTP packets*/
  u_int32 rtp_timestamp; 
  ntp64 ntp_timestamp;
  u_int32 pkt_count; 
  u_int32 oct_count;
  /* The local sender information maintained about this member; we
     use this to generate RTCP SR/RRs. */
  rtp_sndr_local *local_info;
  /* A hash of all SR/RRs (including our own) that we've seen about this
     member. */
  Tcl_HashTable RR_Hash;

  /* This is a local counter of received RTP packets. It is used
     only for deciding on membership state, not for RTCP generation */
  u_int32 recv_rtp_count;

  /* Local counter of received RTP packets for CSRC. It is used
     for membership state */

  u_int32 recv_csrc_count;

  /* Information that this member has about other senders in the session */
  /*  sr_info *sr_itemlist; */

  /* The number of senders in the sr_itemlist */
  /*  int itemlistsize; */

  /* This is a local counter of received (or, for the local member, sent)
   * RTCP packets.  It is used only for informational value. */
  u_int32 rtcp_count;


  /* on_csrc_list is set to TRUE iff the member is currently on
     the csrc list */
  int on_csrc_list;

  /* internal pointer structures.  These pointers are used to connect this
   member to other members in different orderings.  The different orderings
   are: */
  
  /* Memberlist: ordered by current expiry time (expiry time changes
     when packets are received from the member */
  struct link *mmbr_list_link;

  /* Senderlist: ordered by current sender expiry time (ordered by
     # of RTCP packets received without getting RTP packet */
  struct link *sndr_list_link;


  /* Hash table entry for hashing on ssrc */
  struct link *ssrc_hash; 

  /* Hash table entry for hashing on unique ID */
  struct link *unique_id_hash; 

  /* Hash table entry for hashing on CNAME */
  struct link *cname_hash;

} member;


struct dll;

struct link {

  /* Link structure is a generic link in a doubly-linked list.  It points
     to the member in the list, the next link, the previous link, and
     the list that contains the link */
  member *my_member;
  struct link *next_link;
  struct link *prev_link;
  struct dll *in_list;
};

typedef struct dll {
  /* doubly linked list */
  struct link *first_link;
  struct link *last_link;
  long list_size;
} doubly_linked_list;


typedef struct {
   /* Membership list.  A separate type is created for this
      list because a count must be kept track for the
      "confirmed" members on the list, as well as a pointer to the 
      oldest member that has yet to soft-timeout */
   doubly_linked_list actual_list;
   struct link *oldest_not_yet_soft;
   long confirmed_members;
} membership_list;

#define HASHSIZE 1024

typedef struct {
  /* A hash table is just an array of doubly linked lists.  The hash
     function maps to one of the lists, and that list contains all
     the entries that are hashed to the same value */
  doubly_linked_list hashval[HASHSIZE];
} hash_table;


#include "rtp_api_internal.h"

  
/* InitList should be used to construct a new list */  
void InitList(doubly_linked_list *l);

/* Builds a link for a member.  The link is initially not joined into
any list.  The user must bind the member to the link e.g.
the_member->mmbr_list_link = create_link(the_member).  Similarly,
when the link is destroyed, this pointer should be set to NULL. */
struct link *CreateLink(member *the_member);

/* PullMember removes the_link from list l (but does not
   destroy the link).  Returns false if the_link was not
   a member of l */
int PullMember(doubly_linked_list *l, struct link *the_link);

/* Insert link the_link into list l behind link prev_on_list.
   If prev_on_list is NULL, then l is inserted into the front
   of the list.  A requirement is that the_link is currently not a member
   of any list. FALSE is returned if we detect it as part of a list.
   Note that we cannot always catch these instances. */
int UnsortedInsertMember(doubly_linked_list *l,
			  struct link *the_link,
			  struct link *prev_on_list);

/* Insert link the_link into list l sorted in increasing order by
   function sort_func().  sort_func(x,y) < 0 for x<y, >0 for x>y, and
   = 0 for x==y.  The sorting begins at link start_point.  If
   start_point is set to NULL, the sorting starts at the end of the
   list.  Note that choosing the choice of start_point does not effect
   where in the list the_link is placed.  It allows the user to
   optimize the sort by trying to start near where the link is most
   likely to go.  The rules about adding link to list l are similar to
   those for the UnsortedInsertMember() function */
int SortedInsertMember(_RTP_CONTEXT *the_context, doubly_linked_list *l, 
		       struct link *the_link,
		       int (*sort_func)(_RTP_CONTEXT *the_context,
					struct link* compare_me,
					struct link* to_me),
		       struct link *start_point);



/*************************************************
  Functionality for the membership list 
  ************************************************/

void MembershipListInit(membership_list *l);

int MembershipListPullMember(membership_list *l, struct link *the_link);
int SenderListPullMember(membership_list *l, struct link *the_link);

int MembershipListInsertMember(membership_list *l,
			  struct link *the_link);
int SenderListInsertMember(membership_list *l,
			  struct link *the_link);

void ChangeMemberStatus(_RTP_CONTEXT *the_context, 
		       member *m, memberstatus new_status);
void ChangeSenderStatus(_RTP_CONTEXT *the_context, 
		       member *m, senderstatus new_status);




/*********************************
 * Functionality for hash tables *
 *********************************/

void InitSSRCHashTable(_RTP_CONTEXT *the_context);
void DeleteSSRCHashTable(_RTP_CONTEXT *the_context);
int EnterSSRCHashTable(_RTP_CONTEXT *the_context, member *the_member);
member *GetMemberFromSSRCHash(_RTP_CONTEXT *the_context, u_int32 ssrc);
int RemoveMemberFromSSRCHash(_RTP_CONTEXT *the_context, member *the_member);

void InitUniqueIDHashTable(_RTP_CONTEXT *the_context);
void DeleteUniqueIDHashTable(_RTP_CONTEXT *the_context);
int EnterUniqueIDHashTable(_RTP_CONTEXT *the_context, member *the_member);
member *GetMemberFromUniqueIDHash(_RTP_CONTEXT *the_context, person unique_id);
int RemoveMemberFromUniqueIDHash(_RTP_CONTEXT *the_context, member *the_member);

void InitCNAMEHashTable(_RTP_CONTEXT *the_context);
void DeleteCNAMEHashTable(_RTP_CONTEXT *the_context);
int EnterCNAMEHashTable(_RTP_CONTEXT *the_context, member *the_member);
member *GetMemberFromCNAMEHash(_RTP_CONTEXT *the_context, char* cname);
int RemoveMemberFromCNAMEHash(_RTP_CONTEXT *the_context, member *the_member);



/* The following functions are to be used with receiver_report hash tables: */

void Init_RR_Hash(member *the_member);
void Delete_RR_Hash(member *the_member);
receiver_report *Update_RR(member *reporter, member *sender,
			rtcp_report_block *the_block);
void Clear_Member_RRs(_RTP_CONTEXT *the_context, member *the_member);
void Merge_RR_Hashes(member* prev_member, member *new_member);

/* Memory allocation. */

#ifdef _RTP_WATCH_ALLOCATION

void* my_malloc(size_t size);
void* my_calloc(size_t num, size_t size);
void my_free(void* tofree);

#define malloc(size) my_malloc(size)
#define calloc(num, size) my_calloc(num, size)
#define free(tofree) my_free(tofree)

#endif

#endif
