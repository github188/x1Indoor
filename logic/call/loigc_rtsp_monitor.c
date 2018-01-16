/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	loigc_rtsp_monitor.c
  Author:    	
  Version:   	1.0
  Date: 
  Description:  RTSP监视
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_include.h"
#include "storage_include.h"

#define DEFAULT_VIDEO_FMT						H264			// 码流格式
#define DEFAULT_VIDEO_IMGSIZE					CIF				// 视频分辨率
#define DEFAULT_VIDEO_FRAMERATE					15				// 帧率
#define DEFAULT_VIDEO_BITRATE					512				// 码率

#define DEFAULT_SELFIPC_VIDEO_FMT				H264
#define DEFAULT_SELFIPC_VIDEO_IMGSIZE			VGA
#define DEFAULT_SELFIPC_VIDEO_FRAMERATE			15
#define DEFAULT_SELFIPC_VIDEO_BITRATE			768//512

#define DEFAULT_HIKVISION_VIDEO_FMT				H264
#define DEFAULT_HIKVISION_VIDEO_IMGSIZE			CIF
#define DEFAULT_HIKVISION_VIDEO_FRAMERATE		15
#define DEFAULT_HIKVISION_VIDEO_BITRATE			512

#define MONITOR_TIME_MAX						60				// 监视最长时间
#define MONITOR_REQUEST_TIME					7				// 请求时间10秒
#define MONITOR_HEART_TIME						5				// 5秒一次心跳
#define MONIROT_HEART_MAX						3				// 3次心跳

typedef struct
{
	int32 index;									// 监视设备ID
	int32 type;
	MONITOR_STATE_E state;							// 当前监视状态
	uint16 TimeMax;									// 超时时间,最长监视时间, 秒, 0为无限长
	uint16 TimeOut;									// 计时,超时判断
	uint16 HeartTime;								// 心跳计时
	uint16 SendCmd;									// 是否发送命令 0 不需要发 1 发送 
	uint16 Recv;
	uint16 SPS;							
	PFGuiNotify gui_notify;							// GUI 状态通知函数
	PFGuiNotify state_notify;						// GUI 状态通知函数
	struct ThreadInfo mThread;						// 线程
}RTSP_MONITOR_INFO;

static uint32 g_ErrType;
static struct ThreadInfo g_mThread;					// 获取列表线程
static RTSP_MONITOR_INFO g_RtspMonItorInfo;
static PRtspDeviceList g_RtspMonItoRDevList = NULL;
static MONITOR_STATE_E g_PreRtspMonitorState = MONITOR_END;   	

#define RtspGuiNotify(param1, param2)	if (g_RtspMonItorInfo.gui_notify)\
	g_RtspMonItorInfo.gui_notify(param1, param2)

#define RtspStateNotify(param1, param2)	if (g_RtspMonItorInfo.state_notify)\
	g_RtspMonItorInfo.state_notify(param1, param2)

#ifdef _NEW_SELF_IPC_
#define IPC_HEART_TIME			90					// 心跳超时时间s

typedef struct
{
	SelfIPCamera IPList;
	uint32 TimeOut;
}SelfIPCameraList, *PSelfIPCameraList;

static SelfIPCameraList g_ipc_list[MAX_HOME_NUM];	// IPC已保持列表
static PNewMonitorDeviceList g_CommDevlist = NULL;	// 社区列表
static uint32 g_MainDevIP = 0;						// 获得的主机IP
#endif
static uint32 g_BestIP = 0;							// 最优服务器

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
  Function:		rtsp_monitor_state
  Description: 	
  Input: 		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 rtsp_monitor_state (void)
{
	if (MONITOR_END != g_RtspMonItorInfo.state)
	{
		return TRUE;
	}

	return FALSE;
}

