/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_dec.c
  Author:		罗发禄
  Version:		V2.0
  Date: 		2014-04-01
  Description:	音频解码程序
  
History:				 
	1.Date: 
	Author: 
	Modification: 
*********************************************************/
#include <sys/soundcard.h>

#include "audio_endec.h"
#include "logic_audio_dec.h"
#include "logic_audio.h"
#include "logic_media.h"
#include "logic_media_core.h"

#define _AUDIO_DEC_FILE_SAVE_	0

#if _AUDIO_DEC_FILE_SAVE_
#define PCM_DEC_AUDIO			0
#define ALAW_DEC_AUDIO			1
#define PCM_DEC_AUDIO_FILE		"/mnt/nand1-2/dec.pcm"
#define ALAW_DEC_AUDIO_FILE		"/mnt/nand1-2/dec.alaw"
static FILE * mDPCMFIle = NULL;
static FILE * mDALAWFIle = NULL;

/*************************************************
  Function:		g711_dec_file_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void g711_dec_file_close(uint8 mode)
{
	switch (mode)
	{
		case PCM_DEC_AUDIO:
			if (mDPCMFIle)
			{
				fclose(mDPCMFIle);
				mDPCMFIle = NULL;
			}
			break;
			
		case ALAW_DEC_AUDIO:
			if (mDALAWFIle)
			{
				fclose(mDALAWFIle);
				mDALAWFIle = NULL;
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
static void g711_dec_file_open(uint8 mode)
{
	g711_dec_file_close(mode);
	switch (mode)
	{
		case PCM_DEC_AUDIO:
			mDPCMFIle = fopen(PCM_DEC_AUDIO_FILE, "wb");
			break;
			
		case ALAW_DEC_AUDIO:
			mDALAWFIle = fopen(ALAW_DEC_AUDIO_FILE, "wb");
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
static void g711_dec_file_save(uint8 mode, uint8 *data, int len)
{
	switch (mode)
	{
		case PCM_DEC_AUDIO:
			if (mDPCMFIle != NULL)
			{
				fwrite(data, len, 1, mDPCMFIle);
				fflush(mDPCMFIle);
			}
			break;
			
		case ALAW_DEC_AUDIO:
			if (mDALAWFIle != NULL)
			{
				fwrite(data, len, 1, mDALAWFIle);
				fflush(mDALAWFIle);
			}
			break;
			
		default:
			break;
	}
}
#endif

/*************************************************
  Function:		ms_audio_dec_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		AudioDecState * data = (AudioDecState*)malloc(sizeof(AudioDecState));
		
		memset(data, 0, sizeof(AudioDecState));
		data->enPayloadType= PT_G711A;
		data->Samplerate = AUDIO_SAMPLE_RATE_8000;
   		data->Bitwidth = AUDIO_BIT_WIDTH_16;       	/*standard 16bit little endian format, support this format only*/
		data->channelnum = 1;
		data->AParam.IsPack = HI_TRUE;
		data->AParam.PackNum = AUDIO_NUM;
		data->AParam.SpkValue = 95;
		f->private= data;
		f->mcount = 0;
	}

	return 0;
}

