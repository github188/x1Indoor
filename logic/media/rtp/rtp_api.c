/* rtp_api.c: RTP API types, structures, and functions that a user
				 of RTP might require

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include "sysdep.h"

#include "rtp_api_internal.h"
#include "rtp_mlist.h"
#include "rtp_api.h"
#include "rtp_api_ext.h"

#include "rtp_encrypt.h"
#include "rtp_collision.h"
#include "rtp_lowlevel.h"

rtperror RTPLowLevelCreate(context *the_context)
{
	context i;
	InitRandom();

	if (_RTP_contexts_in_use < _RTP_context_above_used)
	{
		/* Not all contexts #'s that have been allocated in the ContextList
			 are in use, so grab one of those. */
		for (i=0; i < _RTP_context_above_used; i++)
		{
			if (ContextList[i] == NULL)
			{
				break;
			}
		}
	}
	else if (ContextList==NULL)
	{
		i = _RTP_context_above_used;
		_RTP_context_above_used = _RTP_INIT_CONTEXTS_AVAIL;
		ContextList = (_RTP_CONTEXT**) calloc(_RTP_INIT_CONTEXTS_AVAIL,	sizeof(_RTP_CONTEXT*));
	}
	else
	{
		/* Need to increase the size of the ContextList. */
		i = _RTP_context_above_used;
		_RTP_context_above_used += _RTP_CONTEXT_INC;
		ContextList = realloc(ContextList,
				sizeof(_RTP_CONTEXT*) * _RTP_context_above_used);
	}

	ContextList[i] = malloc(sizeof(_RTP_CONTEXT));
	_RTP_contexts_in_use++;
	if (ContextList[i]==NULL)
	{
		return errordebug(RTP_CANT_ALLOC_MEM, "RTPLowLevelCreate", "%ld contexts in use",
					_RTP_contexts_in_use);
	}
	ContextList[i]->context_num = i;
	/* We do not set any defaults for the following fields, because
		 they must be filled in by the user, or are filled in when
		 RTPOpenConnection() is called: IP_address, RTP_port,
		 RTCP_port, RTP_socket, RTCP_socket, 
		 key, user_info */
	
	ContextList[i]->CSRCList = NULL;
	ContextList[i]->CSRClen = 0;
	ContextList[i]->contriblist = NULL;
	ContextList[i]->unique_id_counter = 0;
	ContextList[i]->num_remaps = 0;
	ContextList[i]->reconsideration = RTP_RECONSIDERATION_UNCONDITIONAL;
	ContextList[i]->prev_group_size = 0;
	ContextList[i]->session_bandwidth = _RTP_DEFAULT_BANDWIDTH;
	ContextList[i]->rtcp_fraction = (float) _RTP_DEFAULT_RTCP_FRAC;
	ContextList[i]->sender_bw_fraction = _RTP_DEFAULT_SENDER_BW_FRAC;
	SetDefaultPayloadRates(ContextList[i]);
	
	
	/* NOTE: Which of these should be done at the opening
		 of each connection? */
	ContextList[i]->init_RTP_timestamp = random32(i);
	ContextList[i]->time_elapsed = 0;
	/* The sequence # should only use the first 16 bits.
		 The rest is the extension, so keep the extension
		 part initially at 0 */
	ContextList[i]->seq_no = random32(i) % 65536;
	
	ContextList[i]->init_seq_no = ContextList[i]->seq_no;

	/* SSRC is not set until first packet is sent.	However,
		 for now we set the SSRC to 0 so that we can establish a member.
		 We will move the member in the SSRC hash once we change
		 the SSRC */
	ContextList[i]->static_ssrc = 0;
	ContextList[i]->ssrc_mask = 0;
	ContextList[i]->my_ssrc = 0;

	ContextList[i]->hdr_extension = NULL;
	
	/* Initially, nobody has sent me any packets. */
	ContextList[i]->most_recent_rtp_sender = NULL;
	ContextList[i]->most_recent_rtcp_sender = NULL;

	ContextList[i]->most_recent_addr.sa_family = _RTP_ADDRESS_NOT_YET_KNOWN;
	
	ContextList[i]->PreventEntryIntoFlaggingFunctions = FALSE;
	ContextList[i]->UpdateMemberCallBack = NULL;
	ContextList[i]->ChangedMemberInfoCallBack = NULL;
	ContextList[i]->ChangedMemberSockaddrCallBack = NULL;
	ContextList[i]->CollidedMemberCallBack = NULL;
	ContextList[i]->RevertingIDCallBack = NULL;
	
	RTP_Membership_Initialize(ContextList[i]);
	
	/* Place myself on the memberlist */
	ContextList[i]->my_memberinfo = 
		EstablishNewMember(ContextList[i], ContextList[i]->my_ssrc, NULL,
					 NULL, FALSE, RTP_MEMBER_PENDING);

	/* I should always be confirmed */
	ChangeMemberStatus(ContextList[i], ContextList[i]->my_memberinfo,
				 RTP_MEMBER_CONFIRMED);

	ContextList[i]->my_memberinfo->my_addr[0].sa_family =
		_RTP_ADDRESS_NOT_YET_KNOWN;
	ContextList[i]->my_memberinfo->my_addr[1].sa_family =
		_RTP_ADDRESS_NOT_YET_KNOWN;

	gettimeofday(&ContextList[i]->last_rtcp_send, NULL);
	
	*the_context = i;
	return RTP_OK;
}


rtperror RTPLowLevelDestroy(context cid){
	rtperror err;
	contributor_t *con, *tempcon;

	if ((err = ValidRTPContext(cid, "RTPDestroy")) != RTP_OK)
		return err;

	/* Destroy all members and links */
	CleanMembershipList(ContextList[cid], &ContextList[cid]->RTP_MemberList);
	CleanMembershipList(ContextList[cid], &ContextList[cid]->RTP_SenderList);
	DeleteSSRCHashTable(ContextList[cid]);
	DeleteUniqueIDHashTable(ContextList[cid]);
	DeleteCNAMEHashTable(ContextList[cid]);

	/* Remove the CSRC list (if it exists) */
	if (ContextList[cid]->CSRCList != NULL){
		free(ContextList[cid]->CSRCList);
	}

	/* Remove the contributor list (if it exists) */
	con = ContextList[cid]->contriblist;
	while(con != NULL) {
		tempcon = con;
		con = con->next;
		free(tempcon);
	}

	/* Mark the context as unused */
	free (ContextList[cid]);
	ContextList[cid] = NULL;
	_RTP_contexts_in_use--;
	return RTP_OK;
}

rtperror RTPSessionSetRTPStampRate(context cid, int32 payload_type,
					 int32 usec){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetRTPStampRate")) != RTP_OK)
		return err;
	if (payload_type >= _RTP_MAX_PAYLOAD_TYPES){
		return errordebug(RTP_BAD_PROFILE, "RTPSessionSetRTPStampRate",
					"Payload type %ld not allowed", payload_type);
	}
	ContextList[cid]->profileRTPTimeRates[payload_type] = usec;
	return RTP_OK;
}

rtperror RTPSessionGetRTPStampRate(context cid, int32 payload_type,
					 int32 *usec){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetRTPStampRate")) != RTP_OK)
		return err;
	if (payload_type >= _RTP_MAX_PAYLOAD_TYPES){
		return errordebug(RTP_BAD_PROFILE, "RTPSessionGetRTPStampRate",
					"Payload type %ld not allowed", payload_type);
	}
	*usec =	 ContextList[cid]->profileRTPTimeRates[payload_type];
	return RTP_OK;
}


rtperror RTPSessionSetReconsideration(context cid, reconsideration_t value){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetReconsideration")) != RTP_OK)
		return err;

	ContextList[cid]->reconsideration = value;
	return RTP_OK;
}

rtperror RTPSessionGetReconsideration(context cid, reconsideration_t *value){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetReconsideration")) != RTP_OK)
		return err;

	*value =	ContextList[cid]->reconsideration;
	return RTP_OK;
}

rtperror RTPSessionSetExtension(context cid, rtp_hdr_ext *the_ext){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetExtension")) != RTP_OK)
		return err;
	ContextList[cid]->hdr_extension = the_ext;
	return RTP_OK;
}

