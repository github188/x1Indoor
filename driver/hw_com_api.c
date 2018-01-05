/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	hw_com_api.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-09-30
  Description:  串口驱动程序
  				串口1---用于调试和与单片机通信共用
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <stdio.h>				// printf
#include <fcntl.h>				// open
#include <string.h>				// bzero
#include <stdlib.h>				// exit
#include <sys/times.h>			// times
#include <sys/types.h>			// pid_t
#include <termios.h>				// termios, tcgetattr(), tcsetattr()
#include <unistd.h>
#include <sys/ioctl.h>          	// ioctl
//#include <linux/delay.h>
#include <pthread.h>

#include "AppConfig.h"
#include "au_types.h"
#include "hw_ioctrl_api.h"
#include "hw_com_api.h"
#include "storage_include.h"

#define	TTY_DEV						"/dev/ttyS"				// 端口路径
#define TIMEOUT_SEC(buflen, baud) 	(buflen*20 / baud+2)  	// 接收超时
#define TIMEOUT_USEC 				0
#ifdef _JD_MODE_
#define MAX_MONOTOR_DOG_TIME		1000
#else
#define MAX_MONOTOR_DOG_TIME		10
#endif

#if 0
#define MAX_RECV_SIZE			50
#define MAX_RECV_INDEX			3

#define RECV_START_CODE			0xAA				// 起始位
#define RECV_DATA_LEN_POS		2					// 数据长度所在位置 从0开始
#define RECV_DATA_START_LEN		2					// 数据长度之前的字节
#define RECV_DATA_END_LEN		2					// 数据长度之后的字节 加校验字

#define RECV_START				0					// 查找帧头状态
#define RECV_END				1					// 等待接收结束

static uint8 g_RecvData[MAX_RECV_SIZE];
static uint8 g_RecvNum = 0;
static uint8 g_RecvDataLen = 0;
static uint8 g_RecvState = RECV_START;
#endif

static int32 FdCom1 = -1;
static uint8 FeetDog = 0;
static uint8 InitAlarmFlag = 0;
static uint8 InitJdFlag = 0;
static uint8 AnsFlag = 0;
static uint8 Port1Init = 0;
static uint8 g_MonitorFlag = 0;
static uint32 g_MonitorCnt = 0;

static AlarmCallBack AlarmCallBackFunc = NULL;
static JdCallBack JdCallBackFunc = NULL;

/*************************************************
  Function:		get_ptty
  Description: 	获得端口名称
  Input: 		
	1.			端口信息
  Output:		无
  Return:		端口名称
  Others:
*************************************************/
static char * get_ptty(PPORTINFO_T pportinfo)
{
	char * ptty = NULL;

	switch(pportinfo->tty)
	{
		case '0':
			ptty = TTY_DEV"0";
			break;

		case '1':
			ptty = TTY_DEV"1";
			break;

		case '2':
			ptty = TTY_DEV"2";
			break;
	}
	return(ptty);
}

/*************************************************
  Function:		conv_baud
  Description: 	波特率转换函数
  Input: 
	1.			转换前波特率
  Output:		无
  Return:		转换后波特率
  Others:
*************************************************/
static int32 conv_baud(unsigned long int baudrate)
{
	switch(baudrate)
	{
		case 2400:
			return B2400;

		case 4800:
			return B4800;

		case 9600:
			return B9600;

		case 19200:
			return B19200;

		case 38400:
			return B38400;

		case 57600:
			return B57600;

		case 115200:
			return B115200;

		default:
			return B9600;
	}
}

