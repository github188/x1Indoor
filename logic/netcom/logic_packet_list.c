/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_packet_list.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  网络发送接收链表
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_packet_list.h"
#include <semaphore.h>


static PSEND_PACKET g_SendFreeList = NULL;			// 发送空闲节点链表, 单向链表
static PRECIVE_PACKET g_RecvFreeList = NULL;		// 接收空闲节点链表, 单向链表

static PSEND_PACKET g_SendList = NULL;				// 发送包列表
static PSEND_PACKET g_SendLastNode = NULL;			// 发送包列表最后节点

static PRECIVE_PACKET g_ReciveList = NULL;  		// 接收队列
static PRECIVE_PACKET g_ReciveLastNode = NULL;  	// 接收队列尾

static sem_t g_SendListSem;							// 发送队列信号量
static sem_t g_RecvListSem;							// 接收队列信号量

static int g_SendRunning = 0;						// 发送线程是否运行
static int g_ReciveRunning = 0;						// 接收线程是否运行

static pthread_t SendThread;						// 发送线程
static pthread_t TimeOutThread;						// 发送超时重发控制线程
static pthread_t ReciveThread;						// 接收线程
static pthread_t MulTimeOutThread;					// 多包接收超时重发控制线程

static PFSEND_TIMEOUT_ECHO g_SendTimeoutFunc = NULL;// 发送超时应答函数指针

/*************************************************
  Function:    		recvlist_lock
  Description: 		接收列表上锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recvlist_lock(void)
{
	sem_wait(&g_RecvListSem);
}

/*************************************************
  Function:    		recvlist_unlock
  Description: 		接收列表解锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recvlist_unlock(void)
{
	sem_post(&g_RecvListSem);
}

/*************************************************
  Function:    		sendlist_lock
  Description: 		发送列表上锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendlist_lock(void)
{
	sem_wait(&g_SendListSem);
}

/*************************************************
  Function:    		sendlist_unlock
  Description: 		发送列表解锁
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void sendlist_unlock(void)
{
	sem_post(&g_SendListSem);
}

/*************************************************
  Function:    		list_sem_init
  Description: 		信号量初始化
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void list_sem_init(void)
{
	sem_init(&g_SendListSem, 0, 1);   			
	sem_init(&g_RecvListSem, 0, 1);   			
}

/*************************************************
  Function:    		list_sem_del
  Description: 		信号量删除
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void list_sem_del(void)
{
	sem_destroy(&g_SendListSem);   			
	sem_destroy(&g_RecvListSem);   			
}

/*************************************************
  Function:    		net_packet_list_init
  Description: 		初始化链表,分配空间
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_packet_list_init(void)
{
	uint32 i = 0;
	PSEND_PACKET SendNode = NULL;
	PRECIVE_PACKET RecvNode = NULL;
	if (g_SendFreeList == NULL)
	{
		for (i = 0; i < PACKET_NODE_MAX_NUM; i++)
		{
			// 分配一个节点
			SendNode = (PSEND_PACKET)malloc(sizeof(SEND_PACKET));
			if (SendNode == NULL)
			{
				continue;
			}
			else
			{
				memset(SendNode, 0, sizeof(SEND_PACKET));
				SendNode->data = (char*)malloc(PACKET_NODE_DATA_SIZE);
				if (SendNode->data == NULL)
				{
					free(SendNode);
					SendNode = NULL;
					continue;
				}
			}

			// 把节点加到链表
			if (g_SendFreeList == NULL)
			{
				g_SendFreeList = SendNode;
				g_SendFreeList->next = NULL;
			}
			else
			{
				SendNode->next = g_SendFreeList;
				g_SendFreeList = SendNode;
			}
		}
	}

	if (g_RecvFreeList == NULL)
	{
		for (i = 0; i < PACKET_NODE_MAX_NUM; i++)
		{
			// 分配一个节点
			RecvNode = (PRECIVE_PACKET)malloc(sizeof(RECIVE_PACKET));
			if (RecvNode == NULL)
			{
				continue;
			}
			else
			{
				memset(RecvNode, 0, sizeof(RECIVE_PACKET));
				RecvNode->data = (char*)malloc(PACKET_NODE_DATA_SIZE);
				if (RecvNode->data == NULL)
				{
					free(RecvNode);
					RecvNode = NULL;
					continue;
				}
			}

			// 把节点加到链表
			if (g_RecvFreeList == NULL)
			{
				g_RecvFreeList = RecvNode;
				g_RecvFreeList->next = NULL;
			}
			else
			{
				RecvNode->next = g_RecvFreeList;
				g_RecvFreeList = RecvNode;
			}
		}
	}	
	list_sem_init();
}

/*************************************************
  Function:    		get_sendnode_from_freelist
  Description: 		从发送空闲链表中获取节点
  Input: 			无
  Output:			无
  Return:			节点指针
  Others:
*************************************************/
static PSEND_PACKET get_sendnode_from_freelist(void)
{
	PSEND_PACKET tmp = NULL;
	if (g_SendFreeList == NULL)
	{
		log_printf("send free list is null\n");
		return NULL;
	}

	sendlist_lock();
	if (g_SendFreeList->next == NULL)
	{
		tmp = g_SendFreeList;
		g_SendFreeList = NULL;
	}
	else
	{
		tmp = g_SendFreeList;
		g_SendFreeList = tmp->next;
	}
	sendlist_unlock();
	return tmp;
}