rtperror RTPSessionGetExtension(context cid, rtp_hdr_ext **the_ext){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetExtension")) != RTP_OK)
		return err;
	*the_ext = ContextList[cid]->hdr_extension;
	return RTP_OK;
}


rtperror RTPSessionSetBandwidth(context cid, float session_bwidth,
				float rtcp_fraction){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetBandwidth")) != RTP_OK)
		return err;
	if ((session_bwidth > 0.0) && (rtcp_fraction > 0.0)) {
		/* Disallow invalid or meaningless bandwidth values. */
		/* Note: in a future version we will allow bandwidth or fraction 0,
			 which will mean "never send RTCP packets".*/
		/* Note: we should check for NaN and Infinity here too, but I'm not sure
			 how to do that portably. */
		ContextList[cid]->session_bandwidth = session_bwidth;
		ContextList[cid]->rtcp_fraction = rtcp_fraction;
		return RTP_OK;
	} else {
		return errordebug(RTP_BAD_VALUE, "RTPSessionSetBandwidth",
					"context %d, invalid bandwidth and/or fraction %f / %f",
					(int)cid, session_bwidth, rtcp_fraction);
	}
}

rtperror RTPSessionGetBandwidth(context cid, float *session_bwidth,
				float *rtcp_fraction){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetBandwidth")) != RTP_OK)
		return err;
	*session_bwidth = ContextList[cid]->session_bandwidth;
	*rtcp_fraction = ContextList[cid]->rtcp_fraction;
	return RTP_OK;
}

rtperror RTPSessionSetUserInfo(context cid, void *info){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetUserInfo")) != RTP_OK)
		return err;
	ContextList[cid]->user_data = info;
	return RTP_OK;
}

rtperror RTPSessionGetUserInfo(context cid, void **info){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetUserInfo")) != RTP_OK)
		return err;
	*info = ContextList[cid]->user_data;
	return RTP_OK;
}


rtperror RTPSessionSetHighLevelInfo(context cid, void *info){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionSetHighLevelInfo")) != RTP_OK)
		return err;
	ContextList[cid]->nd_data = info;
	return RTP_OK;
}

rtperror RTPSessionGetHighLevelInfo(context cid, void **info){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetHighLevelInfo")) != RTP_OK)
		return err;
	*info = ContextList[cid]->nd_data;
	return RTP_OK;
}


rtperror RTPStartSession(context cid) {
	rtperror err;
	_RTP_CONTEXT *cur_context;
	struct timeval send_time, delta_time;

	if ((err = ValidRTPContext(cid, "RTPStartSession")) != RTP_OK)
		return err;

	cur_context = ContextList[cid];

	cur_context->time_elapsed = 0;
	cur_context->initial = TRUE;
	cur_context->sending_pkt_count = 0;
	cur_context->sending_octet_count = 0;

	gettimeofday(&cur_context->last_rtcp_send, NULL);
	cur_context->random_factor = (.5 + drand48());
	if (cur_context->rtcp_fraction != 0.0 && 
			cur_context->session_bandwidth != 0){
		delta_time = ConvertDoubleToTime(ComputeRTCPSendDelay(cur_context) *
						 cur_context->random_factor);
		send_time =
			AddTimes(&cur_context->last_rtcp_send, &delta_time);
		
		/* Schedule the initial send of the first RTCP packet */
		RTPSetTimer(cid, RTP_TIMER_SEND_RTCP, 0, NULL, &send_time);
	}

	return RTP_OK;
}

rtperror RTPStopSession(context cid, char *reason) {
	rtperror err;
	struct timeval now;

	if ((err = ValidRTPContext(cid, "RTPStopSession")) != RTP_OK)
		return err;

	gettimeofday(&now, NULL);
	RTPSetTimer(cid, RTP_TIMER_SEND_BYE_ALL, 0, reason, &now);

	return(RTP_OK);
}


rtperror RTPBuildByePacket(context cid, int one_csrc, u_int32 ssrc,
				 char *reason, int padding_block, char *buffer,
				 int *length)
{
	char *bye_end;
	_RTP_CONTEXT *the_context;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPBuildByePacket")) != RTP_OK)
		return err;
	the_context = ContextList[cid];

	/* Shouldn't send BYE if never data or RTCP */
	if (the_context->initial && the_context->sending_pkt_count==0){

		err = RTP_ERROR_CANT_SEND_BYE;
		return(err);
	}
	/* initial is set to FALSE because next time, this will not be
		 the first RTCP sending */
	the_context->initial = FALSE;

	/* Should add BYE reconsideration here */

	/* Build packet */
	/* XXX length is ignored as an 'in' parameter -- if the buffer's too small
		 we'll clobber memory! */
	bye_end = buffer;
	bye_end = Build_RRSR_Packets(the_context, bye_end);

	if (one_csrc) {
		bye_end = BuildIndividualByePacket(the_context, bye_end, buffer, 
							 padding_block, reason, ssrc);
	}
	else {
		bye_end = BuildCompoundByePacket(the_context, bye_end, buffer,
						 padding_block, reason);
	}

	*length = bye_end - buffer;

	/* Update context statistics: */
	the_context->last_pkt_size = *length;
	
	/* update the avg size of the estimate by the size of the report
		 packet we just sent */
	the_context->avg_rtcp_size +=
		(the_context->last_pkt_size - the_context->avg_rtcp_size) *
		_RTCP_SIZE_GAIN;

	/* Update the number of RTCP packets sent */
	the_context->my_memberinfo->rtcp_count++;

	return(RTP_OK);
}

rtperror RTPBuildRTCPPacket(context cid, rtcp_subparts_t subparts,
					int padding_block, char *buffer, int *length)
{
	char *cur_pos;
	struct timeval rtcp_delay;
	struct timeval cur_time, send_time;
	struct link *dummy;
	_RTP_CONTEXT *the_context;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPBuildRTCPPacket")) != RTP_OK)
		return err;
	the_context = ContextList[cid];

	cur_pos = buffer;
	/* The current group size refers to the number of confirmed
		 members.	*/

	if (the_context->session_bandwidth == 0.0 ||
			the_context->rtcp_fraction == 0.0){
		return RTP_DONT_SEND_NOW;
	}
	/* Only do reconsideration and rescheduling if this is the first RTCP
		 packet in the split compound */
	if (subparts != RTCP_SUBPARTS_RRSR){
		if (!DoRTCPReconsideration(the_context)) {
			return RTP_DONT_SEND_NOW;
		}
	}

	/* If this is the first time we've sent anything, generate ourselves an
		 SSRC. */
	if (the_context->initial && the_context->sending_pkt_count==0){
		ComputeNewSSRC(the_context);
		
		/* Set previous SSRC to the current SSRC.	We check if some
			 SSRC == my_ssrc or prev_ssrc, so this in essence hides the
			 previous ssrc */
		the_context->my_previous_ssrc = the_context->my_ssrc;
	}

	/* initial is set to FALSE because next time, this will not be
		 the first RTCP sending */
	the_context->initial = FALSE;

	/* Build packet here */
	/* XXX length is ignored as an 'in' paramater -- if the buffer's too small
		 we'll clobber memory! */
	if (subparts == RTCP_SUBPARTS_SDES) {
		dummy = NULL;
		cur_pos = Build_RR_Packet(the_context, cur_pos, &dummy, TRUE);
	}
	else {
		cur_pos = Build_RRSR_Packets(the_context, cur_pos);
	}

	if (subparts != RTCP_SUBPARTS_RRSR){
		cur_pos = Build_SDES_Packet(the_context, cur_pos, buffer, padding_block);
	}

	*length = cur_pos - buffer;

	/* Update context statistics: */
	the_context->last_pkt_size = *length;
	
	/* update the avg size of the estimate by the size of the report
		 packet we just sent */
	/* XXX not correct for sub-parted packets */
	the_context->avg_rtcp_size +=
		(the_context->last_pkt_size - the_context->avg_rtcp_size) *
		_RTCP_SIZE_GAIN;

	/* Update the number of RTCP packets sent */
	the_context->my_memberinfo->rtcp_count++;

	UpdateMembershipLists(the_context);

	if (subparts != RTCP_SUBPARTS_RRSR){
		/* Calculate a new random factor for next time */
		the_context->random_factor = .5 + drand48();
		
		/* Set up next RTCP call */
		gettimeofday(&cur_time, NULL);
		the_context->last_rtcp_send = cur_time;
		rtcp_delay = ConvertDoubleToTime(ComputeRTCPSendDelay(the_context) *
						 the_context->random_factor);	
		send_time = AddTimes(&cur_time, &rtcp_delay);
		RTPSetTimer(the_context->context_num, RTP_TIMER_SEND_RTCP, 0, NULL,
		&send_time);
	}

	return(RTP_OK);
}

	
rtp_packet RTPGetRTPPacket(char *rtppacket, long pktlen){
	rtp_packet the_packet;

	/* If packet is encrypted, then the extra 32-bit header used
		 for encryption should have already been stripped off */
	the_packet.RTP_header = (rtp_hdr_t*) rtppacket;

	if (RTP_X(*the_packet.RTP_header) == 1){ /* Is there an extension? */
		/* if so, then set up RTP_extension field. */
		
		the_packet.RTP_extension = 
			(rtp_hdr_ext*) ((char*)rtppacket + sizeof(rtp_hdr_t) + 
					(RTP_CC(*the_packet.RTP_header) - 1) * 4);

		/* Note that if the packet has not yet been converted to host-byte-
			 ordering, then the extension length will be wrong, and so
			 the packet payload will point to the wrong place. */

		the_packet.payload = 
			((char*) the_packet.RTP_extension + 
			 (the_packet.RTP_extension->elen + 1) * 4);
	}

	/* Otherwise, set extension field to NULL */
	else {
		the_packet.RTP_extension = NULL;
		the_packet.payload = 
			((char*) rtppacket + sizeof(rtp_hdr_t) + 
			 (RTP_CC(*the_packet.RTP_header) - 1) * 4);
	}
	the_packet.payload_len = pktlen + (long)(rtppacket - the_packet.payload);
	if (RTP_P(*the_packet.RTP_header) == 1){
		/* There is padding on this packet.	The # of bytes of padding
			 (stored in the last byte of the packet = where we currently
			 point to as the last byte in the payload) must be subtracted
			 from our current payload estimate. */
			 
		the_packet.payload_len -= 
			(int) the_packet.payload[the_packet.payload_len-1];
	}
	return the_packet;
}


	

