/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_distribute.c
  Author:    	chenbh
  Version:   	2.0
  Date: 		2014-12-11
  Description:  网络数据分发模块
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_distribute.h"
#include <semaphore.h>

typedef struct
{
	unsigned char code;								// 子系统代号
	PFDISTRIBUTE distribute;						// 数据分发函数
	PFRESPONSION responsion;						// 应答处理
}DISTRIBUTE_FUNC;

// 各子系统数据接收分发回调函数
static DISTRIBUTE_FUNC g_distributefuncs[] = {
	{
		SSC_PUBLIC,									// 公共
		NULL,
		NULL
	},
	{
		SSC_INTERPHONE,								// 对讲
		NULL,
		NULL
	},
	{
		SSC_ALARM,									// 报警
		NULL,
		NULL
	},
	{
		SSC_INFO,									// 信息
		NULL,
		NULL
	},
	{
		SSC_EHOME,									// 家电
		NULL,
		NULL
	},
	{
		SSC_MULTIMEDIA,								// 多媒体
		NULL,
		NULL
	},
	{
		SSC_ELIVE,									// E生活
		NULL,
		NULL
	},
	{
		SSC_OTHER,									// 便利功能
		NULL,
		NULL
	},
	{
		SSC_IPDOORPHONE,								// 网关
		NULL,
		NULL
	},
	{
		SSC_VIDEOMONITORING,						// 流媒体
		NULL,
		NULL
	}	
};

static char g_SendBuf[NET_PACKBUF_SIZE];	  		// 发送Buf
#ifndef _AU_PROTOCOL_
static PMAIN_NET_HEAD g_Main_NetHead = (PMAIN_NET_HEAD)g_SendBuf;  // 发送网络主包头
#endif
static PNET_HEAD g_NetHead = (PNET_HEAD)(g_SendBuf+MAIN_NET_HEAD_SIZE);	// 发送网络包头
static uint16 g_CurPackID = 0;						// 发送包ID,首包为1
static PFULL_DEVICE_NO g_FullDeviceNo;				// 设备编号参数
static uint8 g_setyet = 0;                          // 标志位是否已经设置过
static int g_DistributeRunning = 0;					//分发模块线程运行状态
static pthread_t g_DistributeThread;				//分发模块线程

static sem_t g_SendBufSem;							// 发送信号量
static sem_t g_SendWaitSem;							// 接收信号量

/*******************************阻塞命令发送方式********************************************/
// 阻塞发送命令节点结构定义
typedef struct _WAIT_ECHO
{
	uint16 PackageID;								// 发送命令包ID
	uint32 address;									// 发送地址
	uint16 port;									// 发送端口
	uint8 EchoValue;								// 接收应答码
	char * ReciData;								// 接收附加数据
	int32 ReciSize;									// 接收附加数据大小
	struct _WAIT_ECHO * next;						// 下一个节点
}WAIT_ECHO, *PWAIT_ECHO;

static PWAIT_ECHO g_WaitFreeList = NULL;			// 阻塞发送命令空闲链表
static PWAIT_ECHO g_WaitEchoList = NULL;			// 阻塞发送命令链表
//static ROCK_SEM_DATA g_SendWaitSem;					

/*******************************多包接收处理过程********************************************/
typedef struct
{
	uint32 ID;										// 会话ID
	int32 size;										// 接收数据长度
	char * data;									// 接收数据
	char * Recflags;								// 接收标志
	uint16 total;									// 总包数
	uint16 num;										// 已经接收包数
	uint32 address;									// 对端地址
	uint16 port;									// 对端端口号
	PFMAX_RECIVE func; 								// 接收多包回调函数
	PFMAX_SEND_REC_STATE state;						// 接收状态回调函数
	uint32 t0;										// 开始时间, 计算超时用
	uint8 TimeOut;									// 超时时间, 单位秒
}MULT_RECIVE;

#define	REC_MAX_DATA_NUM			1				// 同时接收多包任务数
#define	MULT_SEND_PACKET_SIZE		1024			// 多包发送时,没一包大小
#define	MULT_SEND_MAX_PACKET		(1024*20)		// 最大包数
#define DEFAULT_MULT_RECIVE_TIMEOUT	60				// 默认多包接收超时时间	
static int32 g_CurReiveNum = 0;						// 当前开启的多包接收进程个数
static MULT_RECIVE g_MultRecList[REC_MAX_DATA_NUM];	// 接收多包控制结构链表

/*******************************多包发送处理过程********************************************/
typedef struct _MULT_SEND
{
	struct _MULT_SEND * next;			
	uint32 ID;										// 会话ID
	int32 size;										// 发送数量长度
	char * data;									// 发送数据
	uint32 address;									// 发送地址
	uint16 port;									// 发送端口号
	PFMAX_SEND_ECHO func; 							// 发送应答回调函数
	PFMAX_SEND_REC_STATE state;						// 发送状态回调函数
}MULTI_SEND, *PMULTI_SEND;

static PMULTI_SEND g_MultSendList = NULL;			// 多包发送队列
static PMULTI_SEND g_LastMultSend = NULL;			// 对尾巴
static PMULTI_SEND g_CurMultiSend = NULL;			// 当前发送包
static char * g_SendFlags;							// 发送应答标志
static uint16 g_SendNum = 0;						// 总发送包数
static uint16 g_SendOkNum = 0;						// 发送成功包数
static int32 g_SendSize = 0;						// 当前已经发送字节数
static int32 g_LastSendSize;						// 最后一包字节数

static uint32		DestAreaCode;					// 目标小区编号
static DEVICE_NO	DestDeviceNo;					// 目标设备编号
static uint16		DefaultPriry = PRIRY_DEFAULT;	// 包优先级

#ifndef _AU_PROTOCOL_
DEVICE_NO G_CENTER_DEVNO = {DEVICE_TYPE_CENTER, 0, 0, 0, 0};		// 中心服务器
DEVICE_NO G_MANAGER_DEVNO = {DEVICE_TYPE_MANAGER, 0, 0, 0, 100};	// 管理软件
DEVICE_NO G_BROADCAST_DEVNO = {0, 0, 0, 999999999, 999999999};		// 广播编号
DEVICE_NO G_MULTCMD_DEVNO = {0, 0, 0, 0, 0};						// 分包发送事件目标设备编号
#ifdef _SEND_EVENT_TO_STAIR_
DEVICE_NO G_STAIR_DEVNO = {DEVICE_TYPE_STAIR, 0, 0, 0, 0};			// 梯口机
#endif
#else
DEVICE_NO G_CENTER_DEVNO = {0, 0};									// 中心服务器
DEVICE_NO G_MANAGER_DEVNO = {0, 100};								// 管理软件
DEVICE_NO G_BROADCAST_DEVNO = {999999999, 999999999};				// 广播编号
DEVICE_NO G_MULTCMD_DEVNO = {0, 0};									// 分包发送事件目标设备编号
#endif

// 上报冠林平台事件命令列表
NET_COMMAND g_CmdListReportAurine[] = 
	{
		//一般事件
		CMD_RP_SET_EVENT, CMD_RP_UNSET_EVENT, CMD_RP_PARTSET_EVENT, CMD_RP_BYPASS_EVENT,
		//报警事件
		CMD_RP_FORCE_UNSET_ALARM, CMD_RP_SOS_ALARM, 
		CMD_RP_AREA_BREAK_ALARM, CMD_RP_AREA_FAULT_ALARM,
	}; 

// 上报中心事件命令列表
NET_COMMAND g_CmdListReportServer[] = 
	{
		//一般事件
		CMD_RP_SET_EVENT, CMD_RP_UNSET_EVENT, CMD_RP_PARTSET_EVENT, CMD_RP_BYPASS_EVENT,
		//报警事件
		CMD_RP_FORCE_UNSET_ALARM, CMD_RP_SOS_ALARM, 
		CMD_RP_AREA_BREAK_ALARM, CMD_RP_AREA_FAULT_ALARM,
	}; 
	
// 上报管理员机事件命令列表
NET_COMMAND g_CmdListReportManager[] = 
	{
		//一般事件
		CMD_RP_SET_EVENT, CMD_RP_UNSET_EVENT, CMD_RP_PARTSET_EVENT, CMD_RP_BYPASS_EVENT,
		//报警事件
	    CMD_RP_FORCE_UNSET_ALARM, CMD_RP_SOS_ALARM,
    	CMD_RP_AREA_BREAK_ALARM, CMD_RP_AREA_FAULT_ALARM,CMD_RP_AREA_DEFINE,CMD_RP_ALARM_STATE,
	}; 
	
//static void mult_send_proc(void);
static void recive_multpacket(NET_COMMAND cmd, const PRECIVE_PACKET RecivePacket);