/*************************************************
  Function:		port1_set
  Description: 	设置端口1属性
  Input: 
	1.			待设置的端口信息
  Output:		无
  Return:		无
  Others:
*************************************************/
static int32 port1_set(const PPORTINFO_T pportinfo)
{
	struct termios termios_old, termios_new;
	int32 baudrate, tmp;
	char databit, stopbit, parity, fctl;

	bzero(&termios_old, sizeof(termios_old));
	bzero(&termios_new, sizeof(termios_new));
	cfmakeraw(&termios_new);
	tcgetattr(FdCom1, &termios_old);				// get the serial port attributions

	// 设置端口属性
	baudrate = conv_baud(pportinfo -> baudrate);
	cfsetispeed(&termios_new, baudrate);			// 填入串口输入端的波特率
	cfsetospeed(&termios_new, baudrate);			// 填入串口输出端的波特率
	//termios_new.c_cflag &= ~CLOCAL;				// 控制模式,程序成为端口的占有者
	termios_new.c_cflag |= CLOCAL;					// 控制模式，保证程序不会成为端口的占有者
	termios_new.c_cflag |= CREAD;					// 控制模式，使能端口读取输入的数据

	// 控制模式，flow control
	fctl = pportinfo-> fctl;
	switch (fctl)
	{
		case '0':
			termios_new.c_cflag &= ~CRTSCTS;		// 无流控
			break;

		case '1':
			termios_new.c_cflag |= CRTSCTS;			// 硬流控
			break;

		case '2':
			termios_new.c_iflag |= IXON | IXOFF | IXANY;	// 软流控
			break;
	}

	// 控制模式，data bits
	termios_new.c_cflag &= ~CSIZE;					// 控制模式，屏蔽字符大小位
	databit = pportinfo -> databit;
	switch (databit)
	{
		case '5':
			termios_new.c_cflag |= CS5;
			break;
			
		case '6':
			termios_new.c_cflag |= CS6;
			break;
			
		case '7':
			termios_new.c_cflag |= CS7;
			break;
			
		default:
			termios_new.c_cflag |= CS8;
			break;
	}

	// 控制模式 parity check
	parity = pportinfo -> parity;
	switch (parity)
	{
		case '0':
			termios_new.c_cflag &= ~PARENB;			// no parity check
			break;
		
		case '1':
			termios_new.c_cflag |= PARENB;			// odd check
			termios_new.c_cflag &= ~PARODD;
			break;
		
		case '2':
			termios_new.c_cflag |= PARENB;			// even check
			termios_new.c_cflag |= PARODD;
			break;
	}

	// 控制模式，stop bits
	stopbit = pportinfo -> stopbit;
	if (stopbit == '2')
	{
		termios_new.c_cflag |= CSTOPB;	 			// 2stop bits
	}
	else
	{
		termios_new.c_cflag &= ~CSTOPB;	 			// 1 stop bits
	}

	// other attributions default
	termios_new.c_oflag &= ~OPOST;					// 输出模式，原始数据输出
	termios_new.c_cc[VMIN]  = 3;					// 控制字符, 所要读取字符的最小数量
	termios_new.c_cc[VTIME] = 1;					// 控制字符, 读取第一个字符的等待时间	unit: (1/10)second

	tcflush(FdCom1, TCIFLUSH);						// 溢出的数据可以接收，但不读
	tmp = tcsetattr(FdCom1, TCSANOW, &termios_new);	// 设置新属性，TCSANOW：所有改变立即生效	tcgetattr(fdcom, &termios_old);
	
	return(tmp);
}

/*************************************************
  Function:		port1_init
  Description: 	端口0初始化
  Input: 		无
  Output:		无
  Return:		0: 成功 其它值: 失败
  Others:
*************************************************/
int32 port1_init(void)
{
	#if DEBUG_PRINT
	return FALSE;
	#endif
	
	if (!Port1Init)
	{
		char * ptty;
		PORTINFO_T portinfo =
		{
			'0',                          				// print prompt after receiving
	 		COM1_BOUND,    	               				// baudrate: 9600
	 		'8',                          				// databit: 8
	 		'0',                          				// debug: off
	 		'0',                          				// echo: off
	 		'0',                          				// flow control: none
	 		'1',                          				// default tty: COM1
	 		'0',                          				// parity: none
	 		'1',                          				// stopbit: 1
	 		 0    	                  					// reserved
		};
		
		ptty = get_ptty(&portinfo);
		// FdCom1 = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
		FdCom1 = open(ptty, O_RDWR | O_NOCTTY);	
		if (FdCom1 < 0)
		{
			log_printf("Error: open serial port1 error.\n");
			return FALSE;
		}
		port1_set(&portinfo);							// 设置串口1的各项属性
		Port1Init = 1;
	}
	
	return TRUE;
}

