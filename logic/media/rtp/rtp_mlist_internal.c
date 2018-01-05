/* rtp_mlist_internal.c : types and functionality used by functions 
                          to support the RTP library (but mainly more directly
		          to those pertaining to membership lists).

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#include "sysdep.h"

#include "rtp_mlist_internal.h"

#include "tclHash.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


#ifdef _RTP_WATCH_ALLOCATION 
#define MAXMALLOCS 10000
long mallctr;
long mallcalls;
long freecalls;

void* malladdr[MAXMALLOCS];
void InitMallocs(){
  int i;
  for (i=0;i<MAXMALLOCS;i++){
    malladdr[i] = NULL;
    mallctr = 0;
    mallcalls = 0;
    freecalls = 0;
  }
}

void CheckMallocs(){
  int i;
  int missed = 0;
  for (i=0;i<MAXMALLOCS;i++){
    if (malladdr[i] != NULL){
      printf("Unfreed memory: alloc %d memory loc %p\n", i, malladdr[i]);
      missed++;
    }
  }
  printf("Malloc reports %d missed frees\n", missed);
}

void* my_malloc(size_t size){
  malladdr[mallcalls] = malloc(size);
  mallctr += size;
  printf("call %ld: mallocing %d at %p: tot of %ld\n", mallcalls, size, 
	 malladdr[mallcalls], mallctr);
  mallcalls++;
  return malladdr[mallcalls-1];
}

void* my_calloc(size_t num, size_t size){
  mallctr += size * num;
  malladdr[mallcalls] = calloc(num, size);
  printf("call %ld: callocing %d of %d at %p: tot of %ld\n", mallcalls, num,
	 size, malladdr[mallcalls], mallctr);
  mallcalls++;
  return  malladdr[mallcalls-1];
}

void my_free(void* tofree){
  int i;
  for (i=0; i < MAXMALLOCS; i++){
    if (tofree==malladdr[i]){
      printf("Free: freeing allocation %d at %p\n", i, tofree);
      free(tofree);
      malladdr[i] = NULL;
      return;
    }
  }
  printf("Free: Address %p does not show up in malloc table\n",
	 tofree);
  return;
}

#endif
  

void InitList(doubly_linked_list *l){
  l->first_link = NULL;
  l->last_link = NULL;
  l->list_size = 0;
  return;
}

static void InitHashTable(hash_table *h){
  int i;
  for (i=0; i<HASHSIZE;i++){
    InitList(h->hashval+i);
  }
  return;
}
    

int PullMember(doubly_linked_list *l, struct link *the_link){
  struct link *prev, *next;

  if (the_link->in_list != l){
    return FALSE;
  }
  prev = the_link->prev_link;
  next = the_link->next_link;

  /* Break this member's connections to the list */
  the_link->prev_link = NULL;
  the_link->next_link = NULL;

  if (prev != NULL){
    prev->next_link = next;
  }

  if (next != NULL){
    next->prev_link = prev;
  }

  l->list_size--;

  /* If member was the first (last) member of list l, then change
     the first (last) member to the next (prev) member */
  if (l->first_link ==  the_link){
    l->first_link = next;
  }
  if (l->last_link == the_link){
    l->last_link = prev;
  }
  the_link->in_list = NULL;
  return TRUE;
}

int UnsortedInsertMember(doubly_linked_list *l,
			  struct link *the_link,
			  struct link *prev_on_list){
  /* We do not perform the insert if the the_link is already part
     of some list, and prev_on_list must be a part list l.  */

  if (the_link->in_list != NULL){
    return FALSE;
  }
  if (prev_on_list != NULL){
    if (prev_on_list->in_list != l){
      return FALSE;
    }
  }

  if (prev_on_list == NULL){ /* should be first member on list */
    if (l->list_size > 0){
      the_link->next_link = l->first_link;
      l->first_link->prev_link = the_link;
    }
    else {
      the_link->next_link = NULL;
      l->last_link = the_link;
    }
    the_link->prev_link = NULL;
    l->first_link = the_link;
  }

  else { /* prev_on_list is an actual member */

    /* Set the connection between the_link and prev_on_list */
    the_link->next_link = prev_on_list->next_link;
    prev_on_list->next_link = the_link;
    the_link->prev_link = prev_on_list;

    /* set the connection between the_link and the_link->next_link */
    if (the_link->next_link == NULL){ /* last on list */
      l->last_link = the_link;
    }
    else {
      the_link->next_link->prev_link = the_link;
    }
  }

  l->list_size++;
  the_link->in_list = l;
  return TRUE;
}
    