u_int32 RTPPacketGetCSRC(rtp_packet *rtppkt, int csrc_no){

	return *(u_int32*) ((char*)rtppkt->RTP_header->csrc + (4 * csrc_no));
}

	


int RTPSplitCompoundRTCP(char *rtcppacket, char *indpkts[], long len){
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
		cur_rtcp_pktlen = *((u_int16*) ((char*)curloc + 2));
		len -= 4 * cur_rtcp_pktlen + 4;
		curloc += 4 * cur_rtcp_pktlen + 4;
	}
	return numpkts;
}




rtcp_packet RTPGetRTCPPacket(char *rtcppacket){
	rtcp_packet the_packet;
	char *p = rtcppacket;
	int i;

	the_packet.common = (rtcp_common *) p;
	p += RTCP_COMMON_SIZE;

	switch (the_packet.common->pt){
	case RTCP_PACKET_SR:
		the_packet.variant.sr.sr_fixed = (rtcp_sr *) p;
		p += RTCP_SR_SIZE;
		for (i = 0; i < RTCP_RC(*the_packet.common); i++) {
			the_packet.variant.sr.rblock[i] = (rtcp_report_block *) p;
			p += RTCP_REPORT_BLOCK_SIZE;
		}
		the_packet.variant.sr.extension = p;
		break;
		
	case RTCP_PACKET_RR:
		the_packet.variant.rr.rr_fixed = (rtcp_rr *) p;
		p += RTCP_RR_SIZE;
		for (i = 0; i < RTCP_RC(*the_packet.common); i++) {
			the_packet.variant.rr.rblock[i] = (rtcp_report_block *) p;
			p += RTCP_REPORT_BLOCK_SIZE;
		}
		the_packet.variant.rr.extension = p;
		break;

	case RTCP_PACKET_SDES:
		the_packet.variant.sdes.remainder = p;
		break;

	case RTCP_PACKET_BYE:
		for (i = 0; i < RTCP_RC(*the_packet.common); i++) {
			the_packet.variant.bye.byeblock[i] = (rtcp_bye_block *) p;
			p += RTCP_BYE_BLOCK_SIZE;
		}
		
		/* Check if a reason is present.	This occurs when there are more bytes
			 left at this point, according to 'len' */

		if (rtcppacket + ((the_packet.common->len + 1) * 4) > p) {
			the_packet.variant.bye.reason_length = *p;
			the_packet.variant.bye.reason = p + 1;
		}
		else {
			/* No reason/length present */
			the_packet.variant.bye.reason_length = 0;
			the_packet.variant.bye.reason = NULL;
		}
		break;
	case RTCP_PACKET_APP:
		the_packet.variant.app.app_fixed = (rtcp_app *) p;
		p += RTCP_APP_SIZE;
		the_packet.variant.app.appdata = p;
		break;
	default:
		the_packet.variant.unknown.data = p;
		break;
	}
	return the_packet;
}
		
	
void GetFirstSDESItemForSSRC(rtcp_sdes_item *the_item, char* startpoint){
	/* The startpoint points to the SSRRC descriptor for this chunk */
	the_item->ssrc = ntohl(*(u_int32*)startpoint);
	the_item->type = *(int8*)(startpoint+4);
	if (the_item->type != 0){
		the_item->len = *(int8*)(startpoint+5);
		/* Set offset to the end of the current field */
		the_item->offset = (2 + the_item->len) % 4;
	}
	else {
		the_item->len = 0;
		/* Normally we set the offset to the end of the current field,
			 but in this case, there is no current field, so the padding
			 should start immediately */
		the_item->offset = 0;
	}
	the_item->description = startpoint + 6;

	the_item->chunkno++;
	return;
}

rtcp_sdes_item InitSDESItemIter(rtcp_packet *rtcpsdes){
	rtcp_sdes_item my_item;
	my_item.chunkno = 0;
	GetFirstSDESItemForSSRC(&my_item, rtcpsdes->variant.sdes.remainder);
	return my_item;
}

rtcp_sdes_item GetNextItem(rtcp_sdes_item *prev_item){
	rtcp_sdes_item new_item;
	if (prev_item->type == 0){
		/* Indicates we are at a new SSRC.	The position of the next SSRC
			 appears at the start of the next 32 bit word.	The description
			 field points to 2 after the current type address.	The next
			 word starts at 4 - offset after the type address, which is 2 -
			 offset after the description */

		new_item.chunkno = prev_item->chunkno;
		GetFirstSDESItemForSSRC(&new_item, 
					prev_item->description + 2 - prev_item->offset);
	}
	else {
		new_item.ssrc = prev_item->ssrc;
		new_item.chunkno = prev_item->chunkno;
		new_item.type = *(int8*) (prev_item->description + 
						prev_item->len);
		if (new_item.type == 0){
			/* type 0 item has no string field */
			new_item.len = 0;
			new_item.offset = prev_item->offset;
		}
		else {
			new_item.len = *(int8*)(prev_item->description + 
						prev_item->len + 1);
			new_item.offset = (prev_item->offset + 2 + new_item.len) % 4;
		}
		new_item.description = (int8*)prev_item->description + 
			prev_item->len + 2;
	}
	return new_item;
}
		


rtcp_report_block RTPGetReportBlock(rtcp_packet *rtcprrsr, int blockno){
	rtcp_report_block *the_block;
	rtcp_report_block dummy_block;

	switch(rtcprrsr->common->pt){
	case RTCP_PACKET_SR: /* SR report */
		the_block = rtcprrsr->variant.sr.rblock[blockno];
		break;
	case RTCP_PACKET_RR: /* RR report */
		the_block = rtcprrsr->variant.rr.rblock[blockno];
		break;
	default:
		/* packet type does not have report blocks.	Return garbage */
		the_block = &dummy_block;
		memset((char *)the_block, '\0', sizeof(rtcp_report_block));
	}

	return *the_block;
}

rtcp_bye_block RTPGetByeBlock(rtcp_packet *rtcpbye, int blockno){
	return *rtcpbye->variant.bye.byeblock[blockno];
}


