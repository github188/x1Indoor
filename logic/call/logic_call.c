/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_call.c
  Author:    	chenbh
  Version:   	2.0
  Date: 		2014-12-11
  Description:  对讲逻辑
				
  History:        
    1. Date: 
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_include.h"
#include "storage_include.h"

#define	CALLOUT_TIMEOUT			60					// 呼叫超时
#define LEAVEWORD_TIMEOUT		60					// 启动留言时间
#define	RECORDHINT_TIME			10					// 留言提示音时间
#define LEAVEWORD_TIME			30					// 留言最长时间
#define HEART_TIMEOUT			5					// 心跳超时时间 
#define PHONE_TALK_TIMEOUT		60					// 门口机通话时长
#define FENJI_TALK_TIMEOUT		300					// 分机和户户通话时长

#define	MOVE_TEMP_TIMES			3					// 最多转呼的次数
#ifdef _IP_MODULE_DJ_
#define MAX_ROOM_NUM			(8+10)
#else
#define MAX_ROOM_NUM			(8)
#endif
typedef struct
{
	uint32 ID;
	uint32 Times;
}HEART_PARAM;

typedef struct
{
	uint32 ID;										// 会话ID
	uint32 RemoteAreaNo;							// 被叫方区号
	char CallNo1[30];								// 主叫号
	char CallNo2[30];								// 被叫号
	uint32 address;									// 对方地址
	uint16 port;									// 对方端口
	uint8 AudioPt;
	uint8 VideoPt;
	CALL_STATE_E state;								// 状态值
	int32 TimeMax;									// 超时时间,最长通话时间, 0为无限长,秒
	int32 TimeOut;									// 计时,超时判断
	int32 HeartTime;								// 心跳计时
	LEAVE_WORD_MODE_E LeaveWordMode;				// 留言格式
	LVEWORD_STATE_TYPE LeaveWordState;				// 留言模式
	uint32 LocalMediaAddr;							// 本地媒体地址
	uint16 LocalAudioPort;							// 本地音频端口
	uint16 LocalVideoPort;							// 本地视频端口
	uint32 RemoteMediaAddr;							// 远程媒体地址
	uint16 RemoteAudioPort;							// 远程音频端口
	uint16 RemoteVideoPort;							// 远程视频端口
	uint8 LocalAudioSendrecv;						// _SENDRECV, _SENDONLY, _RECVONLY
	uint8 LocalVideoSendrecv;	
	uint8 RemoteDeviceType;
	uint8 IsStartVideo;
	uint8 IsStartAudio;
	uint8 isNat;									// 是否Nat 地址
	int8 IsTerminal;								// 是否呼叫分机
	PFGuiNotify gui_notify;							// GUI 状态通知函数
	struct ThreadInfo mThread;
}CALL_INFO, *PCALL_INFO;

typedef struct
{
	uint32 address;									// 命令地址
	uint16 port;									// 命令端口
	uint16 EchoValue;								// 应答状态码
	uint32 MediaAddr;								// 媒体地址
	uint16 AudioPort;								// 音频端口
	uint16 VideoPort;								// 视频端口
	uint8 IsSendVideo;
	uint8 SendTimes;								// 未应答次数 0xFF删除标志
}CALL_NODE_INFO, *PCALL_NODE_INFO;

// 有留影留言时使用
const VIDEO_SDP_PARAM g_lyly_venc_parm = 
{
	.encMode = VIDEO_ENCMODE_H264,					// 视频编码格式 0为不受限制
	.frameRate = 15,								// 视频帧率     0为不受限制
	.imgSize = VIDEO_SIZE_VGA,						// 视频分辨率   0为不受限制 
	.resver0 = 0,									// 保留位
	.bitRate = 1024000,								// 视频码率 单位(kbps) 0不受限制
    .resver1 = 0,									// 保留位
    .resver2 = 0,									// 保留位						
};

static CALL_INFO g_CallInfo;
static PCALL_NODE_INFO g_CallList = NULL;  			// 呼叫地址信息

static uint8 g_CallListNum =  0;
static uint8 g_CallRefuseNum = 0;
static uint32 g_CallRefuseList[20] = {0}; 
#ifdef _SEND_SDP_PARAM_
static char g_CallData[163];
#else
static char g_CallData[74];
#endif

static uint8 g_Use_SDP = FALSE;								// 用来判断是否需要发送SDP参数
static CALL_INFO g_BeCallInfo;
static CALL_INFO g_NewBeCallInfo;
static HEART_PARAM HeartParam;
static ZONE_DATE_TIME g_LylyDateTime = {0};

static int16 g_RemainTime;									// 剩余时间	
static uint8 g_ErrType;
static uint8 g_LylyFlg;
static uint8 g_ElevtorAuthorize = FALSE;
static uint8 g_MoveTempTimes = 0;
//static uint8 g_UnlockFlag = FALSE;
static uint16 g_RandSeed = 1;
static uint8 g_Leaveword_Timeout = 0;						// 	启动留言时间

static CALL_STATE_E g_PreBeCallState = CALL_STATE_NONE;   	// 被叫状态
static CALL_STATE_E g_PreCallOutState = CALL_STATE_NONE;   	// 主叫状态

static DEVICE_NO g_CallDestNo;								// 对讲目标设备编号
uint32 g_Manager_IP[MANAGER_COUNTS_MAX] = {0,};				// 从服务器获取的管理机设备IP列表
uint32 g_Manager_Index[MANAGER_COUNTS_MAX] = {0,};			// 从服务器获取的管理机设备索引列表

#define CallGuiNotify(param1, param2)\
	if (g_CallInfo.gui_notify)\
	{\
		g_CallInfo.gui_notify((param1), (param2));\
	}

#define BeCallGuiNotify(param1, param2)\
	if (g_BeCallInfo.gui_notify)\
	{\
		g_BeCallInfo.gui_notify((param1), (param2));\
	}

/*************************************************
  Function:			fill_call_destdevno
  Description: 		填充对讲目标设备编号
  Input: 		  	
  Output:			无
  Return:			BOOLEAN
  Others:
*************************************************/
static void fill_call_destdevno(char *InPutNo, DEVICE_TYPE_E devtype)
{
	char tmp[10] = {0};
	uint8 len = strlen(InPutNo);
	if (len > 9)
	{
		memcpy(tmp, InPutNo, len-9);
		g_CallDestNo.DeviceNo1 = atoi(tmp);
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, InPutNo+len-9, 9);
		g_CallDestNo.DeviceNo2 = atoi(tmp);
	}
	else
	{
		g_CallDestNo.DeviceNo1 = 0;
		g_CallDestNo.DeviceNo2 = atoi(InPutNo);
	}
	
	#ifndef _AU_PROTOCOL_
	g_CallDestNo.DeviceType = devtype;
	#endif
}

/*************************************************
  Function:			ipmodule_call
  Description:		
  Input: 	
  Output:			
  	1.devno		
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static void ipmodule_call(int index, char *data)
{
	//获取IP模块返回的ipad分机信息
	int offset = 0, i = 0 , j = 0, isfind = 0;
	int ipadCount = 0, ipadnewcount = 0;
	int extenNo[10] = {0};
	uint32 ips[10] = {0};
	uint32 newips[10] = {0};
	
	memcpy(&ipadCount, data+2, 4);
	if ( ipadCount <= 0 )
	{
		log_printf(">>>ipmodule_call: ipadCount = 0.\n");
		return ;
	}
	log_printf(">>>ipmodule_call: ipadCount = %d\n", ipadCount);
	
	for(i= 0; i < ipadCount; i++)
	{
		offset = 6 + 8*i;
		memcpy(&extenNo[i], data+offset, 4);
		memcpy(&ips[i], data+offset+4, 4);
	}

	for (i = 0; i < ipadCount; i++)
	{
		for (j = 0; j < g_CallListNum; j++)
		{
			if (g_CallList[j].address == ips[i])
			{
				isfind = 1;
				break;
			}
		}

		for (j = 0; j < g_CallRefuseNum; j++)
		{
			if (g_CallRefuseList[j] == ips[i])
			{
				isfind = 1;
				break;
			}
		}

		if (isfind == 0)
		{
			newips[ipadnewcount++] = ips[i];
		}
	}

	for(i = 0; i < ipadnewcount; i++)
	{
		g_CallList[g_CallListNum+i].address = newips[i];
        g_CallList[g_CallListNum+i].port = NETCMD_UDP_PORT;
        g_CallList[g_CallListNum+i].EchoValue = TRC_TRYING;
		g_CallList[g_CallListNum+i].SendTimes = 0;
	}
	g_CallListNum += ipadnewcount;

	for (i = 0; i< g_CallListNum;i++)
	{
		log_printf("i = %d %x\n",i,g_CallList[i].address);
	}
}

/*************************************************
  Function:			get_fenji_ipaddr
  Description:		获取分机IP
  Input: 	
  Output:			
  	1.devno		
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static uint32 get_fenji_ipaddr(uint32 devno)
{
	#ifdef _IP_MODULE_DJ_
	if (FALSE == get_ipmodule())
	{
		log_printf("get_fenji_ipaddr bindstate err\n");
		return 0;
	}
	
	if (is_main_DeviceNo())
	{
		return get_ipad_extension_addr(devno);
	}
	else
	{
		if (devno == 0)
		{
			return 0;
		}
		
		return ipmodule_get_extensionAddr(devno);
	}
	#else
	return 0;
	#endif
}

/*************************************************
  Function:			get_ring_file
  Description:		获取铃声名称
  Input: 	
  Output:			
  	1.FileName		
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static int32 get_ring_file(char *FileName)
{
	char *tmp = NULL;
	switch (g_BeCallInfo.RemoteDeviceType)
	{
		case DEVICE_TYPE_AREA:
		case DEVICE_TYPE_STAIR:
		case DEVICE_TYPE_MANAGER:
		case DEVICE_TYPE_DOOR_PHONE:
		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_ROOM:
			tmp = storage_get_ring_by_devtype(g_BeCallInfo.RemoteDeviceType);
			break;
		case DEVICE_TYPE_FENJI_NET:
			tmp = storage_get_ring_fenji();
			break;
		default:
			return FALSE;
	}
	sprintf(FileName, "%s", tmp);
	return TRUE;
}

/*************************************************
  Function:			add_inter_record
  Description:		添加通话记录
  Input: 	
  	1.RecordType	记录类型
  Output:			
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static void add_inter_record(CALL_TYPE RecordType, DEVICE_TYPE_E DevType, char *DevStr)
{
	ZONE_DATE_TIME DateTime = {0};
#if 1	
	#if 0
	if (g_UnlockFlag == TRUE)
	{
		g_UnlockFlag = FALSE;
		return;
	}
	#endif
	
	get_timer(&DateTime);
	if (RecordType == OUTGOING)
	{
		// 添加呼出记录
		if (DevType == DEVICE_TYPE_ROOM)
		{
			PFULL_DEVICE_NO myDev = storage_get_devparam();
			char devnostr[30] = {0};
			char callno[30] = {0};
			strcpy(devnostr, myDev->DeviceNoStr);
			devnostr[myDev->DevNoLen - 1] = 0;
			strncpy(callno, DevStr, myDev->DevNoLen - 1);
			callno[myDev->DevNoLen - 1] = 0;
			
			if (!strcmp(devnostr, callno))			// 呼叫分机
			{
				storage_add_callrecord(RecordType, DEVICE_TYPE_FENJI_NET, &DevStr[myDev->DevNoLen - 1], DateTime, FALSE);
			}
			else									// 呼叫住户
			{
				storage_add_callrecord(RecordType, DevType, DevStr, DateTime, FALSE);
			}
		}
		else
		{
			storage_add_callrecord(RecordType, DevType, DevStr, DateTime, FALSE);
		}	
	}
	else
	{
		// 添加被叫记录
		if (DevType == DEVICE_TYPE_MANAGER)
		{
			char CallStr[30] = {0};
			uint32 CallNo = atoi(DevStr);
			sprintf(CallStr, "%d", CallNo);
			log_printf("add inter record : manager call in: %s : %s\n", DevStr, CallStr);
			storage_add_callrecord(RecordType, DevType, CallStr, DateTime, FALSE);
		}
		else
		{
			// 有留影留言时，通话记录与留影留言链接
			if (g_LylyFlg == TRUE)
			{
				storage_add_callrecord(RecordType, DevType, DevStr, g_LylyDateTime, TRUE);
			}
			else
			{
				storage_add_callrecord(RecordType, DevType, DevStr, DateTime, FALSE);
			}
		}
	}
	sys_sync_hint_state();
#endif
}

/*************************************************
  Function:			add_inter_lyly
  Description:		添加留影留言记录
  Input: 	
  Output:			
  Return:			
  Others:	
*************************************************/
static void add_inter_lyly(void)
{
	//LYLY_TYPE LylyType = storage_get_lyly_mode();
	LYLY_TYPE LylyType = g_BeCallInfo.LeaveWordMode;
	log_printf(" ##############  LylyType : %x \n ", LylyType);
	if (g_BeCallInfo.RemoteDeviceType == DEVICE_TYPE_MANAGER)
	{
		char CallStr[30] = {0};
		uint32 CallNo = atoi(g_BeCallInfo.CallNo1);
		sprintf(CallStr, "%d", CallNo);
		log_printf("add_inter_lyly : manager call in: %s : %s\n", g_BeCallInfo.CallNo1, CallStr);
		storage_add_lylyrecord(LylyType, g_BeCallInfo.RemoteDeviceType, CallStr, g_LylyDateTime);
	}
	else
	{
		storage_add_lylyrecord(LylyType, g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1, g_LylyDateTime);
	}	
	sys_sync_hint_state();
}

