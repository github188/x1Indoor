/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_dianti.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-12-18
  Description:  
				电梯控制逻辑,配米立的电梯控制器
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"
#include "logic_include.h"

#ifdef _USE_ELEVATOR_

// mody by caogw 15-05-08 为了解决设备编号过长时
// 层号房号获取错误的问题(固定4位房号)
/*************************************************
  Function:		get_floor
  Description:	获取层号
  Input:		无
  Output:		无	
  Return:		层号
  Others:	
*************************************************/
static uint8 get_floor(void)
{
	char tmp[2];
	int floorno = -1;
	
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint8 StairNoLen = dev->Rule.StairNoLen;
	tmp[0] = dev->DeviceNoStr[StairNoLen];
	tmp[1] = dev->DeviceNoStr[StairNoLen+1];
	floorno = atoi(tmp);
	
	return floorno;
}

/*************************************************
  Function:		get_room
  Description:	获取房号
  Input:		无
  Output:		无	
  Return:		未2位房号
  Others:	
*************************************************/
static uint8 get_room(void)
{
	char tmp[2];
	int roomno = -1;
	
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint8 StairNoLen = dev->Rule.StairNoLen;
	tmp[0] = dev->DeviceNoStr[StairNoLen+2];
	tmp[1] = dev->DeviceNoStr[StairNoLen+3];
	roomno = atoi(tmp);
	
	return roomno;
}

#ifdef _DIANTI_SHUANKAI_
/*************************************************
  Function:		get_room_flag
  Description:	判断房号奇偶标志
  Input:		无
  Output:		无	
  Return:		未2位房号
  Others:	
*************************************************/
static uint8 get_room_flag(void)
{
	uint8 flag;
	char tmp[2];
	int roomno = -1;
	
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint8 StairNoLen = dev->Rule.StairNoLen;
	tmp[0] = dev->DeviceNoStr[StairNoLen+2];
	tmp[1] = dev->DeviceNoStr[StairNoLen+3];
	roomno = atoi(tmp);
	flag = roomno % 2;
	
	return flag;
}
#endif

/*************************************************
  Function:		get_dianti_ip
  Description:	获取电梯控制器IP地址
  Input:		无
  Output:		无	
  Return:		IP地址
  Others:	
*************************************************/
uint32 get_dianti_ip(void)
{
	uint32 ip;

	ip = storage_get_netparam_bytype(DIANTI_IPADDR);
	return ip;
}

/*************************************************
  Function:		dianti_set_cmd
  Description:	发送命令给电梯控制器
  Input:
  Output:
  Return:		无
  Others:		无	
*************************************************/
void dianti_set_cmd(uint8 direction)
{
	int32 sockFd;
	uint32 ipAddr;
	uint8 size;
	uint8 cmdbuf[5] = {0x02, 0x05, 0x01, 0x00, 0x00};
	struct sockaddr_in destAddr;
   	
	sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockFd == -1)
	{
  		log_printf("socket err\n");
  		return;
	}

	#ifdef _DIANTI_SHUANKAI_						// add by luofl 2011-08-11 电梯双开门
	if (get_room_flag())
	{
		cmdbuf[1] = ELEVATOR_DOWN;					
	}
	else
	{
		cmdbuf[1] = ELEVATOR_UP;
	}
	#else
	cmdbuf[1] = direction;
	#endif
	
	cmdbuf[2] = get_floor();						// 层号
	cmdbuf[3] = get_room();							// 房号

	// 设置连接的地址、端口
	ipAddr = get_dianti_ip();
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = htonl(ipAddr);		// 要进行网络字节顺序转换, 低字节在前,高字节在后
	destAddr.sin_port = htons(DIANTI_UDP_PORT);
	size = sendto(sockFd, cmdbuf, sizeof(cmdbuf), 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr));	
	if (size != sizeof(cmdbuf))
	{
		log_printf("sendto error!!! size: %d\n", size);
		// 考虑增加重发机制
	}
	usleep(500*1000);
	sendto(sockFd, cmdbuf, sizeof(cmdbuf), 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr));
	close(sockFd);

	#if 0
	uint8 i;
	for (i = 0; i < sizeof(cmdbuf); i++)
	{
		log_printf("elevator cmdbuf[%d]:  %d\n", i, cmdbuf[i]);
	}
	log_printf("\n");
	#endif
	
  	return;
}