/*************************************************
  Function:    		set_nethead
  Description: 		设置目标设备的区号和设备号, 和优先级
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_nethead(DEVICE_NO DeviceNo, uint16 Priry)
{
	#ifndef _AU_PROTOCOL_
	if (DeviceNo.AreaNo == 0)
	{
		PFULL_DEVICE_NO fulldevno;
		fulldevno = storage_get_devparam();
		if (fulldevno->AreaNo == 0)
		{
			DestAreaCode = 1;
			DeviceNo.AreaNo = 1;
		}
		else
		{
			DestAreaCode = fulldevno->AreaNo;
			DeviceNo.AreaNo = fulldevno->AreaNo;
		}
	}
	else
	{
		DestAreaCode = DeviceNo.AreaNo;
	}
	//printf("set_nethead====== %d\n",DestAreaCode);
	DestDeviceNo = DeviceNo;
	DefaultPriry = Priry;
	#endif
}

/*************************************************
  Function:    		sendbuf_lock
  Description: 		发送缓冲区上锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendbuf_lock(void)
{
	sem_wait(&g_SendBufSem);
}

/*************************************************
  Function:    		sendlbuf_unlock
  Description: 		发送缓冲区解锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendbuf_unlock(void)
{
	sem_post(&g_SendBufSem);
}

/*************************************************
  Function:    		sendbuf_sem_init
  Description: 		信号量初始化
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendbuf_sem_init(void)
{
	sem_init(&g_SendBufSem, 0, 1);   					
}

/*************************************************
  Function:    		sendbuf_sem_del
  Description: 		信号量删除
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendbuf_sem_del(void)
{
	sem_destroy(&g_SendBufSem);   					
}

/*************************************************
  Function:    		sendbuf_lock
  Description: 		发送缓冲区上锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendwait_lock(void)
{
	sem_wait(&g_SendWaitSem);
}

/*************************************************
  Function:    		sendlbuf_unlock
  Description: 		发送缓冲区解锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendwait_unlock(void)
{
	sem_post(&g_SendWaitSem);
}

/*************************************************
  Function:    		sendbuf_sem_init
  Description: 		信号量初始化
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendwait_sem_init(void)
{
	sem_init(&g_SendWaitSem, 0, 1);   					
}

/*************************************************
  Function:    		sendwait_sem_del
  Description: 		信号量删除
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendwait_sem_del(void)
{
	sem_destroy(&g_SendWaitSem);   					
}

/*************************************************
  Function:    		wait_echo_list_init
  Description: 		初始化链表,分配空间
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void wait_echo_list_init(void)
{
	uint32 i = 0;
	PWAIT_ECHO WaitEchoNode = NULL;
	if (g_WaitFreeList == NULL)
	{
		for (i = 0; i < PACKET_NODE_MAX_NUM; i++)
		{
			// 分配一个节点
			WaitEchoNode = (PWAIT_ECHO)malloc(sizeof(WAIT_ECHO));
			if (WaitEchoNode == NULL)
			{
				continue;
			}
			else
			{
				memset(WaitEchoNode, 0, sizeof(WAIT_ECHO));
				WaitEchoNode->ReciData = (char*)malloc(PACKET_NODE_DATA_SIZE);
				if (WaitEchoNode->ReciData == NULL)
				{
					free(WaitEchoNode);
					WaitEchoNode = NULL;
					continue;
				}
			}

			// 把节点加到链表
			if (g_WaitFreeList == NULL)
			{
				g_WaitFreeList = WaitEchoNode;
				g_WaitFreeList->next = NULL;
			}
			else
			{
				WaitEchoNode->next = g_WaitFreeList;
				g_WaitFreeList = WaitEchoNode;
			}
		}
	}
}

/*************************************************
  Function:    		get_waitnode_from_freelist
  Description: 		从空闲链表中获取节点
  Input: 			无
  Output:			无
  Return:			节点指针
  Others:
*************************************************/
static PWAIT_ECHO get_waitnode_from_freelist(void)
{
	PWAIT_ECHO tmp = NULL;
	if (g_WaitFreeList == NULL)
	{
		log_printf("free list is NULL\n");
		return NULL;
	}

	sendwait_lock();
	if (g_WaitFreeList->next == NULL)
	{
		tmp = g_WaitFreeList;
		g_WaitFreeList = NULL;
	}
	else
	{
		tmp = g_WaitFreeList;
		g_WaitFreeList = tmp->next;		
	}
	sendwait_unlock();
	return tmp;
}

/*************************************************
  Function:    		add_waitnode_to_freelist
  Description: 		把节点加入空闲链表
  Input: 			
  	1.node			节点指针
  Output:			无
  Return:			无
  Others:
*************************************************/
static void add_waitnode_to_freelist(PWAIT_ECHO node)
{
	if (node == NULL)
	{
		return;
	}

	sendwait_lock();
	if (g_WaitFreeList == NULL)
	{
		g_WaitFreeList = node;
		g_WaitFreeList->next = NULL;
	}
	else
	{
		node->next = g_WaitFreeList;
		g_WaitFreeList = node;
	}
	sendwait_unlock();
}

/*************************************************
  Function:    		get_package_ID
  Description:		获得发送包ID
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static uint16 get_package_ID(void)
{
	if (g_CurPackID==0xFFFF || g_CurPackID==0)
	{
		g_CurPackID = 1;
	}	
	else
	{
		g_CurPackID++;
	}
	return g_CurPackID;	
}

/*************************************************
  Function:    		encrypt_data
  Description:		发送时加密数据
  Input: 
	1.data			数据部分
	2.size			数据大小
  Output:			无
  Return:			无
  Others:
*************************************************/
static void encrypt_data(char * data, int32 size)
{
	// 无加密
	g_NetHead->Encrypt = 1;							// 加密类型 用作音频组包标志位
	g_NetHead->DataLen = size;						// 数据长度
	memset(g_SendBuf+NET_HEAD_SIZE, 0, sizeof(g_SendBuf)-NET_HEAD_SIZE);
	memcpy(g_SendBuf+NET_HEAD_SIZE, data, size);	// 拷贝附加数据
	g_NetHead->DataLen = size;						// 附加数据长度

	#ifndef _AU_PROTOCOL_
	g_NetHead->DestDeviceNo = DestDeviceNo;			//设备编号
	g_NetHead->Priry = DefaultPriry;
	g_Main_NetHead->subPacketLen = sizeof(NET_HEAD) + g_NetHead->DataLen;
	#endif
}

/*************************************************
  Function:    		direct_send_package
  Description:		网络直接发送，不加到链表中
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
  Output:			无
  Return:			包ID 
  Others:
*************************************************/
static int direct_send_package(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port)
{
	int32 ret = 0;
	uint16 PackageID = 0;

	sendbuf_lock();
	PackageID = get_package_ID();
	log_printf(" PackageID  : 0X%x \n", PackageID);
	#ifndef _AU_PROTOCOL_
	// 主协议包头
	g_Main_NetHead->mainSeq = PackageID;
	g_Main_NetHead->mainPacketType = 0x0000;		// 普通包
	#endif
	
	g_NetHead->PackageID = PackageID;				// 包标识
	g_NetHead->DirectFlag = DIR_SEND;				// 方向标志（主动：0xAA；应答：0x00）
	g_NetHead->SubSysCode = (uint8)(cmd>>8);		// 子系统代号
	g_NetHead->command = (uint8)(cmd & 0xFF);		// 命令值
	g_NetHead->EchoValue = 0;						// 应答码
	encrypt_data(data, size);
	ret = net_send_cmd_packet(g_SendBuf, g_NetHead->DataLen+NET_HEAD_SIZE, address, port);
	
	sendbuf_unlock();
	if (ret == FALSE)
	{
		return 0;
	}	
	return PackageID;
}

/*************************************************
  Function:    		net_direct_send
  Description:		网络直接发送，不加到链表中
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
  Output:			无
  Return:			包ID 
  Others:
*************************************************/
uint16 net_direct_send(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port)
{
	//DECLARE_CUP_SR;
	uint16 PackageID = 0;
	
	//ENTER_CRITICAL(); 关闭中断
	log_printf("address = %x\n",address);
	PackageID = direct_send_package(cmd, data, size, address, port);
	//EXIT_CRITICAL(); 打开中断
	return PackageID;
}