/*************************************************
  Function:		rtsp_monitor_sync_devlist
  Description: 	获取RTSP设备列表
  Input: 		无
  Output:		无
  Return:		
  Others:		
*************************************************/
static void* rtsp_get_monitor_sync_devlist (void *param)
{
	pthread_detach(pthread_self());
	int i;
    int ret = 0;
    char RecData[10000] = {0};
	int ReciSize, ComDevNum;
	uint8 EchoValue;
    char data[10] = {0};
	uint32 rtsp_ip = storage_get_netparam_bytype(RTSP_IPADDR); 
	PFULL_DEVICE_NO dev = storage_get_devparam();
	PMonitorDeviceList g_list;
	
    memcpy(data, &dev->AreaNo, 4);
	ReciSize = 0;
	EchoValue = ECHO_ERROR;

	// add by chenbh 2016-03-09 搜索设备时 将原来列表数据清空
	storage_free_monitordev(COMMUNITY_CAMERA);
	
	// modi by chenbh 2016-06-23 如果没有设置rtsp ip则直接从中心服务器获取设备列表
	#if 1
	if (0 == rtsp_ip)
	{
		rtsp_ip = net_get_center_ip();
	}	
	#else
	// add by chenbh 2015-09-28 配合新款上位机 需要先从服务器获取最优服务器，再从这个服务器获取监视列表
	if (0 == rtsp_ip)
	{
		// add by chenbh 2015-10-09 获取最优RTSP服务器
		uint32 SendData = 0x01;  // 获取RTSP 服务器
		uint32 CenterIP = net_get_center_ip();
		set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
		ret = net_send_command(CMD_GET_SUIT_SERVER, &SendData, 4, CenterIP, NETCMD_UDP_PORT, 2, &EchoValue, RecData, &ReciSize);
		if (TRUE == ret)
		{
			if (EchoValue == ECHO_OK)
			{
				memcpy(&g_Rtsp_ServerInfo.deviceno, (PDEVICE_NO)RecData, sizeof(DEVICE_NO));
				g_Rtsp_ServerInfo.ip = *(uint32 *)(RecData+12);
				g_Rtsp_ServerInfo.comm_port = *(uint32 *)(RecData+16);
				g_Rtsp_ServerInfo.rtsp_port = *(uint32 *)(RecData+20);					
			}
		}
		else
		{
			memset(&g_Rtsp_ServerInfo, 0, sizeof(RTSP_SERVER_INFO));
		}

		log_printf("g_Rtsp_ServerInfo.ip :%x, g_Rtsp_ServerInfo.rtsp_port : %d\n", g_Rtsp_ServerInfo.ip, g_Rtsp_ServerInfo.rtsp_port);
		rtsp_ip = g_Rtsp_ServerInfo.ip;
	}
	#endif

	memset(RecData, 0, sizeof(RecData));

	// add by chenbh 2016-03-09 SELF_IPC有多了一个在线状态
	#ifdef _NEW_SELF_IPC_ 
	storage_free_monitordev_memory(&g_CommDevlist);		// 先清空临时存储的数据
	ret = net_send_command(CMD_GET_AREA_CAMERALIST_EXT, data, 4, rtsp_ip, NETCMD_UDP_PORT, 3, &EchoValue, &RecData, &ReciSize);		
	if (TRUE == ret)
	{
		if (EchoValue == ECHO_OK && ReciSize > 4)
		{
			ComDevNum = (ReciSize-4)/sizeof(NEWCOMMUNITYDEVICE);
			if (ComDevNum == 0)
			{
				storage_malloc_monitordev_memory(0, ComDevNum, &g_list);
				if (ECHO_STORAGE_OK ==  storage_save_monitordev(COMMUNITY_CAMERA, g_list))
				{
					ret = TRUE;
				}
			}
			else
			{
				if ((ReciSize-4) == ComDevNum*sizeof(NEWCOMMUNITYDEVICE))
				{
					storage_malloc_monitordev_memory(0, ComDevNum, &g_list);
					if(NULL == g_list || NULL == g_list->Comdev)
					{
						ret = FALSE;
						goto err;
					}

					// add by chenbh 2016-03-09 将数据存到临时区 UI获取在线状态时用
					storage_free_monitordev_memory(&g_CommDevlist);		
					storage_malloc_new_monitordev_memory(0, ComDevNum, &g_CommDevlist);
					if(NULL != g_CommDevlist && NULL != g_CommDevlist->Comdev)
					{
						memcpy(g_CommDevlist->Comdev, RecData+4, sizeof(NEWCOMMUNITYDEVICE)*ComDevNum);
						g_CommDevlist->Comnum = ComDevNum;
					}
					
					g_list->Comnum = ComDevNum;
					for (i = 0; i < ComDevNum; i++)
					{
						// 结构体还是用以前的 不过记得偏移长度为新结构体长度
						NEWCOMMUNITYDEVICE* PNewComdev = (NEWCOMMUNITYDEVICE *)(RecData+4+sizeof(NEWCOMMUNITYDEVICE)*i);
						g_list->Comdev[i].DeviceIP = PNewComdev->DeviceIP;
						g_list->Comdev[i].DevPort = PNewComdev->DevPort;
						g_list->Comdev[i].CanControlPTZ = PNewComdev->CanControlPTZ;
						g_list->Comdev[i].ChannelNumber = PNewComdev->ChannelNumber;
						memset(g_list->Comdev[i].DeviceName, 0, sizeof(g_list->Comdev[i].DeviceName));
						memset(g_list->Comdev[i].FactoryName, 0, sizeof(g_list->Comdev[i].FactoryName));
						memcpy(g_list->Comdev[i].DeviceName, PNewComdev->DeviceName, sizeof(g_list->Comdev[i].DeviceName));
						memcpy(g_list->Comdev[i].FactoryName, PNewComdev->FactoryName, sizeof(g_list->Comdev[i].FactoryName));
						
						log_printf("***********************************************\n");
						log_printf("g_index %x\n",i);
						log_printf("g_list->Comdev[i].DeviceName %s \n",g_list->Comdev[i].DeviceName);
						log_printf("g_list->Comdev[i].DeviceIP %x \n",g_list->Comdev[i].DeviceIP);
						log_printf("g_list->Comdev[i].DevPort %x \n",g_list->Comdev[i].DevPort);
						log_printf("g_list->Comdev[i].FactoryName %s \n",g_list->Comdev[i].FactoryName);	
						log_printf("g_list->Comdev[i].isOnLine %d \n",PNewComdev->isOnLine);							
						log_printf("***********************************************\n");
					}
					
					if (ECHO_STORAGE_OK ==  storage_save_monitordev(COMMUNITY_CAMERA, g_list))
					{
						ret = TRUE;
					}					
				}
				else
				{
					ret = FALSE;
				}
			}
		}
        else
        {
            ret	= FALSE;
        }        
	}	
	else
	#endif
	{
		ret = net_send_command(CMD_GET_AREA_CAMERALIST, data, 4, rtsp_ip, NETCMD_UDP_PORT, 3, &EchoValue, &RecData, &ReciSize);		
		if (TRUE == ret)
		{
			if (EchoValue == ECHO_OK && ReciSize > 4)
			{
				ComDevNum = (ReciSize-4)/sizeof(COMMUNITYDEVICE);
				if (ComDevNum == 0)
				{
					storage_malloc_monitordev_memory(0, ComDevNum, &g_list);
					if (ECHO_STORAGE_OK ==  storage_save_monitordev(COMMUNITY_CAMERA, g_list))
					{
						ret = TRUE;
					}
				}
				else
				{
					if ((ReciSize-4) == ComDevNum*sizeof(COMMUNITYDEVICE))
					{
						storage_malloc_monitordev_memory(0, ComDevNum, &g_list);
						if(NULL == g_list || NULL == g_list->Comdev)
						{
							ret = FALSE;
							goto err;
						}

						g_list->Comnum = ComDevNum;
						memcpy(g_list->Comdev, RecData+4, sizeof(COMMUNITYDEVICE)*g_list->Comnum);

						for (i = 0; i < ComDevNum; i++)
						{
							log_printf("***********************************************\n");
							log_printf("g_index %x\n",i);
							log_printf("g_list->Comdev[i].DeviceName %s \n",g_list->Comdev[i].DeviceName);
							log_printf("g_list->Comdev[i].DeviceIP %x \n",g_list->Comdev[i].DeviceIP);
							log_printf("g_list->Comdev[i].DevPort %x \n",g_list->Comdev[i].DevPort);
							log_printf("g_list->Comdev[i].FactoryName %s \n",g_list->Comdev[i].FactoryName);
							log_printf("***********************************************\n");
						}
						
						if (ECHO_STORAGE_OK ==  storage_save_monitordev(COMMUNITY_CAMERA, g_list))
						{
							ret = TRUE;
						}					
					}
					else
					{
						ret = FALSE;
					}
				}
			}
	        else
	        {
	            ret	= FALSE;
	        }        
		}
	}
	
	
	
err:

	if (ret)
	{
		storage_free_monitordev_memory(&g_list);
	}

	if (ret)
	{
		RtspStateNotify(MONITOR_GETLIST, TRUE);
	}
	else
	{
		RtspStateNotify(MONITOR_GETLIST, FALSE);
	}
	
	inter_SetThread(&g_mThread);
	pthread_exit(NULL);
	return NULL;
}

/*************************************************
  Function:		recive_rtp_data
  Description: 	
  Input: 		
  	1.data		应答状态
  	2.param		参数
  Output:		无
  Return:		无
  Others:		
*************************************************/
void recive_rtp_data(void)
{
	if (g_RtspMonItorInfo.Recv)
	{
		//rtsp_recv_steams_loop();
	}
}

/*************************************************
  Function:		rtsp_monitor_callback
  Description: 	RTSP 数据回调
  Input: 		
  	1.data		应答状态
  	2.param		参数
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void rtsp_monitor_callback(RTSP_STATE_E echo, int param)
{	
	switch (echo)
	{
		case RTSP_STATE_NONE:
			break;
			
		case RTSP_STATE_CONNECT:
			break;
		
		case RTSP_STATE_PLAY:
			g_RtspMonItorInfo.state = MONITOR_MONITORING;
			break;
			
		case RTSP_STATE_PAUSE:
			break;			
			
		case RTSP_STATE_STOP:
			{
				switch (param)
				{
					case -1:
						g_ErrType = MONITOR_REQUEST_TIMEOUT;
						break;
						
					default:
						g_ErrType = MONITOR_ERR;
						break;
				}
				g_RtspMonItorInfo.SendCmd = 0;
				g_RtspMonItorInfo.state = MONITOR_END;
			}
			break;

		case RTSP_STATE_HEART:
			g_RtspMonItorInfo.HeartTime = 0;
			break;
		
		default:
			break;
			
	}
}

/*************************************************
  Function:    		get_videofmt
  Description:		获取视频类型字符串
  Input:
  	1.ID			视频格式ID
  Output:			
  Return:			转化后的字符串指针
  Others:			
*************************************************/
static char* get_videofmt(unsigned long ID)
{
	switch (ID)
	{
		case H264:
			return H264_STR;

		case MPEG:
			return MPEG_STR;

		default:
			return NULL;
	}
}

/*************************************************
  Function:    		get_videodec
  Description:		获取视频格式字符串
  Input:
  	1.ID			视频格式ID
  Output:			
  Return:			转化后的字符串指针
  Others:			
*************************************************/
static char* get_videodec(unsigned long ID)
{
	switch (ID)
	{
		case QCIF:
			return QCIF_STR;

		case QVGA:
			return QVGA_STR;
			
		case CIF:
			return CIF_STR;
			
		case D1:
			return D1_STR;
			
		case HalfD1:
			return HalfD1_STR;

		case VGA:
			return VGA_STR;

		case _720P:
			return _720P_STR;
			
		default:
			return NULL;
	}
}

