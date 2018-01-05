/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_media.h
  Author:   	陈本惠
  Version:  	2.0
  Date: 		2014-12-11
  Description:  多媒体接口函数
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_media_core.h"
#include "logic_include.h"
#include "logic_video.h"
#include "logic_audio.h"

#ifndef _LOGIC_MEDIA_H_
#define _LOGIC_MEDIA_H_


typedef void (*PMEDIA_CALLBACK)(uint32 cmd, uint32 time, uint32 percent);

#ifdef _AU_PROTOCOL_
#define MEDIA_AUDIO_PORT		24020				// 音频端口
#define MEDIA_VIDEO_PORT		24010				// 视频端口
#else
#define MEDIA_AUDIO_PORT		31420				// 音频端口
#define MEDIA_VIDEO_PORT		31410				// 视频端口
#endif
#define MEDIA_VIDEOL_PORT		31430

#define AUDIO_REC_PT			PT_G711A			// 本地音频录制格式(海思)
#define VIDEO_REC_PT			PT_H264				// 本地视频录制格式(海思)

#define BYTES_PER_PACKET_SHORT 	160

typedef struct 
{
	MSMediaDesc* System;
	MSMediaDesc* VideoEnc;
	MSMediaDesc* VideoDec;
	MSMediaDesc* VideoRtpSend;
	MSMediaDesc* VideoRtpRecv;
	MSMediaDesc* AudioDec;
	MSMediaDesc* AudioAI;
	MSMediaDesc* AudioAo;	
	MSMediaDesc* AudioSfEnc;
	MSMediaDesc* AudioRtpSend;
	MSMediaDesc* AudioRtpRecv;
	MSMediaDesc* FilePlayer;
	MSMediaDesc* JpegDec;
	MSMediaDesc* JpegEnc;
	MSMediaDesc* AlawAgc;	
    MSMediaDesc* Speex; 
	MSMediaDesc* AviPlay; 
	MSMediaDesc* AviRecord; 
	MSMediaDesc* Mp3Play; 
	MSMediaDesc* RtspPlay;
	MSMediaDesc* LylyFilePlayer;
}MediaStream, * PMediaStream;

// 头文件里面不能有定义
extern MediaStream mMediaStream;

// 音视频的格式
typedef enum
{
	/* audio & voice payload type */
	PT_PCMU = 0,
	PT_1016 = 1, 
	PT_G721 = 2,     
	PT_GSM = 3,
	PT_G723 = 4,
	PT_DVI4_8K = 5,     							// clock rate=8,000Hz, channels=1
	PT_DVI4_16K = 6,    							// clock rate=16,000Hz, channels=1 
	PT_LPC = 7,
	PT_PCMA = 8,
	PT_G722 = 9,
	PT_S16BE_STEREO = 10,     						// clock rate=44,100Hz, channels=2
	PT_S16BE_MONO = 11,       						// clock rate=44,100Hz, channels=1
	PT_QCELP = 12,
	PT_CN = 13,
	PT_MPEGAUDIO = 14,
	PT_G728 = 15,
	PT_DVI4_3 = 16,    								// clock rate=11,025Hz, channels=1
	PT_DVI4_4 = 17,    								// clock rate=22,050Hz, channels=1
	PT_G729 = 18,

	/* user-defined */
	PT_G711A    = 19,
	PT_G711U    = 20,
	PT_G726     = 21,
	PT_G729A    = 22,   							// w54130 defined
    PT_LPCM     = 23,
    PT_ILBC     = 24,   							// added by ghtao, 2007-06-21

	/* video payload type */
	PT_CelB = 25,
	PT_JPEG = 26,
	PT_CUSM = 27, 
	PT_NV = 28, 
	PT_PICW = 29, 
	PT_CPV = 30,     
	PT_H261 = 31,
	PT_MPEGVIDEO = 32,
	PT_MPEG2TS = 33,
	PT_H263 = 34,
	PT_SPEG = 35, 

	/* user-defined */
	PT_MPEG2VIDEO = 36,
	PT_AAC = 37,
	PT_WMA9STD = 38,
	PT_HEAAC = 39,
	PT_PCM_VOICE = 40,                         		// (PCM data replay), mono audio channel, low sampling rate.
	PT_PCM_AUDIO = 41,                         		// (PCM data replay), multi audio channels, high sampling rate.
	
    PT_AACLC   = 42,
    PT_MP3     = 43,
    PT_AMR      = 44,
    PT_AMRDTX   = 45, 
    PT_ADPCMA   = 49,
    PT_AEC      = 50,
    
    PT_X_LD = 95, 
	PT_H264 = 96,
	PT_D_GSM_HR = 200, 
	PT_D_GSM_EFR = 201, 
	PT_D_L8 = 202, 
	PT_D_RED = 203, 
	PT_D_VDVI = 204, 
	PT_D_BT656 = 220, 
	PT_D_H263_1998 = 221, 
	PT_D_MP1S = 222, 
	PT_D_MP2P = 223, 
	PT_D_BMPEG = 224, 
	PT_MP4VIDEO = 230,
	PT_MP4AUDIO = 237, 
	PT_VC1 = 238,
	PT_JVC_ASF = 255, 
	PT_D_AVI = 256, 
	PT_MAX = 257
}PAYLOAD_TYPE_E;

