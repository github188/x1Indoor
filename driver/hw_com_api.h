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
#ifndef __HW_COMM_API_H__
#define __HW_COMM_API_H__

//#include "au_types.h"
#define COM1_BOUND				9600				// 串口1波特率9600bps
#define COM2_BOUND				9600				// 串口2波特率9600bps

typedef	void (* AlarmCallBack)(uint8 PortStatus);
typedef	void (* JdCallBack)(uint8 * buf);;

// 串口结构
typedef struct
{
	char	prompt;									// prompt after reciving data
	int32 	baudrate;								// baudrate
	char	databit;								// data bits, 5, 6, 7, 8
	char 	debug;									// debug mode, 0: none, 1: debug
	char 	echo;									// echo mode, 0: none, 1: echo
	char	fctl;									// flow control, 0: none, 1: hardware, 2: software
	char 	tty;									// tty: 0, 1, 2, 3, 4, 5, 6, 7
	char	parity;									// parity 0: none, 1: odd, 2: even
	char	stopbit;								// stop bits, 1, 2
	const 	int32 reserved;							// reserved, must be zero
}PORTINFO_T, * PPORTINFO_T;

/*************************************************
  Function:		port1_init
  Description: 	端口1初始化
  Input:		无 
  Output:		无
  Return:		-1:失败 0:成功
  Others:
*************************************************/
extern int32 port1_init(void);

/*************************************************
  Function:		port1_send
  Description: 	串口1发送
  Input: 
	1.data		待发送数据
	2.datalen	数据长度
  Output:		无
  Return:		-1: 失败 成功: 实际发送长度
  Others:
*************************************************/
extern int32 port1_send(char * data, int32 datalen);

/*************************************************
  Function:		port1_recv
  Description: 	串口1接收
  Input: 
	1.datalen:	读入的数据长度
	2.baudrate:	波特率
  Output:
	1.data		接收到的数据
  Return:		-1: 失败 成功: 实际接收长度
  Others:
*************************************************/
extern int32 port1_recv(char * data, int32 datalen);

/*************************************************
  Function:		port1_close
  Description: 	关闭端口1
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
extern void port1_close(void);

/*************************************************
  Function:		hw_start_monitor_dog
  Description: 	启动监视看门狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
extern void hw_start_monitor_dog(void);

/*************************************************
  Function:		hw_stop_monitor_dog
  Description: 	停止监视看门狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
extern void hw_stop_monitor_dog(void);

/*************************************************
  Function:		hw_clr_monitor_dog
  Description: 	清除监视看门狗计数
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
extern void hw_clr_monitor_dog(void);

/*************************************************
  Function:		hw_monitor_dog
  Description: 	监视看门狗,在固定时间内没有清除标志
  				则强制重启
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
extern void hw_monitor_dog(void);

/*************************************************
  Function:		hw_start_com_dog
  Description: 	启动串口喂狗
  Input: 		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
extern void hw_start_com_dog(void);

/*************************************************
  Function:    		hw_stop_feet_dog
  Description:		停止喂狗
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
extern void hw_stop_feet_dog(void);

/*************************************************
  Function:		init_alarm_callback
  Description:	初始化报警回调函数
  Input: 
	1.func		报警回调函数指针
  Output:
  Return:		成功 = 0，其它值失败
  Others:		报警8个防区端口状态回调,
  	防区端口状态有变化时,单片机会上报
*************************************************/
extern int32 init_alarm_callback(AlarmCallBack func);

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
extern int8 send_485_pack(char * data, uint8 len);

/*************************************************
  Function:		jd_callback_init
  Description:	初始化家电回调函数
  Input: 
	1.func		家电回调函数指针
  Output:
  Return:		成功 = 0，其它值失败
  Others:		
*************************************************/
extern int32 jd_callback_init(JdCallBack func);

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
void hw_set_lcd_pwm0(uint8 flag);
#endif
#endif

