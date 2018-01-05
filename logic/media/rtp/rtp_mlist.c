/* rtp_mlist.c : functionality to provide common operations on
                 membership lists.

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#define _MLIST_C

#include "sysdep.h"

#include "rtp_mlist_internal.h"
#include "rtp_mlist.h"
#include "rtp_collision.h"


#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif



/* Allocate a unique person ID for a member. */
static person AssignID(_RTP_CONTEXT *the_context){
#ifdef _RTP_DEBUG
  printf("ID %ld assigned\n", the_context->unique_id_counter);
#endif

  the_context->unique_id_counter++;
  return the_context->unique_id_counter - 1;
}

/* Initialize the membership-related data structures for a given context.
 * This should be called exactly once per context.
 */
void RTP_Membership_Initialize(_RTP_CONTEXT *the_context){
  InitSSRCHashTable(the_context);
  InitUniqueIDHashTable(the_context);
  InitCNAMEHashTable(the_context);
  MembershipListInit(&the_context->RTP_MemberList);
  MembershipListInit(&the_context->RTP_SenderList);
  return;
}
  
  

  
/* Place a new member on the membership list.
 * The member will be assigned a unique ID, and returned to the caller.
 * fromaddr holds the address from which the packet came that establishes
 * the new member.  If the member is being created for some other reason
 * (e.g. appeared inside the packet, but was not the source of the
 * packet so the address is not known), set fromaddr to NULL.
 */
member *EstablishNewMember(_RTP_CONTEXT *the_context, u_int32 ssrc,
			   void* user_data, struct sockaddr *fromaddr,
			   int addr_is_rtcp, memberstatus status){
  int i;
  member *the_member;
  struct timeval beginning;
  the_member = (member*) malloc(sizeof(member));
  the_member->ssrc_hash = NULL;
  the_member->unique_id_hash = NULL;
  the_member->cname_hash = NULL;
  the_member->sndr_list_link = NULL;

  the_member->ssrc = ssrc;

  the_member->colliding = FALSE;
  the_member->collides_with_next = NULL;
  the_member->collides_with_prev = NULL;

  for (i = 0; i < 2; i++) {
    /* The below expression says "the truth value of addr_is_rtcp is the
       same as the truth value of i."  This is to protect from the case when
       someone passes in a true value other than 1 as addr_is_rtcp. */
    if (fromaddr != NULL && ((addr_is_rtcp != 0) == (i != 0))) {
      the_member->my_addr[i] = *fromaddr;
    } else {
      the_member->my_addr[i].sa_family = _RTP_ADDRESS_NOT_YET_KNOWN;
    }
  }
  /* The new-address callbacks take place below */

  the_member->unique_id = AssignID(the_context);

  gettimeofday(&the_member->last_update_time, NULL);


  /* Start the member off as pending.  The function that called
     EstablishNewMember can change it to something else
     if it should be different */

  the_member->status = status;
  the_member->sendstatus = RTP_SENDER_NOT;
  
  the_member->user_data = user_data;
  for (i = 0; i < _RTP_NUM_SDES_TYPES; i++){
    the_member->sdes_info[i] = NULL;
  }

  the_member->local_info = NULL;

  the_member->rtp_timestamp = 0;
  the_member->ntp_timestamp.secs = 0;
  the_member->ntp_timestamp.frac = 0;
  the_member->pkt_count = 0;
  the_member->oct_count = 0;

  the_member->recv_rtp_count = 0;
  the_member->recv_csrc_count = 0;
  the_member->rtcp_count = 0;

  the_member->mmbr_list_link = CreateLink(the_member);
  if (!MembershipListInsertMember(&the_context->RTP_MemberList,
				  the_member->mmbr_list_link)){

    /* Mention some error? */
#ifdef _RTP_DEBUG
    printf("Oops: Member couldn't be entered into the memberlist\n");
#endif
    return NULL;
  }

  if (!EnterSSRCHashTable(the_context, the_member)){
    /* Mention some error? */
#ifdef _RTP_DEBUG
    printf("Oops: Member couldn't be entered into the ssrc hash table\n");
#endif
    return NULL;
  }
  if (!EnterUniqueIDHashTable(the_context, the_member)){
#ifdef _RTP_DEBUG
    printf("Oops: Member couldn't be entered into the id hash table\n");
#endif
    /* Mention some error? */
    return NULL;
  }

  /* We don't yet add to the CNAME hash table because there
     is no CNAME at this point */
  
  /* Initialize initial send times to 0.  Either an RTP or RTCP packet
     must have been received to create the new member.  The function
     that calls EstablishNewMember() should then set the appropriate
     send time.  Same goes for settin last_rtp_send_time if a packet
     was received indicating that the member is a sender. */
  beginning.tv_sec = 0;
  beginning.tv_usec = 0;
  the_member->last_rtp_send_time = beginning;
  the_member->last_rtcp_send_time = beginning;
  the_member->last_sr_receipt_time = beginning;
  the_member->last_send_basis_time = beginning;

  /* By default, we set from csrc list to false.  The CSRC list addition
     function should change this to true if so desired */
  the_member->on_csrc_list = FALSE; 
  /* Now set member time to current time */
  gettimeofday(&beginning, NULL);
  the_member->last_update_time = beginning;
  /* We flag a new member here, even though this may just be a member
     that has moved due to a collision.
     Code that should only be run when unique members arrive should
     trigger on the ChangedMemberInfoCallBack (field CNAME) instead. */

  if (the_context->UpdateMemberCallBack != NULL) {
    the_context->UpdateMemberCallBack(the_context->context_num,
				      the_member->unique_id,
				      RTP_FLAG_NEW_MEMBER,
				      NULL);
  }
  /* Announce the member's IP address, if we know it, unless the
     member is the local member (we should never call ChangedMemberInfo/Address
     about the local member). */
  if (the_context->ChangedMemberSockaddrCallBack != NULL && fromaddr != NULL &&
      the_member->unique_id != 0) {
    the_context->
      ChangedMemberSockaddrCallBack(the_context->context_num,
				    the_member->unique_id,
				    fromaddr,
				    addr_is_rtcp);
  }
  return the_member;
}

/* Change the time that we last received an RTP or RTCP packet from the_member
 * to newtime.
 */