rtperror RTPSessionGetMemberList(context cid, member_iterator *iter){
	membership_list *the_member_list;

	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetMemberList")) != RTP_OK)
		return err;

	the_member_list = &ContextList[cid]->RTP_MemberList;

	iter->cur_link = the_member_list->actual_list.last_link;
	iter->go_forward = FALSE;
	return RTP_OK;
}

/* This function adds to the contributor list. This list is the
	 set of users in a mixer who are generally being mixed together.
	 Not every one of these users is actually mixed in each packet.
	 The CSRC list in each packet is an indicator of which of these
	 are actually being mixed. This list is used pretty much to
	 send SDES packets for those that are generally being mixed.
	 ssrc is in host order. */

rtperror RTPSessionAddToContributorList(context cid, u_int32 ssrc) {
	member *the_member;
	rtperror err;
	contributor_t *new;

	if ((err = ValidRTPContext(cid, "RTPSessionAddToContributorList")) != RTP_OK)
		return err;

	/* We allow the CSRC to have the same value as a group member, but
		 not the same value as another CSRC member */

	new = ContextList[cid]->contriblist;
	while(new != NULL) {
		if(new->ssrc == ssrc) {
			return errordebug(RTP_SSRC_IN_USE, "RTPSessionAddToContributorList",
			"context %d two CSRCS use SSRC %x",
			(int)cid, (unsigned int) ssrc);
		}
		new = new->next;
	}

		
	/* Create and add new contributor */

	if((new = malloc(sizeof(contributor_t))) == NULL) {
		return errordebug(RTP_CANT_ALLOC_MEM, "RTPSessionAddToContributorList",
					"out of memory\n");
	}
	new->next = ContextList[cid]->contriblist;
	new->ssrc = ssrc;
	ContextList[cid]->contriblist = new;

	/* Now check if this ssrc will collide with the local member.
		 If not, see if there is any other member with this SSRC. If
		 so, handle a collision. When we're all done, we have the
		 member handle for this new member */

	if(ssrc == ContextList[cid]->my_ssrc) {
		the_member = HandleSSRCCollision(ContextList[cid],
						 ContextList[cid]->my_memberinfo,
						 NULL, FALSE, NULL, 1);
	} else {
		the_member = GetMemberFromSSRCHash(ContextList[cid], ssrc);
						 
		if (the_member == NULL){
			the_member = EstablishNewMember(ContextList[cid], ssrc, NULL, NULL,
							FALSE, RTP_MEMBER_PENDING_CONTRIBUTOR);
		} else {
			the_member = HandleSSRCCollision(ContextList[cid], the_member,
							 NULL, FALSE, NULL, 1);
		}
	}

	the_member->on_csrc_list = TRUE;

	return RTP_OK;
}

/* This function places a user into the CSRC List of the RTP packets
	 sent from now on. The user must already be a member of the contributing
	 list for this to work. SSRC in host order */

rtperror RTPSessionAddToCSRCList(context cid, u_int32 ssrc) {
	int size;
	rtperror err;
	contributor_t *cont;

	if ((err = ValidRTPContext(cid, "RTPSessionAddToCSRCList")) != RTP_OK)
		return err;

	/* We create a fixed sized block for CSRC only when they are used */

	if(ContextList[cid]->CSRCList == NULL) {
		ContextList[cid]->CSRCList = (u_int32 *) malloc(sizeof(u_int32) * _RTP_MAX_CSRC);
		ContextList[cid]->CSRClen = 0;
	}
	size = ContextList[cid]->CSRClen;


	/* This SSRC must be on the contributors list */

	cont = ContextList[cid]->contriblist;
	while(cont != NULL) {
		if(cont->ssrc == ssrc) break;
		cont = cont->next;
	}
	if(cont == NULL) {
		return errordebug(RTP_NO_SUCH_CONTRIBUTOR, "RTPSessionAddToCSRCList",
					"No such contributor %d\n",(unsigned int) ssrc);
	}

	if(ContextList[cid]->CSRClen == 15) {
		return errordebug(RTP_CSRC_LIST_FULL, "RTPSessionAddToCSRCList",
					"CSRC list full");
	}

	ContextList[cid]->CSRCList[size] = ssrc;
	ContextList[cid]->CSRClen += 1;
	return RTP_OK;
}

/* CSRC in host order */

rtperror RTPSessionRemoveFromCSRCList(context cid, u_int32 ssrc) {
	int i,foundit,size;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPSessionRemoveFromCSRCList")) != RTP_OK)
		return err;

	/* CSRC List must be set to remove, of course */

	if(ContextList[cid]->CSRCList == NULL) {
		return errordebug(RTP_CSRC_LIST_NOT_SET, "RTPSessionRemoveFromCSRCList",
					"CSRC List not set\n");
	}

	/* Search through the list. When a match is found, copy
		 all CSRC below it up a notch, overwriting old CSRC */

	size = ContextList[cid]->CSRClen;
	foundit = 0;
	for (i=0; i<size; i++){

		if (ContextList[cid]->CSRCList[i] == ssrc) {
			foundit = 1;
		} else if(foundit == 1) {
			ContextList[cid]->CSRCList[i-1] = ContextList[cid]->CSRCList[i];
		}
		
	}

	/* It's an error if there was no match */

	if(foundit == 0) {
		return errordebug(RTP_NO_SUCH_PERSON, "RTPSessionRemoveFromCSRCList",
					"Unkown SSRC\n");
		return(RTP_NO_SUCH_PERSON);
	}

	/* Now decrement the list size, free if zero */

	ContextList[cid]->CSRClen -= 1;
	if(ContextList[cid]->CSRClen == 0)
		free(ContextList[cid]->CSRCList);

	return(RTP_OK);

}

/* SSRC in host order */

rtperror RTPSessionLowLevelRemoveFromContributorList(context cid,
								 u_int32 ssrc,
								 char *reason) {
	member *the_member;
	contributor_t *cont, *pcont;
	rtperror err;
	person p;
	struct timeval now;

	if ((err = ValidRTPContext(cid, "RTPLowLevelSessionRemoveFromContributorList")) != RTP_OK)
		return err;

	cont = ContextList[cid]->contriblist;
	pcont = NULL;
	while(cont != NULL) {
		if(cont->ssrc == ssrc) break;
		pcont = cont;
		cont = cont->next;
	}

	/* It's an error if there was no match */

	if(cont == NULL) {
		return errordebug(RTP_NO_SUCH_PERSON, "RTPSessionRemoveFromContributorList",
					"Unknown SSRC");
	}

	/* Remove from CSRC List */
	RTPSessionRemoveFromCSRCList(cid,ssrc);

	/* Now schedule a BYE for this user. */
	gettimeofday(&now, NULL);
	RTPSetTimer(cid, RTP_TIMER_SEND_BYE_CONTRIBUTOR, ssrc, reason, &now);

	if((err = RTPSessionGetUniqueIDForCSRC(cid, ssrc, &p)) != RTP_OK) {
		return(err);
	}
	the_member = GetMemberFromUniqueIDHash(ContextList[cid], p);

	/* Remove from contrib list */
	if(pcont == NULL)	 /* First in list */
		ContextList[cid]->contriblist = cont->next;
	else
		pcont->next = cont->next;
	free(cont);

	RemoveMember(ContextList[cid], the_member);
	if (ContextList[cid]->UpdateMemberCallBack != NULL){
		ContextList[cid]->UpdateMemberCallBack(cid,
						 the_member->unique_id, 
						 RTP_FLAG_DELETED_MEMBER,
						 NULL);
	}
	DestroyMember(ContextList[cid], the_member);

	
	return(RTP_OK);
}

rtperror RTPSessionGetCSRCList(context cid, u_int32 *value, int32 *size){
	/* We make our own internal copy of the CSRC list. */
	int32 i;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSessionGetCSRCList")) != RTP_OK)
		return err;
	if (ContextList[cid]->CSRCList == NULL){
		return errordebug(RTP_CSRC_LIST_NOT_SET, "RTPSessionGetCSRCList",
					"context %d CSRC List not Set",
					(int)cid);
	}
	if (*size < ContextList[cid]->CSRClen){
		errordebug(RTP_INSUFFICIENT_BUFFER, "RTPSessionGetCSRCList",
				 "context %d buffer size of %d insufficient for CSRC list of length %d",
				 (int)cid, (int)*size, (int)ContextList[cid]->CSRClen);
		*size = ContextList[cid]->CSRClen;
		return RTP_INSUFFICIENT_BUFFER;
	}
	*size = ContextList[cid]->CSRClen;
	for (i=0; i < *size; i++){
		value[i] = ContextList[cid]->CSRCList[i];
	}
	return RTP_OK;
}