/*************************************************
  Function:    		add_sendnode_to_freelist
  Description: 		把节点添加到发送空闲链表
  Input: 			
  	1.node			发送节点
  Output:			无
  Return:			
  Others:
*************************************************/
static void add_sendnode_to_freelist(PSEND_PACKET node)
{
	if (node == NULL)
	{
		return;
	}

	sendlist_lock();
	if (g_SendFreeList == NULL)
	{
		g_SendFreeList = node;
		g_SendFreeList->next = NULL;
	}
	else
	{
		node->next = g_SendFreeList;
		g_SendFreeList = node;
	}
	sendlist_unlock();
}

/*************************************************
  Function:    		get_recvnode_from_freelist
  Description: 		从接收空闲链表中获取节点
  Input: 			无
  Output:			无
  Return:			接收包节点指针
  Others:
*************************************************/
static PRECIVE_PACKET get_recvnode_from_freelist(void)
{
	PRECIVE_PACKET tmp = NULL;
	if (g_RecvFreeList == NULL)
	{
		return NULL;
	}

	recvlist_lock();
	if (g_RecvFreeList->next == NULL)
	{	
		tmp = g_RecvFreeList;
		g_RecvFreeList = NULL;
	}
	else
	{
		tmp = g_RecvFreeList;
		g_RecvFreeList = tmp->next;
	}
	recvlist_unlock();
	return tmp;
}

/*************************************************
  Function:    		add_recvnode_to_freelist
  Description: 		把接收包节点添加到空闲链表
  Input: 			
  	1.node			接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void add_recvnode_to_freelist(PRECIVE_PACKET node)
{
	if (node == NULL)
	{
		return;
	}

	recvlist_lock();
	if (g_RecvFreeList == NULL)
	{
		g_RecvFreeList = node;
		g_RecvFreeList->next = NULL;
	}
	else
	{
		node->next = g_RecvFreeList;
		g_RecvFreeList = node;
	}
	recvlist_unlock();
}

/*************************************************
  Function:    		append_send_packet
  Description: 		加入发送节点到链表
  Input: 
	1.node			插入节点
  Output:			无
  Return:			无
  Others:
*************************************************/
static void append_send_packet(PSEND_PACKET node)
{
	sendlist_lock();
	if (g_SendList == NULL)
	{
		g_SendList = node;
		node->prev = NULL;
	}	
	else
	{
		g_SendLastNode->next = node;
		node->prev = g_SendLastNode;
	}
	g_SendLastNode = node;
	sendlist_unlock();
}

/*************************************************
  Function:    		net_add_send_packet
  Description: 		增加一个发送包到链表
  Input: 
	1.data			发送数据
	2.size			发送数据大小
	3.address		发送地址
	4.port			发送端口
	5.SendTimes		发送次数, 默认发送次数为1
	6.TimeOut		发送超时时间, 默认为TIMEOUT_VALUE
  Output:			无
  Return:			发送包
  Others:
*************************************************/
PSEND_PACKET net_add_send_packet(char * data, int32 size, uint32 address, uint16 port, 
	int32 SendTimes, int32 TimeOut)
{
	PSEND_PACKET sendPacket = NULL;
	if (size > PACKET_NODE_DATA_SIZE)
	{
		log_printf("size err\n");
		return NULL;
	}
	
	sendPacket = get_sendnode_from_freelist();
	if (sendPacket == NULL)
	{
		log_printf("no free send node\n");
		return NULL;
	}	
	memset(sendPacket->data, 0, PACKET_NODE_DATA_SIZE);
	memcpy (sendPacket->data, data, size);
	sendPacket->address = address;					// 对方IP地址
	sendPacket->port = port;						// 对方端口号
	sendPacket->size = size;						// 数据包大小
	sendPacket->SendState = SEND_STATE_SEND_READY;	// 发送状态
	sendPacket->SendTimes = 0; 						// 当前发送次数
	sendPacket->SendTimesMax = SendTimes; 			// 最多发送次数
	sendPacket->TimeOut = TimeOut;
	sendPacket->WaitCount = 0; 						// 等待时间计数
	sendPacket->next = NULL;
	append_send_packet(sendPacket);
	return sendPacket;
}

