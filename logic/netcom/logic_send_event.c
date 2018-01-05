/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_send_event.c
  Author:   	xiewr	
  Version:  	1.0
  Date: 
  Description:  发送事件模块
				上报一般事件或报警事件，如果对方不在线则先保存
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_send_event.h"

#define	FILE_NAME		"D:\\nosend_event.dat"			// 事件发送文件存储路径
#define	SENDTO_NUM		3							// 发送目的地的数量（冠林平台,中心服务器,管理员机）

#pragma pack(push,1)  								// 开始定义数据包, 使用字节对齐方式
// 存储状态
typedef struct _STORE_STATUS
{
	uint32 head;
	uint32 tail;
	uint32 count;
	char reserve[4];								// 保留
}STORE_STATUS;

// 文件头
typedef struct _FILE_HEAD
{
	uint16 RecordCount;								// 可存储的记录数
	uint16 RecordLen;								// 每个记录长
	uint16 SendtoNum;								// 存储STORE_STATUS结构数
	char reserve[10];								// 保留
	STORE_STATUS StoreStatus[SENDTO_NUM];			// 存储状态	
}FILE_HEAD;
#pragma pack(pop)  									// 结束定义数据包, 恢复原对齐方式

typedef enum
{
	 STATE_NODATA	= 0,							// 没有数据发送
	 STATE_READY	= 1,							// 等待发送(可能网络原因无法发送)
	 STATE_SENDING	= 2,							// 正在发送
}STATE_E;

// 发送状态
typedef struct _SEND_STATUS
{
	char state;										// 状态
	char SendTimes;									// 发送次数, 255次后不累计了
	uint32 NextSendTime;							// 下次重发的时间(秒)
	uint16 SendPacketID[3];							// 最后三次发送的包ID
	NET_COMMAND cmd;								// 命令码
	char data[SEND_RECORD_DATA_LEN];
	char size;										// 发送数据长度
}SEND_STATUS;

#define FILE_HEAD_LEN				(sizeof(FILE_HEAD))

#define get_limit_up(index)			(FILE_HEAD_LEN + index * SEND_RECORD_MAXLEN*SEND_RECORD_STOR_COUNT)
#define get_limit_down(index)		(FILE_HEAD_LEN + (index+1)* SEND_RECORD_MAXLEN*SEND_RECORD_STOR_COUNT-SEND_RECORD_MAXLEN)
 
static FILE_HEAD g_FileHead; 						// 文件头
static FILE* g_file;								// 文件
static SEND_STATUS g_SendStatus[SENDTO_NUM];		// 发送状态,顺序同存储顺序
static char *g_FileBuf = NULL;
static char g_UpdateFile = FALSE;					// 是否写文件

static uint32 send_event_data(int32 index);
void net_update_event_file(void);

/*************************************************
  Function:			get_sento_index
  Description: 		获得存储索引值
  Input: 			无
  Output:			无
  Return:			存储索引值
*************************************************/
static int32 get_sento_index(EVENT_SENDTO_E SendTo)
{
	switch (SendTo)
	{
		case EST_AURINE:
			return 0;
			
		case EST_SERVER:
			return 1;
			
		case EST_MANAGER:
			return 2;
			
		default:
			return -1; 
	}
}

/*************************************************
  Function:			get_address
  Description: 		获得指定索引值的IP地址
  Input: 			无
  Output:			无
  Return:			IP地址
*************************************************/
static uint32 get_address(int32 index)
{
	switch (index)
	{
		case 0:
			return net_get_aurine_ip();
			
		case 1:
			return net_get_center_ip();
			
		case 2:
			return net_get_manager_ip(1);
			
		default:
			return 0; 
	}	
}

/*************************************************
  Function:    		read_unsend
  Description:		读网络发送不成功的一条记录
  Input: 
	1.index			位置索引
	2.data			记录列表指针
  Output:			无
  Return:			实际读记录数
  Others:
*************************************************/
static int32 read_unsend(int32 index, char *data)
{
	if (g_file == NULL)
	{	
		net_ini_unsend_file();
		if (g_file == NULL) 
		{
			return 0;
		}	
	}
	if (g_FileHead.StoreStatus[index].count < 1)
	{
		return 0;
	}
	memcpy(data, g_FileBuf+g_FileHead.StoreStatus[index].head, SEND_RECORD_MAXLEN);
	return 1;
}