rtperror RTPSessionGetUniqueIDForCSRC(context cid, u_int32 ssrc, person *p){
	member *the_member;
	rtperror err;
	contributor_t *cont;

	if ((err = ValidRTPContext(cid, "RTPSessionGetUniqueIDForCSRC")) != RTP_OK)
		return err;

	cont = ContextList[cid]->contriblist;
	while(cont != NULL) {
		if (cont->ssrc == ssrc)	break;
		cont = cont->next;
	}
	if (cont == NULL) {
		/* ssrc not on csrc list */
		return errordebug(RTP_NO_SUCH_PERSON, "RTPSessionGetUniqueIDForCSRC",
					"context %d CSRC %x not on CSRC list",
					(int)cid, (unsigned int)ssrc);
		return RTP_NO_SUCH_PERSON;
	}

	the_member = GetMemberFromSSRCHash(ContextList[cid], ssrc);
	while (the_member != NULL){
		if (the_member->on_csrc_list){
			*p = the_member->unique_id;
			return RTP_OK;
		}
		the_member = the_member->collides_with_next;
	}
	return errordebug(RTP_NO_SUCH_PERSON, "RTPSessionGetUniqueIDForCSRC",
				"context %d CSRC %x not on CSRC list", (int)cid, (unsigned int)ssrc);
	return RTP_NO_SUCH_PERSON;
}

rtperror RTPGetRTPHeaderLength(context cid, int *length)
{
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPGetRTPHeaderLength")) != RTP_OK)
		return err;
	
	/* The necessary size of the buffer. */
	/* We subtract one u_int32 from buflen_true because it has a one-element
	 * CSRC array at its end. */
	/* This is the same expression as in BuildRTPHeader below. */

	*length = (sizeof(rtp_hdr_t) - sizeof(u_int32)) /* base header */
		+ (sizeof(u_int32) * ContextList[cid]->CSRClen) /* CSRC list */
		+ ((ContextList[cid]->hdr_extension != NULL) ?	/* Header ext. */
			 (ContextList[cid]->hdr_extension->elen * 4 + 4) :
			 0);

	return(RTP_OK);
}

rtperror RTPBuildRTPHeader(context cid, int32 tsinc, int8 marker,
				 int16 pti, int8 padding, int payload_len,
				 char *buffer, int *buflen)
{
	/* Our packet header will be composed of 3 parts:
	 * - the header
	 * - the CSRC list
	 * - the extension
	 * It is the caller's responsibility to attach the payload and padding.
	 */

	rtp_hdr_t *the_header;
	rtp_hdr_ext *hdr_ext;
	char *bufptr;
	int buflen_true;
	struct timeval now;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPBuildRTPHeader")) != RTP_OK)
		return err;
	if (ContextList[cid]->PreventEntryIntoFlaggingFunctions){
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPBuildRTPHeader",
					"context %d, cannot be called now",
					(int)cid);
	}

	/* The necessary size of the buffer. */
	/* We subtract one u_int32 from buflen_true because it has a one-element
	 * CSRC array at its end. */
	/* This is the same expression as in GetRTPHeaderLength above */
	buflen_true = (sizeof(rtp_hdr_t) - sizeof(u_int32)) /* base header */
		+ (sizeof(u_int32) * ContextList[cid]->CSRClen) /* CSRC list */
		+ ((ContextList[cid]->hdr_extension != NULL) ?	/* Header ext. */
			 (ContextList[cid]->hdr_extension->elen * 4 + 4) :
			 0);

	if (*buflen < buflen_true) {
		return errordebug(RTP_INSUFFICIENT_BUFFER, "RTPBuildRTPHeader",
					"context %d, insufficient buffer provided for RTP header",
					(int)cid);
	}
	*buflen = buflen_true;

	bufptr = buffer;
	the_header = (rtp_hdr_t *) buffer;
	memset(bufptr, '\0', buflen_true);

	/* ??? Is this necessary? */
	ContextList[cid]->PreventEntryIntoFlaggingFunctions = TRUE;

	/* If this is the first time that we send a packet then we need to
		 set our SSRC at this point */
	if (ContextList[cid]->initial && ContextList[cid]->sending_pkt_count==0){
		/* No packets have been sent yet, so compute the initial SSRC now */
		ComputeNewSSRC(ContextList[cid]);
		
		/* Set previous SSRC to the current SSRC.	We check if some
			 SSRC == my_ssrc or prev_ssrc, so this in essence hides the
			 previous ssrc */
		ContextList[cid]->my_previous_ssrc = ContextList[cid]->my_ssrc;
	}
	/* Since we just sent, update the fact that we are a sender */
	gettimeofday(&now, NULL);
	UpdateSenderTime(ContextList[cid], ContextList[cid]->my_memberinfo,
			 now,1);
	ContextList[cid]->time_elapsed += tsinc;

	SET_RTP_VERSION(*the_header, _RTP_VERSION);
	SET_RTP_X(*the_header, 0);
	if (ContextList[cid]->CSRCList == NULL){
		SET_RTP_CC(*the_header, 0);
	}
	else {
		SET_RTP_CC(*the_header, ContextList[cid]->CSRClen);
	}
	SET_RTP_M(*the_header, marker);
	SET_RTP_PT(*the_header, pti);
	SET_RTP_P(*the_header, (padding != 0));
	the_header->seq = (u_int16) ContextList[cid]->seq_no;
	ContextList[cid]->seq_no++;
	the_header->ts = ContextList[cid]->time_elapsed +
		ContextList[cid]->init_RTP_timestamp;
	the_header->ssrc = ContextList[cid]->my_ssrc;

	/* rtp_hdr_t has an extra 1-element array at its end for CSRC's */
	bufptr += sizeof(rtp_hdr_t) - sizeof(u_int32);

	if (ContextList[cid]->CSRCList != NULL) {
		memcpy(bufptr, (char *) ContextList[cid]->CSRCList,
		 sizeof(u_int32) * ContextList[cid]->CSRClen);
		bufptr += sizeof(u_int32) * ContextList[cid]->CSRClen;
	}

	if (ContextList[cid]->hdr_extension != NULL) {
		memcpy(bufptr, (char *) ContextList[cid]->hdr_extension,
		 ContextList[cid]->hdr_extension->elen * 4 + 4);

		hdr_ext = (rtp_hdr_ext *) bufptr;
	}
	else {
		hdr_ext = NULL;
	}

	ContextList[cid]->sending_octet_count += payload_len;

	ContextList[cid]->sending_pkt_count++;

	the_header->seq = htons(the_header->seq);
	the_header->ts = htonl(the_header->ts);
	the_header->ssrc = htonl(the_header->ssrc);
	
	if (hdr_ext != NULL){
		hdr_ext->etype =
			htons(hdr_ext->etype);
		hdr_ext->elen =
			htons(hdr_ext->elen);
	}

	ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
	return(RTP_OK);
}


rtperror RTPFindMember(context cid, memberinfo mi, char *sdes_field,
					 person *p){
	struct link *cur_link;

	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPFindMember")) != RTP_OK)
		return err;

	mi--; /* memberinfo is 1-indexed */
	if (mi >= _RTP_NUM_SDES_TYPES){
		return errordebug(RTP_NO_SUCH_SDES, "RTPMemberInfoSetSDES",
					"context %d no SDES field number %d",
					(int)cid, mi);
	}

	cur_link = ContextList[cid]->RTP_MemberList.actual_list.last_link;
	while (cur_link != NULL){
		if (strcmp(sdes_field, cur_link->my_member->sdes_info[(int) mi])==0){
			*p = cur_link->my_member->unique_id;
			return RTP_OK;
		}
		cur_link = cur_link->prev_link;
	}

	return errordebug(RTP_NO_SUCH_PERSON, "RTPFindMember",
				"context %d no person with SDES field %d of %s",
				(int)cid, (int) mi, sdes_field);
}