/*************************************************
  Function:    		net_send_by_list
  Description:		采用链表发送网络命令包,不阻塞
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.SendTimes		发送次数
	7.TimeOut		单次发送超时时间
  Output:			无
  Return:			包ID
  Others:
*************************************************/
uint16 net_send_by_list(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port, 
				int32 SendTimes, int32 TimeOut)
{
	uint16 PackageID = 0;

	sendbuf_lock();
	PackageID = get_package_ID();
	#ifndef _AU_PROTOCOL_
	// 主协议包头
	g_Main_NetHead->mainSeq = PackageID;
	g_Main_NetHead->mainPacketType = 0x0000;		// 普通包
	#endif
	
	g_NetHead->PackageID = PackageID;				// 包标识
	g_NetHead->DirectFlag = DIR_SEND;				// 方向标志（主动：0xAA；应答：0x00）
	g_NetHead->SubSysCode = (uint8)(cmd>>8);		// 子系统代号
	g_NetHead->command = (uint8)(cmd & 0xFF);		// 命令值
	g_NetHead->EchoValue = 0;						// 应答码
	encrypt_data(data, size);

	if (SendTimes < 1)
	{
		SendTimes = 1;	
	}
	if (TimeOut < 1)
	{
		TimeOut = PACKET_TIMEOUT_VALUE;
	}
	
	net_add_send_packet(g_SendBuf, g_NetHead->DataLen+NET_HEAD_SIZE, address, port, SendTimes, TimeOut);
	
	sendbuf_unlock();
	return PackageID;
}

/*************************************************
  Function:    		add_wait_echo_list
  Description:		将节点加入等待应答列表
  Input: 
	1.node			加入节点
  Output:			无
  Return:			无
  Others:
*************************************************/
static void add_wait_echo_list(PWAIT_ECHO node)
{
	sendwait_lock();
	if (NULL == g_WaitEchoList)
	{
		node->next = NULL;
	}	
	else
	{
		node->next = g_WaitEchoList;
	}	
	g_WaitEchoList = node;
	sendwait_unlock();
}

/*************************************************
  Function:    		remove_wait_echo_list
  Description:		将节点从等待应答列表取下
  Input: 
	1.node			加入节点
  Output:			无
  Return:			无
  Others:
*************************************************/
static void remove_wait_echo_list(PWAIT_ECHO node)
{
	PWAIT_ECHO prev = NULL;
	PWAIT_ECHO temp = g_WaitEchoList;

	sendwait_lock();
	while(temp != NULL)
	{
		if (temp == node)
		{
			if (NULL == prev)
			{
				g_WaitEchoList = temp->next;
			}	
			else
			{
				prev->next = temp->next;
			}	
			break;	
		}
		else
		{
			prev = temp;
			temp = temp->next;
		}	
	}
	sendwait_unlock();
}

/*************************************************
  Function:    		check_is_wait_echo
  Description:		检查是否是等待应答链表中的应答包
  Input: 
	1.RecPacket		网络接收包
  Output:			无
  Return:			TRUE / FALSE
  Others:
*************************************************/
static int32 check_is_wait_echo(const PRECIVE_PACKET RecPacket)
{
	PNET_HEAD NetHead = (PNET_HEAD)(RecPacket->data + MAIN_NET_HEAD_SIZE);
	PWAIT_ECHO temp = g_WaitEchoList;
	sendwait_lock();
	sendwait_unlock();
	if (temp == NULL)
	{
		log_printf("g_WaitEchoList is NULL\n");
	}
	while(temp)
	{
		if (NetHead->PackageID==temp->PackageID &&
			(RecPacket->address==temp->address || (temp->address&0xFF)==0xFF) &&
			RecPacket->port==temp->port)
		{			
			if (temp->ReciData != NULL)
			{	
				temp->ReciSize = NetHead->DataLen;
				//拷贝数据
				memcpy(temp->ReciData, RecPacket->data+NET_HEAD_SIZE, NetHead->DataLen);
				temp->EchoValue = NetHead->EchoValue;
				return TRUE; 
			}
			break;
		}
		temp = temp->next;
	}
	return FALSE;
}

/*************************************************
  Function:    		net_send_command
  Description:		网络命令阻塞发送，该命令在主线程中等待应答
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.TimeOut		发送超时时间
  Output:			
	1.EchoValue		应答码
	2.ReciData		接收应答数据, 注意:需外部分配空间
	3.ReciSize		接收应答数据大小
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int32 net_send_command(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port, int32 TimeOut,
				 uint8 * EchoValue, char * ReciData, int32 * ReciSize)
{	
	int32 ret = FALSE;
	int32 DelayCount = 0;
	
	PWAIT_ECHO node = get_waitnode_from_freelist();
	if (NULL == node)
	{
		log_printf("get node err\n");
		return FALSE;
	}

	add_wait_echo_list(node);
	node->address = address;						// 发送地址
	node->port = port;								// 发送端口			
	node->EchoValue = 0xFF;							// 置未应答
	sendwait_lock();
	node->PackageID = direct_send_package(cmd, data, size, address, port);
	if (0 == node->PackageID)						// 发送不成功返回
	{
		sendwait_unlock();
		remove_wait_echo_list(node);
		add_waitnode_to_freelist(node);
		return FALSE;
	}
	sendwait_unlock();

	if (TimeOut < 1)
	{
		TimeOut = 1;
	}	

	while(1)
	{
		// 有应答, 退出
		if (node->EchoValue != 0xFF)
		{
			break;
		}	
		
		// 判断时间
		DelayCount++;
		if ((DelayCount*50) >= (TimeOut*1000))		// 应答超时
		{
			break;
		}	
		else
		{
			DelayMs_nops(50);
		}	
	}
	
	if (node->EchoValue != 0xFF)
	{
		if (EchoValue)
		{
			*EchoValue = node->EchoValue;			// 接收应答码
		}	
		if (ReciData)
		{
			// 接收附加数据
			memcpy(ReciData, node->ReciData, node->ReciSize);
		}	 
		if (ReciSize)	
		{
			*ReciSize =	node->ReciSize;				// 接收附加数据大小
		}	
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	remove_wait_echo_list(node);
	add_waitnode_to_freelist(node);
	return ret;
}

/*************************************************
  Function:    		get_net_comm_type
  Description:		获取主发包类型
  Input: 
	1.data			数据部分
	2.size			数据大小
  Output:			无
  Return:			无
  Others:
*************************************************/
NET_COMM_TYPE_E get_net_comm_type(uint8 SubSysCode)
{
	NET_COMM_TYPE_E ret = NET_COMM_TYPE_NONE;
	if(SubSysCode == SSC_INTERPHONE)
	{
		ret = NET_COMM_TYPE_CALL;
	}
	else if(SubSysCode == SSC_ALARM)
	{
		ret = NET_COMM_TYPE_EVENT;
	}
	else
	{
		ret = NET_COMM_TYPE_CONTROL;
	}
	return ret;
}

/*************************************************
  Function:    		net_fill_main_nethead
  Description:		主协议包填充
  Input: 
	1.recMainNetHead		接收包主包头
  Return:			无
  Others:
*************************************************/
void net_fill_main_nethead(void)
{
	#ifndef _AU_PROTOCOL_
	uint32 hwEncrypt = _HW_ENCRYPT_;
	memset(g_Main_NetHead->hwEncrypt, 0, sizeof(g_Main_NetHead->hwEncrypt));
	memcpy(&g_Main_NetHead->hwEncrypt[12], &hwEncrypt, 4);
	
	g_Main_NetHead->subProtocolType = SUBPROTOCALTYPE;
	g_Main_NetHead->DirectFlag = DIR_DONOTHING;
	g_Main_NetHead->mainProtocolType = MAINPROTOCOL_VER;
	g_Main_NetHead->sysChipType = SYYCHIPTYPE;
	g_Main_NetHead->subProtocolPackType = SUBPROTOCALPACKTYPE;
	g_Main_NetHead->subProtocolEncrypt = _SUB_PROTOCOL_ENCRYPT_; // 不用加密
	g_Main_NetHead->EchoValue = SUBPROTOCALECHO;
	#endif
}


/*************************************************
  Function:    		net_send_echo_packet
  Description:		发送应答包
  Input: 
	1.recPacket		接收包
	2.echoValue		应答码
	3.data			附加数据
	4.size			附加数据大小
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_send_echo_packet(PRECIVE_PACKET recPacket, ECHO_VALUE_E echoValue, char * data, int32 size)
{
	PNET_HEAD recNetHead;
	PMAIN_NET_HEAD recMainNetHead;
	
	if (NULL == recPacket)
	{
		return;
	}
	recNetHead = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	recMainNetHead = (PMAIN_NET_HEAD)(recPacket->data);
	sendbuf_lock();
	g_NetHead->PackageID = recNetHead->PackageID;	// 包标识
	g_NetHead->DirectFlag = DIR_ACK;				// 方向标志（主动：0xAA；应答：0x00）
	g_NetHead->SubSysCode = recNetHead->SubSysCode;	// 子系统代号
	g_NetHead->command = recNetHead->command;		// 命令值
	g_NetHead->EchoValue = echoValue;				// 应答码	
	encrypt_data(data, size);
	
	#ifndef _AU_PROTOCOL_
	// 网络主包头填充
	g_Main_NetHead->mainSeq = recMainNetHead->mainSeq;
	g_Main_NetHead->mainPacketType = recMainNetHead->mainPacketType;
	g_NetHead->DestDeviceNo = recNetHead->SrcDeviceNo;
	g_NetHead->Priry = recNetHead->Priry;
	#endif
	
	net_send_cmd_packet(g_SendBuf, g_NetHead->DataLen+NET_HEAD_SIZE, recPacket->address, recPacket->port);
	
	sendbuf_unlock();
}

/*************************************************
  Function:    		check_cmd_inlist
  Description:		检查命令是否在列表中
  Input: 
	1.cmd			命令
	2.list			列表
	3.n				列表大小
  Output:			无
  Return:			在与否, TRUE/FALSE
  Others:
*************************************************/
static int32 check_cmd_inlist(NET_COMMAND cmd, NET_COMMAND list[], int32 n)
{
	int32 i;
	
	for ( i= 0; i < n; i++)
	{
		if (list[i] == cmd)
		{
			return TRUE;
		}	
	}
	return FALSE;
}

