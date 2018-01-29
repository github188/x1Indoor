/*********************************************************
  Copyright (C), 2013-2015
  File name:	logic_cloud_itc.c
  Author:   	
  Version:   	1.0
  Date: 		13.8.2
  Description:  
  History:            
*********************************************************/
#include <sys/soundcard.h>

#include "logic_cloud_itc.h"
#include "logic_audio.h"
#include "audio_endec.h"

#ifdef _ENABLE_CLOUD_
#define BYTES_PER_PACKET_SHORT 		160


#define _AUDIO_FILE_SAVE_SEND_		0
#define _AUDIO_FILE_SAVE_RECV_		0
static char g_RecvAudioBuf[500];

#if (_AUDIO_FILE_SAVE_SEND_ || _AUDIO_FILE_SAVE_RECV_)
#define PCM_AUDIO_SEND				0
#define PCM_AUDIO_RECV				1

#define PCM_AUDIO_SEND_FILE			CFG_PUBLIC_DRIVE"/cloud_send_audio.pcm"
#define PCM_AUDIO_RECV_FILE			CFG_PUBLIC_DRIVE"/cloud_recv_audio.pcm"

static FILE * mPCMFIleSend = NULL;
static FILE * mPCMFIleRecv = NULL;

/*************************************************
  Function:		g711_file_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void g711_file_close(uint8 mode)
{
	switch (mode)
	{
		case PCM_AUDIO_SEND:
			if (mPCMFIleSend)
			{
				fclose(mPCMFIleSend);
				mPCMFIleSend = NULL;
			}
			break;

		case PCM_AUDIO_RECV:
			if (mPCMFIleRecv)
			{
				fclose(mPCMFIleRecv);
				mPCMFIleRecv = NULL;
			}
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		g711_file_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void g711_file_open(uint8 mode)
{
	g711_file_close(mode);
	switch (mode)
	{
		case PCM_AUDIO_SEND:
			mPCMFIleSend = fopen(PCM_AUDIO_SEND_FILE, "wb");
			break;


		case PCM_AUDIO_RECV:
			mPCMFIleRecv = fopen(PCM_AUDIO_RECV_FILE, "wb");
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		g711_file_save
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void g711_file_save(uint8 mode, uint8 *data, int len)
{
	switch (mode)
	{
		case PCM_AUDIO_SEND:
			if (mPCMFIleSend != NULL)
			{
				fwrite(data, len, 1, mPCMFIleSend);
				fflush(mPCMFIleSend);
			}
			break;

		case PCM_AUDIO_RECV:
			if (mPCMFIleRecv != NULL)
			{
				fwrite(data, len, 1, mPCMFIleRecv);
				fflush(mPCMFIleRecv);
			}
			break;
			
		default:
			break;
	}
}
#endif

/*************************************************
  Function:		ms_sf_enc_audio_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_reset(struct _MSMediaDesc * f)
{
	CloudItcState * s = (CloudItcState*)f->private;	
	s->PayloadType = PT_G711A;
	s->Sequence = 0;
	s->Enable = HI_FALSE;
	s->func = NULL;
		
	return 0;
}

/*************************************************
  Function:		ms_sf_enc_audio_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		CloudItcState * data = (CloudItcState*)malloc(sizeof(CloudItcState));
		memset(data, 0, sizeof(CloudItcState));
		data->PayloadType = PT_G711A;
		data->Sequence = 0;
		data->Enable = FALSE;
		data->func = NULL;
		
		f->private= data;		
		f->mcount = 0;
	}
	
	return 0;
}

/*************************************************
  Function:		ms_sf_enc_audio_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}

/*************************************************
  Function:			ms_sf_enc_audio_proc
  Description:		
  Input:
  Output:			无
  Return:			无
  Others:
*************************************************/
static void ms_cloud_send_proc(struct _MSMediaDesc * f, mblk_t * arg)
{
	CloudItcState * s = (CloudItcState*)f->private;	
	
	if (arg && s->Enable == HI_TRUE)
	{
		if (s->func)
		{
			s->Sequence++;
			s->func(arg->address, arg->len, arg->marker, s->Sequence);
			#if _AUDIO_FILE_SAVE_SEND_
			if (f->id == MS_CLOUD_SEND_AUDIO_ID)
			{
				g711_file_save(PCM_AUDIO_SEND, arg->address, arg->len);
			}
			#endif
		}
			
	}
}

