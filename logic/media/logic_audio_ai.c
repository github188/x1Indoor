/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_ai.c
  Author:		罗发禄
  Version:		V2.0
  Date: 		2014-04-01
  Description:	音频输入程序
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#include "logic_audio_ai.h"
#include "logic_media.h"
#include "audio_dev.h"

#define	BLOCKTIME         		20//40     //每个缓冲数据块的时长（毫秒）
#define VOICE_ZOOM				0
#define AI_DEV_CHANNELS			1
#define AI_DEV_BITWIDTH			AIO_BIT_WIDTH_16
#define AI_DEV_SAMPLERATE		AIO_SAMPLE_RATE_8

#define _AUDIO_AI_SAVE_			0
#if _AUDIO_AI_SAVE_
#define AI_PCM_AUDIO			0
#define AI_AEC_AUDIO			1
#define AI_PCM_AUDIO_FILE		CFG_PUBLIC_DRIVE"/mic_ai.pcm"
#define AEC_PCM_AUDIO_FILE		CFG_PUBLIC_DRIVE"/ai_aec.pcm"
static FILE * aiAudioFIle = NULL;
static FILE * aiAecFIle = NULL;

/*************************************************
  Function:		g711_file_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void g711_ai_file_close(uint8 mode)
{
	switch (mode)
	{
		case AI_PCM_AUDIO:
			if (aiAudioFIle)
			{
				fclose(aiAudioFIle);
				aiAudioFIle = NULL;
			}
			break;
			
		case AI_AEC_AUDIO:
			if (aiAecFIle)
			{
				fclose(aiAecFIle);
				aiAecFIle = NULL;
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
static void g711_ai_file_open(uint8 mode)
{
	g711_ai_file_close(mode);
	switch (mode)
	{
		case AI_PCM_AUDIO:
			aiAudioFIle = fopen(AI_PCM_AUDIO_FILE, "wb");
			break;
			
		case AI_AEC_AUDIO:
			aiAecFIle = fopen(AEC_PCM_AUDIO_FILE, "wb");
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
static void g711_ai_file_save(uint8 mode, uint8 *data, int len)
{
	switch (mode)
	{
		case AI_PCM_AUDIO:
			if (aiAudioFIle != NULL)
			{
				fwrite(data, len, 1, aiAudioFIle);
				fflush(aiAudioFIle);
			}
			break;
			
		case AI_AEC_AUDIO:
			if (aiAecFIle != NULL)
			{
				fwrite(data, len, 1, aiAecFIle);
				fflush(aiAecFIle);
			}
			break;
			
		default:
			break;
	}
}
#endif

static int st_audio_send_data(char *DataBuffer, int DataSize)
{
	AudioAIParam * mAudio = (AudioAIParam *)ms_audio_ai_desc.private;	
	if (mAudio->Enable == FALSE)
	{
		log_printf("s->Enable == false \n");
		return 0;
	}
	
	//static uint8 aipcmbuf[322];
	int deal_len = AI_DEV_CHANNELS * (AI_DEV_BITWIDTH/8) * (AI_DEV_SAMPLERATE/1000) * BLOCKTIME;
	static uint8 aipcmbuf[AI_DEV_CHANNELS * (AI_DEV_BITWIDTH/8) * (AI_DEV_SAMPLERATE/1000) * BLOCKTIME*2];
	memset(aipcmbuf, 0, sizeof(aipcmbuf));
	//log_printf("size: %d, deal_len[%d]\n", DataSize, deal_len);
	
	#if _AUDIO_AI_SAVE_
	g711_ai_file_save(AI_PCM_AUDIO, DataBuffer, DataSize);
	#endif

	#if (_AEC_TYPE_ == _SW_AEC_)
		//by zxf
		if (mAudio->AecEnable == TRUE)
		{
			echo_cancel_exe2((short *)DataBuffer, (short *)aipcmbuf);
		}	
		else
			memcpy(aipcmbuf, DataBuffer, deal_len);
		
		#if VOICE_ZOOM 								// 消完回声后送到访客端的数据进行放大
		//pcm_audio_mult_int(aipcmbuf, deal_len, 16, 1);
		if (fabs(mAudio->AiAgc-1.0) > 0.000001)
		{
			//log_printf("s->EncParam.AiAgc: %f \n", s->EncParam.AiAgc);
			pcm_audio_mult((unsigned char*)aipcmbuf, deal_len, AI_DEV_BITWIDTH, mAudio->EncParam.AiAgc);
		}
		#endif
	#else
		memcpy(aipcmbuf, DataBuffer, deal_len);
		#if VOICE_ZOOM 								
		//pcm_audio_mult_int(aipcmbuf, deal_len, 16, 1);
		if (fabs(mAudio->AiAgc-1.0) > 0.000001)
		{
			//log_printf("s->EncParam.AiAgcH: %f \n", s->EncParam.AiAgc);
			pcm_audio_mult((unsigned char*)aipcmbuf, deal_len, AI_DEV_BITWIDTH, mAudio->AiAgc);
		}
		#endif
	#endif
	
	// aec.pcm
	#if _AUDIO_AI_SAVE_
	g711_ai_file_save(AI_AEC_AUDIO, (uint8 *)aipcmbuf, DataSize);
	#endif
		
	mblk_t arg;
	ms_media_blk_init(&arg);
	arg.address = (char *)aipcmbuf;
	arg.len = deal_len;
	arg.id = MS_FILTER_NOT_SET_ID;
	ms_media_process(&ms_audio_ai_desc, &arg);
	//audio_frame_check(s, arg.len/2);	// 没什么效果	
	//get_timeofday();
	//ms_media_process_by_id(mMediaStream.VideoEnc, &arg);
	return DataSize;
}

/*************************************************
  Function:		st_audio_ai_start
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int st_audio_ai_start(struct _MSMediaDesc * arg)
{
	#if _AUDIO_AI_SAVE_
    g711_ai_file_open(AI_PCM_AUDIO);
	g711_ai_file_open(AI_AEC_AUDIO);
	#endif	
	AudioAIParam * data = (AudioAIParam*)arg->private;
	return Alsa_Record_Start(AI_DEV_CHANNELS, AI_DEV_BITWIDTH, AI_DEV_SAMPLERATE, BLOCKTIME, st_audio_send_data);
}

/*************************************************
  Function:		st_audio_ai_stop
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int st_audio_ai_stop(struct _MSMediaDesc * arg)
{
	#if _AUDIO_FILE_SAVE_
	g711_file_close(AI_PCM_AUDIO);
	g711_file_close(AI_AEC_AUDIO);
	#endif

	AudioAIParam * data = (AudioAIParam*)arg->private;
	data->Enable = FALSE;	
	Alsa_Record_Close();
}

/*************************************************
  Function:		st_audio_param_reset
  Description:	
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static int st_audio_param_reset(AudioAIParam * data)
{	
	ms_return_val_if_fail(data, -1);	
	memset(data, 0, sizeof(AudioAIParam));
	data->Enable = TRUE;
	data->AecEnable = TRUE;
	data->AiAgc = 1.0;
	return 0;
}

/*************************************************
  Function:		ms_audio_ai_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * ms_audio_ai_thread(void *ptr)
{
	int s32Ret;
	int offtime;
	struct timeval t1, t2, t3, t4;
	MSMediaDesc * f = (MSMediaDesc *)ptr;
	AudioAIParam * s = (AudioAIParam *)f->private;
	uint8 * aipcmbuf = (uint8 *)malloc(641);
	#if (_AEC_TYPE_ == _SW_AEC_)
	uint8 cleanbuf[322];
	#endif
	
	memset(aipcmbuf, 0, sizeof(aipcmbuf));
	log_printf("thread_run: %d,Enable: %d", s->aithread.thread_run , s->Enable);
    while (s->aithread.thread_run && s->Enable)
    {		   
		
		gettimeofday(&t1, NULL);	
		//s32Ret = ext_cap_audio_from_card(aipcmbuf);		
		//s32Ret = CX_Alsa_Record_Func((void *)aipcmbuf, 160, &s32Ret);
		if (s32Ret <= 0) 
		{
			usleep(100);
			continue;
		}

	
		// mic
		#if _AUDIO_AI_SAVE_
		//log_printf("s32Ret : %d\n", s32Ret);
		g711_ai_file_save(AI_PCM_AUDIO, (uint8 *)aipcmbuf, s32Ret);
		#endif
		
		#if (_AEC_TYPE_ == _SW_AEC_)
			//by zxf
			if (s->AecEnable == TRUE)
			{
				echo_cancel_exe2((short *)aipcmbuf, (short *)cleanbuf);
				memcpy(aipcmbuf, cleanbuf, 320);		// 固定按320 字节做消回音
			}		
			#if VOICE_ZOOM 								// 消完回声后送到访客端的数据进行放大
			//pcm_audio_mult_int(aipcmbuf, s32Ret, 16, 2);
			//pcm_audio_mult_int(aipcmbuf, s32Ret, 16, 1);
			#endif
		#else
			#if VOICE_ZOOM								// 咪头输入的信号进行缩小,便于回声抵消
			pcm_audio_mult((unsigned char*)aipcmbuf, s32Ret, AI_DEV_BITWIDTH, s->AiAgc);
			#endif
		#endif
		
		// aec.pcm
		#if _AUDIO_AI_SAVE_
		g711_ai_file_save(AI_AEC_AUDIO, (uint8 *)aipcmbuf, s32Ret);
		#endif
		
	
		mblk_t arg;
		ms_media_blk_init(&arg);
		arg.address =(char *)aipcmbuf;
		arg.len = s32Ret;
		arg.id = MS_FILTER_NOT_SET_ID;
		ms_media_process(f, &arg);
		
		gettimeofday(&t4, NULL);
		offtime = ((t4.tv_sec-t1.tv_sec)*1000000 + (t4.tv_usec-t1.tv_usec));
		if (offtime < 18*1000)
		{
			usleep(18*1000 - offtime);
		}	
    }
	
	#if _AUDIO_AI_SAVE_
    g711_ai_file_close(AI_PCM_AUDIO);
	g711_ai_file_close(AI_AEC_AUDIO);
	#endif	
    s->aithread.thread = -1;
	free(aipcmbuf);
	//alsa_read_uninit(f);
	//CX_Alsa_Record_Close();
	pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NULL;
}

/*************************************************
  Function:		ms_audio_ai_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		AudioAIParam * data = (AudioAIParam*)malloc(sizeof(AudioAIParam));
		st_audio_param_reset(data);
		
		f->private= data;		
		f->mcount = 0;
	}
	
	return 0;
}

/*************************************************
  Function:		ms_enc_h264_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}
 
/*************************************************
  Function:		ms_audio_ai_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = HI_FAILURE;
	AudioAIParam * data = (AudioAIParam*)f->private;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{	
		ret = st_audio_ai_start(f);
		if (HI_SUCCESS != ret)
		{
			goto error0;
		}
		f->mcount++;
		
		data->Enable = TRUE;	// by zxf
		/*
		ms_thread_init(&data->aithread, 100);	
		ret = ms_thread_create(&data->aithread, ms_audio_ai_thread, f);
		if (HI_SUCCESS != ret)
		{
			goto error1;
		}
		else
		{
			f->mcount++;
		}
		*/
	}
	else
	{
		f->mcount++;
	}
	ms_media_unlock(f);
	
	return ret;

