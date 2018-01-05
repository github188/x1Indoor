/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_play_lyly_hit.c
  Author:		luofl
  Version:		V2.0
  Date:			2015-03-17
  Description:	播放留影留言提示音
  
History:				 
	1.Date: 
	Author: 
	Modification: 
*********************************************************/
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/uio.h>
#include <errno.h>
#include <sys/time.h>
#include <semaphore.h>

#include "wav_play.h"
#include "logic_media.h"
#include "logic_play_lyly_hit.h"
#include "logic_audio.h"
#include "logic_include.h"

#define CHUCK_SIZE				80

typedef struct LylyData{
	uint8 channels;						// 通道数	
	int rate;							// 采样率
	int bit_width;						// 采样位宽	
	AUDIO_HIT_PARAM hit_param;			
	MSThread msplaythread;
}LylyData;

typedef struct _read_buf
{
    int iput;       			// the position of put
    int iget;      	 			// the position of get
    int size;
    unsigned char * buffer;
}File_ReadBuf, *pFile_ReadBuf;

static File_ReadBuf *g_ReadBuf = NULL;
static short int g_Format_tag;	
static unsigned int g_ChunkSize = 480;
static unsigned int g_PerFrameNum = 80;

/*************************************************
  Function:			get_AudioParam
  Description:		获取音频参数
  Input: 	
  Output:			AudioParam
  Return:			无
  Others:
*************************************************/
static int get_AudioParam(LylyData *s)
{
	int ret = RT_FAILURE;	
	AUDIO_HIT_PARAM *hit_param = &(s->hit_param);

	log_printf("FileType : %x \n", hit_param->FileType);
	switch (hit_param->FileType)
	{
		case FILE_WAVE:
			{
				ret = wav_play_init(hit_param->filename);
				if (ret == 0)
				{
					g_ReadBuf = (File_ReadBuf *)wave_buf;
					g_Format_tag = wave_header.format_tag;					
					s->bit_width = wave_header.bits_per_sample;
					s->channels  = wave_header.channels;
					s->rate = wave_header.sample_rate;					
				}
			}
			break;

		case FILE_MP3:
			break;

		default:
			break;
	}

	log_printf("bit_width : %d, channels : %d, rate: %d\n", s->bit_width, s->channels, s->rate);
	if (ret == 0)
	{
		log_printf("g_ReadBuf->size : %d, g_ReadBuf->iget: %d\n", g_ReadBuf->size, g_ReadBuf->iget);
	}
	return ret;
}

/*************************************************
  Function:		ms_avi_play_stop
  Description: 	 
  Input: 	
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_lyly_stop(struct _MSMediaDesc * f)
{
	log_printf("****************** ms_audio_play_stop *******************\n");
	
	LylyData *data = (LylyData*)f->private;
	f->mcount = 0;
	switch (data->hit_param.FileType)
	{
		case FILE_WAVE:
			{
				wav_play_uninit();				
			}
			break;

		case FILE_MP3:
			break;

		default:
			break;
	}
	data->msplaythread.thread = -1;
	usleep(1000);
}

/*************************************************
  Function:		ms_lyly_PackStream_pre
  Description: 	
  Input: 		无
  Output:		无
  Return:		 
  Others:		
*************************************************/
static int ms_lyly_PackStream_pre(MSMediaDesc *f)
{
	int i;
	mblk_t arg;
	char buf[512];

	for (i=0; i<20; i++)
	{
		memset(buf, 0, sizeof(buf));
	
		ms_media_blk_init(&arg);
		arg.len = CHUCK_SIZE+4;
		arg.address = buf;
		arg.id = MS_FILTER_NOT_SET_ID;
		ms_media_process(f, &arg);
		usleep(1000);
	}
}

