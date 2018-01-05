/* rtp_api.h : RTP API types, structures, and functions that a user
               of RTP might require

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#ifndef _RTP_API_H
#define _RTP_API_H
#include "../../../include/au_types.h"

#define BITFIELD(field, mask, shift) \
   (((field) & (mask)) >> (shift))
#define SET_BITFIELD(field, val, mask, shift) \
   do { \
     (field) &= ~(mask); \
     (field) |= (((val) << (shift)) & (mask)); \
   } while (0)
  

/* These  define the RTCP packet types */
#define  RTCP_PACKET_SR   200
#define  RTCP_PACKET_RR   201
#define  RTCP_PACKET_SDES 202
#define  RTCP_PACKET_BYE  203
#define  RTCP_PACKET_APP  204

typedef long int context;
typedef long int person;

/* Type sizes */
//typedef long int int32;
//typedef short int int16;
//typedef signed char int8;

typedef unsigned long int u_int32;
typedef unsigned short int u_int16;
typedef unsigned char u_int8;

/* memberstatus is used by the status field in the member structure
   to determine whether a member is currently pending, confirmed,
   or expired */
typedef enum {
  RTP_MEMBER_PENDING,
  RTP_MEMBER_CONFIRMED,
  RTP_MEMBER_EXPIRED,
  RTP_MEMBER_PENDING_CONTRIBUTOR,
  RTP_MEMBER_CONFIRMED_CONTRIBUTOR,
  RTP_MEMBER_EXPIRED_CONTRIBUTOR
} memberstatus;

/* senderstatus determines state of sender. Purported means
   I have received an RR containing them in a report block. 
   Confirmed means I have actually seen a packet from them. Expired
   means I haven't heard from them in a while. Members
   move among these states identically to how they do for member
   state changes */

typedef enum {
  RTP_SENDER_NOT,
  RTP_SENDER_PURPORTED,
  RTP_SENDER_CONFIRMED,
} senderstatus;

typedef struct {
  u_int32 secs;
  u_int32 frac;
} ntp64;


typedef struct {
  u_int8 flags;            /* Protocol version, padding flag, extension flag,
			      and CSRC count */
  u_int8 mpt;              /* marker bit and payload type */
  u_int16 seq;             /* sequence number */
  u_int32 ts;              /* timestamp */
  u_int32 ssrc;            /* synchronization source */
  u_int32 csrc[1];         /* CSRC list */
} rtp_hdr_t;

/* Protocol version */
#define RTP_VERSION(hdr) BITFIELD((hdr).flags, 0xC0, 6)
#define SET_RTP_VERSION(hdr, val) SET_BITFIELD((hdr).flags, (val), 0xC0, 6)

/* Padding flag */
#define RTP_P(hdr) BITFIELD((hdr).flags, 0x20, 5)
#define SET_RTP_P(hdr, val) SET_BITFIELD((hdr).flags, (val), 0x20, 5)

/* Extension flag */
#define RTP_X(hdr) BITFIELD((hdr).flags, 0x10, 4)
#define SET_RTP_X(hdr, val) SET_BITFIELD((hdr).flags, (val), 0x10, 4)

/* CSRC Count */
#define RTP_CC(hdr) BITFIELD((hdr).flags, 0x0F, 0)
#define SET_RTP_CC(hdr, val) SET_BITFIELD((hdr).flags, (val), 0x0F, 0)

/* Marker bit */
#define RTP_M(hdr) BITFIELD((hdr).mpt, 0x80, 7)
#define SET_RTP_M(hdr, val) SET_BITFIELD((hdr).mpt, (val), 0x80, 7)

/* Marker bit */
#define RTP_PT(hdr) BITFIELD((hdr).mpt, 0x7F, 0)
#define SET_RTP_PT(hdr, val) SET_BITFIELD((hdr).mpt, (val), 0x7F, 0)


typedef struct {           /* RTP Header Extension */
  u_int16 etype;           /* extension type */
  u_int16 elen;            /* extension length */
  char epayload[1];        /* extension payload */
} rtp_hdr_ext;



