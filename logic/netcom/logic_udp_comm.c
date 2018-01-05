/*************************************************
  Copyright (C), 2009-2012, Aurine
  File name: 	logic_udp_comm.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  UDP通讯
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "logic_udp_comm.h"
//#include "rockmsg.h"

#define RECV_BUF_LEN			(240 * 1024)		// UDP接收缓存大小, 暂时无用
#define SEND_BUF_LEN			(240 * 1024)		// UDP发送缓存大小, 暂时无用

static int32 g_CmdSocketFd = -1;	 				// 网络命令Socket

static struct sockaddr_in g_DestAddress; 			// 发送地址
static struct sockaddr_in g_ReciveAddress; 			// 接收地址
static char g_ReciveData[NET_PACKBUF_SIZE]; 		// 网络接收包数据
static int g_udpcomm= FALSE;						//通讯是否运行

#if (_SUB_PROTOCOL_ENCRYPT_ != 0x00)
static char g_EncryptBuf[NET_PACKBUF_SIZE];	  		// 发送Buf 加密后数据
#endif
static uint8 g_SetMACFlag = FALSE;           		// add by caogw 20160418 更改MAC烧写工具默认ML协议

/**************************下面为特殊包处理接口************************/
static PFRECIVE_PACKET_PROC g_RecvMultPacketProc = NULL;	// 多包接收处理函数
static PFRECIVE_PACKET_PROC g_RecvDNSPacketProc = NULL;		// DNS包接收处理函数


NET_COMMAND net_get_cmd_by_nethead(const PNET_HEAD netHead);

/*************************************************
  Function:		net_set_mac
  Description:	mac码工具设置mac处理
  Input: 
    1.data		接收数据
    2.len		数据长度
  Output:		无
  Return:       无  
  Others:   	MAC烧写工具默认ML协议
*************************************************/
static void net_set_mac(char *data, int32 len)
{   
	uint8 mac[6] = {0}; 
    RECIVE_PACKET RecPacket;
	
    g_SetMACFlag = TRUE;
    RecPacket.data = data;
    RecPacket.size = len;
    RecPacket.address = ntohl(g_ReciveAddress.sin_addr.s_addr);
    RecPacket.port = ntohs(g_ReciveAddress.sin_port);
                   
    memcpy(mac, data+NET_HEAD_SIZE, 6);
    int32 ret = net_set_local_mac(mac);
    if (ret == TRUE)
    {
        net_send_echo_packet(&RecPacket, ECHO_OK, 0, 0);
    }
    else
    {
        net_send_echo_packet(&RecPacket, ECHO_ERROR, 0, 0);
    }
    g_SetMACFlag = FALSE;
}

/*************************************************
  Function:    		create_udpsocket
  Description:		建立udpsocket
  Input: 
	1.port			端口号
	2.RecBufSize	接收Buf大小
	3.SendBufSize	发送Buf大小
  Output:			无
  Return:			TRUE / FALSE
  Others:
*************************************************/
static int32 create_cmd_socket(uint16 port, int32 RecBufSize, int32 SendBufSize)
{
	int32 socketFd;
	int32 ret;
	int32 flag;
	struct sockaddr_in addrLocal;

	socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketFd < 0)
    {
    	log_printf("get socket err\n");
        return FALSE;
    }
    else
    {
    	log_printf("get socket : %d\n", socketFd);
    }

#if 0		// lwip-v1.0.1的库设置不成功	
	ret = setsockopt(socketFd, SOL_SOCKET, SO_RCVBUF, (char*)&RecBufSize, sizeof(RecBufSize)) ;
	log_printf("set socket SO_RCVBUF ret : %d\n", ret);
    ret = setsockopt(socketFd, SOL_SOCKET, SO_SNDBUF, (char*)&SendBufSize, sizeof(SendBufSize)) ;
    log_printf("set socket SO_SNDBUF ret : %d\n", ret);
