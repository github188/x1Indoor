/* rtp_collision.h : functionality used to resolve SSRC collisions
                     in an RTP session.

Copyright 1997, 1998 Lucent Technologies; all rights reserved
*/

#ifndef _RTP_COLLISION
#define _RTP_COLLISION
#include "rtp_api_internal.h"
#include "rtp_mlist_internal.h"
#include "rtp_mlist.h"

void ComputeNewSSRC(_RTP_CONTEXT *the_context);

member *HandleSSRCCollision(_RTP_CONTEXT *the_context, member *the_member,
			    struct sockaddr *fromaddr,
			    int addr_is_rtcp,
			    char *new_cname, int is_contrib);

member *MergeMembers(_RTP_CONTEXT *the_context, member *prev_member,
		     member *new_member);

#endif
