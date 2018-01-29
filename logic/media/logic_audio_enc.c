/*********************************************************
  Copyright (C), 2013-2015
  File name:	logic_audio_sfenc.c
  Author:   	
  Version:   	1.0
  Date: 		13.8.2
  Description:  
  History:            
*********************************************************/
#include <sys/soundcard.h>

#include "audio_endec.h"
#include "logic_audio_enc.h"
#include "logic_audio.h"


#define _AUDIO_FILE_SAVE_		0
#if _AUDIO_FILE_SAVE_
#define PCM_AUDIO				0
#define ALAW_AUDIO				1
#define PCM_AUDIO_FILE			CFG_PUBLIC_DRIVE"/enc_rtp.pcm"
#define ALAW_AUDIO_FILE			CFG_PUBLIC_DRIVE"/enc_rtp.alaw"
static FILE * mPCMFIle = NULL;
static FILE * mALAWFIle = NULL;

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
		case PCM_AUDIO:
			if (mPCMFIle)
			{
				fclose(mPCMFIle);
				mPCMFIle = NULL;
			}
			break;
			
		case ALAW_AUDIO:
			if (mALAWFIle)
			{
				fclose(mALAWFIle);
				mALAWFIle = NULL;
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
		case PCM_AUDIO:
			mPCMFIle = fopen(PCM_AUDIO_FILE, "wb");
			break;
			
		case ALAW_AUDIO:
			mALAWFIle = fopen(ALAW_AUDIO_FILE, "wb");
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
		case PCM_AUDIO:
			if (mPCMFIle != NULL)
			{
				fwrite(data, len, 1, mPCMFIle);
				fflush(mPCMFIle);
			}
			break;
			
		case ALAW_AUDIO:
			if (mALAWFIle != NULL)
			{
				fwrite(data, len, 1, mALAWFIle);
				fflush(mALAWFIle);
			}
			break;
			
		default:
			break;
	}
}
#endif

