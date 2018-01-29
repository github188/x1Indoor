/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_monitor.c
  Author:    	chenbh
  Version:   	2.0
  Date: 
  Description:  监视
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_include.h"
#include "storage_include.h"

#define MONITOR_TIME_MAX		90					// 监视最长时间
#define	TALK_TIME_MAX			90					// 通话最长时间
#define	HEART_TIMEOUT			5					// 心跳超时

typedef struct
{
	int32 index;									// 监视设备ID
	DEVICE_TYPE_E DevType;							// 监视的设备类型
	MONITOR_STATE_E state;							// 当前监视状态
	uint16 TimeMax;									// 超时时间,最长监视时间, 秒, 0为无限长
	uint16 TimeOut;									// 计时,超时判断
	uint16 HeartTime;								// 心跳计时
	uint32 address;									// 对方地址
	uint16 port;									// 对方端口
	uint16 LocalAudioPort;							// 本地音频端口
	uint16 LocalVideoPort;							// 本地视频端口
	uint16 RemoteAudioPort;							// 远程音频端口, 通话用
	PFGuiNotify gui_notify;							// GUI 状态通知函数
	PFGuiNotify state_notify;						// GUI 状态通知函数
	struct ThreadInfo mThread;						// 线程
}MONITOR_INFO;

static uint32 g_ErrType;
static MONITOR_INFO g_MonitorInfo;
static MONITOR_STATE_E g_PreMonitorState = MONITOR_END;   // 改变前的状态
static DEVICE_NO	g_MoniDestDeviceNo;					  // 监视目标设备编号

char g_buf[3];
char g_data[66];
static uint8 g_Video_Start = 0;
static uint8 g_Audio_Start = 0;

static int16 g_RemainTime;
unsigned char g_mac[6] = {0};

#define GuiNotify(param1, param2)	if (g_MonitorInfo.gui_notify)\
	g_MonitorInfo.gui_notify(param1, param2)

#define StateNotify(param1, param2)	if (g_MonitorInfo.state_notify)\
	g_MonitorInfo.state_notify(param1, param2)

/*************************************************
  Function:			InterSetThread
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
static void inter_SetThread(struct ThreadInfo *thread)
{	
	if (thread)
	{
		thread->running = 0;
		thread->thread = -1;
	}
}

/*************************************************
  Function:				monitor_ini
  Description:			监视初始化
  Input: 	
  	1.GuiProc			GUI回调函数
  Output:				无
  Return:				
  Others:
*************************************************/
void monitor_ini(PFGuiNotify GuiProc)
{
	g_MonitorInfo.gui_notify = GuiProc;
	g_MonitorInfo.port = NETCMD_UDP_PORT;
	g_MonitorInfo.LocalAudioPort = NETAUDIO_UDP_PORT;
	g_MonitorInfo.LocalVideoPort = NETVIDEO_UDP_PORT;
	g_MonitorInfo.RemoteAudioPort = NETAUDIO_UDP_PORT;
}

/*************************************************
  Function:    		st_snap_state_callback
  Description:		抓拍回调
  Input: 
  	state:			0 抓拍出错 1 抓拍成功
  	err  :			出错类型 暂时不用
  Return:			
  Others:
*************************************************/
static void* st_snap_state_callback(int state, int err)
{	
	if (-1 == state)
	{
		log_printf(" snap err !!!\n");
	}
	else
	{
		log_printf(" snap ok !!!\n");
	}
}

/*************************************************
  Function:			monitor_fill_destdevno
  Description: 		监视目标设备编号填充
  Input: 		  	
  Output:			无
  Return:			BOOLEAN
  Others:
*************************************************/
static void monitor_fill_destdevno(void)
{
	#ifndef _AU_PROTOCOL_
	// mody by caogw 防止设备编号过长，数据溢出
	char tmp[10];
	char pcallno[20];
	char pcallroom[20];
	uint8 calltype;
	uint32 index = 0;
	PFULL_DEVICE_NO pdevno;
	calltype = g_MonitorInfo.DevType;
	index = g_MonitorInfo.index;
	memset(pcallno, 0, sizeof(pcallno));
	if (calltype == DEVICE_TYPE_AREA)
	{
		sprintf(pcallno, "%d", index);
	}
	else if (calltype == DEVICE_TYPE_STAIR)
	{
		pdevno = storage_get_devparam();
		memset(pcallroom, 0, sizeof(pcallroom));
		memcpy(pcallroom, pdevno->DeviceNoStr, pdevno->Rule.StairNoLen);
		sprintf(pcallno, "%s%d", pcallroom, index);
	}
	else
	{
		pdevno = storage_get_devparam();
		memset(pcallroom, 0, sizeof(pcallroom));
		memcpy(pcallroom, pdevno->DeviceNoStr, pdevno->DevNoLen-1);
		if (calltype == DEVICE_TYPE_DOOR_PHONE) // 模拟门前机
		{		
			if (index == 1)
			{
				sprintf(pcallno, "%s%d", pcallroom, 6);
			}
			else
			{
				sprintf(pcallno, "%s%d", pcallroom, 7);
			}
		}
		else  // 网络门前机
		{
			if (index == 1)
			{
				sprintf(pcallno, "%s%d", pcallroom, 8);
			}
			else
			{
				sprintf(pcallno, "%s%d", pcallroom, 9);
			}
		}
	}

	if (strlen(pcallno) > 9)
	{
		memcpy(tmp, pcallno, strlen(pcallno)-9);
		g_MoniDestDeviceNo.DeviceNo1 = atoi(tmp);
		memcpy(tmp, pcallno+strlen(pcallno)-9, 9);
		g_MoniDestDeviceNo.DeviceNo2 = atoi(tmp);
	}
	else
	{
		g_MoniDestDeviceNo.DeviceNo1 = 0;
		g_MoniDestDeviceNo.DeviceNo2 = atoi(pcallno);
	}
	memset(&g_MoniDestDeviceNo, 0, sizeof(DEVICE_NO));
	g_MoniDestDeviceNo.AreaNo = storage_get_areano();
	g_MoniDestDeviceNo.DeviceType =  g_MonitorInfo.DevType;
	#endif
}

