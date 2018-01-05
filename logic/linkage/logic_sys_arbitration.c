/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_sys_arbitration.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  系统仲裁:包括媒体和系统提示
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#if 0
#include "include.h"
#include "logic_sys_arbitration.h"
#include "hw_include.h"
#include "storage_types.h"
#endif

#include "logic_include.h"
#include "storage_include.h"

static SYS_MEDIA_TYPE g_CurMediaType;
static MediaPlayCallback g_ProgressProc;
static MediaStopedCallback g_StoppedProc;
static uint8 g_isRepeat;
static uint8 g_ViewPict_State = 0;

static uint8 g_sysHints[SYS_HINT_MAX_NUM];
SYS_LCD_STATE g_LcdState = SYS_LCD_CLOSE;
static 	pthread_mutex_t g_MediaLock;
static uint32 g_RecordTimer = 0;				// 家人留言录制定时器
static uint32 g_RecordTimeOut = 0;				// 家人留言录制时间
static uint32 g_RecordStartTime = 0;			// 家人留言录制开始时间
static LYLY_TYPE g_LylyMode = LYLY_TYPE_VIDEO;	// 播放留影留言模式
static uint8 g_NoDeal = FALSE;

/*************************************************
  Function:			set_curplay_state
  Description: 		设置当前媒体播放状态
  Input: 		
  	1.MediaType		媒体类型
  	2.ProgressProc	进度回调函数
  	3.StoppedProc	被停止时回调函数
  Output:			无
  Return:			无
  Others:			
*************************************************/
static void set_curplay_state(SYS_MEDIA_TYPE MediaType, MediaPlayCallback ProgressProc, MediaStopedCallback StoppedProc, uint8 isRepeat)
{
	g_CurMediaType = MediaType;
	log_printf("g_CurMediaType======%d : \n",g_CurMediaType);
	g_ProgressProc = ProgressProc;
	g_StoppedProc = StoppedProc;
	g_isRepeat = isRepeat;

	if (g_CurMediaType == SYS_MEDIA_NONE || 
		g_CurMediaType == SYS_MEDIA_FAMILY_RECORD)
	{
		log_printf("set_curplay_state : spk off\n");
		hw_speak_off();
	}
	else
	{
		log_printf("set_curplay_state : spk on\n");
		hw_speak_on();
	}
}