#endif    

    flag = 1;
	ret = setsockopt(socketFd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));
	// 防止服务器出错时，端口未被释放，导致重新连接失败
	ret = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
		
    memset(&addrLocal, 0, SOCK_ADDR_SIZE);    
    addrLocal.sin_family      = AF_INET;
	addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    addrLocal.sin_port        = htons(port);
    
    if (bind(socketFd, (struct sockaddr *)&addrLocal, SOCK_ADDR_SIZE))
    {
    	close(socketFd);
		perror("bind");
    	log_printf("bind socket err\n");
    	return FALSE;
    }
    g_CmdSocketFd = socketFd;
    return TRUE;
}

#if (_SUB_PROTOCOL_ENCRYPT_ != 0x00)
/*************************************************
  Function:    		data_encrypt
  Description:		数据加密解密 异或
  Input: 
	1.buf			数据
	2.len			数据长度
  Output:			
  	1.buf			加密、解密出来的数据
  Return:			成功与否
  Others:
*************************************************/
void data_encrypt( char *buf, int len )
{
	int i;
	char *pKey = ENCRYPT_KEY;
	char key;
	log_printf("ENCRYPT_KEY : %s, ENCRYPT_KEY_LEN: %d\n", ENCRYPT_KEY, ENCRYPT_KEY_LEN);

	for( i = 0; i < len; i ++ )
	{
		key = pKey[ i % ENCRYPT_KEY_LEN];
		buf[i] = buf[i] ^ key;
	}
}
#endif


