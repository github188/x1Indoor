/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_rtsp_play.c
  Author:		chenbh
  Version:		V1.0
  Date:			2015-09-28
  Description:	
  
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
#include "logic_rtsp_play.h"

#ifndef O_CLOEXEC
#define O_CLOEXEC       		02000000        /* oct, set close_on_exec */
#endif

#define CIF_DIS_WIDTH 			352
#define CIF_DIS_HEIGTH			288

#if (_LCD_DPI_ == _LCD_800480_)
#define RTSP_DISPLAY_X 			0
#define RTSP_DISPLAY_Y 			0
#define RTSP_DISPLAY_W 			640
#define RTSP_DISPLAY_H 			480
#elif (_LCD_DPI_ == _LCD_1024600_)
#define RTSP_DISPLAY_X 			0
#define RTSP_DISPLAY_Y 			0
#define RTSP_DISPLAY_W 			640
#define RTSP_DISPLAY_H 			480
#endif

#define RtspNotify(s, param1, param2)	if (s)\
	s(param1, param2)

static uint8 g_DisFullScreen = 0;			// 1全屏显示 0缩屏显示
static uint8 g_LastDisFullScreen = 0;		// 最近一次显示 1全屏显示 0缩屏显示

//static S_FB_PARAM g_RtspPlay_FB;


/*************************************************
  Function:			ms_avi_play_InitFB
  Description:	 	初始化FB
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int32 ms_rtsp_play_InitFB(void)
{
#if 0
	g_RtspPlay_FB.lcm_fd = open( FB_DEVICE, O_RDWR | O_CLOEXEC);
	if (g_RtspPlay_FB.lcm_fd <= 0)
	{
		log_printf( "### Error: cannot open LCM device, returns %d!\n", g_RtspPlay_FB.lcm_fd );
		return FALSE;
	}

	if (ioctl(g_RtspPlay_FB.lcm_fd, FBIOGET_VSCREENINFO, &(g_RtspPlay_FB.fb_vinfo)))
	{
		log_printf( "ioctl FBIOGET_VSCREENINFO failed!\n" );
		close( g_RtspPlay_FB.lcm_fd );
		return FALSE;
	}
	g_RtspPlay_FB.u8bytes_per_pixel = g_RtspPlay_FB.fb_vinfo.bits_per_pixel / 8;

	if (ioctl(g_RtspPlay_FB.lcm_fd, FBIOGET_FSCREENINFO, &(g_RtspPlay_FB.fb_finfo)))
	{
		log_printf( "ioctl FBIOGET_FSCREENINFO failed!\n" );
		close( g_RtspPlay_FB.lcm_fd );
        return FALSE;
    }
	
	// Map the device to memory
	g_RtspPlay_FB.pLCMBuffer = mmap( NULL, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres ), PROT_READ|PROT_WRITE, MAP_SHARED, g_RtspPlay_FB.lcm_fd, 0 );
	if (g_RtspPlay_FB.pLCMBuffer == MAP_FAILED)
	{
		log_printf( "### Error: failed to map LCM device to memory!\n" );
		return FALSE;
	}
	else
	{
		log_printf( "### LCM Buffer at:%p, width = %d, height = %d, line_length = %d.\n\n", g_RtspPlay_FB.pLCMBuffer, g_RtspPlay_FB.fb_vinfo.xres, g_RtspPlay_FB.fb_vinfo.yres, g_RtspPlay_FB.fb_finfo.line_length );
	}

	//ioctl(g_RtspPlay_FB.lcm_fd, IOCTL_FB_LOCK, 0);
	//ioctl(g_RtspPlay_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_YCBYCR);
	//wmemset((wchar_t *)g_RtspPlay_FB.pLCMBuffer, 0x80008000, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres)>>2);
#endif
	return TRUE;	
}

/*************************************************
  Function:			ms_rtsp_play_unInitFB
  Description:	 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_rtsp_play_unInitFB(void)
{
#if 0
	if (g_RtspPlay_FB.lcm_fd > 0) 
	{
		log_printf("\n ******************** ms_jpeg_dec_unInitFB  *****************************  \n");
		ioctl(g_RtspPlay_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_RGB565);
		memset(g_RtspPlay_FB.pLCMBuffer, 0x00, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres));
		/*
		ioctl(g_RtspPlay_FB.lcm_fd, IOCTL_FB_LOCK, 0);
		memset(g_RtspPlay_FB.pLCMBuffer, 0x00, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres));	
		ioctl(g_RtspPlay_FB.lcm_fd, IOCTL_FB_UNLOCK, 0);
		*/
		
		if (g_RtspPlay_FB.pLCMBuffer)
		{
			munmap(g_RtspPlay_FB.pLCMBuffer, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres ));
			g_RtspPlay_FB.pLCMBuffer = NULL;
		}
		close(g_RtspPlay_FB.lcm_fd);
		g_RtspPlay_FB.lcm_fd = -1;
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
static void ms_rtsp_play_stop(struct _MSMediaDesc * f)
{
#if 0
	log_printf("****************** ms_avi_play_stop *******************\n");
	RtspPlayParam *psStream = (RtspPlayParam*)f->private;	
	NM_Close(&psStream->m_hMediaHandle);
	//printf("111111111111111111111111\n");
	NM_FreeDMAMem();
	//printf("222222222222222222222222\n");
	ms_rtsp_play_unInitFB();	
	//printf("3333333333333333333333333333\n");
	usleep(1000);
	f->mcount = 0;
	psStream->rtspplaythread.thread= -1;
	psStream->m_hMediaHandle = eNM_INVALID_HMEDIA;
#endif
}