// 播放的音频格式
typedef enum
{
	PLAY_AUDIO_TYPE_WAV,
	PLAY_AUDIO_TYPE_MP3,
	PLAY_AUDIO_TYPE_NONE
}PLAY_AUDIO_TYPE_E;

// 留影留言模式
typedef enum
{
	LWM_AUDIO = 0,									// 仅音频模式
	LWM_AUDIO_PIC,									// 音频+图片模式
	LWM_AUDIO_VIDEO,								// 音视频模式
	LWM_NONE										// 不启用
}LEAVE_WORD_MODE_E;	

typedef struct 
{
	uint32 ip;
	uint16 port;
}RTP_ADDRESS, * PRTP_ADDRESS;

// 媒体文件头结构
typedef struct
{
    uint8 RecFlag;		                            // 二进制: bit0 音频   bit1 视频
    uint8 AudioFormat;	                            // 音频格式
    uint8 VideoFormat;	                            // 视频格式
    uint8 reserved;		                            // 保留, 填0xFF
    uint32 time;		                            // 录制时间
}MEDIA_FILE_HEAD, * PMEDIA_FILE_HEAD;

typedef struct
{
    uint8 reserved;	                                // 保留, 填0xFF
    uint8 flag;		                                // 0-音频, 1-视频
    uint16 size;	                           		// 长度
    uint32 ts;		                                // 时间戳
}MEDIA_HEAD, * PMEDIA_HEAD;

// 音视频同步的时间
typedef struct
{
    uint32 current;
	uint32 redress;
	uint32 ts0;
} SYNC_TIME;

typedef struct _MEDIA_LYLY_CTRL
{
	FILE 			* FileHald;						// 文件指针
	char			FileName[50];					// 文件名
	uint32 			FileSize;						// 文件大小
	MEDIA_FILE_HEAD FileHead;						// 文件头,只有包含HI文件头的文件才有用
	uint8			* data;							// 文件内容
	LEAVE_WORD_MODE_E mode;							// 留影留言类型 1仅音频  2仅视频  3音视频
	SYNC_TIME		TimeA;							// 音频时间戳
	SYNC_TIME		TimeV;							// 视频时间戳
	uint8			state;							// 0-空闲 1-播放 2-录制
	uint8			issave;							// 0:no 1:yes
	PMEDIA_CALLBACK callback;						// 回调
}MEDIA_LYLY_CTRL, * PMEDIA_LYLY_CTRL;