int UpdateMemberTime(_RTP_CONTEXT *the_context, member *the_member,
		     struct timeval newtime){
  if (!MembershipListPullMember(&the_context->RTP_MemberList, 
				the_member->mmbr_list_link)){
    return FALSE;
  }

  the_member->last_update_time = newtime;

  /* We reinsert the member with its new time, and start trying at
     the back of the list which contains most recent times */
  if (!MembershipListInsertMember(&the_context->RTP_MemberList,
				  the_member->mmbr_list_link)){

    /* Mention some error? */
    return FALSE;
  }
  return TRUE;
}


/* Change the time that the_member last sent an RTP packet to newtime.  Note
 * that this does not call UpdateMemberTime to update the last received time
 * as well.
 *
 * If this member is newly a sender, perform the appropriate initializations
 * and call the appropriate callbacks.
 */
int UpdateSenderTime(_RTP_CONTEXT *the_context, member *the_member, 
		     struct timeval newtime, int dataflag){

  int newsender = 0;

  /* We only update the time here if its a data packet that arrived,
     or an RTCP packet for a purported or non-sender */

  if((the_member->sendstatus == RTP_MEMBER_CONFIRMED) &&
     dataflag == 0)
    return TRUE;

  /* Pull (or create) if this is an update event */

  if (the_member->sendstatus == RTP_SENDER_NOT) {

    /* Create the link */
    the_member->sndr_list_link = CreateLink(the_member);
    newsender = 1;

  } else {

    if (!SenderListPullMember(&the_context->RTP_SenderList,
			      the_member->sndr_list_link)){
#ifdef _RTP_DEBUG
      printf("UpdateSenderTime: Couldn't pull member\n");
#endif
      return FALSE;
    }
  }

  /* Update status - if we're a confirmed member, then
     a data packet upgrades us to confirmed. If we're not a
     confirmed member, then any packet upgrades us to purported.
     Finally, if we are not a confirmed sender, and what just
     arrived is an RTCP packet, we become purported */

  if((the_member->status == RTP_MEMBER_CONFIRMED) &&
     (the_member->sendstatus != RTP_SENDER_CONFIRMED) &&
     (dataflag == 1)) {

    ChangeSenderStatus(the_context, the_member, RTP_SENDER_CONFIRMED);

  } else if((the_member->status != RTP_MEMBER_CONFIRMED) ||
	    ((the_member->sendstatus != RTP_SENDER_CONFIRMED) &&
	     (dataflag == 0))) {

    ChangeSenderStatus(the_context, the_member, RTP_SENDER_PURPORTED);

  }

  /* Update sender time */

  if(dataflag == 1) 
    the_member->last_rtp_send_time = newtime;

  /* Update basis time */
  
  the_member->last_send_basis_time = newtime;

  /* We reinsert the member with its new time, and start trying at
     the back of the list which contains most recent times */
  if (!SenderListInsertMember(&the_context->RTP_SenderList,
			  the_member->sndr_list_link)){
    /* Mention some error? */
    return FALSE;
  }

  if (newsender == 1) 
    Init_RR_Hash(the_member);


  return TRUE;
}



/* Return TRUE if themember has sender timed out. */
static int SenderTimedOut(_RTP_CONTEXT *the_context, member *themember,
			  struct timeval now){
  double rtcp_send_delay;
  struct timeval sender_timeout_interval;
  rtcp_send_delay = ComputeSenderRTCPSendDelay(the_context);

  if (rtcp_send_delay == 0.0) {

    sender_timeout_interval = 
      ConvertDoubleToTime(_RTP_SENDER_MAX_TIMEOUT);
  }
  
  else sender_timeout_interval = ConvertDoubleToTime(rtcp_send_delay * 
						       _RTP_SENDER_TIMEOUT);


  return TimeExpired(&themember->last_send_basis_time,
		     &now, &sender_timeout_interval);

}

static int PurportedSenderTimedOut(_RTP_CONTEXT *the_context,
				   member *themember, struct timeval now){
  double rtcp_send_delay;
  struct timeval sender_timeout_interval;
  rtcp_send_delay = ComputeSenderRTCPSendDelay(the_context);

  if (rtcp_send_delay == 0.0) {

    sender_timeout_interval = 
      ConvertDoubleToTime(_RTP_PURPORTED_SENDER_MAX_TIMEOUT);
  }
  
  else sender_timeout_interval = ConvertDoubleToTime(rtcp_send_delay * 
						       _RTP_PURPORTED_SENDER_TIMEOUT);


  return TimeExpired(&themember->last_send_basis_time,
		     &now, &sender_timeout_interval);

}



/* Return TRUE if themember has soft timed out. */
static int SoftTimedOut(_RTP_CONTEXT *the_context, member *themember,
			struct timeval now){
  double rtcp_send_delay;
  struct timeval soft_timeout_interval;
  rtcp_send_delay = ComputeReceiverRTCPSendDelay(the_context);

  if (rtcp_send_delay >= 0.0){
    soft_timeout_interval = ConvertDoubleToTime(rtcp_send_delay * 
						_RTP_SOFT_MEMBER_TIMEOUT);
    return TimeExpired(&themember->last_update_time,
			 &now, &soft_timeout_interval);
  }

  /* If there is no rtcp_send_delay, then can't
     soft timeout */
  return FALSE;
}



/* HardTimedOut returns TRUE if the member hard times out.
   Hard timeouts are just a multiple of the soft timeout */

static int HardTimedOut(_RTP_CONTEXT *the_context, member *themember,
			struct timeval now){
  double rtcp_send_delay;
  struct timeval hard_timeout_interval;
  rtcp_send_delay = ComputeReceiverRTCPSendDelay(the_context);

  if (rtcp_send_delay >= 0.0){
   hard_timeout_interval = ConvertDoubleToTime(rtcp_send_delay * 
						_RTP_SOFT_MEMBER_TIMEOUT *
						_RTP_HARD_MEMBER_TIMEOUT);
    return TimeExpired(&themember->last_update_time,
			 &now, &hard_timeout_interval);
  }

  /* If there is no rtcp_send_delay, then can't
     hard timeout */
  return FALSE;
}