/*************************************************
  Function:		create_sanli_url_name
  Description: 	创建RTSP 转码url
  Input: 		
  	1.ServerIP		服务器IP
  	2.FactoryName 	厂家名称
  	3.IP			IP地址
  	4.Port 			设备端口号
  	5.ChannelNumber	通道号
  	6.outputformat	视频类型	
  	7.resolution	视频格式
  	8.framerate		帧率
  	9.bitrate		码率
  	10.user			用户名
  	11.password		密码
  	12.AreaNo		小区编号
  	13.DevNo		设备号
  	12.urlname		URL名称
  Output:		无
  Return:		无
  Others:
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrate_Framerate_areano_deviceno.sdp
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrate_Framerate_areano_deviceno.sdp?ACCOUNT@user=admin&password=12345
*************************************************/
void create_sanli_url_name(char* ServerIP, HOMEDEVICE* Devinfo, char* urlname)
{
	char tmp[50];
	char fmt[150];
	char userinfo[50];
	char devinfo[50];
	int fmtlen = 0;
	int userinfolen = 0;
	int devinfolen = 0;
	
	if (urlname && ServerIP)
	{
		memset(fmt,0,sizeof(fmt));
		memset(tmp,0,sizeof(tmp));
		memset(userinfo,0,sizeof(userinfo));		
		if (strlen(Devinfo->Password) > 0 && strlen(Devinfo->UserName) > 0)
		{	
			sprintf(userinfo, "?ACCOUNT@user=%s&password=%s", Devinfo->UserName, Devinfo->Password);
			userinfolen = strlen(userinfo);
		}
		sprintf(urlname,"rtsp://%s/LIVECAST_%s_%s_%d_%d.sdp",
				ServerIP, Devinfo->FactoryName, UlongtoIP(Devinfo->DeviceIP), Devinfo->DevPort, Devinfo->ChannelNumber);
		if (userinfolen > 0)
		{
			strcat(urlname,userinfo);
		}
	}
}

/*************************************************
  Function:		create_url_name
  Description: 	创建RTSP 转发url
  Input: 	
  	1.ServerIP		服务器IP
  	2.FactoryName 	厂家名称
  	3.IP			IP地址
  	4.Port 			设备端口号
  	5.ChannelNumber	通道号
  	6.AreaNo		区号	
  	7.DevNo			设备号
  	8.urlname		URL名称
  Output:		无
  Return:		无
  Others:
LIVECAST_FactoryName_IP_Port_ChannelNumber.sdp
LIVECAST_FactoryName_IP_Port_ChannelNumber_areano_deviceno.sdp
*************************************************/
void create_url_name(char* ServerIP,char* FactoryName, char* IP, unsigned int Port, char* ChannelNumber, char* AreaNo, char* DevNo, char* urlname)
{
	if (urlname && FactoryName && ServerIP)
	{
		if (NULL != AreaNo && NULL != DevNo)
		{	
			sprintf(urlname,"rtsp://%s/LIVECAST_%s_%s_%d_%s_%s_%s.sdp",ServerIP,FactoryName,IP,Port,ChannelNumber,AreaNo,DevNo);
		}
		else
		{
			sprintf(urlname,"rtsp://%s/LIVECAST_%s_%s_%d_%s.sdp",ServerIP,FactoryName,IP,Port,ChannelNumber);
		}
	}
}

/*************************************************
  Function:		create_mobile_url_name
  Description: 	创建RTSP 转码url
  Input: 		
  	1.ServerIP		服务器IP
  	2.FactoryName 	厂家名称
  	3.IP			IP地址
  	4.Port 			设备端口号
  	5.ChannelNumber	通道号
  	6.outputformat	视频类型	
  	7.resolution	视频格式
  	8.framerate		帧率
  	9.bitrate		码率
  	10.user			用户名
  	11.password		密码
  	12.AreaNo		小区编号
  	13.DevNo		设备号
  	12.urlname		URL名称
  Output:		无
  Return:		无
  Others:
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp?ACCOUNT@user=admin&password=12345
*************************************************/
void create_mobile_url_name(char* ServerIP, char* ServerPort, HOMEDEVICE* Devinfo, char* AreaNo, char* DevNo, char* urlname)
{
	char tmp[50];
	char fmt[150];
	char userinfo[50];
	char devinfo[50];
	int fmtlen = 0;
	int userinfolen = 0;
	int devinfolen = 0;

	if (urlname && ServerIP)
	{
		memset(fmt, 0, sizeof(fmt));
		memset(tmp, 0, sizeof(tmp));
		memset(userinfo, 0, sizeof(userinfo));		

		sprintf(fmt, "%s", get_videofmt(DEFAULT_VIDEO_FMT));
		fmtlen = strlen(fmt);

		sprintf(tmp, "_%s", get_videodec(DEFAULT_VIDEO_IMGSIZE));
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_VIDEO_BITRATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);
		
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_VIDEO_FRAMERATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		if (NULL != AreaNo && NULL != DevNo)
		{	
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%s_%s", AreaNo, DevNo);
			strcat(fmt, tmp);
			fmtlen += strlen(tmp);
		}

		if (strlen(Devinfo->Password) > 0 && strlen(Devinfo->UserName) > 0)
		{	
			sprintf(userinfo, "?ACCOUNT@user=%s&password=%s", Devinfo->UserName, Devinfo->Password);
			userinfolen = strlen(userinfo);
		}

		sprintf(urlname,"rtsp://%s%s/MOBILELIVECAST_%s_%s_%d_%d_%s.sdp",
				ServerIP, ServerPort, Devinfo->FactoryName, UlongtoIP(Devinfo->DeviceIP), Devinfo->DevPort, Devinfo->ChannelNumber, fmt);
		
		if (userinfolen > 0)
		{
			strcat(urlname,userinfo);
		}
	}
}

/*************************************************
  Function:		create_selfipc_url_name
  Description: 	创建RTSP 转码url
  Input: 		
  	1.ServerIP		服务器IP
  	2.FactoryName 	厂家名称
  	3.IP			IP地址
  	4.Port 			设备端口号
  	5.ChannelNumber	通道号
  	6.outputformat	视频类型	
  	7.resolution	视频格式
  	8.framerate		帧率
  	9.bitrate		码率
  	10.user			用户名
  	11.password		密码
  	12.AreaNo		小区编号
  	13.DevNo		设备号
  	12.urlname		URL名称
  Output:		无
  Return:		无
  Others:
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp?ACCOUNT@user=admin&password=12345
*************************************************/
void create_selfipc_url_name(char* ServerIP, char* ServerPort, HOMEDEVICE* Devinfo, char* AreaNo, char* DevNo, char* urlname)
{
	char tmp[50];
	char fmt[150];
	char userinfo[50];
	char devinfo[50];
	int fmtlen = 0;
	int userinfolen = 0;
	int devinfolen = 0;

	if (urlname && ServerIP)
	{
		memset(fmt, 0, sizeof(fmt));
		memset(tmp, 0, sizeof(tmp));
		memset(userinfo, 0, sizeof(userinfo));		

		sprintf(fmt, "%s", get_videofmt(DEFAULT_SELFIPC_VIDEO_FMT));
		fmtlen = strlen(fmt);

		sprintf(tmp, "_%s", get_videodec(DEFAULT_SELFIPC_VIDEO_IMGSIZE));
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_SELFIPC_VIDEO_BITRATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);
		
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_SELFIPC_VIDEO_FRAMERATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		if (NULL != AreaNo && NULL != DevNo)
		{	
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%s_%s", AreaNo, DevNo);
			strcat(fmt, tmp);
			fmtlen += strlen(tmp);
		}

		if (strlen(Devinfo->Password) > 0 && strlen(Devinfo->UserName) > 0)
		{	
			sprintf(userinfo, "?ACCOUNT@user=%s&password=%s", Devinfo->UserName, Devinfo->Password);
			userinfolen = strlen(userinfo);
		}

		sprintf(urlname,"rtsp://%s%s/MOBILELIVECAST_%s_%s_%d_%d_%s.sdp",
				ServerIP, ServerPort, Devinfo->FactoryName, UlongtoIP(Devinfo->DeviceIP), Devinfo->DevPort, Devinfo->ChannelNumber, fmt);
		
		if (userinfolen > 0)
		{
			strcat(urlname,userinfo);
		}
	}
}