/*************************************************
  Function:			arbi_stop_media
  Description: 		停止媒体操作
  Input: 		
  	1.MediaType		媒体类型
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
static int32 arbi_stop_media(SYS_MEDIA_TYPE MediaType)
{
	switch (MediaType)
	{		
		case SYS_MEDIA_INTERCOM:
			{
				// 停止对讲
				inter_hand_down();
				// mody by chenbh 解决对讲转报警声音出不来问题
				usleep(500*1000);
			}
			break;
			
		case SYS_MEDIA_MONITOR:
			// 停止监视
			if (rtsp_monitor_state())
			{
				log_printf("SYS_MEDIA_MONITOR rtsp\n");
				rtsp_monitor_stop();
				usleep(1000*1000);
			}
			else
			{
				log_printf("SYS_MEDIA_MONITOR monitor\n");
				monitor_stop();
				// 监视转呼叫 延迟等待监视线程退出
				usleep(1000*1000);
			}
			break;
			
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_ALARM:
			// 停止播放声音文件
			media_stop_sound();
			log_printf("arbi_stop_media  arbi_stop_media\n");
			break;
			
		case SYS_MEDIA_FAMILY_RECORD:
			{
				// add by chenbh 删除定时器
				if (0 != g_RecordTimer)
				{
					cancel_aurine_timer(g_RecordTimer, NULL);
					g_RecordTimer = 0;
				}
				// 停止家人留言录制
				media_stop_local_record();
			}
			break;
			
		case SYS_MEDIA_FAMILY_AUDITION:		// 播放家人留言媒体类型 SYS_MEDIA_MUSIC
			// 停止家人留言试听
			break;
			
		case SYS_MEDIA_LEAVEWORD_PLAY:
			// 停止播放留影留言
			if (LYLY_TYPE_VIDEO == g_LylyMode)
			{
				media_stop_lyly();	
			}
			else
			{
				media_stop_sound();
			}
			break;
			
		case SYS_MEDIA_KEY:
			// 停止播放按键音
			break;	
		
		case SYS_MEDIA_NONE:
			break;
			
		default:	
			return SYS_MEDIA_ECHO_ERR;
	}
	if (g_StoppedProc)
	{
		g_StoppedProc();
	}
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_play_alarm
  Description: 		判断是否可以进行响报警声操作
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_play_alarm(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	log_printf("MediaType = %d \n", MediaType);
	//switch (g_CurMediaType)
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
		case SYS_MEDIA_LEAVEWORD_PLAY:
		case SYS_MEDIA_KEY:
			arbi_stop_media(MediaType);
			if (SYS_MEDIA_INTERCOM == MediaType ||
				SYS_MEDIA_MONITOR == MediaType)
			{
				sleep(1);
			}
			//arbi_stop_media(g_CurMediaType);
			break;
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}

	// 查看照片中需要关闭
	if (sys_get_view_picture_state())
	{
		media_stop_show_pict();
	}
	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_oper_intercomm
  Description: 		判断是否可以进行对讲
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_oper_intercomm(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
			return MediaType;
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
		case SYS_MEDIA_LEAVEWORD_PLAY:
		case SYS_MEDIA_KEY:
			arbi_stop_media(MediaType);
			break;
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}

	// 查看照片中需要关闭
	if (sys_get_view_picture_state())
	{
		media_stop_show_pict();
	}
	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_oper_monitor
  Description: 		判断是否可以进行监视
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_oper_monitor(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:	
			return MediaType;
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION: 
		case SYS_MEDIA_LEAVEWORD_PLAY: 
		case SYS_MEDIA_KEY:
			arbi_stop_media(MediaType);
			break;
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}	

	// 查看照片中需要关闭
	if (sys_get_view_picture_state())
	{
		media_stop_show_pict();
	}
	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_play_music
  Description: 		判断是否可以播放音乐
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_play_music(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	log_printf("=======MediaType :%d=============\n",MediaType);
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
		case SYS_MEDIA_LEAVEWORD_PLAY:
			return MediaType;
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_KEY:
			arbi_stop_media(MediaType);
			break;	
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_oper_family_record
  Description: 		判断是否可以家人留言录制
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_oper_family_record(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_FAMILY_AUDITION:
			return MediaType;
		case SYS_MEDIA_FAMILY_RECORD:
			return SYS_MEDIA_ECHO_OK;
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC: 
		case SYS_MEDIA_LEAVEWORD_PLAY:
 		case SYS_MEDIA_KEY:
 			arbi_stop_media(MediaType);
 			break;
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
 	}	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_oper_family_audition
  Description: 		判断是否可以家人留言试听
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_oper_family_audition(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
			return MediaType;
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_LEAVEWORD_PLAY:
 		case SYS_MEDIA_KEY:
 			arbi_stop_media(MediaType);
 			break;	
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_play_leaveword
  Description: 		判断是否可以播放留影留言
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_play_leaveword(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
		case SYS_MEDIA_LEAVEWORD_PLAY:
			return MediaType;
			
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC: 
		case SYS_MEDIA_KEY:
			arbi_stop_media(MediaType);
			break;	
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;
	}	

	// 查看照片中需要关闭
	if (sys_get_view_picture_state())
	{
		media_stop_show_pict();
	}
	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			arbi_play_key
  Description: 		判断是否可以播放按键音
  Input: 		
  Output:			无
  Return:			0-可以 非0-不行
  Others:			
*************************************************/
static int32 arbi_play_key(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (MediaType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_INTERCOM:
		case SYS_MEDIA_MONITOR:
		case SYS_MEDIA_FAMILY_RECORD:
		case SYS_MEDIA_FAMILY_AUDITION:
		case SYS_MEDIA_LEAVEWORD_PLAY:
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_KEY:
			return MediaType; 
		case SYS_MEDIA_NONE:
			break;
		default:
			return SYS_MEDIA_ECHO_ERR;			
	}	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			media_callback
  Description: 		多媒体回调
  Input: 		
  	1.cmd 			命令码:0-err 1-process&time
  	2.time			时间-秒
  	3.percent		百分比-0至100
  Output:			无
  Return:			无
  Others:			
*************************************************/
static void play_media_callback(int32 cmd, int32 time, int32 percent)
{
	if (cmd == 0)
	{
		// 出错通知
		if (g_ProgressProc)
		{
			g_ProgressProc(0, 0, FALSE);
		}
		log_printf("play_media_callback : play err !");
		set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
	}
	else
	{
		// 进度提示
		if (g_ProgressProc)
		{
			g_ProgressProc(time, percent, TRUE);
		}
		
		if (percent == 100)
		{
			if (SYS_MEDIA_NONE != sys_get_media_state() && FALSE == g_isRepeat)
			{
				log_printf("g_NoDeal[%d] : play end !\n", g_NoDeal);
				if (g_NoDeal == FALSE)			// 防止g_ProgressProc 中又调用了媒体部分接口 此处把媒体状态改变造成错误
				{
					set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
				}				
			}			
		}
		
		g_NoDeal = FALSE;
	}
}

/*************************************************
  Function:			on_alarm_timer
  Description:		定时器
  Input: 			
  	1.type 			声音的类型
  Output:			无
  Return:			无
  Others:
*************************************************/
static void* on_record_timer(uint32 ID, void * arg)
{
	if (0 != g_RecordTimer)
	{
		// 定时器里面处理家电部分 耗时太多导致时间不准 不用自加计算时间
		g_RecordTimeOut = time(NULL) - g_RecordStartTime;
		if (SYS_FAMILY_RECORD_MAXTIME <= g_RecordTimeOut)
		{			
			if (g_ProgressProc)
			{
				g_ProgressProc(SYS_FAMILY_RECORD_MAXTIME, 100, TRUE);
			}
			sys_stop_family_record();
		}
		else
		{
			if (g_ProgressProc)
			{
				g_ProgressProc(SYS_FAMILY_RECORD_MAXTIME, ((g_RecordTimeOut*100)/SYS_FAMILY_RECORD_MAXTIME), TRUE);
			}
		}
	}
}

/*************************************************
  Function:			sys_set_no_deal
  Description: 		
  Input: 			
  	1.flg			
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_set_no_deal(uint8 flg)
{
	g_NoDeal = flg;
}

/*************************************************
  Function:			sys_stop_play_audio
  Description: 		停止播放音频文件
  Input: 			
  	1.AudioType		音乐播放类型
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_stop_play_audio(SYS_MEDIA_TYPE AudioType)
{	
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	switch (AudioType)
	{
		case SYS_MEDIA_ALARM:
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
		case SYS_MEDIA_KEY:
			break;
		default:
			return;
	}
	
	if (AudioType != MediaType)
	{
		return;
	}
	
	arbi_stop_media(MediaType);
	set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
}

/*************************************************
  Function:			sys_start_play_audio
  Description: 		开始播放音频
  Input: 			
  	1.MediaType		媒体操作类型
  	2.FileName		需要播放的媒体文件路径
  	3.isRepeat		是否重复播放
  	4.value			设置的音量
  	5.ProgressProc	进度条回调函数
  	6.StoppedProc	被强制终止后回调函数
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_start_play_audio(const SYS_MEDIA_TYPE MediaType, char *FileName, uint8 isRepeat, uint8 volume, 
							MediaPlayCallback ProgressProc, MediaStopedCallback StoppedProc)
{
	int32 ret = SYS_MEDIA_NONE; 
	
	log_printf(" 111 FileName: %s\n MediaType :%d  isRepeat :%d volume :%d\n", FileName, MediaType, isRepeat, volume);
	pthread_mutex_lock(&g_MediaLock);
	switch (MediaType)
	{
		case SYS_MEDIA_NONE:
			return SYS_MEDIA_ECHO_ERR;
			
		case SYS_MEDIA_ALARM:
			ret = arbi_play_alarm();
			break;
			
		case SYS_MEDIA_YUJING:
		case SYS_MEDIA_OPER_HINT:
		case SYS_MEDIA_MUSIC:
			ret = arbi_play_music();
			break;
			
		case SYS_MEDIA_KEY:
			ret = arbi_play_key();
			break;
			
		default:
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_ERR;
	}

	if (SYS_MEDIA_ECHO_OK != ret)
	{
		pthread_mutex_unlock(&g_MediaLock);
		return ret;
	} 

	if (FileName == NULL)
	{
		set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
		pthread_mutex_unlock(&g_MediaLock);
		return SYS_MEDIA_ECHO_ERR;
	}
	// modi by luofl 2014-03-10 关闭媒体后延迟100ms，防止媒体没关闭完全又开启而出错。
	usleep(100*1000);
	set_curplay_state(MediaType, ProgressProc, StoppedProc, isRepeat);

	// 开始播放音频文件
	ret = media_play_sound(FileName, isRepeat, (void*)play_media_callback);
	if (ret == TRUE)
	{
		// 设置音量
		media_set_device(DEVICE_TYPE_NONE);
		media_set_output_volume(volume);
	}
	else
	{
		set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
		pthread_mutex_unlock(&g_MediaLock);
		return SYS_MEDIA_ECHO_ERR;
	}
	pthread_mutex_unlock(&g_MediaLock);
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_start_play_leaveword
  Description: 		开始播放留影留言
  Input: 			
  	1.FileName		文件名
  	2.vorect		播放区域
  	3.ProcessProc	进度回调
  	4.StoppedProc	被强制终止后回调函数
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
//int32 sys_start_play_leaveword(char *FileName, MediaPlayCallback ProcessProc, MediaStopedCallback StoppedProc)
int32 sys_start_play_leaveword(char *FileName, LYLY_TYPE type, uint8 volume, MediaPlayCallback ProcessProc, MediaStopedCallback StoppedProc)
{
	int32 ret = arbi_play_leaveword();
	if (SYS_MEDIA_ECHO_OK != ret)
	{
		return ret;
	}
	set_curplay_state(SYS_MEDIA_LEAVEWORD_PLAY, ProcessProc, StoppedProc, FALSE);

	// 开始播放留影留言
	if (LYLY_TYPE_VIDEO == type)
	{
		ret = media_play_lyly(FileName, (void*)play_media_callback);
		if (ret == TRUE)
		{
			// modi by luofl 2011-08-26 留影留言播放音量固定为6级
			#if 0
			// 获取音量
			uint32 volume = storage_get_ringvolume();
			media_set_output_volume(volume);
			#else
			//media_set_output_volume(6); // 播放avi音频和此操作可能冲突
			#endif
		}
		else
		{
			set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
			return SYS_MEDIA_ECHO_ERR;
		}
	}
	else	// 播放纯声音
	{
		ret = media_play_sound(FileName, 0, (void*)play_media_callback);
		if (ret == TRUE)
		{
			// 设置音量
			media_set_device(DEVICE_TYPE_NONE);
			media_set_output_volume(volume);
		}
		else
		{
			set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
			return SYS_MEDIA_ECHO_ERR;
		}
	}

	g_LylyMode = type;
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_stop_play_leaveword
  Description: 		停止播放留影留言
  Input: 			无
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_stop_play_leaveword(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	if (MediaType != SYS_MEDIA_LEAVEWORD_PLAY)
	{
		return SYS_MEDIA_ECHO_ERR;
	}
	
	// 停止播放留影留言
	if (LYLY_TYPE_VIDEO == g_LylyMode)
	{
		media_stop_lyly();	
	}
	else
	{
		media_stop_sound();
	}
	set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
	g_LylyMode = LYLY_TYPE_MAX;
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_start_family_record
  Description: 		开始家人留言录制
  Input: 			
  	1.FileName		文件路径
  	2.ProcessProc	进度回调函数
  	3.StoppedProc	被强制终止后回调函数
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_start_family_record(char *FileName, MediaPlayCallback ProcessProc, MediaStopedCallback StoppedProc)
{
	int32 ret = arbi_oper_family_record();
	if (SYS_MEDIA_ECHO_OK != ret)
	{
		return ret;
	}
	set_curplay_state(SYS_MEDIA_FAMILY_RECORD, ProcessProc, StoppedProc, FALSE);

	// 开始家人留言录制
	//ret = media_start_local_record(FileName, SYS_FAMILY_RECORD_MAXTIME, (void*)play_media_callback);
	ret = media_start_local_record(FileName);
	if (ret == FALSE)
	{
		set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
		return SYS_MEDIA_ECHO_ERR;
	}
	else	// add by chenbh 增加定时器
	{
		if (0 != g_RecordTimer)
		{
			cancel_aurine_timer(g_RecordTimer, NULL);
		}
		g_RecordStartTime = time(NULL);
		g_RecordTimer = 0;
		g_RecordTimer = add_aurine_realtimer(1000, on_record_timer, NULL);				
	}
		
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_stop_family_record
  Description: 		停止家人留言录制
  Input: 			无
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_stop_family_record(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	if (MediaType != SYS_MEDIA_FAMILY_RECORD)
	{
		return SYS_MEDIA_ECHO_ERR;
	}
	// add by chenbh 删除定时器
	if (0 != g_RecordTimer)
	{
		cancel_aurine_timer(g_RecordTimer, NULL);
		g_RecordTimer = 0;
	}
	
	// 停止家人留言录制
	media_stop_local_record();
	set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_start_family_audition
  Description: 		开始家人留言试听
  Input: 			
  	1.ProcessProc	进度回调
	2.StoppedProc	被强制终止后回调函数  	
  Output:			无
  Return:			0-成功 非0-失败
  Others:			暂不使用
*************************************************/
int32 sys_start_family_audition(MediaPlayCallback ProcessProc, MediaStopedCallback StoppedProc)
{
	int32 ret = arbi_oper_family_audition();
	if (SYS_MEDIA_ECHO_OK != ret)
	{
		return ret;
	}
	set_curplay_state(SYS_MEDIA_FAMILY_AUDITION, ProcessProc, NULL, FALSE);

	// 开始家人留言试听

	// 音量设置
	
	return SYS_MEDIA_ECHO_OK;
}

/*************************************************
  Function:			sys_stop_family_audition
  Description: 		停止家人留言试听
  Input: 			无
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_stop_family_audition(void)
{
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	if (MediaType != SYS_MEDIA_FAMILY_AUDITION)
	{
		return SYS_MEDIA_ECHO_ERR;
	}
	
	// 停止家人留言试听
	
	set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
	return SYS_MEDIA_ECHO_OK;	
}

/*************************************************
  Function:			sys_set_intercomm_state
  Description: 		设置当前对讲状态
  Input: 			
  	1.state			TRUE-处于对讲中 FALSE-不处于对讲中
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_set_intercomm_state(uint8 state)
{
	int32 ret;
	pthread_mutex_lock(&g_MediaLock);
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	if (state)
	{
		ret = arbi_oper_intercomm();
		if (ret != SYS_MEDIA_ECHO_OK)
		{
			pthread_mutex_unlock(&g_MediaLock);
			return ret;
		}
		else
		{
			set_curplay_state(SYS_MEDIA_INTERCOM, NULL, NULL, FALSE);
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_OK;
		}
	}
	else
	{
		if (MediaType != SYS_MEDIA_INTERCOM)
		{
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_ERR;
		}
		else
		{
			set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_OK;
		}
	}
}

/*************************************************
  Function:			sys_set_monitor_state
  Description: 		设置当前监视状态
  Input: 			
  	1.state			TRUE-处于监视中 FALSE-不处于监视中
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_set_monitor_state(uint8 state)
{
	int32 ret;
	pthread_mutex_lock(&g_MediaLock);
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	log_printf("sys_set_monitor_state  MediaType :%d  state: %d\n", MediaType, state);
	if (state)
	{
		ret = arbi_oper_monitor();
		if (ret != SYS_MEDIA_ECHO_OK)
		{
			pthread_mutex_unlock(&g_MediaLock);
			return ret;
		}
		else
		{
			set_curplay_state(SYS_MEDIA_MONITOR, NULL, NULL, FALSE);
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_OK;
		}
	}
	else
	{
		if (MediaType != SYS_MEDIA_MONITOR)
		{
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_ERR;
		}
		else
		{
			set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
			pthread_mutex_unlock(&g_MediaLock);
			return SYS_MEDIA_ECHO_OK;
		}
	}
}

/*************************************************
  Function:			sys_set_view_picture_state
  Description: 		设置图片查看状态
  Input: 			
  	1.state			TRUE-处于图片查看中 FALSE-不处于图片查看中
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_set_view_picture_state(uint8 state)
{
	g_ViewPict_State = state;
	return state;
}

/*************************************************
  Function:			sys_get_view_picture_state
  Description: 		获取图片查看状态
  Input: 			
  	1.state			TRUE-处于图片查看中 FALSE-不处于图片查看中
  Output:			无
  Return:			0-成功 非0-失败
  Others:			
*************************************************/
int32 sys_get_view_picture_state(void)
{
	return g_ViewPict_State;
}

/*************************************************
  Function:			sys_get_media_state
  Description: 		获取系统媒体状态
  Input: 		
  Output:			无
  Return:			系统媒体状态
  Others:			
*************************************************/
SYS_MEDIA_TYPE sys_get_media_state(void)
{
	return g_CurMediaType;
}

/*************************************************
  Function:			sys_stop_cur_media
  Description: 		停止当前媒体
  Input: 		
  Output:			无
  Return:			
  Others:			
*************************************************/
void sys_stop_cur_media(void)
{
	SYS_MEDIA_TYPE CurState = sys_get_media_state();
	arbi_stop_media(CurState);
	set_curplay_state(SYS_MEDIA_NONE, NULL, NULL, FALSE);
}

/*************************************************
  Function:			sys_set_hint_state
  Description: 		设置系统状态
  Input: 		
  	1.HintType		系统状态类型
  	2.flg			标志: TRUE / FALSE
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_set_hint_state(SYS_HINT_TYPE HintType, uint8 flg)
{
	uint8 i = 0;
	
	// 设置全局变量
	g_sysHints[HintType] = flg;
	log_printf("sys_set_hint_state : HintType:%d, flg:%d\n", HintType, flg);

	#ifdef _CP2526_TOUCH_KEY_
	// 操作按键背光灯
	for (i = SYS_HINT_INFO; i < SYS_HINT_MISSED_CALLS+1; i++)
	{
		if (g_sysHints[i] == TRUE)
		{
			// 背光灯闪烁
			log_printf("sys_set_hint_state : key led flash\n");
			hw_key_led_flash();
			return;
		}
	}

	if (g_sysHints[SYS_HINT_OPERATING] || g_sysHints[SYS_HINT_ALARM_STATE])
	{
		// 背光灯高亮
		log_printf("sys_set_hint_state : key led on\n");
		hw_key_led_on();
		return;
	}

	// 背光灯微亮
	log_printf("sys_set_hint_state : key led off\n");
	hw_key_led_off();
	#else
	if (TRUE == g_sysHints[SYS_HINT_INFO])
	{
		hw_message_led_glint();
	}
	else
	{
		hw_message_led_off();
	}

	if (TRUE == g_sysHints[SYS_HINT_ALARM_WARNING])
	{
		hw_alarm_led_glint();
	}
	else if (TRUE == g_sysHints[SYS_HINT_ALARM_STATE])
	{
		hw_alarm_led_on();
	}
	else
	{
		hw_alarm_led_off();
	}
	#endif
	
	return;
}

/*************************************************
  Function:			sys_get_hint_state
  Description: 		获得系统状态
  Input: 		
  	1.HintType		系统状态类型
  Output:			无
  Return:			TRUE / FALSE
  Others:			
*************************************************/
uint8 sys_get_hint_state(SYS_HINT_TYPE HintType)
{
	if (HintType >= SYS_HINT_MAX_NUM || HintType < SYS_HINT_INFO)
	{
		return FALSE;
	}

	// 免打扰即时获取状态
	if (HintType == SYS_HINT_NO_DISTURB)
	{
		g_sysHints[HintType] = linkage_get_nodisturb_state();	
	}
	
	return g_sysHints[HintType];
}

/*************************************************
  Function:			sys_sync_hint_state
  Description: 		同步系统状态
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_sync_hint_state(void)
{
	uint8 flg;
	
	// 获取信息未读状态
	flg = storage_get_msg_state();
	if (flg)
	{
		sys_set_hint_state(SYS_HINT_INFO, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_INFO, FALSE);
	}
	g_MsgNum = flg;
	
	// 获取留影留言未读状态
	flg = storage_get_lylyrecord_flag();
	if (flg)
	{
		sys_set_hint_state(SYS_HINT_LEAVEWORD, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_LEAVEWORD, FALSE);
	}
	g_LylyNum = flg;
	
	// 获取家人留言未读状态
	flg = storage_get_jrlyrecord_flag();
	if (flg == TRUE)
	{
		sys_set_hint_state(SYS_HINT_FAMILY, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_FAMILY, FALSE);
	}
	
	// 获取未接来电未读状态
	flg = storage_get_callrecord_state();
	if (flg == TRUE)
	{
		sys_set_hint_state(SYS_HINT_MISSED_CALLS, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_MISSED_CALLS, FALSE);
	}

	// 初始化免打扰-无免打扰
	flg = storage_get_noface();
	if (flg == TRUE)
	{
		sys_set_hint_state(SYS_HINT_NO_DISTURB, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_NO_DISTURB, FALSE);
	}
}

/*************************************************
  Function:			sys_get_lcd_state
  Description: 		屏幕状态
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
SYS_LCD_STATE sys_get_lcd_state(void)
{
	return g_LcdState;
}

/*************************************************
  Function:			sys_open_lcd
  Description: 		开屏操作
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
uint8 sys_open_lcd(void)
{
	#if 0
	if (SYS_LCD_CLOSE == g_LcdState)
	{
		//PMU_EnterModule(PMU_NORMAL);				// add by luofl 2011-08-16 降低待机功耗
		hw_lcd_pwr_on();
		g_LcdState = SYS_LCD_OPEN;
		sys_set_hint_state(SYS_HINT_OPERATING, TRUE);
		//hw_spk_on();
		DelayMs_nops(150);
		BL_SetBright(storage_get_bright());
		return TRUE;
	}
	return FALSE;
	#else
	if (SYS_LCD_CLOSE == g_LcdState)
	{
		hw_lcd_power_on();							// 开屏电源
		sys_set_hint_state(SYS_HINT_OPERATING, TRUE);
		g_LcdState = SYS_LCD_OPEN;
		usleep(200*1000);
		hw_lcd_back_on();							// 开屏背光
		//hw_speak_on();							// 开喇叭
		return TRUE;
	}
	
	return FALSE;
	#endif
}

/*************************************************
  Function:			sys_close_lcd
  Description: 		关屏操作
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_close_lcd(void)
{
	if (SYS_LCD_OPEN == g_LcdState)
	{
		hw_lcd_back_off();
		g_LcdState = SYS_LCD_CLOSE;
		sys_set_hint_state(SYS_HINT_OPERATING, FALSE);
		hw_speak_off();								// 关喇叭
		hw_lcd_power_off();
	}
}

/*************************************************
  Function:			sys_key_beep
  Description: 		按键音
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_key_beep(void)
{
	#if 0
	// add by luofl 2014-08-28在清洁屏幕下不响按键音
	if (GUID_EXE_SCREEN == ui_get_active_guid())
	{
		return;
	}
	#endif

	if (g_CurMediaType != SYS_MEDIA_NONE)
	{
		return;
	}
	
	if (TRUE == storage_get_keykeep())
	{
		hw_key_beep();
	}
}

/*************************************************
  Function:			sys_init_hint_state
  Description: 		系统上电时初始化系统状态
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
void sys_init_hint_state(void)
{
	uint8 flg;

	//pthread_mutex_init(&g_MediaLock, NULL);
	
	// 获取信息未读状态
	flg = storage_get_msg_state();
	if (flg)
	{
		sys_set_hint_state(SYS_HINT_INFO, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_INFO, FALSE);
	}

	// 获取留影留言未读状态
	flg = storage_get_lylyrecord_flag();
	if (flg)
	{
		sys_set_hint_state(SYS_HINT_LEAVEWORD, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_LEAVEWORD, FALSE);
	}

	// 获取家人留言未读状态
	flg = storage_get_jrlyrecord_flag();
	if (flg == TRUE)
	{
		sys_set_hint_state(SYS_HINT_FAMILY, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_FAMILY, FALSE);
	}

	// 获取未接来电未读状态
	flg = storage_get_callrecord_state();
	if (flg == TRUE)
	{
		sys_set_hint_state(SYS_HINT_MISSED_CALLS, TRUE);
	}
	else
	{
		sys_set_hint_state(SYS_HINT_MISSED_CALLS, FALSE);
	}

	// 初始化免打扰-无免打扰
	storage_set_noface_enable(FALSE);
	sys_set_hint_state(SYS_HINT_NO_DISTURB, FALSE);

	// 获取安防状态
	flg = storage_get_alarm_state();
	// 撤防
	if (flg == 0)
	{
		sys_set_hint_state(SYS_HINT_ALARM_STATE, FALSE);
	}
	// 布防
	else if (flg == 1)
	{
		sys_set_hint_state(SYS_HINT_ALARM_STATE, TRUE);
	}
	// 报警, 有警情
	else
	{
		sys_set_hint_state(SYS_HINT_ALARM_WARNING, TRUE);
	}

	// 初始化系统为操作状态
	log_printf("sys_init_hint_state open or close lcd!\n");
	
	#ifdef _TIMER_REBOOT_NO_LCD_
	if (storage_get_openscreen())
	{
		g_LcdState = SYS_LCD_OPEN;
		timer_reboot_control(FALSE);
	}
	else
	#endif
	{
		sys_open_lcd();
	}
}

/*************************************************
  Function:			thread_mutex_init
  Description: 		线程锁初始化
  Input: 		
  Output:			无
  Return:			无
  Others:			
*************************************************/
void thread_mutex_init(void)
{
	pthread_mutex_init(&g_MediaLock, NULL);
}