/*************************************************
  Function:			get_device
  Description:		输入字符串转换为区号和设备号
  Input: 			
  	1.str			输入字符串
  	2.AreaCode		区号
  	3.devno			设备号
  Output:			无
  Return:			TRUE / FALSE
  Others:			"1#123456789"转换成 1 和 "12345679"
*************************************************/
static int32 get_device(const char * str, uint32 * AreaCode, char * devno)
{
	int32 i, pos = 0, n = strlen(str);
	int32 index = 0;
	char code[9];
	char temp[50];
	memset(temp, 0, sizeof(temp));
	for (i = 0; i <= n; i++)
	{
		if ('#' == str[i] || '*' == str[i])
		{
			if (0 == index)
			{
				index = pos;
			}
			continue;
		}
		temp[pos++] = str[i];
	}
	if (0 == index || index > 9)
	{
		return FALSE;
	}
	
	strncpy(code, temp, index);
	code[index] = 0;
	*AreaCode = atoi(code);
	strcpy(devno, temp + index);
	return TRUE;
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
static void play_recordhint_callback(int32 cmd, int32 time, int32 percent)
{
	if (cmd == 0)
	{
		log_printf("play_recordhint_callback : record hint play err!\n");
		media_start_net_hint(CALL_AUDIO_PT, (char *)storage_get_lyly_tip_path(), (void *)play_recordhint_callback);
	}
	else
	{	
		if (percent == 100)
		{
			log_printf("play_recordhint_callback : record hint play over!\n");
			media_stop_net_hint();
			g_BeCallInfo.state = CALL_STATE_RECORDING;
		}
	}
}

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
  Function:			callout_proc
  Description:		主叫线程执行函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void * callout_proc(void *arg)
{
	pthread_detach(pthread_self());
	time_t t0;
	int32 i = 0;
	int32 times = 0;
	uint32 temp = 0;
	int32 ret = FALSE;
	
	log_printf(" ********** callout_proc start ****************  \n");
	if (CALL_STATE_REQUEST == g_CallInfo.state)
	{
		int32 count  = 0;
		uint32 *IPs  = NULL;
		
		char tmp[31]      = {0};	
		char DeviceNo[30] = {0};
		PFULL_DEVICE_NO myDevno = storage_get_devparam();
		strcpy(DeviceNo, g_CallInfo.CallNo2);

		// 呼出记录 此函数耗时比较多，注意此函数执行过程媒体状态改变的情况
		add_inter_record(OUTGOING, g_CallInfo.RemoteDeviceType, g_CallInfo.CallNo2);
		
		// 回调GUI: 当前处于连接状态-开始获取IP
		g_PreCallOutState = CALL_STATE_REQUEST;
		memset(tmp, 0, sizeof(tmp));		
		CallGuiNotify(CALL_STATE_REQUEST, (uint32)tmp);

		// 获取IP地址
		if (g_CallInfo.isNat == FALSE)
		{
			#ifdef _IP_MODULE_DJ_
			uint8 oper   = 1;
			count = 0;
			if (-1 != g_CallInfo.IsTerminal)
			{
				uint32 fenjiaddr[10] = {0};
				IPs = fenjiaddr;
				IPs[0] = get_fenji_ipaddr(g_CallInfo.IsTerminal);
				if (IPs[0] > 0)
				{
					count = 1;
				}

				if (0 == count)
				{
					if (get_ipmodule())
					{
						if (is_fenji1_DeviceNo())
						{
							oper = 0;
						}
					}
				}
			}
			if (count == 0)
			{	
				if (oper)
				{
					count = net_get_ips(g_CallInfo.RemoteAreaNo, DEVICE_TYPE_ROOM, DeviceNo, &IPs);		
				}
			}
			if (count)
			{
				log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s, count = %d , address = %x \n", DeviceNo, count, IPs[0]);
			}
			#else
			count = net_get_ips(g_CallInfo.RemoteAreaNo, DEVICE_TYPE_ROOM, DeviceNo, &IPs);	
			if (count)
			{
				log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s, count = %d , address = %x \n", DeviceNo, count, IPs[0]);
			}
			#endif
		}
		else
		{
			count = 0;
			log_printf("count=======%d\n", count);
		}
		
		if (0 == count)
		{
			log_printf("lgoic_call.c : logic_call_resident : cann't get ip!\n");
			g_ErrType = END_BY_REQUESET_ERR;
			g_CallInfo.state = CALL_STATE_END;
		}
		else
		{
			if (g_CallList)
			{
				free(g_CallList);
			}
			g_CallList = malloc(sizeof(CALL_NODE_INFO) * (MAX_ROOM_NUM));
			memset(g_CallList, 0, sizeof(CALL_NODE_INFO) * (MAX_ROOM_NUM));
			memset(g_CallRefuseList, 0, 80);
			g_CallRefuseNum = 0;
			g_CallListNum = count;
			for (i = 0; i < count; i++)
			{
				g_CallList[i].address = IPs[i];
				g_CallList[i].port = NETCMD_UDP_PORT;
				g_CallList[i].EchoValue = TRC_UNKNOWN;
			}
			
			memset(g_CallInfo.CallNo1, 0, sizeof(g_CallInfo.CallNo1));
			if (g_CallInfo.isNat)
			{
				sprintf(g_CallInfo.CallNo1, "%d-%s", myDevno->AreaNo, myDevno->DeviceNoStr);
				memset(g_CallInfo.CallNo2, 0, sizeof(g_CallInfo.CallNo2));
				sprintf(g_CallInfo.CallNo2, "%d-%s", g_CallInfo.RemoteAreaNo, DeviceNo);				
			}
			else
			{
				strcpy(g_CallInfo.CallNo1, myDevno->DeviceNoStr);
			}
			g_CallInfo.CallNo1[29] = g_CallInfo.RemoteDeviceType;	

			// 获取IP结束，获取到IP ,增加状态判断,防止报警下对讲不能退出
			if (CALL_STATE_REQUEST == g_CallInfo.state)
			{
				tmp[0] = 1;
				strcpy(tmp+1, g_CallInfo.CallNo2);
				CallGuiNotify(CALL_STATE_REQUEST, (uint32)tmp);
				g_CallInfo.state = CALL_STATE_CALLING;
			}
		}
	}

	if (CALL_STATE_CALLING == g_CallInfo.state)
	{
		uint8 g_SendCall = 1;
		uint8 house_desc[100] = {0};
		CallGuiNotify(g_CallInfo.state, 0);
		
		if (g_RandSeed == 0xFFFF)
		{
			g_RandSeed = 1;
		}
		else
		{
			g_RandSeed++;
		}	
		srand(g_RandSeed);
		
		// 开始请求			
		g_CallInfo.ID = rand();
		g_CallInfo.IsStartAudio = FALSE;
		g_CallInfo.IsStartVideo = FALSE;
		g_CallInfo.TimeOut = 0;
		log_printf("g_CallInfo.ID : %d\n", g_CallInfo.ID);

		memset(g_CallData, 0, sizeof(g_CallData));
		memcpy(g_CallData, &g_CallInfo.ID, 4);  
		memcpy(g_CallData + 8, &g_CallInfo.LocalAudioPort, 2); 
		memcpy(g_CallData + 10, &g_CallInfo.LocalVideoPort, 2); 
		g_CallData[12] = g_CallInfo.AudioPt; 
		g_CallData[13] = g_CallInfo.VideoPt; 
		memcpy(g_CallData + 14, g_CallInfo.CallNo1, sizeof(g_CallInfo.CallNo1));	// 主叫设备号+被叫设备类型
		memcpy(g_CallData + 44, g_CallInfo.CallNo2, sizeof(g_CallInfo.CallNo2));	// 被叫号码，设备号或者电话号码
		
		#ifdef _SEND_SDP_PARAM_
		memcpy(g_CallData + 74, (char *)(&g_venc_parm), sizeof(VIDEO_SDP_PARAM));
		memcpy(g_CallData + 90, (char *)(&g_audio_parm), sizeof(AUDIO_SDP_PARAM));
		//get_houseno_desc(storage_get_devno_str(),house_desc);
		memcpy(g_CallData + 98, house_desc, 64);
		g_CallData[162] = LEAVEWORD_TIMEOUT;   // 最后一位,留言超时时间
		#endif
		
		// 中心呼出记录
		if (g_CallInfo.RemoteDeviceType == DEVICE_TYPE_MANAGER)
		{
			add_inter_record(OUTGOING, g_CallInfo.RemoteDeviceType, g_CallInfo.CallNo2);
		}	

		t0 = time(0);		
		g_CallInfo.TimeOut = 0;
		g_CallInfo.HeartTime = 0;
		g_PreCallOutState = CALL_STATE_CALLING;

		// 填充目标设备编号
		//set_nethead(g_CallDestNo, PRIRY_DEFAULT);
		
		while (CALL_STATE_CALLING == g_CallInfo.state)
		{
			//g_CallInfo.TimeOut = time(0) - t0;
			g_CallInfo.TimeOut++;
			g_CallInfo.HeartTime++;
			log_printf("g_CallInfo.TimeOut: %d, g_CallInfo.HeartTime:%d\n",g_CallInfo.TimeOut, g_CallInfo.HeartTime);
			if (g_CallInfo.TimeOut == 1 || (g_CallInfo.TimeOut >= 5 && g_CallInfo.TimeOut%5 == 0))
			{
				for (i = 0; i < g_CallListNum; i++)
				{
					switch (g_CallList[i].EchoValue)
					{
						case TRC_UNKNOWN:
						case TRC_TRYING:
						case TRC_RINGING:
						case TRC_QUEUED:
						case TRC_BUSY:		
						#ifdef _IP_MODULE_DJ_
						case TRC_MOVE_TEMP:
						#endif
							log_printf("g_CallList[%d].address %x - %d\n",i,g_CallList[i].address,g_CallInfo.TimeOut);
							g_CallList[i].SendTimes++;
							if (g_SendCall)
							{
								set_nethead(g_CallDestNo, PRIRY_DEFAULT);
								net_direct_send(CMD_CALL_CALLING, g_CallData, sizeof(g_CallData), g_CallList[i].address, g_CallList[i].port);
							}
							break;	
							
						default:
							log_printf("call timer proc : default echo value\n");
							g_CallList[i].SendTimes++;
							break;
					}
				}

				media_play_sound(storage_get_ring_out(), FALSE, NULL);
				media_set_output_volume(RING_OUT_VOL);
				if (g_CallInfo.state == CALL_STATE_CALLING)
				{
					uint8 nCallListNum = g_CallListNum;							
					
					i = 0;
					while (i < nCallListNum)
					{		
						// 应答超时
						#ifdef _IP_MODULE_DJ_
						if (g_CallList[i].SendTimes > 2 && g_CallList[i].EchoValue != TRC_TRYING)
						{
							g_CallList[i].EchoValue = TRC_UNKNOWN;
						}
						#else
						if (g_CallList[i].SendTimes > 2)
						{
							g_CallList[i].EchoValue = TRC_UNKNOWN;
						}
						#endif

						// 无应答或不匹配
						if ((TRC_UNKNOWN == g_CallList[i].EchoValue && g_CallList[i].SendTimes > 2) || 	
							TRC_NOT_FOUND == g_CallList[i].EchoValue)
						{	
							if (i < g_CallListNum - 1)
							{
								g_CallList[i] = g_CallList[g_CallListNum - 1];
							}
							g_CallListNum--;
							nCallListNum--;
						}
						// 忙应答
						else if (TRC_BUSY == g_CallList[i].EchoValue) 	
						{			
							i++;					
						}
						else if (TRC_TERMINATED == g_CallList[i].EchoValue)
						{
							if (i < g_CallListNum - 1)
							{
								g_CallList[i] = g_CallList[g_CallListNum - 1];
							}
							g_CallListNum--;
							nCallListNum--;
						}
						else
						{
							i++;
						}
					}

					if (g_CallListNum < 1)
					{
						log_printf("call timer proc : g_CallListNum is 0\n");
						g_SendCall = 0;
						g_ErrType = END_BY_COMM_TIMEOUT;
						g_CallInfo.state = CALL_STATE_END;
						break;
					}
					else
					{
						if (g_CallInfo.TimeOut >= CALLOUT_TIMEOUT)
						{			
							for (i = 0; i < g_CallListNum; i++)
							{
								set_nethead(g_CallDestNo, PRIRY_DEFAULT);
								net_direct_send(CMD_CALL_HANDDOWN, (char *)&g_CallInfo.ID, 4, g_CallList[i].address, g_CallList[i].port);
							}
							log_printf("call timer proc : call timer out\n");	
							g_SendCall = 0;
							g_ErrType = END_BY_CALL_TIMEOUT;
							g_CallInfo.state = CALL_STATE_END;
							break;
						}
					}
				}	
			}

			// 呼叫等待倒计时
			g_RemainTime = CALLOUT_TIMEOUT - g_CallInfo.TimeOut;
			temp = g_CallInfo.TimeOut << 16 | g_RemainTime;
			//CallGuiNotify(CALL_TIMER, temp);
			times = 10;
			while ((times--) > 0 && (g_CallInfo.state == CALL_STATE_CALLING))
			{
				usleep(100*1000);
			}
		}
	}

	if (CALL_STATE_RECORDHINT == g_CallInfo.state)
	{	
		ret = FALSE;
		log_printf("call out proc : AS_INTER_RECORD_HINT\n");
		CallGuiNotify(g_CallInfo.state, 0);
		uint8 RingVolume = storage_get_ringvolume();		
		
		// 停止播放回铃音
		media_stop_sound();
					
		// 开启网络音频
		media_start_net_audio(g_CallInfo.address);
		media_disable_audio_aec();
		media_disable_audio_ai();
		media_set_output_volume(RingVolume);
		
		t0 = time(0);
		g_CallInfo.TimeOut = 0;
		g_CallInfo.HeartTime = 0;
		HeartParam.ID = g_CallInfo.ID;
		g_PreCallOutState = CALL_STATE_RECORDHINT;

		//set_nethead(g_CallDestNo, PRIRY_DEFAULT);			
		while (CALL_STATE_RECORDHINT == g_CallInfo.state)
		{
			HeartParam.Times = 0;
			g_CallInfo.HeartTime++;
			g_CallInfo.TimeOut = time(0) - t0;
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_CallInfo.address, g_CallInfo.port);
			if (g_CallInfo.HeartTime > HEART_TIMEOUT)
			{
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HEART, (char*)&g_CallInfo.ID, 4, g_CallInfo.address, g_CallInfo.port);
				log_printf("call timer proc : record hint heart time out\n");
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_CallInfo.state = CALL_STATE_END;
				break;
			}
	
			times = 10;
			while ((times--) > 0 && CALL_STATE_RECORDHINT == g_CallInfo.state)
			{
				usleep(100*1000);
			}
		}
	}

	if (CALL_STATE_RECORDING == g_CallInfo.state)
	{
		log_printf("call out proc : AS_INTER_RECORDING\n");
		CallGuiNotify(g_CallInfo.state, 0);

		media_disable_audio_aec();
		media_disable_audio_dec();
		media_enable_audio_ai();
		media_add_audio_sendaddr(g_CallInfo.address, g_CallInfo.RemoteAudioPort);

		t0 = time(0);		
		g_RemainTime = 0;
		g_CallInfo.TimeOut = 0;
		g_CallInfo.HeartTime = 0;
		g_PreCallOutState = CALL_STATE_RECORDING;
		
		//set_nethead(g_CallDestNo, PRIRY_DEFAULT);			
		while (CALL_STATE_RECORDING == g_CallInfo.state)
		{
			HeartParam.Times = 0;
			g_CallInfo.HeartTime++;
			g_CallInfo.TimeOut = time(0) - t0;
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_CallInfo.address, g_CallInfo.port);
			if (g_CallInfo.HeartTime > HEART_TIMEOUT)
			{	
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallInfo.address, g_CallInfo.port);//直接发送
				log_printf("call timer proc : recording heart time out\n");
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_CallInfo.state = CALL_STATE_END;
				break;
			}
			temp = g_CallInfo.TimeOut << 16 | g_RemainTime;
			//CallGuiNotify(CALL_TIMER, temp);
			
			times = 10;
			while ((times--) > 0 && (g_CallInfo.state == CALL_STATE_RECORDING))
			{
				usleep(100*1000);
			}
		}
	}

	if (CALL_STATE_TALK == g_CallInfo.state)
	{
		ret = FALSE;
		CallGuiNotify(g_CallInfo.state, 0);
		
		uint8 TalkVolume = storage_get_talkvolume();
		
		log_printf("callout proc : AS_INTER_TALK\n");
		
		if (g_PreCallOutState != CALL_STATE_RECORDHINT && g_PreCallOutState != CALL_STATE_RECORDING)
		{
			media_stop_sound();					
			ret = media_start_net_audio(g_CallInfo.address);
			if (ret == TRUE)
			{
				g_CallInfo.IsStartAudio = TRUE;
				media_enable_audio_aec();
				media_add_audio_sendaddr(g_CallInfo.address, g_CallInfo.RemoteAudioPort);
			}
			else
			{
				log_printf("callout proc : AS_INTER_TALK : start net audio err\n");
			}
		}

		media_enable_audio_dec();
		media_enable_audio_ai();
		if (g_PreCallOutState == CALL_STATE_RECORDHINT)
		{			
			media_add_audio_sendaddr(g_CallInfo.address, g_CallInfo.RemoteAudioPort);
		}

		// 设置通话音量
		media_set_talk_volume(g_CallInfo.RemoteDeviceType, TalkVolume);
		
		// 开启视频
		if (g_CallInfo.LocalVideoSendrecv != _NONE)
		{
			ret = media_start_net_video(g_CallInfo.address, _RECVONLY);
			if (ret == FALSE)
			{
				log_printf("callout proc : AS_INTER_TALK : start net video err\n");	
			}	
		}

		t0 = time(0);
		g_CallInfo.TimeOut = 0;
		g_CallInfo.HeartTime = 0;
		HeartParam.ID = g_CallInfo.ID;
		if (g_CallInfo.TimeMax <= 0)
		{
			g_CallInfo.TimeMax = 0xFFFF;
		}
		HeartParam.Times = g_CallInfo.TimeMax;
		g_PreCallOutState = CALL_STATE_TALK;

		//set_nethead(g_CallDestNo, PRIRY_DEFAULT);			
		while (CALL_STATE_TALK == g_CallInfo.state)
		{
			g_CallInfo.HeartTime++;
			g_CallInfo.TimeOut = time(0) - t0;
			if (g_CallInfo.TimeMax > 0)
			{
				if (g_CallInfo.TimeOut >= g_CallInfo.TimeMax)
				{
					log_printf("call out timer: CALL_STATE_TALK : call time out\n");
					HeartParam.Times = 0;
					set_nethead(g_CallDestNo, PRIRY_DEFAULT);
					net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_CallInfo.address, g_CallInfo.port);
					net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallInfo.address, g_CallInfo.port);

					g_ErrType = END_BY_CALL_TIMEOUT;
					g_CallInfo.state = CALL_STATE_END;
					break;
				}
				else
				{
					// 剩余通话时间
					HeartParam.Times = g_CallInfo.TimeMax - g_CallInfo.TimeOut;

					// 消逝时间+剩余通话时间
					temp = (g_CallInfo.TimeOut << 16) | HeartParam.Times;
					//CallGuiNotify(CALL_TIMER, temp);	
				}
			}
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_CallInfo.address, g_CallInfo.port);
			
			if (g_CallInfo.HeartTime > HEART_TIMEOUT)
			{
				log_printf("call out timer: CALL_STATE_TALK : heart time out\n");
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallInfo.address, g_CallInfo.port);//直接发送
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_CallInfo.state = CALL_STATE_END;
				break;
			}
			//sleep(1);
			times = 10;
			while ((times--) > 0 && (g_CallInfo.state == CALL_STATE_TALK))
			{
				usleep(100*1000);
			}
		}
	}

	log_printf("callout proc : AS_INTER_END : g_ErrType : %d\n", g_ErrType);
	if (CALL_STATE_CALLING == g_PreCallOutState)
	{
		// 关闭回铃音
		media_stop_sound();
	}

	if (g_PreCallOutState == CALL_STATE_RECORDHINT
		|| g_PreCallOutState == CALL_STATE_RECORDING
		|| g_PreCallOutState == CALL_STATE_TALK)
	{
		media_del_audio_send_addr(g_CallInfo.address, MEDIA_AUDIO_PORT);	
		usleep(10*1000);
		media_stop_net_audio();
		usleep(100*1000);
	}

	if (CALL_STATE_TALK == g_PreCallOutState)
	{
		if (g_CallInfo.LocalVideoSendrecv != _NONE)
		{
			media_stop_net_video(_RECVONLY);
		}
	}

	// 取消对讲状态
	sys_set_intercomm_state(FALSE);
	
	g_CallInfo.state = CALL_STATE_END;
	CallGuiNotify(g_CallInfo.state, g_ErrType);
	g_ErrType = END_BY_SELF_ERR;
	g_CallInfo.state = CALL_STATE_NONE;
	g_PreCallOutState = CALL_STATE_NONE;
	g_CallInfo.address = 0;
	memset(&g_NewBeCallInfo, 0, sizeof(g_NewBeCallInfo)); 
	g_CallRefuseNum = 0;
	log_printf("callout proc end!\n");
	inter_SetThread(&g_CallInfo.mThread);
	log_printf("inter_SetThread\n");
    pthread_exit(NULL);
}