/*************************************************
  Function:		create_hikvision_url_name
  Description: 	创建RTSP 转码url
  Input: 		
  	1.ServerIP		服务器IP
  	2.FactoryName 	厂家名称
  	3.IP			IP地址
  	4.Port 			设备端口号
  	5.ChannelNumber	通道号
  	6.outputformat	视频类型	
  	7.resolution	视频格式
  	8.framerate		帧率
  	9.bitrate		码率
  	10.user			用户名
  	11.password		密码
  	12.AreaNo		小区编号
  	13.DevNo		设备号
  	12.urlname		URL名称
  Output:		无
  Return:		无
  Others:
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp
MOBILELIVECAST_FactoryName_IP_Port_ChannelNumber_OutputFormat_Resolution_Bitrat
e_Framerate_areano_deviceno.sdp?ACCOUNT@user=admin&password=12345
*************************************************/
void create_hikvision_url_name(char* ServerIP, char* ServerPort, HOMEDEVICE* Devinfo, char* AreaNo, char* DevNo, char* urlname)
{
	char tmp[50];
	char fmt[150];
	char userinfo[50];
	char devinfo[50];
	int fmtlen = 0;
	int userinfolen = 0;
	int devinfolen = 0;

	if (urlname && ServerIP)
	{
		memset(fmt, 0, sizeof(fmt));
		memset(tmp, 0, sizeof(tmp));
		memset(userinfo, 0, sizeof(userinfo));		

		sprintf(fmt, "%s", get_videofmt(DEFAULT_HIKVISION_VIDEO_FMT));
		fmtlen = strlen(fmt);

		sprintf(tmp, "_%s", get_videodec(DEFAULT_HIKVISION_VIDEO_IMGSIZE));
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_HIKVISION_VIDEO_BITRATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);
		
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "_%d", DEFAULT_HIKVISION_VIDEO_FRAMERATE);
		strcat(fmt, tmp);
		fmtlen += strlen(tmp);

		if (NULL != AreaNo && NULL != DevNo)
		{	
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%s_%s", AreaNo, DevNo);
			strcat(fmt, tmp);
			fmtlen += strlen(tmp);
		}

		if (strlen(Devinfo->Password) > 0 && strlen(Devinfo->UserName) > 0)
		{	
			sprintf(userinfo, "?ACCOUNT@user=%s&password=%s", Devinfo->UserName, Devinfo->Password);
			userinfolen = strlen(userinfo);
		}

		sprintf(urlname,"rtsp://%s%s/MOBILELIVECAST_%s_%s_%d_%d_%s.sdp",
				ServerIP, ServerPort, Devinfo->FactoryName, UlongtoIP(Devinfo->DeviceIP), Devinfo->DevPort, Devinfo->ChannelNumber, fmt);
		
		if (userinfolen > 0)
		{
			strcat(urlname,userinfo);
		}
	}
}

/*************************************************
  Function:		    rtsp_monitor_connect
  Description: 	    RTSP 连接
  Input: 		
  	1.Index	    
  Output:		无
  Return:		TRUE
  Others:		
*************************************************/
int32 rtsp_monitor_connect(void)
{
	char URL[300] = {0};                            // 注意此空间问题，容易造成死机 
	int i,ret = 0;
	char serverIPstr[100];
	char serverportstr[20];
	uint32 ServerIP,HostIP;
	uint32 Serverprot = 554;
	uint32 video_img = DEFAULT_VIDEO_IMGSIZE;
	V_RECT_S rect;
	
	if (NULL == g_RtspMonItoRDevList || g_RtspMonItoRDevList->Devinfo == NULL)
	{	
		return FALSE;	
	}

	log_printf("g_RtspMonItorInfo.index %d ---- %d\n",g_RtspMonItorInfo.index,g_RtspMonItoRDevList->DevNum);
	
	if (MONITOR_REQUEST == g_RtspMonItorInfo.state)
	{
		ServerIP = storage_get_netparam_bytype(RTSP_IPADDR);
		HostIP = storage_get_netparam_bytype(HOST_IPADDR);

		if (ServerIP == 0)
		{
			// add by chenbh 2015-03-31 获取最优RTSP服务器
			int32  ReciSize;
			uint8 EchoValue;
			char RecData[100] = {0};
			uint32 SendData = 0x01;  // 获取RTSP 服务器
			uint32 CenterIP = net_get_center_ip();
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			ret = net_send_command(CMD_GET_SUIT_SERVER, &SendData, 4, CenterIP, NETCMD_UDP_PORT, 2, &EchoValue, RecData, &ReciSize);
			log_printf("CMD_GET_SUIT_SERVER ret : %d. EchoValue: %d\n", ret, EchoValue);
			if (TRUE == ret)
			{
				if (EchoValue == ECHO_OK)
				{
					memcpy(&g_Rtsp_ServerInfo.deviceno, (PDEVICE_NO)RecData, sizeof(DEVICE_NO));
					g_Rtsp_ServerInfo.ip = *(uint32 *)(RecData+sizeof(DEVICE_NO));
					g_Rtsp_ServerInfo.comm_port = *(uint32 *)(RecData+sizeof(DEVICE_NO)+4);
					g_Rtsp_ServerInfo.rtsp_port = *(uint32 *)(RecData+sizeof(DEVICE_NO)+8);					
				}
			}
			else
			{
				memset(&g_Rtsp_ServerInfo, 0, sizeof(RTSP_SERVER_INFO));
			}

			log_printf("g_Rtsp_ServerInfo.ip :%x, g_Rtsp_ServerInfo.rtsp_port : %d\n", g_Rtsp_ServerInfo.ip, g_Rtsp_ServerInfo.rtsp_port);
			log_printf("g_Rtsp_ServerInfo.comm_port :%d\n",g_Rtsp_ServerInfo.comm_port);
			ServerIP = g_Rtsp_ServerInfo.ip;
			Serverprot = g_Rtsp_ServerInfo.rtsp_port;
			g_BestIP = g_Rtsp_ServerInfo.ip;	
		}

		log_printf(" rtsp_monitor_connect ServerIP : %x\n", ServerIP);
		if (ServerIP == 0)
		{
			return FALSE;
		}
		memset(serverportstr,0,sizeof(serverportstr));
		memset(serverIPstr,0,sizeof(serverIPstr));
		sprintf(serverIPstr, "%s",UlongtoIP(ServerIP));

		// modi by chenbh 2016-04-06 由于新塘libnemesi库解析问题 需要将':'改为'#'
		//sprintf(serverportstr, "%c%d", ':', Serverprot);
		sprintf(serverportstr, "%c%d", '#', Serverprot);				

		log_printf("serverportstr : %s\n",serverportstr);
		#if 1
		log_printf("***********************************************\n");
		log_printf("g_index %x\n",g_RtspMonItorInfo.index);
		log_printf("g_list->Comdev[i].DeviceName %s \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DeviceName);
		log_printf("g_list->Comdev[i].DeviceIP %x \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DeviceIP);
		log_printf("g_list->Comdev[i].DevPort %x \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DevPort);
		log_printf("g_list->Comdev[i].FactoryName %s \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName);	
		log_printf("g_list->Comdev[i].UserName %s \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].UserName);	
		log_printf("g_list->Comdev[i].Password %s \n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].Password);	
		log_printf("***********************************************\n");
		#endif

		log_printf("g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName : %s\n",g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName);
	    if (0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "SANLI") || 
			 0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "Sanli"))
		{			
			create_sanli_url_name(serverIPstr, &g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index], URL);
			video_img = DEFAULT_VIDEO_IMGSIZE;
        }
		else if (0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "SELFIPC") ||
				0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "SelfIPC"))
		{
			create_selfipc_url_name(serverIPstr, serverportstr, &g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index], NULL, NULL, URL);
			video_img = DEFAULT_SELFIPC_VIDEO_IMGSIZE;
		}		
		else if (0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "HIKVISION")|| 
			 0 == strcmp(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, "hikvision"))
		{
			  create_hikvision_url_name(serverIPstr, serverportstr, &g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index], NULL, NULL, URL);
			  video_img = DEFAULT_HIKVISION_VIDEO_IMGSIZE;
		}
        else
        {
        	create_mobile_url_name(serverIPstr, serverportstr, &g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index], NULL, NULL, URL);
			video_img = DEFAULT_VIDEO_IMGSIZE;
        }
		
        log_printf("URL = %s\n",URL);

		switch (video_img)
		{
			case QVGA:
				rect.width = 320;
				rect.height = 240;
				break;
				
			case CIF:
				rect.width = 352;
				rect.height = 288;
				break;

			case VGA:
				rect.width = 640;
				rect.height = 480;
				break;

			default:
				rect.width = 640;
				rect.height = 480;
				break;
		}
		
		// 开启rtsp 监视连接
		ret = media_start_rtsp(rect, URL, rtsp_monitor_callback);
        if (-1 == ret)       
		{
			return FALSE;
		}	
		
        return TRUE;
	}
	else
	{
	    log_printf("monitor_rtsp_connect_get_monitor_state = %d\n",g_RtspMonItorInfo.state);
	}
	return FALSE;
}

