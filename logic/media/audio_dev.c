/*************************************************
  Copyright (C), 2006-2016, 
  File name: 	audio_dev.h
  Author:		chenbh
  Version:  	1.0
  Date: 		2015-04-27
  Description:  alsa模块
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "audio_dev.h"
#include "AudioCore.h"

#define PLAYBLOCKS				4
#define RECORDBLOCKS			4
#define PLAYBAK_SYNC_MODE   	true  
#define RECORD_SYNC_MODE   		false  


#define PLAYBAK_DEVNAME			"default"
#define RECORD_DEVNAME			"default"


static alsa_info_t alsa_play_info = 
{
	.StreamId = 0,
	.callback = NULL,
	.status = ALSA_NOT_WORKING,
};

static alsa_info_t alsa_record_info = 
{
	.StreamId = 0,
	.callback = NULL,
	.status = ALSA_NOT_WORKING,
};

static int  alsaSinkDevErrcount =0;

/*************************************************
  Function:			Alsa_Record_Func
  Description:  	从设备读取数据
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
static int Alsa_Record_Func(char *DataBuffer, int DataSize, void *pContext)
{
	if(alsa_record_info.status == ALSA_CAPTURE_START && alsa_record_info.callback) 
	{			
		alsa_record_info.dwRecMuteCnt -= DataSize;
		if (alsa_record_info.dwRecMuteCnt < 0)
			alsa_record_info.dwRecMuteCnt = 0;
		else
			memset(DataBuffer, 0, DataSize);
		
		//printf("alsa_record_info.dwRecMuteCnt[%d]\n", alsa_record_info.dwRecMuteCnt);
		alsa_record_info.callback(DataBuffer, DataSize);
	}

	// 返回值不能为-1  否则会一直没有数据过来
	return DataSize;
}

/*************************************************
  Function:			Alsa_Record_Start
  Description:  	alsa读取设备初始化  
  Input: 
   1、Channel		通道数
   2、Bitwidth		位宽
   3、SampleRate	采样率
   4、BlockTime		块处理时间
   5、Func			回调函数
  Output:		  
  Return:		 
  Others:  
*************************************************/
int Alsa_Record_Start(int Channel, int Bitwidth, int SampleRate, int BlockTime, Audiofun Func)
{
	if ( alsa_record_info.status != ALSA_NOT_WORKING )
	{
		printf(" CX_Alsa_Record_Init status != ALSA_NOT_WORKING \n");
		return 0;
	}

	int  SamplesPerBlock = SampleRate * BlockTime/1000;
	int StreamID = Audio_Recorder_Open();
	if (StreamID == 0)
		return -1;

	// 前面的丢弃
	printf("SampleRate[%d], Channel[%d] ,Bitwidth[%d], SamplesPerBlock[%d]\n", SampleRate, Channel ,Bitwidth, SamplesPerBlock);
	alsa_record_info.dwRecMuteCnt = SampleRate * Channel * (Bitwidth/8) * 3/2;
	Audio_Recorder_SetDevice(StreamID, RECORD_DEVNAME);
	Audio_Recorder_SetFormat(StreamID, Channel, Bitwidth, SampleRate);
	Audio_Recorder_SetCache (StreamID, RECORDBLOCKS, SamplesPerBlock);
	Audio_Recorder_SetMode	(StreamID, RECORD_SYNC_MODE, Alsa_Record_Func, &alsa_record_info);
	
	if (Audio_Recorder_Start(StreamID) == false)
	{
		Audio_Recorder_Close(StreamID);
		StreamID = 0;
		printf("Audio_Recorder_Start FAILED !!!\n");
		return -1;
	}

	alsa_record_info.StreamId = StreamID;
	alsa_record_info.callback = Func;
	alsa_record_info.status = ALSA_CAPTURE_START;
	pthread_mutex_init(&alsa_record_info.lock, NULL);
	return 0;
	
init_record_error:
	
	return -1;
}

/*************************************************
  Function:			CX_Alsa_Read_Close
  Description:  	关闭读设备
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int	Alsa_Record_Close(void)
{
	pthread_mutex_lock(&alsa_record_info.lock);
	if(alsa_record_info.status == ALSA_CAPTURE_START) 
	{
		alsa_record_info.status = ALSA_NOT_WORKING;
		Audio_Recorder_Stop(alsa_record_info.StreamId);
		Audio_Recorder_Close(alsa_record_info.StreamId);
		alsa_record_info.StreamId = 0;
		alsa_record_info.callback = NULL;
		printf("===== CX_Alsa_Source_Close ======\n");
	}
  	pthread_mutex_unlock(&alsa_record_info.lock);
	pthread_mutex_destroy(&alsa_record_info.lock);
  	return 0;
}

/*************************************************
  Function:			Alsa_Record_SetVolume
  Description:  	设置咪头音量
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int Alsa_Record_SetVolume(int volume)
{

}



/*************************************************
  Function:			Alsa_System_SetVolume
  Description:  	设置功放音量
  Input: 
  	volume			范围值待定
  Output:		  
  Return:		 
  Others:  			
*************************************************/
int Alsa_System_SetVolume(int volume)
{
	volume = 0xFFFFFFFF;
	AudioCore_SetPlayVolume((volume & 0xffff) * 100/0xffff);
}