/*************************************************
  Function:    		net_send_event
  Description:		上报事件，含报警事件
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.SendTo		发送方, 冠林平台/中心服务器/管理员机 
  Output:			
  Return:			是否可发送, TRUE/FALSE， FALSE－该命令为非定义的事件命令或数据长度参数错误
  Others:
*************************************************/
int32 net_send_event(NET_COMMAND cmd, char * data, int32 size, EVENT_SENDTO_E SendTo)
{
	uint32 ipaddr = 0;
	
	//向梯口云端接入器发送报警事件
	#ifdef _SEND_EVENT_TO_STAIR_	
	if(cmd == CMD_RP_FORCE_UNSET_ALARM || cmd == CMD_RP_SOS_ALARM || cmd == CMD_RP_AREA_BREAK_ALARM )
	{
		set_nethead(G_STAIR_DEVNO, PRIRY_DEFAULT);
		ipaddr = net_get_aurine_stun_ip();
		if (ipaddr)
		{
			net_send_by_list(cmd, data, size, ipaddr, NETCMD_UDP_PORT, 3, 3);
		}
	}
	#endif
	
	if (g_Event_Server.ip != 0)
	{
		set_nethead(G_MANAGER_DEVNO, PRIRY_DEFAULT);
		net_send_by_list(cmd, data, size, g_Event_Server.ip, g_Event_Server.port, 3, 3);
		return ;
	}
	
	// 判断命令是否在列表中
	switch (SendTo)
	{
		case EST_AURINE:
			if (!check_cmd_inlist(cmd, g_CmdListReportAurine, sizeof(g_CmdListReportAurine)/sizeof(NET_COMMAND)))
			{
				return FALSE;
			}
			ipaddr = net_get_aurine_ip();
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			break;	
			
		case EST_SERVER:
			if (!check_cmd_inlist(cmd, g_CmdListReportServer, sizeof(g_CmdListReportServer)/sizeof(NET_COMMAND)))
			{
				return FALSE;
			}
			ipaddr = net_get_center_ip();
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			break;	
			
		case EST_MANAGER:
			if (!check_cmd_inlist(cmd, g_CmdListReportManager, sizeof(g_CmdListReportManager)/sizeof(NET_COMMAND)))
			{
				return FALSE;
			}
			set_nethead(G_MANAGER_DEVNO, PRIRY_DEFAULT);
			ipaddr = net_get_manager_ip(1);
			if (ipaddr)
			{
				net_send_by_list(cmd, data, size, ipaddr, NETCMD_UDP_PORT, 3, 3);
			}
			ipaddr = net_get_manager_ip(2);
			if (ipaddr)
			{
				net_send_by_list(cmd, data, size, ipaddr, NETCMD_UDP_PORT, 3, 3);
			}
			ipaddr = net_get_manager_ip(3);
			if (ipaddr)
			{
				net_send_by_list(cmd, data, size, ipaddr, NETCMD_UDP_PORT, 3, 3);
			}
			return TRUE;
			
		default:
			return FALSE;	
	}
	
	log_printf("ipaddr = %x\n",ipaddr);
	return net_send_by_list(cmd, data, size, ipaddr, NETCMD_UDP_PORT, 3, 3);
	
}

/*************************************************
  Function:    		get_distribute_func
  Description:		获得指定子系统接收发送包处理回调函数
  Input: 
	1.subSysCode	子系统代号
  Output:			无
  Return:			子系统接收包分发函数
  Others:
*************************************************/
static PFDISTRIBUTE get_distribute_func(unsigned char subSysCode)
{
	int32 i;
	for(i=0; i<(sizeof(g_distributefuncs)/sizeof(DISTRIBUTE_FUNC)); i++)
	{
		if (subSysCode == g_distributefuncs[i].code)
		{
			return g_distributefuncs[i].distribute;
		}
	}
	return NULL;
}

/*************************************************
  Function:    		get_distribute_func
  Description:		获得指定子系统接收应答包处理回调函数
  Input: 
	1.subSysCode	子系统代号
  Output:			无
  Return:			子系统应答包分发函数
  Others:
*************************************************/
static PFRESPONSION get_responsion_func(unsigned char subSysCode)
{
	int32 i;
	for(i=0; i<(sizeof(g_distributefuncs)/sizeof(DISTRIBUTE_FUNC)); i++)
	{
		if (subSysCode == g_distributefuncs[i].code)
		{
			return g_distributefuncs[i].responsion;
		}
	}
	return NULL;	
}

/*************************************************
  Function:    		send_timeout_echo
  Description:		应答超时通知
  Input: 
	1.SendPacket	发送包
  Output:无
  Return:无
  Others:
*************************************************/
static void send_timeout_echo_func(PSEND_PACKET SendPacket)
{
	PNET_HEAD netHead = (PNET_HEAD)(SendPacket->data + MAIN_NET_HEAD_SIZE);
	PFRESPONSION responsion_func = get_responsion_func(netHead->SubSysCode);
	
	if (NULL != responsion_func)
	{
		responsion_func(NULL, SendPacket);
	}	
}

/*************************************************
  Function:    		net_set_recivedata_func
  Description:		设置函数回调函数
  Input: 
	1.subSysCode	子系统代号
	2.distribute	接收发送命令应答处理函数
	3.responsion	接收应答命令应答处理函数
  Output:			无
  Return:			无
  Others:
*************************************************/
int32 net_set_recivedata_func(SUB_SYS_CODE_E SubSysCode, PFDISTRIBUTE distribute, PFRESPONSION responsion)
{
	int32 i; 
	for(i=0; i<(sizeof(g_distributefuncs)/sizeof(DISTRIBUTE_FUNC)); i++)
	{
		if (SubSysCode == g_distributefuncs[i].code)
		{
			g_distributefuncs[i].distribute = distribute;
			g_distributefuncs[i].responsion = responsion;
			return TRUE;
		}
	}
	return FALSE; 
}