rtperror RTPMostRecentRTPTime(context cid, person p,
						struct timeval *the_time){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMostRecentRTPTime")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMostRecentRTPTime")) != RTP_OK)
		return err;
	*the_time = the_member->last_rtp_send_time;
	return RTP_OK;
}
	
rtperror RTPMostRecentRTCPTime(context cid, person p,
						 struct timeval *the_time){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMostRecentRTCPTime")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMostRecentRTCPTime")) != RTP_OK)
		return err;

	*the_time = the_member->last_rtcp_send_time;
	return RTP_OK;
}

rtperror RTPMostRecentRTPPerson(context cid, person *p){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMostRecentRTPPerson")) != RTP_OK)
		return err;
	if (ContextList[cid]->most_recent_rtp_sender == NULL){
		return errordebug(RTP_NO_SUCH_PERSON, "RTPMostRecentRTPPerson",
	"context %d person left or no rtp packets received", (int)cid);
	}
	*p = ContextList[cid]->most_recent_rtp_sender->unique_id;
	return RTP_OK;
}

rtperror RTPMostRecentRTCPPerson(context cid, person *p){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMostRecentRTCPPerson")) != RTP_OK)
		return err;
	if (ContextList[cid]->most_recent_rtcp_sender == NULL){
		return errordebug(RTP_NO_SUCH_PERSON, "RTPMostRecentRTCPPerson",
					"context %d person left or no rtcp packets received",
					(int)cid);
	}
	*p = ContextList[cid]->most_recent_rtcp_sender->unique_id;
	return RTP_OK;
}


rtperror RTPMostRecentSockaddr(context cid, struct sockaddr *addr) {
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPMostRecentAddr")) != RTP_OK)
		return err;

	if (ContextList[cid]->most_recent_addr.sa_family ==
			_RTP_ADDRESS_NOT_YET_KNOWN) {
		return errordebug(RTP_BAD_ADDR, "RTPMostRecentAddr",
					"context %d no packets recieved", (int)cid);
	}

	memcpy(addr, &ContextList[cid]->most_recent_addr, sizeof(struct sockaddr));

	return RTP_OK;
}

/* This function returns the status of the member. The member information is
	 written into the variable s. The sender information is written into
	 the variable sender */

rtperror RTPMemberInfoGetStatus(context cid, person p, memberstatus *s,
				senderstatus *sender ) 
{
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetStatus")) != RTP_OK)
		return err;


	if((err = GetMemberForContext(cid, p, &the_member,
				"RTPMemberGetStatus")) != RTP_OK)
		return err;
	*s = the_member->status;
	*sender = the_member->sendstatus;

	return(RTP_OK);
}
	
rtperror RTPMemberInfoSetSDES(context cid, person p, memberinfo SDES_field,
						char *the_info){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetSDES")) != RTP_OK)
		return err;
	SDES_field--; /* memberinfo is 1-indexed */
	if (SDES_field >= _RTP_NUM_SDES_TYPES){
		return errordebug(RTP_NO_SUCH_SDES, "RTPMemberInfoSetSDES",
					"context %d no SDES field number %d",
					(int)cid, SDES_field);
	}
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetSDES")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetSDES",
					"context %d cannot change person %ld SDES info",
					(int)cid, p);
	}

	/* Clear out any old sdes info (if it exists) */
	if (the_member->sdes_info[SDES_field] != NULL){
		free(the_member->sdes_info[SDES_field]);
		if(SDES_field == RTP_MI_CNAME) {
			RemoveMemberFromCNAMEHash(ContextList[cid], the_member);

		}
	}
	the_member->sdes_info[SDES_field] = (char*) malloc(sizeof(char) * 
								 (strlen(the_info) + 1));
	strcpy(the_member->sdes_info[SDES_field], the_info);
	the_member->sdes_info[SDES_field][strlen(the_info)] = '\0';

	if(SDES_field == RTP_MI_CNAME) {
		EnterCNAMEHashTable(ContextList[cid], the_member);
	}

	return RTP_OK;
}
	
	
	
rtperror RTPMemberInfoGetSDES(context cid, person p, memberinfo SDES_field,
						char *the_info){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetSDES")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetSDES")) != RTP_OK)
		return err;

	SDES_field--; /* memberinfo is 1-indexed */
	if (SDES_field >= _RTP_NUM_SDES_TYPES){
		return errordebug(RTP_NO_SUCH_SDES, "RTPMemberInfoGetSDES",
					"context %d no SDES field number %d",
					(int)cid, SDES_field);
	}

	if (the_member->sdes_info[SDES_field] == NULL){
		the_info[0] = '\0';
	}
	else {
		strcpy(the_info, the_member->sdes_info[SDES_field]);
		the_info[strlen(the_member->sdes_info[SDES_field])] = '\0';
	}
	return RTP_OK;
}


rtperror RTPMemberInfoGetRTPSockaddr(context cid, person p,
						 struct sockaddr *addr) {
	member *the_member;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetRTPAddr")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetRTPAddr")) != RTP_OK)
		return err;

	memcpy(addr, &the_member->my_addr[0], sizeof(struct sockaddr));

	return RTP_OK;
}


rtperror RTPMemberInfoGetRTCPSockaddr(context cid, person p,
							struct sockaddr *addr) {
	member *the_member;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetRTCPAddr")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetRTCPAddr")) != RTP_OK)
		return err;

	memcpy(addr, &the_member->my_addr[1], sizeof(struct sockaddr));

	return RTP_OK;
}

rtperror RTPMemberInfoSetNTP(context cid, person p, 
					 ntp64 NTP_time){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetNTP")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetNTP")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetNTP",
					"context %d cannot change person %ld SDES info",
					(int)cid, p);
		return RTP_PERSON_DATA_PROTECTED;
	}
	the_member->ntp_timestamp = NTP_time;
	return RTP_OK;
}
	
rtperror RTPMemberInfoGetNTP(context cid, person p,
					 ntp64 *NTP_time){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetNTP")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetNTP")) != RTP_OK)
		return err;

	*NTP_time = the_member->ntp_timestamp;
	return RTP_OK;
}


rtperror RTPMemberInfoSetRTP(context cid, person p, 
					 int32 rtp_stamp){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetRTP")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetRTP")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetRTP",
					"context %d cannot change person %ld SDES info",
					(int)cid, p);
	}
	the_member->rtp_timestamp = rtp_stamp;
	return RTP_OK;
}
	
rtperror RTPMemberInfoGetRTP(context cid, person p,
					 int32 *rtp_stamp){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetRTP")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetRTP")) != RTP_OK)
		return err;

	*rtp_stamp = the_member->rtp_timestamp;
	return RTP_OK;
}


rtperror RTPMemberInfoSetPktCount(context cid, person p, 
					int32 count){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetPktCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetPktCount")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetPktCount",
					"context %d cannot change person %ld member info",
					(int)cid, p);
		return RTP_PERSON_DATA_PROTECTED;
	}
	the_member->pkt_count = count;
	return RTP_OK;
}
	
rtperror RTPMemberInfoGetPktCount(context cid, person p,
					int32 *count){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetPktCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetPktCount")) != RTP_OK)
		return err;

	*count = the_member->pkt_count;
	return RTP_OK;
}



rtperror RTPMemberInfoSetRTCPPktCount(context cid, person p, int32 count)
{
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetRTCPPktCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetRTCPPktCount")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetRTCPPktCount",
					"context %d cannot change person %ld member info",
					(int)cid, p);
	}
	the_member->rtcp_count = count;
	return RTP_OK;
}
	
rtperror RTPMemberInfoGetRTCPPktCount(context cid, person p, int32 *count)
{
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetRTCPPktCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetRTCPPktCount")) != RTP_OK)
		return err;

	*count = the_member->rtcp_count;
	return RTP_OK;
}




rtperror RTPMemberInfoSetOctCount(context cid, person p, 
					int32 count){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetOctCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetOctCount")) != RTP_OK)
		return err;

	if (p != 0 && !the_member->on_csrc_list){
		return errordebug(RTP_PERSON_DATA_PROTECTED, "RTPMemberInfoSetOctCount",
					"context %d cannot change person %ld SDES info",
					(int)cid, p);
	}
	the_member->oct_count = count;
	return RTP_OK;
}
	