/*************************************************
  Function:			media_stop_rtsp
  Description:		关闭rtsp
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
int media_stop_rtsp(void);

/*************************************************
  Function:			media_start_rtsp
  Description:		开启rtsp
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
//int media_start_rtsp(char *Url, void *callback_func);

/*************************************************
  Function:			media_get_v4l_addr
  Description:		
  Input:
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
void media_get_v4l_addr(uint8 **addr);

/*************************************************
  Function:			media_start_analog_video
  Description:		启动模拟视频预览
  Input:
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_start_analog_video(void);

/*************************************************
  Function:			media_stop_net_video
  Description:		停止模拟视频预览
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_stop_analog_video(void);

/*************************************************
  Function:			media_set_analog_output_volume
  Description:		设置音频codec输出音量
  Input: 
  	vol				音量等级(0静音 - 8最大)
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_set_analog_output_volume(uint32 vol);

/*************************************************
  Function:			media_play_lyly
  Description:		
  Input: 
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_play_lyly (char *filename, void * proc);

/*************************************************
  Function:			media_stop_lyly
  Description:		
  Input: 
  	vol				音量等级
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
void media_stop_lyly (void);

/*************************************************
  Function:			media_set_output_volume_high
  Description:		设置音频codec输出音量
  Input: 
  	vol				音量等级
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_set_output_volume_high(uint32 vol);

/*************************************************
  Function:			media_set_device
  Description:		设置设备类型
  Input: 
  	dev				设备类型
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
void media_set_device(DEVICE_TYPE_E dev);

/*************************************************
  Function:			media_fill_audio_data
  Description:		将网络音频数据填到buf
  Input: 
  	1.data			数据指针
  	2.len			数据长度
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_fill_audio_data(uint8 *data, uint32 len, uint32 time);

/*************************************************
  Function:			media_fill_video_data
  Description:		将网络视频数据填到buf
  Input: 
  	1.data			数据指针
  	2.len			数据长度
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_fill_video_data(uint8 *data, uint32 len, uint32 time);

/*************************************************
  Function:			media_full_dispaly_video
  Description:		全屏显示视频
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_full_dispaly_video(DEVICE_TYPE_E devtype, uint8 flg);

/*************************************************
  Function:			media_enable_audio_aec
  Description:		使能消回声接口
  Input: 			
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int media_enable_audio_aec(void);

/*************************************************
  Function:			media_disable_audio_aec
  Description:		使能消回声接口
  Input: 			
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int media_disable_audio_aec(void);

// 抓拍
uint32 media_snapshot(char *filename, uint32 dstW, uint32 dstH, DEVICE_TYPE_E DevType);

// 声音播放
uint32 media_play_sound(char *filename, uint8 IsRepeat, void * proc);

// 停止播放
void media_stop_sound (void);

/*************************************************
  Function:			media_start_local_record
  Description:		启动本地录音
  Input: 			
  	1.filename		文件名
  	2.maxtime		录制的最长时间
  	3.proc			回调
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_start_local_record(char *filename);

/*************************************************
  Function:			media_stop_local_record
  Description:		停止本地录音
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
void media_stop_local_record(void);

/*************************************************
  Function:			media_start_net_video
  Description:		启动网络视频播放
  Input: 	
  	1.addres		对端地址
  	2.vorect		视频输出位置
  	3.flag			视频模式 发送、接收
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_start_net_video(uint32 address, uint8 mode);

/*************************************************
  Function:			media_stop_net_video
  Description:		停止网络视频播放
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_stop_net_video(uint8 mode);

/*************************************************
  Function:			media_start_raw_video_file
  Description:		启动播放H264裸码流文件
  Input: 	
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_start_raw_video_file(char * filename);

void media_stop_raw_video_file(void);

/*************************************************
  Function:			media_start_net_audio
  Description:		启动网络音频播放
  Input: 			
  	1.tp			音频格式
  	2.proc			回调
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_start_net_audio(int address, int len);

/*************************************************
  Function:			media_stop_net_audio
  Description:		停止网络音频播放
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_stop_net_audio(void);

/*************************************************
  Function:			meida_start_net_hint
  Description:		启动留言提示音发送
  Input: 			
  	1.tp			发送的格式
  	2.filename		提示音文件
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 meida_start_net_hint(uint8 RemoteDeviceType, char *filename, void * proc);

/*************************************************
  Function:			meida_stop_net_hint
  Description:		停止提示音发送
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 meida_stop_net_hint(void);

/*************************************************
  Function:			meida_start_net_leave_rec
  Description:		启动从网络上录制
  Input: 			
  	1.mode			录制的模式
  	2.atp			音频格式
  	3.vtp			视频格式
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 meida_start_net_leave_rec(LEAVE_WORD_MODE_E mode,
						PAYLOAD_TYPE_E atp, PAYLOAD_TYPE_E vtp, char * filename);

/*************************************************
  Function:			stop_leave_word_net
  Description:		停止录制
  Input: 			
  	1.issave		是否保存(1保存, 0不保存)
  	2.filename		保存的文件名
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 media_stop_net_leave_rec(uint8 issave);

/*************************************************
  Function:			meida_set_audio_send_addr
  Description:		设置音频的发送地址和端口
  Input: 			
  	1.ip			IP地址
  	2.port			端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void meida_set_audio_send_addr(uint32 ip, uint16 port);

/*************************************************
  Function:			media_del_audio_send_addr
  Description:		删除音频的发送地址和端口
  Input: 
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_del_audio_send_addr(uint32 IP, uint16 AudioPort);

/*************************************************
  Function:    		media_add_audio_sendaddr
  Description: 		增加音频发送地址
  Input: 			
  	1.IP			IP地址
  	2.AudioPort		音频端口
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int32 media_add_audio_sendaddr(uint32 IP, uint16 AudioPort);

/*************************************************
  Function:			media_set_dec_enable
  Description:		打开音频输出设置
  Input: 
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_set_dec_enable(void);

/*************************************************
  Function:			media_set_dec_disable
  Description:		关闭音频输出设置
  Input: 
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_set_dec_disable(void);

/*************************************************
  Function:			media_set_video_send_addr
  Description:		设置视频的发送地址和端口
  Input: 			
  	1.ip			IP地址
  	2.port			端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_set_video_send_addr(uint32 ip, uint16 port);

/*************************************************
  Function:			media_del_video_send_addr
  Description:		删除视频的发送地址和端口
  Input: 
  Output:			无
  Return:			无
  Others:
*************************************************/
void media_del_video_send_addr(void);