/*************************************************
  Function:		ms_audio_dec_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}

/*************************************************
  Function:		ms_audio_dec_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_open(struct _MSMediaDesc * f, void * arg)
{	
	AudioDecState * data = (AudioDecState*)f->private;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{
		#if _AUDIO_DEC_FILE_SAVE_
	    g711_dec_file_open(PCM_DEC_AUDIO);
	    g711_dec_file_open(ALAW_DEC_AUDIO);
		#endif	
		data->AoEnable = HI_TRUE;
		data->AlawIndex = 0;
		f->mcount++;
		if (f->preprocess)
		{
			f->preprocess(f);
		} 
	}
	else
	{
		f->mcount++;
	}
	ms_media_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		send_audio_dec
  Description:	发送音频数据到解码器
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void ms_audio_dec_proc(struct _MSMediaDesc * f, mblk_t * arg)
{	
	int i = 0;
	int packnum;
	uint8 datapcm[322];
	
    AudioDecState * s = (AudioDecState*)f->private;
	int byte = s->AParam.byte_per_packet/2;
	//printf("byte==%d\n", byte);
	memset(datapcm, 0, sizeof(datapcm));
	if (s->AoEnable == HI_TRUE)
	{
		packnum = arg->len / byte;
		for (i = 0; i < packnum; i++)
		{
			G711Decoder((short *)datapcm, (unsigned char*)(arg->address + byte*i), byte, 0);
			//alaw_to_pcm(byte, (uint8 *)(arg->address + byte*i), datapcm);
			#if _AUDIO_DEC_FILE_SAVE_
			g711_dec_file_save(PCM_DEC_AUDIO, (uint8 *)datapcm, byte*2);
			g711_dec_file_save(ALAW_DEC_AUDIO, (uint8 *)(arg->address + byte*i), byte);
			#endif

			mblk_t arg1;
			ms_media_blk_init(&arg1);
			arg1.address =(char *)datapcm;
			arg1.len = byte * 2;	
			arg1.id = MS_FILTER_NOT_SET_ID;
			ms_media_process(f, &arg1);
		}
	}
}

/*************************************************
  Function:		ms_audio_enc_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_close(struct _MSMediaDesc * f, void * arg)
{
	uint32 ret = HI_FAILURE;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			#if _AUDIO_DEC_FILE_SAVE_
    		g711_dec_file_close(PCM_DEC_AUDIO);
    		g711_dec_file_close(ALAW_DEC_AUDIO);
			#endif
			if (f->postprocess)
			{
				f->postprocess(f);
			} 
			ret = HI_SUCCESS;
		}
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_audio_dec_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_param(struct _MSMediaDesc * f, void * arg)
{
	AudioDecState * data = (AudioDecState*)f->private;
	AUDIO_PARAM* param = (AUDIO_PARAM*)arg;

	ms_media_lock(f);
	if (f->mcount == 0)
	{		
	   memcpy(&data->AParam, param, sizeof(AUDIO_PARAM));
	   ms_media_unlock(f);	
	   return HI_SUCCESS;
	}
	ms_media_unlock(f);	
	
	return HI_FAILURE;
}

/*************************************************
  Function:		ms_audio_dec_enable
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_dec_enable(struct _MSMediaDesc * f, void * arg)
{
	AudioDecState * data = (AudioDecState*)f->private;
	HI_BOOL param = *((HI_BOOL*)arg);

	ms_media_lock(f);
	if (f->mcount > 0)
	{		
	   data->AoEnable = param;
	   ms_media_unlock(f);	
	   return HI_SUCCESS;
	}
	ms_media_unlock(f);
	
	return HI_FAILURE;
}

static MSMediaMethod methods[] =
{
	{MS_AUDIO_DEC_OPEN,		ms_audio_dec_open},
	{MS_AUDIO_DEC_CLOSE,	ms_audio_dec_close},
	{MS_AUDIO_DEC_PARAM,	ms_audio_dec_param},
	{MS_AUDIO_DEC_ENABLE,	ms_audio_dec_enable},
	{0,						NULL}
};

MSMediaDesc ms_audio_dec_desc =
{
	.id = MS_ALAW_DEC_ID,
	.name = "MsAlawDec",
	.text = "Encoder using Hi3520 Hw",
	.enc_fmt = "ALAW",
	.ninputs = 1,
	.noutputs = 2,
	.outputs = NULL,
	.init = ms_audio_dec_init,
	.uninit = ms_audio_dec_uninit,
	.preprocess = NULL,
	.process = ms_audio_dec_proc,
	.postprocess = NULL,	
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_UNBLOCK, 
	.sharebufsize = 1000,		// 1024*sharebufsize
	.sharebufblk = 200,
	.private = NULL,    		// 存储数据
};

