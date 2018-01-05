/* rtp_api_internal.c : Default settings, internal structures, and
                        various functionality required to support the
                        functionality in rtp_api.h and rtp_api.c
Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#define _RTP_API_INTERNAL_C

#include "sysdep.h"

#include "rtp_api_internal.h"
#include "rtp_mlist.h"
#include "rtp_encrypt.h"
#include "rtp_collision.h"

_RTP_CONTEXT **ContextList = NULL;

long _RTP_context_above_used = 0;

long _RTP_contexts_in_use = 0;

rtperror _RTP_cur_err; 
char _RTP_err_msg[256];


int _RTP_PAYLOAD_CLOCK_CONVERSIONS[_RTP_MAX_PAYLOAD_TYPES] = 
{125, 125, 125, 125, 125, 125, 63, 125, 125, 63, /* 0-9 */
 23, 23, 125, 125, 11, 125, 91, 45, 125, 0,      /* 10-19 */
 0, 0, 0, 0, 0, 11, 11, 0, 11, 0,                /* 20-29 */
 0, 11, 11, 11, 11, 0, 0, 0, 0, 0,               /* 30-39 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 40-49 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 50-59 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 60-69 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 70-79 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 80-89 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 90-99 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 100-109 */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* 110-119 */
 0, 0, 0, 0, 0, 0, 0, 0};                        /* 120-127 */



/* Set default payload rates to those as specified in
   draft-ietf-avt-profile-new-01 as of May 15, 1997 */
void SetDefaultPayloadRates(_RTP_CONTEXT *the_context){
  int i;
  for (i=0; i<_RTP_MAX_PAYLOAD_TYPES;i++){
    the_context->profileRTPTimeRates[i] = 
      (int32) _RTP_PAYLOAD_CLOCK_CONVERSIONS[i];
  }
  return;
}



/* Validate that a putative RTP context actually exists.
   If not, set error messages and return appropriately.*/
rtperror _ValidRTPContext(context cid, char *calling_func)
{
  if (cid >= _RTP_context_above_used){
    return errordebug(RTP_UNKNOWN_CONTEXT, calling_func,
		      "unknown context %d", (int)cid);
  }
  if (ContextList[cid] == NULL){
    return errordebug(RTP_UNKNOWN_CONTEXT, calling_func,
		      "context %d currently not in use", (int)cid);
  }
  return RTP_OK;
}



/* Retrieve the member for a putative person within a given context, or
   complain. */
rtperror _GetMemberForContext(context cid, person p, member **the_member_p,
			      char *calling_func)
{
  if (p==0){
    *the_member_p = ContextList[cid]->my_memberinfo;
  }
  else *the_member_p = GetMemberFromUniqueIDHash(ContextList[cid], p);
  if (*the_member_p==NULL){
    return errordebug(RTP_NO_SUCH_PERSON, calling_func,
		      "context %d person %ld does not exist", (int)cid, p);
  }
  return RTP_OK;
}




/* Add two timevals.
   Does not handle overflow of tv_sec (year-2038 problem) */
struct timeval AddTimes(struct timeval *time1, struct timeval *time2){
  struct timeval addedtimes;
  addedtimes.tv_sec = time1->tv_sec + time2->tv_sec;
  addedtimes.tv_usec = time1->tv_usec + time2->tv_usec;
  if (addedtimes.tv_usec > 999999){ /* tv_usec overflow */
    addedtimes.tv_sec++;
    addedtimes.tv_usec -= 1000000;
  }
  return addedtimes;
}


  
/* Returns TRUE if init_time + interval <= cur_time, i.e., if
   an expiration period has elapsed. */
int TimeExpired(struct timeval *init_time, struct timeval *cur_time,
		struct timeval *interval){
  struct timeval exptime;
  exptime = AddTimes(init_time, interval);
  if (exptime.tv_sec < cur_time->tv_sec){
    return TRUE;
  }
  if (exptime.tv_sec > cur_time->tv_sec){
    return FALSE;
  }
  if (exptime.tv_usec <= cur_time->tv_usec){
    return TRUE;
  }
  return FALSE;
}



/* Convert double to struct timeval; double is in terms of seconds */
struct timeval ConvertDoubleToTime(double interval){
  struct timeval thetime;
  thetime.tv_sec = (long) interval;
  thetime.tv_usec = (long) (1000000 * (interval - (long) interval));
  return thetime;
}

/* Convert struct timeval to NTP timestamp. */
static ntp64 ConvertTimevalToNTP(struct timeval tv)
{
  ntp64 n;

  /* Convert microseconds to 2^-32's of a second (the lsw of an NTP
   * timestamp).  This uses the factorization
   * 2^32/10^6 = 4096 + 256 - 1825/32 which results in a max conversion
   * error of 3 * 10^-7 and an average error of half that.
   *
   * Taken from vic sources.
   */
  u_int32 t = (tv.tv_usec * 1825) >> 5;
  n.frac = ((tv.tv_usec << 12) + (tv.tv_usec << 8) - t);
  
  n.secs = (unsigned long) tv.tv_sec + _GETTIMEOFDAY_TO_NTP_OFFSET;

  return n;
}