/*************************************************
  Function:			media_set_output_volume
  Description:		设置音频codec输出音量
  Input: 
  	vol				音量等级
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
uint32 media_set_output_volume(uint32 vol);

/*************************************************
  Function:			media_set_talk_volume
  Description:		设置通话音量
  Input: 
  	vol				音量等级(0静音 - 8最大)
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_set_talk_volume(DEVICE_TYPE_E devtype, uint32 vol);

/*************************************************
  Function:			media_get_total_time
  Description:		获取媒体文件的总时间
  Input: 			无
  Output:			无
  Return:			总时间 单位:s
  Others:			如果留影留言有:仅视频模式的话
  					这样获取不到正确的时间
  					暂时没有这种模式
*************************************************/
uint32 media_get_total_time(void);

/*************************************************
  Function:		media_clear_fb
  Description: 	清空fb
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int32 media_clear_fb(void);

/*************************************************
  Function:		init_media
  Description: 	媒体初始化
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void media_init(void);

/*************************************************
  Function:		media_play_video_test
  Description: 	测试播放文件
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void media_play_video_test(void);

/*************************************************
  Function:			media_show_pict
  Description:		JPG图片显示
  Input: 			
  	1.filename		图像保存的文件名
  					如果>1时文件名自动加上编号
	2.pos_x			目标图片显示X 坐标
	3.pos_y			目标图片显示Y 坐标
	4.with			目标图片显示宽度
	5.heigh			目标图片显示长度
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int32 media_start_show_pict(char *filename, uint16 pos_x, uint16 pos_y, uint16 with, uint16 heigh);

/*************************************************
  Function:			media_stop_show_pict
  Description:		关闭图片显示
  Input: 			
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
void media_stop_show_pict(void);

/*************************************************
  Function:			media_pause_lyly
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
uint32 media_pause_lyly (void);

/*************************************************
  Function:			media_pause_mp3
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
void media_pause_mp3 (void);

/*************************************************
  Function:			media_play_mp3
  Description:		播放MP3文件
  Input: 			
  	1.filename		文件名
  	2.proc			回调
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
uint32 media_play_mp3 (char *filename, void * proc);

/*************************************************
  Function:			media_stop_lyly
  Description:		停止留影留言播放
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
void media_stop_mp3 (void);

/*************************************************
  Function:    	media_w_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void media_w_close(void);

#endif