/*************************************************
  Function:			rtsp_monitor_proc
  Description: 		监视线程
  Input: 			
  	1.param			参数
  Output:			无
  Return:			BOOLEAN
  Others:
*************************************************/
static void* rtsp_monitor_proc(void *param)
{
	// 设置分离线程
	pthread_detach(pthread_self());
	int16 times = 0;

LabChange:	
	if (MONITOR_REQUEST == g_RtspMonItorInfo.state)
	{
		RtspGuiNotify(g_RtspMonItorInfo.state, 0);
		g_PreRtspMonitorState = g_RtspMonItorInfo.state;
		if(FALSE == rtsp_monitor_connect())
		{
			log_printf("rtsp_monitor_connect return fail!!!! \n");
			g_ErrType = MONITOR_SEARCH_ERR;
			g_RtspMonItorInfo.SendCmd = 0;
			g_RtspMonItorInfo.state = MONITOR_END;
		}
		else
		{
			log_printf("rtsp_monitor_connect return success!!!! \n");
			g_RtspMonItorInfo.Recv = 1;	
		}
		
		g_RtspMonItorInfo.HeartTime = 0;
		g_RtspMonItorInfo.TimeOut = 0;
		log_printf("g_RtspMonItorInfo.state : %d\n", g_RtspMonItorInfo.state);
		while (MONITOR_REQUEST == g_RtspMonItorInfo.state)
		{
			g_RtspMonItorInfo.TimeOut++;

			if (g_RtspMonItorInfo.TimeOut >= MONITOR_REQUEST_TIME)
			{
				g_ErrType = MONITOR_REQUEST_TIMEOUT;
				g_RtspMonItorInfo.SendCmd = 1;
				g_RtspMonItorInfo.state = MONITOR_END;
				log_printf(" g_RtspMonItorInfo.TimeOut >= MONITOR_REQUEST_TIME \n");
				break;				
			}			
			
			times = 50;
			while ((times--) > 0 && MONITOR_REQUEST == g_RtspMonItorInfo.state)
			{
				usleep(20*1000);
			}
		}
	}
	
	if (MONITOR_MONITORING == g_RtspMonItorInfo.state)
	{
		RtspGuiNotify(g_RtspMonItorInfo.state, 0);
		g_PreRtspMonitorState = g_RtspMonItorInfo.state;
		
		g_RtspMonItorInfo.HeartTime = 0;
		g_RtspMonItorInfo.TimeOut = 0;
		g_RtspMonItorInfo.TimeMax = MONITOR_TIME_MAX;
		while (MONITOR_MONITORING == g_RtspMonItorInfo.state)
		{
			g_RtspMonItorInfo.TimeOut++;

			if (g_RtspMonItorInfo.TimeOut >= g_RtspMonItorInfo.TimeMax)
			{
				log_printf("txl TimeOut out %d\n",g_RtspMonItorInfo.TimeOut);					
				if (g_RtspMonItorInfo.TimeOut > (g_RtspMonItorInfo.TimeMax+3))
				{
					log_printf("txl TimeOut oper\n");	
					g_RtspMonItorInfo.state = MONITOR_END;
					break;	
				}
				else
				{
					g_ErrType = MONITOR_OK;
					g_RtspMonItorInfo.Recv = 0;
					g_RtspMonItorInfo.SendCmd = 1;
					g_RtspMonItorInfo.state = MONITOR_END;
					break;
				}
			}
			else
			{
				if (g_RtspMonItorInfo.TimeOut % MONITOR_HEART_TIME == 0)
				{
					g_RtspMonItorInfo.HeartTime++;
					// 心跳包发送
					if (net_get_net_status(NULL))
					{
						//rtsp_send_heart(rtsp_monitor_callback);
					}
					else
					{
						log_printf("AS_RTSP_HEART net err\n");
					}
				}
			}
			
			times = 50;
			while ((times--) > 0 && MONITOR_MONITORING == g_RtspMonItorInfo.state)
			{
				usleep(20*1000);
			}
		}
	}

	// 监视下一个情况 得回到监视请求
	if (MONITOR_REQUEST == g_RtspMonItorInfo.state)
	{
		media_stop_rtsp();
		goto LabChange;
	}

	g_RtspMonItorInfo.Recv = 0;	
	// 发送停止监视命令
	if (g_RtspMonItorInfo.SendCmd)
	{
		g_RtspMonItorInfo.SendCmd = 0;
	}

	media_stop_rtsp();
	
	g_RtspMonItorInfo.Recv = 0;
	g_RtspMonItorInfo.state = MONITOR_END;
	RtspGuiNotify(g_RtspMonItorInfo.state, g_ErrType);
	
	log_printf("monitor proc : AS_MONITOR_END : g_ErrType : %d\n", g_ErrType);
	g_RtspMonItorInfo.state = MONITOR_END;
	g_ErrType = MONITOR_OK;
	sys_set_monitor_state(FALSE);
	log_printf("monitor proc end!\n");	
	inter_SetThread(&g_RtspMonItorInfo.mThread);
	pthread_exit(NULL);
	return NULL;             	// 返回后资源由系统释放
}

/*************************************************
  Function:		rtsp_monitor_sync_devlist
  Description: 	获取RTSP设备列表
  Input: 		无
  Output:		无
  Return:		PMonitorDeviceList 设备列表
  Others:		
*************************************************/
uint32 rtsp_monitor_sync_devlist (PFGuiNotify GuiProc)
{
	g_RtspMonItorInfo.state_notify = GuiProc;
	memset(&g_mThread, 0, sizeof(struct ThreadInfo));
	if (0 != inter_start_thread(&g_mThread, rtsp_get_monitor_sync_devlist, NULL, 0))
	{
		g_RtspMonItorInfo.state_notify(MONITOR_GETLIST, FALSE);
		return FALSE;
	}
	return TRUE;
}