/*************************************************
  Function:    		recive_dns_packet
  Description:		室内机广播DNS呼叫时用到
  Input: 			
  	1.RecivePacket	接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recive_dns_packet(NET_COMMAND cmd, const PRECIVE_PACKET RecivePacket)
{
	uint32 address = 0; 
	uint8 DeviceType = 0;
	char devno[20];
	int32 pos = 0;
	int32 i = 0;
	char data[16];
	PNET_HEAD head = (PNET_HEAD)(RecivePacket->data + MAIN_NET_HEAD_SIZE);

	#ifdef _ETHERNET_DHCP_
	get_local_ipaddress("eth0", &address);
	#else
	address = net_get_local_ipaddress();
	#endif	
	
	log_printf("remote address:0x%x, local address:0x%x\n", RecivePacket->address, address);
	if (RecivePacket->address == address)
	{
		return;
	}	

	if (DIR_SEND == head->DirectFlag)
	{
		DeviceType = RecivePacket->data[NET_HEAD_SIZE];
		log_printf("remote devtype:0x%x, local devtype:0x%x\n", DeviceType, g_FullDeviceNo->DeviceType);
		if (DeviceType != g_FullDeviceNo->DeviceType)
		{
			return;
		}
		memset(devno, 0, sizeof(devno));
		strncpy(devno, RecivePacket->data+NET_HEAD_SIZE+1, head->DataLen-1);

		// 替换通配符
		devno[g_FullDeviceNo->DevNoLen] = 0;
		if ('_'==devno[g_FullDeviceNo->DevNoLen-1] || '*'==devno[g_FullDeviceNo->DevNoLen-1])
		{
			devno[g_FullDeviceNo->DevNoLen-1] = g_FullDeviceNo->DeviceNoStr[g_FullDeviceNo->DevNoLen-1];
		}	
		pos = g_FullDeviceNo->Rule.StairNoLen - g_FullDeviceNo->Rule.CellNoLen;
		
		for (i=0; i<g_FullDeviceNo->Rule.CellNoLen; i++)
		{
			if (devno[pos+i] == '_')
			{
				devno[pos+i] = g_FullDeviceNo->DeviceNoStr[pos+i];
			}	
		}
		log_printf("request devstr:%s, local devstr:%s\n", devno, g_FullDeviceNo->DeviceNoStr);
		if (0 == strcmp(g_FullDeviceNo->DeviceNoStr, devno))
		{
			log_printf("remote address:0x%x, local address:0x%x\n", RecivePacket->address, address);
			memset(data, 0, sizeof(data));
			memcpy(data, &g_FullDeviceNo->DeviceNo, sizeof(DEVICE_NO));
			memcpy(data+sizeof(DEVICE_NO), (char *)(&address), 4);
			log_printf("send data len : sizeof(data):%d\n  ", sizeof(data));
			log_printf("send data : %s \n", data);
			//log_printf("send data DeviceType: %d   DeviceNo2: %d  addres:%0x \n ", g_FullDeviceNo->DeviceNo.DeviceType,g_FullDeviceNo->DeviceNo.DeviceNo2,address);
			// 应答数据:设备编号长度+4B地址
			net_send_echo_packet(RecivePacket, ECHO_OK, data, sizeof(DEVICE_NO)+4);
		}
	}
	else
	{
		//检查是否在等待应答队列中
		if (FALSE == check_is_wait_echo(RecivePacket))
		{
			net_recive_dns_echo(RecivePacket->address, RecivePacket->data+NET_HEAD_SIZE, head->DataLen);
		}
	}
}


/*************************************************
  Function:    		distribute_proc
  Description:		数据分发线程处理过程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void* distribute_proc(void* arg)
{
	PRECIVE_PACKET recPacket;
	PSEND_PACKET sendPacket;
	PFRESPONSION ResponsionFunc;
	PFDISTRIBUTE DistributeFunc;
	PNET_HEAD netHead;
	NET_COMMAND cmd;
	int32 ret = FALSE;

    while (g_DistributeRunning)
    {
		recPacket = net_get_recive_packet();
		if (NULL == recPacket)
		{
			usleep(1000);
			continue;
		}
		
		netHead = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
		log_printf("have packet for distribute! DirectFlag : %x \n ", netHead->DirectFlag);
	// DNS及多包接收分发
	#if 0					
		cmd = net_get_cmd_by_nethead(netHead);
		switch(cmd)
		{
			case CMD_SEND_MAXDATA:					// 多包发送
			case CMD_STOP_SEND_MAXDATA: 			// 中断多包发送
				recive_multpacket(cmd, recPacket);
				continue;
			case CMD_DNS_GETIP:						// DNS广播包
				recive_dns_packet(cmd, recPacket);
				continue;
				break;
			default:
				break;
		}
	#endif
	
		// 上位机主动发送的发送包	
		if (DIR_SEND == netHead->DirectFlag)	
		{
			DistributeFunc = get_distribute_func(netHead->SubSysCode);
			
			if (NULL != DistributeFunc)
			{
				ret = DistributeFunc(recPacket);
			}
			
			// 非法命令,或没有回调函数统一应答错误码	
			if (ret == FALSE)
			{
				net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
			}	
		}
		// 上位机应答包
		else if (DIR_ACK == netHead->DirectFlag)	
		{
			cmd = net_get_cmd_by_nethead(netHead);

			// 判断是否是等待的应答
			if (check_is_wait_echo(recPacket))
			{
				net_del_recive_packet(recPacket);
				continue;
			}
			
			// 判断是否上报事件应答
			/*
			if (check_is_report_event_echo(cmd, netHead->PackageID))
			{
				free_recive_node(recPacket);
				continue;
			}
			*/

			// 数据应答包分发
			sendPacket = net_get_sendpacket_by_responsion(netHead);
			ResponsionFunc = get_responsion_func(netHead->SubSysCode);
			if (NULL != ResponsionFunc)
			{
				ResponsionFunc(recPacket, sendPacket);
			}	
			if (NULL != sendPacket)
			{
				net_del_send_packet(sendPacket);
				sendPacket = NULL;
			}	
		}
		net_del_recive_packet(recPacket);
	}
	
  	pthread_exit(NULL);	
}

/*************************************************
  Function:    		start_distribute_thread
  Description:		启动数据分发线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void start_distribute_thread(void)
{
    if (!g_DistributeRunning)
    {
        g_DistributeRunning = 1;
        pthread_create(&g_DistributeThread, NULL, distribute_proc, NULL);
    }
}

/*************************************************
  Function:    		stop_distribute_thread
  Description:		停止数据分发线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_distribute_thread(void)
{   
	if (g_DistributeRunning)
    {
        g_DistributeRunning = 0 ;
        pthread_join(g_DistributeThread, NULL);
    }
    g_DistributeThread = -1;
}

/*************************************************
  Function:    		net_start_comm
  Description:		启动通讯模块
  Input: 
	1.FullDeviceNo	设备编号
	2.NetParam		传入网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_start_comm(PFULL_DEVICE_NO FullDeviceNo, PNET_PARAM NetParam)
{
	net_packet_list_init();
	sendbuf_sem_init();
	sendwait_sem_init();
	wait_echo_list_init();
	net_set_net_param(NetParam);							// 设置网络参数
	g_FullDeviceNo = FullDeviceNo;							// 设置设备号
	g_NetHead->ProtocolVer = _PROTOCOL_VER_;				// 协议版本
	g_NetHead->DeviceType = g_FullDeviceNo->DeviceType;		// 设备类型

	#ifndef _AU_PROTOCOL_
	g_NetHead->Priry = 0xFF;                                // 协议优先级，暂时定为默认
	g_NetHead->SrcDeviceNo = g_FullDeviceNo->DeviceNo;		// 设备编号
	net_fill_main_nethead();                                // 主协议包填充
	#else
	g_NetHead->DeviceNo = g_FullDeviceNo->DeviceNo;			// 设备编号
	g_NetHead->AreaCode = g_FullDeviceNo->AreaNo;			// 小区编号
	#endif
	net_set_send_timeout_echofunc(send_timeout_echo_func);	// 发送超时应答处理函数
	net_set_dns_packet_recive(recive_dns_packet);			// 设置DNS包接收接口
	net_set_dns_areacode(g_FullDeviceNo->AreaNo);			// 设置DNS小区号
	net_start_udp_comm();									// 启动UDP通讯
	start_distribute_thread();								// 启动数据分发线程
}

/*************************************************
  Function:    		net_stop_comm
  Description:		停止通讯模块
  Input: 			无		
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_comm(void)
{
	net_stop_udp_comm();									// 停止UDP通讯
	stop_distribute_thread();								// 停止数据分发线程
	sendbuf_sem_del();
	sendwait_sem_del();
}

/*************************************************
  Function:    		net_get_comm_device_type
  Description:		获得网络模块设备类型
  Input: 			无
  Output:			无
  Return:			设备类型
  Others:
*************************************************/
DEVICE_TYPE_E net_get_comm_device_type(void)
{
	return g_NetHead->DeviceType;
} 

/*************************************************
  Function:    		net_change_comm_deviceno
  Description:		更新网络模块设备号信息，含小区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_change_comm_deviceno(void)
{
	#ifdef _AU_PROTOCOL_
	g_NetHead->AreaCode = g_FullDeviceNo->AreaNo;	// 小区编号
	g_NetHead->DeviceNo = g_FullDeviceNo->DeviceNo;	// 设备编号
	#else
	g_FullDeviceNo->DeviceNo.AreaNo = g_FullDeviceNo->AreaNo;
	g_FullDeviceNo->DeviceNo.DeviceType = g_FullDeviceNo->DeviceType;
	
	g_NetHead->SrcDeviceNo.AreaNo = g_FullDeviceNo->AreaNo;		    	// 小区编号
	g_NetHead->SrcDeviceNo.DeviceType = g_FullDeviceNo->DeviceType;  	// 设备类型
	g_NetHead->SrcDeviceNo = g_FullDeviceNo->DeviceNo;		// 设备编号
	g_NetHead->DeviceType = g_FullDeviceNo->DeviceType;  	// 设备类型
	#endif
	net_set_dns_areacode(g_FullDeviceNo->AreaNo);
	
	// 重新注册 设备编号更改后 需要重新注册
	#ifdef _USE_NEW_CENTER_
	logic_reg_center_ini_again();
	#endif
}

/*************************************************
  Function:    		free_recive_multipacket
  Description:		释放多包接收包
  Input: 
	1.index			数组中的位置
  Output:			无
  Return:			无
  Others:
*************************************************/
static void free_recive_multipacket(int32 index)
{
	if ((index>=REC_MAX_DATA_NUM)||(g_MultRecList[index].ID==0))
	{
		return;
	}
	
	// 首先置ID为0	
	g_MultRecList[index].ID = 0;
	//g_MultRecList[index].data = NULL;
	//g_MultRecList[index].Recflags = NULL;
	g_MultRecList[index].size = 0;		
	g_MultRecList[index].total = 0;		
	g_MultRecList[index].num = 0;		
	g_MultRecList[index].address = 0;	
	g_MultRecList[index].port = 0;		
	g_MultRecList[index].func = NULL; 		
	if (g_MultRecList[index].data)
	{
		free(g_MultRecList[index].data);
		g_MultRecList[index].data = NULL;
	}
	if (g_MultRecList[index].Recflags)
	{
		free(g_MultRecList[index].Recflags);
		g_MultRecList[index].Recflags = NULL;
	}	
	g_CurReiveNum--; 
	if ((g_MultSendList==NULL)&&(0==g_CurReiveNum))
	{
		net_set_mult_packet_recive(NULL);			//取消多包发送接口
	}
}