/*************************************************
  Function:		dianti_rec_cmd
  Description:	接收电梯控制器的数据
  Input:		无
  Output:		无	
  Return:		无
  Others:	
*************************************************/
void dianti_rec_cmd(void)
{	
	#if 0
	uint8 recbuf[50] = {0};
	int32 sock;
  	struct sockaddr_in * name;
  	int32 recvlen, sendlen;
  
  	// 申请socket，属性为UDP连接，类型为SOCK_DGRAM
  	// 注意: 这里的类型SOCK_DGRAM，与TCP socket的类型SOCK_DGRAM不同!
  	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  	if(sock == -1) 
	{
    	log_printf("socket err\n");
    	return;
  	}

  	// 为bind做准备
  	name = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
  	if(name == NULL) 
	{
    	log_printf("malloc name fail\n");
    	return;
  	}
  	name->sin_family = AF_INET;
  	name->sin_addr.s_addr = 0x180d6e0a;
  	name->sin_port = 0x0700;

  	// 绑定端口
  	bind(sock, (struct sockaddr*)name, sizeof(struct sockaddr));

  	while(1) 
	{
    	log_printf("udpecho_server : recv begin !!\n");
    	recvlen = recv(sock, recbuf, 256, 0);
		log_printf("Recvlen=%d,str:%s\n", recvlen, recbuf);
    	if (recvlen > 0) 
		{
        	do 
			{
          		sendlen=send(sock, recbuf, recvlen, 0);
          		if (sendlen < 0)
          		{
            		log_printf("sendlen == -1,error\n");
           			break;
          		}
          		recvlen -= sendlen;
    		} while(recvlen > 0);
    	}
    	else 
		{
		}
  	}
  	log_printf("Got EOF, looping\n");
	#endif
}

// 上海领科电梯控制
#ifdef _ALAV_LINGKE_
/*************************************************
  Function:		dianti_set_cmd_custom
  Description:	发送命令给电梯控制器
  Input:
  Output:
  Return:		无
  Others:		无	
*************************************************/
void dianti_set_call_custom(char* stairno)
{
	int32 sockFd;
	uint32 ipAddr;
	uint8 floor, size;
	uint8 cmdbuf[5] = {0x02, 0x05, 0x01, 0x00, 0x00};
	struct sockaddr_in destAddr;

	PFULL_DEVICE_NO fulldevno;

	uint8 StairNoLen;   
	fulldevno = storage_get_devparam();
	StairNoLen = fulldevno->Rule.StairNoLen;

	log_printf("dianti_set_call_custom: %s\n", stairno);
	sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockFd == -1)
	{
  		log_printf("socket err\n");
  		return;
	}
	
	cmdbuf[1] = ELEVATOR_CALL;
	
	cmdbuf[2] = get_floor();
	if (stairno != NULL)
	{
		cmdbuf[3] = atoi(stairno+StairNoLen);
	}
	else
	{
		cmdbuf[3] = 0;
	}
	
	ipAddr = get_dianti_ip();
	// 设置连接的地址、端口
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = htonl(ipAddr);		// 要进行网络字节顺序转换, 低字节在前,高字节在后
	destAddr.sin_port = htons(DIANTI_UDP_PORT);
	size = sendto(sockFd, cmdbuf, sizeof(cmdbuf), 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr));	
	if (size != sizeof(cmdbuf))
	{
		log_printf("sendto error!!! size: %d\n", size);
		// 考虑增加重发机制
	}
	usleep(500*1000);
	sendto(sockFd, cmdbuf, sizeof(cmdbuf), 0, (struct sockaddr *)&destAddr, sizeof(struct sockaddr));
	close(sockFd);

	log_printf("elevator direct: %d\n", cmdbuf[1]);
	log_printf("elevator floor: %d\n", cmdbuf[2]);
	log_printf("elevator room: %d\n", cmdbuf[3]);
  	return;
}
#endif
#endif