static void TimeOutPurportedSenders(_RTP_CONTEXT *the_context) {

  struct timeval now;
  struct link *cur_link, *prev_link;

  gettimeofday(&now, NULL);
  cur_link = the_context->RTP_SenderList.actual_list.last_link;
  while (cur_link != NULL){

    /* Once we find a sender that hasn't timed out, we need
       not look any further down the list */
    if (!PurportedSenderTimedOut(the_context, cur_link->my_member, now)){
      break;
    }

#ifdef _RTP_DEBUG
    printf("Member %lx no longer a sender\n", 
	   cur_link->my_member->ssrc);
#endif

    prev_link = cur_link->prev_link;
    SenderListPullMember(&the_context->RTP_SenderList, cur_link);
    cur_link->my_member->sndr_list_link = NULL;
    ChangeSenderStatus(the_context, cur_link->my_member, RTP_SENDER_NOT);
    if (cur_link->my_member->local_info != NULL) {
      /* Sender's local info needs destroying */
      free(cur_link->my_member->local_info);
      cur_link->my_member->local_info = NULL;
    }
    Delete_RR_Hash(cur_link->my_member);
    if(cur_link == the_context->RTP_SenderList.oldest_not_yet_soft)
      the_context->RTP_SenderList.oldest_not_yet_soft = prev_link;
    free(cur_link);
    cur_link = prev_link;
  }

}

static void TimeOutSenders(_RTP_CONTEXT *the_context) {
  struct timeval now;
  struct link *cur_link, *prev_link;

  gettimeofday(&now, NULL);
  cur_link = the_context->RTP_SenderList.oldest_not_yet_soft;
  while (cur_link != NULL){

    /* Once we find a sender that hasn't timed out, we need
       not look any further down the list */
    if (!SenderTimedOut(the_context, cur_link->my_member, now)){
      break;
    } else if(cur_link->my_member->sendstatus == RTP_SENDER_PURPORTED) {
      cur_link = cur_link->prev_link;
      continue;
    }

#ifdef _RTP_DEBUG
    printf("Member %lx no longer a sender\n", 
	   cur_link->my_member->ssrc);
#endif

    prev_link = cur_link->prev_link;
    if(cur_link->my_member->sendstatus == RTP_SENDER_CONFIRMED) {
      ChangeSenderStatus(the_context, cur_link->my_member, RTP_SENDER_PURPORTED);
      the_context->RTP_SenderList.confirmed_members--;
    }
    cur_link = prev_link;
  }

  the_context->RTP_SenderList.oldest_not_yet_soft = cur_link;

}

void UpdateMembershipLists(_RTP_CONTEXT *the_context){
  struct timeval now;
  struct link *cur_link, *prev_link;
  gettimeofday(&now, NULL);
  /* Lists are sorted in increasing timestamp order, so anyone
     that expires would be at the front of the list */

  /* First we do sender list */

  TimeOutPurportedSenders(the_context);
  TimeOutSenders(the_context);
  
  /* Next we do member list for hard timeouts.
   If a hard timeout occurs, regardless of the member's
   state, it is removed from the memberlist barring the following
   exceptions:  If the member on the list is the local member.
   If the member appears in the SSRC list */

  cur_link = the_context->RTP_MemberList.actual_list.last_link;
  while (cur_link != NULL){
    prev_link = cur_link->prev_link;
    if (cur_link->my_member == the_context->my_memberinfo){
      /* We don't ever want to take ourselves off the membership
	 list until we send BYE */
      cur_link = prev_link;
      continue;
    }
    if (cur_link->my_member->on_csrc_list == TRUE){
      /* CSRC stay as well */
      cur_link = prev_link;
      continue;
    }

    /* Any member that hard timeouts should be removed 
       If the current link hasn't hard timed out, then no
       further members on the list will hard time out
       either, and we can abort the check. */

    if (HardTimedOut(the_context, cur_link->my_member, now)){

      member *the_member = cur_link->my_member;
      RemoveMember(the_context, cur_link->my_member);
      if (the_context->UpdateMemberCallBack != NULL){
	the_context->UpdateMemberCallBack(the_context->context_num,
					  the_member->unique_id, 
					  RTP_FLAG_DELETED_MEMBER,
					  NULL);
      }
      DestroyMember(the_context, the_member);
    } else {
      break;
    }


    /* Move onto the next link and check for more hard timeouts */
    cur_link = prev_link;
  }

  /* Now check for soft timeouts on the memberlist.  We jump right
   to the oldest_not_yet_soft because any older links have
   already soft timed-out. */

  cur_link = the_context->RTP_MemberList.oldest_not_yet_soft;

  while (cur_link != NULL){
    if (SoftTimedOut(the_context, cur_link->my_member, now)){
      
      prev_link = cur_link->prev_link;
      
      if (cur_link->my_member == the_context->my_memberinfo){
	/* We don't ever want to change our own status */
	cur_link = prev_link;
	continue;
      }
      /* Don't time out the CSRC members either */
      if(cur_link->my_member->on_csrc_list == TRUE) {
	cur_link = prev_link;
	continue;
      }
      
      if (cur_link->my_member->status == RTP_MEMBER_CONFIRMED){
	/* confirmed members become expired */
	ChangeMemberStatus(the_context, cur_link->my_member,
			   RTP_MEMBER_EXPIRED);
      }
      else if (cur_link->my_member->status == RTP_MEMBER_CONFIRMED_CONTRIBUTOR) {
	/* confirmed contributors become expired contributors */
	ChangeMemberStatus(the_context, cur_link->my_member, RTP_MEMBER_EXPIRED_CONTRIBUTOR);
      }
      else if ((cur_link->my_member->status == RTP_MEMBER_PENDING) ||
	       (cur_link->my_member->status == RTP_MEMBER_PENDING_CONTRIBUTOR)) {
	member *the_member = cur_link->my_member;

	/* Pending members get destroyed on soft_timeouts */
	RemoveMember(the_context, the_member);
	if (the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    the_member->unique_id, 
					    RTP_FLAG_DELETED_PENDING,
					    NULL);
	}
	DestroyMember(the_context, the_member);
      }
      
      cur_link = prev_link;
    }
    
    /* Went beyond the soft timeout point */
    
    else break;
  }

  /* When we reach here, cur_link is set to the oldest member
     that hasn't soft timed out yet.  If no such members
     exist, it equals NULL */
  the_context->RTP_MemberList.oldest_not_yet_soft = cur_link;
	
  return;
}



/* Removes a member's info from the current session.
 * The member is pulled off the Member List and the Sender List
 * (if on the Sender List).  The member should be destroyed after
 * any appropriate callbacks are called.
 */
