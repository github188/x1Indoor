/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_dianti.h
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
#ifndef _DIANTI_H_
#define _DIANTI_H_

#ifndef _AU_PROTOCOL_
#define DIANTI_UDP_PORT			9000				// 电梯控制器UDP端口: 9000
#define ELEVATOR_UP				0x01 				// 电梯上行
#define ELEVATOR_DOWN			0x02				// 电梯下行
#define ELEVATOR_CALL			0x03				// 电梯被动召梯
#else
// 冠林电梯控制器
#define DIANTI_UDP_PORT			8000				// 电梯控制器UDP端口: 8000
#define ELEVATOR_UP				0x03 				// 电梯上行
#define ELEVATOR_DOWN			0x02				// 电梯下行
#define ELEVATOR_CALL			0x05				// 电梯被动召梯
#define ELEVATOR_ROOM			0x0B				// 电梯互访
#endif

// 第三方电梯控制器
#define OTHER_ELEVATOR_UP		0x01 				// 电梯上行
#define OTHER_ELEVATOR_DOWN		0x02				// 电梯下行
#define OTHER_ELEVATOR_CALL		0x03				// 电梯召梯

/*************************************************
  Function:		dianti_set_cmd
  Description:	发送命令给电梯控制器
  Input:
  Output:
  Return:		无
  Others:		无	
*************************************************/
void dianti_set_cmd(uint8 direction);

/*************************************************
  Function:		dianti_rec_cmd
  Description:	接收电梯控制器的数据
  Input:		无
  Output:		无	
  Return:		无
  Others:	
*************************************************/
void dianti_rec_cmd(void);

/*************************************************
  Function:		get_dianti_ip
  Description:	获取电梯控制器IP地址
  Input:		无
  Output:		无	
  Return:		IP地址
  Others:	
*************************************************/
uint32 get_dianti_ip(void);
#endif