/* Returns time difference in RTP scale between two NTP timestamps.
   The result is in milliseconds and so should be an integer. */    
double RTPTimeDiff(_RTP_CONTEXT *the_context, 
		  struct timeval *later_time,
		  struct timeval *earlier_time,
		  int8 payload_type){
  double diff = (double) (later_time->tv_sec - earlier_time->tv_sec) *
    1000000 + (double) (later_time->tv_usec - earlier_time->tv_usec);
  if (the_context->profileRTPTimeRates[(int)payload_type] == 0){
    return 0;
  }
  return (diff / (double) the_context->profileRTPTimeRates[(int)payload_type]);
}



/* Compute what the current delay interval (in seconds) between RTCP packets
 * would be for the session described by the_context.
 * The 'sender' variable determines whether we calculate sender or receiver
 * delays.
 * This does not include the random variation in the delay.
 */
static double InternalComputeRTCPSendDelay(_RTP_CONTEXT *the_context,
					   int sender){
  long competition;
  double the_delay;
  double rtcp_min_time = _RTCP_MIN_TIME;
  double rtcp_bw; 
  if (the_context->initial){
    rtcp_min_time /= 2.0;
    the_context->avg_rtcp_size = 128;
  }
  
  /* If there were active senders, give them at least a minimum share
     of RTCP bandwidth.  Otherwise, all participants share the RTCP
     bandwidth equally */
  competition = the_context->RTP_MemberList.confirmed_members;
  rtcp_bw = the_context->session_bandwidth * the_context->rtcp_fraction *
    128; /* mult by 128 to convert for bytes / sec */
  if (the_context->RTP_SenderList.confirmed_members > 0 &&
      the_context->RTP_SenderList.confirmed_members < competition *
      the_context->sender_bw_fraction){
    if (sender){
      rtcp_bw *= the_context->sender_bw_fraction;
      competition = the_context->RTP_SenderList.confirmed_members;
    }
    else {
      rtcp_bw *= 1.0 - the_context->sender_bw_fraction;
      competition -= the_context->RTP_SenderList.confirmed_members;
    }
  }

  /* The effective number of sites times the average packet size is
     the total number of octets sent when each site sends a report.
     Dividing this by the effective bandwidth gives the time
     interval over which those packets must be sent in order to meet
     the bandwidth target, with a minimum enforced.  In that time
     interval we send one report so this time is also our average
     time between reports. */
  the_delay = the_context->avg_rtcp_size * competition / rtcp_bw;
  if (the_delay < rtcp_min_time){
    the_delay = rtcp_min_time;
  }
  
   return the_delay;
}



/* Compute our own RTCP interval, based on whether or not we're a sender. */
double ComputeRTCPSendDelay(_RTP_CONTEXT *the_context){
  int sender = (the_context->my_memberinfo->sendstatus == RTP_SENDER_CONFIRMED);

  return InternalComputeRTCPSendDelay(the_context, sender);
}



/* Compute the RTCP interval for any sender. */
double ComputeSenderRTCPSendDelay(_RTP_CONTEXT *the_context){
  return InternalComputeRTCPSendDelay(the_context, TRUE);
}



/* Compute the RTCP interval for any receiver */
double ComputeReceiverRTCPSendDelay(_RTP_CONTEXT *the_context){
  return InternalComputeRTCPSendDelay(the_context, FALSE);
}


/* Perform RTCP Reconsideration.  Return TRUE if the packet should be sent
   now, FALSE if it should be deferred. */
int DoRTCPReconsideration(_RTP_CONTEXT *the_context)
{
  /* The current group size refers to the number of confirmed
     members.  */
  int32 cur_group_size = the_context->RTP_MemberList.confirmed_members;
  struct timeval rtcp_delay;
  struct timeval cur_time, send_time;
  int send_it;

  gettimeofday(&cur_time, NULL);
    
  switch (the_context->reconsideration){
  case RTP_RECONSIDERATION_OFF: /* no reconsideration performed */
    send_it = TRUE;
    break;
  case RTP_RECONSIDERATION_CONDITIONAL: 
    /* If group size did not grow, no reconsideration performed */
    if (cur_group_size <= the_context->prev_group_size){
      send_it = TRUE;
      break;
    }
    /* Otherwise FALL THROUGH */
  case RTP_RECONSIDERATION_UNCONDITIONAL:
  default:
    the_context->random_factor = .5 + drand48();
    rtcp_delay = ConvertDoubleToTime(ComputeRTCPSendDelay(the_context) *
				     the_context->random_factor);
    if (!TimeExpired(&the_context->last_rtcp_send, &cur_time, &rtcp_delay)){
      /* Not ready to send.  Put it back on the timer list with
	 its new time to send */
      send_time = AddTimes(&the_context->last_rtcp_send, &rtcp_delay);
      RTPSetTimer(the_context->context_num, RTP_TIMER_SEND_RTCP, 0, NULL,
		  &send_time);
      send_it = FALSE;
    }
    else {
      send_it = TRUE;
    }
  }
  the_context->prev_group_size = cur_group_size;
  return(send_it);
}




