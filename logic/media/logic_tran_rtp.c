/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	au_tran_rtp.c
  Author:		罗发禄
  Version:		V2.0
  Date: 		2014-03-28
  Description:	RTP应用封装程序
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#include "logic_video.h"
#include "logic_tran_rtp.h"

typedef struct 
{
	RtpSession * Session;
	unsigned char IsSend;
}RTPSendState;

typedef struct 
{
	RtpSession * Session;
	unsigned char IsRecv;
}RTPRecvState;

/*************************************************/
/*				    RTP SEND					 */
/*************************************************/
/*************************************************
  Function:		ms_rtp_send_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		RTPSendState * data = (RTPSendState*)ms_new(RTPSendState, 1);
		
		memset(data, 0, sizeof(RTPSendState));
		data->Session = NULL;
		data->IsSend = 1;
		f->private = data;
		f->mcount = 0;
	}

	return 0;
}

/*************************************************
  Function:		ms_rtp_send_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_uninit(struct _MSMediaDesc * f)
{  
	if (f->mcount == 0)
	{
		ms_free(f->private);
	}
	
	return 0;
}

/*************************************************
  Function:		ms_rtp_send_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_rtp_send_process(struct _MSMediaDesc * f, mblk_t * arg)
{
	if (NULL != arg)
	{
		RTPSendState * data = (RTPSendState*)f->private;
		
		// del by chenbh 解决多分机时 一台挂机此处被锁在ms_rtp_send_addr_dec 
		// 导致视频卡顿一下
		//ms_media_lock(f);
		rtp_session_send(data->Session, arg);
		//ms_media_unlock(f);
	}
}

/*************************************************
  Function:		ms_rtp_send_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_open(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = RT_FAILURE;
	RTPSendState * data = (RTPSendState*)f->private;
	RTP_OPEN_S * rtp = (RTP_OPEN_S*)arg;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{
		if (NULL == rtp)
		{
			goto error;
		}		

		data->Session = rtp_session_new(rtp->Port);
		rtp_session_set_payload_type(data->Session, rtp->Pt);
		rtp_session_set_recv_port(data->Session, rtp->Port);
		rtp_session_set_username(data->Session, rtp->UserName);
		ret = rtp_session_open(data->Session, RTP_SESSION_SEND);
		f->mcount = 1;

		if (f->preprocess)
		{
			f->preprocess(f);
		}
	}
	else
	{
		f->mcount++;
		ret = RT_SUCCESS;
	}
	
error:
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_rtp_send_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_close(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = -1;	
	RTPSendState * data = (RTPSendState*)f->private;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			ret = rtp_session_close(data->Session);
		}
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_rtp_send_addr_add
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_addr_add(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	RTPSendState * data = (RTPSendState*)f->private;
	RTP_ADDRESS_S * rtp = (RTP_ADDRESS_S*)arg;
	
	if (rtp == NULL)
	{
		return -1;
	}
	ms_media_lock(f);
	ret = rtp_session_sendaddr_add(data->Session, rtp->Ip, rtp->Port);
	ms_media_unlock(f);

	return ret;
}

/*************************************************
  Function:		ms_rtp_send_addr_dec
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_addr_dec(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	RTPSendState * data = (RTPSendState*)f->private;
	RTP_ADDRESS_S* rtp = (RTP_ADDRESS_S*)arg;
	
	if (rtp == NULL)
	{
		return -1;
	}
	ms_media_lock(f);
	ret = rtp_session_sendaddr_dec(data->Session, rtp->Ip, rtp->Port);
	ms_media_unlock(f);

	return ret;
}

/*************************************************
  Function:			ms_rtp_send_addr_count
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_send_addr_count(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	RTPSendState * data = (RTPSendState*)f->private;

	ms_media_lock(f);
	ret = rtp_get_addr_count(data->Session->RtpCid);
	ms_media_unlock(f);
	
	return ret;
}

static MSMediaMethod methods_v[] =
{ 
	{MS_RTP_SEND_V_OPEN,		ms_rtp_send_open},
	{MS_RTP_SEND_V_CLOSE, 		ms_rtp_send_close},
	{MS_RTP_ADDRESS_V_ADD, 		ms_rtp_send_addr_add},
	{MS_RTP_ADDRESS_V_DEC,	 	ms_rtp_send_addr_dec},
	{MS_RTP_ADDRESS_V_COUNT,	ms_rtp_send_addr_count},
	{0,							NULL}
};

MSMediaDesc ms_rtp_send_v_desc = 
{
	.id = MS_RTP_SEND_V_ID,
	.name = "MsRtpSendVideo",
	.text = "Rtp Core Api",
	.enc_fmt = "rtp",
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_rtp_send_init,
	.uninit = ms_rtp_send_uninit,
	.preprocess = NULL,
	.process = ms_rtp_send_process,
	.postprocess = NULL,	
	.methods = methods_v,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 3000,		// 1024*sharebufsize
	.sharebufblk = 500,
	.private = NULL, 	
};	

static MSMediaMethod methods_a[] =
{
	{MS_RTP_SEND_A_OPEN,		ms_rtp_send_open},
	{MS_RTP_SEND_A_CLOSE, 		ms_rtp_send_close},
	{MS_RTP_ADDRESS_A_ADD, 		ms_rtp_send_addr_add},
	{MS_RTP_ADDRESS_A_DEC, 		ms_rtp_send_addr_dec},
	{MS_RTP_ADDRESS_A_COUNT,	ms_rtp_send_addr_count},
	{0,							NULL}
};

MSMediaDesc ms_rtp_send_a_desc =
{
	.id = MS_RTP_SEND_A_ID,
	.name = "MsRtpSendAudio",
	.text = "Rtp Core Api",
	.enc_fmt = "rtp",
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_rtp_send_init,
	.uninit = ms_rtp_send_uninit,
	.preprocess = NULL,
	.process = ms_rtp_send_process,
	.postprocess = NULL,
	.methods = methods_a,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 	
};	

/*************************************************/
/*				    RTP RECV					 */
/*************************************************/
/*************************************************
  Function:		ms_rtp_recv_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		RTPRecvState * data = (RTPRecvState*)ms_new(RTPRecvState, 1);
		
		memset(data, 0, sizeof(RTPRecvState));
		data->Session = NULL;
		data->IsRecv = 1;
		f->private = data;
		f->mcount = 0;
	}

	return 0;
}

/*************************************************
  Function:		ms_rtp_recv_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
	}
	
	return 0;
}

/*************************************************
  Function:		ms_rtp_recv_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_open(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = RT_FAILURE;
	RTPRecvState * data = (RTPRecvState*)f->private;
	RTP_OPEN_S* rtp = (RTP_OPEN_S*)arg;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{
		if (NULL == rtp)
		{
			goto error;
		}		

		data->Session = rtp_session_new(rtp->Port);
		rtp_session_set_private(data->Session, (void *)f);
		rtp_session_set_payload_type(data->Session, rtp->Pt);
		rtp_session_set_recv_port(data->Session, rtp->Port);
		rtp_session_set_username(data->Session, rtp->UserName);
		ret = rtp_session_open(data->Session, RTP_SESSION_RECV);
		f->mcount = 1;

		if (f->preprocess)
		{
			f->preprocess(f);
		} 
		log_printf("Port[%d], pt[%d]\n", rtp->Port, rtp->Pt);
	}
	else
	{
		f->mcount++;
		ret = RT_SUCCESS;
	}
	
error:
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_rtp_recv_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_close(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = -1;	
	RTPRecvState * data = (RTPRecvState*)f->private;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			ret = rtp_session_close(data->Session);
			if (f->postprocess)
			{
				f->postprocess(f);
			} 
		}
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_rtp_recv_func
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_rtp_recv_func(struct _RtpSession *rtp, mblk_t * arg)
{
	if (rtp && arg)
	{
		 MSMediaDesc * f = rtp->private;		 
		 if (f && f->process)
		 {
		 	f->process(f, arg);
		 }
	}
}

/*************************************************
  Function:		ms_rtp_recv_addr_add
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_addr_add(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	RTPSendState * data = (RTPSendState*)f->private;
	RTP_ADDRESS_S * rtp = (RTP_ADDRESS_S*)arg;
	
	if (rtp == NULL)
	{
		return -1;
	}
	
	ms_media_lock(f);
	ret = rtp_session_recvaddr_add(data->Session, rtp->Ip, ms_rtp_recv_func);
	ms_media_unlock(f);

	log_printf("ip[%x], ret[%d]\n", rtp->Ip, ret);
	return ret;
}

/*************************************************
  Function:		ms_rtp_recv_addr_dec
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtp_recv_addr_dec(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	RTPSendState * data = (RTPSendState*)f->private;
	RTP_ADDRESS_S* rtp = (RTP_ADDRESS_S*)arg;
	if (rtp == NULL)
	{
		return -1;
	}
	
	ms_media_lock(f);
	ret = rtp_session_recvaddr_dec(data->Session);
	ms_media_unlock(f);

	return ret;
}

/*************************************************
  Function:		ms_rtp_recv_process
  Description: 	
  Input: 		
  Output:		  
  Return:		 
  Others:
*************************************************/
static void ms_rtp_recv_process(struct _MSMediaDesc * f, mblk_t * arg)
{
	if (NULL != arg)
	{
		ms_media_process(f, arg);
	}
}

