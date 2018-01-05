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
static uint8 g_UseForLocal = FALSE;				// 如果是本地录制使用 设置为1

#if _AUDIO_FILE_SAVE_
#define PCM_AUDIO				0
#define ALAW_AUDIO				1
#define PCM_AUDIO_FILE			"/mnt/nand1-2/enc_rtp.pcm"
#define ALAW_AUDIO_FILE			"/mnt/nand1-2/enc_rtp.alaw"
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
  Function:		Audio_Sf_PackStream
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int Audio_Sf_PackStream(MSMediaDesc * f, uint8 * pstStream)
{
	uint8 send = 0;
	uint32 nlen = 0, extlen = 4;
	mblk_t arg;

	// add by chenbh 本地录制时 需要把前面四字节数据去除 
	if (TRUE == g_UseForLocal)
	{
		extlen = 0;
	}
	
	AudioSfEncState * s = (AudioSfEncState*)f->private;
	memcpy(&s->Alaw[extlen + s->AlawIndex*SAMPLE_AUDIO_PTNUMPERFRM], pstStream, SAMPLE_AUDIO_PTNUMPERFRM);
	if (s->EncParam.IsPack == HI_FALSE)
	{
		send = 1;
		nlen = SAMPLE_AUDIO_PTNUMPERFRM+extlen;
	}
	else
	{
		s->AlawIndex++;
		if (s->AlawIndex == s->EncParam.PackNum)
		{
			send = 1;
			nlen = s->EncParam.PackNum*SAMPLE_AUDIO_PTNUMPERFRM+extlen;
		}
	}
	
	if (send == 1)
	{
		ms_media_blk_init(&arg);
		arg.len = nlen;
		arg.address = (char*)s->Alaw;
		
		#if _AUDIO_FILE_SAVE_
		if (s->EncParam.IsPack == HI_TRUE)
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
			
			Audio_Sf_PackStream(f, data);
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
	AudioSfEncState * s = (AudioSfEncState*)f->private;	
	int ret = -1;
	
	ms_media_lock(f);
	#if _AUDIO_FILE_SAVE_
	g711_file_open(ALAW_AUDIO);
	#endif
	
	log_printf("f->mcount : %d \n", f->mcount);
	if (f->mcount == 0)
	{
		g_UseForLocal = FALSE;
		s->AlawIndex = 0;
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
	AudioSfEncState * data = (AudioSfEncState*)f->private;
	AUDIO_PARAM param = * ((AUDIO_PARAM*)arg);

	ms_media_lock(f);
	if (f->mcount == 0)
	{	
		memcpy(&data->EncParam, &param, sizeof(AUDIO_PARAM));
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
	ms_media_lock(f);
	if (f->mcount > 0)
	{	
		g_UseForLocal = *(uint8 *)arg;
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
		AudioSfEncState * data = (AudioSfEncState*)malloc(sizeof(AudioSfEncState));
		memset(data, 0, sizeof(AudioSfEncState));
		data->PayloadType = PT_G711A;
		data->Samplerate = AUDIO_SAMPLE_RATE_8000;
   		data->Bitwidth = AUDIO_BIT_WIDTH_16;       	// standard 16bit little endian format, support this format only
		data->channelnum = 1;
		data->EncParam.IsPack = HI_TRUE;			// 多包发送
		data->EncParam.PackNum = AUDIO_NUM;			// 一次发送的包数
		data->EncParam.MicValue = 95;
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

MSMediaDesc ms_audio_sf_enc_desc =
{
	.id = MS_ALAW_SF_ENC_ID,
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

