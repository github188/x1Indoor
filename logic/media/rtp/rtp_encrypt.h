/* rtp_encrypt.h : functionality used to provide encryption
                     within an RTP session.

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include "rtp_api.h"
#include "rtp_api_ext.h"

#include "rtp_api_internal.h"

#ifdef WE_DO_NETWORK
/* Encrypt a packet that is stored in a struct iovec structure.
   Set IsRTP to TRUE if packet is RTP, to FALSE for RTCP */
rtperror DoEncryption(context cid, struct iovec *pktpart, int pktlen,
		      int IsRTP);
  /* NOTE: We need to add functionality to perform encryption.
     If encryption is performed, then we can place the encrypted
     packet in a single location and do not need to perform
     scatter / gather.  thus, this function should do both
     the encyrption and the sending */
#endif

/* DoDecryption not only decrypts the packet but also removes
   the 32-bit random prefix. */
rtperror DoDecryption(context cid, char *decrypt_buff,
		      long decrypt_bufflen);

int IsValidRTPPacket(_RTP_CONTEXT *the_context, rtp_packet *the_pkt);

int IsValidRTCPPacket(_RTP_CONTEXT *the_context, char **pktpos,
		      int num_parts, long totlen);