/*************************************************
  Function:    		check_hwEncrypt
  Description:		检查主包头数据
  Input: 
	1.PMainNethead			校验的数据
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
static int32 check_MainNethead(PMAIN_NET_HEAD PMainNethead)
{
	#ifndef _AU_PROTOCOL_
	uint32 U32Encrypt = 0;
	uint8 hwEncrypt[16] = {0};
	
	memcpy(hwEncrypt, PMainNethead->hwEncrypt, sizeof(PMainNethead->hwEncrypt));
	memcpy(&U32Encrypt, &hwEncrypt[12], 4);
	log_printf("U32Encrypt : %08x\n", U32Encrypt);
	log_printf("subProtocolEncrypt : %02x\n", PMainNethead->subProtocolEncrypt);
	
	// 米立版本可以兼容米立老版本,其他的版本只能跟自己通讯
	if ((_HW_ENCRYPT_ == _ML_HWENCRYPT_ && U32Encrypt != 0 && U32Encrypt != _ML_HWENCRYPT_) || 
		(_HW_ENCRYPT_ != _ML_HWENCRYPT_ && U32Encrypt != _HW_ENCRYPT_))
	{
		log_printf("MainNethead hwEncrypt error!!!!\n ");
		return FALSE;
	}

	// 加密类型不对 返回错误
	if (PMainNethead->subProtocolEncrypt != 0X00 && PMainNethead->subProtocolEncrypt != _SUB_PROTOCOL_ENCRYPT_)
	{
		log_printf("MainNethead subProtocolEncrypt error!!!!\n ");
		return FALSE;
	}
	return TRUE;
	#endif
}

/*************************************************
  Function:    		check_packet
  Description:		检查包是否合法
  Input: 
	1.data			校验的数据
	2.size			待校验的数据长度
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
static int32 check_packet(const char * data, int32 size)
{
	int32 ret = FALSE;
	PNET_HEAD nethead = NULL;
	PMAIN_NET_HEAD PMainNethead = NULL;
	
	if (size > NET_PACKBUF_SIZE || size < NET_HEAD_SIZE)
	{	
		log_printf("size err!\n");
		return FALSE;
	}	
	
	nethead = (PNET_HEAD)(data + MAIN_NET_HEAD_SIZE);
	#ifndef _AU_PROTOCOL_
	PMainNethead = (PMAIN_NET_HEAD)(data);
	ret = check_MainNethead(PMainNethead);
	if (ret == FALSE)
	{
		log_printf("MainNethead err !!!!\n");
		return FALSE;
	}
	#endif
	
	// 长度
	if (nethead->DataLen != (size-NET_HEAD_SIZE))
	{	
		log_printf("data len err : size: %d, datalen: %d\n", size, nethead->DataLen);
		return FALSE;
	}	
	
	// 版本	
	if (nethead->ProtocolVer != _PROTOCOL_VER_)
	{
		log_printf("protocol ver err!  nethead->ProtocolVer : %x\n", nethead->ProtocolVer);
		return FALSE;	
	}	
	
	// 方向
	if (nethead->DirectFlag != DIR_SEND && nethead->DirectFlag != DIR_ACK)
	{
		log_printf("direct flg err!\n");
		return FALSE;	
	}
	
	return TRUE; 
}

/*************************************************
  Function:    		recive_cmd_data_proc
  Description:		接收UDP网络命令
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* recive_cmd_data_proc(void *arg)
{
	uint32 ret;
	int32 len = 0;
	int32 addr_len = SOCK_ADDR_SIZE;
	NET_COMMAND cmd = 0;
	RECIVE_PACKET RecPacket;
	PNET_HEAD head;
	
	#ifndef _AU_PROTOCOL_
    PMAIN_NET_HEAD PMainNethead = NULL; // modify by caogw  20160418
    #endif

	#if (_SUB_PROTOCOL_ENCRYPT_ != 0x00)
	char *encrypt_databuf = NULL;
    //PMAIN_NET_HEAD PMainNethead = NULL; // modify by caogw  20160418
	#endif

	//static int32 RecvPacketCount = 0;				// 测试接收包个数	
	
	if (g_CmdSocketFd >= 0)
	{
		while (1)
		{
			len = recvfrom(g_CmdSocketFd, g_ReciveData, sizeof(g_ReciveData), 0, (struct sockaddr *)&g_ReciveAddress, &addr_len);
			if (len <= 0)
			{
				continue;
			}
			else
			{			
				// 测试接收包个数
				#if 0	
				RecvPacketCount++;	
				if ((RecvPacketCount%100==0) && (RecvPacketCount >= 100))
				{
					log_printf("len:%d, RecvPacketCount:%d, addr:0x%x, port:%d\n", len, RecvPacketCount, ntohl(g_ReciveAddress.sin_addr.s_addr), ntohs(g_ReciveAddress.sin_port));
				}	
				net_send_cmd_packet(g_ReciveData, 100, ntohl(g_ReciveAddress.sin_addr.s_addr), ntohs(g_ReciveAddress.sin_port));
			
				break;
				#endif		
			}

			log_printf("***************** recv packet len : %d *******************  \n", len);
			#ifndef _AU_PROTOCOL_
            PMainNethead = (PMAIN_NET_HEAD)(g_ReciveData);
            if (PMainNethead->subProtocolEncrypt == _NO_PROTOCOL_ENCRYPT_)
            {
                head = (PNET_HEAD)(g_ReciveData + MAIN_NET_HEAD_SIZE);
                cmd = net_get_cmd_by_nethead(head);
                if (cmd == CMD_SET_MAC)
                {
                    net_set_mac(g_ReciveData, len);
                    continue;
                }
            }
            g_SetMACFlag = FALSE;
			#endif
			
			// 数据解密
			#if (_SUB_PROTOCOL_ENCRYPT_ != 0x00)
			encrypt_databuf = (char *)g_ReciveData + MAIN_NET_HEAD_SIZE;
			PMainNethead = (PMAIN_NET_HEAD)(g_ReciveData);
			log_printf("recv len : %d , PMainNethead->subPacketLen: %d\n", len, PMainNethead->subPacketLen);
			data_encrypt(encrypt_databuf, (len - MAIN_NET_HEAD_SIZE));
			#endif
			
			// 过滤包, 检查是否合法	
			if (!check_packet(g_ReciveData, len))
			{
				continue;
			}

			// dns广播包和多包接收直接处理,不进入队列
		#if 1
			RecPacket.data = g_ReciveData;
			RecPacket.size = len;
			RecPacket.address = ntohl(g_ReciveAddress.sin_addr.s_addr);
			RecPacket.port = ntohs(g_ReciveAddress.sin_port);

			head = (PNET_HEAD)(g_ReciveData + MAIN_NET_HEAD_SIZE);
			cmd = net_get_cmd_by_nethead(head);

			log_printf("recive_cmd_data_proc: %04x\n", cmd);
			switch (cmd)
			{											
				case CMD_SEND_MAXDATA:					// 多包发送
				case CMD_STOP_SEND_MAXDATA: 			// 中断多包发送
					if (g_RecvMultPacketProc != NULL)	// 如果有设置多包发送接收函数
					{
		 				g_RecvMultPacketProc(cmd, &RecPacket);
					}
					break;
					 
				case CMD_DNS_GETIP:						// DNS广播包
					if (g_RecvDNSPacketProc != NULL)
					{
						g_RecvDNSPacketProc(cmd, &RecPacket);
					}
					break;
					
				case CMD_SET_MAC:						// 设置MAC
				{					
					uint8 mac[6] = {0};					
					memcpy(mac, g_ReciveData+NET_HEAD_SIZE, 6);
					ret = net_set_local_mac(mac);
					if (ret == TRUE)
					{
						net_send_echo_packet(&RecPacket, ECHO_OK, 0, 0);
					}
					else
					{
						net_send_echo_packet(&RecPacket, ECHO_ERROR, 0, 0);
					}
					break;
				}

				#ifdef _ADD_BLUETOOTH_DOOR_
				case CMD_NETDOOR_SNAP_REPORT:
				{	
					recv_bluetoothdoor_pic_start(&RecPacket);
					break;
				}

				case CMD_REQ_SYN_TIME:
					{
						ZONE_DATE_TIME LocalDateTime = {0};
						DATE_TIME SendTime = {0};
						get_timer(&LocalDateTime);
						get_utc_time(&LocalDateTime, 8);

						SendTime.year = LocalDateTime.year;
						SendTime.month = LocalDateTime.month;
						SendTime.day = LocalDateTime.day;
						SendTime.hour = LocalDateTime.hour;
						SendTime.min = LocalDateTime.min;
						SendTime.sec = LocalDateTime.sec;
						SendTime.week = LocalDateTime.week;

						net_send_echo_packet(&RecPacket, ECHO_OK, &SendTime, sizeof(DATE_TIME));
					}
					break;
				#endif

				// 增加布防、撤防、局防操作同步到分机 add by luofl 2011-4-27
				#ifdef _ALARM_OPER_SYNC_
				case CMD_SYNC_SET_EVENT:
					ret = alarm_check_devno(&RecPacket);
					if (ret == TRUE)
					{
						alarm_set_response();		// 布防操作同步到分机
					}
					break;

				case CMD_SYNC_UNSET_EVENT:
					ret = alarm_check_devno(&RecPacket);
					if (ret == TRUE)
					{
						alarm_unset_response();		// 撤防操作同步到分机
					}
					break;

				case CMD_SYNC_PARTSET_EVENT:
					ret = alarm_check_devno(&RecPacket);
					if (ret == TRUE)
					{
						alarm_partset_response();	// 局防操作同步到分机
					}
					break;
				#endif
				
				default:							// 其他命令, 发消息至网络命令分发线程
					net_add_recive_packet(g_ReciveData, len, RecPacket.address, RecPacket.port);
		
					break;	
			}
		#else	// dns广播包, 多包接收放在接收线程处理
			net_add_recive_packet(g_ReciveData, len, ntohl(g_ReciveAddress.sin_addr.s_addr), ntohs(g_ReciveAddress.sin_port));
			//RockOSSendMsg(MB_NETDIST, AS_NET_RECV_PACKET_DISTRIBUTE, NULL);
		#endif	
		}
		
		pthread_exit(NULL);
		return NULL;
	}
	
	return NULL;
}


/*************************************************
  Function:    		udp_get_cmd_by_nethead
  Description:		获得命令值(含子系统代号)
  Input: 
	1.netHead		网络包头
  Output:			无
  Return:			网络命令
  Others:
*************************************************/
NET_COMMAND net_get_cmd_by_nethead(const PNET_HEAD netHead)
{
	return ((netHead->SubSysCode<<8)&0xFF00) | netHead->command;
}