int RemoveMember(_RTP_CONTEXT *the_context, member *remove_me){
  /* A member is not removed if it is within the CRSC list. */

  if (remove_me->sendstatus != RTP_SENDER_NOT){
    /* Is also a sender - must remove sender link and sender structures also */
    Delete_RR_Hash(remove_me);

    if (!SenderListPullMember(&the_context->RTP_SenderList, 
		    remove_me->sndr_list_link)){
      return FALSE;
    }
    free(remove_me->sndr_list_link);
    remove_me->sndr_list_link = NULL;
    ChangeSenderStatus(the_context, remove_me, RTP_SENDER_NOT);
  }

  if (remove_me->local_info != NULL) {
    free(remove_me->local_info);
    remove_me->local_info = NULL;
  }

  if (remove_me->unique_id == 0){
    return FALSE;
  }

  /* Remove member list link */
  if (!MembershipListPullMember(&the_context->RTP_MemberList,
				remove_me->mmbr_list_link)){
    return FALSE;
  }


  /* When we had a bye list, we used to add the member to the bye
     list at this point and reuse the memberlist link for this purpose.
     But now we just destroy the link when we remove the member
     from the memberlist */
  free(remove_me->mmbr_list_link);
  remove_me->mmbr_list_link = NULL;

  /* Remove from SSRC hash table */
  if (remove_me->ssrc_hash != NULL){
    RemoveMemberFromSSRCHash(the_context, remove_me);
  }

  /* Removal from Unique ID hash table should occur when deleted */

  return TRUE;
}


/* Removes the member from any hash tables and lists that it is a member of,
 * calls appropriate destruction callbacks, and destroys the member and any
 * associated data. 
 */
int DestroyMember(_RTP_CONTEXT *the_context, member *destroy_me){
  int i;


  /* Delete the member's SDES entries. */
  for (i = 0; i < _RTP_NUM_SDES_TYPES; i++) {
    if (destroy_me->sdes_info[i] != NULL) {
      free(destroy_me->sdes_info[i]);
    }
  }

  if (destroy_me->local_info != NULL) {
    free(destroy_me->local_info);
  }

  /* Remove ourselves from anyone else's receiver_report hashes. */
  Clear_Member_RRs(the_context, destroy_me);

  if (destroy_me->mmbr_list_link != NULL){
    /* Member needs to be removed from memberlist */
    MembershipListPullMember(&(the_context->RTP_MemberList), 
			     destroy_me->mmbr_list_link);
    free(destroy_me->mmbr_list_link);
  }

  if (destroy_me->sendstatus != RTP_SENDER_NOT){
    /* Member needs to be removed from senderlist */
    Delete_RR_Hash(destroy_me);
    SenderListPullMember(&the_context->RTP_SenderList, destroy_me->sndr_list_link);
    free(destroy_me->sndr_list_link);
    destroy_me->sndr_list_link = NULL;
    ChangeSenderStatus(the_context, destroy_me, RTP_SENDER_NOT);
  }

  if (destroy_me->ssrc_hash != NULL){
    /* Member needs to be removed from ssrc hash */
    RemoveMemberFromSSRCHash(the_context, destroy_me);
  }

  if (destroy_me->unique_id_hash != NULL){
    /* Member needs to be removed from unique id hash */
    RemoveMemberFromUniqueIDHash(the_context, destroy_me);
  }

  if (destroy_me->cname_hash != NULL){
    /* Member needs to be removed from CNAME hash */
    RemoveMemberFromCNAMEHash(the_context, destroy_me);
  }

  /* If it collides with any other links, update those it collides with */
  if (destroy_me->colliding){
    if (destroy_me->collides_with_next != NULL){
      destroy_me->collides_with_next->collides_with_prev = 
	destroy_me->collides_with_prev;
      if (destroy_me->collides_with_next->collides_with_prev == NULL &&
	  destroy_me->collides_with_next->collides_with_next == NULL){
	EnterSSRCHashTable(the_context,
			   destroy_me->collides_with_next);
	destroy_me->collides_with_next->colliding = FALSE;
      }
    }
    if (destroy_me->collides_with_prev != NULL){
      destroy_me->collides_with_prev->collides_with_next = 
	destroy_me->collides_with_next;
      if (destroy_me->collides_with_prev->collides_with_next == NULL &&
	  destroy_me->collides_with_prev->collides_with_prev == NULL){
	EnterSSRCHashTable(the_context,
			   destroy_me->collides_with_prev);
	destroy_me->collides_with_prev->colliding = FALSE;
      }
    }
  }

  /* If the member was the most recent RTP or RTCP sender, it isn't any
     more. */
  if (the_context->most_recent_rtp_sender == destroy_me)
    the_context->most_recent_rtp_sender = NULL;
  if (the_context->most_recent_rtcp_sender == destroy_me)
    the_context->most_recent_rtcp_sender = NULL;

  /* And finally, the member is destroyed */
  free(destroy_me);
  return TRUE;
}


/* ??? Why are Clean* in this file rather than mlist_internal? */

/* Remove all links from within a list. */
void CleanList(_RTP_CONTEXT *the_context, doubly_linked_list *the_list){
  struct link *cur_link, *next_link;
  cur_link = the_list->first_link;
  while (cur_link != NULL){
    next_link = cur_link->next_link;
    DestroyMember(the_context, cur_link->my_member);
    cur_link = next_link;
  }
  return;
}




/* Wipes all members off of the membership list */
void CleanMembershipList(_RTP_CONTEXT *the_context, membership_list *l){
  CleanList(the_context, &l->actual_list);
  l->confirmed_members = 0;
  l->oldest_not_yet_soft = NULL;
}



/* Remove all links from within a hash table. */
void CleanHash(_RTP_CONTEXT *the_context, hash_table *the_hash){
  int i;
  for (i=0; i<HASHSIZE;i++){
    CleanList(the_context, the_hash->hashval + i);
  }
  return;
}


/* We have received data from the given ssrc and fromaddr (either RTP or
 * RTCP).  If a corresponding member exists, update its last-send time;
 * otherwise, create a new member.
 * If we have a member with the given ssrc but the fromaddr is different,
 * there are two likely possibilities -- this is a collision, or the member
 * moved IP addresses.  (We used to assume it was always a collision, but
 * that fails for multi-homed hosts or IP mobility.)  If there is some
 * member on the collides_with list which has this fromaddr, we know we're
 * (almost certainly) coming from that member; if not, pick one of the
 * members with that ssrc arbitrarily (currently this will be the last one
 * in the collides_with list).
 * If the fromaddr was _RTP_ADDRESS_NOT_YET_KNOWN, we pick the first member
 * on the list, similarly arbitrarily.
 * These arbitrary choices are unlikely to matter since the odds of an ssrc
 * collision happening at the same time as an ip-address change are rather
 * slim to say the least, so the list should only be one member long.
 * Note that because of this ambiguity, this function doesn't set the
 * my_addr field or call the callback informing the user of the changed
 * address unless the address was not yet known.  */
