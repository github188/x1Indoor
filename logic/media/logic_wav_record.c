/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_media_record.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include "audio_dev.h"
#include "audio_endec.h"
#include "wav_record.h"
#include "logic_wav_record.h"

#define WAV_AUDIO_FORMAT		eWAVE_FORMAT_PCM//eWAVE_FORMAT_G711_ALAW
#define DEF_DISK_SPACE_REQ		1024*1024		// 预留1M空间

#define	BLOCKTIME         		20//40     //每个缓冲数据块的时长（毫秒）
#define AI_DEV_CHANNELS			1
#define AI_DEV_BITWIDTH			AIO_BIT_WIDTH_16
#define AI_DEV_SAMPLERATE		AIO_SAMPLE_RATE_8

typedef struct _WavRecordParam{
	char filename[100];
	bool record_start;
	S_WavFileWriteInfo sWavFileInfo;
}WavRecordParam;

static int ms_wav_record_callbak(char *DataBuffer, int DataSize)
{
	WavRecordParam * data = (WavRecordParam*)(ms_wav_record_desc.private);

	if (data->record_start)
	{
		if (eWAVE_FORMAT_PCM == WAV_AUDIO_FORMAT)
		{
			WavFileUtil_Write_WriteData(&(data->sWavFileInfo), DataBuffer, DataSize);
		}
		else if (eWAVE_FORMAT_G711_ALAW == WAV_AUDIO_FORMAT)
		{
			unsigned char alaw[320];			
			G711Encoder((short *)DataBuffer, alaw, DataSize/2, 0);
			WavFileUtil_Write_WriteData(&(data->sWavFileInfo), alaw, DataSize/2);
		}
	}
}

/*************************************************
  Function:		ms_wav_record_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * ms_wav_record_thread(void *ptr)
{	
	MSMediaDesc *f = (MSMediaDesc *)ptr;
	WavRecordParam * data = (WavRecordParam*)(f->private);
	
	if (data->filename == NULL)
	{		
		goto err;
	}

	char pchFilePath[256] = {0};	
	memset(pchFilePath, 0, sizeof(pchFilePath));
	sprintf(pchFilePath, "%s.wav", data->filename); 

	if(WavFileUtil_Write_Initialize(&(data->sWavFileInfo), pchFilePath) == FALSE){
		log_printf("Create WAV file failed!\n");
		goto err;
	}

	
	uint16	u16BitsPerSample = (WAV_AUDIO_FORMAT == eWAVE_FORMAT_PCM) ? 16 : 8;
	if(WavFileUtil_Write_SetFormat(&(data->sWavFileInfo), WAV_AUDIO_FORMAT, 1, AIO_SAMPLE_RATE_8, u16BitsPerSample) == FALSE)
	{
		WavFileUtil_Write_Finish(&(data->sWavFileInfo));
		log_printf("Set WAV file format failed!\n");
		goto err;
	}
	
	data->record_start = TRUE;
	while(f->thread.thread_run)
	{
		usleep(100*1000);	
	}
	data->record_start = FALSE;
	
	f->thread.thread = -1;	
	WavFileUtil_Write_Finish(&(data->sWavFileInfo));
	return;
	
err:
	log_printf(" return err !!!\n");

}

/*************************************************
  Function:		ms_wav_record_start
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_start(struct _MSMediaDesc * arg)
{
	uint64 freeDisk = FSCheckSpareSpace("nand");
	log_printf("freeDisk[%ld]\n", freeDisk);
	if (freeDisk >= DEF_DISK_SPACE_REQ)
	{
		return Alsa_Record_Start(AI_DEV_CHANNELS, AI_DEV_BITWIDTH, AI_DEV_SAMPLERATE, BLOCKTIME, ms_wav_record_callbak);
	}
	else
		return HI_FAILURE;
}

/*************************************************
  Function:		ms_wav_record_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_open(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = HI_FAILURE;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{			
		ret = ms_wav_record_start(f);
		if(HI_SUCCESS != ret)
		{
			goto End_NSRecord;
		}
		else
		{
			f->mcount++;
			ms_thread_init(&f->thread,20);	
			ret = ms_thread_create(&f->thread, ms_wav_record_thread, f);
			if(HI_SUCCESS != ret)
			{
				Alsa_Record_Close();
				goto End_NSRecord;
			}
		}
	}
	ms_media_unlock(f);
	return ret;
	
End_NSRecord:
	log_printf(" ********** END ERROR :%d ********* \n ", ret);
	ms_media_unlock(f);
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_wav_record_close
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_close(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = RT_FAILURE;
	log_printf(" f->mcount : %d \n", f->mcount);

	ms_media_lock(f);
	if( f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if(f->mcount == 0)
		{
			Alsa_Record_Close();
			ms_thread_quit(&f->thread);
			ret = RT_SUCCESS;
		} 
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_wav_record_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_param(struct _MSMediaDesc * f, void * arg)
{
	WavRecordParam *s = (WavRecordParam *)(f->private);
	char *filename = (char *)arg;
	if (arg == NULL)
	{
		return RT_FAILURE;
	}
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		memset(s, 0, sizeof(WavRecordParam));
		strcpy(s->filename, filename);
		log_printf("s->filename[%s]\n", s->filename);
		ms_media_unlock(f);	
		return RT_SUCCESS;
	}
	ms_media_unlock(f);	
	
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_wav_record_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		WavRecordParam *data = (WavRecordParam *)malloc(sizeof(WavRecordParam));		
		memset(data, 0, sizeof(WavRecordParam));
		data->record_start = FALSE;
		
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_wav_record_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_wav_record_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return RT_SUCCESS;
	}
	
	return RT_FAILURE;
}

static MSMediaMethod methods[] =
{
	{MS_WAV_RECORD_PARAM,	ms_wav_record_param},
	{MS_WAV_RECORD_OPEN,	ms_wav_record_open},
	{MS_WAV_RECORD_CLOSE,	ms_wav_record_close},
	{0, NULL}
};

MSMediaDesc ms_wav_record_desc =
{
	.id = MS_WAV_RECORD_ID,
	.name = "MsWavRecord",
	.text = "Wav Record",
	.enc_fmt = "wav",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_wav_record_init,
	.uninit = ms_wav_record_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 2000,							// 1024*sharebufsize
	.sharebufblk = 500,
	.private = NULL, 
};	