/*************************************************
  Function:    		free_recive_multipacket_all
  Description:		释放全部多包接收包
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void free_recive_multipacket_all(void)
{
	int32 i;
	for (i=0; i<REC_MAX_DATA_NUM; i++)
	{
		free_recive_multipacket(i);
	}	
	g_CurReiveNum = 0;	
}

/*************************************************
  Function:    		recive_multipacket_send
  Description:		接收到发送包处理
  Input: 
	1.recPacket		接收发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recive_multipacket_send(PRECIVE_PACKET recPacket)
{
	int32 i;
	uint16 CurNum;
	int32 size;
	char data[6];
	PNET_HEAD net_head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	uint32 ID = *(uint32 *)(recPacket->data+NET_HEAD_SIZE);
	uint16 total = *(uint16 *)(recPacket->data+NET_HEAD_SIZE+4);
	
	//超过最大包数,不接受
	if (total > MULT_SEND_MAX_PACKET)
	{
		log_printf("total package num is too big\n");
		return;
	}	
	CurNum = *(uint16 *)(recPacket->data+NET_HEAD_SIZE+6);
	size = net_head->DataLen - 8;
	//非法判断
	if (CurNum > total)
	{
		log_printf("cur tol num is bigger than total package\n");
		return;
	}	
	if (size != (recPacket->size-NET_HEAD_SIZE-8))
	{
		log_printf("size is err!\n");
		return;
	}	
	if (CurNum < total)
	{
		if (size!=MULT_SEND_PACKET_SIZE)
		{
			log_printf("package size is err!\n");
			return;
		}	
	}
	
	//应答	
	memcpy(data, recPacket->data+NET_HEAD_SIZE, 4);
	memcpy(data+4, recPacket->data+NET_HEAD_SIZE+6, 2);
	net_send_echo_packet(recPacket, ECHO_OK, data, 6);
	
	//查找会话ID
	for (i=0; i<REC_MAX_DATA_NUM; i++)
	{
		if (ID==g_MultRecList[i].ID)
		{
			if (recPacket->address!=g_MultRecList[i].address )
			{
				log_printf("addr is err\n");
				return;
			}	
			if (recPacket->port!=g_MultRecList[i].port )
			{
				log_printf("port is err\n");
				return;
			}	
			g_MultRecList[i].t0 = 0;
			//第一次收到包
			if (NULL==g_MultRecList[i].Recflags)
			{
				g_MultRecList[i].total = total;
				//分配
				g_MultRecList[i].Recflags = malloc(total);
				//内存分配失败
				if (NULL==g_MultRecList[i].Recflags)
				{
					log_printf("flags malloc err!\n");
					if (g_MultRecList[i].func)
					{
						g_MultRecList[i].func(g_MultRecList[i].ID, NULL, 0);
					}
					free_recive_multipacket(ID);
					return;
				}
				memset(g_MultRecList[i].Recflags, 0, total);
				
				//分配数据空间
				g_MultRecList[i].data = malloc(total*MULT_SEND_PACKET_SIZE);
				//内存分配失败
				if (NULL==g_MultRecList[i].data)
				{	
					log_printf("data malloc err!\n");
					if (g_MultRecList[i].func)
					{
						g_MultRecList[i].func(g_MultRecList[i].ID, NULL, 0);
					}				
					free_recive_multipacket(ID);
					return;
				}
				memset(g_MultRecList[i].data, 0, total*MULT_SEND_PACKET_SIZE);
			}
			else
			{
				if (total!=g_MultRecList[i].total)
				{
					log_printf("total is err!\n");
					return;
				}	
			}
			if (g_MultRecList[i].Recflags[CurNum-1] != 1)
			{	
				memcpy(g_MultRecList[i].data+(CurNum-1)*MULT_SEND_PACKET_SIZE, 
					recPacket->data+NET_HEAD_SIZE+8,size);
				g_MultRecList[i].size += size;
				g_MultRecList[i].num++;

				//接收OK标志
				g_MultRecList[i].Recflags[CurNum-1] = 1;
				
				if (g_MultRecList[i].state != NULL)
				{
					g_MultRecList[i].state(g_MultRecList[i].ID, total, g_MultRecList[i].num, g_MultRecList[i].size);
				}	
				//接收完毕
				if (g_MultRecList[i].num==g_MultRecList[i].total)
				{
					if (g_MultRecList[i].func)
					{
						g_MultRecList[i].func(g_MultRecList[i].ID, g_MultRecList[i].data, g_MultRecList[i].size);
					}	
					//释放分配的内存
					free_recive_multipacket(i);
				}
			}
			break;
		}
	}		
}

/*************************************************
  Function:    		stop_multipacket
  Description:		停止某个多包发送或接收过程
  Input: 			
  	1.ID			回话ID
  	2.flag			发送或接收标志, 0x01-中断发送(发送方发送) 0x02-中断接收(接收方发送)
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_multipacket(uint32 ID, uint8 flag, uint32 address)
{
	int32 i;
	if (flag==0x01)
	{
		PMULTI_SEND prev = NULL; 
		PMULTI_SEND cur = g_MultSendList;
		while(cur)
		{
			if (cur->ID==ID && cur->address==address)
			{
				if (prev==NULL)
				{
					g_MultSendList = cur->next;
				}
				else
				{
					prev->next = cur->next;
				}
				if (cur == g_LastMultSend)
				{
					g_LastMultSend = prev; 
				}	
				if (cur == g_CurMultiSend)
				{
					g_CurMultiSend = NULL;
				}	
				free(cur);
				break;
			}
			prev = cur;
			cur = cur->next;
		}
	}
	else if (flag == 0x02)
	{
		for(i=0; i<REC_MAX_DATA_NUM; i++)
		{
			if (g_MultRecList[i].ID==ID && g_MultRecList[i].address==address)
			{
				free_recive_multipacket(i);
				break;
			}	
		}
	}
}

/*************************************************
  Function:    		recive_multipacket_echo
  Description:		接收到应答包处理
  Input: 
	1.recPacket		接收应答包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recive_multipacket_echo(const PRECIVE_PACKET recPacket)
{
	uint32 * id = (uint32 *)(recPacket->data+NET_HEAD_SIZE);
	uint16 * pos = (uint16 *)(recPacket->data+NET_HEAD_SIZE+4);

	if (g_CurMultiSend!=NULL && g_SendOkNum<g_SendNum)
	{			
		if ((g_CurMultiSend->ID== *id) &&
			(g_CurMultiSend->address== recPacket->address) &&
			(g_CurMultiSend->port== recPacket->port))
		{
			// 给未应答包置位
			if (1!=g_SendFlags[*pos-1])
			{
				g_SendFlags[*pos-1] = 1;
				g_SendOkNum++;
				
				if (*pos==g_SendNum)
				{
					g_SendSize += g_LastSendSize;
				}	
				else
				{
					g_SendSize += MULT_SEND_PACKET_SIZE;
				}	
				if (g_CurMultiSend->state != NULL)
				{
					g_CurMultiSend->state(g_CurMultiSend->ID, g_SendNum, g_SendOkNum, g_SendSize);					
				}	
			}	
		}	
	}
}

/*************************************************
  Function:    		recive_multpacket
  Description:		接收多包发送处理过程
  Input: 			
  	1.RecivePacket	接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recive_multpacket(NET_COMMAND cmd, const PRECIVE_PACKET RecivePacket)
{
	uint32 ID;
	uint8 flag;
	PNET_HEAD net_head = (PNET_HEAD)(RecivePacket->data + MAIN_NET_HEAD_SIZE);
	switch (cmd)
	{
		case CMD_SEND_MAXDATA:
			if (DIR_SEND == (net_head->DirectFlag))
			{
				recive_multipacket_send(RecivePacket);	
			}	
			else
			{
				recive_multipacket_echo(RecivePacket);
			}	
			break;
		case CMD_STOP_SEND_MAXDATA:
			if (DIR_SEND == (net_head->DirectFlag))
			{
				ID = *((uint32 *)(RecivePacket->data+NET_HEAD_SIZE));
				flag = RecivePacket->data[NET_HEAD_SIZE+4];
				if (flag!=0x01 && flag!=0x02)
				{
					net_send_echo_packet(RecivePacket, ECHO_ERROR, NULL, 0);
				}	
				else
				{	
					stop_multipacket(ID, flag, RecivePacket->address);
					net_send_echo_packet(RecivePacket, ECHO_OK, NULL, 0);
				}	
			}	
			break;
	}	
}

/*************************************************
  Function:    		net_start_multipacket_recive
  Description:		开始分包接收数据
  Input: 
	1.address		对方IP地址
	2.port			对方端口号
	3.TimeOut		超时时间, 秒, 0时,为默认超时时间
	4.func			接收成功后应答回调函数
	5.state			接收状态回调函数
  Output:			无
  Return:			接收会话ID, 0为忙应答
  Others:
*************************************************/
uint32 net_start_multipacket_recive(uint32 address, uint16 port, uint8 TimeOut, PFMAX_RECIVE func, PFMAX_SEND_REC_STATE state)
{
	static int32 ID = 0;
	int32 i;
	int32 index = -1;
	if (NULL == func)
	{
		return 0;
	}
	
	if (g_CurReiveNum == REC_MAX_DATA_NUM)
	{
		return 0;
	}
	
	for (i=0; i<REC_MAX_DATA_NUM; i++)
	{
		if (0==g_MultRecList[i].ID)
		{
			
			index = i;
			break;
		}
	}		

	if (-1==index)
	{
		return 0;
	}
	if (ID==0 || ID==0xFFFFFFFF )
	{
		ID = 1;
	}	
	else
	{
		ID++;	
	}	
	g_MultRecList[index].data = NULL;					//接收数据
	g_MultRecList[index].Recflags = NULL;				//接收标志
	g_MultRecList[index].size = 0;						//接收数据长度
	g_MultRecList[index].total = 0;						//总包数
	g_MultRecList[index].num = 0;						//已经接收包数
	g_MultRecList[index].address = address;				//对端地址
	g_MultRecList[index].port = port;					//对端端口号
	g_MultRecList[index].func = func; 					//接收多包回调函数
	g_MultRecList[index].state = state;					//接收状态回调函数
	g_MultRecList[index].t0 = 0;						//开始时间
	if (TimeOut==0)
	{
		g_MultRecList[index].TimeOut = DEFAULT_MULT_RECIVE_TIMEOUT;
	}	
	else	
	{
		g_MultRecList[index].TimeOut = TimeOut;			//超时秒数
	}	
	//最后赋值ID
	g_MultRecList[index].ID = ID;						//会话ID
	g_CurReiveNum++;
	net_set_mult_packet_recive(recive_multpacket); 		// 设置多包发送接口
	
	return ID;
}