typedef struct {
  rtp_hdr_t *RTP_header;
  rtp_hdr_ext *RTP_extension;
  char *payload;         /* payload portion of RTP packet */
  long payload_len;        /* The length of the payload */
} rtp_packet;



typedef struct {               /* REPORT BLOCK */
  u_int32 ssrc;                /* ssrc identifier */
  unsigned int frac_lost:8;    /* Fraction lost */
  unsigned int cum_lost:24;    /* Cumulative pkts lost */
  u_int32 highest_seqno;       /* highest seqno received */
  u_int32 jitter;              /* interarrival jitter */
  u_int32 lsr;                 /* last SR timestamp */
  u_int32 dlsr;                /* delay since last SR */
} rtcp_report_block;

#define RTCP_REPORT_BLOCK_SIZE (6 * 4)



typedef struct {          /* SDES item */
  u_int32 ssrc;           /* SSRC of the member described by the SDES item */
  int chunkno;            /* The index of the SDES chunk */
  u_int8 type;            /* SDES item type */
  u_int8 len;             /* SDES item length */
  char *description;      /* SDES item description (255 char limit) */
  int offset;             /* Describes the position of the type field
                             octet within int32: for internal use of
                             iterator */
} rtcp_sdes_item;



typedef struct {         /* BYE component */
  u_int32 ssrccsrc;      /* The ssrc or csrc that is leaving */
} rtcp_bye_block;

#define RTCP_BYE_BLOCK_SIZE 4


typedef struct {                 /* Common header of RTCP packets */
  u_int8 flags;                  /* Protocol version, padding flags, and 
				    report block count */
  u_int8 pt;                     /* payload type */
  u_int16 len;                   /* length */
} rtcp_common;

#define RTCP_COMMON_SIZE 4

#define RTCP_VERSION(com) BITFIELD((com).flags, 0xC0, 6)
#define SET_RTCP_VERSION(com, val) SET_BITFIELD((com).flags, (val), 0xC0, 6)

#define RTCP_P(com) BITFIELD((com).flags, 0x20, 5)
#define SET_RTCP_P(com, val) SET_BITFIELD((com).flags, (val), 0x20, 5)

#define RTCP_RC(com) BITFIELD((com).flags, 0x1F, 0)
#define SET_RTCP_RC(com, val) SET_BITFIELD((com).flags, (val), 0x1F, 0)

typedef struct {               /* SR-specific fields -- fixed part */
  u_int32 ssrc;                /* SSRC of sender */
  u_int32 ntp_secs;            /* NTP timestamp -- integer part */
  u_int32 ntp_frac;            /* NTP timestamp -- fractional part */
  u_int32 rtp_stamp;           /* RTP timestamp */
  u_int32 pkt_count;           /* Sender's packet count */
  u_int32 oct_count;           /* Sender's octet count */
} rtcp_sr;

#define RTCP_SR_SIZE (6 * 4)


typedef struct {
  u_int32 ssrc;                /* SSRC of sender */
} rtcp_rr;

#define RTCP_RR_SIZE 4


typedef struct {
  u_int32 ssrccsrc; /* the ssrc or csrc */
  u_int32 name;     /* an ascii name */
} rtcp_app;

#define RTCP_APP_SIZE (2 * 4)


/* An RTCP packet.  This will consist of pointers into
 * the actual packet. */
typedef struct {                     /* RTCP Packet */
  rtcp_common *common;               /* Common headers */

  union {
    struct {                         /* SR specific fields */
      rtcp_sr *sr_fixed;
      rtcp_report_block *rblock[32]; /* report blocks */
      char* extension;
    } sr;

    struct {                         /* RR specific fields */
      rtcp_rr *rr_fixed;
      rtcp_report_block *rblock[32]; /* report blocks */
      char* extension;
    } rr;

    struct {
      char *remainder;
    } sdes;
    
    struct {                         /* BYE specific fields */
      rtcp_bye_block *byeblock[32];  /* bye SSRCs, CSRCs */
      u_int8 reason_length;
      char* reason;
    } bye;

    struct {                         /* APP specific fields */
      rtcp_app *app_fixed;
      char *appdata;                 /* application dependent data */
    } app;

    struct {
      char *data;
    } unknown;

  } variant;
} rtcp_packet;