int SortedInsertMember(_RTP_CONTEXT *the_context, doubly_linked_list *l, 
		  struct link *the_link,
		  int (*sort_func)(_RTP_CONTEXT *the_context,
				   struct link* compare_me,
				   struct link* to_me),
		  struct link *start_point){
  int try_down = TRUE;
  int try_up = TRUE;

  if (the_link->in_list != NULL){
    return FALSE;
  }
  if (start_point != NULL){
    if (start_point->in_list != l){
      return FALSE;
    }
  }


  if (l->list_size == 0){
    return UnsortedInsertMember(l, the_link, NULL);
  }

  /* If the user doesn't supply a starting point for the search,
     by default we start at the back of the list */
  if (start_point == NULL){
    start_point = l->last_link;
  }

  /* If we get here, then the list size is not 0, so start_point must
     be non-NULL */

  /* First we walk up the list */
  while (try_down && start_point != NULL){
    if ((*sort_func)(the_context, the_link, start_point) < 0){
      start_point = start_point->prev_link;
      /* Since we need to move down, we never need to try moving up */
      try_up = FALSE;
    }
    else try_down = FALSE;
  }

  /* Now walk down the list.  (Note that we only do this if
   we never moved while walking up the list, so start_point
   must be non-NULL) */

  while (try_up){
    if (start_point->next_link == NULL){
      /* We have reached the end of the list.  start_point is
	 therefore our predecessor */
      try_up = FALSE;
    }
    else if ((*sort_func)(the_context, the_link, start_point->next_link) > 0){
      start_point = start_point->next_link;
    }
    else try_up = FALSE;
  }

  return UnsortedInsertMember(l, the_link, start_point);
}


struct link *CreateLink(member *the_member){
  struct link *new_link;

#ifdef _RTP_DEBUG
  printf("Build linkfor member id %ld, SSRC %lx\n", the_member->unique_id,
	 the_member->ssrc);
#endif

  new_link = malloc(sizeof(struct link));
  new_link->my_member = the_member;
  new_link->next_link = NULL;
  new_link->prev_link = NULL;
  new_link->in_list = NULL;
  return new_link;
}


/***********************************
  MEMBERSHIP LIST FUNCS 
  **********************************/

/* Initializes the membership list structure.  Since only
   one such list exists per context, this should only
   be called when the context is created */

void MembershipListInit(membership_list *l){
  InitList(&l->actual_list);
  l->oldest_not_yet_soft = NULL;
  l->confirmed_members = 0;
}





/* MembershipListPullMember should be the function called
   to remove a member from the list. */

int MembershipListPullMember(membership_list *l, struct link
			     *the_link){ 

  /* First, we must check whether or not this member getting
     pulled off the list is being pointed to by the oldest_not_yet_soft
     pointer, and if it is, move that pointer to point to the 
     previous member on the list, who is the next youngest. */

  if (l->oldest_not_yet_soft == the_link){
    l->oldest_not_yet_soft = the_link->prev_link;
  }

  /* If this member is a confirmed member, decrement the 
     confirmed count by 1 */

  if (the_link->my_member->status == RTP_MEMBER_CONFIRMED){
    l->confirmed_members--;
  }

  /* And now, actually pull the member off the list */
  return PullMember(&l->actual_list, the_link);
}