/* Compute the values for SR or RR packets for the_member, based
 * on the statistics we have observed about them.
 *
 * This is used both by BuildBlockInfo (below) and the various API calls
 * that enquire about member statistics. */
void ComputeBlockInfo(ARG_UNUSED(_RTP_CONTEXT *,the_context),
		      member *the_member, rtcp_report_block *cur_block,
		      u_int32 *expected_ptr)
{
  struct timeval cur_time;
  long int deltime, udeltime;
  ntp64 lsr_ntp;
  u_int32 expected, expected_interval, lost_interval, cum_lost;

  cur_block->ssrc = the_member->ssrc;


  /* Compute cumulative lost and fraction lost */
  expected = the_member->local_info->highest_seqno -
    the_member->local_info->first_seqno + 1;

  if (expected_ptr != NULL) {
    /* BuildBlockInfo needs the value of expected */
    *expected_ptr = expected;
  }
  cum_lost = (expected - 
	      (the_member->local_info->num_pkts_recd +
	       the_member->local_info->num_pkts_recd_since_last));

  /* 24-bit quantity; clamp at 0xffffff rather than wrap around. */
  cur_block->cum_lost = (cum_lost <= 0xffffff ? cum_lost : 0xffffff);

  expected_interval = expected - the_member->local_info->expected_prior;

  lost_interval = expected_interval -
    the_member->local_info->num_pkts_recd_since_last;
  if (expected_interval == 0 || lost_interval <= 0){
    cur_block->frac_lost = 0;
  }
  else {
    cur_block->frac_lost = (lost_interval << 8) / expected_interval;
  }

  cur_block->highest_seqno = the_member->local_info->highest_seqno;

  /* Jitter update done in UpdateMemberInfoByRTP -- it needs to be updated
     with each RTP packet that is received. */
  cur_block->jitter = (u_int32) the_member->local_info->jitter;

  /* compute lsr and dlsr */
  if (the_member->last_sr_receipt_time.tv_sec == 0) {
    cur_block->lsr = 0;
    cur_block->dlsr = 0;
  } else {
    lsr_ntp = ConvertTimevalToNTP(the_member->last_sr_receipt_time);

    cur_block->lsr = (u_int32) (lsr_ntp.secs << 16 | lsr_ntp.frac >> 16);

    gettimeofday(&cur_time, NULL);
    deltime = cur_time.tv_sec - the_member->last_sr_receipt_time.tv_sec;
    udeltime = cur_time.tv_usec -
      the_member->last_sr_receipt_time.tv_usec;
    cur_block->dlsr = (u_int32)
      (65536.0 * ((double) deltime + (double) udeltime / 1000000.0));
  }

  return;
}

/* Build a report block for SR and RR packets for the user with the given
   ssrc, based on the statistics we have observed about them. */
static void BuildBlockInfo(_RTP_CONTEXT *the_context, u_int32 ssrc,
			   rtcp_report_block *cur_block){
  member *the_member = GetMemberFromSSRCHash(the_context, ssrc);
  u_int32 expected;

  if (the_member == NULL){
    the_member = EstablishNewMember(the_context, ssrc, NULL, NULL,
				    FALSE, RTP_MEMBER_PENDING);
#ifdef _RTP_DEBUG
  printf("EstablishNewMember in BuildBlockInfo for member id %ld (SSRC 0x%lx)\n",
	 the_member->unique_id, ssrc);
#endif
  }
  if (the_member->local_info == NULL){
#ifdef _RTP_DEBUG
    printf("Null local_info for member id %ld (SSRC 0x%lx)\n",
	   the_member->unique_id, ssrc);
#endif
    /* no info exists, so send everything empty */
    /* XXX: Should never happen -- we shouldn't send a block for someone we
       haven't accumulated information about.  However, it does -- some
       cases with purported senders.  BUG */
    cur_block->ssrc = ssrc;
    cur_block->frac_lost = 0;
    cur_block->cum_lost = 0;
    cur_block->highest_seqno = 0;
    cur_block->jitter = 0;
    cur_block->lsr = 0;
    cur_block->dlsr = 0;
    return;
  }

  ComputeBlockInfo(the_context, the_member, cur_block, &expected);

#ifdef _RTP_DEBUG
  printf("Computed block info for member id %ld (SSRC 0x%lx)\n",
	 the_member->unique_id, ssrc);
#endif

  /* Update the_info's fields that represent information since the
     last send of an RTCP packet.  These include: expected_prior and
     num_pkts_recd_since_last, */
  the_member->local_info->expected_prior = expected;
  the_member->local_info->num_pkts_recd +=
    the_member->local_info->num_pkts_recd_since_last;
  the_member->local_info->num_pkts_recd_since_last = 0;

  /* Update the receiver_reports for this member as though we had received
     this block in an RR packet. */
  Update_RR(the_context->my_memberinfo, the_member, cur_block);
  
  return;
}



