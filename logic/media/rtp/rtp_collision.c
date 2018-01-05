/* rtp_collision.c : functionality used to resolve SSRC collisions
                     in an RTP session.

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/


#include "sysdep.h"

#include "rtp_api_internal.h"
#include "rtp_collision.h"
#include "rtp_encrypt.h"

/* Compute a new SSRC for the_context, and do any updating as far as the
 * context colliding. */
void ComputeNewSSRC(_RTP_CONTEXT *the_context){
  u_int32 new_ssrc;
  member *other_member;
  int loopcount;

  /* Only redraw a certain number of times. If we can't find
     a unique SSRC, give up */
  loopcount = 0;
  do {
#ifdef _RTP_SEMI_RANDOM
    /* SSRCs that collide will not collide after this call, even
       without the initializing factor */
    new_ssrc = random32(the_context->my_ssrc)
      + (long) the_context->last_rtcp_send.tv_usec;
#else
    new_ssrc = random32(the_context->my_ssrc);
#endif

    /* Now, we take into account the fact that there may be static
       bits required in the SSRC */

    new_ssrc = (the_context->static_ssrc & the_context->ssrc_mask) |
      (new_ssrc & ~(the_context->ssrc_mask));
    other_member = GetMemberFromSSRCHash(the_context, new_ssrc);
    loopcount++;
  } while ((other_member != NULL) && (loopcount < _RTP_MAXIMUM_SSRC_REDRAWS));

  if(loopcount == _RTP_MAXIMUM_SSRC_REDRAWS) {
    return;
  }

  RemoveMemberFromSSRCHash(the_context, the_context->my_memberinfo);
  the_context->my_memberinfo->ssrc = new_ssrc;
  the_context->my_ssrc = new_ssrc;
  EnterSSRCHashTable(the_context, the_context->my_memberinfo);
  /* If I collided with anyone and pointed at them, then put
     them on the SSRC Hash table */
  if (the_context->my_memberinfo->collides_with_prev == NULL){
    /* If not NULL, then somebody else is already on the SSRC hash
       for this SSRC */
    if (the_context->my_memberinfo->collides_with_next != NULL){
      
#ifdef _RTP_DEBUG
      printf("Hashing a collider\n");
#endif
      
      EnterSSRCHashTable(the_context,
			 the_context->my_memberinfo->collides_with_next);
    }
  }
  else {
    the_context->my_memberinfo->collides_with_prev->collides_with_next = 
      the_context->my_memberinfo->collides_with_next;
  }
  if (the_context->my_memberinfo->collides_with_next != NULL){
    the_context->my_memberinfo->collides_with_next->collides_with_prev =
      the_context->my_memberinfo->collides_with_prev;
  }
  
  /* No longer collides - moved out of colliding state */
  the_context->my_memberinfo->colliding = FALSE; 
  the_context->my_memberinfo->collides_with_next = NULL;
  the_context->my_memberinfo->collides_with_prev = NULL;
}


/* When an SSRC collision is detected, this function should be called.
   If the collision is detected by address and CNAME is not known for
   new member, new_cname should be set to NULL.  This can happen when
   the collision is due to the application setting the CSRC to
   conflict with someone else.  new_addr should never be null (since a
   packet always comes from somewhere) except for the CSRC case again.
   A particular member will send SDES packets for its SSRC and any
   CSRCs it provides.  Thus, its SSRC as well as the list of CSRCs
   will be marked as coming from that address. */