int SenderListPullMember(membership_list *l, struct link
			     *the_link){ 

  /* First, we must check whether or not this member getting
     pulled off the list is being pointed to by the oldest_not_yet_soft
     pointer, and if it is, move that pointer to point to the 
     previous member on the list, who is the next youngest. */

  if (l->oldest_not_yet_soft == the_link){
    l->oldest_not_yet_soft = the_link->prev_link;
  }

  /* If this member is a confirmed member, decrement the 
     confirmed count by 1 */

  if (the_link->my_member->sendstatus == RTP_SENDER_CONFIRMED){
    l->confirmed_members--;
  }

  /* And now, actually pull the member off the list */
  return PullMember(&l->actual_list, the_link);
}

  
    



/* Add a member to the membership list.  The member will
   always be added to the front of the list.  This is
   sufficient because members are only added to the
   list when they have recently been updated */

int MembershipListInsertMember(membership_list *l,
			       struct link *the_link){

  /* First, try and insert the member into the wrapped list.  If the
     insert fails because the member is already on the list,
     forward the failing result without continuing.  This
     would indicate some sort of bug occurring. */

  if (UnsortedInsertMember(&l->actual_list, the_link, NULL) == FALSE){
    return FALSE;
  }

  /* If the inserted member is confirmed, update the confirmed
     counter */

  if (the_link->my_member->status == RTP_MEMBER_CONFIRMED){
    l->confirmed_members++;
  }

  /* If everyone else has soft timed out, then oldest_not_yet_soft
     will point to NULL.  Instead, it should point to the
     newly joined member */

  if (l->oldest_not_yet_soft == NULL){
    l->oldest_not_yet_soft = the_link;
  }

  return TRUE;
}

/* Add a member to the sender list.  The member will
   always be added to the front of the list.  This is
   sufficient because members are only added to the
   list when they have recently been updated */

int SenderListInsertMember(membership_list *l,
			       struct link *the_link){

  /* First, try and insert the member into the wrapped list.  If the
     insert fails because the member is already on the list,
     forward the failing result without continuing.  This
     would indicate some sort of bug occurring. */

  if (UnsortedInsertMember(&l->actual_list, the_link, NULL) == FALSE){
    return FALSE;
  }

  /* If the inserted member is confirmed, update the confirmed
     counter */

  if (the_link->my_member->sendstatus == RTP_SENDER_CONFIRMED){
    l->confirmed_members++;
  }

  /* If everyone else has soft timed out, then oldest_not_yet_soft
     will point to NULL.  Instead, it should point to the
     newly joined member */

  if (l->oldest_not_yet_soft == NULL){
    l->oldest_not_yet_soft = the_link;
  }

  return TRUE;
}


void ChangeSenderStatus(_RTP_CONTEXT *the_context,
			member *m, senderstatus new_status) {

  int callbackf = RTP_FLAG_NONE;


  if(new_status == RTP_SENDER_CONFIRMED) {

    if(m->sendstatus == RTP_SENDER_NOT) {
      callbackf = RTP_FLAG_NEW_SENDER;
    } else if(m->sendstatus == RTP_SENDER_PURPORTED) {
      callbackf = RTP_FLAG_NEW_SENDER;
    }
  }
  else if(new_status == RTP_SENDER_PURPORTED) {

    if(m->sendstatus == RTP_SENDER_NOT) {
      callbackf = RTP_FLAG_PURPORTED_SENDER;
    } else if(m->sendstatus == RTP_SENDER_CONFIRMED) {
      callbackf = RTP_FLAG_EXPIRED_SENDER;
    }

  } else if(new_status == RTP_SENDER_NOT) {

    if(m->sendstatus == RTP_SENDER_CONFIRMED) {
      callbackf = RTP_FLAG_DELETED_SENDER;
    } else if(m->sendstatus == RTP_SENDER_PURPORTED) {
      callbackf = RTP_FLAG_DELETED_SENDER;
    }

  }

  m->sendstatus = new_status;
  if(callbackf != RTP_FLAG_NONE) {
    if(the_context->UpdateMemberCallBack != NULL){
      the_context->UpdateMemberCallBack(the_context->context_num,
					m->unique_id, 
					callbackf,
					NULL);
    }
  }


  
}