/*************************************************
  Function:		port1_send
  Description: 	串口1发送
  Input: 
	1.			待发送数据
	2.			数据长度
  Output:		无
  Return:		-1: 失败 成功: 实际发送长度
  Others:
*************************************************/
int32 port1_send(char * data, int32 datalen)
{
	int32 len = 0;

	len = write(FdCom1, data, datalen);				// 实际写入的长度
	if(len == datalen)
	{
		//usleep(30000);
		usleep(5);									// 等待FIFO中的数据发送完毕
		return(len);
	}
	else
	{
		tcflush(FdCom1, TCOFLUSH);					// 写入失败清空FIFO
		return(-1);
	}
}

/*************************************************
  Function:		port1_recv
  Description: 	串口1接收
  Input: 
	1.datalen:	读入的数据长度
  Output:
	1.data		接收到的数据
  Return:		实际收到的字节数
  Others:
*************************************************/
int32 port1_recv(char * data, int32 datalen)
{
	uint8 i;
	int32 readlen, fs_sel;
	fd_set	fs_read;

	FD_ZERO(&fs_read);
	FD_SET(FdCom1, &fs_read);
	fs_sel = select(FdCom1+1, &fs_read, NULL, NULL, 0);	// 阻塞直到接收到数据
	if (fs_sel)
	{
		for (i = 0; i < datalen; i++)
		{
			read(FdCom1, data+i, 1);
			usleep(50);
		}
		tcflush(FdCom1, TCIFLUSH);
		return(datalen);
	}
	else
	{
		tcflush(FdCom1, TCIFLUSH);
		return(-1);
	}
}

/*************************************************
  Function:		port1_recv_jd
  Description: 	串口1接收家电数据
  Input: 
	1.datalen:	读入的数据长度
  Output:
	1.data		接收到的数据
  Return:		实际收到的字节数
  Others:
*************************************************/
int32 port1_recv_jd(char * data)
{
	uint8 i;
	int32 len = 0, fs_sel;
	fd_set fs_read;

	if (FdCom1 < 0)
	{
		return -1;
	}
	
	FD_ZERO(&fs_read);
	FD_SET(FdCom1, &fs_read);
	fs_sel = select(FdCom1+1, &fs_read, NULL, NULL, 0); // 阻塞直到接收到数据
	if (fs_sel)
	{
		len = read(FdCom1, data, 3);
		#if 0
		//uint8 i;
		printf("len = %d\n", len);
		for (i = 0; i < len; i++)
		{
			printf("  %02x", data[i]);
		}
		printf("\n===================\n");
		#endif
		
		if (data[0] == 0xAA && data[1] == 0x02 && data[2] < 50)
		{
			for (i = 0; i < data[2]; i++)
			{
				read(FdCom1, data+3+i, 1);
				usleep(200);
			}
			len = data[2] + 3;
		}
		
		tcflush(FdCom1, TCIFLUSH);
		return len;
	}
	else
	{
		tcflush(FdCom1, TCIFLUSH);
		return -1;
	}
}

/*************************************************
  Function:		port1_close
  Description: 	关闭端口
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void port1_close(void)
{
	close(FdCom1);
}

/*************************************************
  Function:		send_to_com1
  Description: 	发送一帧数据到串口1
  Input: 
	1.			待发送数据
	2.			数据长度
  Output:		无
  Return:		实际发送长度
  Others:
*************************************************/
static int8 send_to_com1(char * data, uint8 len)
{
	int8 ret;
	uint8 i, sum = 0;

	// 计算校验和
	#ifdef _USE_MA802_MCU_
	for (i = 2; i < len-1; i++)
	{
		sum += *(data+i);
	}
	* (data + len-1) = sum;
	#else
	for (i = 1; i < len-1; i++)
	{
		sum += *(data+i);
	}
	* (data + len-1) = 0 - sum;
	#endif
	ret = port1_send(data, len);
	
	return ret;
}