struct link;

typedef struct { /* Iterator used to walk through lists of members */
  struct link *cur_link;
  int go_forward; /* Should iterate up or down list */
} member_iterator;



/* receiver_report holds the information that one member reported
   about another in its receiver reports (SR or RR packets).
   We refer to the member the receiver report is describing as S (the
   sender), and the member who sent the receiver report as R (the
   receiver). */
typedef struct {
  person reporter;        /* R's unique id */
  u_int8 fraction_lost;   /* fraction of packets sent by S that R lost */
  u_int32 cum_lost;       /* cumulative packets sent by S that R lost */
  u_int32 highest_seqno;  /* highest sequence # that R received from S */
  u_int32 jitter;         /* R's jitter calculation for SR packets from S */
  u_int32 last_sr;        /* LSR (last SR timestamp received at R from S:
                             middle 32 bits of NTP timestamp) */
  u_int32 delay_last_sr;  /* DLSR (delay [in units of 1/65536 seconds]
                             between R receiving the SR from S and R sending
                             the SR / RR packet */
} receiver_report;


/* This mirrors the definition of Tcl_HashSearch in tclHash.h, but without the
   types defined there, so we don't have to expose them in the API. */
typedef struct hashsearch {
  void *table;
  int nextbucket;
  void *nextentry;
} receiver_report_iterator;


/* The valid range for SDES item types */
typedef u_int8 memberinfo;

/* Defined SDES fields. */
/* These reflect the IANA registrations as of 1998-07-01 */
enum {
  RTP_MI_CNAME = 1,
  RTP_MI_NAME = 2,
  RTP_MI_EMAIL = 3,
  RTP_MI_PHONE = 4,
  RTP_MI_LOC = 5,
  RTP_MI_TOOL = 6,
  RTP_MI_NOTE = 7,
  RTP_MI_PRIV = 8,
  RTP_MI_H323_CADDR = 9,
};




/* These enums define the operating modes for reconsideration.
   Unconditional is default, and should not generally be changed. */

typedef enum {
  RTP_RECONSIDERATION_UNCONDITIONAL,
  RTP_RECONSIDERATION_CONDITIONAL,
  RTP_RECONSIDERATION_OFF,
} reconsideration_t;

/* These enums define the encryption modes supported by the library.
   Full encrypts all RTP and RTCP packets. Partial encrypts all RTP, but
   only non-SDES RTCP. None is no encryption. None is default. */

typedef enum {
  RTP_ENCRYPTION_FULL,
  RTP_ENCRYPTION_PARTIAL,
  RTP_ENCRYPTION_NONE,
} encryption_t;

/* For error codes, we are making use of elemedia standard
codes:

Format of return codes for all APIs

   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
  +---+-----------+---------------+-------------------------------+
  |Sev| Facility  | Fac-Sub-Code  |               Code            |
 +---+-----------+---------------+-------------------------------+

  where

      Sev - is the severity code (2 bits)

          00 - Success
          01 - Informational
          10 - Warning
          11 - Error

      R - is a reserved bit   (2 bits)

      Facility - is the facility code ( 6 bits)

              Fac-Sub-Code - is the facility sub code.

      Code - is the facility's status code (16 bits)

      */


#define EM_MASK(BITS,SHIFT)     (((1U << (BITS)) - 1) << (SHIFT))

#define EM_CODE_SHIFT                           0U
#define EM_CODE_BITS                            16U
#define EM_CODE_MASK                            EM_MASK(EM_CODE_BITS,EM_CODE_SHIFT)

#define EM_FACILITY_SUB_CODE_SHIFT       16U
#define EM_FACILITY_SUB_CODE_BITS        8U
#define EM_FACILITY_SUB_CODE_MASK        EM_MASK(EM_FACILITY_SUB_CODE_BITS,\
                                                                                        EM_FACILITY_SUB_CODE_SHIFT)
#define EM_FACILITY_CODE_SHIFT          24U
#define EM_FACILITY_CODE_BITS           6U
#define EM_FACILITY_CODE_MASK           EM_MASK(EM_FACILITY_CODE_BITS,\
                                                                                        EM_FACILITY_CODE_SHIFT)