/*************************************************
  Function:		ms_audio_sf_enc_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_open(struct _MSMediaDesc * f, void * arg)
{
	CloudItcState * s = (CloudItcState*)f->private;	
	int ret = -1;
	
	ms_media_lock(f);
	log_printf("f->mcount : %d \n", f->mcount);
	if (f->mcount == 0)
	{
		if (f->preprocess)
		{
			f->preprocess(f);
		}
		ret = 0;
		s->Sequence = 0;
		f->mcount++;
		#if _AUDIO_FILE_SAVE_SEND_
		if (f->id == MS_CLOUD_SEND_AUDIO_ID)
		{
			g711_file_open(PCM_AUDIO_SEND);
		}
		#endif
	}
	else
	{
		f->mcount++;
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_audio_sf_enc_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			ms_cloud_send_reset(f);
			if (f->postprocess)
			{
				f->postprocess(f);
			}
			ret = 0;
			#if _AUDIO_FILE_SAVE_SEND_
			if (f->id == MS_CLOUD_SEND_AUDIO_ID)
			{
				g711_file_close(PCM_AUDIO_SEND);
			}
			#endif
		} 
	}

	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_cloud_send_func
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_func(struct _MSMediaDesc * f, void * arg)
{
	CloudItcState * data = (CloudItcState*)f->private;

	ms_media_lock(f);		
	//if (data->func == NULL)
	{
		data->func = (MsCloudDataFunc)arg;
	}
	ms_media_unlock(f);	
	return HI_SUCCESS;
}

/*************************************************
  Function:		ms_audio_sf_enc_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_Payload(struct _MSMediaDesc * f, void * arg)
{
	CloudItcState * data = (CloudItcState*)f->private;

	ms_media_lock(f);
	if (f->mcount == 0)
	{	
		data->PayloadType = *(uint8 *)arg;
		log_printf("data->PayloadType : %d\n", data->PayloadType);
		ms_media_unlock(f);	
		return 0;
	}
	ms_media_unlock(f);	
	return -1;
}

/*************************************************
  Function:		ms_audio_dec_enable
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_send_enable(struct _MSMediaDesc * f, void * arg)
{
	CloudItcState * data = (CloudItcState*)f->private;
	HI_BOOL param = *((HI_BOOL*)arg);

	ms_media_lock(f);
		
	data->Enable = param;
	log_printf("data->Enable : %d\n", data->Enable);

	ms_media_unlock(f);

	return HI_SUCCESS;
}


static MSMediaMethod send_audio_methods[] =
{
	{MS_CLOUD_SEND_AUDIO_OPEN,		ms_cloud_send_open},
	{MS_CLOUD_SEND_AUDIO_CLOSE,		ms_cloud_send_close},
	{MS_CLOUD_SEND_AUDIO_PAYLOAD, 	ms_cloud_send_Payload},
	{MS_CLOUD_SEND_AUDIO_FUNC, 		ms_cloud_send_func},
	{MS_CLOUD_SEND_AUDIO_ENABLE,	ms_cloud_send_enable},
	{0,							NULL}
};

MSMediaDesc ms_cloud_send_audio_desc =
{
	.id = MS_CLOUD_SEND_AUDIO_ID,
	.name = "MsCloudAudioSend",
	.text = "Cloud Send",
	.enc_fmt = "PCM",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_cloud_send_init,
	.uninit = ms_cloud_send_uninit,
	.preprocess = NULL,
	.process = ms_cloud_send_proc,
	.postprocess = NULL,		
	.methods = send_audio_methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};

static MSMediaMethod send_video_methods[] =
{
	{MS_CLOUD_SEND_VIDEO_OPEN,		ms_cloud_send_open},
	{MS_CLOUD_SEND_VIDEO_CLOSE,		ms_cloud_send_close},
	{MS_CLOUD_SEND_VIDEO_PAYLOAD, 	ms_cloud_send_Payload},
	{MS_CLOUD_SEND_VIDEO_FUNC, 		ms_cloud_send_func},
	{MS_CLOUD_SEND_VIDEO_ENABLE,	ms_cloud_send_enable},
	{0,							NULL}
};

MSMediaDesc ms_cloud_send_video_desc =
{
	.id = MS_CLOUD_SEND_VIDEO_ID,
	.name = "MsCloudVideoSend",
	.text = "Cloud Send",
	.enc_fmt = "H264",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_cloud_send_init,
	.uninit = ms_cloud_send_uninit,
	.preprocess = NULL,
	.process = ms_cloud_send_proc,
	.postprocess = NULL,		
	.methods = send_video_methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};

/*************************************************
  Function:		ms_cloud_recv_reset
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_reset(struct _MSMediaDesc * f)
{
	CloudItcState * s = (CloudItcState*)f->private;	
	s->PayloadType = PT_G711A;
	s->Enable = HI_FALSE;
	s->func = NULL;
		
	return 0;
}

/*************************************************
  Function:		ms_cloud_recv_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		CloudItcState * data = (CloudItcState*)malloc(sizeof(CloudItcState));
		memset(data, 0, sizeof(CloudItcState));
		data->PayloadType = PT_G711A;
		data->Enable = FALSE;
		data->func = NULL;
		
		f->private= data;		
		f->mcount = 0;
	}
	
	return 0;
}

/*************************************************
  Function:		ms_cloud_recv_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}

/*************************************************
  Function:		ms_cloud_recv_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	
	ms_media_lock(f);
	log_printf("f->mcount : %d \n", f->mcount);
	if (f->mcount == 0)
	{
		if (f->preprocess)
		{
			f->preprocess(f);
		}
		ret = 0;
		f->mcount++;
		#if _AUDIO_FILE_SAVE_RECV_
		g711_file_open(PCM_AUDIO_RECV);
		#endif
	}
	else
	{
		f->mcount++;
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_cloud_recv_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			ms_cloud_recv_reset(f);
			if (f->postprocess)
			{
				f->postprocess(f);
			}
			ret = 0;
			#if _AUDIO_FILE_SAVE_RECV_
			g711_file_close(PCM_AUDIO_RECV);
			#endif
		} 
	}

	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_cloud_recv_enable
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_enable(struct _MSMediaDesc * f, void * arg)
{
	CloudItcState * data = (CloudItcState*)f->private;
	HI_BOOL param = *((HI_BOOL*)arg);

	ms_media_lock(f);
		
	data->Enable = param;
	log_printf("data->Enable : %d\n", data->Enable);

	ms_media_unlock(f);

	return HI_SUCCESS;
}

/*************************************************
  Function:		ms_cloud_recv_enable
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_cloud_recv_data(struct _MSMediaDesc * f, void * arg)
{
	ms_media_lock(f);
	CloudItcState * data = (CloudItcState*)f->private;
	
	//获取音频数据长度
	if (data->Enable == HI_TRUE)
	{
		int size = 0, i = 0;
		memcpy(&size, arg, 4);

		#if _AUDIO_FILE_SAVE_RECV_
		g711_file_save(PCM_AUDIO_RECV, (uint8 *)arg+4, size);
		#endif
			
		int packnum = size / BYTES_PER_PACKET_SHORT;
		for (i = 0; i < packnum; i++)
		{	
			
			mblk_t arg1;
			ms_media_blk_init(&arg1);
			memset(g_RecvAudioBuf, 0, sizeof(g_RecvAudioBuf));
			memcpy(g_RecvAudioBuf, (uint8 *)arg+4+i*BYTES_PER_PACKET_SHORT, BYTES_PER_PACKET_SHORT);
			arg1.address =(char *)g_RecvAudioBuf;
			arg1.len = BYTES_PER_PACKET_SHORT;	
			arg1.id = MS_FILTER_NOT_SET_ID;
			ms_media_process(f, &arg1);
		}
	}
	ms_media_unlock(f);
	return HI_SUCCESS;
}

static MSMediaMethod recv_methods[] =
{
	{MS_CLOUD_RECV_AUDIO_OPEN,		ms_cloud_recv_open},
	{MS_CLOUD_RECV_AUDIO_CLOSE,		ms_cloud_recv_close},
	{MS_CLOUD_RECV_AUDIO_ENABLE,	ms_cloud_recv_enable},
	{MS_CLOUD_RECV_AUDIO_DATA,		ms_cloud_recv_data},
	{0,							NULL}
};

MSMediaDesc ms_cloud_recv_audio_desc =
{
	.id = MS_CLOUD_RECV_AUDIO_ID,
	.name = "MsCloudRecv",
	.text = "Cloud Recv",
	.enc_fmt = "PCM",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_cloud_recv_init,
	.uninit = ms_cloud_recv_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,		
	.methods = recv_methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};
#endif