/*************************************************
  Function:    	send_feetdog_cmd
  Description:	发送喂狗命令
  Input: 		无
  Output:		无
  Return:		TRUE/FALSE
  Others:
*************************************************/
static int32 send_feetdog_cmd(void)
{
	#ifdef _USE_MA802_MCU_
	// 下发命令格式:
	// 长度：后面所有字节总数(不包括本身)
	// 校验字：控制字到数据域（包含控制字）的Uint8无进位累加和
	// 帧起始 + 数据长度 + 控制字 + 主命令 + 次命令 + 数据域 + 校验和
	// buf[0]	buf[1]	   buf[2]	 buf[3]	 buf[4]	  buf[5]   buf[6]
	int8 ret;
	uint8 len = 6;
	char buf[10] = {0xAA, 0x04, 0x80, 0x05, 0x02, 0x87};
	#else
	// 下发命令格式:
	// 长度：从byte2开始到最后一个字节
	// 校验字：buf[6] = 0 - buf[1] - buf[2]……- buf[5]
	// 同步码 + 地址 + 数据长度 + 流水号(2字节) + 命令 + 校验和
	// buf[0]	buf[1] buf[2]	 buf[3]	 buf[4]	  buf[5] buf[6]
	int8 ret;
	uint8 i, len = 7;
	char buf[10] = {0xAA, 0x8F, 0x04, 0x00, 0x00, 0x0FF, 0x00};
	#endif
	
	if (FdCom1 < 0)
	{
		return FALSE;
	}

	if (AnsFlag)				// 有等应答时不发喂狗命令
	{
		AnsFlag = 0;
		return FALSE;
	}
	
	ret = send_to_com1(buf, len);
	#if 0
	uint8 i;
	for (i = 0; i < len; i++)
	{
		//printf("buf[%d]: %02x\n", i, buf[i]);
		printf("%02x-", buf[i]);
	}
	printf("\n");
	#endif
	if (ret == len)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
	return FALSE;
}

/*************************************************
  Function:		send_disable_dog
  Description: 	发看门狗禁能命令
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static int32 send_disable_dog(void)
{
	int8 ret = FALSE;
	#ifdef _USE_MA802_MCU_
	uint8 len = 7;
	char buf[10] = {0xAA, 0x05, 0xC0, 0x05, 0x01, 0x00, 0xC7};
	#else
	uint8 len = 7;
	char buf[10] = {0xAA, 0x8F, 0x04, 0x00, 0x00, 0xE2, 0x00};
	#endif
	
	if (FdCom1 < 0)
	{
		return FALSE;
	}
	
	ret = send_to_com1(buf, len);
	#if 0
	uint8 i;
	for (i = 0; i < len; i++)
	{
		//printf("buf[%d]: %02x\n", i, buf[i]);
		printf("%02x-", buf[i]);
	}
	printf("\n");
	#endif

	if (ret == len)
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	
	return ret;
}

#if 0
/*************************************************
  Function:		send_enable_dog
  Description: 	发看门狗使能命令
  Input: 		无
  Output:		无
  Return:		无
  Others:		只有ML8-V1M有这条命令
*************************************************/
static int32 send_enable_dog(void)
{
	int8 ret = FALSE;
	#ifdef _USE_MA802_MCU_
	uint8 len = 7;
	char buf[10] = {0xAA, 0x05, 0xC0, 0x05, 0x01, 0x01, 0xC7};

	if (FdCom1 < 0)
	{
		return FALSE;
	}
	
	ret = send_to_com1(buf, len);
	#if 0
	uint8 i;
	for (i = 0; i < len; i++)
	{
		printf("buf[%d]: %02x\n", i, buf[i]);
	}
	#endif

	if (ret == len)
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	
	return ret;
	#else
	return ret;
	#endif
}
#endif

/*************************************************
  Function:		feetdog_thread
  Description: 	喂狗线程
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void * feetdog_thread(void* data)
{
	while (1)
	{
		// 监视GUI看门狗
		hw_monitor_dog();
		
		if (!FeetDog)
		{
			send_feetdog_cmd();
		}
		else
		{
			send_disable_dog();						// 停止喂狗,让单片机立即重启
		}
		sleep(2);
	}
}

/*************************************************
  Function:		start_feet_dog
  Description: 	启动喂狗线程
  Input: 		无
  Output:		无
  Return:		0: 成功 其它值: 失败
  Others:
*************************************************/
static int32 start_feet_dog(void)
{
	pthread_t th;
	pthread_attr_t new_attr;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&new_attr, SCHED_FIFO);
	pthread_attr_setscope(&new_attr, PTHREAD_SCOPE_SYSTEM);
	//pthread_attr_setstacksize(&new_attr, 48*1024);
	pthread_create(&th, &new_attr, feetdog_thread, NULL);
	
	return 0;
}

