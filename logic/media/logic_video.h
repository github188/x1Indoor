/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_video.h
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2014-12-11
  Description:  视频处理
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _AU_VIDEO_H_
#define _AU_VIDEO_H_

#include <linux/fb.h>
#include "logic_include.h"
#include "logic_media_core.h"

#define 	RT_SUCCESS  			0
#define	 	RT_FAILURE   			-1

#define		H264VER          		0x00010000
#define 	H264VER_MAJOR    		(H264VER>>16)
#define 	H264VER_MINOR    		(H264VER&0xffff)

#define 	DISPLAY_MODE_RGB555     0
#define 	DISPLAY_MODE_RGB565     1
#define 	DISPLAY_MODE_RGB888_MODE0     2
#define 	DISPLAY_MODE_RGB888_MODE1     3
#define 	DISPLAY_MODE_CBYCRY     4
#define 	DISPLAY_MODE_YCBYCR     5
#define 	DISPLAY_MODE_CRYCBY     6
#define 	DISPLAY_MODE_YCRYCB     7

#define 	IOCTL_LCD_GET_DMA_BASE      _IOR('v', 32, unsigned int *)
#define 	IOCTL_LCD_ENABLE_INT		_IO('v', 28)
#define 	IOCTL_LCD_DISABLE_INT		_IO('v', 29)
#define 	IOCTL_FB_LOCK				_IOW('v', 64, unsigned int)	
#define 	IOCTL_FB_UNLOCK				_IOW('v', 65, unsigned int)
#define 	VIDEO_FORMAT_CHANGE			_IOW('v', 50, unsigned int)  

#define FB_DEVICE       	"/dev/fb0"  

typedef enum
{
 	FB_DISPLAY_MODE_RGB555 = 0,
 	FB_DISPLAY_MODE_RGB565 = 1,
 	FB_DISPLAY_MODE_CBYCRY = 4,
 	FB_DISPLAY_MODE_YCBYCR = 5,
 	FB_DISPLAY_MODE_CRYCBY = 6,
 	FB_DISPLAY_MODE_YCRYCB = 7,

}FB_DISPLAY_MODE_E;			// FB 显示模式

typedef enum
{
	STREAM_SRC_H264_DEC = 0,	// 源为解码端
	STREAM_SRC_ANOLOG   = 1,	// 源为模拟设备视频
}STREAM_SRC_E;

typedef enum
{
	VS_CLOSE,			                            // 关闭状态
	VS_NETTALK_BOTH,	                            // 网络通话状态,双向视频
	VS_NETTALK_SEND,	                            // 网络通话状态,仅发送视频
	VS_NETTALK_RECIVE,	                            // 网络通话状态,仅接收视频
	VS_NETSNAP,
}VIDEO_STATE_E;

typedef enum
{
	AVI_CMD_STOP,	                           		// 停止
	AVI_CMD_PAUSE,	                            	// 暂停/播放切换
	AVI_CMD_PLAY,	                            	// 播放
	AVI_CMD_CHANGE,									// 切换播放文件
}AVI_CMD_E;

typedef enum
{
	MP3_CMD_STOP,	                           		// 停止
	MP3_CMD_PAUSE,	                            	// 暂停/播放切换
	MP3_CMD_PLAY,	                            	// 播放
	MP3_CMD_CHANGE,									// 切换播放文件
}MP3_CMD_E;

// RTSP状态
typedef enum
{
	RTSP_STATE_NONE = 0,
	RTSP_STATE_CONNECT,
	RTSP_STATE_OPER_PLAY,
	RTSP_STATE_PLAY,
	RTSP_STATE_PAUSE,
	RTSP_STATE_OPER_STOP,
	RTSP_STATE_STOP,

	// 以下状态仅供心跳回调用，不属于RTSP当前状态
	RTSP_STATE_HEART,								// 心跳命令
}RTSP_STATE_E;

typedef struct AVFrame 
{
    unsigned char *data[4];
} AVFrame;

typedef struct AVPacket 
{
    unsigned char *data;
    int     size;
} AVPacket;