error0:
error1:
	st_audio_ai_stop(f);
	
//error0:	
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_audio_ai_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = HI_FAILURE;
	AudioAIParam * data = (AudioAIParam*)f->private;
	
	data->Enable = FALSE;
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			//ms_thread_quit(&data->aithread);	
			st_audio_param_reset(data);
			st_audio_ai_stop(f);
			ret = HI_SUCCESS;
		} 
	}
	ms_media_unlock(f);

	return ret;
}

/*************************************************
  Function:		ms_audio_ai_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_param(struct _MSMediaDesc * f, void * arg)
{
	AudioAIParam * data = (AudioAIParam*)f->private;
	AUDIO_PARAM *param = ((AUDIO_PARAM*)arg);

	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		data->AiAgc = param->AiAgc;
		log_printf("data->AiAgc[%f]\n", data->AiAgc);
		ms_media_unlock(f);	
		return HI_SUCCESS;
	}
	
	ms_media_unlock(f);	
	return HI_FAILURE;
}

/*************************************************
  Function:		ms_audio_ai_enable
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_enable(struct _MSMediaDesc * f, void * arg)
{
	AudioAIParam * data = (AudioAIParam*)f->private;
	bool Enable = *(bool *)(arg);
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{		
		data->Enable = Enable;		
		ms_media_unlock(f);	
		return HI_SUCCESS;
	}
	ms_media_unlock(f);
	
	return HI_FAILURE;
}

/*************************************************
  Function:		ms_audio_ai_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_ai_aec_enable(struct _MSMediaDesc * f, void * arg)
{
	AudioAIParam * data = (AudioAIParam*)f->private;
	bool AecEnable = *(bool *)(arg);
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{		
		data->AecEnable = AecEnable;
		if(AecEnable == TRUE)
			AudioCore_SetAecMode(true);
		else
			AudioCore_SetAecMode(false);
		ms_media_unlock(f);	
		return HI_SUCCESS;
	}
	ms_media_unlock(f);
	
	return HI_FAILURE;
}

static MSMediaMethod methods[] =
{
	{MS_AUDIO_AI_OPEN,	ms_audio_ai_open},
	{MS_AUDIO_AI_CLOSE,	ms_audio_ai_close},
	{MS_AUDIO_AI_PARAM, ms_audio_ai_param},
	{MS_AUDIO_AI_ENABLE, ms_audio_ai_enable},
	{MS_AUDIO_AI_AEC_ENABLE, ms_audio_ai_aec_enable},
	{0,					NULL}
};

MSMediaDesc ms_audio_ai_desc = 
{
	.id = MS_AUDIO_AI_ID,
	.name = "MsAudioAi",
	.text = "Audio AI using N32926 Hw",
	.enc_fmt = "PCM",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_audio_ai_init,
	.uninit = ms_audio_ai_uninit,
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