/*************************************************
  Function:		lyly_PackStream
  Description: 	
  Input: 		无
  Output:		无
  Return:		 
  Others:
*************************************************/
static int ms_lyly_PackStream(MSMediaDesc *f, char *pstStream, int len)
{
	mblk_t arg;
	char buf[512];
	log_printf("len : %d\n", len);

	// 门口机单包发送会失败 只有对方设备为梯口机 才发送单包
	if (g_PerFrameNum == 1) 
	{
		int i;
		int num = len/CHUCK_SIZE;
		
		for (i=0; i<num; i++)
		{
			memset(buf, 0, sizeof(buf));
			memcpy(buf+4, pstStream+i*CHUCK_SIZE, CHUCK_SIZE);
			ms_media_blk_init(&arg);
			arg.len = CHUCK_SIZE+4;
			arg.address = buf;
			arg.id = MS_FILTER_NOT_SET_ID;
			ms_media_process(f, &arg);
		}

		if (len%CHUCK_SIZE)
		{
			memset(buf, 0, sizeof(buf));
			memcpy(buf+4, pstStream+num*CHUCK_SIZE, (len-num*CHUCK_SIZE));
			ms_media_blk_init(&arg);
			arg.len = (len-num*CHUCK_SIZE)+4;
			arg.address = buf;
			arg.id = MS_FILTER_NOT_SET_ID;
			ms_media_process(f, &arg);
		}
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		memcpy(buf+4, pstStream, len);

		ms_media_blk_init(&arg);
		arg.len = len+4;
		arg.address = buf;
		arg.id = MS_FILTER_NOT_SET_ID;
		ms_media_process(f, &arg);
	}
	
    return HI_SUCCESS;
}

/*************************************************
  Function:		ms_lyly_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void *ms_lyly_thread(void *arg)
{
	ms_return_val_if_fail(arg, NULL);
	int state = 0;
	MSMediaDesc *f = (MSMediaDesc *)arg;
	LylyData *data = (LylyData*)f->private;	
	AUDIO_HIT_PARAM *hit_param = &(data->hit_param);
	log_printf("data->channels:%d, data->rate:%d, data->bit_width:%d, IsPack:%d, PackNum:%d\n", \
		data->channels, data->rate, data->bit_width, data->hit_param.IsPack, data->hit_param.PackNum);
	
	if (data->hit_param.IsPack)
	{
		g_ChunkSize = CHUCK_SIZE*(hit_param->PackNum);
	}
	else
	{
		g_ChunkSize = CHUCK_SIZE;
	}
	g_PerFrameNum = hit_param->PerFrameNum;
	
	unsigned char *alaw_buf = malloc(g_ChunkSize+10);
	unsigned char *ulaw_buf = malloc(g_ChunkSize+10);
	unsigned char *pcm_buf = malloc(g_ChunkSize*2+10);
	int audio_len = g_ChunkSize;
	int audio_chunk = g_ChunkSize;
	if (g_Format_tag == AUDIO_A_LAW || g_Format_tag == AUDIO_U_LAW)
	{
		audio_chunk = g_ChunkSize;
	}
	else
	{
		audio_chunk = g_ChunkSize*2;
	}


	// 计算延迟时间  ms
	int delay_time = 0;
	delay_time = (audio_len*1000)/(data->channels*data->rate*(data->bit_width/8));
	delay_time = (delay_time-10) ? (delay_time-10) : 0;		// 扣除程序执行需要时间
	log_printf("delay_time: %d\n", delay_time);
	
	if (1 == g_PerFrameNum && delay_time > 10)
	{
		delay_time -= 5;
	}
	
	// 先发送空数据 防止对方收到包后做丢包处理时将数据包丢弃造成声音播放不完整 暂时不用
	//ms_lyly_PackStream_pre(f);
	
	while (data->msplaythread.thread_run)
	{	
		usleep(delay_time*1000);
		
		if((g_ReadBuf->iget + audio_chunk) < g_ReadBuf->size)
		{
		    audio_len = audio_chunk;
		} 
		else 
		{
		    audio_len = g_ReadBuf->size - g_ReadBuf->iget;
		}

		if (g_Format_tag == AUDIO_A_LAW)
		{
			memset(alaw_buf, 0, g_ChunkSize);			
			memcpy(alaw_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);
			ms_lyly_PackStream(f, (char *)alaw_buf, audio_len);			
		}
		else if (g_Format_tag == AUDIO_U_LAW)
		{
			memset(ulaw_buf, 0, g_ChunkSize);
			memset(alaw_buf, 0, g_ChunkSize);
			memcpy(ulaw_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);
			ulaw_to_alaw(audio_len, alaw_buf, ulaw_buf);
			ms_lyly_PackStream(f, (char *)alaw_buf, audio_len);			
		}
		else
		{
			memset(pcm_buf, 0, g_ChunkSize);
			memset(alaw_buf, 0, g_ChunkSize);
			memcpy(pcm_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);
			G711Encoder((short *)pcm_buf, alaw_buf, audio_len/2, 0);
			ms_lyly_PackStream(f, (char *)alaw_buf, audio_len/2);						
		}
		
		g_ReadBuf->iget += audio_len;
		if(g_ReadBuf->iget >= g_ReadBuf->size)
		{		   
	       	ms_percent_callback(f, 1, 100);	
			break;
	   
		}         		
		
	}
	
error:
	if (alaw_buf)
	{
		free(alaw_buf);
	}
	if (ulaw_buf)
	{
		free(ulaw_buf);
	}
	if (pcm_buf)
	{
		free(pcm_buf);
	}
	printf(" ms_audio_play_thread error!!! \n");
	ms_lyly_stop(f);	
	data->msplaythread.thread = -1;
	pthread_detach(pthread_self());
    pthread_exit(NULL);
}

/*************************************************
  Function:		ms_lyly_start
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_lyly_param(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);
	
	LylyData *s = (LylyData*)f->private;	
	AUDIO_HIT_PARAM *AudioParam = (AUDIO_HIT_PARAM *)arg;
	if (AudioParam->filename == NULL ||
		AudioParam->FileType != FILE_WAVE)			// 暂时只支持WAV 的播放
	{
		return RT_FAILURE;
	}
	log_printf("filename: %s, FileType: %d\n", AudioParam->filename, AudioParam->FileType);
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
	   memcpy(&s->hit_param, AudioParam, sizeof(AUDIO_HIT_PARAM));
	   ret = RT_SUCCESS;
	}
	ms_media_unlock(f);	
	return ret;
}

/*************************************************
  Function:		ms_lyly_start
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_lyly_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	LylyData *s = (LylyData*)f->private;
	AUDIO_HIT_PARAM *audio_param = &(s->hit_param);
	log_printf("****************** ms_audio_play_open *******************\n");
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		// 获取音频播放参数
		ret = get_AudioParam(s);
		if (ret == -1)
		{
			goto error;
		}
		
		// 开启播放线程
		ms_thread_init(&s->msplaythread,20);	
		ret = ms_thread_create(&s->msplaythread, ms_lyly_thread, f);		
		if (ret == RT_FAILURE)
		{	
			goto error;
		}
		else
		{
			f->mcount++;
		}
	}
	else
	{
		f->mcount++;
	}
	
	ms_media_unlock(f);
	return RT_SUCCESS;	
	
error:	
	f->mcount = 0;
	ms_media_unlock(f);

	if (g_ReadBuf != NULL)
	{
		if (g_ReadBuf->buffer != NULL)
		{
			free(g_ReadBuf->buffer);
			g_ReadBuf->buffer = NULL;
		}
		free(g_ReadBuf);
		g_ReadBuf = NULL;
	}
	log_printf("ms_avi_play_open return error \n");
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_lyly_close
  Description: 	
  Input: 		无
  Output:		无
  Return:		 
  Others:
*************************************************/
static int ms_lyly_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	LylyData * data = (LylyData*)f->private;
	log_printf("****************** ms_audio_play_close *******************\n");
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		log_printf("****************** ms_audio_play_close *******************\n");
		f->mcount = 0;
		if (f->mcount == 0)
		{
			ms_thread_quit(&data->msplaythread);
			ret = RT_SUCCESS;
		}
	}
	ms_media_unlock(f);

	log_printf("****************** ms_audio_play_close *******************\n");
	return ret;
}