typedef struct video_params
{
    unsigned int bit_rate;
    unsigned int width;   //length per dma buffer
    unsigned int height;
    unsigned int framerate;
    unsigned int frame_rate_base;
    unsigned int gop_size;
    unsigned int qmax;
    unsigned int qmin;
    unsigned int quant;
    unsigned int intra;
    unsigned int targetw;
    unsigned int targeth;
} video_params;


// 画屏的位置
typedef struct auRECT_S
{
    unsigned int   x;
    unsigned int   y;
    unsigned int   width;
    unsigned int   height;
}V_RECT_S, * V_PRECT_S;


typedef void (*PAVI_CALLBACK)(uint32 cmd, uint32 time, uint32 percent);
typedef void (*PMP3_CALLBACK)(uint32 cmd, uint32 time, uint32 percent);
typedef void (*PRTSP_CALLBACK)(RTSP_STATE_E echo, int param);

typedef struct _AviPlayState{
	AVI_CMD_E cmd;
	uint8 filename[100];
	PAVI_CALLBACK callback;
	
}AviPlayState;

typedef struct _Mp3PlayState{
	AVI_CMD_E cmd;
	uint8 filename[100];
	PAVI_CALLBACK callback;
	
}Mp3PlayState;

typedef struct _RecordParam{
	uint8 filename[100];
	uint8 RecordMode;
	uint8 AudioFormat;
	uint8 VideoFormat;
	
}RecordParam;





/*************************************************
  Function:			rtsp_set_full_screen
  Description:		全频操作
  Input: 	
  	1.flg			是否全屏		
  Output:			无
  Return:			无
  Others:
*************************************************/
int rtsp_set_full_screen(uint8 flg);

/*************************************************
  Function:			rtsp_play_open
  Description:		开启rtsp播放
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 rtsp_play_open(V_RECT_S rect, char *Url, void *callback_func);

/*************************************************
  Function:			rtsp_play_close
  Description:		关闭rtsp播放
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 rtsp_play_close(void);

/*************************************************
  Function:			inter_full_screen
  Description:		全频操作
  Input: 	
  	1.flg			是否全屏		
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_full_screen(uint8 flg);


/*************************************************
  Function:    		open_video_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_video_mode(VIDEO_STATE_E mode, int addr);

/*************************************************
  Function:    		close_video_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int close_video_mode(VIDEO_STATE_E mode);

/*************************************************
  Function:    	set_jpg_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
void set_jpg_dec_param(char *filename, uint16 pos_x, uint16 pos_y, uint16 width, uint16 heigh);

/*************************************************
  Function:    	set_jpg_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
void set_jpg_enc_param(char *filename, uint16 width, uint16 heigh, DEVICE_TYPE_E DevType);

/*************************************************
  Function:    	open_jpeg_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_jpeg_dec(void);

/*************************************************
  Function:    	close_jpeg_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_jpeg_dec(void);

/*************************************************
  Function:    	open_jpeg_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_jpeg_enc(void);

/*************************************************
  Function:    	close_jpeg_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_jpeg_enc(void);

/*************************************************
  Function:			lyly_play_start
  Description:		启动留影留言
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int32 lyly_play_start(char *filename, void * proc);

/*************************************************
  Function:			lyly_play_stop
  Description:		
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
void lyly_play_stop(void);

/*************************************************
  Function:			lyly_play_pause
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 lyly_play_pause(void);

/*************************************************
  Function:			mp3_play_pause
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 mp3_play_pause(void);

/*************************************************
  Function:			lyly_play_start
  Description:		启动留影留言
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int32 mp3_play_start(char *filename, void * proc);

/*************************************************
  Function:			mp3_play_stop
  Description:		主动调用，播放完一首会自己反初始化
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
void mp3_play_stop(void);

/*************************************************
  Function:    	get_avi_record_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
int32 set_avi_record_param(uint8 mode, uint8 atp, uint8 vtp, char * filename);

/*************************************************
  Function:    	lyly_record_start
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int lyly_record_start(void);

/*************************************************
  Function:    	lyly_record_stop
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int lyly_record_stop(void);
#endif