/*************************************************
  Function:			write_unsend
  Description:		写网络发送不成功的记录
  Input: 
	1.index			位置索引
	2.data			数据指针
  Output:			无
  Return:			成功与否
*************************************************/
static int32 write_unsend(int32 index, char *data)
{
	//DECLARE_CUP_SR;
	int32 pos;
	int32 up;
	int32 down;
	
	if (g_file == NULL)
	{
		net_ini_unsend_file();
	}
	if (g_file == NULL)
	{
		return FALSE;
	}
	
	//ENTER_CRITICAL();

	// 定位
	pos = g_FileHead.StoreStatus[index].tail;
	memcpy(g_FileBuf+pos, data, SEND_RECORD_MAXLEN);
	
	up = get_limit_up(index);
	down = get_limit_down(index);
		
	// 写文件头
	if (g_FileHead.StoreStatus[index].count>=SEND_RECORD_STOR_COUNT)
	{
		if (g_FileHead.StoreStatus[index].head==down)
		{
			g_FileHead.StoreStatus[index].head = up;
		}	
		else
		{
			g_FileHead.StoreStatus[index].head += SEND_RECORD_MAXLEN;
		}	
		g_FileHead.StoreStatus[index].tail	= g_FileHead.StoreStatus[index].head; 
		g_FileHead.StoreStatus[index].count = SEND_RECORD_STOR_COUNT;
	}
	else
	{	
		g_FileHead.StoreStatus[index].count++;
		if (g_FileHead.StoreStatus[index].tail==down)
		{
			g_FileHead.StoreStatus[index].tail = up; 
		}	
		else
		{
			g_FileHead.StoreStatus[index].tail += SEND_RECORD_MAXLEN;
		}	
	}
	//EXIT_CRITICAL();
	return TRUE;
}

/*************************************************
  Function:			del_unsend
  Description: 		删除网络发送不成功的一条记录
  Input: 
	1.index			位置索引
  Output:			无
  Return:			成功与否
*************************************************/
static int32 del_unsend(int32 index)
{
	//DECLARE_CUP_SR;
	int32 up;
	int32 nextup;
	int32 head;
	
	if (g_file == NULL)
	{
		net_ini_unsend_file();
	}	
	if (g_file == NULL)
	{
		return FALSE;
	}	
	up = get_limit_up(index);
	nextup = get_limit_up(index+1);
	
	//清空了
	//ENTER_CRITICAL();  关中断
	if (1 == g_FileHead.StoreStatus[index].count)
	{
		g_FileHead.StoreStatus[index].head = up;
		g_FileHead.StoreStatus[index].tail = up;
		g_FileHead.StoreStatus[index].count = 0;
	}	
	else
	{
		g_FileHead.StoreStatus[index].count -= 1;
		head = g_FileHead.StoreStatus[index].head + SEND_RECORD_MAXLEN;
		if (nextup == head)
		{
			head = up;
		}
		g_FileHead.StoreStatus[index].head = head;
	}	
	//EXIT_CRITICAL();  开中断
	return TRUE;
}

/*************************************************
  Function:    		send_event_data
  Description:		从文件中加载数据发送
  Input: 			无
	1.index			位置索引
  Output:			无
  Return:			超时时间, 秒, 0为错误
  Others:
*************************************************/
static uint32 send_event_data(int32 index)
{
	uint32 address;
	int32 id;
	uint32 n;
	int32 i;
	
	if (g_SendStatus[index].state == STATE_NODATA)
	{
		return 0;
	}
	address = get_address(index);	
	if (0 == address) 
	{
		return 0;
	}
	if (index == 2)
	{
		set_nethead(G_MANAGER_DEVNO, PRIRY_HIGHEST);
	}
	else
	{
		set_nethead(G_CENTER_DEVNO, PRIRY_HIGHEST);
	}
	id = net_direct_send(g_SendStatus[index].cmd, g_SendStatus[index].data, 
					g_SendStatus[index].size, address, NETCMD_UDP_PORT);
	g_SendStatus[index].state = STATE_SENDING;
	
	//设置下次重发时间
	switch(g_SendStatus[index].SendTimes)
	{
		case 0:
		case 1:
		case 2:
			n = 2;
			break;
		case 3:
		case 4:
		case 5:
			n = 6;
			break;
		case 6:
		case 7:
		case 8:
			n = 12;
			break;
		default:	
			n = 60;
			break;
	}

	g_SendStatus[index].NextSendTime = n;

	// 更新最近3个包ID
	i = g_SendStatus[index].SendTimes%3;
	g_SendStatus[index].SendPacketID[i] = id; 
	if (g_SendStatus[index].SendTimes < 10)
	{
		g_SendStatus[index].SendTimes++;
	}	
	return n;
}

