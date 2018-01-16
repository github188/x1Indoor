
/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_audio_play.c
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2015-04-29
  Description:  WAV文件播放
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "audio_dev.h"
#include "audio_endec.h"
#include "wav_play.h"
#include "logic_wav_play.h"

#define	BLOCKTIME         20     //每个缓冲数据块的时长（毫秒）
#define CHUNK_SIZE		  160			

typedef struct _read_buf
{
    int iput;       			// the position of put
    int iget;      	 			// the position of get
    int size;
    unsigned char * buffer;
}File_ReadBuf, *pFile_ReadBuf;

static File_ReadBuf *g_ReadBuf = NULL;
static short int g_Format_tag;	// wav 音频数据格式 01-pcm 06-alaw 07-ulaw
static unsigned int g_ChunkSize = CHUNK_SIZE;

/*************************************************
  Function:			get_AudioParam
  Description:		获取音频参数
  Input: 	
  Output:			AudioParam
  Return:			无
  Others:
*************************************************/
static int get_AudioParam(AudioPlayState *s)
{
	int ret = RT_FAILURE;
	AUDIOPLAY_PARAM *PlayParam = &(s->AudioPlay_Param);
	WAV_AUDIO_PARAM *audio_param = &(s->audio_param);

	log_printf("FileType : %x \n", s->AudioPlay_Param.FileType);
	switch (s->AudioPlay_Param.FileType)
	{
		case FILE_WAVE:
			{
				ret = wav_play_init(PlayParam->filename);
				if (ret == 0)
				{
					g_ReadBuf = (File_ReadBuf *)wave_buf;
					g_Format_tag = wave_header.format_tag;					
					audio_param->bit_width = wave_header.bits_per_sample;
					audio_param->channels  = wave_header.channels;
					audio_param->rate = wave_header.sample_rate;					
				}
			}
			break;

		case FILE_MP3:
			break;

		default:
			break;
	}

	log_printf("bit_width : %d, channels : %d, rate: %d\n", audio_param->bit_width, audio_param->channels, audio_param->rate);
	if (ret == 0)
	{
		log_printf("g_ReadBuf->size : %d, g_ReadBuf->iget: %d\n", g_ReadBuf->size, g_ReadBuf->iget);
	}
	return ret;
}

/*************************************************
  Function:		audio_play_param_reset
  Description:	
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static int audio_play_param_reset(AudioPlayState *data)
{	
	ms_return_val_if_fail(data, -1);
	
	memset(data, 0, sizeof(AudioPlayState));		
	data->Play_State = AUDIO_STATE_STOP;

	data->audio_param.channels = 1;
	data->audio_param.bit_width = AIO_BIT_WIDTH_16;
	data->audio_param.rate = AIO_SAMPLE_RATE_8;
	
	data->AudioPlay_Param.IsRepeat = FALSE;
	data->AudioPlay_Param.FileType = FILE_NONE;
	data->AudioPlay_Param.callback = NULL;

	data->msplaythread.thread = -1;
	data->msplaythread.thread_run = 0;
	g_ChunkSize = CHUNK_SIZE;
	return 0;
}

/*************************************************
  Function:			lyly_err_callback
  Description:		错误回调
  Input: 	
  	1.f				
  	2.percent		播放百分比*100 小于0 表示出错
  	3.time			文件总时长
  	4.cmd			0 失败回调 其他正常
  Output:			无
  Return:			无
  Others:
  (int cmd, int time, int percent)
*************************************************/
void ms_audio_play_callback(struct _MSMediaDesc * f, int cmd, int time, int percent)
{
	AudioPlayState *s = (AudioPlayState *)f->private;	
	if (s->AudioPlay_Param.callback)
	{
		s->AudioPlay_Param.callback(cmd, time, percent);
	}
}