/*************************************************
  Function:		ms_rtsp_play_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void* ms_rtsp_play_thread(void *proc)
{
#if 0
	MSMediaDesc *f = (MSMediaDesc *)proc;
	RtspPlayParam *psStream = (RtspPlayParam*)f->private;	

	int32_t		i32Ret = 0;
	S_NM_MEDIAINFO		sMediaInfo;
	S_NM_VIDEOCTX		sVideoOutCtx;
	S_NM_MEDIAREAD_IF	*psMediaReadIF;
	S_NM_AUDIOCTX		sAudioOutCtx;
	
	log_printf(" val: %s\n", psStream->val);
	
	S_NM_RTSPREAD_PRIV sRTSPPriv;
	psMediaReadIF = (S_NM_MEDIAREAD_IF *)&g_NM_sRTSPRead_IF; //&g_NM_sAVIRead_IF;

	//static char *url = "rtsp://10.110.18.28:554/MOBILELIVECAST_SelfIPC_10.110.18.235_554_1_h264_QVGA_512_15.sdp?ACCOUNT@user=12345&password=12345";
	//static char *url = "rtsp://12345:12345@10.110.18.28#554/MOBILELIVECAST_SelfIPC_10.110.18.231_554_1_h264_QVGA_512_15.sdp?ACCOUNT@user=12345&password=12345";
	//static char *url =  "rtsp://12345:12345@10.110.18.28#554/MOBILELIVECAST_HIKVISION_10.110.18.24_8000_1_h264_QVGA_512_15.sdp?ACCOUNT@user=12345&password=12345";
	sRTSPPriv.pszURI = psStream->val;
	sRTSPPriv.bTCP = FALSE;
	printf("sRTSPPriv.pszURI: %s\n", sRTSPPriv.pszURI);
	psStream->m_hMediaHandle = NM_OpenPlayMedia(psMediaReadIF, eNM_PROFILE_FULL, &sRTSPPriv); // 返回正确进入监视中
	if (psStream->m_hMediaHandle == eNM_INVALID_HMEDIA) 
	{
		log_printf("play URL stream %s ERROR %d\n", psStream->val, psStream->m_hMediaHandle);
		//psStream->callback(RTSP_STATE_STOP, -1);
		RtspNotify(psStream->callback, RTSP_STATE_STOP, -1);
		goto error;		
	}
	else 
	{
		// NM_OpenPlayMedia比较耗时 返回成功时可能外部已经把该线程暂停了
		if (psStream->rtspplaythread.thread_run == 0)
		{
			printf("thread_run over !!!!\n");
			goto error;
		}
	}
	
	NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);

	/*
	// add by chenbh 显示按原始数据大小显示
	uint32 display_width = 0;
	uint32 display_height = 0;
	uint32 display_addr = 0;
	log_printf("u32Width: %d, u32Height: %d\n", sMediaInfo.sVideoInfo.u32Width, sMediaInfo.sVideoInfo.u32Height);
	display_width = sMediaInfo.sVideoInfo.u32Width;
	display_height = sMediaInfo.sVideoInfo.u32Height;
	*/
	sAudioOutCtx.ePCMType = eNM_PCM_S16LE;
	sAudioOutCtx.u32ChannelNum = sMediaInfo.sAudioInfo.u32ChannelNum;
	sAudioOutCtx.u32SampleRate = sMediaInfo.sAudioInfo.u32SampleRate;

	if (sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_MJPG)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_VIDEODEC, &g_NM_sMJPGDec_IF);
	else if (sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_JPEG)
		NM_RegCodecIF(psStream->m_hMediaHandle, eNM_CODEC_VIDEODEC, &g_NM_sJPEGDec_IF);
	else if (sMediaInfo.sVideoInfo.eVideoType == eNM_VIDEO_H264)
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

	sMediaInfo.sCommonInfo.u64JumpDuration = 1;
	NM_SetMediaInfo(psStream->m_hMediaHandle, eNM_INFO_COMMON, &sMediaInfo.sCommonInfo, eNM_COMMONINFO_JUMPDURATION);
	
	// 设置视频框大小
	uint32 display_width = 0;
	uint32 display_height = 0;
	uint32 display_addr = 0;
	display_width = psStream->rect.width;
	display_height = psStream->rect.height;
	if (display_width == 0 || display_height == 0)
	{
		display_width = CIF_DIS_WIDTH;
		display_height = CIF_DIS_HEIGTH;
	}
	
	memset(&sVideoOutCtx, 0, sizeof(sVideoOutCtx));
	NM_GetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);
	//sVideoOutCtx.u32X = (RTSP_DISPLAY_W-CIF_DIS_WIDTH)/2;
	//sVideoOutCtx.u32Y = (RTSP_DISPLAY_H-CIF_DIS_HEIGTH)/2;
	//sVideoOutCtx.u32Width = CIF_DIS_WIDTH;
	//sVideoOutCtx.u32Height = CIF_DIS_HEIGTH;
	log_printf("sVideoOutCtx.u32Width: %d, sVideoOutCtx.u32Height: %d\n", sVideoOutCtx.u32Width, sVideoOutCtx.u32Height);
	if (display_width <= RTSP_DISPLAY_W)
	{
		sVideoOutCtx.u32X = (RTSP_DISPLAY_W-display_width)/2;
		sVideoOutCtx.u32Width = display_width;
	}
	else
	{
		sVideoOutCtx.u32X = 0;
		sVideoOutCtx.u32Width = RTSP_DISPLAY_W;
	}

	if (display_height <= RTSP_DISPLAY_H)
	{
		sVideoOutCtx.u32Y = (RTSP_DISPLAY_H-display_height)/2;
		sVideoOutCtx.u32Height = display_height;
	}	
	else
	{
		sVideoOutCtx.u32Y = 0;
		sVideoOutCtx.u32Height = RTSP_DISPLAY_H;
	}
	display_addr = (uint32)sVideoOutCtx.pDataVAddr;
	sVideoOutCtx.pDataVAddr = sVideoOutCtx.pDataVAddr + (sVideoOutCtx.u32Y * sVideoOutCtx.u32StrideW + sVideoOutCtx.u32X)*2;
	NM_SetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);

	// 保存一份实际显示大小
	display_width = sVideoOutCtx.u32Width;
	display_height = sVideoOutCtx.u32Height;
	
	// 进入播放线程
	i32Ret = NM_Play(psStream->m_hMediaHandle);  
	if (eNM_ERROR_NONE != i32Ret)
	{
		log_printf("NM_Play return error!!!! \n");
		//psStream->callback(RTSP_STATE_STOP, 0);
		RtspNotify(psStream->callback, RTSP_STATE_STOP, 0);
		goto error;
	}

	uint8 rtsp_play = 0;
	while (psStream->rtspplaythread.thread_run)
	{
		NM_GetMediaInfo(psStream->m_hMediaHandle, (S_NM_MEDIAINFO*)&sMediaInfo);
		if (sMediaInfo.sCommonInfo.eStatus == eNM_STATUS_PLAYING)
		{
			// 只发送一次状态
			if (0 == rtsp_play)
			{
				RtspNotify(psStream->callback, RTSP_STATE_PLAY, 0);
				rtsp_play = 1;
			}

			if (g_DisFullScreen != g_LastDisFullScreen)
			{
				g_LastDisFullScreen = g_DisFullScreen;				
				memset(&sVideoOutCtx, 0, sizeof(sVideoOutCtx));
				NM_GetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);
				if (1 == g_LastDisFullScreen)
				{
					sVideoOutCtx.u32X = 0;
					sVideoOutCtx.u32Y = 0;
					sVideoOutCtx.u32Width = RTSP_DISPLAY_W;
					sVideoOutCtx.u32Height = RTSP_DISPLAY_H;
				}
				else
				{
					sVideoOutCtx.u32X = (RTSP_DISPLAY_W-display_width)/2;
					sVideoOutCtx.u32Y = (RTSP_DISPLAY_H-display_height)/2;
					sVideoOutCtx.u32Width = display_width;
					sVideoOutCtx.u32Height = display_height;
				}				
				sVideoOutCtx.pDataVAddr = (void *)display_addr + (sVideoOutCtx.u32Y * sVideoOutCtx.u32StrideW + sVideoOutCtx.u32X)*2;
				NM_SetContext(psStream->m_hMediaHandle, eNM_CTX_VIDEOOUT, &sVideoOutCtx);
				//wmemset((wchar_t *)g_RtspPlay_FB.pLCMBuffer, 0x80008000, (g_RtspPlay_FB.fb_finfo.line_length * g_RtspPlay_FB.fb_vinfo.yres)>>2);
			}
		}
		else if(sMediaInfo.sCommonInfo.eStatus == eNM_STATUS_STOPPED) // 播放结束退出 
		{			
			printf("sMediaInfo.sCommonInfo.eStatus == eNM_STATUS_STOPPED \n");
			//psStream->callback(RTSP_STATE_STOP, 0);
			RtspNotify(psStream->callback, RTSP_STATE_STOP, 0);
			break;
		}		
		usleep(1000*100);
		continue;
	}
	