static member* UpdateTimeOrCreateMember(_RTP_CONTEXT *the_context,
					u_int32 ssrc,
					struct sockaddr *fromaddr,
					int addrlen,
					int addr_is_rtcp,
					memberstatus status){
  member *pkt_origin_member;
  struct timeval tvalptr;
  int addr_idx = (addr_is_rtcp != 0);

  pkt_origin_member = GetMemberFromSSRCHash(the_context, ssrc);
  if (pkt_origin_member==NULL){
    pkt_origin_member = EstablishNewMember(the_context, ssrc, NULL,
					   fromaddr, addr_is_rtcp, status);
  } else {
    if (fromaddr->sa_family != _RTP_ADDRESS_NOT_YET_KNOWN) {
      /* Find the first member on the collides_with list (the list of
	 members with this ssrc) whose address either matches fromaddr, or
	 does not have a valid address. */
      while (pkt_origin_member->collides_with_next != NULL) {
	if (pkt_origin_member->my_addr[addr_idx].sa_family ==
	    _RTP_ADDRESS_NOT_YET_KNOWN) {
	  break;
	}
	if (!FromDifferentSource(fromaddr,
				 &pkt_origin_member->my_addr[addr_idx],
				 addrlen)) {
	  break;
	}
	pkt_origin_member = pkt_origin_member->collides_with_next;
      }
      if (pkt_origin_member->my_addr[addr_idx].sa_family ==
	  _RTP_ADDRESS_NOT_YET_KNOWN) {
	pkt_origin_member->my_addr[addr_idx] = *fromaddr;

	if (the_context->ChangedMemberSockaddrCallBack != NULL &&
	    pkt_origin_member->unique_id != 0) {

	  the_context->
	    ChangedMemberSockaddrCallBack(the_context->context_num,
					  pkt_origin_member->unique_id,
					  fromaddr,
					  addr_is_rtcp);
	}
      }
    }

    gettimeofday(&tvalptr, NULL);
    UpdateMemberTime(the_context, pkt_origin_member, tvalptr);
  }
  return pkt_origin_member;
}

rtperror UpdateMemberInfoBySR(_RTP_CONTEXT *the_context, 
			      rtcp_packet *the_packet,
			      struct sockaddr *fromaddr,
			      int addrlen) {
  member *pkt_origin_member;
  rtcp_report_block the_block;
  struct timeval tvalptr;
  unsigned int ssrc_iter;

  gettimeofday(&tvalptr, NULL);

  pkt_origin_member =
    UpdateTimeOrCreateMember(the_context,
			     the_packet->variant.sr.sr_fixed->ssrc,
			     fromaddr, addrlen, TRUE, RTP_MEMBER_PENDING);
  pkt_origin_member->recv_rtp_count++;

  /* Now deal with member state changes. The member should move to pending if it has sent two RT(c)P packets */
  if(((pkt_origin_member->recv_rtp_count > 1) &&
      (pkt_origin_member->status == RTP_MEMBER_PENDING)) ||
     (pkt_origin_member->status == RTP_MEMBER_EXPIRED)){
    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED);
  }

  UpdateSenderTime(the_context, pkt_origin_member, tvalptr, 0);

  /* update remaining fields based on receipt of packet */
  /* NTP timestamp */
  pkt_origin_member->ntp_timestamp.secs =
    the_packet->variant.sr.sr_fixed->ntp_secs;
  pkt_origin_member->ntp_timestamp.frac =
    the_packet->variant.sr.sr_fixed->ntp_frac;
  /* RTP timestamp */
  pkt_origin_member->rtp_timestamp =
    the_packet->variant.sr.sr_fixed->rtp_stamp;
  /* pkt and oct counts */
  pkt_origin_member->pkt_count = 
    the_packet->variant.sr.sr_fixed->pkt_count;
  pkt_origin_member->oct_count =
    the_packet->variant.sr.sr_fixed->oct_count;

  /* For each sender report block, update sender info as held by
     this member */
  for (ssrc_iter=0; ssrc_iter < RTCP_RC(*the_packet->common); ssrc_iter++) {
    member *reported_sender;
    the_block = RTPGetReportBlock(the_packet, ssrc_iter);
    /* NOTE: We don't update sender times based on the
       report block, because 2 receivers could perpetually update
       each other's sender list even after the sender has left. */
    /* We create members from the SSRC list if they are not
       already part of the session */
    if ((reported_sender =
	 GetMemberFromSSRCHash(the_context, the_block.ssrc)) == NULL) {
      reported_sender =
	EstablishNewMember(the_context, the_block.ssrc,
			   NULL, fromaddr, TRUE, RTP_MEMBER_PENDING);
    }
    UpdateSenderTime(the_context, reported_sender, tvalptr, 0);
    Update_RR(pkt_origin_member, reported_sender, &the_block);
  }

  /* update our local info about the member's last SR send time */
  pkt_origin_member->last_sr_receipt_time = tvalptr;

  return(RTP_OK);

}
rtperror UpdateMemberInfoByRR(_RTP_CONTEXT *the_context, 
			      rtcp_packet *the_packet,
			      struct sockaddr *fromaddr,
			      int addrlen) {


  member *pkt_origin_member;
  rtcp_report_block the_block;
  struct timeval tvalptr;
  unsigned int ssrc_iter;

  gettimeofday(&tvalptr, NULL);


  pkt_origin_member =
    UpdateTimeOrCreateMember(the_context,
			     the_packet->variant.rr.rr_fixed->ssrc,
			     fromaddr, addrlen, TRUE, RTP_MEMBER_PENDING);
  pkt_origin_member->recv_rtp_count++;

  /* Now deal with member state changes. The member should move to pending if it has sent two RTP packets */
  if(((pkt_origin_member->recv_rtp_count > 1) &&
      (pkt_origin_member->status == RTP_MEMBER_PENDING)) ||
     (pkt_origin_member->status == RTP_MEMBER_EXPIRED)){
    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED);
  }

  for (ssrc_iter=0; ssrc_iter < RTCP_RC(*the_packet->common); ssrc_iter++){
    member *reported_sender;
    the_block = RTPGetReportBlock(the_packet, ssrc_iter);
    /* NOTE: We don't update sender times based on the
       report block, because 2 receivers could perpetually update
       each other's sender list even after the sender has left. */
    /* We create a member that is on the SSRC list if it is 
       not already part of the session */
    if ((reported_sender = 
	 GetMemberFromSSRCHash(the_context, the_block.ssrc)) == NULL) {
      reported_sender = 
	EstablishNewMember(the_context, the_block.ssrc, NULL,
			     fromaddr, TRUE, RTP_MEMBER_PENDING);
    }
    UpdateSenderTime(the_context, reported_sender, tvalptr, 0);
    Update_RR(pkt_origin_member, reported_sender, &the_block);
  }

  return(RTP_OK);
}