member *HandleSSRCCollision(_RTP_CONTEXT *the_context, member *the_member,
			    struct sockaddr *fromaddr,
			    int addr_is_rtcp,
			    char *new_cname, int is_contrib){
  member *other_member;
  unsigned int the_flag;
  contributor_t *cont;

#ifdef _RTP_DEBUG
  printf("Complaint with member %lx  ", the_member->unique_id);
  printf("SSRC = %ld\n", the_member->ssrc);
#endif

  /* Let's just make sure that we are on the last member in
     the colliding list */
  while (the_member->collides_with_next != NULL){
    the_member = the_member->collides_with_next;
  }

  /* We report 3 different kinds of collisions: collisions with
     our SSRC, collisions with one of our CSRCs, or a (third
     party) observed collision. */

  the_member->colliding = TRUE;
  
  /* Create the colliding member. */
  other_member = EstablishNewMember(the_context, the_member->ssrc, 
				    NULL, fromaddr, addr_is_rtcp,
				    RTP_MEMBER_PENDING);

  /* This is a new member. We are here only if we have learned about
     them through an SDES. So, if they are the first chunk, its a 
     confirmed member. For any other chunk, its a contributor. The flag
     in the function call indicates which. So, change status appropriately.
     EstablishMember will have set it to pending. */

  if(is_contrib == 0) {
    ChangeMemberStatus(the_context, other_member, RTP_MEMBER_CONFIRMED);
  } else {
    ChangeMemberStatus(the_context, other_member, RTP_MEMBER_CONFIRMED_CONTRIBUTOR);
  }

  /* We don't want multiple colliding members in the SSRC hash,
     but they do appear in memberlist, senderlist, unique id hash, and
     CNAME hash (when they get their cname) */

  RemoveMemberFromSSRCHash(the_context, other_member);
  other_member->colliding = TRUE;
  other_member->collides_with_prev = the_member;
  the_member->collides_with_next = other_member;

#ifdef _RTP_DEBUG
  printf("Member %p collides next is %p\n", the_member, other_member);
#endif

  if (new_cname != NULL){
    other_member->sdes_info[0] = (char*) malloc(strlen(new_cname) + 1);
    strcpy(other_member->sdes_info[0], new_cname);
    EnterCNAMEHashTable(the_context, other_member);
  }
     
  /* Check for CSRC collision */
  cont = the_context->contriblist;
  the_flag = 0;
  while(cont != NULL) {
    if (the_member->ssrc == cont->ssrc) {
      the_flag = RTP_FLAG_A_CSRC_COLLIDES;
      break;
    }
    cont = cont->next;
  }
  /* Call callback for CSRC collision */
  if ((the_flag == RTP_FLAG_A_CSRC_COLLIDES) && 
      (the_context->CollidedMemberCallBack != NULL)){
    the_context->CollidedMemberCallBack(the_context->context_num,
					the_member->unique_id,
					other_member->unique_id,
					the_flag);
  }

  /* Now check if our current SSRC collides */
  
  if (the_member->ssrc == the_context->my_ssrc){
#ifdef _RTP_DEBUG
    printf("Member %ld ssrc %ld colliding with my ssrc will trigger a BYE\n",
	   other_member->unique_id, the_member->ssrc);
#endif
    /* Send BYE and change SSRC */
    the_flag = RTP_FLAG_COLLIDE_WITH_ME;
    if (the_context->CollidedMemberCallBack != NULL){
      the_context->CollidedMemberCallBack(the_context->context_num,
					  the_member->unique_id,
					  other_member->unique_id,
					  the_flag);
    }

  }


  /* If the collision is with the local SSRC, then schedule a BYE packet
     and update the SSRC */
  /* XXX: collisions with our CSRC's? */
  if (the_member->ssrc == the_context->my_ssrc){
    struct timeval now;
    gettimeofday(&now, NULL);
    RTPSetTimer(the_context->context_num, RTP_TIMER_SEND_BYE_COLLISION,
		the_member->ssrc, "Leaving to resolve a collision",
		&now);

    /* Store our old ssrc value */
    
    the_context->my_previous_ssrc = the_context->my_memberinfo->ssrc;
    
    /* Pick new values for SSRC */
    ComputeNewSSRC(the_context);
#ifdef _RTP_DEBUG
    printf("Out of HandleSSRCCollis...\n");
#endif
  }

  /* Now we've handled all the callbacks - return the pointer to our new member */

  return(other_member);
}


static void MergeLocalInfo(rtp_sndr_local *prev_local, rtp_sndr_local *new_local)
{
  struct timeval notime;
  notime.tv_sec = 0;
  notime.tv_usec = 0;

  if (new_local->prev_sndtime > prev_local->prev_sndtime) {
    prev_local->prev_sndtime = new_local->prev_sndtime;
  }
  if (TimeExpired(&prev_local->prev_rcvtime,
		  &new_local->prev_rcvtime, &notime)) {
    prev_local->prev_rcvtime = new_local->prev_rcvtime;
  }
  
  prev_local->num_pkts_recd +=  new_local->num_pkts_recd;
  
  if (prev_local->highest_seqno < new_local->highest_seqno) {
    prev_local->highest_seqno = new_local->highest_seqno;
  }
}


/* Take two members with different SSRCs but the same CNAME and join them
 * into a single member.  The new_member is then destroyed.
 * Return the member which was preserved.
 */