rtperror RTPMemberInfoGetOctCount(context cid, person p,
					int32 *count){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetOctCount")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetOctCount")) != RTP_OK)
		return err;

	*count = the_member->oct_count;
	return RTP_OK;
}


rtperror RTPMemberInfoSetSSRC(context cid, u_int32 ssrc, u_int32 mask){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetSSRC")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, 0, &the_member,
				 "RTPMemberInfoSetSSRC")) != RTP_OK)
		return err;

	ContextList[cid]->static_ssrc = ssrc;
	ContextList[cid]->ssrc_mask = mask;

	return RTP_OK;
}
	
rtperror RTPMemberInfoGetSSRC(context cid, person p,
						u_int32 *ssrc){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetSSRC")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetSSRC")) != RTP_OK)
		return err;

	*ssrc = the_member->ssrc;
	return RTP_OK;
}


rtperror RTPMemberInfoSetUserInfo(context cid, person p, 
					void* info){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoSetUserInfo")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoSetUserInfo")) != RTP_OK)
		return err;

	the_member->user_data = info;
	return RTP_OK;
}

	
rtperror RTPMemberInfoGetUserInfo(context cid, person p,
					void** info){
	member *the_member;
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPMemberInfoGetUserInfo")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPMemberInfoGetUserInfo")) != RTP_OK)
		return err;

	*info = the_member->user_data;
	return RTP_OK;
}


rtperror RTPSenderInfoGetFirstReceiverReport(context cid, person p,
							 receiver_report_iterator *the_iterator, receiver_report *report){

	member *the_member;
	Tcl_HashEntry *the_entry;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPSenderInfoGetFirstReceiverReport")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPSenderInfoGetFirstReceiverReport")) != RTP_OK)
		return err;

	if (the_member->sendstatus == RTP_SENDER_NOT) {
		return errordebug(RTP_NO_SUCH_SENDER, "RTPSenderInfoGetFirstReceiverReport",
					"context %d, person %ld is not a sender",	(int)cid, p);
	}

	the_entry = Tcl_FirstHashEntry(&the_member->RR_Hash,
				 (Tcl_HashSearch *) the_iterator);
	if(the_entry != NULL) {
		memcpy(report, Tcl_GetHashValue(the_entry), sizeof(receiver_report));
		return RTP_OK;
	} else {
		return errordebug(RTP_END_OF_LIST, "RTPSenderInfoGetFirstReceiverReport",
					"Last hash entry reached");
	}
}



rtperror RTPSenderInfoGetNextReceiverReport(context cid, person p,
							receiver_report_iterator *the_iterator, receiver_report *report){

	member *the_member;
	Tcl_HashEntry *the_entry;
	rtperror err;

	if ((err = ValidRTPContext(cid, "RTPSenderInfoGetNextReceiverReport")) != RTP_OK)

		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPSenderInfoGetNextReceiverReport")) != RTP_OK)
		return err;

	if (the_member->sendstatus == RTP_SENDER_NOT) {
		return errordebug(RTP_NO_SUCH_SENDER, "RTPSenderInfoGetNextReceiverReport",
					"context %d, person %ld is not a sender",	(int)cid, p);
	}

	the_entry = Tcl_NextHashEntry((Tcl_HashSearch *) the_iterator);

	if(the_entry != NULL) {
		memcpy(report, Tcl_GetHashValue(the_entry), sizeof(receiver_report));
		return RTP_OK;
	} else {
		return errordebug(RTP_END_OF_LIST, "RTPSenderInfoGetNextReceiverReport",
					"Last hash entry reached");
	}
}



rtperror RTPSenderInfoGetLocalReception(context cid, person p,
					receiver_report *report)
{
	member *the_member;
	rtperror err;
	rtcp_report_block the_block;

	if ((err = ValidRTPContext(cid, "RTPSenderInfoGetFracLost")) != RTP_OK)
		return err;
	if ((err = GetMemberForContext(cid, p, &the_member,
				 "RTPSenderInfoGetFracLost")) != RTP_OK)
		return err;

	if (the_member->local_info == NULL) {
		return errordebug(RTP_NO_SUCH_SENDER, "RTPSenderInfoGetFracLost",
					"context %d, person %ld is not a sender",	(int)cid, p);
	}

	ComputeBlockInfo(ContextList[cid], the_member, &the_block, NULL);

	report->reporter			= ContextList[cid]->my_memberinfo->unique_id;
	report->fraction_lost = the_block.frac_lost;
	report->cum_lost			= the_block.cum_lost;
	report->highest_seqno = the_block.highest_seqno;
	report->jitter				= the_block.jitter;
	report->last_sr			 = the_block.lsr;
	report->delay_last_sr = the_block.dlsr;

	return RTP_OK;
}



rtperror RTPPacketReceived(context cid, char *rtp_pkt_stream, int len,
				 struct sockaddr from_addr, int fromaddrlen,
				 int isRTCP, int possible_loopback){
	u_int32 pkt_ssrc;
	rtp_packet rtpp;
	rtcp_packet rtcpp;
	rtcp_sdes_item sdi;
	rtcp_bye_block bb;
	int parts;
	int i;
	char* segment[_RTP_MAX_PKTS_IN_COMPOUND];
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPPacketReceived")) != RTP_OK)
		return err;
	if (ContextList[cid]->PreventEntryIntoFlaggingFunctions){
		return errordebug(RTP_CANT_CALL_FUNCTION, "RTPPacketReceived",
					"context %d, cannot be called now",
					(int)cid);
	}

	ContextList[cid]->PreventEntryIntoFlaggingFunctions = TRUE;

	/* Store this address as early as possible */
	memcpy(&ContextList[cid]->most_recent_addr, &from_addr,
	 fromaddrlen);

	/* If RTP packet, process it */
	if (!isRTCP){
#ifndef WORDS_BIGENDIAN
		FlipRTPByteOrdering(rtp_pkt_stream, len);
#endif

		rtpp = RTPGetRTPPacket(rtp_pkt_stream, len);
		if (!IsValidRTPPacket(ContextList[cid], &rtpp)){
			ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
			return errordebug(RTP_BAD_PACKET_FORMAT, "RTPPacketReceived",
	"context %d, invalid RTP packet format", (int)cid);
		}
		ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;

		/* If this received packet came from me, then don't process it.	This is
			 the case if the high-level specified a possible loopback (typically
			 meaning that the source addr of the packet matched my own) and the
			 ssrc equals either my ssrc or my previous ssrc */
		/* XXX: This will give very occasional false positives rather than
			 notice a SSRC collision properly, especially on those platforms
			 (Solaris, Windows) where getsockname() doesn't return an IP address
			 for a connected UDP socket. */
		if (possible_loopback &&
	(rtpp.RTP_header->ssrc == ContextList[cid]->my_memberinfo->ssrc ||
	 rtpp.RTP_header->ssrc == ContextList[cid]->my_previous_ssrc)){
			return errordebug(RTP_PACKET_LOOPBACK, "RTPPacketReceived",
			"context %d, got my own RTP packet", (int)cid);
		}
	 

		return UpdateMemberInfoByRTP(ContextList[cid], &rtpp, &from_addr,
				 fromaddrlen);
	}

///////
//by zxf
	ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
	return RTP_OK;
/////////


	/* Otherwise, is RTCP packet, and need to split the compound and
		 process each part */
	parts = SplitAndHostOrderLengthCompoundRTCP(rtp_pkt_stream, segment, len);

#ifndef WORDS_BIGENDIAN
	for(i = 0; i < parts; i++) {
		FlipRTCPByteOrdering(segment[i]);
	}
#endif

	if (!IsValidRTCPPacket(ContextList[cid], segment, parts, len))
	{
		ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
		return errordebug(RTP_BAD_PACKET_FORMAT, "RTPPacketReceived",
			"context %d, invalid RTCP packet format", (int)cid);
	}

	for (i=0; i<parts; i++)
	{
		rtcpp = RTPGetRTCPPacket(segment[i]);

		if (i==0)
		{
			/* Check the 1st packet to see if this packet originated
	 			from here.	If so, just ignore it. */
			/* Note that this fails for our first packet, since my_addr isn't
	 			set properly, but that's not a big deal. */
			switch(rtcpp.common->pt)
			{
				case RTCP_PACKET_SR:
					pkt_ssrc = rtcpp.variant.sr.sr_fixed->ssrc;
					break;
				case RTCP_PACKET_RR:
					pkt_ssrc = rtcpp.variant.rr.rr_fixed->ssrc;
					break;
				case RTCP_PACKET_SDES:
					/* This should be illegal. But, we'll just check the first
		 			SDES item */
					sdi = InitSDESItemIter(&rtcpp);
					pkt_ssrc = sdi.ssrc;
					break;
				case RTCP_PACKET_BYE:
					bb = RTPGetByeBlock(&rtcpp, 0);
					pkt_ssrc = bb.ssrccsrc;
					break;
				default:
					return errordebug(RTP_BAD_PACKET_FORMAT, "RTPPacketReceived",
					"context %d, invalid RTCP packet format", (int)cid);
			}
			if (possible_loopback &&
				(pkt_ssrc == ContextList[cid]->my_memberinfo->ssrc ||
		 		pkt_ssrc == ContextList[cid]->my_previous_ssrc))
		 	{
				ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
				return errordebug(RTP_PACKET_LOOPBACK, "RTPPacketReceived",
					"context %d, got my own RTCP packet", (int)cid);
			}
		}
		UpdateMemberInfoByRTCP(ContextList[cid], &rtcpp, &from_addr, fromaddrlen, i);
	}
	ContextList[cid]->PreventEntryIntoFlaggingFunctions = FALSE;
	return RTP_OK;
}