error:
	ms_rtsp_play_stop(f);
	log_printf("[%d] over\n", __LINE__);
	pthread_detach(pthread_self());
    pthread_exit(NULL);
#endif
	
}

/*************************************************
  Function:		ms_rtsp_play_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_open(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;
	int8 times = 3;
	int	i32Ret;
	RtspPlayParam *s = (RtspPlayParam*)f->private;	
	log_printf("****************** ms_rtsp_play_open *******************\n");
	ms_media_lock(f);
	
	if (f->mcount == 0)
	{		
		g_LastDisFullScreen = 0;
		g_DisFullScreen = 0;

		
		ms_rtsp_play_unInitFB();
		if (FALSE == ms_rtsp_play_InitFB())
		{
			log_printf("FB init return error \n");
			goto error0;
		}
		
		
		while (times > 0)
		{
			times--;
			i32Ret = NM_AllocDMAMem(g_RtspPlay_FB.fb_vinfo.xres * g_RtspPlay_FB.fb_vinfo.yres);
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
		
		ms_thread_init(&s->rtspplaythread, 20);	
		ret = ms_thread_create(&s->rtspplaythread, ms_rtsp_play_thread, f);		
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
	NM_FreeDMAMem();
	
error1:	
	ms_rtsp_play_unInitFB();
	
error0:	
	f->mcount = 0;
	ms_media_unlock(f);
	log_printf("ms_avi_play_open return error \n");
#endif
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_rtsp_play_close
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	RtspPlayParam * data = (RtspPlayParam*)f->private;
	log_printf("****************** ms_rtsp_play_close *******************\n");
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount = 0;
		ms_thread_quit(&data->rtspplaythread);
		usleep(1000*100);
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_rtsp_play_param
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_param(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);	
	RtspPlayParam *s = (RtspPlayParam*)f->private;	
	ms_media_lock(f);	
	if (f->mcount == 0)
	{
		if (s->val == NULL)
		{
			s->val = (uint8 *)malloc(sizeof(unsigned char)*100);
		}

		RtspPlayParam *param = (RtspPlayParam *)arg;
		strcpy(s->val, param->val);
		memcpy(&(s->rect), &(param->rect), sizeof(V_RECT_S));
		s->callback = param->callback;
		log_printf("s->val: %s s->rect.width: %d, s->rect.height: %d\n", s->val, s->rect.width, s->rect.height);
		ret = RT_SUCCESS;
	}	
	ms_media_unlock(f);	
	return ret;
}

/*************************************************
  Function:		ms_rtsp_play_level
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_level(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);	
	
	ms_media_lock(f);	
	if (f->mcount > 0)
	{
		g_DisFullScreen = *(uint8 *)arg;
		log_printf("g_DisFullScreen : %d\n", g_DisFullScreen);
	}	
	ms_media_unlock(f);	
	return ret;
}

/*************************************************
  Function:		ms_rtsp_play_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_init(struct _MSMediaDesc * f)
{
#if 0
	if (NULL == f->private)
	{
		memset(&g_RtspPlay_FB, 0, sizeof(S_FB_PARAM));
		RtspPlayParam *data = (RtspPlayParam*)malloc(sizeof(RtspPlayParam));	
		data->val = (uint8 *)malloc(sizeof(unsigned char)*100);
		data->m_hMediaHandle = eNM_INVALID_HMEDIA;
		data->callback = NULL;
		f->private = data;
		f->mcount = 0;
	}
#endif
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_rtsp_play_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_rtsp_play_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		RtspPlayParam *data = f->private;
		ms_free(data->val);
		ms_free(data);
		data->callback = NULL;
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}


static MSMediaMethod methods[]={
	{MS_RTSP_PLAY_PARAM,	ms_rtsp_play_param},
	{MS_RTSP_PLAY_OPEN,		ms_rtsp_play_open},
	{MS_RTSP_PLAY_CLOSE,	ms_rtsp_play_close},
	{MS_RTSP_PLAY_LEVEL, 	ms_rtsp_play_level},
	{0, NULL}
};

MSMediaDesc ms_rtsp_play_desc={
	.id = MS_RTSP_PLAY_ID,
	.name = "MsRtspPlay",
	.text = "Rtsp Play",
	.enc_fmt = "h264",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_rtsp_play_init,
	.uninit = ms_rtsp_play_uninit,
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