member *MergeMembers(_RTP_CONTEXT *the_context, member *prev_member,
		  member *new_member){
  struct timeval notime;
  int i;
  member *swapmem;
  contributor_t *cont;
  unsigned int the_flag = 0;
  void *pt_to_user_data;
  struct link *temp_link;
  notime.tv_sec = 0;
  notime.tv_usec = 0;

  /* Always keep the original member (with smaller unique id) around */
  if (new_member->unique_id < prev_member->unique_id){
    swapmem = new_member;
    new_member = prev_member;
    prev_member = swapmem;
  }

#ifdef _RTP_DEBUG
  printf("**********\n*********\n*******Merging members %ld and %ld into %ld",
	 prev_member->unique_id, new_member->unique_id,
	 prev_member->unique_id);
#endif
  
  if (TimeExpired(&prev_member->last_update_time,
		 &new_member->last_update_time, &notime)){
    prev_member->last_update_time = new_member->last_update_time;
  }
  if (TimeExpired(&prev_member->last_rtp_send_time,
		 &new_member->last_rtp_send_time, &notime)){
    prev_member->last_rtp_send_time = new_member->last_rtp_send_time;
  }
  if (TimeExpired(&prev_member->last_rtp_send_time,
		 &new_member->last_rtp_send_time, &notime)){
    prev_member->last_rtp_send_time = new_member->last_rtp_send_time;
  }
  if (TimeExpired(&prev_member->last_rtcp_send_time,
		 &new_member->last_rtcp_send_time, &notime)){
    prev_member->last_rtcp_send_time = new_member->last_rtcp_send_time;
  }
  if (TimeExpired(&prev_member->last_sr_receipt_time,
		 &new_member->last_sr_receipt_time, &notime)){
    prev_member->last_sr_receipt_time = new_member->last_sr_receipt_time;
  }

  /* We don't need to update the unique id because prev_member has
     the unique ID that we wish to use (it already was assoc. with
     the CNAME).  However, we place the new member's unique_id in
     the remap section. */
  the_context->ID_remap[the_context->num_remaps] = new_member->unique_id;
  the_context->ID_orig_val[the_context->num_remaps] = prev_member->unique_id;
  the_context->num_remaps++;
  the_flag &= RTP_FLAG_UNIQUE_ID_REMAP;
  pt_to_user_data = NULL;
  if (prev_member->user_data != NULL && new_member->user_data != NULL){
    pt_to_user_data = new_member->user_data;
  }
  if (the_context->RevertingIDCallBack != NULL){
    the_context->RevertingIDCallBack(the_context->context_num,
				     prev_member->unique_id,
				     new_member->unique_id,
				     pt_to_user_data,
				     the_flag);
  }
  /* If RevertingIDCallBack is NULL, then we call BYE in its place. */
  else   if (the_context->UpdateMemberCallBack != NULL){
    the_context->UpdateMemberCallBack(the_context->context_num,
				      new_member->unique_id,
				      the_flag,
				      NULL);
  }



  if (the_context->num_remaps == _RTP_MAX_UNIQUE_ID_REMAPS){

    /* ran out of remapping space.  Though this should never happen,
       just in case, we flush out the first half of the remap buffer. */

    for (i=0; i < _RTP_MAX_UNIQUE_ID_REMAPS / 2; i++){
      the_context->ID_remap[i] =
	the_context->ID_remap[i + _RTP_MAX_UNIQUE_ID_REMAPS / 2];
      the_context->ID_orig_val[i] =
	the_context->ID_orig_val[i + _RTP_MAX_UNIQUE_ID_REMAPS / 2];
    }
    the_context->num_remaps = _RTP_MAX_UNIQUE_ID_REMAPS / 2;
  }

  
  if (prev_member->user_data == NULL){
    prev_member->user_data = new_member->user_data;
    new_member->user_data =  NULL;
  }

  prev_member->ssrc = new_member->ssrc;
  
  /* If the new SSRC also turns out to collide, we better stay in
     the colliding state. */
  
  prev_member->colliding = new_member->colliding;
  
  /* Keep the old source addresses (my_addr[]) */
  
  /* Copy any SDES info from new member.  The CNAME is copied because
     it should be NULL */


  for (i = 0; i< _RTP_NUM_SDES_TYPES; i++){
    if (new_member->sdes_info[i] != NULL){
      int announce = 0;
      if (prev_member->sdes_info[i] != NULL){
	if (strcmp(prev_member->sdes_info[i], new_member->sdes_info[i]) != 0)
	  announce = 1;
	free(prev_member->sdes_info[i]);
      } else {
	announce = 1;
      }
      prev_member->sdes_info[i] = 
	(char*) malloc(strlen(new_member->sdes_info[i]) + 1);
      strcpy(prev_member->sdes_info[i], new_member->sdes_info[i]);

      if (the_context->ChangedMemberInfoCallBack != NULL && announce) {
	the_context->
	  ChangedMemberInfoCallBack(the_context->context_num,
				    prev_member->unique_id,
				    i, prev_member->sdes_info[i],
				    RTP_FLAG_MEMBER_INFO_CHANGES);
      }
    }
  }
  if (new_member->rtp_timestamp > prev_member->rtp_timestamp){
    prev_member->rtp_timestamp = new_member->rtp_timestamp;
  }

  /* Note: treating an NTP timestamp as a struct timeval is a kludge,
     but it will work here. */
  
  if (TimeExpired((struct timeval *) &prev_member->ntp_timestamp,
		  (struct timeval *) &new_member->ntp_timestamp,
		  &notime)) {
    prev_member->ntp_timestamp = new_member->ntp_timestamp;
  }

  /* XXX if the old member was a sender (has receiver_reports) and the new
     one isn't, the new member should become a putative sender. */
  
  if (new_member->local_info != NULL) {
    if (prev_member->local_info == NULL) {
      prev_member->local_info = new_member->local_info;
      new_member->local_info = NULL;
    } else {
      MergeLocalInfo(prev_member->local_info, new_member->local_info);
    }
  }
  /* If new_member->local_info was NULL, we can just leave things as they 
     were. */

  /* If both members were senders, merge their receiver_report hashes.
     XXX: we need to do the right thing in the putative sender case. */
  if (prev_member->sendstatus != RTP_SENDER_NOT &&
      new_member->sendstatus != RTP_SENDER_NOT) {
    Merge_RR_Hashes(prev_member, new_member);
  }

  /* A member on the CSRC list and another member with the same SSRC
     could only be detected when they have different CNAMES.  This
     is because we could not use the address or the fact that one
     existed before the other.  It may be the CSRC is also a member
     of the group.  When resolution occurs for this situation, the
     prev_member will be marked as on_csrc_list but not only_on_csrc_list.
     We don't know which member this resolves to.  Thus, we check
     to see if the SSRC appears on the csrc list.  If so, we know
     that we are on the csrc list.  Otherwise, we know we are not.
     As to whether or not we are only from the csrc list, for now we
     assume not.  If we are wrong, the situation will be rectified
     when the member times out. */

  prev_member->on_csrc_list = FALSE;
  cont = the_context->contriblist;
  while(cont != NULL) {
    if (prev_member->ssrc == cont->ssrc){
      prev_member->on_csrc_list = TRUE;
      break;
    }
    cont = cont->next;
  }

 /* Swap ssrc_hash pointers so that prev_member gets hashed based
     on new ssrc, and previous pointer links will be destroyed when
     the new_member is destroyed */
  temp_link = prev_member->ssrc_hash;
  prev_member->ssrc_hash = new_member->ssrc_hash;
  if (prev_member->ssrc_hash == NULL){
    /* The new member never was hashed to the new SSRC, so then
       create the new hash */
    EnterSSRCHashTable(the_context, prev_member);
  }
  else prev_member->ssrc_hash->my_member = prev_member;
  new_member->ssrc_hash = temp_link;
  if (new_member->ssrc_hash == NULL){
    /* The prev member never was hashed to the new SSRC, so then
       create the new hash */
    EnterSSRCHashTable(the_context, new_member);
  }
  new_member->ssrc_hash->my_member = new_member;
  
  
  /* Update Colliding lists */
  if (new_member->collides_with_prev != NULL){
    new_member->collides_with_prev->collides_with_next = 
      new_member->collides_with_next;
  }

  if (new_member->collides_with_next != NULL){
    new_member->collides_with_next->collides_with_prev = 
      new_member->collides_with_prev;
  }
  
  if (GetMemberFromSSRCHash(the_context, new_member->ssrc) ==
      new_member){
    /* new_member was the member with the SSRC in question that was
       placed in the SSRC hash.  Pull it out, and put
       the new member in front of the collides list in the hash table. */

    /* NOTE: if conditional eval'd to TRUE, then new_member should
       have been at the front of the collide list, so
       new_member->collides_with_prev should be NULL */
    RemoveMemberFromSSRCHash(the_context, new_member);
    if (new_member->collides_with_next != NULL){
      EnterSSRCHashTable(the_context, new_member->collides_with_next);
    }
  }

  /* If we wanted to be extra careful, we'd traverse the whole sender list
     and merge together all the receiver_reports stored on sender lists.
     But there isn't really any need to be this careful; the new member's
     reports will get deleted by DestroyMember, and the previous member's
     reports will get updated soon enough as it sends new ones. */

  /* Destroy new_member */
  DestroyMember(the_context, new_member);

  /* Put prev_member in CNAME hash */
  EnterCNAMEHashTable(the_context, prev_member);

  return prev_member;
}
