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
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <locale.h>
#include <assert.h>
#include <sys/poll.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <pthread.h>

#ifndef	_AUDIO_DEV_H_
#define	_AUDIO_DEV_H_

typedef void (*Audiofun)(char *DataBuffer, int DataSize);


typedef struct
{
	int StreamId;
	int SampleRate;
	int Bitwidth;
	int Channel;
	int BlockTime;				// 块处理超时时间
	int dwRecMuteCnt;			// 丢弃的数据大小 在capture前面的数据丢弃
	int status;
	Audiofun callback;
	pthread_mutex_t lock;					
}alsa_info_t,*palsa_info_t;

typedef enum {
	ALSA_NO_ERROR 		= 0,
	ALSA_WRBUF_UNDERRUN = -1,
	ALSA_WRBUF_SENDFAIL = -2,
	ALSA_RDBUF_RECVFAIL = -3
} AlsaErrNum;

typedef enum {
	ALSA_NOT_WORKING = 0,
	ALSA_PLAYBAK_START,
	ALSA_CAPTURE_START
} AlsaStatusNum;

typedef enum {
	ALSA_MODE_RECORD,
	ALSA_MODE_PLAY
} AlSA_MODE_E;




#ifdef __cplusplus
extern "C" {
#endif


/*************************************************
  Function:			Alsa_Play_SetVolume
  Description:  	设置喇叭音量
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int Alsa_Play_SetVolume(int volume);

/*************************************************
  Function:			Alsa_Record_SetVolume
  Description:  	设置咪头音量
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int Alsa_Record_SetVolume(int volume);

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
int Alsa_Record_Start(int Channel, int Bitwidth, int SampleRate, int BlockTime, Audiofun Func);

/*************************************************
  Function:			CX_Alsa_Read_Close
  Description:  	关闭读设备
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int	Alsa_Record_Close(void);

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
int	Alsa_Play_Func(void * Data, int size, int* status);

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
int	Alsa_Play_Start(int Channel, int Bitwidth, int SampleRate, int BlockTime);

/*************************************************
  Function:			CX_Alsa_Write_Close
  Description:  	关闭写进程
  Input: 
  Output:		  
  Return:		 
  Others:  
*************************************************/
int	Alsa_Play_Close(void);
#ifdef __cplusplus
}
#endif


#endif