/*************************************************
  Function:			becall_proc
  Description:		被叫线程执行函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void * becall_proc(void *arg)
{
	pthread_detach(pthread_self());
	time_t t0;
	uint32 temp = 0;
	char CallNo[32] = {0};
	char data[8] = {0};
	int32 NoDisturbState = FALSE;
	static int32 times = 10;
	LVEWORD_STATE_TYPE LvdWordState = LVEWORD_TYPE_NONE;	

	g_BeCallInfo.IsStartAudio = FALSE;
	HeartParam.ID = g_BeCallInfo.ID;
	memcpy(data, (char *)&g_BeCallInfo.ID, 4);
	strcpy(CallNo, g_BeCallInfo.CallNo1);
	CallNo[30] = g_BeCallInfo.RemoteDeviceType;
	CallNo[31] = g_BeCallInfo.LocalVideoSendrecv;

	if (g_BeCallInfo.state == CALL_STATE_CALLING)
	{
		g_PreBeCallState = CALL_STATE_CALLING;
		
		BeCallGuiNotify(g_BeCallInfo.state, 0);

		// 启动视频
		if (g_BeCallInfo.LocalVideoSendrecv != _NONE)
		{
			log_printf("g_BeCallInfo.LocalVideoSendrecv : 0X%x\n", g_BeCallInfo.LocalVideoSendrecv);
			media_start_net_video(g_BeCallInfo.address, g_BeCallInfo.LocalVideoSendrecv);	
			usleep(500*1000);						// mody by chenbh 解决有时候视频出不来问题
		}
		
		NoDisturbState = linkage_get_nodisturb_state();		
		LvdWordState = linkage_get_lvd_state();				

		// 获取留影留言格式
		g_BeCallInfo.LeaveWordMode = storage_get_lyly_mode();
		g_BeCallInfo.LeaveWordState = LvdWordState;
		if (g_BeCallInfo.LeaveWordMode != LWM_NONE)
		{
			if (g_BeCallInfo.RemoteDeviceType != DEVICE_TYPE_STAIR
				&& g_BeCallInfo.RemoteDeviceType != DEVICE_TYPE_AREA
				&& g_BeCallInfo.RemoteDeviceType != DEVICE_TYPE_DOOR_NET)
			{
				g_BeCallInfo.LeaveWordMode = LWM_AUDIO;
			}	

			// 中心机不进入留言
			if (g_BeCallInfo.RemoteDeviceType == DEVICE_TYPE_MANAGER)
			{
				g_BeCallInfo.LeaveWordMode = LWM_NONE;
			}
		}

		log_printf("LeaveWordMode : %d, LeaveWordState: %d\n", g_BeCallInfo.LeaveWordMode, g_BeCallInfo.LeaveWordState);
		
		if (FALSE == is_main_DeviceNo())		// edit by txl 111013 修改分机不进行留影留言
		{
			g_BeCallInfo.LeaveWordMode = LWM_NONE;
		}
		
		// 不处于免打扰状态 edit by chenbh 留言模式为空需要振铃
		if (NoDisturbState == FALSE 
			&& ((LVEWORD_TYPE_AUTO != g_BeCallInfo.LeaveWordState) || 
			(g_BeCallInfo.LeaveWordMode == LWM_NONE)))
		{
			uint8 RingVolume = storage_get_ringvolume();
			char RingFile[50] = {0};

			// 获取铃声音量
			get_ring_file(RingFile);
			media_play_sound(RingFile, TRUE, NULL);
			media_set_output_volume(RingVolume);
		}	
												
		t0 = time(0);			
		g_ErrType = END_BY_SELF_ERR;
		g_BeCallInfo.TimeOut = 0;
		g_BeCallInfo.HeartTime = 0;

		while(CALL_STATE_CALLING == g_BeCallInfo.state)
		{
			g_BeCallInfo.TimeOut = time(0) - t0;
			log_printf("g_BeCallInfo.TimeOut = %d \n",g_BeCallInfo.TimeOut);
			if (LWM_NONE != g_BeCallInfo.LeaveWordMode)
			{
				// LEAVEWORD_TIMEOUT-5: 防止呼叫方呼叫超时先挂断，所以提前进入超时留影留言
				if (LVEWORD_TYPE_AUTO == g_BeCallInfo.LeaveWordState
					|| (g_BeCallInfo.TimeOut >= (LEAVEWORD_TIMEOUT-5) && LVEWORD_TYPE_TIMEOUT == g_BeCallInfo.LeaveWordState)
					|| (g_Leaveword_Timeout != 0 && g_BeCallInfo.TimeOut >= (g_Leaveword_Timeout-5) && LVEWORD_TYPE_TIMEOUT == g_BeCallInfo.LeaveWordState))
				{	 	 
					if (is_main_DeviceNo())
					{
						g_BeCallInfo.state = CALL_STATE_RECORDHINT;       
					}
					else
					{
						set_nethead(g_CallDestNo, PRIRY_DEFAULT);
						net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
						g_ErrType = END_BY_SELF_ERR;
						g_BeCallInfo.state = CALL_STATE_END;
					} 
					break;													
				}
			}

			// HEART_TIMEOUT+2: 防止临界情况 			
			if (g_BeCallInfo.HeartTime > HEART_TIMEOUT+2)
			{
				log_printf("CALL_STATE_CALLING HEART_TIMEOUT \n");
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_BeCallInfo.state = CALL_STATE_END;
			}		

			if (g_BeCallInfo.TimeOut <= LEAVEWORD_TIMEOUT)
			{
				g_RemainTime = LEAVEWORD_TIMEOUT - g_BeCallInfo.TimeOut;
				temp = g_CallInfo.TimeOut << 16 | g_RemainTime;
				BeCallGuiNotify(CALL_TIMER, temp);
			}
			else
			{
				log_printf("CALL_STATE_CALLING LEAVEWORD_TIMEOUT \n");
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				g_ErrType = END_BY_SELF_ERR;
				g_BeCallInfo.state = CALL_STATE_END;
			}
			g_BeCallInfo.HeartTime++;
			//sleep(1);
			times = 10;
			while ((times--) > 0 && (CALL_STATE_CALLING == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}
	}
	
	if (CALL_STATE_RECORDHINT == g_BeCallInfo.state)
	{
		char * HintFile = NULL;		
		log_printf("becall_proc : AS_INTER_RECORD_HINT \n");
		BeCallGuiNotify(g_BeCallInfo.state, 0);	

		data[4] = CALL_STATE_RECORDHINT;
		set_nethead(g_CallDestNo, PRIRY_DEFAULT);
		net_direct_send(CMD_CALL_ANSWER, data, 5, g_BeCallInfo.address, g_BeCallInfo.port);
		memset(data + 4, 0xFF, 4);
		
		// 关闭振铃音
		media_stop_sound();
		
		// 获取留言提示音
		HintFile = storage_get_lyly_tip_path();
		usleep(200 * 1000);    	// 延迟防止v1室内机留言提示音少您字
		// 开启音频文件网络发送
		if (HintFile != NULL)
		{
			log_printf("becall proc : hint file is %s\n", HintFile);
			media_start_net_hint(g_BeCallInfo.RemoteDeviceType, HintFile, (void *)play_recordhint_callback);
			media_add_audio_sendaddr(g_BeCallInfo.address, g_BeCallInfo.RemoteAudioPort);
		}
		else
		{
			log_printf("becall proc : hint file is NULL\n");
		}

		/*
		data[4] = CALL_STATE_RECORDHINT;
		net_direct_send(CMD_CALL_ANSWER, data, 5, g_BeCallInfo.address, g_BeCallInfo.port);
		memset(data + 4, 0xFF, 4);*/

		t0 = time(0);
		g_BeCallInfo.TimeOut = 0;
		g_BeCallInfo.HeartTime = 0;
		g_PreBeCallState = CALL_STATE_RECORDHINT;

		while (CALL_STATE_RECORDHINT == g_BeCallInfo.state)
		{				
			HeartParam.ID = g_BeCallInfo.ID;
			g_BeCallInfo.TimeOut = time(0) - t0;
			g_BeCallInfo.HeartTime++;
			HeartParam.Times = RECORDHINT_TIME - g_BeCallInfo.TimeOut;
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_BeCallInfo.address, g_BeCallInfo.port);

			if (g_BeCallInfo.TimeOut >= SYS_FAMILY_RECORD_MAXTIME)
			{					
				g_BeCallInfo.state = CALL_STATE_RECORDING;
			}
			
			if (g_BeCallInfo.HeartTime > HEART_TIMEOUT)
			{
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_BeCallInfo.state = CALL_STATE_END;
			}

			times = 10;
			while ((times--) > 0 && (CALL_STATE_RECORDHINT == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}		
	}

	log_printf("g_BeCallInfo.state : 0X%x\n", g_BeCallInfo.state);
	if (CALL_STATE_RECORDING == g_BeCallInfo.state)
	{	
		char lvdFile[50] = {0};		
		log_printf("becall_proc : AS_INTER_RECORDING \n");
		BeCallGuiNotify(g_BeCallInfo.state, 0);
		
		// 获取留影留言路径文件名
		get_timer(&g_LylyDateTime);
		get_lylyrecord_path(lvdFile, &g_LylyDateTime);
			
		// 关闭留言提示音接口
		media_stop_net_hint();		
		
		// 开启留言录制接口
		int32 ret = meida_start_net_leave_rec(g_BeCallInfo.LeaveWordMode, CALL_AUDIO_PT, CALL_VIDEO_PT, lvdFile);
		if (ret == FALSE)
		{
			log_printf(" meida_start_net_leave_rec return error \n");
			g_BeCallInfo.state = CALL_STATE_END;
			g_PreBeCallState = CALL_STATE_NONE;
			g_ErrType = END_BY_USER_HANDDOW;
		}
		else
		{		
			data[4] = CALL_STATE_RECORDING; 
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_ANSWER, data, 5, g_BeCallInfo.address, g_BeCallInfo.port);
			memset(data + 4, 0xFF, 4);

			t0 = time(0);
			g_BeCallInfo.TimeOut = 0;
			g_BeCallInfo.HeartTime = 0;
			g_PreBeCallState = CALL_STATE_RECORDING;
		}
			
		while (CALL_STATE_RECORDING == g_BeCallInfo.state)
		{					
			temp = 0;
			HeartParam.ID = g_BeCallInfo.ID;
			g_BeCallInfo.TimeOut = time(0) - t0;
			g_BeCallInfo.HeartTime++;
			
			if (g_BeCallInfo.TimeOut >= LEAVEWORD_TIME)
			{
				// 挂断对方
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);

				g_ErrType = END_BY_CALL_TIMEOUT;
				g_BeCallInfo.state = CALL_STATE_END;
				break;
			}
			
			HeartParam.Times = LEAVEWORD_TIME - g_BeCallInfo.TimeOut;
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 8, g_BeCallInfo.address, g_BeCallInfo.port);
						
			// 消逝时间+剩余时间
			temp = (g_BeCallInfo.TimeOut << 16) | HeartParam.Times;
			BeCallGuiNotify(CALL_TIMER, temp);
			
			if (g_BeCallInfo.HeartTime > HEART_TIMEOUT)
			{
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_BeCallInfo.state = CALL_STATE_END;
			}
			times = 10;
			while ((times--) > 0 && (CALL_STATE_RECORDING == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}
	}

	log_printf("g_BeCallInfo.state : 0X%x\n", g_BeCallInfo.state);
	if (CALL_STATE_TALK == g_BeCallInfo.state)
	{
		log_printf("becall_proc : AS_INTER_TALK \n");
		BeCallGuiNotify(g_BeCallInfo.state, 0);

		// 停止铃声
		if (g_PreBeCallState == CALL_STATE_CALLING)
		{
			media_stop_sound();
		}
		
		if (g_PreBeCallState == CALL_STATE_RECORDHINT)
		{
			// 关闭留言提示音接口
			media_stop_net_hint();
		}

		if (g_PreBeCallState == CALL_STATE_RECORDING)
		{
			// 取消留言录制
			media_stop_net_leave_rec(FALSE);
		}

		// 设置通话音量	
		media_set_talk_volume(g_BeCallInfo.RemoteDeviceType, storage_get_talkvolume());

		// add by luofl 2011-12-07 增加咪头输入设置
		//media_set_input_volume(storage_get_micvolume());

		data[4] = CALL_STATE_TALK; 
		set_nethead(g_CallDestNo, PRIRY_DEFAULT);
		net_direct_send(CMD_CALL_ANSWER, data, 5, g_BeCallInfo.address, g_BeCallInfo.port);
		memset(data + 4, 0xFF, 4);

		// 开启通话接口
		if (media_start_net_audio(g_BeCallInfo.address) == TRUE)
		{
			g_BeCallInfo.IsStartAudio = TRUE;
		}
		media_add_audio_sendaddr(g_BeCallInfo.address, g_BeCallInfo.RemoteAudioPort);

		// 已接记录
		add_inter_record(INCOMING, g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1);

		t0 = time(0);		
		g_BeCallInfo.TimeOut = 0;
		g_BeCallInfo.HeartTime = 0;		
		g_RemainTime = 0;
		g_PreBeCallState = CALL_STATE_TALK;
		
		while (CALL_STATE_TALK == g_BeCallInfo.state)
		{
			g_BeCallInfo.TimeOut = time(0) - t0;
			HeartParam.Times = 0;
			set_nethead(g_CallDestNo, PRIRY_DEFAULT);
			net_direct_send(CMD_CALL_HEART, (char*)&HeartParam, 4, g_BeCallInfo.address, g_BeCallInfo.port);
			
			// 消逝时间+剩余时间
			temp = (g_BeCallInfo.TimeOut << 16) | g_RemainTime;
			BeCallGuiNotify(CALL_TIMER, temp);
		
			if (g_BeCallInfo.HeartTime > HEART_TIMEOUT)
			{
				set_nethead(g_CallDestNo, PRIRY_DEFAULT);
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				g_ErrType = END_BY_COMM_TIMEOUT;
				g_BeCallInfo.state = CALL_STATE_END;
				log_printf("becall_timer_proc : CALL_STATE_TALK : HEART_TIMEOUT\n");
			}
			g_BeCallInfo.HeartTime++;
			times = 10;
			while ((times--) > 0 && (CALL_STATE_TALK == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}			
	}

	log_printf("becall_proc : AS_INTER_END : current state: %d, g_ErrType: %d\n", g_BeCallInfo.state, g_ErrType);

	if (g_PreBeCallState == CALL_STATE_CALLING)
	{
		// 关闭振铃铃音
		media_stop_sound();
	}	
				
	if (g_PreBeCallState == CALL_STATE_RECORDHINT)
	{
		media_del_audio_send_addr(g_BeCallInfo.address, MEDIA_AUDIO_PORT);
		usleep(10*1000);
		media_stop_net_hint();	
	}

	if (g_PreBeCallState == CALL_STATE_RECORDING)
	{
		if (g_ErrType == END_BY_USER_HANDDOW)
		{
			// 取消录制
			media_stop_net_leave_rec(FALSE);
		}
		else
		{
			// 关闭并保存录制
			g_LylyFlg = TRUE;
			media_stop_net_leave_rec(TRUE);
		}
	}

	if (g_PreBeCallState == CALL_STATE_TALK)
	{
		media_del_audio_send_addr(g_BeCallInfo.address, MEDIA_AUDIO_PORT);
		usleep(10*1000);
		media_stop_net_audio();	
	}

	if (g_BeCallInfo.LocalVideoSendrecv != _NONE)
	{
		media_stop_net_video(g_BeCallInfo.LocalVideoSendrecv);
	}
	
	sys_set_intercomm_state(FALSE);
	g_BeCallInfo.state = CALL_STATE_END;
	BeCallGuiNotify(g_BeCallInfo.state, g_ErrType);
	g_BeCallInfo.state = CALL_STATE_NONE;
	g_BeCallInfo.address = 0;
	g_Leaveword_Timeout = 0;
	memset(&g_NewBeCallInfo, 0, sizeof(g_NewBeCallInfo)); 
	g_ElevtorAuthorize = FALSE;

	// 存储比较耗时，等界面退出在存储
	// 未接记录
	if (g_PreBeCallState != CALL_STATE_TALK)
	{
		add_inter_record(MISSED, g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1);
	}
	
	if (g_LylyFlg)
	{
		add_inter_lyly();
	}
	g_LylyFlg = FALSE;
	g_PreBeCallState = CALL_STATE_NONE;
	g_BeCallInfo.LeaveWordMode = LWM_NONE;
	g_BeCallInfo.IsStartAudio = FALSE;
	log_printf("becall_proc end!\n");
	inter_SetThread(&g_BeCallInfo.mThread);		
    pthread_exit(NULL);
}

#ifdef _DOOR_PHONE_
/*************************************************
  Function:			phone_becall_proc
  Description:		被叫线程执行函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void * phone_becall_proc(void *arg)
{
	pthread_detach(pthread_self());
	time_t t0;								
	uint32 temp = 0;
	char CallNo[32] = {0};
	char data[8] = {0};
	static int32 times = 10;
	int32 NoDisturbState = FALSE;
	
	HeartParam.ID = g_BeCallInfo.ID;
	memcpy(data, (char *)&g_BeCallInfo.ID, 4);
	strcpy(CallNo, g_BeCallInfo.CallNo1);
	CallNo[30] = g_BeCallInfo.RemoteDeviceType;
	CallNo[31] = g_BeCallInfo.LocalVideoSendrecv;

	// [开启视频接口函数]
	media_start_analog_video();
		
	// 增加状态判断
	if (g_BeCallInfo.state == CALL_STATE_CALLING)
	{
		BeCallGuiNotify(g_BeCallInfo.state, 0);

		NoDisturbState = linkage_get_nodisturb_state();	
		
		// 不处于免打扰状态 免打扰状态不响铃声
		if (NoDisturbState == FALSE)
		{
			uint8 RingVolume = storage_get_ringvolume();
			char RingFile[50] = {0};

			get_ring_file(RingFile);
			media_play_sound(RingFile, TRUE, NULL);
			media_set_output_volume(RingVolume);
		}

		uint8 count = 0;
		t0 = time(0);	
		
		g_ErrType = END_BY_SELF_ERR;
		g_BeCallInfo.TimeOut = 0;
		g_BeCallInfo.HeartTime = 0;
		g_PreBeCallState = CALL_STATE_CALLING;

		while(CALL_STATE_CALLING == g_BeCallInfo.state)
		{
			g_BeCallInfo.TimeOut = time(0) - t0;	

			// 门前机铃声的发送
			if (count%4 == 0)
			{
				hw_mk_ring_back();
				//log_printf("hw_mk_ring_back  count: %d\n", count);
			}
			count++;
			
			if (g_BeCallInfo.TimeOut <= LEAVEWORD_TIMEOUT)
			{
				g_RemainTime = LEAVEWORD_TIMEOUT - g_BeCallInfo.TimeOut;
				temp = g_CallInfo.TimeOut << 16 | g_RemainTime;
				BeCallGuiNotify(CALL_TIMER, temp);
			}
			else
			{
				log_printf("CALL_STATE_CALLING LEAVEWORD_TIMEOUT \n");
				g_ErrType = END_BY_SELF_ERR;
				g_BeCallInfo.state = CALL_STATE_END;
			}
			//sleep(1);
			times = 10;
			while ((times--) > 0 && (CALL_STATE_CALLING == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}
	}

	log_printf("g_BeCallInfo.state : 0X%x\n", g_BeCallInfo.state);
	if (CALL_STATE_TALK == g_BeCallInfo.state)
	{
		log_printf("becall_proc : AS_INTER_TALK \n");

		// 停止铃声
		if (g_PreBeCallState == CALL_STATE_CALLING)
		{
			media_stop_sound();
		}
	
		// 设置通话音量	
		media_set_talk_volume(g_BeCallInfo.RemoteDeviceType, storage_get_talkvolume());
		
		t0 = time(0);		
		g_BeCallInfo.TimeOut = 0;
		g_BeCallInfo.HeartTime = 0;		
		g_RemainTime = 0;
		g_PreBeCallState = CALL_STATE_TALK;
		
		BeCallGuiNotify(g_BeCallInfo.state, 0);
		// 已接记录
		add_inter_record(INCOMING, g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1);

		while (CALL_STATE_TALK == g_BeCallInfo.state)
		{
			g_BeCallInfo.TimeOut = time(0) - t0;
			g_RemainTime = g_BeCallInfo.TimeMax - g_BeCallInfo.TimeOut;
			if (g_BeCallInfo.TimeOut >= g_BeCallInfo.TimeMax)
			{
				g_BeCallInfo.state = CALL_STATE_END;
				break;
			}
			else
			{
				// 消逝时间+剩余时间
				temp = (g_BeCallInfo.TimeOut << 16) | g_RemainTime;
				BeCallGuiNotify(CALL_TIMER, temp);		
			}	
			
			//sleep(1);
			times = 10;
			while ((times--) > 0 && (CALL_STATE_TALK == g_BeCallInfo.state))
			{
				usleep(100*1000);
			}
		}			
	}
	log_printf("becall_proc : AS_INTER_END : current state: %d, g_ErrType: %d\n", g_BeCallInfo.state, g_ErrType);
	
	// [关闭视频接口]
	media_stop_analog_video();
	
	// [关闭被叫的接口]
	hw_switch_digit();
	
	if (g_PreBeCallState == CALL_STATE_CALLING)
	{
		// 关闭振铃铃音
		media_stop_sound();
	}
		
	// 未接记录
	if (g_PreBeCallState != CALL_STATE_TALK)
	{
		add_inter_record(MISSED, g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1);
	}
	
	sys_set_intercomm_state(FALSE);
	g_BeCallInfo.state = CALL_STATE_END;
	BeCallGuiNotify(g_BeCallInfo.state, g_ErrType);
	g_BeCallInfo.state = CALL_STATE_NONE;
	g_PreBeCallState = CALL_STATE_NONE;
	g_BeCallInfo.address = 0;
	memset(&g_NewBeCallInfo, 0, sizeof(g_NewBeCallInfo)); 
	g_ElevtorAuthorize = FALSE;
	log_printf("becall_proc end!");
	
	inter_SetThread(&g_BeCallInfo.mThread);		
    pthread_exit(NULL);
}
#endif

/*************************************************
  Function:			InterThreadExit
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
static int inter_ThreadExit(struct ThreadInfo *thread)
{
	int hangupCount = 0;

	if (NULL == thread)
	{
		return false;
	}
	
	while (thread->running == 1)
	{
		if (hangupCount++ > 5000)
		{
			log_printf("InterThreadExit time out\n");
			return false;
		}
		usleep(20);
	}
	
	return true;
}

/*************************************************
  Function:			inter_start_thread
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int inter_start_thread(struct ThreadInfo* thread, void* proc, void* param, int ID)
{
	if (NULL == thread)
	{
		log_printf("inter_start_thread failed : thread == NULL\n");
		return -1;
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	thread->exit = inter_ThreadExit;
	thread->running = 1;

	if (0 != pthread_create(&thread->thread, &attr, (void*)proc, (void*)param))
	{
		thread->running = 0;
		pthread_attr_destroy(&attr);
		log_printf("inter_start_thread failed : pthread_create error \n");
		return -1;
	}
	
	pthread_attr_destroy(&attr);
	return 0;
}

/*************************************************
  Function:			call_out
  Description:		开始呼叫,呼叫参数设置完再调用该函数建立线程
  Input: 	
  Output:			无
  Return:			无
  Others:
*************************************************/
int32 call_out(void)
{	
	if (0 != inter_start_thread(&g_CallInfo.mThread, callout_proc, (void*)&g_CallInfo, g_CallInfo.ID))
	{
		g_CallInfo.state = CALL_STATE_NONE;
		return FALSE;
	}	
	return TRUE;
}

/*************************************************
  Function:			call_in
  Description:		开始呼叫
  Input: 	
  	1.recPacket		接收到包
  Output:			无
  Return:			0-成功 1-一般错误 2-系统忙
  Others:
*************************************************/
static int32 call_in(const PRECIVE_PACKET recPacket)
{	
	char *data = NULL;
	char devnostr[30] = {0};
	char callno[30] = {0};	
	PNET_HEAD head = NULL;
	DEVICE_NO DestDevno;
	PFULL_DEVICE_NO myDev = storage_get_devparam();
	int32 ret = FALSE;

	// 查看接收视频参数
	VIDEO_SDP_PARAM videoparam;
	
	head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	if (head->DeviceType == DEVICE_TYPE_STAIR)
	{
		#ifdef _OTHER_ELEVATOR_
		set_stair0_addr();
		#endif
	}
	log_printf("call in: g_BeCallInfo.state : %d\n", g_BeCallInfo.state);	
	#ifdef _AU_PROTOCOL_
	DestDevno = head->DeviceNo;
	#else
	DestDevno = head->SrcDeviceNo;
	#endif
	
	data = recPacket->data + NET_HEAD_SIZE;
	g_BeCallInfo.address = recPacket->address;		
	g_BeCallInfo.port = recPacket->port;			
	g_BeCallInfo.TimeMax = 0;						// 超时时间,最长通话时间, 0为无限长,秒

	// 会话ID 4B＋媒体IP地址4B＋音频端口＋视频端口＋音频编码1B＋视频编码1B＋主叫号码串30B＋被叫号码串30B				
	g_BeCallInfo.ID = *(uint32 *)data;
	g_BeCallInfo.RemoteMediaAddr = *(uint32 *)(data + 4);
	if (0 == g_BeCallInfo.RemoteMediaAddr)
	{
		g_BeCallInfo.RemoteMediaAddr = recPacket->address;
	}
	g_BeCallInfo.RemoteAudioPort = *(uint16 *)(data + 8);
	g_BeCallInfo.RemoteVideoPort = *(uint16 *)(data + 10);
	g_BeCallInfo.AudioPt = data[12];
	g_BeCallInfo.VideoPt = data[13];
	memcpy(g_BeCallInfo.CallNo1, data + 14, 30); 	// 主叫设备号+设备类型
	memcpy(g_BeCallInfo.CallNo2, data + 44, 30); 	// 被叫设备号或电话号码
	g_Use_SDP = FALSE;
	#ifdef _SEND_SDP_PARAM_
	//调试时候用  视频参数
	if (head->DataLen >= 162)
	{
		memcpy(&videoparam, data+74, sizeof(VIDEO_SDP_PARAM));
		log_printf(" videoparam.bitRate= %d\n videoparam.frameRate=%d\n videoparam.imgSize= %d\n ", videoparam.bitRate, videoparam.frameRate, videoparam.imgSize);

		log_printf("head->DataLen : %d \n", head->DataLen);
		g_Leaveword_Timeout = 0;
		if (head->DataLen == 162)  
		{
			g_Leaveword_Timeout = 0;
		}
		else	// 兼容模拟门前机 加入最后一项留言超时时间
		{
			g_Leaveword_Timeout = (uint8)data[162];
		}
		g_Use_SDP = TRUE;
		log_printf("g_Leaveword_Timeout : %d \n", g_Leaveword_Timeout);
	}
	#endif
	
	switch (head->DeviceType)
	{
		case DEVICE_TYPE_MANAGER:
		case DEVICE_TYPE_AREA:
		case DEVICE_TYPE_STAIR:
			g_BeCallInfo.RemoteDeviceType = head->DeviceType;
			break; 

		// 增加门前机设备类型的情况 底下室内机类型处理方式照样可以用	
		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_DOOR_PHONE:	
			strcpy(devnostr, myDev->DeviceNoStr);
			devnostr[myDev->DevNoLen - 1] = 0;
			strncpy(callno, g_BeCallInfo.CallNo1, myDev->DevNoLen - 1);
			callno[myDev->DevNoLen - 1] = 0;
			if (!strcmp(devnostr, callno))			
			{
				strncpy(callno, g_BeCallInfo.CallNo1, myDev->DevNoLen);
				// 网络门前机
				if (callno[myDev->DevNoLen - 1] == '8'					
					|| callno[myDev->DevNoLen - 1] == '9')				
				{
					g_BeCallInfo.RemoteDeviceType = head->DeviceType;
					// 传给GUI显示的字符: '1'/'2'
					memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
					if (callno[myDev->DevNoLen - 1] == '8')
					{
						g_BeCallInfo.CallNo1[0] = '1';
					}
					else
					{	
						g_BeCallInfo.CallNo1[0] = '2';
					}
				}
				else
				{
					// 兼容AH8-E9BVAC做主机,带两个模拟门前机，模拟门前机呼入时，主机转呼分机
					if (g_BeCallInfo.CallNo1[29] == DEVICE_TYPE_DOOR_NET ||
						g_BeCallInfo.CallNo1[29] == DEVICE_TYPE_DOOR_PHONE)
					{
						memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
						if (callno[myDev->DevNoLen - 1] == '6')
						{
							g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
							g_BeCallInfo.CallNo1[0] = '1';
							break;
						}
						else if (callno[myDev->DevNoLen - 1] == '7')
						{
							g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
							g_BeCallInfo.CallNo1[0] = '2';
							break;
						}
					}					
				}
			}
			else	
			{	
				g_BeCallInfo.RemoteDeviceType = head->DeviceType;		
			}
			break;

		case DEVICE_TYPE_ROOM:
			strcpy(devnostr, myDev->DeviceNoStr);
			devnostr[myDev->DevNoLen - 1] = 0;
			strncpy(callno, g_BeCallInfo.CallNo1, myDev->DevNoLen - 1);
			callno[myDev->DevNoLen - 1] = 0;
			if (!strcmp(devnostr, callno))			
			{
				strncpy(callno, g_BeCallInfo.CallNo1, myDev->DevNoLen);
				if (callno[myDev->DevNoLen - 1] == '8'					
					|| callno[myDev->DevNoLen - 1] == '9')				
				{
					// 网络门前机
					g_BeCallInfo.RemoteDeviceType = g_BeCallInfo.CallNo1[29];

					// 传给GUI显示的字符: '1'/'2'
					memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
					if (callno[myDev->DevNoLen - 1] == '8')
					{
						g_BeCallInfo.CallNo1[0] = '1';
					}
					else
					{	
						g_BeCallInfo.CallNo1[0] = '2';
					}
				}
				else
				{
					// 室内分机
					g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_FENJI_NET;

					// 兼容AH8-E9BVAC做主机,带两个模拟门前机，模拟门前机呼入时，主机转呼分机
					if (g_BeCallInfo.CallNo1[29] == DEVICE_TYPE_DOOR_NET)
					{
						memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
						if (callno[myDev->DevNoLen - 1] == '6')
						{
							g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
							g_BeCallInfo.CallNo1[0] = '1';
							break;
						}
						else if (callno[myDev->DevNoLen - 1] == '7')
						{
							g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
							g_BeCallInfo.CallNo1[0] = '2';
							break;
						}
					}
					
					// 传给GUI显示的字符: '0'-'7'
					memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
					g_BeCallInfo.CallNo1[0] = callno[myDev->DevNoLen - 1];
				}
			}
			else	// 住户
			{	
				g_BeCallInfo.RemoteDeviceType = head->DeviceType;		
			}
			break;
		default:														
			g_BeCallInfo.RemoteDeviceType = g_BeCallInfo.CallNo1[29];
			break;
	}
	
	g_BeCallInfo.CallNo1[29] = 0;
	if (g_BeCallInfo.RemoteDeviceType == DEVICE_TYPE_NONE)
	{
		log_printf("call in : device type err! \n");
		return 1;
	}
	
	g_BeCallInfo.LocalMediaAddr = 0;
	g_BeCallInfo.LocalAudioPort = NETAUDIO_UDP_PORT;
	g_BeCallInfo.LocalVideoPort = NETVIDEO_UDP_PORT;
	g_BeCallInfo.IsStartVideo = FALSE;
	g_BeCallInfo.IsStartAudio = FALSE;
	g_BeCallInfo.isNat = FALSE;
	
	switch (g_BeCallInfo.RemoteDeviceType)
	{
		case DEVICE_TYPE_MANAGER:					
			#ifdef _MANAGER_IS_VIDEO_ 
			g_BeCallInfo.LocalVideoSendrecv = _RECVONLY;
			#else
			g_BeCallInfo.LocalVideoSendrecv = _NONE;
			#endif
			break;
			
		case DEVICE_TYPE_AREA:						
		case DEVICE_TYPE_STAIR:						
			g_BeCallInfo.LocalVideoSendrecv = _RECVONLY;			
			break;
			
		case DEVICE_TYPE_ROOM:								
			if (!net_is_lan_ip(recPacket->address))				
			{
				log_printf("call in : ip is not in lan\n");
				return 1;
			}
			else
			{
				g_BeCallInfo.LocalVideoSendrecv = _NONE;				
			}
			break;
			
		case DEVICE_TYPE_FENJI_NET:		
			g_BeCallInfo.LocalVideoSendrecv = _NONE;
			break;
			
		case DEVICE_TYPE_DOOR_NET:		
			g_BeCallInfo.LocalVideoSendrecv = _RECVONLY;
			break;
			
		default:
			log_printf("call in : remote dev type err\n");
			return 1;
	}

	// add by chenbh 2016-02-18 不需要视频时 LocalVideoPort值应为0
	if (_NONE == g_BeCallInfo.LocalVideoSendrecv)
	{
		g_BeCallInfo.LocalVideoPort = 0;
	}
	
	// 系统仲裁
	ret = sys_set_intercomm_state(TRUE);
	if (ret != 0)
	{
		log_printf("call in : sys is busy now, ret: %d\n", ret);
		show_sys_event_hint(AS_CALLIN);
		return 2;
	}
	
	// 启动被叫应用
	INTER_INFO_S BeCallInfo;
	BeCallInfo.InterType = INTER_CALLIN_E;
	BeCallInfo.DevType = g_BeCallInfo.RemoteDeviceType;
	sprintf(BeCallInfo.DevStr, "%s", g_BeCallInfo.CallNo1);	
	g_BeCallInfo.state = CALL_FORMSHOW_CALLING;
	BeCallGuiNotify(g_BeCallInfo.state, &BeCallInfo);
	g_BeCallInfo.state = CALL_STATE_CALLING;
	g_CallDestNo = DestDevno;
	usleep(100*1000);  			// 睡眠过程可能触发警情、把状态改变
	if (0 != inter_start_thread(&g_BeCallInfo.mThread, becall_proc, (void*)&g_BeCallInfo, g_BeCallInfo.ID))
	{
		g_BeCallInfo.state = CALL_STATE_NONE;
		return false;
	}	
	
	return 0;
}

/*************************************************
  Function:			new_call_in
  Description:		新的呼入
  Input: 	
  	1.recPacket		接收到包
  Output:			无
  Return:			
  Others:
*************************************************/
static void new_call_in(const PRECIVE_PACKET recPacket, char *NewCallNo)
{
	char *data = NULL;
	char LocalDevStr[30] = {0};
	char CallNo[30] = {0};	
	char tmp[30] = {0};
	PNET_HEAD head = NULL;
	PFULL_DEVICE_NO myDev = storage_get_devparam();
	uint8 RemoteDeviceType = 0;
	
	data = recPacket->data + NET_HEAD_SIZE;
	memcpy(tmp, data + 14, 30); 					// 主叫设备号+设备类型

	head =  (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	switch (head->DeviceType)
	{
		case DEVICE_TYPE_MANAGER:
		case DEVICE_TYPE_AREA:
		case DEVICE_TYPE_STAIR:
			sprintf(CallNo, "%s", tmp);
			RemoteDeviceType = head->DeviceType;
			break;
			
		case DEVICE_TYPE_DOOR_NET:
			strcpy(LocalDevStr, myDev->DeviceNoStr);
			LocalDevStr[myDev->DevNoLen - 1] = 0;
			strncpy(CallNo, tmp, myDev->DevNoLen - 1);
			CallNo[myDev->DevNoLen - 1] = 0;
			RemoteDeviceType = head->DeviceType;
			if (!strcmp(LocalDevStr, CallNo))		// 房号一致			
			{
				memset(CallNo, 0, sizeof(CallNo));
				if (tmp[myDev->DevNoLen - 1] == '8'					
					|| tmp[myDev->DevNoLen - 1] == '9')				
				{
					// 传给GUI显示的字符: '1'/'2'
					if (tmp[myDev->DevNoLen - 1] == '8')
					{
						CallNo[0] = '1';
					}
					else
					{	
						CallNo[0] = '2';
					}
				}
				else
				{
					if (tmp[29] == DEVICE_TYPE_DOOR_NET)
					{
						RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
						if (tmp[myDev->DevNoLen - 1] == '6')
						{
							CallNo[0] = '1';
						}
						else if (tmp[myDev->DevNoLen - 1] == '7')
						{
							CallNo[0] = '2';
						}
						break;
					}				
				}
			}
			break;

		case DEVICE_TYPE_ROOM:
			strcpy(LocalDevStr, myDev->DeviceNoStr);
			LocalDevStr[myDev->DevNoLen - 1] = 0;
			strncpy(CallNo, tmp, myDev->DevNoLen - 1);
			CallNo[myDev->DevNoLen - 1] = 0;
			if (!strcmp(LocalDevStr, CallNo))		// 房号一致			
			{
				memset(CallNo, 0, sizeof(CallNo));
				if (tmp[myDev->DevNoLen - 1] == '8'					
					|| tmp[myDev->DevNoLen - 1] == '9')				
				{
					// 网络门前机
					RemoteDeviceType = tmp[29];

					// 传给GUI显示的字符: '1'/'2'
					if (tmp[myDev->DevNoLen - 1] == '8')
					{
						CallNo[0] = '1';
					}
					else
					{	
						CallNo[0] = '2';
					}
				}
				else
				{
					// 室内分机
					RemoteDeviceType = DEVICE_TYPE_FENJI_NET;
					// add by xiewr 101209, AH8-E9BVAC模拟门前机呼入新来电提示
					if (tmp[29] == DEVICE_TYPE_DOOR_NET)
					{
						RemoteDeviceType = DEVICE_TYPE_DOOR_NET;
						if (tmp[myDev->DevNoLen - 1] == '6')
						{
							CallNo[0] = '1';
						}
						else if (tmp[myDev->DevNoLen - 1] == '7')
						{
							CallNo[0] = '2';
						}
						break;
					}
					// end by xiewr 101209
					
					// 传给GUI显示的字符: '0'-'7'
					CallNo[0] = tmp[myDev->DevNoLen - 1];
				}
			}
			else				// 住户
			{	
				memset(CallNo, 0, sizeof(CallNo));
				sprintf(CallNo, "%s", tmp);
				RemoteDeviceType = head->DeviceType;		
			}
			break;
			
		default:							
			sprintf(CallNo, "%s", tmp);
			RemoteDeviceType = tmp[29];
			break;
	}
	
	CallNo[29] = RemoteDeviceType;
	if (NewCallNo)
	{
		memcpy(NewCallNo, CallNo, sizeof(CallNo));
	}

	g_NewBeCallInfo.address = recPacket->address;		
	memset(g_NewBeCallInfo.CallNo1, 0, sizeof(g_NewBeCallInfo.CallNo1));
	g_NewBeCallInfo.ID = *(uint32 *)data;
	sprintf(g_NewBeCallInfo.CallNo1, "%s", CallNo);
	g_NewBeCallInfo.CallNo1[29] = 0;
	g_NewBeCallInfo.RemoteDeviceType = RemoteDeviceType;
}

#ifdef _DOOR_PHONE_
/*************************************************
  Function:				phone_call_in
  Description:			模拟门前机呼入
  Input: 	
  	1.index				门前机索引 1、2
  Output:				无
  Return:				0 成功
  Others:
*************************************************/
int32 phone_call_in(uint8 index)
{
	switch (g_BeCallInfo.state)
	{
		case CALL_STATE_NONE:
			{
				if (g_CallInfo.state == CALL_STATE_NONE)	
				{
					INTER_INFO_S BeCallInfo;

					srand(time(0));					
					g_BeCallInfo.ID = rand();			// 与数字设备区别开	
					g_BeCallInfo.address = 0XFFFFFFFF;  // 与数字设备区别开
					g_BeCallInfo.RemoteDeviceType = DEVICE_TYPE_DOOR_PHONE;
					g_BeCallInfo.LocalVideoSendrecv = _RECVONLY;
					g_BeCallInfo.TimeMax = PHONE_TALK_TIMEOUT;
					memset(g_BeCallInfo.CallNo1, 0, sizeof(g_BeCallInfo.CallNo1));
					if (index == 1)
					{
						g_BeCallInfo.CallNo1[0] = '1';
					}
					else
					{
						g_BeCallInfo.CallNo1[0] = '2';
					}

					// 系统仲裁
					int32 ret = sys_set_intercomm_state(TRUE);
					if (ret != 0)
					{
						log_printf("call in : sys is busy now, ret: %d\n", ret);
						show_sys_event_hint(AS_CALLIN);
						return 2;
					}

					BeCallInfo.InterType = INTER_CALLIN_E;
					BeCallInfo.DevType = g_BeCallInfo.RemoteDeviceType;
					sprintf(BeCallInfo.DevStr, "%s", g_BeCallInfo.CallNo1);	
					g_BeCallInfo.state = CALL_FORMSHOW_CALLING;
					BeCallGuiNotify(g_BeCallInfo.state, &BeCallInfo);
					g_BeCallInfo.state = CALL_STATE_CALLING;
					usleep(100*1000);
					hw_mk_start();
					
					if (0 != inter_start_thread(&g_BeCallInfo.mThread, phone_becall_proc, (void*)&g_BeCallInfo, g_BeCallInfo.ID))
					{
						g_BeCallInfo.state = CALL_STATE_NONE;
						return false;
					}
					return TRUE;
				}
				else
					return FALSE;
			}
			break;
			
		default:
			return FALSE;
	}

	return FALSE;
}
#endif

/*************************************************
  Function:				inter_call_resident
  Description:			呼叫住户
  Input: 	
  	1.mode				0 区内 1 区间
  	2.InPutNo			输入号码
  Output:				无
  Return:				0 成功
  Others:
*************************************************/
int32 inter_call_resident(int32 mode, char * InPutNo)
{
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint32 LocalAreaCode = dev->AreaNo;
	uint32 InputAreaCode = 0; 
	char no[50] = {0};								// 输入号码
	char DeviceNo[50] = {0};						// 获取IP的设备号码
	int32 ret = 0;
	
	strcpy(no, InPutNo);	

	// 如果区号为0,则为散户，采用区间通话
    if (LocalAreaCode == 0)
    {
    	mode = 1;
	}
	
	if (1 == mode)						
	{
		if (!get_device(no, &InputAreaCode, DeviceNo))
		{
			return 1;
		}
		if (LocalAreaCode==0 && strlen(no)<10)
		{
        	sprintf(no, "0#%s", no);
        }       
		if (InputAreaCode == LocalAreaCode)
		{
			strcpy(no, DeviceNo);
			mode = 0;
		}
		else
		{
			// 末位添加分机号0
			int32 l = strlen(DeviceNo);
			DeviceNo[l] = '0'; 						
			DeviceNo[l+1] = 0;
		}
	}
	
	if (0 == mode)					
	{
		int32 len = strlen(no);
		
		if (dev->Rule.UseCellNo)				// 启用了单元号
		{
			if (len == dev->DevNoLen)			// 拨打全号(即包括分机号)
			{
				// 和自己全号比较,相同则返回1,不同则赋值
				if (!(strcmp(no, dev->DeviceNoStr)))
				{
					return 1;
				}				
				strncpy(DeviceNo, no, dev->DevNoLen);
			}
			else if (len == dev->DevNoLen - 1)	// 拨打房号(没有分机号)
			{
				strncpy(DeviceNo, dev->DeviceNoStr, dev->DevNoLen - 1);
				
				// 和自己房号比较,相同返回1
				if (!(strcmp(no, DeviceNo)))
				{	
					return 1;
				}

				strncpy(DeviceNo, no, dev->DevNoLen - 1);
				DeviceNo[dev->DevNoLen - 1] = '_';
				DeviceNo[dev->DevNoLen] = 0;
			}
			else								
			{
				// 只拨打房号
				if (len <= dev->Rule.RoomNoLen && len > 0)
				{
					uint8 i = 0;
					uint8 DecLen = 0;
					uint32 InputRoomNo = atoi(no);
					uint32 LocalRoomNo = 0;
					strncpy(DeviceNo, dev->DeviceNoStr+dev->Rule.StairNoLen, dev->Rule.RoomNoLen);
					LocalRoomNo = atoi(DeviceNo);
					log_printf("inter_call_resident : InputRoomNo : %d, LocalRoomNo : %d\n", InputRoomNo, LocalRoomNo);

					// 房号与本机房号一样
					if (InputRoomNo == LocalRoomNo || InputRoomNo == 0)
					{
						return 1;
					}

					memset(DeviceNo, 0, sizeof(DeviceNo));
					strncpy(DeviceNo, dev->DeviceNoStr, dev->Rule.StairNoLen);
					DecLen = dev->Rule.RoomNoLen-len;
					for (i = 0; i < dev->Rule.RoomNoLen; i++)
					{
						if (i < DecLen)
						{
							DeviceNo[dev->Rule.StairNoLen+i] = '0';
						}
						else
						{
							DeviceNo[dev->Rule.StairNoLen+i] = no[i-DecLen];
						}
					}
					DeviceNo[dev->DevNoLen - 1] = '_';
					DeviceNo[dev->DevNoLen] = 0;
					log_printf("inter_call_resident : DeviceNo : %s\n", DeviceNo);
				}
				else
				{
					return 1;
				}	
			}
		}
		else									// 没有启用单元号
		{
			// 拨打号码为:梯号+房号+分机号 或 梯号+单元号+房号+分机号
			if ((len + dev->Rule.CellNoLen) == dev->DevNoLen
				|| len == dev->DevNoLen)								
			{
				int32 i;
				
				// 和自己全号比较,相同则返回1
				int32 pos = dev->Rule.StairNoLen - dev->Rule.CellNoLen;
				strncpy(DeviceNo, no, pos);			// 拷贝梯口号(除去单元号)

				// 未输入单元号情况下, 单元号用本机单元号填充, 再与本机房号比较
				if ((len + dev->Rule.CellNoLen) == dev->DevNoLen)
				{
					for (i = 0; i < dev->Rule.CellNoLen; i++)
					{
						DeviceNo[pos] = dev->DeviceNoStr[pos];
						pos++;
					}
				}
				
				if ((len + dev->Rule.CellNoLen) == dev->DevNoLen)
				{
					strcpy(DeviceNo + pos, no + dev->Rule.StairNoLen - dev->Rule.CellNoLen);
				}
				else if (len == dev->DevNoLen)
				{
					strcpy(DeviceNo + pos, no + dev->Rule.StairNoLen);
				}
				if (!(strcmp(DeviceNo, dev->DeviceNoStr)))
				{
					return 1;
				} 
				
				// 单元号替换成通配符,未输入单元号情况下
				if ((len + dev->Rule.CellNoLen) == dev->DevNoLen)
				{
					pos = dev->Rule.StairNoLen - dev->Rule.CellNoLen;
					for (i = 0; i < dev->Rule.CellNoLen; i++)
					{
						DeviceNo[pos] = '_';
						pos++;
					}
				}
				DeviceNo[dev->DevNoLen] = 0;
			}
			else if((len + dev->Rule.CellNoLen + 1) == dev->DevNoLen
					|| (len+1) == dev->DevNoLen) 	// 拨打号码为:梯号+房号	或 梯号+单元号+房号						
			{
				int32 i;
				char LocalDeviceNo[20] = {0};
				int32 pos = dev->Rule.StairNoLen - dev->Rule.CellNoLen;
				strncpy(DeviceNo, no, pos);			// 拷贝梯号
				
				if ((len + dev->Rule.CellNoLen + 1) == dev->DevNoLen)
				{
					for (i = 0; i < dev->Rule.CellNoLen; i++)
					{
						DeviceNo[pos] = dev->DeviceNoStr[pos];
						pos++;
					}
				}

				if ((len + dev->Rule.CellNoLen + 1) == dev->DevNoLen)
				{
					strcpy(DeviceNo + pos, no + dev->Rule.StairNoLen - dev->Rule.CellNoLen);
				}	
				else if ((len+1) == dev->DevNoLen)
				{
					strcpy(DeviceNo + pos, no + dev->Rule.StairNoLen - dev->Rule.CellNoLen);
				}
				
				// 与本机房号相同
				strncpy(LocalDeviceNo, dev->DeviceNoStr, dev->DevNoLen - 1);
				if (!(strcmp(DeviceNo, LocalDeviceNo)))
				{
					return 1;
				}
				
				// 单元号替换成通配符,未输入单元号情况下
				if ((len + dev->Rule.CellNoLen + 1) == dev->DevNoLen)
				{
					pos = dev->Rule.StairNoLen - dev->Rule.CellNoLen;
					for (i = 0; i < dev->Rule.CellNoLen; i++)
					{
						DeviceNo[pos] = '_';
						pos++;
					}
				}
				DeviceNo[dev->DevNoLen - 1] = '_';
				DeviceNo[dev->DevNoLen] = 0;
			}
			else
			{
				// 只拨打房号
				if (len <= dev->Rule.RoomNoLen && len > 0)
				{
					uint8 i = 0;
					uint8 DecLen = 0;
					uint32 InputRoomNo = atoi(no);
					uint32 LocalRoomNo = 0;
					strncpy(DeviceNo, dev->DeviceNoStr+dev->Rule.StairNoLen, dev->Rule.RoomNoLen);
					LocalRoomNo = atoi(DeviceNo);
					log_printf("inter_call_resident : InputRoomNo : %d, LocalRoomNo : %d\n", InputRoomNo, LocalRoomNo);

					// 房号与本机房号一样
					if (InputRoomNo == LocalRoomNo || InputRoomNo == 0)
					{
						return 1;
					}

					memset(DeviceNo, 0, sizeof(DeviceNo));
					strncpy(DeviceNo, dev->DeviceNoStr, dev->Rule.StairNoLen);
					DecLen = dev->Rule.RoomNoLen-len;
					for (i = 0; i < dev->Rule.RoomNoLen; i++)
					{
						if (i < DecLen)
						{
							DeviceNo[dev->Rule.StairNoLen+i] = '0';
						}
						else
						{
							DeviceNo[dev->Rule.StairNoLen+i] = no[i-DecLen];
						}
					}
					DeviceNo[dev->DevNoLen - 1] = '_';
					DeviceNo[dev->DevNoLen] = 0;
					log_printf("inter_call_resident : DeviceNo : %s\n", DeviceNo);
				}
				else
				{
					return 1;
				}	
			}
		}
	}
	log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s\n", DeviceNo);

	g_CallInfo.TimeMax = FENJI_TALK_TIMEOUT;
	g_CallInfo.AudioPt = CALL_RTP_AUDIO_PT;
	g_CallInfo.VideoPt = CALL_RTP_VIDEO_PT;
	g_CallInfo.LocalAudioSendrecv = _SENDRECV;	 
	g_CallInfo.LocalVideoSendrecv = _NONE;			// 低成本AH8不做室内机间视频
	g_CallInfo.LocalAudioPort = NETAUDIO_UDP_PORT;
	g_CallInfo.LocalVideoPort = 0;											
	g_CallInfo.RemoteDeviceType = DEVICE_TYPE_ROOM;
	g_CallInfo.IsTerminal = -1;
	if (mode)
	{
		g_CallInfo.RemoteAreaNo = InputAreaCode;
		g_CallInfo.isNat = TRUE;
	}
	else
	{
		g_CallInfo.RemoteAreaNo = LocalAreaCode;
		g_CallInfo.isNat = FALSE;
	}

	memset(g_CallInfo.CallNo2, 0, sizeof(g_CallInfo.CallNo2));
	strcpy(g_CallInfo.CallNo2, DeviceNo);

	ret = sys_set_intercomm_state(TRUE);
	if (ret != 0)
	{
		return 2;
	}
	
	g_CallInfo.state = CALL_STATE_REQUEST;
	fill_call_destdevno(InPutNo, g_CallInfo.RemoteAreaNo);
	log_printf("CALL_STATE_REQUEST 000\n");
	return 0;
}

/*************************************************
  Function:				inter_call_sub_terminal
  Description:			呼叫分机
  Input: 	
  	1.InPutNo			输入号码
  Output:				无
  Return:				0 成功
  Others:
*************************************************/
int32 inter_call_sub_terminal(char *InputNo)
{
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint32 LocalAreaCode = dev->AreaNo;
	char no[50] = {0};								// 输入号码
	char DeviceNo[50] = {0};						// 获取IP的设备号码
	int32 len = 0;
	int32 ret = 0;
	
	strcpy(no, InputNo);	
	
	len = strlen(no);
	if (len != 1)
	{
		return 1;
	}
		
	if (no[0] < '0' || no[0] > '7')
	{
		return 1;
	}
	strncpy(DeviceNo, dev->DeviceNoStr, dev->DevNoLen);
	if (DeviceNo[dev->DevNoLen - 1] == no[0])		// 与本机分机号重复
	{
		return 1;
	}

	DeviceNo[dev->DevNoLen - 1] = no[0];
	DeviceNo[dev->DevNoLen] = 0;		
	log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s\n", DeviceNo);

	g_CallInfo.TimeMax = FENJI_TALK_TIMEOUT;
	g_CallInfo.AudioPt = CALL_RTP_AUDIO_PT;
	g_CallInfo.VideoPt = CALL_RTP_VIDEO_PT;
	g_CallInfo.LocalAudioSendrecv = _SENDRECV;	 
	g_CallInfo.LocalVideoSendrecv = _NONE;		// 低成本AH8不做室内机间视频
	g_CallInfo.LocalAudioPort = NETAUDIO_UDP_PORT;
	g_CallInfo.LocalVideoPort = 0;											
	g_CallInfo.RemoteDeviceType = DEVICE_TYPE_ROOM;
	g_CallInfo.RemoteAreaNo = LocalAreaCode;
	g_CallInfo.isNat = FALSE;
	g_CallInfo.IsTerminal = no[0]-'0';

	memset(g_CallInfo.CallNo2, 0, sizeof(g_CallInfo.CallNo2));
	strcpy(g_CallInfo.CallNo2, DeviceNo);

	ret = sys_set_intercomm_state(TRUE);
	if (ret != 0)
	{
		return 2;
	}

	g_CallInfo.state = CALL_STATE_REQUEST;
	fill_call_destdevno(InputNo, g_CallInfo.RemoteAreaNo);
	return 0;
}

/*************************************************
  Function:				inter_call_manager
  Description:			呼叫管理员机
  Input: 	
  	1.InPutNo			输入号码
  Output:				无
  Return:				0 成功
  Others:
*************************************************/
int32 inter_call_manager(char *InputNo)
{
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint32 LocalAreaCode = dev->AreaNo;
	char DeviceNo[50] = {0};						// 获取IP的设备号码
	int32 len = 0;
	uint32 ipaddr = 0;
	uint32 ManagerNo = 0;
	int32 ret = 0;
	
	strcpy(DeviceNo, InputNo);	
	
	len = strlen(DeviceNo);
	
	if (len != MANAGER_LEN)
	{
		return 1;
	}

	ManagerNo = atoi(DeviceNo);
	switch (ManagerNo)
	{
		case MANAGER_NUM+1:
			ipaddr = net_get_manager_ip(1);
			break;
			
		case MANAGER_NUM+2:
			ipaddr = net_get_manager_ip(2);
			break;
			
		case MANAGER_NUM+3:
			ipaddr = net_get_manager_ip(3);
			break;
			
		default:
			break;	// 中心机可能超过三台
	}	
	if (ipaddr <= 0)
	{
		if (net_get_manager_ip(1) == 0 &&
			net_get_manager_ip(2) == 0 &&
			net_get_manager_ip(3) == 0)
		{
			if (g_Manager_IP[0] != 0)
			{
				ipaddr = g_Manager_IP[0];
			}
			else
			{
				return 2;
			}
		}
		else
		{
			return 2;
		}
	}
	log_printf("lgoic_call.c : logic_call_resident : DeviceNo = %s, ipaddr = 0x%x\n", DeviceNo, ipaddr);
	
	if (g_CallList)
	{
		free(g_CallList);
	}
	g_CallList = malloc(sizeof(CALL_NODE_INFO));
	memset(g_CallList, 0, sizeof(CALL_NODE_INFO));
	g_CallListNum = 1;
	g_CallList[0].address = ipaddr;
	g_CallList[0].port = NETCMD_UDP_PORT;
	g_CallList[0].EchoValue = TRC_UNKNOWN;

	g_CallInfo.TimeMax = 90;
	g_CallInfo.AudioPt = CALL_RTP_AUDIO_PT;
	g_CallInfo.VideoPt = CALL_RTP_VIDEO_PT;

	g_CallInfo.LocalAudioSendrecv = _SENDRECV;	
	#ifdef _MANAGER_IS_VIDEO_
	g_CallInfo.LocalVideoSendrecv = _RECVONLY;	
	#else
	g_CallInfo.LocalVideoSendrecv = _NONE;
	#endif
	g_CallInfo.LocalAudioPort = NETAUDIO_UDP_PORT;

	#ifdef _MANAGER_IS_VIDEO_
	g_CallInfo.LocalVideoPort = NETVIDEO_UDP_PORT;											
	#else
	g_CallInfo.LocalVideoPort = 0;											
	#endif										
	g_CallInfo.RemoteDeviceType = DEVICE_TYPE_MANAGER;
	g_CallInfo.RemoteAreaNo = LocalAreaCode;
	g_CallInfo.isNat = FALSE;

	memset(g_CallInfo.CallNo1, 0, sizeof(g_CallInfo.CallNo1));
	strcpy(g_CallInfo.CallNo1, dev->DeviceNoStr);
	g_CallInfo.CallNo1[29] = g_CallInfo.RemoteDeviceType;
	memset(g_CallInfo.CallNo2, 0, sizeof(g_CallInfo.CallNo2));
	strcpy(g_CallInfo.CallNo2, DeviceNo);
	g_MoveTempTimes = 0;
	
	ret = sys_set_intercomm_state(TRUE);
	if (ret != 0)
	{
		return 2;
	}
	fill_call_destdevno(InputNo, g_CallInfo.RemoteAreaNo);
	g_CallInfo.state = CALL_STATE_CALLING;
	return 0;
}

/*************************************************
  Function:				inter_answer_call
  Description:			被叫接听
  Input: 			
  	1.VideoSendrecv		视频
  Output:				无
  Return:				成功与否 TRUE/FALSE
  Others:	
*************************************************/
int32 inter_answer_call(int32 * VideoSendrecv)
{
	if (g_BeCallInfo.state == CALL_STATE_TALK)
	{
		return TRUE;
	}
	if (g_BeCallInfo.state == CALL_STATE_NONE || g_BeCallInfo.state == CALL_STATE_END) // edit by chenbh 留言也能进入通话
	{
		return FALSE;
	}

	if (VideoSendrecv)
	{
		*VideoSendrecv = g_BeCallInfo.LocalVideoSendrecv;
	}	

	// 模拟门前机处理
	#ifdef _DOOR_PHONE_
	if (g_BeCallInfo.RemoteDeviceType == DEVICE_TYPE_DOOR_PHONE)
	{
		//hw_switch_analog();
		hw_mk_talk();
	}
	#endif
	
	g_BeCallInfo.state = CALL_STATE_TALK;

	return TRUE;
}

/*************************************************
  Function:			phone_hand_down
  Description:		门口挂断室内
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
void phone_hand_down(void)
{
	g_ErrType = END_BY_USER_HANDDOW;
	if (g_BeCallInfo.state != CALL_STATE_NONE)
	{
		g_BeCallInfo.state = CALL_STATE_END;
	}
}

/*************************************************
  Function:			inter_hand_down
  Description:		挂断
  Input: 			无
  Output:			无
  Return:			成功与否 TRUE/FALSE
  Others:
*************************************************/
void inter_hand_down(void)
{	
	log_printf("inter hand down : call state:%d, becall state:%d\n", g_CallInfo.state, g_BeCallInfo.state);
	g_ErrType = END_BY_USER_HANDDOW;
	if (g_CallInfo.state != CALL_STATE_NONE)
	{
		if (g_CallInfo.mThread.running != 0)
		{
			if (g_CallInfo.state == CALL_STATE_CALLING)
			{
				int32 i;
				for (i = 0; i < g_CallListNum; i++)
				{
					net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallList[i].address, g_CallList[i].port);
				}
			}
			else if (g_CallInfo.state == CALL_STATE_REQUEST)
			{
				log_printf("inter hand down : call request state\n");
			}
			else
			{
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallInfo.address, g_CallInfo.port);
			}										
			g_CallInfo.state = CALL_STATE_END;
		}
		else
		{
			log_printf("inter hand down : call thread handle is NULL\n");
		}
	}
	else if (g_BeCallInfo.state != CALL_STATE_NONE)
	{
		if (g_BeCallInfo.mThread.running != 0)
		{
			#ifdef _DOOR_PHONE_
			if (g_BeCallInfo.RemoteDeviceType == DEVICE_TYPE_DOOR_PHONE)
			{
				// 关闭操作在线程里面处理
				g_BeCallInfo.state = CALL_STATE_END;
			}
			else
			#endif
			{
				net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				if (g_BeCallInfo.state == CALL_STATE_CALLING)
				{			
					// 再发一次挂断，避免呼叫方收不到命令再次呼入
					net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_BeCallInfo.ID, 4, g_BeCallInfo.address, g_BeCallInfo.port);
				}
			}
			g_BeCallInfo.state = CALL_STATE_END;
		}
	}
}