/* Zero-pad a buffer to an padding-block byte boundary, for encryption. */
/* N.B.: blocksize must be > 0, and probably should be at least a multiple
   of 4 and a power of 2.  8 is standard (DES); 16 is developing (AES, RC-6) */
static int AddPad(char* cur, int cur_tot_len, int padding_block){
  int pad = padding_block - (cur_tot_len % padding_block);
  if (pad == padding_block){
    return 0;
  }
  memset(cur, 0, pad - 1);
  cur[pad-1] = (char) pad;
  return pad;
}
  
  
/* Build an SR packet and store it in the buffer.
 * Returns a pointer to where the packet ends, to make it easy to compound
 * packets in the buffer.
 */
char *Build_SR_Packet(_RTP_CONTEXT *the_context,
		      char *buffer, struct link **first_sender_link){
  struct timeval cur_time;
  ntp64 cur_time_ntp;
  rtcp_packet the_packet;
  struct link *cur_link = *first_sender_link;
  int num_senders = 0;
  int pktlen;
  char *p = buffer;

  the_packet.common = (rtcp_common *) p;
  p += RTCP_COMMON_SIZE;
  the_packet.variant.sr.sr_fixed = (rtcp_sr *) p;
  p += RTCP_SR_SIZE;

  SET_RTCP_VERSION(*the_packet.common, _RTP_VERSION);
  SET_RTCP_P(*the_packet.common, 0); /* SR packets are never padded */
  the_packet.common->pt = RTCP_PACKET_SR;
  the_packet.variant.sr.sr_fixed->ssrc = the_context->my_ssrc;
  gettimeofday(&cur_time, NULL);
  cur_time_ntp = ConvertTimevalToNTP(cur_time);
  the_packet.variant.sr.sr_fixed->ntp_secs = cur_time_ntp.secs;
  the_packet.variant.sr.sr_fixed->ntp_frac = cur_time_ntp.frac;

  /* NOTE: update time-elpased field based on NTP change in time */
  the_packet.variant.sr.sr_fixed->rtp_stamp = 
    the_context->init_RTP_timestamp + the_context->time_elapsed;
  the_packet.variant.sr.sr_fixed->pkt_count =
    the_context->sending_pkt_count;
  the_packet.variant.sr.sr_fixed->oct_count =
    the_context->sending_octet_count;

  while (num_senders < 31 && cur_link != NULL){
    /* Don't send an RR for anyone except a confirmed sender,
       and also don't send an RR for ourselves ! */

    if((cur_link->my_member->sendstatus == RTP_SENDER_CONFIRMED) &&
       (cur_link->my_member->unique_id != 0)) {
      the_packet.variant.sr.rblock[num_senders] = (rtcp_report_block *) p;
      p += RTCP_REPORT_BLOCK_SIZE;
      BuildBlockInfo(the_context, cur_link->my_member->ssrc,
		     the_packet.variant.sr.rblock[num_senders]);
      num_senders++;
    }
    cur_link = cur_link->next_link;

  }
  SET_RTCP_RC(*the_packet.common, num_senders);
  /* SR packets should never be padded since are always multiple
     lengths of 32 bits */
  *first_sender_link = cur_link;
  pktlen = 6 + sizeof(rtcp_report_block) * num_senders / 4;
  the_packet.common->len = htons((u_int16) pktlen);
    
#ifndef WORDS_BIGENDIAN
  FlipRTCPByteOrdering(buffer);
#endif

  return buffer + 4 * (pktlen+1);
}  


/* Build an RR packet and store it in the buffer.
 * Returns a pointer to where the packet ends, to make it easy to compound
 * packets in the buffer.
 * The build_empty parameter is used for partial DES encryption --
 * it forces the RR packet to contain no info except the minimum
 * required. */