/*************************************************
  Function:			Alsa_Play_SetVolume
  Description:  	设置喇叭音量
  Input: 
  	volume			0-100
  Output:		  
  Return:		 
  Others:  			
*************************************************/
int Alsa_Play_SetVolume(int volume)
{
	if (alsa_play_info.status != ALSA_PLAYBAK_START)
	{
		printf(" CX_Alsa_Play_Init status != ALSA_PLAYBAK_START \n");
		return 0;
	}
	
	Audio_Player_SetVolume(alsa_play_info.StreamId, volume);	
}

/*************************************************
  Function:			Alsa_Play_Func
  Description:  	向alsa设备写入数据 播放
  Input: 
   1、data			数据指针
   2、size			数据大小
  Output:		  
   1、status		返回值
  Return:		 
  Others:  
*************************************************/
int	Alsa_Play_Func(void * Data, int size, int* status)
{
	int frameSize;																																																														
	int written;																																																														
  	int totalBuffer;																																																														
  	int offsetBuffer;																																																														
	int allDataSent=0;		

	pthread_mutex_lock(&alsa_play_info.lock);
	if(alsa_play_info.status == ALSA_PLAYBAK_START) 
	{			
		*status = ALSA_NO_ERROR;	
		Audio_Player_Write(alsa_play_info.StreamId, Data, size, (alsa_play_info.BlockTime)*PLAYBLOCKS);				
	}
	
	pthread_mutex_unlock(&alsa_play_info.lock);
	return size;
}

/*************************************************
  Function:			Alsa_Play_Start
  Description:  	alsa写设备初始化 播放初始化
  Input: 
   1、Channel		通道数
   2、Bitwidth		位宽
   3、SampleRate	采样率
   4、BlockTime		块处理时间
  Output:		  
  Return:		 
  Others:  
*************************************************/
int	Alsa_Play_Start(int Channel, int Bitwidth, int SampleRate, int BlockTime)
{
	if (alsa_play_info.status != ALSA_NOT_WORKING)
	{
		printf(" CX_Alsa_Play_Init status != ALSA_NOT_WORKING \n");
		return 0;
	}

	int Volume = 100;
	int  SamplesPerBlock = SampleRate * BlockTime/1000;
	alsa_play_info.Channel = Channel;
	alsa_play_info.Bitwidth = Bitwidth;
	alsa_play_info.SampleRate = SampleRate;
	alsa_play_info.BlockTime = BlockTime;	
		
	int StreamID = Audio_Player_Open();
	if (StreamID == 0)
		return -1;
	
	Audio_Player_SetDevice(StreamID, PLAYBAK_DEVNAME);
	Audio_Player_SetFormat(StreamID, alsa_play_info.Channel, alsa_play_info.Bitwidth, SampleRate);
	Audio_Player_SetCache (StreamID, PLAYBLOCKS, SamplesPerBlock);
	//Audio_Player_SetMode  (StreamID, AUDIO_SYNC_MODE, waveOutProc, this);
	Audio_Player_SetMode  (StreamID, PLAYBAK_SYNC_MODE, NULL, NULL);
	Audio_Player_SetVolume(StreamID, Volume);

	if (Audio_Player_Start(StreamID) == false)
	{
		Audio_Player_Close(StreamID);
		StreamID = 0;
		printf("Audio_Player_Start FAILED !!!\n");
		return -1;
	}

	alsa_play_info.StreamId = StreamID;	
	alsa_play_info.status = ALSA_PLAYBAK_START;
	pthread_mutex_init(&alsa_play_info.lock, NULL);
	return 0;
	
init_palyback_error:
	
	return -1;
	
}


/*************************************************
  Function:			CX_Alsa_Write_Close
  Description:  	关闭写进程
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int	Alsa_Play_Close(void)
{
	printf("===== CX_Alsa_Sink_Close [%d] ======\n",alsa_play_info.status);

	pthread_mutex_lock(&alsa_play_info.lock);
	if(alsa_play_info.status == ALSA_PLAYBAK_START) 
	{
		alsa_play_info.status = ALSA_NOT_WORKING;
		Audio_Recorder_Stop(alsa_play_info.StreamId);
		Audio_Recorder_Close(alsa_play_info.StreamId);
		alsa_play_info.StreamId = 0;
	}
	pthread_mutex_unlock(&alsa_play_info.lock);
	
	printf("===== CX_Alsa_Sink_Close [%d] end ======\n",alsa_record_info.status);
	
	pthread_mutex_destroy(&alsa_play_info.lock);
  	return 0;
}


