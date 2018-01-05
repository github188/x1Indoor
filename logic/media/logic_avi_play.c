/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_avi_play.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-09-28
  Description:	4.3寸室内机留影留言AVI 文件播放
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
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
#include "logic_avi_play.h"

#ifdef _TFT_7_
#define AVI_DISPLAY_X 			0
#define AVI_DISPLAY_Y 			0
#define AVI_DISPLAY_W 			640
#define AVI_DISPLAY_H 			480
#else
#define AVI_DISPLAY_X 			0
#define AVI_DISPLAY_Y 			0
#define AVI_DISPLAY_W 			380
#define AVI_DISPLAY_H 			272
#endif

//static S_FB_PARAM g_AviPlay_FB;
static uint8 g_playover_callback = 0;		// 播放结束的回调
static uint8 g_AviPlay_Repeat = 0;			// 重复播放

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
void ms_avi_play_callback(struct _MSMediaDesc * f, uint32 cmd, uint32 param1, uint32 param2)
{
	AviPlayParam *s = (AviPlayParam*)f->private;	
	if (s->callback)
	{
		s->callback(cmd, param1, param2);
	}
}
/*************************************************
  Function:			ms_avi_play_InitFB
  Description:	 	初始化FB
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int32 ms_avi_play_InitFB(void)
{
#if 0
	g_AviPlay_FB.lcm_fd = open( FB_DEVICE, O_RDWR );
	if (g_AviPlay_FB.lcm_fd <= 0)
	{
		log_printf( "### Error: cannot open LCM device, returns %d!\n", g_AviPlay_FB.lcm_fd );
		return 0;
	}

	if (ioctl(g_AviPlay_FB.lcm_fd, FBIOGET_VSCREENINFO, &(g_AviPlay_FB.fb_vinfo)))
	{
		log_printf( "ioctl FBIOGET_VSCREENINFO failed!\n" );
		close( g_AviPlay_FB.lcm_fd );
		return 0;
	}
	g_AviPlay_FB.u8bytes_per_pixel = g_AviPlay_FB.fb_vinfo.bits_per_pixel / 8;

	if (ioctl(g_AviPlay_FB.lcm_fd, FBIOGET_FSCREENINFO, &(g_AviPlay_FB.fb_finfo)))
	{
		log_printf( "ioctl FBIOGET_FSCREENINFO failed!\n" );
		close( g_AviPlay_FB.lcm_fd );
        return 0;
    }

	// Map the device to memory
	g_AviPlay_FB.pLCMBuffer = mmap( NULL, (g_AviPlay_FB.fb_finfo.line_length * g_AviPlay_FB.fb_vinfo.yres ), PROT_READ|PROT_WRITE, MAP_SHARED, g_AviPlay_FB.lcm_fd, 0 );
	if (g_AviPlay_FB.pLCMBuffer == MAP_FAILED)
	{
		log_printf( "### Error: failed to map LCM device to memory!\n" );
		return 0;
	}
	else
	{
		log_printf( "### LCM Buffer at:%p, width = %d, height = %d, line_length = %d.\n\n", g_AviPlay_FB.pLCMBuffer, g_AviPlay_FB.fb_vinfo.xres, g_AviPlay_FB.fb_vinfo.yres, g_AviPlay_FB.fb_finfo.line_length );
	}

	ioctl(g_AviPlay_FB.lcm_fd, IOCTL_FB_LOCK, 0);
	ioctl(g_AviPlay_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_YCBYCR);
	wmemset((wchar_t *)g_AviPlay_FB.pLCMBuffer, 0x80008000, (g_AviPlay_FB.fb_finfo.line_length * g_AviPlay_FB.fb_vinfo.yres)>>2);
	return 1;
#endif
}

/*************************************************
  Function:			ms_avi_play_unInitFB
  Description:	 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_avi_play_unInitFB(void)
{
#if 0
	if (g_AviPlay_FB.lcm_fd > 0) 
	{
		log_printf("\n ******************** ms_jpeg_dec_unInitFB  *****************************  \n");
		ioctl(g_AviPlay_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_RGB565);
		
		ioctl(g_AviPlay_FB.lcm_fd, IOCTL_FB_LOCK, 0);
		memset(g_AviPlay_FB.pLCMBuffer, 0x00, (g_AviPlay_FB.fb_finfo.line_length * g_AviPlay_FB.fb_vinfo.yres));	
		ioctl(g_AviPlay_FB.lcm_fd, IOCTL_FB_UNLOCK, 0);
		
		munmap(g_AviPlay_FB.pLCMBuffer, (g_AviPlay_FB.fb_finfo.line_length * g_AviPlay_FB.fb_vinfo.yres ));
		close(g_AviPlay_FB.lcm_fd);
		g_AviPlay_FB.lcm_fd = -1;
	}	
#endif
}

/*************************************************
  Function:		ms_avi_play_stop
  Description: 	 
  Input: 	
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_avi_play_stop(struct _MSMediaDesc * f)
{
#if 0
	log_printf("****************** ms_avi_play_stop *******************\n");
	AviPlayParam *psStream = (AviPlayParam*)f->private;	
	NM_Close(&psStream->m_hMediaHandle);
	#ifndef __USE_FBIOPAN__
	//log_printf("111111111111111111\n");
	NM_FreeDMAMem();
	//log_printf("22222222222222222222\n");
	ms_avi_play_unInitFB();
	#endif
	//log_printf("33333333333333333333\n");
	usleep(1000);
	f->mcount = 0;
	psStream->aviplaythread.thread= -1;
	psStream->state = AVI_STATE_STOP;
	psStream->m_hMediaHandle = eNM_INVALID_HMEDIA;
#endif
}

/*************************************************
  Function:		ms_avi_play_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void* ms_avi_play_thread(void *proc)
{
#if 0
	g_playover_callback = 0;
	MSMediaDesc *f = (MSMediaDesc *)proc;
	AviPlayParam *psStream = (AviPlayParam*)f->private;	

	int32_t		i32Ret = 0;
	int32_t		i32FileNo = 0;
	S_NM_MEDIAINFO		sMediaInfo;
	S_NM_VIDEOCTX		sVideoOutCtx;
	E_NM_MEDIATYPE		eMediaType = eNM_MEDIA_UNKNOWN;	
	
	log_printf("cmd :%x, val: %s\n", psStream->state, psStream->val);
	NM_RegFileReadIF(eNM_MEDIA_AVI, &g_NM_sAVIRead_IF); 

	i32FileNo = open((char *)psStream->val, O_RDONLY);
	if (i32FileNo <= 0) 
	{
		log_printf("play Media file %s ERROR\n", psStream->val);
		close(i32FileNo);
		ms_avi_play_callback(f, 0, 0, 0); // 出错的回调
		goto error;
	}
	
	eMediaType = eNM_MEDIA_AVI;
	//eMediaType = ms_avi_play_GetMediaType(psStream->val);
	psStream->m_hMediaHandle = (uint32)NM_OpenPlayFile(i32FileNo, (const char*)psStream->val, eMediaType, eNM_PROFILE_FULL);
	if (psStream->m_hMediaHandle == eNM_INVALID_HMEDIA) 
	{
		log_printf("play URL stream %s ERROR %d\n", psStream->val, psStream->m_hMediaHandle);
		ms_avi_play_callback(f, 0, 0, 0); // 出错的回调
		goto error;		
	}

	NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);

	//log_printf("sMediaInfo.sVideoInfo.eVideoType: %d\n", sMediaInfo.sVideoInfo.eVideoType);
	
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

	// add by chenbh 显示按原始数据大小显示
	uint32 display_width = 0;
	uint32 display_height = 0;
	display_width = sMediaInfo.sVideoInfo.u32Width;
	display_height = sMediaInfo.sVideoInfo.u32Height;
	
	log_printf("display_width: %d, display_height:%d", display_width, display_height);
	
	// 设置视频框大小
	memset(&sVideoOutCtx, 0, sizeof(sVideoOutCtx));
	NM_GetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);
	#ifdef __USE_FBIOPAN__
		Smpl_FB_Open(&sVideoOutCtx);

		if(g_pu8FBTmpBuf){
			sVideoOutCtx.pDataVAddr = g_pu8FBTmpBuf;
			sVideoOutCtx.u32X=0;
			sVideoOutCtx.u32Y=0;
		}
		sVideoOutCtx.pfnFillCB = Smpl_FB_Fill;
	#endif
	if (display_width < AVI_DISPLAY_W && display_height < AVI_DISPLAY_H)
	{
		sVideoOutCtx.u32X = (AVI_DISPLAY_W-display_width)/2;
		sVideoOutCtx.u32Y = (AVI_DISPLAY_H-display_height)/2;
		sVideoOutCtx.u32Width = display_width;
		sVideoOutCtx.u32Height = display_height;
	}
	else
	{
		sVideoOutCtx.u32X = AVI_DISPLAY_X;
		sVideoOutCtx.u32Y = AVI_DISPLAY_Y;
		sVideoOutCtx.u32Width = AVI_DISPLAY_W;
		sVideoOutCtx.u32Height = AVI_DISPLAY_H;
	}
	


    //modify by wangzc 20151026 更改播放.avi 文件显示起始位置不在(0, 0)位置的视频显示
    sVideoOutCtx.pDataVAddr = sVideoOutCtx.pDataVAddr + (sVideoOutCtx.u32Y * sVideoOutCtx.u32StrideW + sVideoOutCtx.u32X)*2;
	NM_SetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);

	// 延迟等 UI处理完再做视频显示
	usleep(1000);

	// 设置播放音量
	NM_SetPlayVolume(AVI_PLAY_VOL);
	log_printf("Play volume is %d\n", NM_GetPlayVolume());
	
	// 进入播放线程
	i32Ret = NM_Play(psStream->m_hMediaHandle);  
	if (eNM_ERROR_NONE != i32Ret)
	{
		log_printf("NM_Play return error!!!! \n");
		ms_avi_play_callback(f, 0, 0, 0); 			// 出错的回调
		goto error;
	}
	
	while (psStream->aviplaythread.thread_run)
	{
		NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);
		//log_printf("sMediaInfo.sCommonInfo.eStatus: %d\n", sMediaInfo.sCommonInfo.eStatus);
		if (sMediaInfo.sCommonInfo.eStatus == eNM_STATUS_STOPPED) // 播放结束退出
		{
			if (g_AviPlay_Repeat == 1)
			{
				NM_Seek(psStream->m_hMediaHandle, 0);
				NM_Resume(psStream->m_hMediaHandle);
			}
			else
			{
				g_playover_callback = 1;
				break;
			}
		}
		usleep(1000);
		continue;
	}
	
error:
	#ifdef __USE_FBIOPAN__
	Smpl_FB_ClearFB();
	Smpl_FB_Close();
	#endif
	close(i32FileNo);
	ms_avi_play_stop(f);
	if (g_playover_callback == 1)
	{
		ms_avi_play_callback(f, 1, 1, 100);        	// 等清空完窗口再回调,否则视频没清空，ui底图就刷清了
	}
	g_playover_callback = 0;

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
static int ms_avi_play_open(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;
	int8 times = 3;
	int	i32Ret;
	AviPlayParam *s = (AviPlayParam*)f->private;	
	log_printf("****************** ms_avi_play_open *******************\n");
	ms_media_lock(f);
	system("sync; echo 3 > /proc/sys/vm/drop_caches");   // 释放cache内存
	
	if (f->mcount == 0)
	{		
		#ifndef __USE_FBIOPAN__
		ms_avi_play_unInitFB();
		if (!ms_avi_play_InitFB())
		{
			log_printf("FB init return error \n");
			goto error0;
		}

		while (times > 0)
		{
			times--;
			i32Ret = NM_AllocDMAMem(g_AviPlay_FB.fb_vinfo.xres * g_AviPlay_FB.fb_vinfo.yres );
			if (i32Ret == 0)
			{
				break;
			}
		}		
		if (i32Ret != 0)
		{
			log_printf("NM_AllocDMAMem return error : %d \n", i32Ret);
			goto error1;
		}
		#endif
		
		ms_thread_init(&s->aviplaythread,200);	
		ret = ms_thread_create(&s->aviplaythread, ms_avi_play_thread, f);		
		if (ret == RT_FAILURE)
		{	
			goto error2;
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
	
error2:
	#ifndef __USE_FBIOPAN__
	NM_FreeDMAMem();
	#endif
	
error1:	
	#ifndef __USE_FBIOPAN__
	ms_avi_play_unInitFB();
	#endif
	
error0:	
	f->mcount = 0;
	ms_media_unlock(f);
	s->state = AVI_STATE_STOP;
	ms_avi_play_callback(f, 0, 0, 0); // 出错的回调
	log_printf("ms_avi_play_open return error \n");
	return RT_FAILURE;
#endif
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
static int ms_avi_play_close(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;
	AviPlayParam * data = (AviPlayParam*)f->private;
	log_printf("****************** ms_avi_play_close *******************\n");
	system("sync; echo 3 > /proc/sys/vm/drop_caches");   // 释放cache内存
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount = 0;
		ms_thread_quit(&data->aviplaythread);
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);
	return ret;
#endif
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
static int ms_avi_play_state(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);
	AviPlayParam *s = (AviPlayParam*)f->private;	
	AviPlayState *AviParam = (AviPlayState *)arg;
	log_printf("****************** ms_avi_play_state *******************\n");
	log_printf("AviParam->cmd : %d , AviParam->filename: %s, s->state : %x \n", AviParam->cmd, AviParam->filename, s->state);
	ms_media_lock(f);	
	switch (AviParam->cmd)
	{
		case AVI_CMD_PLAY:
			{
				if (s->state == AVI_STATE_STOP)
				{
					memcpy(s->val, AviParam->filename, sizeof(AviParam->filename));
					s->callback = AviParam->callback;
					s->state = AVI_STATE_PLAY;
					ret = RT_SUCCESS;
				}
				else
					ret = RT_FAILURE;
			}
			break;
			
		case AVI_CMD_PAUSE:
			{
				if (s->state == AVI_STATE_PLAY)
				{
					NM_Pause(s->m_hMediaHandle);
					s->state = AVI_STATE_PAUSE;
				}
				else if (s->state == AVI_STATE_PAUSE)
				{
					NM_Resume(s->m_hMediaHandle);
					s->state = AVI_STATE_PLAY;
					
				}
				else
				{
					ret = RT_FAILURE;
				}
			}
			break;

		case AVI_CMD_CHANGE:
			break;

		case AVI_CMD_STOP:
			{
				s->state = AVI_STATE_STOP;
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
static int ms_avi_play_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		AviPlayParam *data = (AviPlayParam*)malloc(sizeof(AviPlayParam));	
		data->state = AVI_STATE_STOP;
		data->val = (uint8 *)malloc(sizeof(unsigned char)*100);
		//data->m_hMediaHandle = eNM_INVALID_HMEDIA;
		data->callback = NULL;
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_avi_play_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_play_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		AviPlayParam *data = f->private;
		ms_free(data->val);
		ms_free(data);
		data->callback = NULL;
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}


static MSMediaMethod methods[]={
	{MS_AVI_PLAY_STATE,	ms_avi_play_state},
	{MS_AVI_PLAY_OPEN,	ms_avi_play_open},
	{MS_AVI_PLAY_CLOSE,	ms_avi_play_close},
	{0, NULL}
};

MSMediaDesc ms_avi_play_desc={
	.id = MS_AVI_PLAY_ID,
	.name = "MsAviPlay",
	.text = "Avi Play",
	.enc_fmt = "avi",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_avi_play_init,
	.uninit = ms_avi_play_uninit,
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