static MSMediaMethod methods_ra[] =
{
	{MS_RTP_RECV_A_OPEN,		ms_rtp_recv_open},
	{MS_RTP_RECV_A_CLOSE, 		ms_rtp_recv_close},
	{MS_RTP_ADDR_A_RECV_ADD, 	ms_rtp_recv_addr_add},
	{MS_RTP_ADDR_A_RECV_DEC, 	ms_rtp_recv_addr_dec},	
	{0,							NULL}
};

MSMediaDesc ms_rtp_recv_a_desc = 
{
	.id = MS_RTP_RECV_A_ID,
	.name = "MsRtpRecvAudio",
	.text = "Rtp Core Api",
	.enc_fmt = "rtp",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_rtp_recv_init,
	.uninit = ms_rtp_recv_uninit,
	.preprocess = NULL, 
	.process = ms_rtp_recv_process,
	.postprocess = NULL,	
	.methods = methods_ra,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 	
};	

static MSMediaMethod methods_rv[] =
{
	{MS_RTP_RECV_V_OPEN,		ms_rtp_recv_open},
	{MS_RTP_RECV_V_CLOSE, 		ms_rtp_recv_close},
	{MS_RTP_ADDR_V_RECV_ADD, 	ms_rtp_recv_addr_add},
	{MS_RTP_ADDR_V_RECV_DEC, 	ms_rtp_recv_addr_dec},	
	{0,							NULL}
};

MSMediaDesc ms_rtp_recv_v_desc =
{
	.id = MS_RTP_RECV_V_ID,
	.name = "MsRtpRecvVideo",
	.text = "Rtp Core Api",
	.enc_fmt = "rtp",
	.noutputs = 2,
	.outputs = NULL,
	.init = ms_rtp_recv_init,
	.uninit = ms_rtp_recv_uninit,
	.preprocess = NULL,
	.process = ms_rtp_recv_process,
	.postprocess = NULL,	
	.methods = methods_rv,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 	
};