/*************************************************
  Function:    		net_send_cmd_packet
  Description:		发送cmd包
  Input: 
	1.data			发送数据
	2.size			数据大小
	3.address		发送地址
	4.port			发送端口
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
int32 net_send_cmd_packet(char * data, int32 size, uint32 address, uint16 port)
{
	fd_set fdWrite; 
	int32 ret = 0;
	int32 SendSize = 0;
	
	if (g_CmdSocketFd < 0)
	{
		return FALSE;
	}	
	if (address == 0)
	{
		return FALSE;
	}
	
	g_DestAddress.sin_addr.s_addr = htonl(address);
  	g_DestAddress.sin_port        = htons(port);
    
	/*
	struct timeval timeout;  
	timeout.tv_sec  = 0 ;
	timeout.tv_usec = 0;
	*/

	FD_ZERO(&fdWrite); 
	FD_SET(g_CmdSocketFd, &fdWrite);
	ret = select(FD_SETSIZE, NULL, &fdWrite, NULL, NULL);
	
	if (ret > 0)
	{
		#if (_SUB_PROTOCOL_ENCRYPT_ != 0x00)  		// 数据需要加密
        if (data != NULL && size != 0 && g_SetMACFlag == FALSE)//modify by wangzc 20160413 更改MAC烧写工具默认ML协议
		{
			char *encrypt_databuf = NULL;
			memset(g_EncryptBuf, 0, sizeof(g_EncryptBuf));
			memcpy(g_EncryptBuf, data, size);
			encrypt_databuf = (char *)g_EncryptBuf + MAIN_NET_HEAD_SIZE;
			data_encrypt(encrypt_databuf, (size-MAIN_NET_HEAD_SIZE));
			SendSize = sendto(g_CmdSocketFd, g_EncryptBuf, size, 0, (struct sockaddr *)&g_DestAddress, SOCK_ADDR_SIZE);	
		}
		else
		#endif
		SendSize = sendto(g_CmdSocketFd, data, size, 0, (struct sockaddr *)&g_DestAddress, SOCK_ADDR_SIZE);	
		//usleep(10);
	}

	if (SendSize == size)
	{	
		log_printf("net_send_cmd_packet : size:  %d \n",size);
		return TRUE;
	}	
	else
	{
		log_printf("send err: ret: %d\n", SendSize);
		return FALSE;
	}
}

