/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_media_record.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机留影留言AVI 文件录制
  
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
 
#include "wav_record.h"
#include "logic_avi_record.h"

#define _SAVE_RECV_AUDIO_  		0
#define _SAVE_RECV_VIDEO_  		0
#define _ANALYSIS_SPS_FRAME_	1					// mediainfo数据根据sps数据包解析出来的数据填充

#define DEFAULT_SAVE_FOLDER		"/mnt/nand1-2/LYLY"
#define DEF_REC_FN_LEN			50					// Maximum length of AVI file name
#define DEF_MAX_REC_TIME		100					// eNM_UNLIMIT_TIME	//Maximum recording time
#define DEF_DISK_SPACE_REQ		(50 * 1024 * 1024) 
#define DEF_AUDIO_SAMPLE_RATE	8000

static uint8 g_get_Iframe = 0;

#if _SAVE_RECV_VIDEO_
static FILE *pRecordH264File;
#define RECORD_H264_VIDEO 		"/mnt/nand1-2/LYLY/record_video.h264"
#endif

#if _SAVE_RECV_AUDIO_
static FILE *pRecordAlawRecFile;
#define RECORD_ALAW_AUDIO 		"/mnt/nand1-2/LYLY/record_audio.alaw"
#endif

#if (_SAVE_RECV_VIDEO_ || _SAVE_RECV_AUDIO_)
/*************************************************
  Function:			Record_Recv_SaveFie
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
static void Record_Recv_SaveFie(FILE *pFile, mblk_t *arg)
{
    if (!pFile)
    {
        return ;
    }
    fwrite(arg->address, arg->len, 1, pFile);
    fflush(pFile);
}
#endif

bool IsDiskSpaceEnough(char *pchSaveFolder)
{
	struct statfs sDiskStat;
	uint64_t u64DiskAvailSpace;
	
	if(statfs(pchSaveFolder, &sDiskStat) < 0)
		return false;

	u64DiskAvailSpace =  (uint64_t)sDiskStat.f_bsize * (uint64_t)sDiskStat.f_bavail;

	if(u64DiskAvailSpace < DEF_DISK_SPACE_REQ)
		return false;

	return true;
}

static int NewWavFile(char *pchSaveFilename, S_WavFileWriteInfo *psWavInfo)
{
#if 0
	char *pchFilePath = NULL;

	pchFilePath = malloc(strlen(pchSaveFilename) + DEF_REC_FN_LEN);
	if(pchFilePath == NULL)
		return -1;

	//sprintf(pchFilePath, "%s/NVTDV%012"PRId64".wav", pchSaveFolder, u64FNPostIdx); 
	sprintf(pchFilePath, "%s.wav", pchSaveFilename); 

	if(WavFileUtil_Write_Initialize(psWavInfo, pchFilePath) == FALSE){
		log_printf("Create WAV file failed!\n");
		free(pchFilePath);
		return -2;
	}
	
	if(WavFileUtil_Write_SetFormat(psWavInfo, eWAVE_FORMAT_G711_ALAW, 1, DEF_AUDIO_SAMPLE_RATE, 8) == FALSE){
		WavFileUtil_Write_Finish(psWavInfo);
		log_printf("Set WAV file format failed!\n");
		free(pchFilePath);
		return -3;
	}
	
	free(pchFilePath);
	return 0;
#endif
}


#if _ANALYSIS_SPS_FRAME_

/*************************************************
  Function:		ms_avirecord_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * ms_avirecord_thread(void *ptr)
{
#if 0
	uint8 *pu8AVIMem = NULL;	
	bool bAudioOnlyMode = FALSE;
	S_AVIUTIL_AVIHANDLE sAVIHandle;	
	S_WavFileWriteInfo sWavFileInfo;
	int32 i32Ret = 0,  i32AVIFd = -1;	
	MSMediaDesc *f = (MSMediaDesc *)ptr;
	RecordParam * data = (RecordParam*)(f->private);	
	
	if (data->RecordMode != LWM_AUDIO_VIDEO)
	{
		bAudioOnlyMode = TRUE;
	}

	// avi格式初始化在 获取SPS数据帧后初始化
	if (TRUE == bAudioOnlyMode)
	{
		if (ms_avi_info_init(data, &i32AVIFd, &sAVIHandle, &sWavFileInfo, &pu8AVIMem) < 0)
		{
			goto exit_prog;
		}
	}
	
#if _SAVE_RECV_VIDEO_
pRecordH264File = fopen(RECORD_H264_VIDEO, "wb");
#endif

#if _SAVE_RECV_AUDIO_
pRecordAlawRecFile = fopen(RECORD_ALAW_AUDIO, "wb");
#endif

		
	mblk_t video_arg;
	mblk_t audio_arg;
	uint8 video_echo = FALSE;
	uint8 audio_echo = FALSE;
	if (bAudioOnlyMode == FALSE)
	{
		g_get_Iframe = 0;
		while(f->thread.thread_run)
		{
			//Acquire packet from video list
			video_echo = ms_queue_get_chunk(f, &video_arg, 2);	

			audio_echo = ms_queue_get_chunk(f, &audio_arg, 1);			
			
			if (video_echo == FALSE && audio_echo == FALSE)	
			{
				usleep(10*1000);
				continue;
			}
			
			/*
			//Checking disk free space
			if(IsDiskSpaceEnough(pchSaveFolder) == false)
				break;
				*/
			if (g_get_Iframe == 0)
			{
				if (video_echo == TRUE)
				{
					if ((0x1f & video_arg.address[4]) == 0x07)
					{
						video_echo = TRUE;
						g_get_Iframe = 1;
						if (ms_avi_info_init(data, &i32AVIFd, &sAVIHandle, &sWavFileInfo, &pu8AVIMem) < 0)
						{
							goto exit_prog;
						}
					}
					else
					{
						ms_queue_pop_chunk(f, 2);
						ms_queue_pop_chunk(f, 1);
						video_echo = FALSE;
						continue;
					}
				}
				else if (audio_echo == TRUE)
				{
					ms_queue_pop_chunk(f, 1);
					audio_echo = FALSE;
					continue;
				}
			}

			#if _SAVE_RECV_VIDEO_
			if (video_echo == TRUE && bAudioOnlyMode == FALSE)
			{
				Record_Recv_SaveFie(pRecordH264File, &video_arg);
			}
			#endif

			#if _SAVE_RECV_AUDIO_
			if (audio_echo == TRUE)
			{
				Record_Recv_SaveFie(pRecordAlawRecFile, &audio_arg);
			}
			#endif

			// Write to AVI file
			if(video_echo == TRUE && bAudioOnlyMode == FALSE)
			{
				AVIUtility_AddVideoFrame(&sAVIHandle, (uint8 *)video_arg.address, video_arg.len);
				ms_queue_pop_chunk(f, 2);
			}

			if(audio_echo == TRUE)
			{
				AVIUtility_AddAudioFrame(&sAVIHandle, (uint8 *)audio_arg.address, audio_arg.len);
				ms_queue_pop_chunk(f, 1);
			}
		}
	}
	else
	{
		while(f->thread.thread_run)
		{
			audio_echo = ms_queue_get_chunk(f, &audio_arg, 1);
			if (audio_echo == FALSE)	
			{
				usleep(10*1000);
				continue;
			}

			/*
			//Checking disk free space
			if(IsDiskSpaceEnough(pchSaveFolder) == false)
				break;
				*/

			#if _SAVE_RECV_AUDIO_
			if (audio_echo == TRUE)
			{
				Record_Recv_SaveFie(pRecordAlawRecFile, &audio_arg);
			}
			#endif
		
			if (audio_echo == TRUE)
			{
				WavFileUtil_Write_WriteData(&sWavFileInfo, audio_arg.address, audio_arg.len);
				ms_queue_pop_chunk(f, 1);
			}
		}
	}

	log_printf(" exit avirecord thread!!!!!!!!!!! \n");
	f->thread.thread = -1;

	if (bAudioOnlyMode == FALSE)
	{
		// Finialize AVI file
		if (i32AVIFd > 0)
		{
			AVIUtility_FinalizeAVIFile(&sAVIHandle);
		}
	}
	else
	{
		// Finnailiz Wav file
		WavFileUtil_Write_Finish(&sWavFileInfo);
	}
	fflush(fPListFile);
	fsync(fileno(fPListFile));