#ifdef _DOOR_PHONE_
/*************************************************
  Function:			phone_monitor_proc
  Description: 		模拟门前机监视线程
  Input: 			
  	1.param			参数
  Output:			无
  Return:			BOOLEAN
  Others:
*************************************************/
static void* phone_monitor_proc(void *param)
{
	// 设置分离线程
	pthread_detach(pthread_self());
	time_t t0;
	//int32 RequestTimes = 3;  
	int32 ret = FALSE;
	static int32 times = 10;
	
	
LabChange:

	t0 = time(0);
	g_Video_Start = 0;
	//RequestTimes = 3;
	if (MONITOR_REQUEST == g_MonitorInfo.state)			
	{
		g_PreMonitorState = MONITOR_REQUEST;
		GuiNotify(g_MonitorInfo.state, g_MonitorInfo.index);
		g_MonitorInfo.state = MONITOR_MONITORING;  // 模拟门前机直接监视 
		usleep(1000);
		#if 0
		//  3次不成功就退出
		while (RequestTimes)
		{
			// 【开启监视接口】索引号传入
			//ret = [开启监视接口函数];
			if (ret == FALSE)
			{
				RequestTimes--;
			}
			else
			{
				g_MonitorInfo.state = MONITOR_MONITORING;
				break;
			}
		}
		if (RequestTimes <= 0)
		{
			g_MonitorInfo.state = MONITOR_END;
		}
		#endif
	}

	t0 = time(0);
	if(MONITOR_MONITORING == g_MonitorInfo.state)	
	{	
		log_printf(" *************** MONITOR_MONITORING ******************* \n");
		g_PreMonitorState = MONITOR_MONITORING;
		hw_mk_start();
		// [开启视频接口]
		ret = media_start_analog_video();
		if (ret == -1)
		{
			g_MonitorInfo.state = MONITOR_END;
			g_PreMonitorState = MONITOR_END;
		}
		else
		{
			GuiNotify(g_MonitorInfo.state, 0);	
			g_Video_Start = 1;
			
			g_MonitorInfo.TimeMax = MONITOR_TIME_MAX;	
			g_MonitorInfo.TimeOut = 0;
			g_RemainTime = 0;
		}
		
		while (MONITOR_MONITORING == g_MonitorInfo.state)
		{
			g_MonitorInfo.TimeOut = time(0) - t0;
			if (g_MonitorInfo.TimeMax > 0)
			{
				//剩余时间
				g_RemainTime = g_MonitorInfo.TimeMax - g_MonitorInfo.TimeOut;
				log_printf("g_RemainTime : %d\n ", g_RemainTime);
				if (g_RemainTime <= 0)
				{
					g_ErrType = MONITOR_MONITORING_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				else
				{
					//GuiNotify(MONITOR_TIMER, g_RemainTime);
				}

				times = 10;
				while ((times--) > 0 && MONITOR_MONITORING == g_MonitorInfo.state)
				{
					usleep(100*1000);
				}
			}
		}
	}

	t0 = time(0);
	if(MONITOR_TALKING == g_MonitorInfo.state)
	{
		log_printf("MONITOR_TALKING == g_MonitorInfo.state \n");

		// [监视进入通话接口]
		//hw_switch_analog();
		hw_mk_talk();
		
		g_PreMonitorState = MONITOR_TALKING;
		GuiNotify(g_MonitorInfo.state, 0);
		
		g_MonitorInfo.TimeMax = TALK_TIME_MAX;		
		g_MonitorInfo.TimeOut = 0;
		g_MonitorInfo.HeartTime = 0;
		
		while (MONITOR_TALKING == g_MonitorInfo.state)
		{
			if (g_MonitorInfo.TimeMax > 0)
			{
				g_RemainTime = 0;
				g_MonitorInfo.TimeOut = time(0) - t0;
				g_RemainTime = g_MonitorInfo.TimeMax - g_MonitorInfo.TimeOut;					
				if (g_RemainTime <= 0)
				{
					log_printf("monitor timer proc : talking time out\n");
					g_ErrType = MONITOR_TALKING_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				else
				{
					//GuiNotify(MONITOR_TIMER, g_RemainTime);
				}
					
				times = 10;
				while ((times--) > 0 && MONITOR_TALKING == g_MonitorInfo.state)
				{
					usleep(100*1000);
				}
			}
		}
	}

	// 监视下一个情况 
	if (MONITOR_REQUEST == g_MonitorInfo.state)
	{
		// [关闭视频接口]
		media_stop_analog_video();
		goto LabChange;
	}
	
	log_printf("monitor proc : AS_MONITOR_END : g_ErrType : %d\n", g_ErrType);
	media_stop_analog_video();	
	g_Video_Start = 0;
	hw_switch_digit();  // [关闭监视接口]
	sys_set_monitor_state(FALSE);
	log_printf("monitor proc end!\n");
	
	g_MonitorInfo.state = MONITOR_END;
	g_PreMonitorState = MONITOR_END;
	GuiNotify(g_MonitorInfo.state, g_ErrType);
	inter_SetThread(&g_MonitorInfo.mThread);

	pthread_exit(NULL);
	return NULL;             // 返回后资源由系统释放
}
#endif

/*************************************************
  Function:			monitor_proc
  Description: 		监视线程
  Input: 			
  	1.param			参数
  Output:			无
  Return:			BOOLEAN
  Others:
*************************************************/
static void* monitor_proc(void *param)
{
	// 设置分离线程
	pthread_detach(pthread_self());
	time_t t0;
	int32 size = 0;
	int32 ret = FALSE;
	static int32 times = 10;	
	
LabChange:
	g_Video_Start = 0;
	g_Audio_Start = 0;
	ret = media_start_net_video(g_MonitorInfo.address, _RECVONLY);
	if (ret == TRUE)
	{
		log_printf("monitor_proc : monitoring : start net video OK\n");
		g_Video_Start = 1;
	}
	
	t0 = time(0);
	if (MONITOR_REQUEST == g_MonitorInfo.state)			
	{
		g_PreMonitorState = MONITOR_REQUEST;
		monitor_fill_destdevno();
		GuiNotify(g_MonitorInfo.state, g_MonitorInfo.index);
		g_MonitorInfo.TimeOut = 0;
		g_MonitorInfo.HeartTime = 0;
		while (MONITOR_REQUEST == g_MonitorInfo.state)
		{
			memset(g_data, 0, sizeof(g_data));
			memcpy(g_data, &g_MonitorInfo.LocalVideoPort, 2);
			g_MonitorInfo.TimeOut = time(0) - t0;
			// 监视请求5s后没有回应退出
			if (g_MonitorInfo.TimeOut >= 5)
			{
				log_printf("monitor_timer_proc : request time out \n");
				g_ErrType = MONITOR_REQUEST_TIMEOUT;
				g_MonitorInfo.state = MONITOR_END;
				break;
			}

			// 以下每秒发送一次连接请求
			if (g_MonitorInfo.DevType == DEVICE_TYPE_DOOR_NET)							
			{
				// 监视网络门前机
				uint8 *tmp = NULL;
				MAC_TYPE MacType = DOOR1_MAC;
				if (g_MonitorInfo.index == 1)
				{
					MacType = DOOR1_MAC;
				}
				else
				{
					MacType = DOOR2_MAC;
				}
				
				tmp = storage_get_mac(MacType);
				memcpy(g_mac, tmp, 6);
				
				g_data[2] = DEVICE_TYPE_DOOR_NET;
				g_data[3] = (uint8)g_MonitorInfo.index;
				
				#ifdef _SEND_SDP_PARAM_
				memcpy(&g_data[4], (char *)(&g_venc_parm), 16);
				memcpy(&g_data[20], (char *)(&g_audio_parm), 8);
				memcpy(&g_data[28], g_mac, 6);
				size = 66;
				#else
				memcpy(&g_data[4], g_mac, 6);
				size = 10;
				#endif
				
				set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
				net_direct_send(CMD_MONITOR, g_data, size, g_MonitorInfo.address, g_MonitorInfo.port);
			}
			else
			{
				#ifdef _TY_STAIR_
				if (storage_get_extmode(EXT_MODE_GENERAL_STAIR))
				{
					g_data[2] = (uint8)g_MonitorInfo.DevType;
					g_data[3] = (uint8)g_MonitorInfo.index;
					
					#ifdef _SEND_SDP_PARAM_
					memcpy(&g_data[4], (char *)(&g_venc_parm), 16);
					memcpy(&g_data[20], (char *)(&g_audio_parm), 8);
					size = 66;
					#else
					size = 4;
					#endif
					
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_MONITOR, g_data, size, g_MonitorInfo.address, g_MonitorInfo.port);
				}
				else
				#endif
				{
					g_data[2] = (uint8)g_MonitorInfo.DevType;
					g_data[3] = (uint8)g_MonitorInfo.index;
					
					#ifdef _SEND_SDP_PARAM_
					memcpy(&g_data[4], (char *)(&g_venc_parm), 16);
					memcpy(&g_data[20], (char *)(&g_audio_parm), 8);
					size = 66;
					#else
					size = 4;
					#endif
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_MONITOR, g_data, size, g_MonitorInfo.address, g_MonitorInfo.port);
				}		
			}	

			// 睡眠时间会影响视频出来快慢
			times = 10;
			while ((times--) > 0 && MONITOR_REQUEST == g_MonitorInfo.state)
			{
				usleep(100*1000);
			}
		}

		// 解决链接转监视太快 UI切换失败问题
		if (time(0) - t0 < 2)
		{
			usleep(500*1000);
		}
	}

	t0 = time(0);
	if(MONITOR_MONITORING == g_MonitorInfo.state)	
	{
		g_MonitorInfo.TimeMax = MONITOR_TIME_MAX;	
		g_MonitorInfo.TimeOut = 0;
		g_MonitorInfo.HeartTime = 0;
		g_RemainTime = 0;
		while (MONITOR_MONITORING == g_MonitorInfo.state)
		{
			g_MonitorInfo.TimeOut = time(0) - t0;
			if (g_MonitorInfo.TimeMax > 0)
			{
				// 剩余时间
				g_RemainTime = g_MonitorInfo.TimeMax - g_MonitorInfo.TimeOut;
				log_printf("g_RemainTime : %d\n ", g_RemainTime);
				if (g_RemainTime <= 0)
				{
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_STOP_MONITOR, g_buf, 2, g_MonitorInfo.address, g_MonitorInfo.port);
					g_ErrType = MONITOR_MONITORING_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				else
				{
					//GuiNotify(MONITOR_TIMER, g_RemainTime);
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_MONITOR_HEART, g_buf, 2, g_MonitorInfo.address, g_MonitorInfo.port);
				}

				if (g_MonitorInfo.HeartTime > HEART_TIMEOUT)
				{
					g_ErrType = MONITOR_HEART_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				g_MonitorInfo.HeartTime++;

				times = 10;
				while ((times--) > 0 && MONITOR_MONITORING == g_MonitorInfo.state)
				{
					usleep(100*1000);
				}
			}
		}
	}

	if (MONITOR_TALKING == g_MonitorInfo.state)
	{
		if (media_start_net_audio(g_MonitorInfo.address))
		{						
			log_printf("media_start_net_audio return ok\n ");
			g_Audio_Start = 1;
			uint8 volume = storage_get_talkvolume();
			media_enable_audio_aec();
			media_set_talk_volume(g_MonitorInfo.DevType, volume);			
			media_add_audio_sendaddr(g_MonitorInfo.address, g_MonitorInfo.RemoteAudioPort);
		}	
		else
		{						
			log_printf(" media_start_net_audio return error\n ");
			g_MonitorInfo.state = MONITOR_END;
		}
	}
	
	t0 = time(0);
	if (MONITOR_TALKING == g_MonitorInfo.state)
	{				
		log_printf("MONITOR_TALKING == g_MonitorInfo.state \n");
		GuiNotify(g_MonitorInfo.state, 0);
		g_PreMonitorState = MONITOR_TALKING;
		g_MonitorInfo.TimeMax = TALK_TIME_MAX;		
		g_MonitorInfo.TimeOut = 0;
		g_MonitorInfo.HeartTime = 0;
		
		while (MONITOR_TALKING == g_MonitorInfo.state)
		{
			if (g_MonitorInfo.TimeMax > 0)
			{
				g_RemainTime = 0;
				g_MonitorInfo.TimeOut = time(0) - t0;
				// 剩余时间
				g_RemainTime = g_MonitorInfo.TimeMax - g_MonitorInfo.TimeOut;					
				if (g_RemainTime <= 0)
				{
					log_printf("monitor timer proc : talking time out\n");
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_STOP_MONITOR, g_buf, 2, g_MonitorInfo.address, g_MonitorInfo.port);
					g_ErrType = MONITOR_TALKING_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				else
				{
					//GuiNotify(MONITOR_TIMER, g_RemainTime);
					set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
					net_direct_send(CMD_MONITOR_HEART, g_buf, 2, g_MonitorInfo.address, g_MonitorInfo.port);
				}
				
				if (g_MonitorInfo.HeartTime > HEART_TIMEOUT)
				{
					g_ErrType = MONITOR_HEART_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
					break;
				}
				g_MonitorInfo.HeartTime++;
				
				times = 10;
				while ((times--) > 0 && MONITOR_TALKING == g_MonitorInfo.state)
				{
					usleep(100*1000);
				}
			}
		}
	}

	// 监视下一个情况 得回到监视请求
	if (MONITOR_REQUEST == g_MonitorInfo.state)
	{
		media_stop_net_video(_RECVONLY);
		goto LabChange;
	}

	log_printf("monitor proc : g_PreMonitorState : %d\n", g_PreMonitorState);

	// 关闭视频接口
	if (g_Video_Start == 1)
	{
		g_Video_Start = 0;
		media_stop_net_video(_RECVONLY);	
	}
	
	if (g_Audio_Start == 1)
	{
		media_del_audio_send_addr(g_MonitorInfo.address, MEDIA_AUDIO_PORT);
		usleep(10*1000);
		media_stop_net_audio();		
	}

	log_printf("monitor proc : AS_MONITOR_END : g_ErrType : %d\n", g_ErrType);
	sys_set_monitor_state(FALSE);
	log_printf("monitor proc end!\n");
	g_MonitorInfo.state = MONITOR_END;
	g_PreMonitorState = MONITOR_END;
	GuiNotify(g_MonitorInfo.state, g_ErrType);
	g_ErrType = MONITOR_OK;
	inter_SetThread(&g_MonitorInfo.mThread);

	pthread_exit(NULL);
	return NULL;             	// 返回后资源由系统释放
}