/*************************************************
  Function:		hw_start_com_dog
  Description: 	启动串口喂狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void hw_start_com_dog(void)
{
	if (FALSE == port1_init())
	{
		return;
	}
	
	// 不用此命令,只要有发心跳,单片机就使能看门狗
	//send_enable_dog();
	start_feet_dog();
}

/*************************************************
  Function:    	hw_stop_feet_dog
  Description:	停止喂狗,让单片机复位主芯片
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_stop_feet_dog(void)
{	
	#ifdef _JD_MODE_
	uint8 buf[7] = {0xAA, 0x8F, 0x04, 0x00, 0x00, 0xE2, 0x00};
	jd_stop_feet_dog(buf, sizeof(buf));
	jd_stop_feet_dog(buf, sizeof(buf));
	jd_stop_feet_dog(buf, sizeof(buf));
	#else
	FeetDog = 1;
	#endif
}

/*************************************************
  Function:		send_alarm_answer
  Description: 	发送报警应答命令
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static int32 send_alarm_answer(void)
{
	int8 ret = FALSE;
	uint8 len = 7;
	char buf[10] = {0xAA, 0x05, 0x00, 0x04, 0x01, 0x00, 0x05};

	if (FdCom1 < 0)
	{
		return FALSE;
	}

	AnsFlag = 1;
	ret = send_to_com1(buf, len);
	#if 0
	uint8 i;
	for (i = 0; i < len; i++)
	{
		log_printf("buf[%d]: %02x\n", i, buf[i]);
	}
	#endif

	if (ret == len)
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	
	return ret;
}

/*************************************************
  Function:		com1_receive_thread
  Description: 	串口1接收处理线程
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void * com1_receive_thread(void* data)
{
	uint8 len = 7;
	char buf[10] = {""};

	log_printf("Ready receive data form com1!!!\n");
	for (;;)
	{
		memset(buf, 0, sizeof(buf));
		port1_recv(buf, len);
		
		#if 0
		uint8 i;
		log_printf("len = %d\n", len);
		for (i = 0; i < len; i++)
		{
			printf("  %02x", buf[i]);
		}
		log_printf("\n===================\n");
		#endif
		//send_to_com1(buf, len);					// 用于调试,将收到的数据发送回串口
		
		// 以下根据协议更改
		if (buf[0] == 0xAA)
		{
			#if 1
			if ((buf[3] == 0x04) && (buf[4] == 0x01))
			{
				send_alarm_answer();
				if (AlarmCallBackFunc)				// 防区状态上报
				{
					AlarmCallBackFunc(buf[5]);
				}
			}
			#endif
		}
	}
}

/*************************************************
  Function:		send_485_pack
  Description: 	发送一帧数据到485
  Input: 
	1.			待发送数据
	2.			数据长度
  Output:		无
  Return:		实际发送长度
  Others:
*************************************************/
int8 send_485_pack(char * data, uint8 len)
{	
	int8 ret;
	uint8 i, sum = 0;

	hw_485_send();
	usleep(1000);
	
	// 计算校验和
	for (i = 1; i < len-1; i++)
	{
		sum += *(data+i);
	}
	* (data + len-1) = 0 - sum;
	ret = port1_send(data, len);

	if ((JD_FACTORY_ACBUS == storage_get_extmode(EXT_MODE_JD_FACTORY)))
	{
		usleep(20*1000);
	}
	else
	{
		usleep(30*1000);
	}
	hw_485_recv();
	
	return ret;
}