exit_prog:
	if (i32AVIFd > 0)
	{
		close(i32AVIFd);
	}
	if (pu8AVIMem)
	{
		free(pu8AVIMem);
	}
	
	pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NULL;
#endif
}
#else
/*************************************************
  Function:		ms_avirecord_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * ms_avirecord_thread(void *ptr)
{
#if 0
	uint8 *pu8AVIMem = NULL;
	bool bAudioOnlyMode = FALSE;
	char pchSaveFilename[100] = {0};						
	int32 i32Ret = 0,  i32AVIFd = -1;
	
	S_NM_VIDEOCTX sVideoInCtx;
	E_VIN_DEV eVinDevType = eVIN_DEV_UVC_H264;
	int32 i32AudioFrameSize = DEF_AUDIO_SAMPLE_RATE;
	
	MSMediaDesc *f = (MSMediaDesc *)ptr;
	RecordParam * data = (RecordParam*)(f->private);
	
	if (data->filename == NULL)
	{
		return NULL;
	}
	else
		memcpy(pchSaveFilename, data->filename, sizeof(data->filename));
	
	if (data->RecordMode != LWM_AUDIO_VIDEO)
	{
		bAudioOnlyMode = TRUE;
	}
	log_printf("pchSaveFilename : %s \n", pchSaveFilename);	

	eVinDevType = eVIN_DEV_UVC_H264;

	// 视频参数必须为实际参数 否则录制下来 播放时容易死机
	memset(&sVideoInCtx, 0x00, sizeof(S_NM_VIDEOCTX));
	get_h264_rect(&sVideoInCtx.u32Width, &sVideoInCtx.u32Height);
	//sVideoInCtx.u32Width = 640;
	//sVideoInCtx.u32Height = 480;
	sVideoInCtx.u32StrideW = sVideoInCtx.u32Width;
	sVideoInCtx.u32StrideH = sVideoInCtx.u32Height;
	log_printf("sVideoInCtx.u32StrideW: %d, sVideoInCtx.u32StrideH: %d\n", sVideoInCtx.u32StrideW, sVideoInCtx.u32StrideH);
	
	S_AVIUTIL_AVIHANDLE sAVIHandle;
	S_AVIUTIL_FILEATTR sAVIFileAttr;
	S_WavFileWriteInfo sWavFileInfo;
	
	if (bAudioOnlyMode == FALSE)
	{
		uint32 u32RecTimeSec = DEF_MAX_REC_TIME; 
		memset(&sAVIHandle, 0x00, sizeof(S_AVIUTIL_AVIHANDLE));
		memset(&sAVIFileAttr, 0x00, sizeof(S_AVIUTIL_FILEATTR));

		if(u32RecTimeSec == eNM_UNLIMIT_TIME)
			sAVIHandle.bIdxUseFile = TRUE;    

		if(eVinDevType == eVIN_DEV_UVC_JPEG)
			sAVIFileAttr.u32VideoFcc = MJPEG_FOURCC;
		else if(eVinDevType == eVIN_DEV_UVC_H264)
			sAVIFileAttr.u32VideoFcc = H264_FOURCC;
		else if(eVinDevType == eVIN_DEV_UVC_RAW)
			sAVIFileAttr.u32VideoFcc = YUYV_FOURCC;

		sAVIFileAttr.u32FrameWidth = sVideoInCtx.u32Width;
		sAVIFileAttr.u32FrameHeight = sVideoInCtx.u32Height;
		
		// modi by chenbh 2015-03-17 如果改为15帧则录制的时候会死机
		sAVIFileAttr.u32FrameRate = 30;//15;

		sAVIFileAttr.u32SamplingRate = DEF_AUDIO_SAMPLE_RATE;
		sAVIFileAttr.u32AudioFmtTag = ALAW_FMT_TAG;
		sAVIFileAttr.u32AudioChannel = 1;
		sAVIFileAttr.u32AudioFrameSize = i32AudioFrameSize;

		i32AVIFd = NewAVIFile(pchSaveFilename, &sAVIHandle, &sAVIFileAttr, &pu8AVIMem, u32RecTimeSec);
		if(i32AVIFd < 0)
		{
			i32Ret = -6;
			goto exit_prog;
		}
	}
	else
	{
		// Open wav file for audio only mode
		if(NewWavFile(pchSaveFilename, &sWavFileInfo) < 0)
		{
			i32Ret = -8;
			goto exit_prog;
		}
	}

#if _SAVE_RECV_VIDEO_
pRecordH264File = fopen(RECORD_H264_VIDEO, "wb");
#endif

#if _SAVE_RECV_AUDIO_
pRecordAlawRecFile = fopen(RECORD_ALAW_AUDIO, "wb");
#endif

	mblk_t video_arg;
	mblk_t audio_arg;
	uint8 video_echo = FALSE;
	uint8 audio_echo = FALSE;
	if (bAudioOnlyMode == FALSE)
	{
		g_get_Iframe = 0;
		while(f->thread.thread_run)
		{

			//Acquire packet from video list
			video_echo = ms_queue_get_chunk(f, &video_arg, 2);	

			audio_echo = ms_queue_get_chunk(f, &audio_arg, 1);			
			
			if (video_echo == FALSE && audio_echo == FALSE)	
			{
				usleep(10*1000);
				continue;
			}
			
			/*
			//Checking disk free space
			if(IsDiskSpaceEnough(pchSaveFolder) == false)
				break;
				*/
			if (g_get_Iframe == 0)
			{
				if (video_echo == TRUE)
				{
					if ((0x1f & video_arg.address[4]) == 0x07)
					{
						video_echo = TRUE;
						g_get_Iframe = 1;
						//get_h264_rect(int * witgh, int * heigth);
					}
					else
					{
						ms_queue_pop_chunk(f, 2);
						ms_queue_pop_chunk(f, 1);
						video_echo = FALSE;
						continue;
					}
				}
				else if (audio_echo == TRUE)
				{
					ms_queue_pop_chunk(f, 1);
					audio_echo = FALSE;
					continue;
				}
			}

			#if _SAVE_RECV_VIDEO_
			if (video_echo == TRUE && bAudioOnlyMode == FALSE)
			{
				Record_Recv_SaveFie(pRecordH264File, &video_arg);
			}
			#endif

			#if _SAVE_RECV_AUDIO_
			if (audio_echo == TRUE)
			{
				Record_Recv_SaveFie(pRecordAlawRecFile, &audio_arg);
			}
			#endif

			// Write to AVI file
			if(video_echo == TRUE && bAudioOnlyMode == FALSE)
			{
				AVIUtility_AddVideoFrame(&sAVIHandle, (uint8 *)video_arg.address, video_arg.len);
				ms_queue_pop_chunk(f, 2);
			}

			if(audio_echo == TRUE)
			{
				AVIUtility_AddAudioFrame(&sAVIHandle, (uint8 *)audio_arg.address, audio_arg.len);
				ms_queue_pop_chunk(f, 1);
			}
		}
	}
	else
	{
		while(f->thread.thread_run)
		{
			audio_echo = ms_queue_get_chunk(f, &audio_arg, 1);
			if (audio_echo == FALSE)	
			{
				usleep(10*1000);
				continue;
			}

			/*
			//Checking disk free space
			if(IsDiskSpaceEnough(pchSaveFolder) == false)
				break;
				*/

			#if _SAVE_RECV_AUDIO_
			if (audio_echo == TRUE)
			{
				Record_Recv_SaveFie(pRecordAlawRecFile, &audio_arg);
			}
			#endif
		
			if (audio_echo == TRUE)
			{
				WavFileUtil_Write_WriteData(&sWavFileInfo, audio_arg.address, audio_arg.len);
				ms_queue_pop_chunk(f, 1);
			}
		}
	}

	log_printf(" exit avirecord thread!!!!!!!!!!! \n");
	f->thread.thread = -1;

	if (bAudioOnlyMode == false)
	{
		// Finialize AVI file
		AVIUtility_FinalizeAVIFile(&sAVIHandle);
	}
	else
	{
		// Finnailiz Wav file
		WavFileUtil_Write_Finish(&sWavFileInfo);
	}

	fflush(fPListFile);
	fsync(fileno(fPListFile));