/*************************************************
  Function:		rtsp_monitor_get_devlist
  Description: 	获取RTSP设备列表
  Input: 		
  	flg			0 社区监视 1 家居监视
  Output:		无
  Return:		PMonitorDeviceList 设备列表
  Others:		
*************************************************/
PRtspDeviceList rtsp_monitor_get_devlist (uint8 flg)
{
	PMonitorDeviceList g_list;
	int max = 0;
	int i,index = 0;
	
	if (g_RtspMonItoRDevList)
	{
		if (g_RtspMonItoRDevList->Devinfo)
		{
			free(g_RtspMonItoRDevList->Devinfo);
			g_RtspMonItoRDevList->Devinfo = NULL;
		}
		free(g_RtspMonItoRDevList);
		g_RtspMonItoRDevList = NULL;
	}

	g_RtspMonItoRDevList = (PRtspDeviceList)malloc(sizeof(RtspDeviceList)+1);
	if (NULL == g_RtspMonItoRDevList)
	{
		return NULL;
	}
	
	storage_get_monitordev_used(&g_list);

	// 社区监视
	if (0 == flg)
	{
		max = g_list->Comnum;
		g_list->Homenum = 0;
	}
	else
	{
		max = g_list->Homenum;
		g_list->Comnum = 0;
	}

	g_RtspMonItoRDevList->Devinfo = (PHOMEDEVICE)malloc(sizeof(HOMEDEVICE)*max+1);
	if (NULL == g_RtspMonItoRDevList->Devinfo)
	{
		g_RtspMonItoRDevList->DevNum = 0;
		goto exit;
	}
	g_RtspMonItoRDevList->DevNum = 0;

	for (i = 0; i < g_list->Comnum; i++)
	{
		//if (0 == strcmp(g_list->Comdev[i].FactoryName, "SANLI") || 0 == strcmp(g_list->Comdev[i].FactoryName,"Sanli"))
		{
			sprintf(g_RtspMonItoRDevList->Devinfo[index].DeviceName, "%s", g_list->Comdev[i].DeviceName);
			sprintf(g_RtspMonItoRDevList->Devinfo[index].FactoryName, "%s", g_list->Comdev[i].FactoryName);
			g_RtspMonItoRDevList->Devinfo[index].DeviceIP = g_list->Comdev[i].DeviceIP;
			g_RtspMonItoRDevList->Devinfo[index].DevPort = g_list->Comdev[i].DevPort;
			g_RtspMonItoRDevList->Devinfo[index].ChannelNumber = g_list->Comdev[i].ChannelNumber;
			memset(g_RtspMonItoRDevList->Devinfo[index].UserName, 0, sizeof(g_RtspMonItoRDevList->Devinfo[index].UserName));
			memset(g_RtspMonItoRDevList->Devinfo[index].Password, 0, sizeof(g_RtspMonItoRDevList->Devinfo[index].Password));
			g_RtspMonItoRDevList->Devinfo[index].CanControlPTZ = g_list->Comdev[i].CanControlPTZ;
			index++;
		}
	}

	for (i = 0; i < g_list->Homenum; i++)
	{
		memcpy(&g_RtspMonItoRDevList->Devinfo[index], &g_list->Homedev[i], sizeof(HOMEDEVICE));
		index++;
	}

	g_RtspMonItoRDevList->DevNum = index;
	
exit:	
	storage_free_monitordev_memory(&g_list);
	return g_RtspMonItoRDevList;
}

/*************************************************
  Function:			monitor_start
  Description:		开始监视
  Input: 	
  	1.ID			应用ID
  	2.DevType		设备类型
  	3.index			设备索引
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 rtsp_monitor_start(uint32 index, uint32 type, PFGuiNotify GuiProc)
{
	log_printf("rtsp_monitor_start %d\n",g_RtspMonItorInfo.state);

	int32 ret = sys_set_monitor_state(TRUE);
	if (ret != 0)
	{
		return FALSE;
	}	
	
	if (g_RtspMonItorInfo.state != MONITOR_END)
	{
		log_printf("monitor start : busy state: %d\n", g_RtspMonItorInfo.state);
		return FALSE;
	}
	
	if (NULL == g_RtspMonItoRDevList || NULL == g_RtspMonItoRDevList->Devinfo || g_RtspMonItoRDevList->DevNum == 0)
	{
		return FALSE;	
	}

	g_RtspMonItorInfo.Recv = 0;
	g_RtspMonItorInfo.index = index;
	g_RtspMonItorInfo.type = type;

	//g_RtspMonItorInfo.state = MONITOR_END;
	g_RtspMonItorInfo.gui_notify = GuiProc;
	//rtsp_init(rtsp_monitor_callback, rtsp_video_callback, NULL); // 连接到监视切换状态回调
	//rtsp_set_debug(0);
	
	g_RtspMonItorInfo.state = MONITOR_REQUEST;
	if (0 != inter_start_thread(&g_RtspMonItorInfo.mThread, rtsp_monitor_proc, (void*)&g_RtspMonItorInfo, index))
	{
		g_RtspMonItorInfo.state = MONITOR_END;
		return FALSE;
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
int32 rtsp_monitor_stop(void)
{
	log_printf("rtsp_monitor_stop %x\n",g_RtspMonItorInfo.state);	
	if (g_RtspMonItorInfo.state != MONITOR_END)
	{
		g_RtspMonItorInfo.SendCmd = 1;
		g_RtspMonItorInfo.Recv = 0;		
		g_RtspMonItorInfo.state = MONITOR_END;
	}
	return TRUE;
}

/*************************************************
  Function:				rtsp_monitor_list_free
  Description:			
  Input: 	
  	1.GuiProc			GUI回调函数
  Output:				无
  Return:				
  Others:
*************************************************/
void rtsp_monitor_list_free(void)
{
	if (g_RtspMonItoRDevList)
	{
		if (g_RtspMonItoRDevList->Devinfo)
		{
			free(g_RtspMonItoRDevList->Devinfo);
			g_RtspMonItoRDevList->Devinfo = NULL;
		}
		free(g_RtspMonItoRDevList);
		g_RtspMonItoRDevList = NULL;
	}
}

/*************************************************
  Function:				rtsp_monitor_deini
  Description:			RTSP监视初始化
  Input: 	
  	1.GuiProc			GUI回调函数
  Output:				无
  Return:				
  Others:
*************************************************/
int rtsp_monitor_deini(void)
{
	return rtsp_monitor_stop();
}

static HOMEDEVICE Homedev[MAX_HOME_NUM+1];
static char gsenddata[1000];
/*************************************************
  Function:			rtsp_distribute
  Description:		
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static void get_homeCamera(const PRECIVE_PACKET recPacket)
{
	uint32 j,i,size,max = 0;
	int len = sizeof(HOMEDEVICE);
	char *info = (char*)Homedev;
	
	size = get_homedev_from_storage(Homedev);
	log_printf("size %d len %d\n",size,len);
	if (0 == size)
	{
		max = 0;
		goto exit;
	}

	j = 0;
	for (i = 0; i < size; i++)
	{
		if (Homedev[i].EnableOpen)
		{
			memcpy(gsenddata+4+(j*len), info+(i*len), len);		
			j++;
			max++;
		}
	}

exit:
	memcpy(gsenddata, &max, 4);
	size = (sizeof(HOMEDEVICE)*(max) + 4);
	net_send_echo_packet(recPacket, ECHO_OK, gsenddata, size);
	log_printf("get_homeCamera 2 %d\n",size);
}

/*************************************************
  Function:			rtsp_distribute
  Description:		
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static void get_homeCamera_ext(const PRECIVE_PACKET recPacket)
{
	uint32 j,i,size,max = 0;
	int len = sizeof(HOMEDEVICE);
	int NewLen = sizeof(NEWHOMEDEVICE);
	char *info = (char*)Homedev;
	
	size = get_homedev_from_storage(Homedev);
	log_printf("size %d len %d\n",size,len);
	if (0 == size)
	{
		max = 0;
		goto exit;
	}

	j = 0;
	memset(gsenddata, 0, sizeof(gsenddata));
	for (i = 0; i < size; i++)
	{
		if (Homedev[i].EnableOpen)
		{
			// 注意 NewLen 和 len的区别
			memcpy(gsenddata+4+(j*NewLen), info+(i*len), len);		
			j++;
			max++;
		}
	}

exit:
	memcpy(gsenddata, &max, 4);
	size = (sizeof(NEWHOMEDEVICE)*(max) + 4);
	net_send_echo_packet(recPacket, ECHO_OK, gsenddata, size);
	log_printf("get_homeCamera 2 %d\n",size);
}

#ifdef _NEW_SELF_IPC_
/*************************************************
  Function:			init_ipc_state
  Description:		
  Input: 	
  Output:			无
  Return:			
  Others:			ipc状态初始化 默认离线状态
*************************************************/
void init_ipc_state(void)
{
	int i = 0;
	for (i=0; i<MAX_HOME_NUM; i++)
	{
		g_ipc_list[i].TimeOut = 0;
	}

	PMonitorDeviceList CommDevlist = NULL;
	storage_get_comm_monitordev(&CommDevlist);
	storage_free_monitordev_memory(&g_CommDevlist);
	storage_malloc_new_monitordev_memory(0, CommDevlist->Comnum, &g_CommDevlist);
	if(NULL != g_CommDevlist && NULL != g_CommDevlist->Comdev)
	{
		for (i=0; i<(CommDevlist->Comnum); i++)
		{
			memset(&g_CommDevlist->Comdev[i], 0, sizeof(NEWCOMMUNITYDEVICE));
			g_CommDevlist->Comdev[i].DeviceIP = CommDevlist->Comdev[i].DeviceIP;
			g_CommDevlist->Comdev[i].DevPort = CommDevlist->Comdev[i].DevPort;
			g_CommDevlist->Comdev[i].CanControlPTZ = CommDevlist->Comdev[i].CanControlPTZ;
			g_CommDevlist->Comdev[i].ChannelNumber = CommDevlist->Comdev[i].ChannelNumber;
			memcpy(g_CommDevlist->Comdev[i].DeviceName, CommDevlist->Comdev[i].DeviceName, sizeof(CommDevlist->Comdev[i].DeviceName));
			memcpy(g_CommDevlist->Comdev[i].FactoryName, CommDevlist->Comdev[i].FactoryName, sizeof(CommDevlist->Comdev[i].FactoryName));
			g_CommDevlist->Comdev[i].isOnLine = 1;		// 默认都是在线的
		}
	}
}