/*************************************************
  Function:    		remove_send_packet
  Description: 		移除发送包, 不添加到空闲链表
  Input: 
	1.node			要移除的节点
  Output:			无
  Return:			无
  Others:
*************************************************/
static void remove_send_packet(PSEND_PACKET node)
{
	if (node == NULL)
	{
		return;
	}

	sendlist_lock();
	if (g_SendList == node)
	{
		g_SendList = node->next;
		if (g_SendList != NULL)
		{
			g_SendList->prev = NULL;
		}	
		if (g_SendLastNode == node)
		{
			g_SendLastNode = NULL;
		}	
	}
	else if (g_SendLastNode == node)
	{
		g_SendLastNode = g_SendLastNode->prev;
		g_SendLastNode->next = NULL;
	}
	else	
	{
		node->prev->next = node->next; 
		node->next->prev = node->prev;
	}
	node->next = NULL;
	node->prev = NULL;
	sendlist_unlock();
}

/*************************************************
  Function:    		net_del_send_packet
  Description: 		从发送链表移除一个网络包, 加到空闲链表
  Input: 
	1.node			发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_del_send_packet(PSEND_PACKET node)
{
	if (node == NULL)
	{
		return;
	}
	add_sendnode_to_freelist(node);
}

/*************************************************
  Function:    		find_send_packet
  Description:		从发送列表中找一包发送包
  Input: 			无
  Output:			无
  Return:			发送包
  Others:
*************************************************/
static PSEND_PACKET find_send_packet(void)
{
	PSEND_PACKET node = g_SendList;
	
	while (node != NULL)
	{
		if (node->SendState == SEND_STATE_SEND_READY)  // 符合条件
		{
			break;
		}
		node = node->next;
	}
	return node;
}

/*************************************************
  Function:    		net_send_proc
  Description:		发送线程处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_send_proc(void *arg)  
{  
	while (g_SendRunning)
	{
		// 查找一包可以发送的包
		PSEND_PACKET sendPacket = find_send_packet();
		if (sendPacket != NULL)
		{
			sendPacket->SendState = SEND_STATE_SENDING;
			net_send_cmd_packet(sendPacket->data, sendPacket->size, sendPacket->address, sendPacket->port);
			sendPacket->SendTimes++;
			sendPacket->SendState = SEND_STATE_WAIT_ECHO;
			usleep(10*1000);
		}
		else
		{
			usleep(500*1000);
			// 此处考虑是否加入信号量判断
		}
	}	
	pthread_exit(NULL);
} 

/*************************************************
  Function:    		net_timeout_proc
  Description:		超时线程处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_timeout_proc(void *arg)
{
	PSEND_PACKET node;
	PSEND_PACKET temp;
	while (g_SendRunning)
	{
		node = g_SendList;
		
		if (node != NULL)
		{
			while (node != NULL)
			{
				if (node->SendState == SEND_STATE_WAIT_ECHO)  			// 等待应答
				{
					node->WaitCount++; 
					if (node->WaitCount >= node->TimeOut) 				// 应答超时
					{
						if (node->SendTimes < node->SendTimesMax)  		// 准备重发
						{
							node->WaitCount = 0;
							node->SendState = SEND_STATE_SEND_READY;
						}
						else
						{
							node->SendState = SEND_STATE_ECHO_TIMEOUT; 	// 应答超时, 取消发送
							temp = node;
							node = node->next;
							if (NULL != g_SendTimeoutFunc)
							{
								g_SendTimeoutFunc(temp);				// 通知发送方
							}	
							remove_send_packet(temp);
							net_del_send_packet(temp);
							continue; 
						}
					}
				}
				node = node->next;
			}

			sleep(1);
		}
		else
		{
			sleep(1);
			// 此处考虑是否加入信号量判断
		}
	}
	
	pthread_exit(NULL);
}

/*************************************************
  Function:    		net_add_recive_packet
  Description:		接收网络数据包线程执行函数
  Input: 			
  	1.data			数据
  	2.size			大小
  	3.address		地址
  	4.port			端口号
  Output:			无
  Return:			无
  Others:			添加一个接收包到接收链表
*************************************************/
void net_add_recive_packet(char *data, uint32 size, uint32 address, uint16 port)
{
	PRECIVE_PACKET recivePacket = NULL;

	if (0 == size || data == NULL || size > PACKET_NODE_DATA_SIZE)
	{
		return;
	}
	recivePacket = get_recvnode_from_freelist();
	if (NULL == recivePacket)
	{
		return;
	}
	
 	recivePacket->address = address;
 	recivePacket->port = port;
 	recivePacket->size = size;
 	memcpy (recivePacket->data, data, size);
 	recivePacket->next = NULL;

 	recvlist_lock();
 	if (NULL == g_ReciveList)
	{
		g_ReciveLastNode = recivePacket;
		g_ReciveList = recivePacket;
	}
	else
	{
		g_ReciveLastNode->next = recivePacket;
		g_ReciveLastNode = recivePacket;
	}
	recvlist_unlock();
}