#define EM_SEVERITY_SHIFT                       30U
#define EM_SEVERITY_BITS                        2U
#define EM_SEVERITY_MASK                        EM_MASK(EM_SEVERITY_BITS,\
                                                                                EM_SEVERITY_SHIFT)

/* Severity codes */

#define EM_SEVERITY_SUCCESS               0U
#define EM_SEVERITY_INFORMATIONAL         1U
#define EM_SEVERITY_WARNING               2U
#define EM_SEVERITY_ERROR                 3U

 /* List of codes common to all modules */

#define EM_PENDING                                        1U
#define EM_INVALID_PARAM                          2U    /* wrong parameter passed               */
#define EM_NOT_IMPLEMENTED                        3U    /* for protocol features.               */
#define EM_MEM_ALLOC_FAILED                       4U    /* not enough memory                    */
#define EM_UNKNOWN_ERROR                          5U    /* no specific error code               */
#define EM_NOT_SUPPORTED                          6U    /* for invalid operations               */
#define EM_NOT_PRESENT                            7U    /* could not get requested item */
#define EM_BUFFER_TOO_SMALL                       8U    /* too little space supplied    */
#define EM_SOFTWARE_EXPIRED                       9U    /* s/w evaluation period expired*/
#define EM_INVALID_FSM_STIMULUS          10U    /* stim incompatible with state */
#define EM_INVALID_RESULT_ARG            11U    /* msgPtr in result not set     */
#define EM_LIBRARY_NOT_INTIALIZED        12U    /* InitLibrary not called               */
#define EM_INSUFFICIENT_INFO             13U    /* not enough information supplied*/
#define EM_INVALID_SEQ_NUMBER            14U    /* sequence number mismatch     */
#define EM_FSM_NO_OPERATION                      15U    /* state machine stim caused NOOP*/
#define EM_CODE_MAX                             256U

/*
 * Well known facility codes
 */

#define FACILITY_RTP                              3U

#define EM_STATUS(SEV,FAC,SUB,CODE)\
        (((SEV) << EM_SEVERITY_SHIFT) & EM_SEVERITY_MASK) |\
        (((FAC) << EM_FACILITY_CODE_SHIFT) & EM_FACILITY_CODE_MASK)     |\
        (((SUB) << EM_FACILITY_SUB_CODE_SHIFT) & EM_FACILITY_SUB_CODE_MASK) |\
        (((CODE) << EM_CODE_SHIFT) & EM_CODE_MASK)

#define RTP_SUCC(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_WARN(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_INFO(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)
#define RTP_ERR(CODE)   EM_STATUS(EM_SEVERITY_SUCCESS, FACILITY_RTP, 0, CODE)