/*************************************************
  Function:    		net_stop_multipacket_recive
  Description:		中断分包接收数据, (用户可以中断分包接收)
  Input: 
	1.ID			接收会话ID
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_multipacket_recive(uint32 ID)
{
	int32 i;
	for (i=0; i<REC_MAX_DATA_NUM; i++)
	{
		if (ID==g_MultRecList[i].ID)
		{
			//发送中断接收命令
			char data[5];
			memcpy(data, &ID, 4);
			data[4] = 0x02;
			set_nethead(G_MULTCMD_DEVNO, PRIRY_DEFAULT);
			net_direct_send(CMD_STOP_SEND_MAXDATA, data, 5, g_MultRecList[i].address, g_MultRecList[i].port);
			free_recive_multipacket(i);
			break;
		}
	}		
}

/*************************************************
  Function:    		net_recive_mult_timeout_proc
  Description:		多包接收超时检查
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_recive_mult_timeout_proc(void *arg)
{
	int32 i;

	while (1)
	{
		if (g_CurReiveNum == 0)
		{
			usleep(500*1000);
		}
		
		for (i=0; i<REC_MAX_DATA_NUM; i++)
		{
			if (g_MultRecList[i].ID>0 && g_MultRecList[i].TimeOut>0)
			{
				g_MultRecList[i].t0++;
				
				//接收超时了 500ms 定时器
				if (g_MultRecList[i].t0*NET_MULTI_RECV_CHECK_TICKS*5 > g_MultRecList[i].TimeOut*1000)
				{
					if (g_MultRecList[i].func)
					{
						g_MultRecList[i].func(g_MultRecList[i].ID, NULL, 0);
					}	
					free_recive_multipacket(i);
				}
			}
		}
		usleep(500*1000);
	}
}

/*************************************************
  Function:    		mult_send
  Description:		多包发送一包
  Input: 			
  	1.id			ip地址
  	2.num			包总数
  	3.curNo			当前包号, 从1开始
  	4.data			包数据
  	5.size			数据大小
  	6.address		发送地址
  	7.port			发送端口
  Output:			无
  Return:			无
  Others:
*************************************************/
static void mult_send(uint32 id, uint16 num, uint16 curNo, char * data, int32 size, 
						uint32 address, uint16 port)
{
	uint16 PackageID = get_package_ID();
	
	g_NetHead->PackageID = PackageID;						//包标识
	g_NetHead->DirectFlag = DIR_SEND;						//方向标志（主动：0xAA；应答：0x00）
	g_NetHead->SubSysCode = (uint8)(CMD_SEND_MAXDATA>>8);	//子系统代号
	g_NetHead->command = (uint8)(CMD_SEND_MAXDATA & 0xFF);	//命令值
	g_NetHead->EchoValue = 0;								//应答码
	g_NetHead->Encrypt = 0;									//加密类型
	g_NetHead->DataLen = size + 8;							//数据长度

	#ifndef _AU_PROTOCOL_
	// 主协议包头
	set_nethead(G_MULTCMD_DEVNO, PRIRY_DEFAULT);
	g_Main_NetHead->mainSeq = PackageID;
	g_Main_NetHead->mainPacketType = 0x0000;				// 普通包
	g_Main_NetHead->subPacketLen = sizeof(NET_HEAD) + g_NetHead->DataLen;
	g_NetHead->Priry = 0xFF;
	g_NetHead->DestDeviceNo = DestDeviceNo;
	g_NetHead->Priry = DefaultPriry;
	#endif
	
	memcpy(g_SendBuf+NET_HEAD_SIZE, &id, 4);				//会话ID
	memcpy(g_SendBuf+NET_HEAD_SIZE+4, &num, 2);				//包总数
	memcpy(g_SendBuf+NET_HEAD_SIZE+6, &curNo, 2);			//当前包号
	memcpy(g_SendBuf+NET_HEAD_SIZE+8, data, size);			//拷贝附加数据

	net_send_cmd_packet(g_SendBuf, g_NetHead->DataLen+NET_HEAD_SIZE, address, port);
}