/*************************************************
  Function:		ms_lyly_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_lyly_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		LylyData *data = (LylyData*)malloc(sizeof(LylyData));
		memset(data,0,sizeof(LylyData));
		data->channels = 1;
		data->rate = 8000;
		data->bit_width = 8;
		data->hit_param.FileType = FILE_WAVE;
		data->hit_param.IsPack = 1;
		data->hit_param.PackNum = 4;
		data->msplaythread.thread = -1;
		data->msplaythread.thread_run = 0;
	
		f->private= data;
		f->mcount = 0;
	}

	return HI_SUCCESS;
}

/*************************************************
  Function:		ms_lyly_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_lyly_uninit(struct _MSMediaDesc * f)
{
	LylyData *data = (LylyData*)f->private;
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return HI_SUCCESS;
	}

	return HI_FAILURE;
}

static MSMediaMethod methods[] = 
{
	{MS_AUDIO_LYLY_OPEN,	ms_lyly_open},
	{MS_AUDIO_LYLY_CLOSE,	ms_lyly_close},
	{MS_AUDIO_LYLY_PARAM , 	ms_lyly_param},
	{0,						NULL}
};

MSMediaDesc ms_file_lyly_desc =
{
	.id = MS_FILE_LYLY_ID,
	.name = "MSFileLYLY",
	.text = "Raw files and wav reader",
	.enc_fmt = "n32926",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_lyly_init,
	.uninit = ms_lyly_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};
	