char *Build_RR_Packet(_RTP_CONTEXT *the_context,
		      char *buffer, struct link **first_sender_link,
		      int build_empty){
  rtcp_packet the_packet;
  struct link *cur_link = *first_sender_link;
  int num_senders = 0;
  int pktlen;
  char *p = buffer;

  the_packet.common = (rtcp_common *) p;
  p += RTCP_COMMON_SIZE;
  the_packet.variant.rr.rr_fixed = (rtcp_rr *) p;
  p += RTCP_RR_SIZE;
  
  SET_RTCP_VERSION(*the_packet.common, _RTP_VERSION);
  SET_RTCP_P(*the_packet.common, 0); /* RR packets are never padded */
  the_packet.common->pt = RTCP_PACKET_RR;
  the_packet.variant.rr.rr_fixed->ssrc = the_context->my_ssrc;

  while (num_senders < 31 && cur_link != NULL && !build_empty) {
    /* Don't send an RR for anyone except a confirmed sender,
       and also don't send an RR for ourselves! */

    if((cur_link->my_member->sendstatus == RTP_SENDER_CONFIRMED) &&
       (cur_link->my_member->unique_id != 0)) {
      the_packet.variant.rr.rblock[num_senders] = (rtcp_report_block *) p;
      p += RTCP_REPORT_BLOCK_SIZE;
      BuildBlockInfo(the_context, cur_link->my_member->ssrc,
		     the_packet.variant.rr.rblock[num_senders]);
      num_senders++;
    }
    cur_link = cur_link->next_link;
  }

  /* Above fixed part of header is 8 bytes.  Must also offset
     for the rtcp report block part */
  SET_RTCP_RC(*the_packet.common, num_senders);
  pktlen = 1 + RTCP_REPORT_BLOCK_SIZE * num_senders / 4;

  the_packet.common->len = htons((u_int16) pktlen);
  *first_sender_link = cur_link;

#ifndef WORDS_BIGENDIAN
  FlipRTCPByteOrdering(buffer);
#endif

  return buffer + 4 * (pktlen + 1);
}

/* Build all the RR/SR packets for an RTCP packet. */
char *Build_RRSR_Packets(_RTP_CONTEXT *the_context, char *buffer) {
  struct link *senderlink;
  char *cur_pos = buffer;

  senderlink = the_context->RTP_SenderList.actual_list.first_link;
  do {
    if (the_context->my_memberinfo->sendstatus == RTP_SENDER_CONFIRMED){
      /* I am a sender, so make the first packet an SR packet */
      cur_pos = Build_SR_Packet(the_context, cur_pos, &senderlink);
      /* Declare this to be the last time we "received" an SR packet
	 from ourselves. */
      gettimeofday(&the_context->my_memberinfo->last_sr_receipt_time, NULL);
    }
    else cur_pos = Build_RR_Packet(the_context, cur_pos, &senderlink,
				   FALSE);
  } while (senderlink != NULL);
  /* This sends extra RR / SR pkts until sender list has been exhausted. */

  return cur_pos;
}


/* Place the given member's member's SDES fields into the buffer.
   Returns the new buffer offset */
static int PlaceSDESInfoForMember(member *the_member, char *buffer,
				  int init_buffer_offset){
  u_int32 *ssrc_ptr;
  int tot_byte_len = init_buffer_offset;
  int i;
  ssrc_ptr = (u_int32*)(buffer + tot_byte_len);
  *ssrc_ptr = ntohl(the_member->ssrc);
  tot_byte_len += 4;
  for (i = 0; i < _RTP_NUM_SDES_TYPES; i++){
    if (the_member->sdes_info[i] != NULL){
      *(buffer + tot_byte_len) = i+1;
      *(buffer + tot_byte_len + 1) = 
	strlen(the_member->sdes_info[i]);
      strcpy(buffer + tot_byte_len + 2,
	     the_member->sdes_info[i]);
      tot_byte_len += 2 + strlen(the_member->sdes_info[i]);
    }
  }

  if (tot_byte_len == init_buffer_offset+4){
    /* No SDES items were filled in.  Might as well just skip over
       this SSRC */
    return init_buffer_offset;
  }
  /* Otherwise pad with 0's until reach a 32 bit boundary.  Pad with
     between 1 and 4 0's */
  do {
    *(buffer + tot_byte_len) = '\0';
    tot_byte_len++;
  }
  while (tot_byte_len % 4 != 0);
  return tot_byte_len;
}



/* Build an SDES packet and store it in the buffer.
 * Returns a pointer to where the packet ends, to make it easy to compound
 * packets in the buffer.
 * If padding_block is non-zero, the packet will provide padding
 * to make the total length 0 mod (padding_block).
 */
char *Build_SDES_Packet(_RTP_CONTEXT *the_context, char *buffer,
			char *buffer_top, int padding_block){
  /* NOTE: currently every supplied SDES field will be sent
     in the SDES packet. */
  rtcp_packet the_packet;
  int padding,rcount;
  member *the_member;
  int tot_byte_len;
  int new_byte_len;
  contributor_t *cont, temp;
  person p;

  /* Prepend the local member to the contriblist temporarily.
     This way, the code that follows just walks through the list */

  temp.ssrc = the_context->my_ssrc;
  temp.next = the_context->contriblist;
  rcount = 0;
  cont = &temp;


  while(cont != NULL) {
    the_packet.common = (rtcp_common *) buffer;
    SET_RTCP_VERSION(*the_packet.common, _RTP_VERSION);
    SET_RTCP_P(*the_packet.common, 0); /* changed if set during encryption */
    the_packet.common->pt = RTCP_PACKET_SDES;
    tot_byte_len = 4;

    while((cont != NULL) && (rcount < 31)) {
      if(cont->ssrc == the_context->my_ssrc) {
	the_member = GetMemberFromUniqueIDHash(the_context, 0);
      } else {
	RTPSessionGetUniqueIDForCSRC(the_context->context_num, cont->ssrc, &p);
	the_member = GetMemberFromUniqueIDHash(the_context, p);
      }
      if (the_member == NULL){
	the_member = EstablishNewMember(the_context, cont->ssrc,
					NULL, NULL, FALSE, RTP_MEMBER_PENDING);
      }
      new_byte_len = PlaceSDESInfoForMember(the_member, buffer,
					    tot_byte_len);
      rcount++;
      if (new_byte_len == tot_byte_len){
	/* Skipped over SDES info for this member.  Don't count it in
	   the # of CSRCs with SDES info */
	rcount--;
      } else tot_byte_len = new_byte_len;
      cont = cont->next;
    }
    SET_RTCP_RC(*the_packet.common, rcount);
    if((padding_block > 0) && (cont == NULL)) {
      padding = AddPad(buffer+tot_byte_len, 
		       buffer+tot_byte_len - buffer_top,
		       padding_block);
      if (padding > 0){
	tot_byte_len += padding;
	SET_RTCP_P(*the_packet.common, 1);
      }

    }

    the_packet.common->len = htons((u_int16)(tot_byte_len / 4 - 1));

#ifndef WORDS_BIGENDIAN
    FlipRTCPByteOrdering(buffer);
#endif

    rcount = 0;
    buffer += tot_byte_len;
  }

  return buffer;
}