#define EM_RTP_TOO_MANY_CONTEXTS                    (EM_CODE_MAX + 1)
#define EM_RTP_UNKNOWN_CONTEXT                      (EM_CODE_MAX + 2)
#define EM_RTP_NOSOCKET                             (EM_CODE_MAX + 3)
#define EM_RTP_CANT_GET_SOCKET                      (EM_CODE_MAX + 4)
#define EM_RTP_CANT_BIND_SOCKET                     (EM_CODE_MAX + 5)
#define EM_RTP_CANT_SET_SOCKOPT                     (EM_CODE_MAX + 6)
#define EM_RTP_SOCKET_WRITE_FAILURE                 (EM_CODE_MAX + 7)
#define EM_RTP_SOCKET_READ_FAILURE                  (EM_CODE_MAX + 8)
#define EM_RTP_SOCKET_MISMATCH                      (EM_CODE_MAX + 9)
#define EM_RTP_FIXED_WHEN_OPEN                      (EM_CODE_MAX + 10)
#define EM_RTP_BAD_ADDR                             (EM_CODE_MAX + 11)
#define EM_RTP_BAD_PORT                             (EM_CODE_MAX + 12)
#define EM_RTP_CANT_SEND                            (EM_CODE_MAX + 13)
#define EM_RTP_CANT_CLOSE_SESSION                   (EM_CODE_MAX + 14)
#define EM_RTP_BAD_MI                               (EM_CODE_MAX + 15)
#define EM_RTP_BAD_PROFILE                          (EM_CODE_MAX + 16)
#define EM_RTP_SOCKET_EMPTY                         (EM_CODE_MAX + 17)
#define EM_RTP_PACKET_LOOPBACK                      (EM_CODE_MAX + 18)
#define EM_RTP_CSRC_LIST_NOT_SET                    (EM_CODE_MAX + 19)
#define EM_RTP_CSRC_APPEARS_TWICE                   (EM_CODE_MAX + 20)
#define EM_RTP_BAD_PACKET_FORMAT                    (EM_CODE_MAX + 21)
#define EM_RTP_NO_SUCH_PERSON                       (EM_CODE_MAX + 22)
#define EM_RTP_PERSON_DATA_PROTECTED                (EM_CODE_MAX + 23)
#define EM_RTP_NO_SUCH_SENDER                       (EM_CODE_MAX + 24)
#define EM_RTP_NO_SUCH_SDES                         (EM_CODE_MAX + 25)
#define EM_RTP_CANT_USE_ENCRYPTION                  (EM_CODE_MAX + 26)
#define EM_RTP_DECRYPTION_FAILURE                   (EM_CODE_MAX + 27)
#define EM_RTP_END_OF_LIST                          (EM_CODE_MAX + 28)
#define EM_RTP_CANT_CALL_FUNCTION                   (EM_CODE_MAX + 29)
#define EM_RTP_SSRC_IN_USE                          (EM_CODE_MAX + 30)
#define EM_RTP_CANT_GETSOCKNAME                     (EM_CODE_MAX + 31)
#define EM_RTP_NO_SUCH_CONTRIBUTOR                  (EM_CODE_MAX + 32)
#define EM_RTP_CSRC_LIST_FULL                       (EM_CODE_MAX + 33)
#define EM_RTP_DONT_SEND_NOW                        (EM_CODE_MAX + 34)
#define EM_RTP_CANT_SEND_BYE                        (EM_CODE_MAX + 35)