/*************************************************
  Function:				inter_call_ini
  Description:			对讲初始化
  Input: 	
  	1.CallGuiNotify		呼叫GUI回调函数
  	2.BeCallGuiNotify	被叫GUI回调函数
  Output:				无
  Return:				
  Others:
*************************************************/
void inter_call_ini(PFGuiNotify CallGuiNotify, PFGuiNotify BeCallGuiNotify)
{
	g_CallInfo.gui_notify = CallGuiNotify;
	g_BeCallInfo.gui_notify = BeCallGuiNotify;
}

/*************************************************
  Function:			inter_video_snap
  Description:		抓拍
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 inter_video_snap(void)
{
	uint32 ret;
	char FileName[50] = {0};
	ZONE_DATE_TIME DateTime;	
	get_timer(&DateTime);
	get_photo_path(FileName, &DateTime);
	
	if (g_BeCallInfo.state != CALL_STATE_NONE && g_BeCallInfo.state != CALL_STATE_END)
	{
		switch (g_BeCallInfo.RemoteDeviceType)
		{
			case DEVICE_TYPE_STAIR:	
			case DEVICE_TYPE_AREA:
			case DEVICE_TYPE_DOOR_NET:
			case DEVICE_TYPE_DOOR_PHONE:
				break;
				
			default:
				return FALSE;
					
		}
		ret = media_snapshot(FileName, st_snap_state_callback, g_BeCallInfo.RemoteDeviceType);
		if (ret == TRUE)
		{
			storage_add_photo(g_BeCallInfo.RemoteDeviceType, g_BeCallInfo.CallNo1, DateTime);
		}	
	}	
	else
	{
		ret = FALSE;
	}

	return ret;

}

/*************************************************
  Function:			inter_unlock
  Description:		室内机开锁
  Input: 			无
  Output:			无
  Return:			TRUE / FALSE
  Others:
*************************************************/
int32 inter_unlock(void)
{

	if (g_BeCallInfo.state == CALL_STATE_NONE)
	{
		return FALSE;
	}
	
	switch (g_BeCallInfo.RemoteDeviceType)
	{
		case DEVICE_TYPE_AREA:						// 区口机呼入
		case DEVICE_TYPE_STAIR:						// 梯口机呼入
		case DEVICE_TYPE_DOOR_NET:					// 网络门前机呼入
			{
				/*									// del by xiewr 20101129, 兼容AH8其他室内机，无开锁命令应答
				uint8 EchoValue;

				int32 ret = net_send_command(CMD_UNLOCK, NULL, 0, g_BeCallInfo.address, g_BeCallInfo.port, 2, &EchoValue, NULL, NULL);
				log_printf("inter_unlock :  ret = %d, EchoValue = %d \n", ret, EchoValue);
				
				if (ret && EchoValue == ECHO_OK)
				{
					return TRUE;
				}
				return FALSE;
				*/
				// add by xiewr 20101129, 兼容AH8其他室内机，无开锁命令应答
				set_nethead(g_CallDestNo, PRIRY_REALTIME);
				net_direct_send(CMD_UNLOCK, NULL, 0, g_BeCallInfo.address, g_BeCallInfo.port);	
				#if 0
				// add by luofl 2014-07-11 不在通话状态下开锁时,挂断
				if (g_BeCallInfo.state != CALL_STATE_TALK)
				{
					g_UnlockFlag = TRUE;
					inter_hand_down();
				}
				#endif
				return TRUE;
			}
			break;

		#ifdef _DOOR_PHONE_
		case DEVICE_TYPE_DOOR_PHONE:				// 模拟门前机呼入
			{
				int32 ret1 = hw_mk_lock();
				if (ret1 == 0)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}				
			}
			break;
		#endif
			
		default:
			return FALSE;
	}

}