/* Build a BYE packet for one member, and place it in the buffer.
 * Returns a pointer to where the packet ends, to make it easy to compound
 * packets in the buffer.
 * ssrc is the SSRC for whom the BYE is from - either a CSRC, or the local
 * member in case of a collision.
 * reason is the BYE reason string for leaving.
 * If padding_block is non-zero, the packet will provide padding
 * to make the total length 0 mod (padding_block).
 *
 * We don't validate that the specified csrc actually is one of ours, since
 * it may have changed (in the case of a collision).
 */
char *BuildIndividualByePacket(ARG_UNUSED(_RTP_CONTEXT *,the_context),
			       char *buffer, char *buffer_top,
			       int padding_block, char *reason, u_int32 ssrc)
{ 
  rtcp_packet the_packet;
  int padding,length,nullinserts;
  char *cur_pos, *len_field;

  cur_pos = buffer;
  the_packet.common = (rtcp_common *) cur_pos;
  cur_pos += RTCP_COMMON_SIZE;

  SET_RTCP_VERSION(*the_packet.common, _RTP_VERSION);
  SET_RTCP_P(*the_packet.common, 0); /* changed if set during encryption */
  the_packet.common->pt = RTCP_PACKET_BYE;
  SET_RTCP_RC(*the_packet.common, 1);

  the_packet.variant.bye.byeblock[0] = (rtcp_bye_block *) cur_pos;
  cur_pos += RTCP_BYE_BLOCK_SIZE;

  the_packet.variant.bye.byeblock[0]->ssrccsrc = ssrc;

  len_field = cur_pos;
  cur_pos++;

  if(reason != NULL) {
    length = strlen(reason);
    if(length > 255)
      length = 255;
    strncpy(cur_pos, reason, length);
    cur_pos += length;
    *len_field = length;
    length += 1;
  } else {
    *len_field = 0;
    length = 1;
  }
  
  /* Now, length has been set to the number of octets
     written, including the len field. if this number is 
     not a multiple of 4, we must add extra null terminations */

  nullinserts = 4 - (length % 4);
  if(nullinserts == 4) nullinserts = 0;
  memset(cur_pos, 0, nullinserts);
  
  cur_pos += nullinserts;

  padding = 0;
  if (padding_block > 0) {
    padding = AddPad(cur_pos, cur_pos - buffer_top, padding_block);
    if (padding > 0){
      SET_RTCP_P(*the_packet.common, 1);
    }
  }
  the_packet.common->len = htons((u_int16)((cur_pos + padding -
					    buffer - 4) / 4));
  
#ifndef WORDS_BIGENDIAN
  FlipRTCPByteOrdering(buffer);
#endif

  buffer = cur_pos + padding;
  return buffer;
}


/* Build a BYE packet for the local member and all CSRC's.
 * Returns a pointer to where the packet ends, to make it easy to compound
 * packets in the buffer.
 * reason is the BYE reason string for leaving.
 * If padding_block is non-zero, the packet will provide padding
 * to make the total length 0 mod (padding_block).
 */
