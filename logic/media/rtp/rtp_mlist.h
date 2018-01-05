/* rtp_mlist.h : functionality to provide common operations on
                 membership lists.


Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/


#ifndef _MLIST_H
#define _MLIST_H

#include "rtp_mlist_internal.h"
#include "rtp_api_internal.h"

void RTP_Membership_Initialize(_RTP_CONTEXT *the_context);

member *EstablishNewMember(_RTP_CONTEXT *the_context, u_int32 ssrc,
			   void* user_data, struct sockaddr *fromaddr,
			   int addr_is_rtcp, memberstatus status);

int UpdateMemberTime(_RTP_CONTEXT *the_context, member *the_member,
		     struct timeval newtime);


int UpdateSenderTime(_RTP_CONTEXT *the_context, member *the_member,
		     struct timeval newtime, int dataflag);

void UpdateMembershipLists(_RTP_CONTEXT *the_context);


int RemoveMember(_RTP_CONTEXT *the_context, member *remove_me);

int DestroyMember(_RTP_CONTEXT *the_context, member *destroy_me);

void CleanList(_RTP_CONTEXT *the_context, doubly_linked_list *the_list);

void CleanMembershipList(_RTP_CONTEXT *the_context, membership_list *l);

void CleanHash(_RTP_CONTEXT *the_context, hash_table *the_hash);

rtperror UpdateMemberInfoByRTCP(_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen,
				int part_in_compound_pkt);

rtperror UpdateMemberInfoBySR  (_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen);

rtperror UpdateMemberInfoByRR  (_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen);

rtperror UpdateMemberInfoBySDES(_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen);

rtperror UpdateMemberInfoByBYE (_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen);


rtperror UpdateMemberInfoByRTP(_RTP_CONTEXT *the_context, 
			       rtp_packet *the_packet,
			       struct sockaddr *fromaddr,
			       int addrlen);

#endif
