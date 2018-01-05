/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_mp3_play.c
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2014-12-11
  Description:  MP3 音频播放
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <wchar.h>
#include <sys/mman.h>

#include "logic_include.h"
#include "logic_mp3_play.h"

static uint8 g_Mp3Play_Repeat = 0;					// 重复播放

/*************************************************
  Function:			lyly_err_callback
  Description:		错误回调
  Input: 	
  	1.f				
  	2.cmd			0 出错 1正确
  	3.param1		时间秒 暂时没用 默认填1
  	4.param2		播放百分比
  Output:			无
  Return:			无
  Others:
*************************************************/
void ms_mp3_play_callback(struct _MSMediaDesc * f, uint32 cmd, uint32 param1, uint32 param2)
{
	Mp3PlayParam *s = (Mp3PlayParam*)f->private;	
	if (s->callback)
	{
		s->callback(cmd, param1, param2);
	}
}

/*************************************************
  Function:		ms_avi_play_stop
  Description: 	 
  Input: 	
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_mp3_play_stop(struct _MSMediaDesc * f)
{
#if 0
	log_printf("****************** ms_mp3_play_stop *******************\n");
	Mp3PlayParam *psStream = (Mp3PlayParam*)f->private;	
	NM_Close(&psStream->m_hMediaHandle);
	psStream->m_hMediaHandle = eNM_INVALID_HMEDIA;
	usleep(1000);
	f->mcount = 0;
	psStream->mp3playthread.thread= -1;
	psStream->state = MP3_STATE_STOP;
#endif
}

/*************************************************
  Function:		ms_mp3_play_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void* ms_mp3_play_thread(void *proc)
{
#if 0
	MSMediaDesc *f = (MSMediaDesc *)proc;
	Mp3PlayParam *psStream = (Mp3PlayParam*)f->private;	

	int32_t		i32Ret = 0;
	int32_t		i32FileNo = 0;
	S_NM_MEDIAINFO		sMediaInfo;
	E_NM_MEDIATYPE		eMediaType = eNM_MEDIA_UNKNOWN;

	NM_RegFileReadIF(eNM_MEDIA_MP3, &g_NM_sMP3Read_IF); 

	log_printf("cmd :%x, val: %s\n", psStream->state, psStream->val);
	i32FileNo = open((char *)psStream->val, O_RDONLY);
	if (i32FileNo <= 0) 
	{
		log_printf("play Media file %s ERROR\n", psStream->val);
		close(i32FileNo);
		ms_mp3_play_callback(f, 0, 0, 0); 			// 出错的回调
		goto error;
	}
	
	eMediaType = eNM_MEDIA_MP3;
	psStream->m_hMediaHandle = (uint32)NM_OpenPlayFile(i32FileNo, (const char*)psStream->val, eMediaType, eNM_PROFILE_AUDIO);
	if (psStream->m_hMediaHandle == eNM_INVALID_HMEDIA) 
	{
		log_printf("play URL stream %s ERROR %d\n", psStream->val, psStream->m_hMediaHandle);
		ms_mp3_play_callback(f, 0, 0, 0); 			// 出错的回调
		goto error;		
	}

	NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);
		
	if (sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_MJPG)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_VIDEODEC, &g_NM_sMJPGDec_IF);
	else if (sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_JPEG)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_VIDEODEC, &g_NM_sJPEGDec_IF);
	else if(sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_H264)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_VIDEODEC, &g_NM_sH264Dec_IF);

	if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_ULAW)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sULawDec_IF);
	else if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_ALAW)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sALawDec_IF);
	else if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_ADPCM)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sADPCMDec_IF);
	else if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_MP3)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sMP3Dec_IF);
	else if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_AAC)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sAACDec_IF);
	else if (sMediaInfo.sAudioInfo.eAudioType == eNM_AUDIO_G726)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_AUDIODEC, &g_NM_sG726Dec_IF);

	NM_SetPlayVolume(MP3_PLAY_VOL);
	log_printf("Play volume is %d\n", NM_GetPlayVolume());

	// 进入播放线程
	i32Ret = NM_Play(psStream->m_hMediaHandle);  
	if (i32Ret != eNM_ERROR_NONE)
	{
		log_printf("NM_Play return error  !!!!\n");
		ms_mp3_play_callback(f, 0, 0, 0); 			// 出错的回调
		goto error;
	}

	
	while (psStream->mp3playthread.thread_run)
	{
		NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);
		if(sMediaInfo.sCommonInfo.eStatus == eNM_STATUS_STOPPED) // 播放结束退出
		{
			if (g_Mp3Play_Repeat == 1)
			{
				NM_Seek(psStream->m_hMediaHandle, 0);
				NM_Resume(psStream->m_hMediaHandle);
			}
			else
			{
				ms_mp3_play_callback(f, 1, 1, 100);
				break;
			}	
		}		
		
		usleep(1000);
		continue;
	}
	
error:
	ms_mp3_play_stop(f);
	log_printf("[%d] over\n", __LINE__);
	pthread_detach(pthread_self());
    pthread_exit(NULL);
#endif
}

/*************************************************
  Function:		ms_avi_play_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_mp3_play_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	Mp3PlayParam *s = (Mp3PlayParam*)f->private;	
	log_printf("****************** ms_mp3_play_open *******************\n");
	ms_media_lock(f);
	if (f->mcount == 0)
	{		

		ms_thread_init(&s->mp3playthread,20);	
		ret = ms_thread_create(&s->mp3playthread, ms_mp3_play_thread, f);		
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
	s->state = MP3_STATE_STOP;
	log_printf("ms_avi_play_open return error \n");
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_avi_play_close
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_mp3_play_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	Mp3PlayParam * data = (Mp3PlayParam*)f->private;
	log_printf("****************** ms_mp3_play_close *******************\n");
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount = 0;
		ms_thread_quit(&data->mp3playthread);
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_avi_play_state
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_mp3_play_state(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);
	Mp3PlayParam *s = (Mp3PlayParam*)f->private;	
	Mp3PlayState *Mp3Param = (Mp3PlayState *)arg;
	log_printf("****************** ms_mp3_play_state *******************\n");
	log_printf("Mp3Param->cmd : %d , Mp3Param->filename: %s, s->state : %x \n", Mp3Param->cmd, Mp3Param->filename, s->state);
	ms_media_lock(f);	
	switch (Mp3Param->cmd)
	{
		case MP3_CMD_PLAY:
			{
				if (s->state == MP3_STATE_STOP)
				{
					memcpy(s->val, Mp3Param->filename, sizeof(Mp3Param->filename));
					s->callback = Mp3Param->callback;
					s->state = MP3_STATE_PLAY;
					ret = RT_SUCCESS;
				}
				else
					ret = RT_FAILURE;
			}
			break;
			
		case MP3_CMD_PAUSE:
			{
				if (s->state == MP3_STATE_PLAY)
				{
					NM_Pause(s->m_hMediaHandle);
					s->state = MP3_STATE_PAUSE;
				}
				else if (s->state == MP3_STATE_PAUSE)
				{
					NM_Resume(s->m_hMediaHandle);
					s->state = MP3_STATE_PLAY;
					
				}
				else
				{
					ret = RT_FAILURE;
				}
			}
			break;

		case MP3_CMD_CHANGE:
			break;

		case MP3_CMD_STOP:
			{
				s->state = MP3_STATE_STOP;
			}
			break;
			
		default:
			ret = RT_FAILURE;
			break;
	}
	ms_media_unlock(f);	
	return ret;
#endif
}

/*************************************************
  Function:		ms_avi_play_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_mp3_play_init(struct _MSMediaDesc * f)
{
#if 0
	if (NULL == f->private)
	{
		Mp3PlayParam *data = (Mp3PlayParam*)malloc(sizeof(Mp3PlayParam));	
		data->state = MP3_STATE_STOP;
		data->val = (uint8 *)malloc(sizeof(unsigned char)*100);
		data->m_hMediaHandle = eNM_INVALID_HMEDIA;
		data->callback = NULL;
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
#endif
}

/*************************************************
  Function:		ms_avi_play_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_mp3_play_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		Mp3PlayParam *data = f->private;
		ms_free(data->val);
		ms_free(data);
		data->callback = NULL;
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}

static MSMediaMethod methods[] =
{
	{MS_MP3_PLAY_STATE,	ms_mp3_play_state},
	{MS_MP3_PLAY_OPEN,	ms_mp3_play_open},
	{MS_MP3_PLAY_CLOSE,	ms_mp3_play_close},
	{0, NULL}
};

MSMediaDesc ms_mp3_play_desc =
{
	.id = MS_MP3_PLAY_ID,
	.name = "MsMp3Play",
	.text = "Mp3 Play",
	.enc_fmt = "mp3",
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_mp3_play_init,
	.uninit = ms_mp3_play_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 0,				// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};