rtperror UpdateMemberInfoBySDES(_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen) {


  member *pkt_origin_member = NULL;
  member *prev_member;
  rtcp_sdes_item sdes_item;
  char **member_item;
  char the_item[100];
  u_int32 cur_ssrc, prev_ssrc;
  struct timeval tvalptr;
  unsigned int ssrc_iter;
  gettimeofday(&tvalptr, NULL);

  if (RTCP_RC(*the_packet->common)==0){
    return(RTP_OK);
  }

  ssrc_iter = 0;
  sdes_item = InitSDESItemIter(the_packet);

  /* Make prev_ssrc not match initial ssrc.  This is so the
     member will be retrieved*/
  prev_ssrc = sdes_item.ssrc + 1;

  /* The item pointed to does not represent the end of
     all items in the packet until it's type has
     value 0 and its chunk # = that of the rc field of the
     sdes packet. */
  while (sdes_item.type != 0 || 
	 ssrc_iter < RTCP_RC(*the_packet->common)){
    /* only has stuff worth processing if the time type
       is not 0. */

    if (sdes_item.type > 0 && sdes_item.type < _RTP_NUM_SDES_TYPES + 1){

      cur_ssrc = sdes_item.ssrc;
      if (cur_ssrc != prev_ssrc){
	  /* NOTE: This will update the membership time of every
	     member that contains info in the SDES packet */

	if(ssrc_iter == 0) {
	  pkt_origin_member =
	    UpdateTimeOrCreateMember(the_context, cur_ssrc,
				     fromaddr, addrlen, TRUE, 
				     RTP_MEMBER_PENDING);

	  pkt_origin_member->recv_rtp_count++;

	  /* Now deal with member state changes. The member should
	     move to pending if it has sent two RTP packets */
	  if(((pkt_origin_member->recv_rtp_count > 1) &&
	      (pkt_origin_member->status == RTP_MEMBER_PENDING)) ||
	     (pkt_origin_member->status == RTP_MEMBER_EXPIRED)){
	    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED);
	  }

	} else {
	  pkt_origin_member =
	    UpdateTimeOrCreateMember(the_context, cur_ssrc,
				     fromaddr, addrlen, TRUE, 
				     RTP_MEMBER_PENDING_CONTRIBUTOR);
	  
	  pkt_origin_member->recv_csrc_count++;

	  /* Now deal with member state changes. The member should
	     move to pending if it has sent two RTP packets */
	  if(((pkt_origin_member->recv_csrc_count > 1) &&
	      (pkt_origin_member->status == RTP_MEMBER_PENDING_CONTRIBUTOR)) ||
	     (pkt_origin_member->status == RTP_MEMBER_EXPIRED_CONTRIBUTOR)){
	    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED_CONTRIBUTOR);
	  }


	}

      }

      /* set member_item to point to the location where we 
	 store the SDES item for the member. */
      member_item = pkt_origin_member->sdes_info + sdes_item.type - 1;
      if (*member_item != NULL){
	  
	/* Previous SDES info for this field existed.  If the
	   field remains the same, then do not make a change. */
	if (strncmp(*member_item, sdes_item.description,
		    sdes_item.len) != 0){
	  if (sdes_item.type == 1){
	    /* If we get here, then we found a member with the same
	       SSRC but different CNAME.  We need to
	       search through the rest of the colliding members
	       to see if we can find a member with the same
	       address and CNAME -- i.e. if we've already seen and handled
	       this collision.  If not, handle the collision now.
	       If so, just have pkt_origin_member point to the found
	       member. */

#ifdef _RTP_DEBUG
	    printf("Members with different CNAMEs collide\n");
#endif

	    while (pkt_origin_member->collides_with_next != NULL){
	      if (strncmp(*member_item, sdes_item.description,
			  sdes_item.len) == 0){
		return(RTP_OK);
	      }
	      pkt_origin_member = pkt_origin_member->collides_with_next;
	      member_item = pkt_origin_member->sdes_info +
		sdes_item.type - 1;
	    }


#ifdef _RTP_DEBUG
	    printf("Handle1...\n");
#endif
	    
	    pkt_origin_member =
	      HandleSSRCCollision(the_context, pkt_origin_member,
				  fromaddr, TRUE, the_item,ssrc_iter);
	  }
	  else {
	    /* We are changing an SDES item other than CNAME.  Free the old
	       one here, and put the new one in below. */
	    free(*member_item);
	    *member_item = NULL;
	  }
	} 
	if (sdes_item.type == 1 &&
	    fromaddr->sa_family != _RTP_ADDRESS_NOT_YET_KNOWN) {
	  /* Either we've found the member with this CNAME and SDES, or
	     we've just created a new member.  Check to see if the
	     member's addr is the same as the packet's fromaddr; if not,
	     announce an IP address change for the member. */
	  if (pkt_origin_member->my_addr[1].sa_family ==
	      _RTP_ADDRESS_NOT_YET_KNOWN ||
	      FromDifferentSource(fromaddr, &pkt_origin_member->my_addr[1],
				  addrlen)) {
	    pkt_origin_member->my_addr[1] = *fromaddr;

	    if (the_context->ChangedMemberSockaddrCallBack != NULL &&
		pkt_origin_member->unique_id != 0) {
	      the_context->
		ChangedMemberSockaddrCallBack(the_context->context_num,
					      pkt_origin_member->unique_id,
					      &pkt_origin_member->my_addr[1],
					      TRUE);
	    }
	  }
	}
      }
      else if (sdes_item.type == 1){
	/* CNAME was previously NULL for member, so we must check to
	   see if the CNAME has already been used.  If so, we need
	   to finish up moving a member due to a collision.
	   If not, we note that this is a new member. */
	strncpy(the_item, sdes_item.description, sdes_item.len);
	the_item[sdes_item.len] = '\0';
	prev_member = GetMemberFromCNAMEHash(the_context, the_item);
	if (prev_member != NULL){ /* CNAME already used. */
	  if (prev_member == the_context->my_memberinfo) {
	    /* The already-used CNAME was our own!  Through error or
	       malice or mis-configuration, we've received a packet which
	       didn't come from our IP address or have our ssrc, but which
	       has our CNAME.  Assume this was a looped back packet. */
	    return errordebug(RTP_PACKET_LOOPBACK, "UpdateMemberInfoByRTCP",
			      "context %d, got a foreign packet with our CNAME",
			      (int)the_context->context_num);

	  }
	  /* CNAMEs are unique.  Thus, we can be sure that we
	     have retrieved the sole member with the same CNAME,
	     and that these 2 members should be merged. */

#ifdef _RTP_DEBUG
	  printf ("*** Going to merge members... ***\n");
#endif

	  pkt_origin_member = 
	    MergeMembers(the_context, prev_member, pkt_origin_member);
	  member_item = pkt_origin_member->sdes_info + sdes_item.type - 1;
	}
      }


      /* Whether we just freed memory or it has never been 
	 allocated, now we add the new SDES item */
	
      if (*member_item == NULL){
	*member_item = malloc(sdes_item.len + 1);
	strncpy(*member_item, sdes_item.description, sdes_item.len);
	(*member_item)[sdes_item.len] = '\0';
	if (sdes_item.type == 1){

	  /* Item is a CNAME.  CNAME data has just come in for this
	     member so we add the member to the CNAME hash at this
	     point */
	  EnterCNAMEHashTable(the_context, pkt_origin_member);

	  /* Change status to confirmed since we've got cname */
	  if((ssrc_iter == 0) &&
	     (pkt_origin_member->status != RTP_MEMBER_CONFIRMED)) {
	    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED);
	  } else if(ssrc_iter != 0) {
	    /* This is a CSRC who has a new CNAME. They're now a confirmed contributor */
	    ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED_CONTRIBUTOR);
	  }


	}
	if (the_context->ChangedMemberInfoCallBack != NULL) {
	  the_context->
	    ChangedMemberInfoCallBack(the_context->context_num,
				      pkt_origin_member->unique_id,
				      sdes_item.type, *member_item,
				      RTP_FLAG_MEMBER_INFO_CHANGES);
	}
      }
      prev_ssrc = cur_ssrc;
    }
    else (ssrc_iter++);
    if (ssrc_iter < RTCP_RC(*the_packet->common)){
      sdes_item = GetNextItem(&sdes_item);
    }
      
  }

  return(RTP_OK);
}