/*************************************************
  Function:    		load_send_data
  Description:		从文件中加载数据发送
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void load_send_data(void)
{
	int32 i;
	uint32 n = 0;									// 最小超时时间
	uint32 t;
	char RecordBuf[SEND_RECORD_MAXLEN];
	
	for (i=0; i<SENDTO_NUM; i++)
	{
		// 加载数据
		if (g_FileHead.StoreStatus[i].count>0 && g_SendStatus[i].state==STATE_NODATA)
		{
			// 读错误
			if (read_unsend(i, RecordBuf) != 1) 
			{
				continue;
			}	
			g_SendStatus[i].cmd = ((RecordBuf[1]<<8)&0xFF00) | RecordBuf[2];
			g_SendStatus[i].size = RecordBuf[0] - SEND_RECORD_HEAD_LEN;
			memcpy(g_SendStatus[i].data, RecordBuf+SEND_RECORD_HEAD_LEN, g_SendStatus[i].size); 
			g_SendStatus[i].SendPacketID[0] = 0;
			g_SendStatus[i].SendPacketID[1] = 0;
			g_SendStatus[i].SendPacketID[2] = 0;
			g_SendStatus[i].SendTimes = 0; 
			g_SendStatus[i].state = STATE_READY;
			t = send_event_data(i);
			if (n==0)
			{
				n = t;
			}	
			else	
			{
				n = (t>0 && t<n)? t : n;
			}	
		}	
	}
	// 开始定时器
	if (n > 0)
	{
		
	}	
}

/*************************************************
  Function:			send_ok_proc
  Description: 		发送事件成功应答处理函数
  Input: 
	1.index			索引值
  Output:			无
  Return:			无
*************************************************/
static void send_ok_proc(int32 index)
{
	uint32 n;
	char RecordBuf[SEND_RECORD_MAXLEN];
	
	g_SendStatus[index].state = STATE_NODATA;
	del_unsend(index);
	
	// 加载发送
	if (g_FileHead.StoreStatus[index].count > 0)
	{
		// 加载数据
		if (read_unsend(index, RecordBuf) != 1)
		{
			return;
		}
		g_SendStatus[index].cmd = ((RecordBuf[1]<<8)&0xFF00) | RecordBuf[2];
		g_SendStatus[index].size = RecordBuf[0] - SEND_RECORD_HEAD_LEN;
		memcpy(g_SendStatus[index].data, RecordBuf+SEND_RECORD_HEAD_LEN, g_SendStatus[index].size); 
		g_SendStatus[index].SendPacketID[0] = 0;
		g_SendStatus[index].SendPacketID[1] = 0;
		g_SendStatus[index].SendPacketID[2] = 0;
		g_SendStatus[index].state = STATE_READY;
		n = send_event_data(index);
		if (n > 0)
		{
			
		}
	}
}

/*************************************************
  Function:			net_ini_unsend_file
  Description: 		启动时,初始化事件文件
  Input: 			无
  Output:			无
  Return:			无
*************************************************/
void net_ini_unsend_file(void)
{
	int32 i;
	FILE * fp;
	int32 head;
	//char RecordBuf[SEND_RECORD_MAXLEN]; 			// 记录buf
	
	if (g_file != NULL)
	{
		return;
	}
		
	memset(g_SendStatus, 0, sizeof(g_SendStatus));
	for(i=0; i<SENDTO_NUM; i++)
	{
		g_SendStatus[i].state = STATE_NODATA;		//没有数据发送状态
	}

	if (g_FileBuf == NULL)
	{
		g_FileBuf = malloc(sizeof(FILE_HEAD)+SEND_RECORD_STOR_COUNT*SENDTO_NUM);
	}
	if (g_FileBuf)
	{
		memset(g_FileBuf, 0, sizeof(FILE_HEAD)+SEND_RECORD_STOR_COUNT*SENDTO_NUM);
	}
		
	// 读文件信息
	fp = fopen(FILE_NAME, "r+");
	if (fp != NULL)
	{
		// 读文件头
		fread(g_FileBuf, 1, sizeof(FILE_HEAD)+SEND_RECORD_STOR_COUNT*SENDTO_NUM, fp);
		memcpy(&g_FileHead, g_FileBuf, sizeof(FILE_HEAD));
		
		// 判断文件是否合法
		if (g_FileHead.RecordCount==SEND_RECORD_STOR_COUNT &&
			g_FileHead.RecordLen==SEND_RECORD_MAXLEN &&
			g_FileHead.SendtoNum==SENDTO_NUM)
		{
			g_file = fp;
			load_send_data();
			return;									// 文件有效
		}
		// 无效,需要初始化
		fclose(fp);
	}
	
	fp = fopen(FILE_NAME,"w+");
	if(fp == NULL)
	{
		return;
	}
	// 写文件头
	memset(&g_FileHead, 0, sizeof(g_FileHead));
	g_FileHead.RecordCount = SEND_RECORD_STOR_COUNT;
	g_FileHead.RecordLen = SEND_RECORD_MAXLEN;
	g_FileHead.SendtoNum = SENDTO_NUM;
	memset(g_FileHead.reserve, 0xFF, sizeof(g_FileHead.reserve));
	memcpy(g_FileHead.reserve, "EVENT.DAT", 9);
	for (i=0; i<SENDTO_NUM; i++)
	{
		head = get_limit_up(i);
		g_FileHead.StoreStatus[i].head = head;
		g_FileHead.StoreStatus[i].tail = head;
		g_FileHead.StoreStatus[i].count = 0;
		memset(g_FileHead.StoreStatus[i].reserve, 0xFF, sizeof(g_FileHead.StoreStatus[i].reserve));
	}	
	memcpy(g_FileBuf, &g_FileHead, sizeof(FILE_HEAD));
	g_file = fp;
}

