/* rtp_encrypt.c : functionality used to provide encryption
                     within an RTP session.

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include "sysdep.h"

#include "rtp_api_internal.h"
#include "rtp_encrypt.h"

#ifdef UNDEF
#undef UNDEF
#endif

#ifdef UNDEF /* ... */
rtperror DoEncryption(context cid, struct iovec *pktpart, int pktlen,
		      int IsRTP){
  /* Encrypts and sends packet */
  /* NOTE: Can't use _RTP_Bufferspace to hold encrypted packet because
     the RTCP packet is already in _RTP_Bufferspace */
  char encryptbuf[_RTP_MAX_PKT_SIZE];
  int32 random_header = random32(cid);
  int tot_len, rundelete;
  int i, errchk;
  int use_socket;
  address_holder_t *s, *prevs;
  rtperror err;
  

#ifdef _RTP_DEBUG
  printf("Encrypting ");
  if (IsRTP){
    printf("RTP\n");
  }
  else printf("RTCP\n");
#endif


  err = RTP_OK;
  if (ContextList[cid]->encrypt_encryptfunc == NULL){
    return errordebug(RTP_CANT_USE_ENCRYPTION, "DoEncryption",
		      "context %d encryption function not set",
		      (int)cid);
    return RTP_CANT_USE_ENCRYPTION;
  }
  /* Now copy the data into the buffer where DES can then be performed */
  memcpy(encryptbuf, (char*) &random_header, sizeof(random_header));
  tot_len = sizeof(random_header);
  for (i=0; i < pktlen; i++){   
    memcpy(&encryptbuf[tot_len], (char*)  pktpart[i].iov_base, 
	   pktpart[i].iov_len);
    tot_len += pktpart[i].iov_len;
  }


  /* NOTE: Here is where we want to call the encryption algorithm */

  ContextList[cid]->encrypt_encryptfunc(cid, encryptbuf,
					(int) tot_len, 
					ContextList[cid]->key);

  /* Now send the packet */
  s = ContextList[cid]->send_addr_list;
  rundelete = FALSE;
  while(s != NULL) {

    if(IsRTP)
      use_socket = s->rtpsocket;
    else
      use_socket = s->rtcpsocket;

    if(s->deleteflag == FALSE) {
      errchk = send(use_socket, encryptbuf, tot_len, 0);
    

      if (errchk < 0){
	err = errordebug(RTP_SOCKET_WRITE_FAILURE, "DoEncryption",
			 "context %d couldn't send encrypted packet",
			 (int)cid);
	/* XXX need prevent entry flag set */
	if (ContextList[cid]->SendErrorCallBack != NULL) {
	  ContextList[cid]->SendErrorCallBack(cid,
					      inet_ntoa(s->address),
					      ntohs(s->port),
					      s->ttl);
	}
      }
    } else {
      rundelete = TRUE;
    }

    s = s->next;
  }

  prevs = NULL;
  if(rundelete == TRUE) {
    s = ContextList[cid]->send_addr_list;

    while(s != NULL) {
      if(s->deleteflag == TRUE) {

	if(prevs == NULL) 
	  ContextList[cid]->send_addr_list = s->next;
	else
	  prevs->next = s->next;

	close(s->rtpsocket);
	close(s->rtcpsocket);
	my_free(s);
      }
      prevs = s;
      s = s->next;
    }
  }


  return err;
}
#endif

#ifdef UNDEF /* ... */
/* DoDecryption should not only decrypt the packet but it should also
   shift the packet over the 32 bit random prefix */
rtperror DoDecryption(context cid, char *decrypt_buff,
		      long decrypt_bufflen){
  int i;

#ifdef _RTP_DEBUG
  printf("Decrypting\n");
#endif

  if (ContextList[cid]->encrypt_decryptfunc == NULL){
    return errordebug(RTP_CANT_USE_ENCRYPTION, "DoDecryption",
		      "context %d decryption function not set",
		      (int)cid);
  }

  ContextList[cid]->encrypt_decryptfunc(cid, decrypt_buff, 
					(int) decrypt_bufflen, 
					ContextList[cid]->key);

  for (i=0; i < decrypt_bufflen - 4; i++){
    decrypt_buff[i] = decrypt_buff[i+4];
  } 
  return RTP_OK;
}
#endif

int IsValidRTPPacket(ARG_UNUSED(_RTP_CONTEXT*, the_context),
		     rtp_packet *the_pkt){
  if (RTP_VERSION(*the_pkt->RTP_header) != _RTP_VERSION){
    return FALSE;
  }
  /* NOTE: Need to validate payload type */

  /* pt cannot be SR or RR.  But current payload won't even allow that 
   because it's only 7 bytes long */
  /*  if (the_pkt->RTP_header->pt == RTCP_PACKET_RR || the_pkt->RTP_header->pt == RTCP_PACKET_RR){
    return FALSE;
  } */


  /* NOTE: Check for valid sequence # */

  return TRUE;
}

int IsValidRTCPPacket(ARG_UNUSED(_RTP_CONTEXT*, the_context), char **pktpos, 
		      int num_parts, long totlen){
  int i;
  long cur_len = 0;
  rtcp_packet the_pkt;
  for (i=0; i< num_parts; i++){
    the_pkt = RTPGetRTCPPacket(pktpos[i]);
    if (RTCP_VERSION(*the_pkt.common) != _RTP_VERSION){
#ifdef _RTP_DEBUG
      printf("RTCP wrong version: %d in pkt %d\n",
	     RTCP_VERSION(*the_pkt.common), i);
#endif
      return FALSE;
    }
    /* NOTE: Need to validate payload type */
    
    if (i == 0){
      
      /* First pkt in compound should not have padding. 
	 NOTE: This might not be true if all pkts encrypted
	 except SDES pkts.  Might need to pad this first packet
	 to encrypt the rest. */
      if (RTCP_P(*the_pkt.common) != 0){
#ifdef _RTP_DEBUG
	printf("RR or SR packet padded\n");
#endif
	return FALSE;
      }
    }
    
    cur_len += the_pkt.common->len * 4 + 4;
  }
  if (cur_len != totlen){
#ifdef _RTP_DEBUG
    printf("Packet has wrong length\n");
#endif
    return FALSE;
  }
  return TRUE;
  
}