rtperror RTPCurrentMember(context cid, member_iterator *iter,
				person *the_member){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPCurrentMember")) != RTP_OK)
		return err;
	if (iter->cur_link == NULL){
		return errordebug(RTP_END_OF_LIST, "RTPCurrentMember",
					"context %d end of list",
					(int)cid);
		return RTP_END_OF_LIST;
	}
	*the_member = iter->cur_link->my_member->unique_id;
	return RTP_OK;
}


rtperror RTPNextMember(context cid, member_iterator *iter,
					 person *the_member){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPNextMember")) != RTP_OK)
		return err;
	if (iter->cur_link == NULL){
		return errordebug(RTP_END_OF_LIST, "RTPNextMember",
					"context %d end of list",
					(int)cid);
	}
	if (iter->go_forward){
		iter->cur_link = iter->cur_link->next_link;
	}
	else iter->cur_link = iter->cur_link->prev_link;
	return RTPCurrentMember(cid, iter, the_member);
}


rtperror RTPSetUpdateMemberCallBack(context cid, void (*f)(context, person, rtpflag, char *)){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSetUpdateMemberCallBack")) != RTP_OK)
		return err;
	ContextList[cid]->UpdateMemberCallBack = f;

	return RTP_OK;
}



rtperror RTPSetChangedMemberInfoCallBack(context cid,
					 void (*f)(context, person, memberinfo, char*, rtpflag)){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSetChangedMemberInfoCallBack")) != RTP_OK)
		return err;
	ContextList[cid]->ChangedMemberInfoCallBack = f;
	return RTP_OK;
}

rtperror RTPSetChangedMemberSockaddrCallBack(context cid,
	 void (*f)(context, person, struct sockaddr*, int)){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSetChangedMemberAddressCallBack")) != RTP_OK)
		return err;
	ContextList[cid]->ChangedMemberSockaddrCallBack = f;
	return RTP_OK;
}


rtperror RTPSetCollidedMemberCallBack(context cid, 
							void (*f)(context, person, person,
						rtpflag)){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSetCollidedMemberCallBack")) != RTP_OK)
		return err;
	ContextList[cid]->CollidedMemberCallBack = f;
	return RTP_OK;
}

rtperror RTPSetRevertingIDCallBack(context cid, 
					 void (*f)(context, person, person, 
							 void*, rtpflag)){
	rtperror err;
	if ((err = ValidRTPContext(cid, "RTPSetRevertingMemberCallBack")) != RTP_OK)
		return err;

	ContextList[cid]->RevertingIDCallBack = f;
	return RTP_OK;
}


char *RTPStrError(rtperror err){
	static char msg[64];
	switch(err){
	case RTP_OK:
		return "No error";
	case RTP_CANT_ALLOC_MEM:		
		return "Memory allocation failed";
	case RTP_TOO_MANY_CONTEXTS:	
		return "Maximum number of contexts exceeded";
	case RTP_UNKNOWN_CONTEXT:					
		return "Context destroyed or never created";
	case RTP_NOSOCKET:								 
		return "Socket not open for reading or writing";
	case RTP_CANT_GET_SOCKET:		
		return "Can't obtain a new socket";
	case RTP_CANT_BIND_SOCKET:
		return "Can't bind socket";
	case RTP_CANT_SET_SOCKOPT:		
		return "Unable to set socket option";
	case RTP_SOCKET_WRITE_FAILURE:	
		return "Unable to write to socket";
	case RTP_SOCKET_READ_FAILURE:	
		return "Unable to read from socket";
	case RTP_SOCKET_MISMATCH:		
		return "Socket does not belong to session";
	case RTP_FIXED_WHEN_OPEN:		
		return "Change not allowed during open session";
	case RTP_BAD_ADDR:								 
		return "Invalid address specified";
	case RTP_BAD_PORT:								 
		return "Invalid port specified";
	case RTP_CANT_CLOSE_SESSION:			 
		return "Unable to close session";
	case RTP_BAD_MI:									 
		return "No such membership info";
	case RTP_BAD_PROFILE:							
		return "Invalid rtpprofile specified";
	case RTP_SOCKET_EMPTY:						 
		return "no data to receive at socket";
	case RTP_PACKET_LOOPBACK:		
		return "Packet looped back";
	case RTP_INSUFFICIENT_BUFFER:			
		return "Allocated buffer too small";
	case RTP_CSRC_LIST_NOT_SET:	
		return "CSRC list not yet set";
	case RTP_CSRC_APPEARS_TWICE:			 
		return "2 CSRCs on list have same SSRC";
	case RTP_BAD_PACKET_FORMAT:				
		return "Packet has incorrect format";
	case RTP_NO_SUCH_PERSON:					 
		return "Person not session member";
	case RTP_PERSON_DATA_PROTECTED:	
		return "Person data protected";
	case RTP_NO_SUCH_SENDER:					 
		return "No such sender in current session";
	case RTP_NO_SUCH_SDES:						 
		return "No such SDES field";
	case RTP_CANT_USE_ENCRYPTION:	
		return "Encryption / decryption function not set";
	case RTP_DECRYPTION_FAILURE:			 
		return "Packet was unsuccessfully decrypted";
	case RTP_END_OF_LIST:							
		return "End of iterating list";
	case RTP_CANT_CALL_FUNCTION:	
		return "Not allowed to recurse";
	case RTP_SSRC_IN_USE:
		return "SSRC in use";
	case RTP_CANT_GETSOCKNAME:
		return "Couldn't get socket name";
	case RTP_BAD_VALUE:
		return "Parameter out of range";
	case RTP_NO_SUCH_CONTRIBUTOR:
		return "No such contributor";
	case RTP_CSRC_LIST_FULL:
		return "CSRC list full";
	case RTP_DONT_SEND_NOW:
		return "Don't send this packet now";
	default:
		sprintf(msg, "Unknown RTP Error %d", err);
		return msg;
	}
}
		
		
char *RTPDebugStr(void){
	static char error_msg[512];

#ifdef _RTP_DEBUG
	sprintf(error_msg, "%s:\n%s\n", RTPStrError(_RTP_cur_err), _RTP_err_msg);
#else
	sprintf(error_msg, "%s\n", RTPStrError(_RTP_cur_err));
#endif

	switch(_RTP_cur_err){
	case RTP_CANT_ALLOC_MEM:

	case RTP_CANT_GET_SOCKET:
	case RTP_CANT_BIND_SOCKET:
	case RTP_CANT_SET_SOCKOPT:
	case RTP_SOCKET_WRITE_FAILURE:
	case RTP_SOCKET_READ_FAILURE:
	case RTP_BAD_ADDR:
	case RTP_BAD_PORT:
	case RTP_CANT_CLOSE_SESSION:
	case RTP_SOCKET_EMPTY:
	case RTP_CANT_GETSOCKNAME:
		sprintf(error_msg + strlen(error_msg),
			"\n%s\n", strerror(errno));
		break;
	default:
		break;
	}

	return error_msg;
}