rtperror UpdateMemberInfoByBYE(_RTP_CONTEXT *the_context, 
			       rtcp_packet *the_packet,
			       struct sockaddr *fromaddr,
			       int addrlen) {


  member *pkt_origin_member;
  u_int32 cur_ssrc;
  unsigned int ssrc_iter;
  int i;
  rtcp_bye_block bb;


  /* Copy the reason code (non null terminated) from the packet
     into the context variable, so it can be returned to the
     callback */
  for(i = 0; i < the_packet->variant.bye.reason_length; i++) {
    the_context->byereason[i] = the_packet->variant.bye.reason[i];
  }
  the_context->byereason[i] = '\0';

  /* Update info for the various members */
  for (ssrc_iter=0; ssrc_iter < RTCP_RC(*the_packet->common); ssrc_iter++){

    bb = RTPGetByeBlock(the_packet, ssrc_iter);
    cur_ssrc = bb.ssrccsrc;

    pkt_origin_member =
      UpdateTimeOrCreateMember(the_context, cur_ssrc,
			       fromaddr, addrlen, TRUE, 
			       RTP_MEMBER_PENDING);

    /* The member that is about to go BYE has been placed on the
       membership list if it wasn't already there, and timestamped
       with the current time (since we just received a BYE packet
       for it, the current time is appropriate).  We now destroy it
       unless the BYE was (believed to be) due to a collision and the
       CNAME is known.  In such a case, we leave the member on the
       member-list.  Note that the remove will fail if the member is in
       the CSRC list */

    if (!pkt_origin_member->colliding ||
	pkt_origin_member->sdes_info[0] == NULL){

      RemoveMember(the_context, pkt_origin_member);

      if (the_context->UpdateMemberCallBack != NULL){
	the_context->UpdateMemberCallBack(the_context->context_num,
					  pkt_origin_member->unique_id,
					  RTP_FLAG_MEMBER_LEAVES,
					  the_context->byereason);
      }
      DestroyMember(the_context, pkt_origin_member);
    }
  }

  return(RTP_OK);
}

/* Update the member information of those members whose information is
 * contained in the RTCP packet received.
 */
rtperror UpdateMemberInfoByRTCP(_RTP_CONTEXT *the_context, 
				rtcp_packet *the_packet,
				struct sockaddr *fromaddr,
				int addrlen,
				int part_in_compound_pkt){

  member *pkt_origin_member = NULL;
  rtperror err;
  struct timeval tvalptr;

  err = RTP_OK;
  gettimeofday(&tvalptr, NULL);

#ifdef _RTP_DEBUG
  printf("Updating member info for part %d of pkt\n", part_in_compound_pkt);
#endif

  switch (the_packet->common->pt){
  case RTCP_PACKET_SR: /* SR RTCP packet */
    err = UpdateMemberInfoBySR(the_context, the_packet, fromaddr, addrlen);
    break;
						 
  case RTCP_PACKET_RR: /* RR RTCP packet */
    err = UpdateMemberInfoByRR(the_context, the_packet, fromaddr, addrlen);
    break;

  case RTCP_PACKET_SDES: /* SDES RTCP packet */
    err = UpdateMemberInfoBySDES(the_context, the_packet, fromaddr, addrlen);
    break;
    
  case RTCP_PACKET_BYE: /* BYE RTCP packet */
    err = UpdateMemberInfoByBYE(the_context, the_packet, fromaddr, addrlen);
    break;

  case RTCP_PACKET_APP: /* APP RTCP packet */
    /* APP packets have relevant info, other than the fact we
       should update the membership timestamp of the sender */
    pkt_origin_member = 
      UpdateTimeOrCreateMember(the_context, 
			       the_packet->variant.app.app_fixed->ssrccsrc,
			       fromaddr, addrlen, TRUE, 
			       RTP_MEMBER_PENDING);
    pkt_origin_member->recv_rtp_count++;

    /* Now deal with member state changes. The member should
       move to pending if it has sent two RTP packets */
    if(((pkt_origin_member->recv_rtp_count > 1) &&
	(pkt_origin_member->status == RTP_MEMBER_PENDING)) ||
       (pkt_origin_member->status == RTP_MEMBER_EXPIRED)){
      ChangeMemberStatus(the_context,pkt_origin_member,RTP_MEMBER_CONFIRMED);
    }
    
    break;
  default:
    return errordebug(RTP_BAD_PACKET_FORMAT, "UpdateMemberInfoByRTCP",
		      "context %d, invalid RTCP payload type for packet format",
		      (int)the_context->context_num);
  }

  /* Update the fact that this is the most recent rtcp sender */
  the_context->most_recent_rtcp_sender = pkt_origin_member;

  /* Update the sender's RTCP count and most recent RTCP packet send time. */
  if (pkt_origin_member != NULL){
    /* Only update the RTCP count if this was the first packet in the
       compound.
       Note: The statistics are wrong if the different packets in the compound
       had different ssrc's. */
    if (part_in_compound_pkt == 0) {
        pkt_origin_member->rtcp_count++;
    }
    pkt_origin_member->last_rtcp_send_time = tvalptr;
  }

  /*Update membership lists on RTCP reception*/

  UpdateMembershipLists(the_context);

  return err;
}
    