typedef enum {
  RTP_OK =                   0,                                   /* No error */
  RTP_CANT_ALLOC_MEM =       RTP_ERR(EM_MEM_ALLOC_FAILED),         /* Memory allocation failed */
  RTP_TOO_MANY_CONTEXTS =    RTP_ERR(EM_RTP_TOO_MANY_CONTEXTS), /* Max # of contexts exceeded */
  RTP_UNKNOWN_CONTEXT =      RTP_ERR(EM_RTP_UNKNOWN_CONTEXT),         /* Context never created, or destroyed */
  RTP_NOSOCKET =             RTP_ERR(EM_RTP_NOSOCKET),                /* Socket not open for r/w */
  RTP_CANT_GET_SOCKET =      RTP_ERR(EM_RTP_CANT_GET_SOCKET),           /* Can't obtain a new socket */
  RTP_CANT_BIND_SOCKET =     RTP_ERR(EM_RTP_CANT_BIND_SOCKET),          /* Can't bind the socket */
  RTP_CANT_SET_SOCKOPT =     RTP_ERR(EM_RTP_CANT_SET_SOCKOPT),          /* Unable to set certain socket options */
  RTP_SOCKET_WRITE_FAILURE = RTP_ERR(EM_RTP_SOCKET_WRITE_FAILURE),      /* Unable to write to a socket */
  RTP_SOCKET_READ_FAILURE =  RTP_ERR(EM_RTP_SOCKET_READ_FAILURE),       /* Unable to read from a socket */
  RTP_SOCKET_MISMATCH =      RTP_ERR(EM_RTP_SOCKET_MISMATCH),           /* Socket does not belong to session */
  RTP_FIXED_WHEN_OPEN =      RTP_ERR(EM_RTP_FIXED_WHEN_OPEN),           /* change not allowed during open session */
  RTP_BAD_ADDR =             RTP_ERR(EM_RTP_BAD_ADDR),                 /* The address specified is bad */
  RTP_BAD_PORT =             RTP_ERR(EM_RTP_BAD_PORT),                 /* The port specified is bad */
  RTP_CANT_SEND =            RTP_ERR(EM_RTP_CANT_SEND),         /* Sending packets not allowed because
                                                                           destination address was never set */
  RTP_CANT_CLOSE_SESSION =   RTP_ERR(EM_RTP_CANT_CLOSE_SESSION),       /* Unable to close the current session */
  RTP_BAD_MI =               RTP_ERR(EM_RTP_BAD_MI),                   /* No such membership info */
  RTP_BAD_PROFILE =          RTP_ERR(EM_RTP_BAD_PROFILE),              /* Invalid rtpprofile specified */
  RTP_SOCKET_EMPTY =         RTP_WARN(EM_RTP_SOCKET_EMPTY),             /* no data to receive at socket */
  RTP_PACKET_LOOPBACK =      RTP_WARN(EM_RTP_PACKET_LOOPBACK),          /* packet sent from local address with
                                                                           SSRC identical to current SSRC or 
                                                                           identical to the previous SSRC
                                                                           (before a collision).  i.e. it
                                                                           is believed the packet looped back */
                        
  RTP_INSUFFICIENT_BUFFER =  RTP_ERR(EM_BUFFER_TOO_SMALL),      /* allocated buffer too small */
  RTP_CSRC_LIST_NOT_SET =    RTP_ERR(EM_RTP_CSRC_LIST_NOT_SET), /* CSRC list not yet set */
  RTP_CSRC_APPEARS_TWICE=    RTP_WARN(EM_RTP_CSRC_APPEARS_TWICE),       /* 2 CSRCs on list have same SSRC */
  RTP_BAD_PACKET_FORMAT =    RTP_WARN(EM_RTP_BAD_PACKET_FORMAT),        /* Packet has incorrect format */
  RTP_NO_SUCH_PERSON =       RTP_ERR(EM_RTP_NO_SUCH_PERSON),           /* Person not session member */
  RTP_PERSON_DATA_PROTECTED =RTP_ERR(EM_RTP_PERSON_DATA_PROTECTED) ,    /* Not allowed to alter this field of data
                                                                           for this person */
  RTP_NO_SUCH_SENDER =       RTP_ERR(EM_RTP_NO_SUCH_SENDER),           /* No such sender in current session */
  RTP_NO_SUCH_SDES =         RTP_ERR(EM_RTP_NO_SUCH_SDES),             /* No such SDES field */
  RTP_CANT_USE_ENCRYPTION =  RTP_ERR(EM_RTP_CANT_USE_ENCRYPTION),       /* Encryption /decryption cannot be used
                                                                           because the encryption / decryption
                                                                           functions were not set. */
  RTP_DECRYPTION_FAILURE =   RTP_ERR(EM_RTP_DECRYPTION_FAILURE),       /* Packet was unsuccessfully decrypted */
  RTP_END_OF_LIST =          RTP_INFO(EM_RTP_END_OF_LIST),              /* Called by iterator when no more members
                                                                             exist on the list */
  RTP_CANT_CALL_FUNCTION =   RTP_WARN(EM_RTP_CANT_CALL_FUNCTION),       /* Function call not allowed at this
                                                                           time (to prevent recursion that would
                                                                           cause side effects) */
  RTP_SSRC_IN_USE =          RTP_WARN(EM_RTP_SSRC_IN_USE),              /* Attempt to create a CSRC with an
                                                                           SSRC value already used in the CSRC
                                                                           list */
  RTP_CANT_GETSOCKNAME =     RTP_ERR(EM_RTP_CANT_GETSOCKNAME),         /* Couldn't get socket name */
  RTP_BAD_VALUE =            RTP_ERR(EM_INVALID_PARAM),                /* Parameter out of range */
  RTP_NO_SUCH_CONTRIBUTOR =  RTP_ERR(EM_RTP_NO_SUCH_CONTRIBUTOR),      /* CSRC List member set to someone
                                                                            not a contributor */
  RTP_CSRC_LIST_FULL =       RTP_ERR(EM_RTP_CSRC_LIST_FULL),           /* The CSRC list has 15 members */
  RTP_DONT_SEND_NOW =        RTP_INFO(EM_RTP_DONT_SEND_NOW),            /* A packet was reconsidered and should be
                                                                             sent later */
  RTP_ERROR_CANT_SEND_BYE =  RTP_INFO(EM_RTP_CANT_SEND_BYE)             /* An attempt was made to send a BYE packet
                                                                           when no RTP or RTCP has been sent previously */
} rtperror;