/* Member status should always be changed with this function
   because it will also grab the appropriate memberlist
   and update the confirmed member count. */

void ChangeMemberStatus(_RTP_CONTEXT *the_context, 
		       member *m, memberstatus new_status){
  membership_list *the_list = &the_context->RTP_MemberList;


  /* Update number of confirmed members */

  if((new_status == RTP_MEMBER_CONFIRMED) &&
     (m->status != RTP_MEMBER_CONFIRMED)) 
    the_list->confirmed_members++;

  if((new_status != RTP_MEMBER_CONFIRMED) &&
     (m->status == RTP_MEMBER_CONFIRMED)) 
    the_list->confirmed_members--;

  /* The rest are just for callbacks */

  if (m->mmbr_list_link != NULL){
      
    if((new_status == RTP_MEMBER_CONFIRMED) ||
       (new_status == RTP_MEMBER_CONFIRMED_CONTRIBUTOR)) {

      if ((m->status == RTP_MEMBER_PENDING) ||
	  (m->status == RTP_MEMBER_PENDING_CONTRIBUTOR)) {

	m->status = new_status;
	if(the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    m->unique_id, 
					    RTP_FLAG_MEMBER_CONFIRMED,
					    NULL);
	}
      }
      else if ((m->status == RTP_MEMBER_EXPIRED) ||
	       (m->status == RTP_MEMBER_EXPIRED_CONTRIBUTOR)) {

	m->status = new_status;
	if(the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    m->unique_id, 
					    RTP_FLAG_MEMBER_ALIVE,
					    NULL);
	}
      }

    } else if((new_status == RTP_MEMBER_EXPIRED) ||
	      (new_status == RTP_MEMBER_EXPIRED_CONTRIBUTOR)) {

      if ((m->status == RTP_MEMBER_CONFIRMED) ||
	  (m->status == RTP_MEMBER_CONFIRMED_CONTRIBUTOR)) {
	m->status = new_status;
	if(the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    m->unique_id, 
					    RTP_FLAG_EXPIRED_MEMBER,
					    NULL);
	}
      }
    /* This shouldn't happen, but just in case... */
      else if ((m->status == RTP_MEMBER_PENDING) ||
	       (m->status == RTP_MEMBER_PENDING_CONTRIBUTOR)) {
	m->status = new_status;
	if(the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    m->unique_id, 
					    RTP_FLAG_EXPIRED_MEMBER,
					    NULL);
	}
      }

    } else if((new_status == RTP_MEMBER_PENDING) ||
	      (new_status == RTP_MEMBER_PENDING_CONTRIBUTOR)) {
      
      /* Same with this */
      if ((m->status == RTP_MEMBER_CONFIRMED) ||
	  (m->status == RTP_MEMBER_CONFIRMED_CONTRIBUTOR)) {
	m->status = new_status;

	/* No callback */
      }
      else if ((m->status == RTP_MEMBER_EXPIRED) ||
	       (m->status == RTP_MEMBER_EXPIRED_CONTRIBUTOR)) {
	m->status = new_status;
	if(the_context->UpdateMemberCallBack != NULL){
	  the_context->UpdateMemberCallBack(the_context->context_num,
					    m->unique_id, 
					    RTP_FLAG_MEMBER_ALIVE,
					    NULL);
	}
      }

    }
  }

  m->status = new_status;
  return;
}


  

  


/**********************************
 HASH TABLE FUNCS
 **********************************/

extern void CleanHash(_RTP_CONTEXT *the_context, hash_table *the_hash);

void InitSSRCHashTable(_RTP_CONTEXT *the_context)
{
  InitHashTable(&the_context->RTP_SSRC_Hash);
}


void DeleteSSRCHashTable(_RTP_CONTEXT *the_context)
{
  CleanHash(the_context, &the_context->RTP_SSRC_Hash);
}