/*************************************************
  Function:			inter_elevator_authorize
  Description:		电梯授权
  Input: 			无
  Output:			无
  Return:			TRUE / FALSE
  Others:
*************************************************/
int32 inter_elevator_authorize(void)
{
	char data[61] = {0};
	char LocalStariNo[20] = {0};
	char RemoteStariNo[20] = {0};
	PFULL_DEVICE_NO devno =  storage_get_devparam();
	int32 ret = 0;
	uint32 addr = get_stair0_addr();

	log_printf("inter_elevator_authorize : addr : 0x%x\n", addr);
	if (addr <= 0)
	{
		return FALSE;
	}
	
	if (g_BeCallInfo.state != CALL_STATE_TALK)
	{
		return FALSE;
	}
	if (g_BeCallInfo.RemoteDeviceType != DEVICE_TYPE_ROOM 
		&& g_BeCallInfo.RemoteDeviceType != DEVICE_TYPE_STAIR)
	{
		return FALSE;
	}	
	
	strncpy(LocalStariNo, devno->DeviceNoStr, devno->Rule.StairNoLen);
	strncpy(RemoteStariNo, g_BeCallInfo.CallNo1, devno->Rule.StairNoLen);
	// 不同单元互呼
	if (strcmp(LocalStariNo, RemoteStariNo) != 0)
	{
		return FALSE;
	}
		
	// 已经被授权，只能授权一次
	if (g_ElevtorAuthorize == TRUE)
	{
		return FALSE;
	}
	
	data[0] = g_BeCallInfo.RemoteDeviceType;
	memcpy(data+1, g_BeCallInfo.CallNo1, 30);
	memcpy(data+31, devno->DeviceNoStr, sizeof(devno->DeviceNoStr));
	log_printf("inter_elevator_authorize : CallNo1:%s, Local Dev Str:%s, addr:0x%x\n", g_BeCallInfo.CallNo1, devno->DeviceNoStr, addr);
	set_nethead(g_CallDestNo, PRIRY_DEFAULT);
	ret = net_direct_send(CMD_ELEVATOR_CALL_AUTHORIZE, data, 61, addr, NETCMD_ELEVATOR_PORT);
	if (ret > 0)
	{
		g_ElevtorAuthorize = TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#ifdef _OTHER_ELEVATOR_
/*************************************************
  Function:		other_dianti_set_cmd
  Description:	发送命令给第三方电梯控制器
  Input:
  	cmd			命令值
  	callno		主叫号码
  Output:		无
  Return:		TRUE/FALSE
  Others:		01-上行 02-下行 03-召唤电梯到一楼,然后
  				上行到本楼	
*************************************************/
int32 other_dianti_send_cmd(uint8 cmd)
{
	uint8 i;
	int32 ret = 0;
	char data[61] = {0};
	PFULL_DEVICE_NO devno =  storage_get_devparam();
	uint32 tcp485Addr = get_dianti_ip();
	uint32 addr = get_stair0_addr();

	log_printf("other_dianti_send_cmd : addr : 0x%x\n", addr);
	if (addr <= 0)
	{
		return FALSE;
	}

	log_printf("other_dianti_send_cmd---1 : CallNo1:%s, Local Dev Str:%s, tcp485Addr:0x%x\n", g_BeCallInfo.CallNo1, devno->DeviceNoStr, tcp485Addr);
	data[0] = cmd;		
	memcpy(data+1, g_BeCallInfo.CallNo1, 30);
	memcpy(data+31, devno->DeviceNoStr, sizeof(devno->DeviceNoStr));
	data[51] = (uint8)(tcp485Addr >> 24) & 0xff;
	data[52] = (uint8)(tcp485Addr >> 16) & 0xff;
	data[53] = (uint8)(tcp485Addr >> 8) & 0xff;
	data[54] = (uint8)(tcp485Addr) & 0xff;

	log_printf("SendBuf:\n");
	for (i = 0; i < 61; i++)
	{
		log_printf(" - %02x", data[i]);
	}
	log_printf("\n");
	
	ret = net_direct_send(CMD_ELEVATOR_OTHER_CTRL, data, 61, addr, NETCMD_ELEVATOR_PORT);
	if (ret > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif

/*************************************************
  Function:			inter_call_distribute
  Description:		对讲呼叫类发送包命令处理函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 inter_call_distribute(const PRECIVE_PACKET recPacket)
{
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	int32 echo = ECHO_OK;
	int32 cmd = head->command | SSC_INTERPHONE << 8;
	uint32 CallID = *(uint32 *)(recPacket->data + NET_HEAD_SIZE);
	int32 i;
	
	switch (cmd)
	{	
		case CMD_CALL_CALLING:	
		{
			char EchoData[35] = {0};
			uint16 state = TRC_RINGING;	
			static uint8 doCallIn = FALSE;
			uint8 len = 2;
			memset(EchoData, 0, sizeof(EchoData));
				
 			log_printf("call_distribute: CMD_CALL_CALLING : g_BeCallInfo.state:%d\n", g_BeCallInfo.state);
			switch (g_BeCallInfo.state)
			{
				case CALL_STATE_NONE:		
					if (g_CallInfo.state == CALL_STATE_NONE)	
					{
						if (doCallIn)
		                {
		                    if (g_BeCallInfo.address == recPacket->address)
		                    {
		                    	// 不处理,不应答
		                        return TRUE;		
		                    }    
		                    else
		                    {
		                        state = TRC_BUSY;
		                    }    
		                }
		                else
		                {
		                	uint8 ret = 0;
		                    doCallIn = TRUE;
		                    ret = call_in(recPacket);
		                    if (ret == 1)
		                    {
		                        echo = ECHO_ERROR;
		                    }    
		                    else if (ret == 2)
		                    {
		                    	state = TRC_BUSY;
		                    }
		                    doCallIn = FALSE;
		                }
					}
					else															
					{
						// 新的呼入提示
						char CallNo[30] = {0};
						if (g_CallInfo.state == CALL_STATE_TALK)
						{
							/*
							char *data = recPacket->data + NET_HEAD_SIZE;
							memcpy(CallNo, data + 14, 30);
							CallNo[29] = 0;
							
							if (g_CallInfo.gui_notify)
							{
								g_CallInfo.gui_notify(CALL_STATE_CALLING, (uint32)CallNo);
							}
							*/
							// modi by luofl 2011-09-09解决在通话中只显示一个新来电问题
							//if (g_NewBeCallInfo.ID == 0)
							{
								new_call_in(recPacket, CallNo);
								CallGuiNotify(CALL_NEW_CALLING, (uint32)CallNo);
							}	
						}
						state = TRC_BUSY;
					}
					break;
				case CALL_STATE_CALLING:											
					if (CallID == g_BeCallInfo.ID && g_BeCallInfo.address == recPacket->address)
					{
						state = TRC_RINGING;
						g_BeCallInfo.HeartTime = 0;							
					}
					else
					{
						state = TRC_BUSY;
					}
					break;
				case CALL_STATE_RECORDHINT:	
				case CALL_STATE_RECORDING:	
				case CALL_STATE_TALK:
					if (CallID==g_BeCallInfo.ID && g_BeCallInfo.address==recPacket->address)
					{
						state = TRC_OK;											
					}
					else
					{
						// 新的呼入提示
						char CallNo[30]; 
						memset(CallNo, 0, sizeof(CallNo));
						if (g_BeCallInfo.state == CALL_STATE_TALK)
						{
							/*
							char *data = recPacket->data + NET_HEAD_SIZE;
							memcpy(CallNo, data + 14, 30);
							CallNo[29] = 0;
							if (g_BeCallInfo.gui_notify)
							{
								g_BeCallInfo.gui_notify(CALL_STATE_CALLING, (uint32)CallNo);
							}
							*/
							// modi by luofl 2011-09-09解决在通话中只显示一个新来电问题
							//if (g_NewBeCallInfo.ID == 0)
							{
								new_call_in(recPacket, CallNo);
								BeCallGuiNotify(CALL_NEW_CALLING, (uint32)CallNo);
							}
						}
						state = TRC_BUSY;
					}
					break;
				case CALL_STATE_END:
					if (CallID==g_BeCallInfo.ID && g_BeCallInfo.address==recPacket->address)
					{
						state = TRC_OK;
					}
					else
					{
						state = TRC_BUSY;
					}
					break;
				default:
					echo = ECHO_ERROR;
					break;
			}
			
			if (ECHO_OK == echo)
			{
				memcpy(EchoData, &state, 2);
				len = 2;
				switch (state)
				{
					case TRC_TRYING:
						break;
						
					case TRC_RINGING:
						if (DEVICE_TYPE_DOOR_NET == g_BeCallInfo.RemoteDeviceType)
						{
							set_audio_pack_mode(1);	// GM8126门前机应答是单包,实际是要组包发过去,否则会无声音
						}
						else
						{
							set_audio_pack_mode(head->Encrypt);
						}
						memcpy(EchoData+2, &g_BeCallInfo.LocalMediaAddr, 4);
						memcpy(EchoData+6, &g_BeCallInfo.LocalAudioPort, 2);
						memcpy(EchoData+8, &g_BeCallInfo.LocalVideoPort, 2);
						len = 10;
						break;
						
					case TRC_QUEUED:
						break;
						
					case TRC_OK:
						EchoData[2] = g_BeCallInfo.state;				
						len = 3;
						break;
						
					case TRC_NOT_FOUND:
						break;
						
					case TRC_BUSY:
						break;
				}

				#ifdef _SEND_SDP_PARAM_
				if (g_Use_SDP == TRUE)
				{
					if (storage_get_lyly_enable() && TRUE == is_main_DeviceNo())
					{
						memcpy(EchoData+len, &g_lyly_venc_parm, sizeof(VIDEO_SDP_PARAM));
					}
					else
					{
						memcpy(EchoData+len, &g_venc_parm, sizeof(VIDEO_SDP_PARAM));
					}
					len += sizeof(VIDEO_SDP_PARAM);
					memcpy(EchoData+len, &g_audio_parm, sizeof(AUDIO_SDP_PARAM));
					len += sizeof(AUDIO_SDP_PARAM);
				}
				#endif
				net_send_echo_packet(recPacket, echo, EchoData, len);
			}
			else
			{
				log_printf("call_distribute: echo err!\n");
				net_send_echo_packet(recPacket, echo, NULL, 0);
			}
			break;
		}
			
		case CMD_CALL_ANSWER:		
		{
			uint8 state = recPacket->data[NET_HEAD_SIZE+4];
			if (head->DataLen != 5)
			{
				break;
			}
			
			switch (g_CallInfo.state)
			{
				case CALL_STATE_CALLING:							//呼叫状态切换到其它状态
					if (g_CallListNum > 0 && g_CallList != NULL)  	//呼叫地址信息
					{
						for (i = 0; i < g_CallListNum; i++)
						{
							if (g_CallList[i].address == recPacket->address)
							{
								g_CallInfo.address = g_CallList[i].address;
								g_CallInfo.port = g_CallList[i].port;
								g_CallInfo.RemoteMediaAddr = g_CallList[i].MediaAddr;	
								g_CallInfo.RemoteAudioPort = g_CallList[i].AudioPort;	

								set_audio_pack_mode(head->Encrypt);
								g_CallInfo.state = state;
							}
							else									//中断其它呼叫
							{
								g_CallList[i].SendTimes = 0xFF;
								net_direct_send(CMD_CALL_HANDDOWN, (char*)&g_CallInfo.ID, 4, g_CallList[i].address, g_CallList[i].port);
							}
						}
					}
					break;
				case CALL_STATE_RECORDHINT:							//提示状态切换到留言或通话状态
					if (g_CallInfo.address == recPacket->address 
						&& (CALL_STATE_RECORDING == state || CALL_STATE_TALK == state))
					{	
						g_CallInfo.state = state;
					}
					break;
				case CALL_STATE_RECORDING: 							//留言状态切换到通话状态
					if (g_CallInfo.address == recPacket->address && CALL_STATE_TALK == state)
					{
						g_CallInfo.state = state;
					}
					break;
				default:
					break;
			}
			break;
		}
		
		case CMD_CALL_HANDDOWN:	
		{	
			log_printf("CMD_CALL_HANDDOWN : CallID : %d, g_CallInfo.ID : %d, g_BeCallInfo.ID : %d\n", CallID, g_CallInfo.ID, g_BeCallInfo.ID);

			if (CallID == g_CallInfo.ID)
			{
				if (g_CallInfo.state == CALL_STATE_CALLING)
				{
					if (g_CallListNum < 1 || g_CallList == NULL) 	//呼叫地址信息
					{
						break;
					}
					
					for (i = 0; i < g_CallListNum; i++)
					{
						if (g_CallList[i].address == recPacket->address)
						{
							g_CallList[i].EchoValue = TRC_TERMINATED;
							g_CallRefuseList[g_CallRefuseNum++] = g_CallList[i].address;
							break;
						}
					}
					break;
				} 
				if (g_CallInfo.state != CALL_STATE_NONE && g_CallInfo.state != CALL_STATE_END
				 	&& g_CallInfo.address == recPacket->address)
				{
					g_ErrType = END_BY_OTHER_SIDE;
					g_CallInfo.state = CALL_STATE_END;
				}
			}
			
			if (CallID == g_BeCallInfo.ID)
			{
				if (g_BeCallInfo.state != CALL_STATE_NONE && g_BeCallInfo.state != CALL_STATE_END
					&& g_BeCallInfo.address == recPacket->address)
				{
					g_ErrType = END_BY_OTHER_SIDE;
					g_BeCallInfo.state = CALL_STATE_END;
				}
			}		

			// modi by luofl 2011-09-09解决在通话中只显示一个新来电问题
			// 新的呼入挂机
			/*
			if (CallID == g_NewBeCallInfo.ID && 
				CallID != g_BeCallInfo.ID &&
				CallID != g_CallInfo.ID &&
				recPacket->address == g_NewBeCallInfo.address)
			*/

			// add by luofl 2012-02-20 修正被叫时室内机重启后对方发一条挂断命令,会增加一条空记录
			if (g_BeCallInfo.address || g_CallInfo.address)
			{
				if (recPacket->address != g_BeCallInfo.address && recPacket->address != g_CallInfo.address)	
				{
					// add by luofl 2011-09-20 修正未接来电中记录有时为空的问题
					char CallNo[30]; 
					memset(CallNo, 0, sizeof(CallNo));
					new_call_in(recPacket, CallNo);
					add_inter_record(MISSED, g_NewBeCallInfo.RemoteDeviceType, g_NewBeCallInfo.CallNo1);
					memset(&g_NewBeCallInfo, 0, sizeof(g_NewBeCallInfo));
					BeCallGuiNotify(CALL_NEW_CALLING, 0);
				}
			}
			break;
		}
		
		case CMD_CALL_HEART:		
			//log_printf("CMD_CALL_HEART : CallID: %d, g_CallInfo.ID:%d, g_BeCallInfoID:%d\n", CallID, g_CallInfo.ID, g_BeCallInfo.ID);
			//留言剩余时间由被叫方传送到呼叫方
			if (CallID == g_CallInfo.ID)
			{
				if (g_CallInfo.address == recPacket->address)
				{
					g_CallInfo.HeartTime = 0;
				}
				if (g_CallInfo.state == CALL_STATE_RECORDING && head->DataLen == 8)
				{
					uint32 times = *(uint32 *)(recPacket->data + NET_HEAD_SIZE + 4);
					g_RemainTime = times & 0xFFFF;
				}
			}
			//通话剩余时间由呼叫方传送到被叫方
			if (CallID == g_BeCallInfo.ID)
			{
				if (g_BeCallInfo.address == recPacket->address)
				{
					g_BeCallInfo.HeartTime = 0;
				}
				if (g_BeCallInfo.state == CALL_STATE_TALK && head->DataLen == 8)
				{
					uint32 times = 	*(uint32 *)(recPacket->data + NET_HEAD_SIZE + 4);
					g_RemainTime = times & 0xFFFF;
					//log_printf("NET HEART : g_RemainTime:%d\n", g_RemainTime);
				}
			}
			break;
			
		default:
			return FALSE;
	}

	return TRUE;
}