/*************************************************
  Function:    		net_close_unsend_file
  Description:		关闭未发送成功文件(在程序结束时关闭)
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_close_unsend_file(void)
{
	if (NULL != g_file)
	{
		net_update_event_file();
		fclose(g_file);
		g_file = NULL;
	}
}

/*************************************************
  Function:			net_send_event_from_file
  Description: 		从存储的文件中发送指定发送方的事件
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.SendTo		发送方, 冠林平台/中心服务器/管理员机 
  Output:			无
  Return:			是否成功
*************************************************/
int32 net_send_event_from_file(NET_COMMAND cmd, char * data, int32 size, EVENT_SENDTO_E SendTo)
{
	int32 index;
	char EventBuf[SEND_RECORD_MAXLEN];
	
	// 判断数据大小是否超过最大记录长度, 减去2个命令长度和一个字节存储长度
	if (size > SEND_RECORD_DATA_LEN)
	{
		return FALSE;
	}
	index = get_sento_index(SendTo);
	if (-1 == index)
	{
		return FALSE; 	
	}
	
	memset(EventBuf, 0, SEND_RECORD_MAXLEN);
	EventBuf[0] = size + SEND_RECORD_HEAD_LEN; 		// 数据长度
	EventBuf[1] = (cmd >> 8) & 0xFF;
	EventBuf[2] = cmd & 0xFF;
	memcpy(EventBuf+SEND_RECORD_HEAD_LEN, data, size);
	
	// 先存储
	write_unsend(index, EventBuf);

	// 加载发送
	load_send_data();
	g_UpdateFile = TRUE;
	return TRUE;
}

/*************************************************
  Function:			net_check_is_report_event_echo
  Description: 		判断该应答包是否上报事件的应当包
  Input: 
	1.ID			应答命令包
  Output:			无
  Return:			是否成功
*************************************************/
int32 net_check_is_report_event_echo(NET_COMMAND cmd, uint16 ID)
{
	int32 i, j;
	if (ID == 0)
	{
		return FALSE;
	}
	
	for(i=0; i<SENDTO_NUM; i++)
	{ 
		if (cmd != g_SendStatus[i].cmd)
		{
			continue;
		}
		for (j=0; j<3; j++)							// 检查最后三包的ID号
		{
			if (ID==g_SendStatus[i].SendPacketID[j])
			{
				send_ok_proc(i);
				return TRUE;
			}	
		}	
	}
	return FALSE;
}

/*************************************************
  Function:			net_update_event_file
  Description: 		定时更新事件发送文件
  Input: 			无
  Output:			无
  Return:			无
*************************************************/
void net_update_event_file(void)
{
	if (g_UpdateFile)
	{
		if (g_file)
		{
			fseek(g_file, 0, SEEK_SET);
			fwrite(g_FileBuf, 1, sizeof(FILE_HEAD)+SEND_RECORD_STOR_COUNT*SENDTO_NUM, g_file);
		}
	}
	g_UpdateFile = FALSE;
}

