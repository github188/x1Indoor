#ifndef _RTP_HIGHLEVEL_INTERNAL_H
#define _RTP_HIGHLEVEL_INTERNAL_H

#include "rtp_api.h"
#include "rtp_api_ext.h"

#ifdef WIN32
#include "rtp_win32.h"
#endif

#ifdef __unix
#include "rtp_unix.h"
#endif

/* The default TTL used for a multicast channel (currently cannot
   set TTL field of unicast channel */
#define _RTP_DEFAULT_TTL 128

/* Default encryption NONE */
#define _RTP_DEFAULT_ENCRYPTION RTP_ENCRYPTION_NONE

/* Maximum packet size (size of buffers we use to store packets */
#define _RTP_MAX_PKT_SIZE 10000

/* The maximum length of a UDP port number represented as a string */
/* We overestimate a bit. */
#define _RTP_MAX_PORT_STR_SIZE 8

/* Default port is 5000 */
#define _RTP_DEFAULT_PORT 5000

/* Maximum number of tries to allocate a dynamic RTP port */

#define _BIND_COUNTER  20

/* Starting UDP port for dynamic ports */

#define _UDP_PORT_BASE  49152

/* Range of UDP dynamic ports */

#define _UDP_PORT_RANGE 16382

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

/* address_holder type
 *
 *  This is used to hold addresses to send to for RTP and RTCP packets.
 *  It is a linked list of addresses, ports, and TTL's.
 */

typedef struct address_holder {
  struct in_addr address;        /* IP address, in network order */
  u_int16 port;                  /* Port number, network order */
  u_int8 ttl;                    /* ttl */
  socktype rtpsocket;         /* Socket for data */
  socktype rtcpsocket;        /* socket for rtcp */
  struct address_holder *next;   /* next address in list */
  int deleteflag;                /* Set to 1 when to be deleted */
} address_holder_t;


typedef struct {
  /*********************************
   * Fields that cannot be altered *
   * during a connection           *
   *********************************/

  /* A boolean that is set to true when the connection is opened
     (i.e. between the time when RTPOpenConnection() and RTPCloseConnection()
     have been called). */
  int connection_opened; 

  /* This holds the list of addresses to send to */
  address_holder_t *send_addr_list;

  /* This is the local address to listen to */
  address_holder_t *recv_addr_list;

  /* The local source addresses for RTP and RTCP, if we're sending to a
     multicast group; else AF_UNSPEC address family.  Used for loopback
     detection. */
  struct sockaddr_in rtp_sourceaddr;
  struct sockaddr_in rtcp_sourceaddr;

  /******************************
   * Fields that can be altered *
   * during a session.          *
   ******************************/

  /* Whether or not encryption is being used */
  encryption_t use_encryption;

  /* The key used for encryption / decryption */
  void* key;

  /* By setting PreventEntryIntoFlaggingFunctions to true, those
     functions that return flags (and make callbacks) cannot be
     entered.  This prevents recursive looping of the functions
     calling callbacks which call the function, etc. */
  int PreventEntryIntoFlaggingFunctions;

  /*******************************
   * Encryption function storage *
   *******************************/
  void (*encrypt_initfunc)(context, void*);
  void (*encrypt_encryptfunc)(context, char*, int, void*);
  void (*encrypt_decryptfunc)(context, char*, int, void*);

  /*****************************
   * Callback function storage *
   *****************************/
  void (*SendErrorCallBack)(context, char *, u_int16, u_int8);
  void (*ChangedMemberAddressCallBack)(context, person, char *, char *, int);


  /* A large buffer that we use to construct packets */
  char packet_buffer[_RTP_MAX_PKT_SIZE];
  //add by zhengk
  char SendFlag; //0-每一个发送地址都有一个发送socket与之关联, 1-发送就用RTP/和RTCP socke

} hl_context;

/* Defined in host2ip.c */

/* Convert a string, representing a dotted decimal or host name, into
   network byte ordered address. */
struct in_addr host2ip(char *);


#endif