char *BuildCompoundByePacket(_RTP_CONTEXT *the_context, char *buffer,
			     char *buffer_top, int padding_block,
			     char *reason){ 
  rtcp_packet the_packet;
  int padding,length,nullinserts,rcount;
  char *cur_pos, *len_field;
  member *the_member;
  contributor_t *cont, temp;
  person p;

  /* Build a list of SSRC to be put in the BYE packet.
     Include the local member and the contributor list. */

  temp.ssrc = the_context->my_ssrc;
  temp.next = the_context->contriblist;

  rcount = 0;
  cont = &temp;

  /*generate multiple BYE packets, until we have run out
    of SSRC to make them for */

  while(cont != NULL) {
    cur_pos = buffer;
  
    the_packet.common = (rtcp_common *) cur_pos;
    cur_pos += RTCP_COMMON_SIZE;
    SET_RTCP_VERSION(*the_packet.common, _RTP_VERSION);
    SET_RTCP_P(*the_packet.common, 0); /* changed if set during encryption */
    the_packet.common->pt = RTCP_PACKET_BYE;

    while((cont != NULL) && (rcount < 31)) {
      /* ??? Why is this necessary? */
      if(cont->ssrc == the_context->my_ssrc) {
	the_member = GetMemberFromUniqueIDHash(the_context, 0);
      } else {
	RTPSessionGetUniqueIDForCSRC(the_context->context_num, cont->ssrc, &p);
	the_member = GetMemberFromUniqueIDHash(the_context, p);
      }
      if (the_member == NULL){
	EstablishNewMember(the_context, cont->ssrc,
			   NULL, NULL, FALSE, RTP_MEMBER_PENDING);
      }
      the_packet.variant.bye.byeblock[rcount] = (rtcp_bye_block *) cur_pos;
      cur_pos += RTCP_BYE_BLOCK_SIZE;

      the_packet.variant.bye.byeblock[rcount]->ssrccsrc = cont->ssrc;
      rcount++;
      cont = cont->next;
    }
    SET_RTCP_RC(*the_packet.common, rcount);
    len_field = cur_pos;
    cur_pos++;

    if(reason != NULL) {
      length = strlen(reason);
      if(length > 255)
	length = 255;
      strncpy(cur_pos, reason, length);
      cur_pos += length;
      *len_field = length;
      length += 1;
    } else {
      *len_field = 0;
      length = 1;
    }
  
  /* Now, length has been set to the number of octets
     written, including the len field. if this number is 
     not a multiple of 4, we must add extra null terminations */

    nullinserts = 4 - (length % 4);
    if(nullinserts == 4) nullinserts = 0;
    memset(cur_pos, 0, nullinserts);

    cur_pos += nullinserts;

    padding = 0;
    if((padding_block > 0) && (cont == NULL)) {
      padding = AddPad(cur_pos, cur_pos - buffer_top, padding_block);
      if (padding > 0){
	SET_RTCP_P(*the_packet.common, 1);
      }
    }
    the_packet.common->len = htons((u_int16)((cur_pos + padding -
                                              buffer - 4) / 4));

#ifndef WORDS_BIGENDIAN
    FlipRTCPByteOrdering(buffer);
#endif

    rcount = 0;
    buffer = cur_pos + padding;
  }

  return buffer;
}



/* Given the person id p, return the current canonical identifier for that
 * member.  (Person ids can become invalid as members are merged. */
rtperror RemapPerson(_RTP_CONTEXT *the_context, person *p){
  int i;
  for (i=0; i < the_context->num_remaps; i++){
    if (the_context->ID_remap[i] == *p){
      *p = the_context->ID_orig_val[i];
      RemapPerson(the_context, p);
      return RTP_OK;
    }
  }
  return RTP_OK;
}



/* Does the same thing as the API call RTPSplitCompoundRTCP except that it
   also places the length field of the compound packets in network
   byte order. */
int SplitAndHostOrderLengthCompoundRTCP(char *rtcppacket, char *indpkts[],
					int len){
  /* If packet is encrypted, then the extra 32-bit header used
     for encryption should have already been stripped off */
  int numpkts = 0;
  char* curloc = rtcppacket;
  u_int16 cur_rtcp_pktlen;
  while(len > 0){
    indpkts[numpkts] = curloc;
    numpkts++;
    /* set cur_rtcp_pktlen = length field of numpkts'th RTCP packet
       in the compound packet */
    cur_rtcp_pktlen = htons(*((u_int16*) ((char*)curloc + 2)));
    *((u_int16*) ((char*)curloc + 2)) = cur_rtcp_pktlen;
    len -= 4 * cur_rtcp_pktlen + 4;
    curloc += 4 * cur_rtcp_pktlen + 4;
  }
#ifdef _RTP_DEBUG
  if (len < 0) {
    printf("SplitAndHostOrderLengthCompoundRTCP: len (== %d) < 0\n",
           len);
  }
#endif
  return numpkts;
}


#ifndef WORDS_BIGENDIAN /* Various byte-ordering functions */

/* Flip the byte ordering of RTP packets.
 */
void FlipRTPByteOrdering(char *the_packet, int pktlen)
{
  rtp_hdr_t *the_hdr = (rtp_hdr_t*) the_packet;
  rtp_packet the_pkt;

  the_hdr->seq = ntohs(the_hdr->seq);
  the_hdr->ts = ntohl(the_hdr->ts);
  the_hdr->ssrc = ntohl(the_hdr->ssrc);
  the_pkt = RTPGetRTPPacket(the_packet, pktlen);
  /* SSRC and CSRC stay in network byte order */
  if (the_pkt.RTP_extension != NULL){
    the_pkt.RTP_extension->etype =
      ntohs(the_pkt.RTP_extension->etype);
    the_pkt.RTP_extension->elen =
      ntohs(the_pkt.RTP_extension->elen);
  }
  return;
}
 