/*************************************************
  Function:    		mult_send_proc
  Description:		多包发送线程处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void mult_send_proc(void)
{	
	static int32 sendtimes = 0;						// 发送次数
	static int32 GroupNum = 28;						// 每组个数
	static int32 SleepValue = 10;					// 停止时间			

	if (g_MultSendList == NULL)
	{
		return;
	}
	
	if (g_CurMultiSend == NULL)
	{
		g_CurMultiSend = g_MultSendList;
		
		// 发送包数及最后一包字节数
		g_SendNum = g_CurMultiSend->size/MULT_SEND_PACKET_SIZE;		
		g_LastSendSize = MULT_SEND_PACKET_SIZE;
		if ((g_CurMultiSend->size%MULT_SEND_PACKET_SIZE) > 0)
		{	
			g_LastSendSize = g_CurMultiSend->size - g_SendNum*MULT_SEND_PACKET_SIZE;
			g_SendNum++;
		}	
		
		g_SendOkNum = 0;
		g_SendSize = 0;
		g_SendFlags = malloc(g_SendNum);					//发送应答标志
		if (g_SendFlags)
		{
			memset(g_SendFlags, 0, g_SendNum);
		}
		else
		{
			//应答发送失败
			if (g_CurMultiSend->func != NULL)	
			{
				g_CurMultiSend->func(g_CurMultiSend->ID, 1);
			}
			g_MultSendList = g_MultSendList->next;
			free(g_CurMultiSend);
			g_CurMultiSend = NULL;
			if ((g_MultSendList==NULL)&&(0==g_CurReiveNum))
			{
				net_set_mult_packet_recive(NULL);	// 取消多包发送接口
			}
			return;
		}
		
		sendtimes = 0;
		GroupNum = 28;
		SleepValue = 10;
	}
	else
	{
		int32 i = 0;
		int32 LastSendOK = g_SendOkNum; 			// 上次发送成功数
		char * data = g_CurMultiSend->data;
		int32 SendNum = 0;							// 发一遍, 本次发送到包数

		// 发送除最后一包外未发送成功的包
		for(i = 0; i < g_SendNum-1; i++)
		{
			if (g_SendFlags[i]==0)
			{	
				mult_send(g_CurMultiSend->ID, g_SendNum, i+1, data, MULT_SEND_PACKET_SIZE, 
					g_CurMultiSend->address, g_CurMultiSend->port);
				SendNum++;
				DelayMs_nops(5);
				if (SendNum%GroupNum==0)
				{
					DelayMs_nops(SleepValue);	
				}
			}
			data += MULT_SEND_PACKET_SIZE;
		}

		// 发送最后一包.发送完一遍,发送次数增加
		if (g_SendFlags[g_SendNum-1]==0)
		{	
			mult_send(g_CurMultiSend->ID, g_SendNum, i+1, data, 
				g_LastSendSize, g_CurMultiSend->address, g_CurMultiSend->port);
			DelayMs_nops(10);	
			data += g_LastSendSize;
		}
		sendtimes++;

		// 等待应答
		i = 5;
		while(g_SendOkNum<g_SendNum && i>0)
		{
			// 本次发送都已经接收
			if ((g_SendOkNum-LastSendOK)==SendNum)
			{
				break;
			}	
			DelayMs_nops(100);
			i--;
		}
			
		if (g_SendOkNum == g_SendNum)
		{
			// 应答发送成功
			if (g_CurMultiSend->func!=NULL)	
			{
				g_CurMultiSend->func(g_CurMultiSend->ID, 0);
			}	
			g_MultSendList = g_MultSendList->next;
			free(g_CurMultiSend);
			g_CurMultiSend = NULL;
			free(g_SendFlags);
			g_SendFlags = NULL;
			if ((g_MultSendList==NULL)&&(0==g_CurReiveNum))
			{
				net_set_mult_packet_recive(NULL);			//取消多包发送接口
			}
			return;
		}

		// 无应答或发送次数超过100次,发送失败
		if(LastSendOK == g_SendOkNum || sendtimes>100)
		{
			//应答发送失败
			if (g_CurMultiSend->func!=NULL)	
			{
				g_CurMultiSend->func(g_CurMultiSend->ID, 1);
			}	
			g_MultSendList = g_MultSendList->next;
			free(g_CurMultiSend);
			g_CurMultiSend = NULL;
			free(g_SendFlags);
			g_SendFlags = NULL;
			if ((g_MultSendList==NULL)&&(0==g_CurReiveNum))
			{
				net_set_mult_packet_recive(NULL);			//取消多包发送接口
			}
			return;
		}
		else //计算丢包率, 减少发送间隔发送包数及增加发送间隔时间
		{
			if (1==GroupNum)
			{
				SleepValue += 10; 					//调整时间
			}
			else
			{	
				int32 LostRate =100*(SendNum-(g_SendOkNum-LastSendOK))/SendNum;
				if (LostRate>0)
				{
					int32 n = LostRate/10;

					//调整GroupNum值
					if (n==0)
					{
						GroupNum--;
					}	
					else	
					{
						GroupNum -= n;
					}	
					if (GroupNum<1)
					{
						GroupNum = 1;
						SleepValue += 10;
					}	
				}	
			}
		}
	}	
}	

/*************************************************
  Function:    		net_multi_send_check
  Description:		检查是否有多包发送任务需要处理
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_multi_send_check(void)
{
	if (g_MultSendList)
	{
	#if USE_DISTRIBUTE_THREAD
		//RockOSSendMsg(MB_NETDIST, AS_NET_MULTI_SEND_NOTIFY, NULL);
	#endif	
	}
	else
	{
		return;
	}
}

/*************************************************
  Function:    		net_start_multi_packet_send
  Description:		分包数据发送
  Input: 
	1.ID			会话ID
	2.data			数据
	3.size			数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.func			发送成功后应答回调函数
	7.state			发送状态回调函数
  Output:			无
  Return:			是否成功, TRUE / FALSE
  Others:
*************************************************/
int32 net_start_multi_packet_send(uint32 ID, char * data, int32 size, uint32 address, uint16 port, PFMAX_SEND_ECHO func, PFMAX_SEND_REC_STATE state)
{
	int32 num;
	PMULTI_SEND node = NULL;
	if (data == NULL)
	{
		return FALSE;
	}
	if (size < 1) 
	{
		return FALSE;
	}	
	num = size/MULT_SEND_PACKET_SIZE;
	if (size%MULT_SEND_PACKET_SIZE != 0)
	{
		num++;
	}	
	if (num > MULT_SEND_MAX_PACKET)
	{
		return FALSE;
	}
		
	node = (PMULTI_SEND)calloc(sizeof(MULTI_SEND), 1);
	if (node==NULL)
	{
		return FALSE;
	}	
	node->ID = ID;
	node->size = size;
	node->data = data;
	node->address = address;
	node->port = port;
	node->func = func;
	node->state = state;
	node->next = NULL;
	if (NULL==g_MultSendList)
	{	
		g_MultSendList = node;
		g_LastMultSend = node;
	}	
	else
	{
		g_LastMultSend->next = node;
		g_LastMultSend = node;
	}
	net_set_mult_packet_recive(recive_multpacket); 		// 设置多包发送接口
#if USE_DISTRIBUTE_THREAD	
	//RockOSSendMsg(MB_NETDIST, AS_NET_MULTI_SEND_NOTIFY, NULL);
#endif	
	return TRUE;
}

/*************************************************
  Function:    		net_stop_multipacket_send
  Description:		中断分包发送数据, (用户可以中断分包发送)
  Input: 
	1.ID			发送会话ID
	2.address		发送地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_multipacket_send(uint32 ID, uint32 address)
{
	PMULTI_SEND prev = NULL; 
	PMULTI_SEND cur = g_MultSendList;
	char data[5];
	while(cur)
	{
		if (cur->ID==ID && cur->address==address)
		{
			if (prev==NULL)
			{
				g_MultSendList = cur->next;
			}
			else
			{
				prev->next = cur->next;
			}
			if (cur==g_LastMultSend)
			{
				g_LastMultSend = prev; 
			}	
			if (cur==g_CurMultiSend)
			{
				g_CurMultiSend = NULL;
			}	
			//发送中断发送命令
			memcpy(data, &ID, 4);
			data[4] = 0x01;
			set_nethead(G_MULTCMD_DEVNO, PRIRY_DEFAULT);
			net_direct_send(CMD_STOP_SEND_MAXDATA, data, 5, cur->address, cur->port);
			free(cur);
			break;
		}
		prev = cur;
		cur = cur->next;
	}
}

/*************************************************
  Function:    		net_get_utc_time
  Description:		获取UTC时间
  Input: 
  Output:			
  	1.DateTime		时间结构体
  Return:			无
  Others:			默认时区为8
*************************************************/
void net_now_time(NET_DATA_TIME * DateTime)
{
	ZONE_DATE_TIME LocalTime;
	get_timer(&LocalTime);
	memset(DateTime, 0, sizeof(NET_DATA_TIME));
	DateTime->year = LocalTime.year;
	DateTime->month = LocalTime.month;
	DateTime->day = LocalTime.day;
	DateTime->hour = LocalTime.hour;
	DateTime->min = LocalTime.min;
	DateTime->sec = LocalTime.sec;
}

/*************************************************
  Function:    		net_get_utc_time
  Description:		获取UTC时间
  Input: 
  Output:			
  	1.DateTime		时间结构体
  Return:			无
  Others:			默认时区为8
*************************************************/
void net_get_utc_time(NET_DATA_TIME * DateTime)
{
	ZONE_DATE_TIME LocalTime;
	get_timer(&LocalTime);

#if 0
	// 8 时区默认
	if (LocalTime.hour >= 8)
	{
		LocalTime.hour -= 8;
	}
	else
	{
		// 时钟
		LocalTime.hour = (LocalTime.hour+24) - 8;

		// day > 1
		if (LocalTime.day > 1)
		{
			LocalTime.day--;
		}
		// day == 1
		else
		{
			// month > 1
			if (LocalTime.month > 1)
			{
				LocalTime.month--;
				LocalTime.day = MonthDays(LocalTime.year, LocalTime.month);
			}
			// month == 1
			else
			{
				LocalTime.year--;
				LocalTime.month = 12;
				LocalTime.day = MonthDays(LocalTime.year, LocalTime.month);
			}
		}
	}
#else
	get_utc_time(&LocalTime, 8);
#endif

	memset(DateTime, 0, sizeof(NET_DATA_TIME));
	DateTime->year = LocalTime.year;
	DateTime->month = LocalTime.month;
	DateTime->day = LocalTime.day;
	DateTime->hour = LocalTime.hour;
	DateTime->min = LocalTime.min;
	DateTime->sec = LocalTime.sec;
}