/*************************************************
  Function:			ipc_ontimer
  Description:		
  Input: 	
  Output:			无
  Return:			
  Others:			ipc状态定时器
*************************************************/
void ipc_ontimer(void)
{
	int i = 0;
	for (i=0; i<MAX_HOME_NUM; i++)
	{
		if (g_ipc_list[i].TimeOut > 0)
		{
			g_ipc_list[i].TimeOut--;
		}
	}
}

/*************************************************
  Function:			ipc_ontimer
  Description:		
  Input: 	
  	index			设备编号索引 0-9
  Output:			无
  Return:			0 离线 1 在线
  Others:			获得IPC在线状态 监视列表中显示用
*************************************************/
uint8 get_ipc_state(uint8 index)
{
	if (g_ipc_list[index].TimeOut > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*************************************************
  Function:			get_commdev_state
  Description:		
  Input: 	
  	IP				设备IP
  Output:			无
  Return:			0 离线 1 在线
  Others:			获得IPC在线状态 监视列表中显示用
*************************************************/
uint8 get_commdev_state(uint32 IP)
{
	int i = 0;
	if (g_CommDevlist == NULL || g_CommDevlist->Comdev == NULL)
	{
		return 0;
	}
	
	for (i=0; i<(g_CommDevlist->Comnum); i++)
	{
		if (g_CommDevlist->Comdev[i].DeviceIP == IP)
		{
			return g_CommDevlist->Comdev[i].isOnLine;
		}
	}
	
	return 0;
}

/*************************************************
  Function:			set_ipc_online
  Description:		
  Input: 	
  	index			设备编号索引 0-9
  Output:			无
  Return:			
  Others:			设置IPC在线状态
*************************************************/
void set_ipc_online(uint8 index, uint32 IP)
{
	g_ipc_list[index].TimeOut = IPC_HEART_TIME;
	g_ipc_list[index].IPList.address = IP;
	g_ipc_list[index].IPList.index = index;
}

/*************************************************
  Function:			ipc_ontimer
  Description:		
  Input: 	
  	index			设备编号索引 0-9
  Output:			无
  Return:			
  Others:			设置中启用IPC记得调用这个函数
*************************************************/
void set_ipc_outline(uint8 index)
{
	g_ipc_list[index].TimeOut = 0;
}

/*************************************************
  Function:			fenji_sync_ipc_list
  Description:		
  Input: 	
  Output:			无
  Return:			
  Others:			分机向主机同步列表
*************************************************/
int fenji_sync_ipc_list(void)
{
	// 分机才需要向主机同步列表
	if (FALSE == is_main_DeviceNo())
	{
		char DeviceNo[30] = {0};		
		PFULL_DEVICE_NO dev = storage_get_devparam();
		memset(DeviceNo, 0, sizeof(DeviceNo));
		strncpy(DeviceNo, dev->DeviceNoStr, dev->DevNoLen);
		DeviceNo[dev->DevNoLen-1] = '0';
		DeviceNo[dev->DevNoLen] = 0;

		int j = 0;
		for (j=0; j<MAX_HOME_NUM; j++)
		{
			g_ipc_list[j].TimeOut = 0;
		}
		
		if (g_MainDevIP == 0)
		{
			uint32 *IPs  = NULL;				
			
			int count = net_get_ips(dev->AreaNo, DEVICE_TYPE_ROOM, DeviceNo, &IPs);	
			if (count)
			{
				log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s, count = %d , address = %x \n", DeviceNo, count, IPs[0]);
				g_MainDevIP = IPs[0];

				#ifndef _AU_PROTOCOL_
				DEVICE_NO DestNo;
				DestNo.AreaNo = dev->AreaNo;
				DestNo.DeviceType = DEVICE_TYPE_ROOM;
				DestNo.DeviceNo1 = 0;
				DestNo.DeviceNo2 = atoi(DeviceNo);
				set_nethead(DestNo, PRIRY_DEFAULT);	
				#endif

				uint8 EchoValue;
				uint32 ReciSize;
				char RecData[150] = {0};
				int ret = net_send_command(CMD_GET_HOME_ONLINE_LIST_EXT, NULL, 0, g_MainDevIP, NETCMD_UDP_PORT, 3, &EchoValue, RecData, &ReciSize);
				if (TRUE == ret)
				{
					log_printf("ReciSize : %d, EchoValue: %x\n", ReciSize, EchoValue);
					if (EchoValue == ECHO_OK && ReciSize > 4)
					{
						int i = 0, counts;
						memcpy(&counts, RecData, 4);
						log_printf("counts: %d\n", counts);

						// 防止过来counts错误导致卡死
						if (counts && counts < MAX_HOME_NUM*2)
						{
							PMonitorDeviceList Devlist = NULL;
							storage_free_monitordev_memory(&Devlist);
							storage_get_home_monitordev(&Devlist);
							SelfIPCamera IPC;
							for (i=0; i<counts; i++)
							{
								//SelfIPCamera *IPC = (SelfIPCamera *)(RecData+4+sizeof(SelfIPCamera)*i);
								memset(&IPC, 0, sizeof(SelfIPCamera));
								memcpy(&IPC, (RecData+4+sizeof(SelfIPCamera)*i), sizeof(SelfIPCamera));
								log_printf("IPC.address: %x, IPC.index: %d\n", IPC.address, IPC.index);
								if (Devlist->Homedev[IPC.index].EnableOpen &&
									(0 == strcmp(Devlist->Homedev[IPC.index].FactoryName, "SELFIPC") ||
									0 == strcmp(Devlist->Homedev[IPC.index].FactoryName, "SelfIPC")))
								{
									// 目前只有比较IP 地址
									log_printf("Devlist->Homedev[IPC.index].DeviceIP: %x\n", Devlist->Homedev[IPC.index].DeviceIP);
									if (IPC.address != 0)
									{
										Devlist->Homedev[IPC.index].DeviceIP = IPC.address;		
										set_ipc_online(IPC.index, IPC.address);
									}									
								}								
							}
							storage_save_monitordev(HOME_CAMERA, Devlist);
							storage_free_monitordev_memory(&Devlist);							
						}
					}				
				}
				else
				{
					g_MainDevIP = 0;
					return FALSE;
				}				
			}
			else
			{
				g_MainDevIP = 0;
				return FALSE;
			}
		}
		else
		{
			#ifndef _AU_PROTOCOL_
			DEVICE_NO DestNo;
			DestNo.AreaNo = dev->AreaNo;
			DestNo.DeviceType = DEVICE_TYPE_ROOM;
			DestNo.DeviceNo1 = 0;
			DestNo.DeviceNo2 = atoi(DeviceNo);
			set_nethead(DestNo, PRIRY_DEFAULT);	
			#endif

			uint8 EchoValue;
			uint32 ReciSize;
			char RecData[150] = {0};
			log_printf("g_MainDevIP: %x\n", g_MainDevIP);
			int ret = net_send_command(CMD_GET_HOME_ONLINE_LIST_EXT, NULL, 0, g_MainDevIP, NETCMD_UDP_PORT, 3, &EchoValue, RecData, &ReciSize);
			if (TRUE == ret)
			{
				log_printf("ReciSize : %d, EchoValue: %x\n", ReciSize, EchoValue);
				if (EchoValue == ECHO_OK && ReciSize > 4)
				{
					int i = 0, counts;
					memcpy(&counts, RecData, 4);
					log_printf("counts: %d\n", counts);
					// 防止过来counts错误导致卡死
					if (counts && counts < MAX_HOME_NUM*2)
					{
							PMonitorDeviceList Devlist = NULL;
							storage_free_monitordev_memory(&Devlist);
							storage_get_home_monitordev(&Devlist);
							SelfIPCamera IPC;
							for (i=0; i<counts; i++)
							{
								//SelfIPCamera *IPC = (SelfIPCamera *)(RecData+4+sizeof(SelfIPCamera)*i);
								memset(&IPC, 0, sizeof(SelfIPCamera));
								memcpy(&IPC, (RecData+4+sizeof(SelfIPCamera)*i), sizeof(SelfIPCamera));
								log_printf("IPC.address: %x, IPC.index: %d\n", IPC.address, IPC.index);
								if (Devlist->Homedev[IPC.index].EnableOpen &&
									(0 == strcmp(Devlist->Homedev[IPC.index].FactoryName, "SELFIPC") ||
									0 == strcmp(Devlist->Homedev[IPC.index].FactoryName, "SelfIPC")))
								{
									// 目前只有比较IP 地址
									log_printf("Devlist->Homedev[IPC.index].DeviceIP: %x\n", Devlist->Homedev[IPC.index].DeviceIP);
									if (IPC.address != 0)
									{
										Devlist->Homedev[IPC.index].DeviceIP = IPC.address;		
										set_ipc_online(IPC.index, IPC.address);
									}									
								}								
							}
							storage_save_monitordev(HOME_CAMERA, Devlist);
							storage_free_monitordev_memory(&Devlist);							
						}
				}				
			}
			else
			{
				g_MainDevIP = 0;
				return FALSE;
			}				
		}
	}
	else
	{
		return TRUE;
	}		
}
#endif

/*************************************************
  Function:			set_control_ptz
  Description:		云台方向和焦距控制
  Input:			
  	1.cmd			云台控制命令
  	2.action 		0 启动控制 1 停止控制
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void set_control_ptz(uint8 cmd, uint8 action)
{
	char data[100] = {0};
	uint32 rtsp_ip = storage_get_netparam_bytype(RTSP_IPADDR);

	if (0 == rtsp_ip)
	{
		rtsp_ip = g_BestIP;
	}

	sprintf(data,"PTZCONTROL_1_%s_%s_%d_%d_%d:%d:2",
				g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, UlongtoIP(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DeviceIP), 
				g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DevPort, g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].ChannelNumber, cmd, action);
	net_direct_send(CMD_RTSP_PTZ_CONTROL, data, sizeof(data), rtsp_ip, NETCMD_UDP_PORT);
}

/*************************************************
  Function:			preset_control_ptz
  Description:		云台预置点控制
  Input:			
  	1.index			预置点索引
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void preset_control_ptz(uint8 index)
{
	char data[100] = {0};
	uint32 rtsp_ip = storage_get_netparam_bytype(RTSP_IPADDR);

	if (0 == rtsp_ip)
	{
		rtsp_ip = g_BestIP;
	}

	sprintf(data,"PTZCONTROL_1_%s_%s_%d_%d_%d:%d",
				g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].FactoryName, UlongtoIP(g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DeviceIP), 
				g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].DevPort, g_RtspMonItoRDevList->Devinfo[g_RtspMonItorInfo.index].ChannelNumber, GOTO_PRESET, index);
	net_direct_send(CMD_RTSP_PTZ_CONTROL, data, sizeof(data), rtsp_ip, NETCMD_UDP_PORT);
}

/*************************************************
  Function:			rtsp_distribute
  Description:		
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 rtsp_distribute(const PRECIVE_PACKET recPacket)
{
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	int32 cmd = head->command | SSC_VIDEOMONITORING << 8;
	
	switch (cmd)
	{											
		case CMD_GET_HOME_CAMERALIST:								
			{
				log_printf("CMD_GET_HOME_CAMERALIST\n");
				#ifdef _IP_MODULE_
				if (recPacket->size > NET_HEAD_SIZE)
				{	
					char *data = recPacket->data + NET_HEAD_SIZE;
					char bindcode[10] = {0};				
					memcpy(bindcode, data, 6);
					if (get_ipmodule_bindcode() != atoi(bindcode))
					{
						net_send_echo_packet(recPacket,ECHO_MAC, NULL, 0);
						break;
					}
				}
				#endif			
				get_homeCamera(recPacket);
				return TRUE;
			}

		case CMD_GET_HOME_CAMERALIST_EXT:								
			{
				log_printf("CMD_GET_HOME_CAMERALIST\n");
				#ifdef _IP_MODULE_
				if (recPacket->size > NET_HEAD_SIZE)
				{	
					char *data = recPacket->data + NET_HEAD_SIZE;
					char bindcode[10] = {0};				
					memcpy(bindcode, data, 6);
					if (get_ipmodule_bindcode() != atoi(bindcode))
					{
						net_send_echo_packet(recPacket,ECHO_MAC, NULL, 0);
						break;
					}
				}
				#endif			
				get_homeCamera_ext(recPacket);
				return TRUE;
			}

		#ifdef _NEW_SELF_IPC_
		case CMD_GET_HOME_ONLINE_LIST_EXT:
			{
				if (TRUE == is_main_DeviceNo())
				{
					#ifdef _AU_PROTOCOL_
					DEVICE_NO RemoDevno = head->DeviceNo;			
					#else
					DEVICE_NO RemoDevno = head->SrcDeviceNo;
					#endif
					DEVICE_NO LocalDevno = storage_get_devno();
					log_printf("remo: %d, local: %d\n", RemoDevno.DeviceNo2, LocalDevno.DeviceNo2);
					if ((RemoDevno.DeviceNo2/10) == (LocalDevno.DeviceNo2/10))	// 去除一个分机号
					{		
						int i = 0, index = 0;
						char data[150] = {0};
						memset(data, 0, sizeof(data));
						for (i=0; i<MAX_HOME_NUM; i++)
						{
							if (g_ipc_list[i].TimeOut > 0)
							{
								memcpy(data+4+index*(sizeof(SelfIPCamera)), &g_ipc_list[i].IPList, sizeof(SelfIPCamera));
								index++;
							}
						}
						memcpy(data, &index, 4);
						log_printf("index: %d\n", index);
						net_send_echo_packet(recPacket, ECHO_OK, data, 4+index*(sizeof(SelfIPCamera)));
					}					
				}
				return TRUE;
			}
			break;
		#endif
		
		default:
			break;
	}
	return FALSE;
}

/*************************************************
  Function:			rtsp_responsion
  Description:		
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void rtsp_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket)
{

}