/*************************************************
  Function:			inter_call_responsion
  Description:		对讲呼叫类应答包命令处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void inter_call_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket)
{
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	int32 cmd = head->command | SSC_INTERPHONE << 8;

	if (recPacket == NULL)
	{
		return;
	}	

	switch (cmd)
	{
		case CMD_CALL_CALLING:	
		{
			if (g_CallInfo.state == CALL_STATE_CALLING)
			{
				int32 i;
				for (i = 0; i < g_CallListNum; i++)
				{ 
					if (recPacket->address == g_CallList[i].address)
					{
						char * data = recPacket->data + NET_HEAD_SIZE;

						//状态码2B＋参数（见状态码）+ 音视频参数
						memcpy(&g_CallList[i].EchoValue, data, 2);
						log_printf("call response : echo value : %d\n", g_CallList[i].EchoValue);
						switch (g_CallList[i].EchoValue)
						{
							case TRC_RINGING:
								if (head->DataLen >= 10)
								{		
									g_CallList[i].SendTimes = 0;
									
									//IP地址4B+音频端口＋视频端口
									memcpy(&g_CallList[i].MediaAddr, data+2, 4);
									if (g_CallList[i].MediaAddr==0)
									{
										g_CallList[i].MediaAddr = recPacket->address;
									}
									memcpy(&g_CallList[i].AudioPort, data+6, 2);
									memcpy(&g_CallList[i].VideoPort, data+8, 2);
									break;
								}
									
							case TRC_BUSY:
								g_CallList[i].SendTimes = 0;
								break;
							case TRC_TRYING:
								g_CallList[i].SendTimes = 0;
								break;
							case TRC_QUEUED:
								g_CallList[i].SendTimes = 0;
								break;
							case TRC_OK:
								g_CallList[i].SendTimes = 0;
								break;

							// 呼叫中心转接
							case TRC_MOVE_TEMP:	
								g_CallList[i].SendTimes = 0;
								switch (g_CallInfo.RemoteDeviceType)
								{
									case DEVICE_TYPE_MANAGER:
										if (g_MoveTempTimes < 3)
										{
											uint32 ipaddr = 0;
											uint16 port = 0;
											set_nethead(G_MANAGER_DEVNO, PRIRY_DEFAULT);
											net_direct_send(CMD_CALL_HANDDOWN, (char *)&g_CallInfo.ID, 4, g_CallList[i].address, g_CallList[i].port);
											
											memcpy(&ipaddr, data+2, 4);
											memcpy(&port, data+6, 2);
											g_CallList[i].address = ipaddr;
											g_CallList[i].port = port;
											g_CallList[i].EchoValue = TRC_UNKNOWN;
											g_MoveTempTimes++;
											log_printf("TRC_MOVE_TEMP : ipaddr: 0x%x, port: %d, movetemp times: %d\n", ipaddr, port, g_MoveTempTimes);
										}
										else
										{
											g_CallList[i].EchoValue = TRC_BUSY;
										}
										break;

									case DEVICE_TYPE_ROOM:
										ipmodule_call(i, data);
										break;
									
									default:
										g_CallList[i].EchoValue = TRC_BUSY;
										g_CallList[i].SendTimes = 0;
										break;
								}
								
								
								break;
							}
						return;
					}
				}
			}
			break;
		}	
		case CMD_CALL_HANDDOWN:		
			break;
		default:
			break;
		}
}