/*************************************************
  Function:    		net_get_recive_packet
  Description:		读取一个接收包, 从接收链表中移除该节点
  Input: 			无
  Output:			无
  Return:			接收包
  Others:
*************************************************/
PRECIVE_PACKET net_get_recive_packet(void)
{
	PRECIVE_PACKET node;
	if (NULL == g_ReciveList)
	{
		return NULL;
	}

	recvlist_lock();
	node = g_ReciveList;
	if (g_ReciveLastNode == g_ReciveList)
	{
		g_ReciveList = NULL;
		g_ReciveLastNode = NULL;
	}
	else
	{
		g_ReciveList = g_ReciveList->next;
	}
	recvlist_unlock();
	node->next = NULL;
	return node;
}

/*************************************************
  Function:    		net_del_recive_packet
  Description:		把一个接收包添加到空闲链表
  Input: 			
	1.node			包节点
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_del_recive_packet(PRECIVE_PACKET node)
{
	if (NULL == node)
	{
		return;
	}
	 
	add_recvnode_to_freelist(node);
}

/*************************************************
  Function:    		net_get_sendpacket_by_responsion
  Description:		根据应答包头,获得发送包
  Input: 
	1.netHead		应答包头
  Output:			无
  Return:			发送包
  Others:
*************************************************/
PSEND_PACKET net_get_sendpacket_by_responsion(PNET_HEAD netHead)
{
	PSEND_PACKET node = g_SendList;
	
	while (node != NULL)
	{
		if (node->SendState == SEND_STATE_WAIT_ECHO && 
			((PNET_HEAD)((node->data) + MAIN_NET_HEAD_SIZE))->PackageID == netHead->PackageID)  // 符合条件
		{
			remove_send_packet(node);	
			return node;
		}
		node = node->next;
	}
	return NULL;	
}

/*************************************************
  Function:    		net_set_send_timeout_echofunc
  Description:		设置发送超时应答回调函数
  Input: 
	1.func			发送超时应答函数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_send_timeout_echofunc(PFSEND_TIMEOUT_ECHO func)
{
	g_SendTimeoutFunc = func;
}

/*************************************************
  Function:    		start_comm_thread
  Description:		启动网络线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_start_comm_thread(void)
{
 	g_SendRunning = TRUE ;
 	g_ReciveRunning = TRUE ;
 	pthread_create(&SendThread, NULL, net_send_proc, NULL);
 	pthread_create(&TimeOutThread, NULL, net_timeout_proc, NULL);
 	pthread_create(&ReciveThread, NULL, recive_cmd_data_proc, NULL);
	pthread_create(&MulTimeOutThread, NULL, net_recive_mult_timeout_proc, NULL);
}

/*************************************************
  Function:    		stop_comm_thread
  Description:		停止网络线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_comm_thread(void)
{
	g_SendRunning = FALSE ;
	g_ReciveRunning = FALSE ;

	pthread_join(ReciveThread, NULL);
	pthread_join(TimeOutThread, NULL);
	pthread_join(SendThread, NULL);
	list_sem_del();
}