int EnterSSRCHashTable(_RTP_CONTEXT *the_context, member *the_member){
  int into_list;
  hash_table *the_table = &the_context->RTP_SSRC_Hash;
  if (the_member->ssrc_hash != NULL){

#ifdef _RTP_DEBUG
    printf("Hey! I've been hashed\n");
#endif

    return FALSE;  /* Already been hashed */
  }
  into_list = (int) (the_member->ssrc % HASHSIZE);

  the_member->ssrc_hash = CreateLink(the_member);
  if (!UnsortedInsertMember(the_table->hashval+into_list, 
		       the_member->ssrc_hash,
			   NULL)){
    return FALSE;
  }
  return TRUE;
}
		       
		       


member *GetMemberFromSSRCHash(_RTP_CONTEXT *the_context, u_int32 ssrc){
  int into_list = (int) (ssrc % HASHSIZE);
  hash_table *the_table = &the_context->RTP_SSRC_Hash;
  struct link *cur_link = the_table->hashval[into_list].first_link;
  while (cur_link != NULL){
    if (ssrc == cur_link->my_member->ssrc){
      return cur_link->my_member;
    }
    cur_link = cur_link->next_link;
  }
  return NULL; /* Not in hash table */
}
      
  

int RemoveMemberFromSSRCHash(ARG_UNUSED(_RTP_CONTEXT*, the_context),
			     member *the_member){
  struct link *link_to_destroy = the_member->ssrc_hash;
  if (!PullMember(link_to_destroy->in_list, link_to_destroy)){
    return FALSE;
  }
  the_member->ssrc_hash = NULL;
  free(link_to_destroy);
  return TRUE;
} 


/* Unique ID Hash Table funcs */

void InitUniqueIDHashTable(_RTP_CONTEXT *the_context)
{
  InitHashTable(&the_context->RTP_ID_Hash);
}


void DeleteUniqueIDHashTable(_RTP_CONTEXT *the_context)
{
  CleanHash(the_context, &the_context->RTP_ID_Hash);
}


int EnterUniqueIDHashTable(_RTP_CONTEXT *the_context, member *the_member){
  int into_list;
  hash_table *the_table = &the_context->RTP_ID_Hash;
  if (the_member->unique_id_hash != NULL){
    return FALSE;  /* Already been hashed */
  }
  into_list = (int) (the_member->unique_id % HASHSIZE);
  the_member->unique_id_hash = CreateLink(the_member);
  UnsortedInsertMember(the_table->hashval+into_list, 
		       the_member->unique_id_hash,
		       NULL);
  return TRUE;
}
		       
		       


member *GetMemberFromUniqueIDHash(_RTP_CONTEXT *the_context, person id){
  int into_list;
  hash_table *the_table = &the_context->RTP_ID_Hash;
  struct link *cur_link;
  RemapPerson(the_context, &id);
  into_list = (int) (id % HASHSIZE);
  cur_link = the_table->hashval[into_list].first_link;
  /* First we remap the person.  This should be fairly lightweight
     because there should be few remappings. */
  while (cur_link != NULL){
    if (id == cur_link->my_member->unique_id){
      return cur_link->my_member;
    }
    cur_link = cur_link->next_link;
  }
  return NULL; /* Not in hash table */
}
      
  

int RemoveMemberFromUniqueIDHash(ARG_UNUSED(_RTP_CONTEXT *,the_context),
				 member *the_member){
  struct link *link_to_destroy = the_member->unique_id_hash;
  if (!PullMember(link_to_destroy->in_list, link_to_destroy)){
    return FALSE;
  }
  the_member->unique_id_hash = NULL;
  free(link_to_destroy);
  return TRUE;
} 


/* CNAME Hash Table funcs */

void InitCNAMEHashTable(_RTP_CONTEXT *the_context)
{
  InitHashTable(&the_context->RTP_CNAME_Hash);
}


void DeleteCNAMEHashTable(_RTP_CONTEXT *the_context)
{
  CleanHash(the_context, &the_context->RTP_CNAME_Hash);
}


static int HashOnName(char *the_name){
  unsigned long into_list = 0;
  unsigned int i;
  for (i=0; i < strlen(the_name); i++){
    into_list *= 33;
    into_list += the_name[i];
  }
  return (0x3FF & into_list);
}