/*************************************************
  Function:		st_audio_param_reset
  Description:	
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static int st_audio_param_reset(AudioEncParam * data)
{	
	ms_return_val_if_fail(data, -1);	
	memset(data, 0, sizeof(AudioEncParam));
	data->isPack = FALSE;
	data->isLocalRecord = FALSE;
	return 0;
}

/*************************************************
  Function:		st_audio_enc_packet
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int st_audio_enc_packet(MSMediaDesc * f, uint8 * pstStream)
{
	uint8 send = 0;
	uint32 nlen = 0, extlen = 4;
	mblk_t arg;

	AudioEncParam * data = (AudioEncParam*)f->private;
	// add by chenbh 本地录制时 需要把前面四字节数据去除 
	if (TRUE == data->isLocalRecord)
	{
		extlen = 0;
	}
	
	AudioEncParam * s = (AudioEncParam*)f->private;
	memcpy(&s->Alaw[extlen + s->AlawIndex*SAMPLE_AUDIO_PTNUMPERFRM], pstStream, SAMPLE_AUDIO_PTNUMPERFRM);
	if (s->isPack == FALSE)
	{
		send = 1;
		nlen = SAMPLE_AUDIO_PTNUMPERFRM+extlen;
	}
	else
	{
		s->AlawIndex++;
		if (s->AlawIndex == AUDIO_PACKET_NUM)
		{
			send = 1;
			nlen = AUDIO_PACKET_NUM*SAMPLE_AUDIO_PTNUMPERFRM+extlen;
		}
	}
	
	if (send == 1)
	{
		ms_media_blk_init(&arg);
		arg.len = nlen;
		arg.address = (char*)s->Alaw;
		
		#if _AUDIO_FILE_SAVE_
		if (s->isPack == TRUE)
		{
			g711_file_save(ALAW_AUDIO, &(s->Alaw[extlen]), SAMPLE_AUDIO_PTNUMPERFRM * 6);
		}
		else
			g711_file_save(ALAW_AUDIO, &(s->Alaw[extlen]), SAMPLE_AUDIO_PTNUMPERFRM);
		#endif
		
		arg.id = MS_FILTER_NOT_SET_ID;
		s->AlawIndex = 0;
		ms_media_process(f, &arg);	
	}
	
    return 0;
}

/*************************************************
  Function:			ms_audio_enc_proc
  Description:		
  Input:
  Output:			无
  Return:			无
  Others:
*************************************************/
static void ms_audio_enc_proc(struct _MSMediaDesc * f, mblk_t * arg)
{
	int i, pack;
	uint8 data[500];
	
	if (arg)
	{
		pack = arg->len / (2*SAMPLE_AUDIO_PTNUMPERFRM);
		for (i = 0; i < pack; i++)
		{
			G711Encoder((short *)(arg->address + i*(2*SAMPLE_AUDIO_PTNUMPERFRM)), (uint8 *)data, SAMPLE_AUDIO_PTNUMPERFRM, 0);
			
			st_audio_enc_packet(f, data);
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
static int ms_audio_enc_open(struct _MSMediaDesc * f, void * arg)
{
	AudioEncParam * s = (AudioEncParam*)f->private;	
	int ret = -1;
	
	ms_media_lock(f);
	#if _AUDIO_FILE_SAVE_
	g711_file_open(ALAW_AUDIO);
	#endif
	
	log_printf("f->mcount : %d \n", f->mcount);
	if (f->mcount == 0)
	{
		s->isLocalRecord = FALSE;
		if (f->preprocess)
		{
			f->preprocess(f);
		}
		ret = 0;
		f->mcount++;
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
static int ms_audio_enc_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = -1;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			if (f->postprocess)
			{
				f->postprocess(f);
			}
			ret = 0;
			AudioEncParam * data = (AudioEncParam*)f->private;
			st_audio_param_reset(data);
		} 
	}
	#if _AUDIO_FILE_SAVE_
	g711_file_close(ALAW_AUDIO);
	#endif
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_audio_sf_enc_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_enc_param(struct _MSMediaDesc * f, void * arg)
{
	AudioEncParam * data = (AudioEncParam*)f->private;
	AUDIO_PARAM *param = (AUDIO_PARAM*)arg;

	ms_media_lock(f);
	if (f->mcount == 0)
	{	
		data->isPack = param->isPack;
		ms_media_unlock(f);	
		return 0;
	}
	ms_media_unlock(f);
	
	return -1;
}

/*************************************************
  Function:		ms_audio_enc_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_enc_local(struct _MSMediaDesc * f, void * arg)
{
	AudioEncParam * data = (AudioEncParam*)f->private;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{	
		data->isLocalRecord = TRUE;
		ms_media_unlock(f);	
		return 0;
	}
	ms_media_unlock(f);
	
	return -1;
}

/*************************************************
  Function:		ms_audio_enc_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_enc_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		AudioEncParam * data = (AudioEncParam*)malloc(sizeof(AudioEncParam));
		st_audio_param_reset(data);
		
		f->private= data;		
		f->mcount = 0;
	}
	
	return 0;
}

/*************************************************
  Function:		ms_audio_enc_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_enc_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}
static MSMediaMethod methods[] =
{
	{MS_AUDIO_SF_ENC_OPEN,		ms_audio_enc_open},
	{MS_AUDIO_SF_ENC_CLOSE,		ms_audio_enc_close},
	{MS_AUDIO_SF_ENC_PARAM, 	ms_audio_enc_param},
	{MS_AUDIO_SF_ENC_LOCAL, 	ms_audio_enc_local},
	{0,							NULL}
};

MSMediaDesc ms_audio_enc_desc =
{
	.id = MS_ALAW_ENC_ID,
	.name = "MsAlawEncSw",
	.text = "Encoder using Hi3520 Sf",
	.enc_fmt = "ALAW",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_audio_enc_init,
	.uninit = ms_audio_enc_uninit,
	.preprocess = NULL,
	.process = ms_audio_enc_proc,
	.postprocess = NULL,		
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};