/* Update the member information of those members whose information is
 * contained in the RTP packet received.
 */
rtperror UpdateMemberInfoByRTP(_RTP_CONTEXT *the_context, 
			       rtp_packet *the_packet,
			       struct sockaddr *fromaddr,
			       int addrlen){
  u_int32 member_ssrc = the_packet->RTP_header->ssrc;
  int32 seq_inc;
  unsigned int i;
  struct timeval curtime;
  member *sending_member;
  double cur_jitter;
  /*  printf("RTP update member: family: %d\n", (int) fromaddr->sa_family); */

  
  sending_member =
    UpdateTimeOrCreateMember(the_context, member_ssrc,
			     fromaddr, addrlen, FALSE,
			     RTP_MEMBER_PENDING);

  /* Increment local count */
  sending_member->recv_rtp_count++;

  /* Now deal with member state changes. The member should move to pending if it has sent two RTP packets */
  if(((sending_member->recv_rtp_count > 1) &&
      (sending_member->status == RTP_MEMBER_PENDING)) ||
     (sending_member->status == RTP_MEMBER_EXPIRED)){
    ChangeMemberStatus(the_context,sending_member,RTP_MEMBER_CONFIRMED);
  }

  /* Update status on sender queue */
  gettimeofday(&curtime, NULL);
  /* Update the fact that this is the most recent sender */
  the_context->most_recent_rtp_sender = sending_member;
  /* Update the fact that we know that the member is still a sender */
  UpdateSenderTime(the_context, sending_member, curtime,1);
  
  for (i=0; i < RTP_CC(*the_packet->RTP_header); i++){
    member_ssrc = RTPPacketGetCSRC(the_packet, i);
    sending_member = UpdateTimeOrCreateMember(the_context, member_ssrc,
					      fromaddr, addrlen, FALSE,
					      RTP_MEMBER_PENDING_CONTRIBUTOR);

    /* Increment local count */
    sending_member->recv_csrc_count++;

    /* Now deal with CSRC member state changes. The member should
       move to pending if it has sent two RTP packets with its CSRC*/

    if(((sending_member->recv_csrc_count > 1) &&
	(sending_member->status == RTP_MEMBER_PENDING_CONTRIBUTOR)) ||
       (sending_member->status == RTP_MEMBER_EXPIRED_CONTRIBUTOR)) {
      ChangeMemberStatus(the_context,sending_member,RTP_MEMBER_CONFIRMED_CONTRIBUTOR);
    }

  } 

  /* Update jitter */
  if (sending_member->local_info == NULL){
    /* local info has not been created, so create it, store the
       delay of the first packet and set jitter to -1, so that
       next packet received, we can set jitter */
    sending_member->local_info = malloc(sizeof(rtp_sndr_local));
    sending_member->local_info->jitter = -1;
    sending_member->local_info->prev_sndtime = the_packet->RTP_header->ts;
    gettimeofday(&sending_member->local_info->prev_rcvtime, NULL);
    sending_member->local_info->num_pkts_recd = 0;
    sending_member->local_info->num_pkts_recd_since_last = 0;
    sending_member->local_info->expected_prior = 0;
    sending_member->local_info->first_seqno = 
      (u_int32) the_packet->RTP_header->seq;
    sending_member->local_info->highest_seqno =
      sending_member->local_info->first_seqno;
    sending_member->local_info->expected_prior = 0;
  }
  else {
    gettimeofday(&curtime, NULL);
    cur_jitter = fabs((double) the_packet->RTP_header->ts -
		      (double) sending_member->local_info->prev_sndtime -
		      RTPTimeDiff(the_context, &curtime, 
				  &sending_member->local_info->prev_rcvtime,
		      (int8) RTP_PT(*the_packet->RTP_header)));
    if (sending_member->local_info->jitter == -1){
      /* This is the initial jitter calculation, so set
	 the jitter to the currently calculated value */
      sending_member->local_info->jitter = cur_jitter;
    }
    else {
      /* Update the jitter estimate (based on exponentially increasing
	 scale) */
      sending_member->local_info->jitter *= 15.0 / 16.0;
      sending_member->local_info->jitter += cur_jitter / 16.0;
    }
    sending_member->local_info->prev_sndtime = the_packet->RTP_header->ts;
    sending_member->local_info->prev_rcvtime = curtime;
  }

  sending_member->local_info->num_pkts_recd_since_last++;
  seq_inc = the_packet->RTP_header->seq -
    (sending_member->local_info->highest_seqno % 65536);
  if (seq_inc > 0){
    sending_member->local_info->highest_seqno += seq_inc;
  }
  else if (seq_inc < -32768) {
    /* The sequence number has decreased by more than half its possible
       values, so we presume that it has wrapped around.
       Adding the change plus 65536 will update the 32-bit extended sequence
       number (sequence number plus cycle counter) correctly. */
    /* XXX it's not clear to me that this does the right thing in the
       case of sequence number discontinuities, especially in the wake of a
       MergeMembers. */
    sending_member->local_info->highest_seqno += (65536 + seq_inc);
  }
  return RTP_OK;
}