int EnterCNAMEHashTable(_RTP_CONTEXT *the_context, member *the_member){
  hash_table *the_table = &the_context->RTP_CNAME_Hash;
  char *cname = the_member->sdes_info[0];
  int into_list;
  if (the_member->cname_hash != NULL){
    return FALSE;  /* Already been hashed */
  }
  if (cname==NULL){
    /* NOTE: Should we detect when a CNAME is NULL.  This probably
       indicates a problem (or is it just that the RTCP packet
       for the member has not yet arrived.  Note that currently, it
       is possible for the local member to not set its CNAME in the
       session. */
    into_list = 0;
  }
  else into_list = HashOnName(cname);
  the_member->cname_hash = CreateLink(the_member);
  UnsortedInsertMember(the_table->hashval+into_list, 
		       the_member->cname_hash,
		       NULL);
  return TRUE;
}
		       


member *GetMemberFromCNAMEHash(_RTP_CONTEXT *the_context, char *cname){
  int into_list = HashOnName(cname);
  hash_table *the_table = &the_context->RTP_CNAME_Hash;
  struct link *cur_link;
  cur_link = the_table->hashval[into_list].first_link;
  while (cur_link != NULL){
    if (cur_link->my_member->sdes_info[0] != NULL){
      if (strcmp(cname, cur_link->my_member->sdes_info[0])==0){
	return cur_link->my_member;
      }
    }
    cur_link = cur_link->next_link;
  }
  return NULL; /* Not in hash table */
}
      
  

int RemoveMemberFromCNAMEHash(ARG_UNUSED(_RTP_CONTEXT*, the_context),
			      member *the_member){
  struct link *link_to_destroy = the_member->cname_hash;
  if (!PullMember(link_to_destroy->in_list, link_to_destroy)){
    return FALSE;
  }
  the_member->cname_hash = NULL;
  free(link_to_destroy);
  return TRUE;
} 

/*
<<<<<<< rtp_mlist_internal.c

sr_info *FindSR_InfoWithSSRC(member *the_member, u_int32 ssrc){
  sr_info *cur_info = the_member->sr_itemlist;
  while (cur_info != NULL){
    if (cur_info->ssrc == ssrc){
      return cur_info;
    }
    cur_info = cur_info->nextinf;
  }
  return NULL; 
}

sr_info *CreateSR_InfoShell(member *the_member, u_int32 ssrc){
  sr_info *the_info = malloc(sizeof(sr_info));
  

  the_info->nextinf = the_member->sr_itemlist;
  the_member->sr_itemlist = the_info;
  the_info->previnf = NULL;
  if (the_info->nextinf != NULL){
    the_info->nextinf->previnf = the_info;
  }
  
  the_member->itemlistsize++;
  the_info->ssrc = ssrc;
  the_info->local_info = NULL;
  return the_info;
}
*/

/*********************
 * RR functions *
 *********************/

/* Initialize the RR hash for a sender. */
void Init_RR_Hash(member *the_member)
{
  Tcl_HashTable *the_table = &the_member->RR_Hash;

  Tcl_InitHashTable(the_table, TCL_ONE_WORD_KEYS);
}

/* Destroy the RR hash for a sender, and all its associated values. */
void Delete_RR_Hash(member *the_member)
{
  Tcl_HashTable *the_table = &the_member->RR_Hash;
  Tcl_HashEntry *the_entry;
  Tcl_HashSearch the_search;
  
  the_entry = Tcl_FirstHashEntry(the_table, &the_search);
  while (the_entry != NULL) {
    free(Tcl_GetHashValue(the_entry));
    the_entry = Tcl_NextHashEntry(&the_search);
  }

  Tcl_DeleteHashTable(the_table);
}


/* Update the receiver_report the member 'reporter' reported about the member
   'sender'.  If this is the first block reported since this member
   became a sender, create the appropriate receiver_report first. */