typedef enum {
RTP_FLAG_NEW_MEMBER,              /* New Member(s) joined session */
RTP_FLAG_NEW_SENDER,              /* Member (re)started sending */
RTP_FLAG_EXPIRED_MEMBER,          /* Member(s) expired from session */
RTP_FLAG_EXPIRED_SENDER,          /* Member(s) expired as senders, and are
                                     now purported */
RTP_FLAG_MEMBER_LEAVES,           /* Member(s) sent BYE packet */
RTP_FLAG_OBSERVE_COLLISION,       /* Collision detected */
RTP_FLAG_A_CSRC_COLLIDES,         /* Member(s) have same SSRC as a
                                     member in local CSRC list */
RTP_FLAG_UNIQUE_ID_REMAP,         /* A member involved in a collision
                                     returned to the session with
                                     a new SSRC and was initially
                                     thought to be a new member,
                                     and was assigned another
                                     unique ID.  It has just
                                     been realized that this
                                     is in fact not a new member,
                                     and so the API will revert
                                     to using the original unique
                                     ID for the member. */
RTP_FLAG_MEMBER_INFO_CHANGES,     /* One of a member's SDES fields
                                     has changed. */
RTP_FLAG_MEMBER_ALIVE,            /* The member's liveness status
                                     has changed from expired to confirmed. */
RTP_FLAG_MEMBER_CONFIRMED,        /* The members status has changed from
                                     pending to confirmed */
RTP_FLAG_DELETED_PENDING,         /* A pending member has been deleted 
                                     due to soft timeout */
RTP_FLAG_DELETED_MEMBER,          /* An expired member has been deleted */
RTP_FLAG_ADDRESS_CHANGES,         /* A member's IP address has changed. */
RTP_FLAG_COLLIDE_WITH_ME,         /* There is a collision between some member and
                                     the local member */
RTP_FLAG_PURPORTED_SENDER,        /* Information has been received indicating this
                                     user is a sender, although no data packets
                                     have yet been received for them */
RTP_FLAG_DELETED_SENDER,          /* All state information associated with this
                                             sender is now destroyed */
RTP_FLAG_NONE
} rtpflag;

rtperror RTPSessionSetRTPStampRate(context cid, int32 payload_type,
        int32 usec);
rtperror RTPSessionGetRTPStampRate(context cid, int32 payload_type,
        int32 *usec);

rtperror RTPSessionSetKey(context cid, void* value);
rtperror RTPSessionGetKey(context cid, void** value);

rtperror RTPSessionSetReconsideration(context cid, reconsideration_t value);
rtperror RTPSessionGetReconsideration(context cid, reconsideration_t *value);

rtperror RTPSessionSetExtension(context cid, rtp_hdr_ext *the_ext);
rtperror RTPSessionGetExtension(context cid, rtp_hdr_ext **the_ext);

rtperror RTPSessionSetBandwidth(context cid, float session_bwidth,
                                float rtcp_fraction);
rtperror RTPSessionGetBandwidth(context cid, float *session_bwidth,
                                float *rtcp_fraction);

rtperror RTPSessionSetUserInfo(context cid, void* info);
rtperror RTPSessionGetUserInfo(context cid, void** info);

rtperror RTPSessionAddToCSRCList(context cid, u_int32 ssrc);
rtperror RTPSessionRemoveFromCSRCList(context cid, u_int32 ssrc);
rtperror RTPSessionAddToContributorList(context cid, u_int32 ssrc);
/* RemoveFromContributorList is either high-level or low-level */

rtperror RTPSessionGetCSRCList(context cid, u_int32 *value, int32 *size);

rtperror RTPSessionGetUniqueIDForCSRC(context cid, u_int32 ssrc, person *p);

rtperror RTPSessionGetMemberList(context cid, member_iterator *iter);
rtperror RTPSessionGetMemberListLen(context cid, long *the_len);