/*************************************************
  Function:		ms_audio_play_start
  Description: 	 
  Input: 	
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_start(AudioPlayState* s)
{
	int  Bitwidth = 16;
	int  Volume = 100;
	int  StreamID = 0;
	int  Channels = s->audio_param.channels;
	if (g_Format_tag == AUDIO_A_LAW || g_Format_tag == AUDIO_U_LAW)
	{
		Bitwidth = s->audio_param.bit_width*2;
	}
	else
	{
		Bitwidth = s->audio_param.bit_width;
	}
	int  SampleRate = s->audio_param.rate;
	return Alsa_Play_Start(Channels, Bitwidth, SampleRate, BLOCKTIME);	
}

/*************************************************
  Function:		ms_avi_play_stop
  Description: 	 
  Input: 	
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_audio_play_stop(struct _MSMediaDesc * f)
{
	log_printf("****************** ms_audio_play_stop *******************\n");
	AudioPlayState *data = (AudioPlayState*)f->private;
	f->mcount = 0;
	switch (data->AudioPlay_Param.FileType)
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
	Alsa_Play_Close();
	data->Play_State = AUDIO_STATE_STOP;
	data->msplaythread.thread = -1;
	audio_play_param_reset(data);
	usleep(1000);
}

/*************************************************
  Function:		ms_avi_play_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void* ms_audio_play_thread(void *arg)
{
	ms_return_val_if_fail(arg, NULL);
	int state = 0;
	MSMediaDesc *f = (MSMediaDesc *)arg;
	AudioPlayState *data = (AudioPlayState*)f->private;	
	uint8 IsRepeat = data->AudioPlay_Param.IsRepeat;
	
	int  SamplesPerBlock = (data->audio_param.rate)*BLOCKTIME/1000;	// 320
	g_ChunkSize = SamplesPerBlock * (data->audio_param.channels) * ((data->audio_param.bit_width) / 8);
	log_printf("SamplesPerBlock[%d], g_ChunkSize[%d]\n", SamplesPerBlock, g_ChunkSize);
	
	unsigned char *alaw_buf = malloc(g_ChunkSize);
	unsigned char *ulaw_buf = malloc(g_ChunkSize);
	unsigned char *pcm_buf = malloc(g_ChunkSize);
	int audio_len = g_ChunkSize;
	int audio_chunk = g_ChunkSize;
	if (g_Format_tag == AUDIO_A_LAW || g_Format_tag == AUDIO_U_LAW)
	{
		audio_chunk = g_ChunkSize/2;
	}
	else
	{
		audio_chunk = g_ChunkSize;
	}

	// 将时间信息回调给应用
	int PreCallbakSize = 0;	// 上次回调时播放的长度
	int PerSecPlaySize = ((data->audio_param.rate) * (data->audio_param.channels) * ((data->audio_param.bit_width) / 8));
	int WavFilePlayTime = (g_ReadBuf->size)/((data->audio_param.rate) * (data->audio_param.channels) * ((data->audio_param.bit_width) / 8));
	ms_audio_play_callback(f, 1, WavFilePlayTime, 0);
	
	while (data->msplaythread.thread_run)
	{
		if (AUDIO_STATE_PAUSE == data->Play_State)
		{
			memset(pcm_buf, 0, g_ChunkSize);
			Alsa_Play_Func((void *)pcm_buf, g_ChunkSize, &state);
			//Audio_Player_Write(data->StreamID, (void *)pcm_buf, g_ChunkSize, BLOCKTIME*PLAYBLOCKS);
			usleep(500);
			continue;
		}
		
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
			memset(pcm_buf, 0, g_ChunkSize);
			memcpy(alaw_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);
			G711Decoder((short *)pcm_buf, alaw_buf, audio_len, 0);			
		}
		else if (g_Format_tag == AUDIO_U_LAW)
		{
			memset(ulaw_buf, 0, g_ChunkSize);
			memset(pcm_buf, 0, g_ChunkSize);
			memcpy(ulaw_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);
			G711Decoder((short *)pcm_buf, ulaw_buf, audio_len, 1);
		}
		else
		{
			memset(pcm_buf, 0, g_ChunkSize);
			memcpy(pcm_buf, (g_ReadBuf->buffer+g_ReadBuf->iget), audio_len);			
		}

		Alsa_Play_Func((void *)pcm_buf, g_ChunkSize, &state);
		
		if(g_ReadBuf->iget >= g_ReadBuf->size)
		{
		    if(!IsRepeat)
			{
		        ms_audio_play_callback(f, 1, WavFilePlayTime, 100);
				break;
		    } 
			else 
		    {
		        g_ReadBuf->iget = 0;
		    }
		}  
		else
		{
			// 大概没0.5s会上报一次
			if ((g_ReadBuf->iget - PreCallbakSize) >= (PerSecPlaySize/2))
			{
				PreCallbakSize = g_ReadBuf->iget;
				ms_audio_play_callback(f, 1, WavFilePlayTime, ((g_ReadBuf->iget)*100)/g_ReadBuf->size);
			}
		}

		//log_printf("audio_len[%d]\n", audio_len);
		g_ReadBuf->iget += audio_len;
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
	printf(" ms_audio_play_thread exit!!! \n");
	ms_audio_play_stop(f);	
	data->msplaythread.thread = -1;
	pthread_detach(pthread_self());
    pthread_exit(NULL);
}

/*************************************************
  Function:		ms_wav_play_control
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_control(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);
	
	AudioPlayState *s = (AudioPlayState*)f->private;	
	AUDIO_PLAY_STATE_E state = (AUDIO_PLAY_STATE_E)arg; 
	log_printf("state : %d \n", state);
	
	ms_media_lock(f);	
	switch (state)
	{
		case AUDIO_STATE_START:					// 播放和暂停的切换	
		case AUDIO_STATE_PAUSE:
			{
				if (s->Play_State == AUDIO_STATE_START)
				{
					s->Play_State = AUDIO_STATE_PAUSE;
					ret = RT_SUCCESS;
				}
				else if (s->Play_State == AUDIO_STATE_PAUSE)
				{
					s->Play_State = AUDIO_STATE_START;
					ret = RT_SUCCESS;
				}
				else
				{
					ret = RT_FAILURE;
				}
			}
			break;

		case AUDIO_STATE_STOP:				// 停止播放
			{
				//s->Play_State = AUDIO_STATE_STOP;
			}
			break;
			
		default:
			ret = RT_FAILURE;
			break;
	}
	ms_media_unlock(f);	
	
	return ret;
}

/*************************************************
  Function:		ms_wav_play_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	AudioPlayState *s = (AudioPlayState*)f->private;
	WAV_AUDIO_PARAM *audio_param = &(s->audio_param);
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

		ret = ms_audio_play_start(s);
		if (ret == -1)
		{
			goto error;
		}
		
		// 开启播放线程
		ms_thread_init(&s->msplaythread,1000);	
		ret = ms_thread_create(&s->msplaythread, ms_audio_play_thread, f);		
		if (ret == RT_FAILURE)
		{	
			goto error;
		}
		else
		{
			s->Play_State = AUDIO_STATE_START;
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
	s->Play_State = AUDIO_STATE_STOP;
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
  Function:		ms_wav_play_close
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_close(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	AudioPlayState * data = (AudioPlayState*)f->private;
	log_printf("****************** ms_audio_play_close *******************\n");
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount--;
		if (f->mcount == 0)
		{
			data->Play_State = AUDIO_STATE_STOP;
			ms_thread_quit(&data->msplaythread);
			ret = RT_SUCCESS;
		}
	}
	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_wav_play_level
  Description: 	 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_param(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	ms_return_val_if_fail(arg, -1);
	
	AudioPlayState *s = (AudioPlayState*)f->private;	
	AUDIOPLAY_PARAM *AudioParam = (AUDIOPLAY_PARAM *)arg;
	if (AudioParam->filename == NULL ||
		AudioParam->FileType != FILE_WAVE)			// 暂时只支持WAV 的播放
	{
		return RT_FAILURE;
	}
	log_printf("filename: %s, is_loop: %d, FileType: %d\n", AudioParam->filename, AudioParam->IsRepeat, AudioParam->FileType);
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
	   memcpy(&s->AudioPlay_Param, AudioParam, sizeof(AUDIOPLAY_PARAM));
	   ret = RT_SUCCESS;
	}
	ms_media_unlock(f);	
	return ret;
}

/*************************************************
  Function:		ms_wav_play_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		AudioPlayState *data = (AudioPlayState*)malloc(sizeof(AudioPlayState));	
		audio_play_param_reset(data);
		
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_wav_play_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_audio_play_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		AudioPlayState *data = f->private;
		ms_free(data);
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}

static MSMediaMethod methods[] =
{
	{MS_AUDIO_PLAY_CONTROL,	ms_audio_play_control},
	{MS_AUDIO_PLAY_PARAM, 	ms_audio_play_param},
	{MS_AUDIO_PLAY_OPEN,	ms_audio_play_open},
	{MS_AUDIO_PLAY_CLOSE,	ms_audio_play_close},
	{0, NULL}
};

MSMediaDesc ms_file_player_desc =
{
	.id = MS_FILE_PLAYER_ID,
	.name = "MsAudioPlay",
	.text = "AUDIO Play",
	.enc_fmt = "audio",
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_audio_play_init,
	.uninit = ms_audio_play_uninit,
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