receiver_report *Update_RR(member *reporter, member *sender,
			rtcp_report_block *the_block)
{
  Tcl_HashEntry *the_entry;
  receiver_report *the_info;
  int created;
  Tcl_HashTable *the_table = &sender->RR_Hash;

  if (the_block->ssrc != sender->ssrc) {
#ifdef _RTP_DEBUG
    fprintf(stderr, "Update_RR: block ssrc != sender ssrc\n");
#endif
    return NULL;
  }

  if (sender->sendstatus == RTP_SENDER_NOT) {
    /* Reported sender is not a sender by our standards. */
    return NULL;
  }

  the_entry = Tcl_CreateHashEntry(the_table, (char *) reporter->unique_id,
				  &created);
  if (created) {
    the_info = (receiver_report *) malloc(sizeof(receiver_report));
    the_info->reporter = reporter->unique_id;
    Tcl_SetHashValue(the_entry, (ClientData) the_info);
  } else {
    the_info = (receiver_report *) Tcl_GetHashValue(the_entry);
  }

  /* Update all the parameters */
  the_info->fraction_lost = the_block->frac_lost;
  the_info->cum_lost = the_block->cum_lost;
  the_info->highest_seqno = the_block->highest_seqno;
  the_info->jitter = the_block->jitter;
  the_info->last_sr = the_block->lsr;
  the_info->delay_last_sr = the_block->dlsr;

  return the_info;
}


/* Delete any receiver_reports this member has reported from the hash tables for
   every sender. */
void Clear_Member_RRs(_RTP_CONTEXT *the_context, member *the_member)
{
  struct link *cur_link;
  Tcl_HashTable *the_table;
  Tcl_HashEntry *the_entry;

  cur_link = the_context->RTP_SenderList.actual_list.first_link;

  while (cur_link != NULL) {
    the_table = &cur_link->my_member->RR_Hash;
    the_entry = Tcl_FindHashEntry(the_table, (char *) the_member->unique_id);
    if (the_entry != NULL) {
      free(Tcl_GetHashValue(the_entry));
      Tcl_DeleteHashEntry(the_entry);
    }
    cur_link = cur_link->next_link;
  }
}

static void Merge_RRs(receiver_report *prev_sr, receiver_report *new_sr){

  prev_sr->fraction_lost = new_sr->fraction_lost;
  prev_sr->cum_lost += new_sr->cum_lost;
  if (prev_sr->highest_seqno < new_sr->highest_seqno){
    prev_sr->highest_seqno = new_sr->highest_seqno;
  }
  if (prev_sr->last_sr < new_sr->last_sr){
    prev_sr->last_sr = new_sr->last_sr;
  }
  return;
}

void Merge_RR_Hashes(member* prev_member, member *new_member)
{
  Tcl_HashTable *prev_table = &prev_member->RR_Hash;
  Tcl_HashTable *new_table = &new_member->RR_Hash;
  Tcl_HashEntry *prev_entry, *new_entry;
  Tcl_HashSearch the_search;
  receiver_report *prev_info, *new_info;
  int created;

  new_entry = Tcl_FirstHashEntry(new_table, &the_search);
  while (new_entry != NULL) {
    prev_entry = Tcl_CreateHashEntry(prev_table,
				     Tcl_GetHashKey(new_table, new_entry),
				     &created);
    if (created) {
      prev_info = (receiver_report *) malloc(sizeof(receiver_report));
      Tcl_SetHashValue(prev_entry, (ClientData) prev_info);
      new_info = (receiver_report *) Tcl_GetHashValue(new_entry);
      memcpy(prev_info, new_info, sizeof(receiver_report));
    } else {
      prev_info = (receiver_report *) Tcl_GetHashValue(prev_entry);
      new_info = (receiver_report *) Tcl_GetHashValue(new_entry);
      Merge_RRs(prev_info, new_info);
    }

    new_entry = Tcl_NextHashEntry(&the_search);
  }

  return;
}



  
  

/* A function tclHash.c uses when something's wrong.  It's got a big
   "shouldn't happen" around it. */

void panic(char *format, ...)
{
  va_list args;

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  abort();
}