rtperror RTPSetUpdateMemberCallBack(context cid,
        void (*f)(context, person, rtpflag, char *));

rtperror RTPSetChangedMemberInfoCallBack(context cid,
        void (*f)(context, person, memberinfo, char*, rtpflag));

rtperror RTPSetCollidedMemberCallBack(context cid, 
        void (*f)(context, person, person, rtpflag));

rtperror RTPSetRevertingIDCallBack(context cid, 
        void (*f)(context, person, person, void*, rtpflag));

rtperror RTPCurrentMember(context cid, member_iterator *iter,
                                person *the_member);
rtperror RTPNextMember(context cid, member_iterator *iter,
                                person *the_member);




rtp_packet RTPGetRTPPacket(char *rtppacket, long pktlen);
u_int32 RTPPacketGetCSRC(rtp_packet *rtppkt, int csrc_no);

int RTPSplitCompoundRTCP(char *rtcppacket, char *indpkts[], long len);
rtcp_packet RTPGetRTCPPacket(char *rtcppacket);
void GetFirstSDESItemForSSRC(rtcp_sdes_item *the_item, char* startpoint);
rtcp_sdes_item InitSDESItemIter(rtcp_packet *rtcpsdes);
rtcp_sdes_item GetNextItem(rtcp_sdes_item *prev_item);


rtcp_report_block RTPGetReportBlock(rtcp_packet *rtcprrsr, int blockno);

rtcp_bye_block RTPGetByeBlock(rtcp_packet *rtcpbye, int blockno);

rtperror RTPMemberInfoGetStatus(context cid, person p, memberstatus *s, senderstatus *sender );

rtperror RTPMemberInfoSetSDES(context cid, person p, memberinfo SDES_field,
        char *the_info);
rtperror RTPMemberInfoGetSDES(context cid, person p, memberinfo SDES_field,
        char *the_info);



rtperror RTPMemberInfoSetNTP(context cid, person p, 
                             ntp64 NTP_time);
        
rtperror RTPMemberInfoGetNTP(context cid, person p,
                             ntp64 *NTP_time);


rtperror RTPMemberInfoSetRTP(context cid, person p, 
                int32 rtp_stamp);
        
rtperror RTPMemberInfoGetRTP(context cid, person p,
         int32 *RTP_time);


rtperror RTPMemberInfoSetPktCount(context cid, person p, 
                int32 count);
        
rtperror RTPMemberInfoGetPktCount(context cid, person p,
         int32 *count);


rtperror RTPMemberInfoSetRTCPPktCount(context cid, person p, int32 count);
        
rtperror RTPMemberInfoGetRTCPPktCount(context cid, person p, int32 *count);


rtperror RTPMemberInfoSetOctCount(context cid, person p, 
                int32 count);
        
rtperror RTPMemberInfoGetOctCount(context cid, person p,
         int32 *count);

rtperror RTPMemberInfoSetSSRC(context cid, u_int32 ssrc, u_int32 mask);
        
rtperror RTPMemberInfoGetSSRC(context cid, person p,
         u_int32 *ssrc);

rtperror RTPMemberInfoSetUserInfo(context cid, person p, 
                void* info);
        
rtperror RTPMemberInfoGetUserInfo(context cid, person p,
                void** info);



rtperror RTPSenderInfoGetFirstReceiverReport(context cid, person p,
         receiver_report_iterator *the_iterator, receiver_report *report);

rtperror RTPSenderInfoGetNextReceiverReport(context cid, person p,
         receiver_report_iterator *the_iterator, receiver_report *report);


rtperror RTPSenderInfoGetLocalReception(context cid, person p,
         receiver_report *report);


rtperror RTPFindMember(context cid, memberinfo mi, char *sdes_field,
                 person *p);


rtperror RTPMostRecentRTPTime(context cid, person p,
                              struct timeval *the_time);
rtperror RTPMostRecentRTCPTime(context cid, person p,
                              struct timeval *the_time);
rtperror RTPMostRecentRTPPerson(context cid, person *p);
rtperror RTPMostRecentRTCPPerson(context cid, person *p);



char *RTPStrError(rtperror err);

char *RTPDebugStr(void);
#endif