/* Fix the byte ordering of an SR or RR report block */
static void FixReportBlockOrdering(rtcp_report_block *rb)
{
  rb->ssrc = ntohl(rb->ssrc);
  Flip24(((char*) rb) + 5);
  rb->highest_seqno = ntohl(rb->highest_seqno);
  rb->jitter = ntohl(rb->jitter);
  rb->lsr = ntohl(rb->lsr);
  rb->dlsr = ntohl(rb->dlsr);
}



/* Fix the byte ordering of received RTCP packets.
 * is_nw_to_host should be set to true when converting from host to
 * network order; when going the other direction it should be set to false.
 */
void FlipRTCPByteOrdering(char *buffer)
{
  int i, rc;
  rtcp_packet the_packet;

  the_packet = RTPGetRTCPPacket(buffer);
  rc = RTCP_RC(*the_packet.common);

  /* length had to be modified during splitting packet (see
    SplitAndHostOderLengthCompoundRTCP(), so it is not adjusted
    in here.  when building a packet, it must be adjusted in
    the building function call. */
  switch(the_packet.common->pt) {
  case RTCP_PACKET_SR:
    the_packet.variant.sr.sr_fixed->ssrc =
      ntohl(the_packet.variant.sr.sr_fixed->ssrc);

    the_packet.variant.sr.sr_fixed->ntp_secs = 
      ntohl(the_packet.variant.sr.sr_fixed->ntp_secs);
    the_packet.variant.sr.sr_fixed->ntp_frac = 
      ntohl(the_packet.variant.sr.sr_fixed->ntp_frac);
    the_packet.variant.sr.sr_fixed->rtp_stamp = 
      ntohl(the_packet.variant.sr.sr_fixed->rtp_stamp);
    the_packet.variant.sr.sr_fixed->pkt_count = 
      ntohl(the_packet.variant.sr.sr_fixed->pkt_count);
    the_packet.variant.sr.sr_fixed->oct_count = 
      ntohl(the_packet.variant.sr.sr_fixed->oct_count);

    for (i = 0; i < rc; i++) {
      FixReportBlockOrdering(the_packet.variant.sr.rblock[i]);
    }
    break;

  case RTCP_PACKET_RR:
    the_packet.variant.rr.rr_fixed->ssrc =
      ntohl(the_packet.variant.rr.rr_fixed->ssrc);
  
    for (i = 0; i < rc; i++) {
      FixReportBlockOrdering(the_packet.variant.rr.rblock[i]);
    }
    break;

  case RTCP_PACKET_BYE:
    for (i = 0; i < rc; i++) {
      the_packet.variant.bye.byeblock[i]->ssrccsrc =
	ntohl(the_packet.variant.bye.byeblock[i]->ssrccsrc);
    }    
    break;

  case RTCP_PACKET_APP:
    the_packet.variant.app.app_fixed->ssrccsrc =
      ntohl(the_packet.variant.app.app_fixed->ssrccsrc);
    the_packet.variant.app.app_fixed->name =
      ntohl(the_packet.variant.app.app_fixed->name);
    break;

  case RTCP_PACKET_SDES:
  default:
    /* SDES packets, and unknown packet types, have no overlay data to
       reorder */
    break;
  }
}




/* Flip a 24-bit quantity on a little-endian machine -- byte 3 becomes
 * byte 1 and vice-versa.
 * This is sufficient for the cum_packets_lost field in the RTCP report
 * block.
 */
void Flip24(char *the_24bit_thing){  
  char temp = *the_24bit_thing;
  the_24bit_thing[0] = the_24bit_thing[2];
  the_24bit_thing[2] = temp;
  return;
}

#endif

/* Debugging */

#ifdef _RTP_DEFINE_EDEBUG

#ifdef _RTP_DEBUG
rtperror errordebug(rtperror err, char *func, char *msg, ...)
{
  va_list args;
  int len;

  _RTP_cur_err = err;
#ifdef HAVE_SNPRINTF
  len = snprintf(_RTP_err_msg, sizeof(_RTP_err_msg), "Call from %s(): ", func);
#else
  len = sprintf(_RTP_err_msg, "Call from %s(): ", func);
#endif

#ifdef HAVE_VSNPRINTF
  va_start(args, msg);
  vsnprintf(_RTP_err_msg + len, sizeof(_RTP_err_msg) - len, msg, args);
  va_end(args);
#else
  va_start(args, msg);
  vsprintf(_RTP_err_msg + len, msg, args);
  va_end(args);
#endif
  return err;
}

#else

rtperror errordebug(rtperror err, ARG_UNUSED(char *,func),
		    ARG_UNUSED(char *, msg), ...)
{
  _RTP_cur_err = err;
  return err;
}

#endif
#endif