exit_prog:
	if (i32AVIFd > 0)
	{
		close(i32AVIFd);
	}
	if (pu8AVIMem)
	{
		free(pu8AVIMem);
	}
	
	pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NULL;
#endif
}
#endif

/*************************************************
  Function:		ms_avi_record_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_record_open(struct _MSMediaDesc * f, void * arg)
{
	int32 ret = HI_FAILURE;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{	
		
		ms_thread_init(&f->thread,20);	
		ret = ms_thread_create(&f->thread, ms_avirecord_thread, f);
		if(HI_SUCCESS != ret)
		{
			goto End_NSRecord;
		}
		else
		{
			f->mcount++;
		}
	}
	ms_media_unlock(f);
	return RT_SUCCESS;
	
End_NSRecord:
	log_printf(" ********** END ERROR :%d ********* \n ", ret);
	ms_media_unlock(f);
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_avi_record_close
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_record_close(struct _MSMediaDesc * f, void * arg)
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
			ms_thread_quit(&f->thread);
			ret = RT_SUCCESS;
		} 
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_avi_record_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_record_param(struct _MSMediaDesc * f, void * arg)
{
	RecordParam *s = (RecordParam *)(f->private);
	RecordParam * param = (RecordParam*)arg;
	if (arg == NULL)
	{
		return RT_FAILURE;
	}
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		memset(s, 0, sizeof(RecordParam));
		memcpy(s->filename, param->filename, sizeof(param->filename));
		s->AudioFormat = param->AudioFormat;
		s->RecordMode = param->RecordMode;
		s->VideoFormat = param->VideoFormat;
		ms_media_unlock(f);	
		return RT_SUCCESS;
	}
	ms_media_unlock(f);	
	
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_avi_record_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_record_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{

		RecordParam *data = (RecordParam *)malloc(sizeof(RecordParam));
		data->AudioFormat = PT_G711A;
		data->RecordMode = LWM_AUDIO_VIDEO;
		data->VideoFormat = PT_H264;
		memset(data->filename, 0, sizeof(data->filename));
		//data->filename = NULL;
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_avi_record_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_avi_record_uninit(struct _MSMediaDesc * f)
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
	{MS_AVI_RECORD_PARAM,	ms_avi_record_param},
	{MS_AVI_RECORD_OPEN,	ms_avi_record_open},
	{MS_AVI_RECORD_CLOSE,	ms_avi_record_close},
	{0, NULL}
};

MSMediaDesc ms_avi_record_desc =
{
	.id = MS_AVI_RECORD_ID,
	.name = "MsAviRecord",
	.text = "Avi Record",
	.enc_fmt = "avi",
	.ninputs = 2,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_avi_record_init,
	.uninit = ms_avi_record_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_UNBLOCK,
	.sharebufsize = 2000,							// 1024*sharebufsize
	.sharebufblk = 500,
	.private = NULL, 
};	