/*************************************************
  Function:		jd_receive_thread
  Description: 	串口1接收处理线程(接收家电模块发出的数据)
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void * jd_receive_thread(void* data)
{
	uint8 len = 0;
	static char buf[51] = {""};

	log_printf("Ready receive data form com1!!!\n");
	for (;;)
	{
		memset(buf, 0, sizeof(buf));
		len = port1_recv_jd(buf);
		//send_485_pack(buf, len);						// 用于调试,将收到的数据发送回串口

		#if 0
		uint8 i;
		printf("len = %d\n", len);
		for (i = 0; i < len; i++)
		{
			printf("  %02x", buf[i]);
		}
		printf("\n===================\n");
		#endif
		
		if (JdCallBackFunc)
		{
			JdCallBackFunc(buf);
		}
	}
}

/*************************************************
  Function:		hw_start_monitor_dog
  Description: 	启动监视看门狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void hw_start_monitor_dog(void)
{
	g_MonitorFlag = 1;
}

/*************************************************
  Function:		hw_stop_monitor_dog
  Description: 	停止监视看门狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void hw_stop_monitor_dog(void)
{
	g_MonitorFlag = 0;
}

/*************************************************
  Function:		hw_clr_monitor_dog
  Description: 	清除监视看门狗计数
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void hw_clr_monitor_dog(void)
{
	g_MonitorCnt = 0;
}

/*************************************************
  Function:		hw_monitor_dog
  Description: 	监视看门狗,在固定时间内没有清除标志
  				则强制重启
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void hw_monitor_dog(void)
{
	if (g_MonitorFlag)
	{
		g_MonitorCnt++;
		if (g_MonitorCnt == MAX_MONOTOR_DOG_TIME)
		{
			log_printf("miniGUI reboot!!!!!!\n");
			hw_stop_feet_dog();
			//system_reboot_app();
		}
	}
	else
	{
		g_MonitorCnt = 0;
	}
	//log_printf("g_MonitorFlag: %d   g_MonitorCnt: %d\n", g_MonitorFlag, g_MonitorCnt);
}

#ifdef _TIMER_REBOOT_NO_LCD_
/*************************************************
  Function:     hw_set_lcd_pwm0
  Description:  设置单片机接管屏背光控制
  Input:       	
  	1.flag: 	TRUE: 关屏 FALSE: 开屏				
  Output:       无
  Return:       无
  Others:
*************************************************/
void hw_set_lcd_pwm0(uint8 flag)
{   
    uint8 buf[7] = {0xAA, 0x8F, 0x04, 0x00, 0x00, 0xF1, 0x00};
	
    if(flag == TRUE)
    {
        buf[5] = 0xF1;//设置单片机接管屏背光控制
    }
    else
    {
        buf[5] = 0xF2;
    }
	#ifdef _JD_MODE_
    jd_stop_feet_dog(buf, sizeof(buf));
    jd_stop_feet_dog(buf, sizeof(buf));
    jd_stop_feet_dog(buf, sizeof(buf));
	#endif
}
#endif

/*************************************************
  Function:		init_alarm_callback
  Description:	初始化报警回调函数(防区状态由单片机检测)
  Input: 
	1.func		报警回调函数指针
  Output:
  Return:		成功 = 0，其它值失败
  Others:		报警8个防区端口状态回调,
  	防区端口状态有变化时,单片机会上报
*************************************************/
int32 init_alarm_callback(AlarmCallBack func)
{
	pthread_t th;
	pthread_attr_t new_attr;
	
	if (!InitAlarmFlag)
	{	
		InitAlarmFlag = 1;
		if (func != NULL)
		{
			if (FALSE == port1_init())
			{
				return;
			}
			AlarmCallBackFunc = func;
			pthread_attr_init (&new_attr);
			pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
			//pthread_attr_setstacksize (&new_attr, 48*1024);
			pthread_create(&th, &new_attr, com1_receive_thread, NULL);
		}
	}
	
	return 0;
}

/*************************************************
  Function:		jd_callback_init
  Description:	初始化家电回调函数
  Input: 
	1.func		家电回调函数指针
  Output:
  Return:		成功 = 0，其它值失败
  Others:		
*************************************************/
int32 jd_callback_init(JdCallBack func)
{
	pthread_t th;
	pthread_attr_t new_attr;
	
	if (!InitJdFlag)
	{	
		InitJdFlag = 1;
		if (func != NULL)
		{
			if (FALSE == port1_init())
			{
				return;
			}
			JdCallBackFunc = func;
			hw_485_recv();
			pthread_attr_init (&new_attr);
			pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
			//pthread_attr_setstacksize (&new_attr, 48*1024);
			pthread_create(&th, &new_attr, jd_receive_thread, NULL);
		}
	}
	return 0;
}