/*************************************************
  Function:		get_next_stair
  Description: 	监视下个梯口
  Input: 		
  	1.direct	监视下一个或上一个
  Output:		
  	1.ipaddr	梯口ip
  	2.index		梯口号索引
  Return:		无
  Others:
*************************************************/
static int32 get_next_stair(int32 direct, uint32 *ipaddr, int32 *index)
{
	int32 i, ID = -1;
	uint32 Address = 0;

	// 监视下一个
	if (direct)
	{
		if (g_MonitorInfo.index < (STAIR_DEVICE_NUM-1))
		{
			for (i = g_MonitorInfo.index + 1; i < STAIR_DEVICE_NUM; i++)
			{
				Address = get_stair_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
		
		// not fount
		if (-1 == ID)
		{
			for (i = 0; i < g_MonitorInfo.index; i++)
			{
				Address = get_stair_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
	}
	// 监视上一个
	else
	{
		if (g_MonitorInfo.index > 0)
		{
			for (i = g_MonitorInfo.index-1; i > -1; i--)
			{
				Address = get_stair_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
		
		// not found
		if (-1 == ID)
		{
			for (i = STAIR_DEVICE_NUM - 1; i > g_MonitorInfo.index; i--)
			{
				//Address = get_stair_address_inlist(ID);
				Address = get_stair_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
	}
	
	if (-1 == ID || Address == 0)
	{
		log_printf("get next stiar : don't get next stair\n");
		return FALSE;
	}
	else
	{
		*ipaddr = Address;
		*index = ID;
		log_printf("get next stiar : ipaddr:0x%x, index:%d\n", *ipaddr, *index);
		return TRUE;
	}
}

/*************************************************
  Function:		get_next_stair
  Description: 	监视下个区口
  Input: 		
  	1.direct	监视下一个或上一个
  Output:		
  	1.ipaddr	梯口ip
  	2.index		梯口号索引
  Return:		无
  Others:
*************************************************/
static int32 get_next_area(int32 direct, uint32 *ipaddr, int32 *index)
{
	int32 i, ID = -1;
	uint32 Address = 0;

	if (direct)
	{
		if (g_MonitorInfo.index < AREA_DEVICE_NUM)
		{
			for (i = g_MonitorInfo.index + 1; i <= AREA_DEVICE_NUM; i++)
			{
				Address = get_area_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
		if (-1 == ID)
		{
			for (i = 1; i < g_MonitorInfo.index; i++)
			{
				Address = get_area_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
	}
	else
	{
		if (g_MonitorInfo.index > 1)
		{
			for (i = g_MonitorInfo.index - 1; i > 0; i--)
			{
				Address = get_area_address_inlist(i);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
		if (-1 == ID)
		{
			for (i = AREA_DEVICE_NUM; i > g_MonitorInfo.index; i--)
			{
				Address = get_area_address_inlist(ID);
				if (0 != Address)
				{
					ID = i;
					break;
				}
			}
		}
	}
	
	if (-1 == ID || Address == 0)
	{
		log_printf("get next area : don't get next area\n");
		return FALSE;
	}
	else
	{
		*ipaddr = Address;
		*index = ID;
		log_printf("get next area : ipaddr:0x%x, index:%d\n", *ipaddr, *index);
		return TRUE;
	}
}

/*************************************************
  Function:		get_next_netdoor
  Description: 	监视下个梯口
  Input: 		
  	1.direct	监视下一个或上一个
  Output:		
  	1.ipaddr	梯口ip
  	2.index		梯口号索引
  Return:		无
  Others:
*************************************************/
static int32 get_next_netdoor(int32 direct, uint32 *ipaddr, int32 *index)
{
	int32 ID = -1;
	uint32 Address = 0;

	if (g_MonitorInfo.index == 1)
	{
		Address = get_netdoor_address_inlist(2);
		#ifdef _IP_MODULE_DJ_
		set_netdoor_ip(EXT_MODE_NETDOOR2, Address);
		#endif
		if (Address != 0)
		{	
			ID = 2;
		}
	}
	else
	{
		Address = get_netdoor_address_inlist(1);
		#ifdef _IP_MODULE_DJ_
		set_netdoor_ip(EXT_MODE_NETDOOR1, Address);
		#endif
		if (Address != 0)
		{	
			ID = 1;
		}
	}
	
	if (-1 == ID || Address == 0)
	{
		log_printf("get next netdoor : don't get next netdoor\n");
		return FALSE;
	}
	else
	{
		*ipaddr = Address;
		*index = ID;
		log_printf("get next netdoor : ipaddr:0x%x, index:%d\n", *ipaddr, *index);
		return TRUE;
	}
}

/*************************************************
  Function:		get_next_phonedoor
  Description: 	监视下个模拟门前机
  Input: 		
  	1.direct	监视下一个或上一个
  Output:		
  	1.ipaddr	梯口ip
  	2.index		梯口号索引
  Return:		无
  Others:
*************************************************/
static int32 get_next_phonedoor(int32 direct, uint32 *ipaddr, int32 *index)
{
	int32 ret = 0;
	if (g_MonitorInfo.index == 1)
	{
		ret = get_phonedoor_address_inlist(2);
		if (ret == 0)
		{
			return FALSE;
		}
		else
			*index = 2;
	}
	else
	{
		ret = get_phonedoor_address_inlist(1);
		if (ret == 0)
		{
			return FALSE;
		}
		else
			*index = 1;
	}
	
	return TRUE;
}

/*************************************************
  Function:			monitor_start
  Description:		开启监视
  Input: 	
   	1.DevType		设备类型
  	2.index			设备索引 从0开始
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_start(DEVICE_TYPE_E DevType, int8 index)
{
	PMONITORINFO info = NULL;
	log_printf(" devtype :%d , index : %d \n", DevType, index);
	int32 ret = sys_set_monitor_state(TRUE);
	if (ret != 0)
	{
		log_printf("monitor search dev : sys is busy : state:%d\n", ret);
		return FALSE;
	}
	
	info = storage_get_monitorinfo(DevType, index);
	if (info == NULL)
	{
	 	log_printf(" storage_get_monitorinfo return NULL \n");
		sys_set_monitor_state(FALSE);	// add by caogw 2015-9-25 解决搜索列表失败时，重置媒体状态
		return FALSE;
	}
	
	g_MonitorInfo.DevType = info->DeviceType;
	g_MonitorInfo.address = info->IP;
	g_MonitorInfo.index = info->index;
	free(info);	
	info = NULL;
	log_printf("DevType : %x, address: %x, index: %d\n", g_MonitorInfo.DevType, g_MonitorInfo.address, g_MonitorInfo.index);

	// 监视状态
	g_MonitorInfo.state = MONITOR_REQUEST;

	switch (g_MonitorInfo.DevType)
	{
		#ifdef _DOOR_PHONE_
		case DEVICE_TYPE_DOOR_PHONE:
		{ 
			if (0 != inter_start_thread(&g_MonitorInfo.mThread, phone_monitor_proc, (void*)&g_MonitorInfo, index))
			{
				g_MonitorInfo.state = MONITOR_END;
				return FALSE;
			}
			break;
		}
		#endif

		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_AREA:
		case DEVICE_TYPE_STAIR:
		{
			if (0 != inter_start_thread(&g_MonitorInfo.mThread, monitor_proc, (void*)&g_MonitorInfo, index))
			{
				g_MonitorInfo.state = MONITOR_END;
				return FALSE;
			}
			break;
		}

		default:
			log_printf(" monitor devtype is not support!!!!!! \n");
			return FALSE;
	}
	return TRUE;
}

/*************************************************
  Function:			monitor_next
  Description:		监视切换
  Input: 	
  	1.direct		方向: TRUE-下一个 FALSE-上一个
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_next(int32 direct)
{
	uint32 ipaddr = 0;
	int32 index = 0;
	int32 ret = FALSE;
	
	if (g_MonitorInfo.state != MONITOR_MONITORING)
	{
		log_printf("monitor next : state err : %d\n", g_MonitorInfo.state);
		return FALSE;
	}

	switch (g_MonitorInfo.DevType)
	{
		case DEVICE_TYPE_STAIR:
			ret = get_next_stair(direct, &ipaddr, &index);			
			break;
			
		case DEVICE_TYPE_AREA:
			ret = get_next_area(direct, &ipaddr, &index);
			break;
			
		case DEVICE_TYPE_DOOR_NET:
			ret = get_next_netdoor(direct, &ipaddr, &index);
			break;

		case DEVICE_TYPE_DOOR_PHONE:
			ret = get_next_phonedoor(direct, NULL, &index);
			break; 

		default:
			log_printf("DevType[ %x] is not true\n", g_MonitorInfo.DevType);
			break;
	}

	// 模拟门前机 特殊处理
	if (DEVICE_TYPE_DOOR_PHONE == g_MonitorInfo.DevType)
	{
		if (ret == TRUE)
		{
			// 监视下个模拟门前机
			g_MonitorInfo.index = index;
			g_MonitorInfo.state = MONITOR_REQUEST;  // 模拟门前机直接监视 
			return TRUE;
		}
		return FALSE;
	}

	if (ipaddr == 0)
	{
		return FALSE;
	}
	else
	{
		set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
		net_direct_send(CMD_STOP_MONITOR, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port);
		
		g_MonitorInfo.address = ipaddr;
		g_MonitorInfo.index = index;
		g_MonitorInfo.state = MONITOR_REQUEST;
		return TRUE;
	}
}

/*************************************************
  Function:			monitor_change
  Description:		切换到监视某一个设备
  Input: 	
  	1.index			设备索引
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_change(int32 index)
{
	uint32 ipaddr = 0;
	
	if (g_MonitorInfo.state != MONITOR_MONITORING)
	{
		return FALSE;
	}

	switch (g_MonitorInfo.DevType)
	{
		case DEVICE_TYPE_STAIR:
			ipaddr = get_stair_address(index);
			break;
			
		case DEVICE_TYPE_AREA:
			ipaddr = get_area_address(index);
			break;
			
		case DEVICE_TYPE_DOOR_NET:
			ipaddr = get_door_address(index);
			break;
			
		case DEVICE_TYPE_DOOR_PHONE:
			ipaddr = get_phonedoor_address(index);
			break;
			
		default:
			log_printf("monitor change : dev type err\n");
			return FALSE;
	}

	// 模拟门前机特殊处理
	if (DEVICE_TYPE_DOOR_PHONE == g_MonitorInfo.DevType)
	{
		if (ipaddr == TRUE)
		{
			// 监视下个模拟门前机
			g_MonitorInfo.index = index;
			g_MonitorInfo.state = MONITOR_REQUEST;
			return TRUE;
		}
		return FALSE;
	}

	if (ipaddr != 0)
	{
		set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
		net_direct_send(CMD_STOP_MONITOR, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port);
		
		g_MonitorInfo.index = index;
		g_MonitorInfo.address = ipaddr;
		g_MonitorInfo.state = MONITOR_REQUEST;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*************************************************
  Function:			monitor_talk
  Description:		监视转通话
  Input: 	
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_talk(void)
{
	char * data = (char *)&g_MonitorInfo.LocalAudioPort;			
	if (g_MonitorInfo.state != MONITOR_MONITORING)
	{
		return FALSE;
	}

	if (g_MonitorInfo.DevType == DEVICE_TYPE_DOOR_PHONE)
	{
		g_MonitorInfo.state = MONITOR_TALKING;
	}
	else
	{
		set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
		net_direct_send(CMD_MONITOR_TO_TALK, data, 2, g_MonitorInfo.address, g_MonitorInfo.port);
	}
	return TRUE;
}

/*************************************************
  Function:			ui_monitor_stop
  Description:		结束监视或通话 UI按挂机调用该接口
  Input: 	
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 ui_monitor_stop(void)
{
	if (g_MonitorInfo.state == MONITOR_END)
	{
		if (g_MonitorInfo.mThread.running == 0)
		{
			sys_set_monitor_state(FALSE);
			GuiNotify(g_MonitorInfo.state, MONITOR_OK);
		}
	}
	else if (g_MonitorInfo.state != MONITOR_END)
	{
		g_ErrType = MONITOR_OK;

		if (g_MonitorInfo.DevType == DEVICE_TYPE_DOOR_PHONE)
		{
			g_MonitorInfo.state = MONITOR_END;
		}
		else
		{
			set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
			net_direct_send(CMD_STOP_MONITOR, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port);		
		}
		
		g_MonitorInfo.state = MONITOR_END;
	}
	return TRUE;
}

/*************************************************
  Function:			monitor_stop
  Description:		结束监视或通话
  Input: 	
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_stop(void)
{
	if (g_MonitorInfo.state != MONITOR_END)
	{
		g_ErrType = MONITOR_OK;

		if (g_MonitorInfo.DevType == DEVICE_TYPE_DOOR_PHONE)
		{
			g_MonitorInfo.state = MONITOR_END;
		}
		else
		{
			set_nethead(g_MoniDestDeviceNo, PRIRY_DEFAULT);
			net_direct_send(CMD_STOP_MONITOR, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port);		
		}
		
		g_MonitorInfo.state = MONITOR_END;
	}
	return TRUE;
}

/*************************************************
  Function:			monitor_video_snap
  Description:		抓拍
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 monitor_video_snap(void)
{
	uint32 ret;
	char FileName[50] = {0};
	ZONE_DATE_TIME DateTime;
	
	log_printf("[%d]\n", __LINE__);	
	get_timer(&DateTime);
	get_photo_path(FileName, &DateTime);
	if (g_MonitorInfo.state == MONITOR_MONITORING || g_MonitorInfo.state == MONITOR_TALKING)
	{
		ret = media_snapshot(FileName, st_snap_state_callback, g_MonitorInfo.DevType);
		if (ret == TRUE)
		{
			char DevStr[20] = {0};
			if (g_MonitorInfo.DevType == DEVICE_TYPE_STAIR)
			{
				char StairNo[5] = {0};
				memset(StairNo, 0, sizeof(StairNo));
				sprintf(StairNo, "%d", g_MonitorInfo.index);
				get_stair_fullno(DevStr, StairNo);		
			}
			else
			{
				sprintf(DevStr, "%d", g_MonitorInfo.index);
			}	
			storage_add_photo(g_MonitorInfo.DevType, DevStr, DateTime);
		}	
	}	
	else
	{
		ret = FALSE;
	}
	return ret;
}

/*************************************************
  Function:			monitor_unlock
  Description:		室内机监视开锁
  Input: 			无
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
int32 monitor_unlock(void)
{
	int32 ret = 0;
	switch (g_MonitorInfo.state)
	{
		case MONITOR_MONITORING:
		case MONITOR_TALKING:
		{	
			#ifdef _DOOR_PHONE_
			if (g_MonitorInfo.DevType == DEVICE_TYPE_DOOR_PHONE)
			{
				int32 ret1 = hw_mk_lock();
				if (ret1 == 0)
				{
					ret = TRUE;
				}
				else
				{
					ret = FALSE;
				}
			}
			else
			#endif
			{
				uint8 EchoValue;
				#if 0     // modi by chenbh 和被叫一致开锁命令不用应答
				int32 ret1 = net_send_command(CMD_UNLOCK, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port, 2, &EchoValue, NULL, NULL);
				if (ret1 && EchoValue == ECHO_OK)
				{
					ret = TRUE;
				}
				else
				{
					ret = FALSE;
				}
				#else
				set_nethead(g_MoniDestDeviceNo, PRIRY_REALTIME);
				net_direct_send(CMD_UNLOCK, NULL, 0, g_MonitorInfo.address, g_MonitorInfo.port);	
				ret = TRUE;
				#endif
			}
			break;
		}
		default:
			ret = FALSE;
			break;
	}
	return ret;
}

/*************************************************
  Function:		fill_devno_by_index
  Description: 	填充设备编号
  Input: 		
  	1.DevType   设备类型
  	2.index		设备索引
  Output:		无
  Return:		
  Others:		
*************************************************/
void fill_devno_by_index(DEVICE_TYPE_E DevType, int8 index, char *devno)
{
	if (DEVICE_TYPE_AREA == DevType || DEVICE_TYPE_DOOR_NET == DevType || DEVICE_TYPE_DOOR_PHONE == DevType)
	{
		sprintf(devno, "%d", index);
	}
	else if (DEVICE_TYPE_STAIR == DevType)
	{
		PFULL_DEVICE_NO pdevno;
		// mody by caogw 增加数组空间，预防数据溢出
		char stairno[10] = {0};
		pdevno = storage_get_devparam();
		memset(stairno, 0, sizeof(stairno));
		memcpy(stairno, pdevno->DeviceNoStr, pdevno->Rule.StairNoLen);
		sprintf(devno, "%s%05d", stairno, index);
	}
	log_printf("fill_devno_by_index  devno :%s \n", devno);
}

/*************************************************
  Function:		get_monitor_sync_devlist
  Description: 	获取设备列表
  Input: 		无
  Output:		无
  Return:		
  Others:		
*************************************************/
static uint32 get_monitor_sync_devlist (void)
{
	int32 ret = FALSE;

	log_printf("monitor proc : search dev start\n");
	
	g_MonitorInfo.state = MONITOR_SEARCH;

	switch (g_MonitorInfo.DevType)
	{
		case DEVICE_TYPE_STAIR:
			ret = search_stair_list();
			break;
		case DEVICE_TYPE_AREA:
			ret = search_area_list();
			break;
		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_DOOR_PHONE:
			ret = search_door_list();
			break;
		default:	
			StateNotify(MONITOR_GETLIST, FALSE);
			g_MonitorInfo.state = MONITOR_END;
			return FALSE;
	}

	if (ret == TRUE)
	{		
		StateNotify(MONITOR_GETLIST, TRUE);
	}
	else
	{
		StateNotify(MONITOR_GETLIST, FALSE);
	}
	
	g_MonitorInfo.state = MONITOR_END;
	return ret;
}

/*************************************************
  Function:		rtsp_monitor_sync_devlist
  Description: 	获取RTSP设备列表
  Input: 		无
  Output:		无
  Return:		PMonitorDeviceList 设备列表
  Others:		
*************************************************/
uint32 monitorlist_sync_devlist(PFGuiNotify GuiProc, DEVICE_TYPE_E DevType)
{
	g_MonitorInfo.state_notify = GuiProc;
	g_MonitorInfo.DevType = DevType;
	get_monitor_sync_devlist();
	return 0;
}


/*************************************************
  Function:			monitor_distribute
  Description:		监视命令发送包处理函数
  Input: 	
  	1.recPacket		接受网络包
  Output:			无
  Return:			成功与否,true/fasle
  Others:
*************************************************/
int32 monitor_distribute(const PRECIVE_PACKET recPacket)
{
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	uint16 cmd = head->command | SSC_INTERPHONE << 8;
	switch (cmd)
	{	
		case CMD_STOP_MONITOR:											//中断监视
			if (g_MonitorInfo.address == recPacket->address)	
			{	
				// 停止监视接口
				log_printf(" recv cmd:  CMD_STOP_MONITOR\n");
				net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
				g_MonitorInfo.state = MONITOR_END;
				//monitor_stop();
				
			}
			break;
			
		case CMD_MONITOR_HEART:											//监视握手（心跳）
			if (g_MonitorInfo.address == recPacket->address)
			{
				g_MonitorInfo.HeartTime = 0;
			}
			break;
			
		default:
			return FALSE;
	}
	return TRUE;
}

/*************************************************
  Function:			monitor_responsion
  Description:		监视命令应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void monitor_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket)
{
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	uint16 cmd = head->command | SSC_INTERPHONE << 8;
	switch (cmd)
	{	
		case CMD_MONITOR:							// 监视
			if (g_MonitorInfo.state == MONITOR_REQUEST)
			{
				log_printf("monitor net response : CMD_MONITOR : EchoValue : %d\n", head->EchoValue);
				if (head->EchoValue == ECHO_OK)
				{
					g_MonitorInfo.state = MONITOR_MONITORING;
					g_PreMonitorState = MONITOR_MONITORING;
					GuiNotify(g_MonitorInfo.state, 0);	
				}
				else if(head->EchoValue == ECHO_BUSY)
				{
					g_ErrType = MONITOR_BUSY;
					g_MonitorInfo.state = MONITOR_END;
				}
				else
				{
					g_ErrType = MONITOR_REQUEST_TIMEOUT;
					g_MonitorInfo.state = MONITOR_END;
				}
			}
			break;
			
		case CMD_MONITOR_TO_TALK:					// 监视转通话
			log_printf("monitor net response : CMD_MONITOR_TO_TALK : echo:%d, curstate:%d\n", head->EchoValue, g_MonitorInfo.state);
			if (g_MonitorInfo.state == MONITOR_MONITORING)
			{
				if (head->EchoValue == ECHO_OK)
				{
					if (DEVICE_TYPE_DOOR_NET == g_MonitorInfo.DevType)
					{
						set_audio_pack_mode(1);		// GM8126门前机应答是单包,实际是要组包发过去,否则会无声音
					}
					else
					{
						set_audio_pack_mode(head->Encrypt);
					}

					#if 0
					if (media_start_net_audio(g_MonitorInfo.address))
					{						
						if (g_MonitorInfo.state == MONITOR_MONITORING)
						{
							log_printf("media_start_net_audio return ok\n ");
							g_MonitorInfo.state = MONITOR_TALKING;
							GuiNotify(g_MonitorInfo.state, 0);
							uint8 volume = storage_get_talkvolume();
							//media_set_output_volume(volume);
							media_set_talk_volume(g_MonitorInfo.DevType, volume);
							// add by luofl 2011-12-07 增加咪头输入设置
							//media_set_input_volume(storage_get_micvolume());
							g_MonitorInfo.RemoteAudioPort = *((uint16 *)(recPacket->data + NET_HEAD_SIZE));
							media_add_audio_sendaddr(g_MonitorInfo.address, g_MonitorInfo.RemoteAudioPort);
						}
						else
						{
							usleep(500*1000);
							media_stop_net_audio();
						}
						
					}	
					else
					{						
						log_printf(" media_start_net_audio return error\n ");
					}
					#else
					g_MonitorInfo.state = MONITOR_TALKING;
					g_MonitorInfo.address = recPacket->address;
					g_MonitorInfo.RemoteAudioPort = *((uint16 *)(recPacket->data + NET_HEAD_SIZE));
					#endif
				}
				// mody by caogw 2016-11-11 防止对方过来别的错误，导致UI显示错乱问题
				//else if (head->EchoValue == ECHO_BUSY)
				else
				{
					GuiNotify(g_MonitorInfo.state, MONITOR_BUSY);
				}
			}
			break;
			
		case CMD_STOP_MONITOR:						// 中断监视
			break;
			
		default:
			break;
	}
}

#if MONITOR_FAX_IP
/*************************************************
  Function:			monitor_request
  Description:		监视请求
  Input: 	
   	1.ID			应用ID
  	2.DevType		设备类型
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 test_monitor_start(void)
{
	//log_printf("test_monitor_start 11111111111111111111\n ");
	int32 ret = sys_set_monitor_state(TRUE);
	if (ret != 0)
	{
		log_printf("monitor search dev : sys is busy : state:%d\n", ret);
		return FALSE;
	}
	
	g_MonitorInfo.DevType = DEVICE_TYPE_STAIR;
	g_MonitorInfo.address = 0XA6E1090;      // ip 10.110.16.176
	g_MonitorInfo.index = 0;

	g_MonitorInfo.state = MONITOR_REQUEST;
	if (0 != inter_start_thread(&g_MonitorInfo.mThread, monitor_proc, (void*)&g_MonitorInfo, 1))
	{
		g_MonitorInfo.state = MONITOR_END;
		return false;
	}
	return TRUE;
}
#endif