/*************************************************
  Function:    		net_start_udp_comm
  Description:		启动UDP通讯
  Input: 			无
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
int32 net_start_udp_comm(void)
{
	int32 ret; 
	g_DestAddress.sin_family      = AF_INET ;
	g_DestAddress.sin_addr.s_addr = 0;
    g_DestAddress.sin_port        = 0; 
    memset(&(g_DestAddress.sin_zero), 0, 8);
	ret = create_cmd_socket(NETCMD_UDP_PORT, RECV_BUF_LEN, SEND_BUF_LEN);
	if (ret)
	{
		net_start_comm_thread();
		g_udpcomm = TRUE ;
	 	return TRUE;
	}
	return FALSE;
}

/*************************************************
  Function:    		net_stop_udp_comm
  Description:		停止UDP通讯
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_udp_comm(void)
{
	if (FALSE == g_udpcomm)
		return;
	
	net_stop_comm_thread();
	
	if (g_CmdSocketFd >= 0)
	{
		close(g_CmdSocketFd);
		g_CmdSocketFd = -1;
	}
	
	g_udpcomm = FALSE;
}


/*************************************************
  Function:    		net_set_mult_packet_recive
  Description: 		设置接收多包接收处理函数
  Input: 
	1.func			多包接收处理函数
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_mult_packet_recive(PFRECIVE_PACKET_PROC func)
{
	g_RecvMultPacketProc = func;
}

/*************************************************
  Function:    		net_set_dns_packet_recive
  Description: 		设置DNS包发送处理函数
  Input: 
	1.func			多包接收处理函数
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_dns_packet_recive(PFRECIVE_PACKET_PROC func)
{
	g_RecvDNSPacketProc = func;
}

