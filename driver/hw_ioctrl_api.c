/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	hw_ioctrl_api.c
  Author:		罗发禄
  Version:		V2.0
  Date:			2014-09-28
  Description:	4.3寸室内机GPIO驱动应用程序封装
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>			
#include <asm/ioctl.h>	
//#include <linux/delay.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <linux/hdreg.h>

#include "hw_ioctrl_api.h"
#include "au_debug.h"

static int32 Fd = 0;
static uint8 InitFlag = 0;
static uint32 ReadBuf = 0;
static IO_HOOK IoEventFunc = NULL;

#ifdef _DOOR_PHONE_
/*************************************************
  Function:		hw_mic_switch_analog
  Description: 	咪头切换到模拟输入(适用于与门前机通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mic_switch_analog(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MIC_SWITCH, 1);

	return 0;
}

/*************************************************
  Function:		hw_mic_switch_digit
  Description: 	咪头切换到数字输入(适用于网络通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mic_switch_digit(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MIC_SWITCH, 0);

	return 0;
}

/*************************************************
  Function:		hw_spk_switch_analog
  Description: 	喇叭切换到模拟输出(适用于与门前机通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_spk_switch_analog(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, SPK_SWITCH, 1);

	return 0;
}

/*************************************************
  Function:		hw_spk_switch_digit
  Description: 	喇叭切换到数字输出(适用于网络通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_spk_switch_digit(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, SPK_SWITCH, 0);

	return 0;
}

/*************************************************
  Function:		hw_mk_lock
  Description: 	门口机开锁
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_lock(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_LOCK, 1);

	return 0;
}

/*************************************************
  Function:		hw_mk_power_on
  Description: 	打开门口机电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_power_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_PWR, 0);

	return 0;
}

/*************************************************
  Function:		hw_mk_power_off
  Description: 	关闭门口机电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_power_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_PWR, 1);

	return 0;
}

/*************************************************
  Function:		hw_mk_video_on
  Description: 	打开门口机视频
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_video_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_VIDEO_CTRL, 0);

	return 0;
}

/*************************************************
  Function:		hw_mk_video_off
  Description: 	关闭门口机视频
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_video_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_VIDEO_CTRL, 1);

	return 0;
}

/*************************************************
  Function:		hw_mk_talk
  Description: 	门口机通话控制
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_talk(void)
{
	hw_mc34118_on();
	hw_mic_switch_analog();
	hw_spk_switch_analog();
	//hw_speak_on();
	//hw_mk_power_on();
	//hw_mk_video_on();
}

/*************************************************
  Function:		hw_mk_start
  Description:  门前机工作控制
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_start(void)
{
	hw_mc34118_off();
	hw_mk_power_on();
	hw_mk_video_on();
}

/*************************************************
  Function:		hw_mk_ring_back
  Description:  响门口回铃声
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_ring_back(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MK_RING, 0);

	return 0;
}

/*************************************************
  Function:		hw_reset_tw9912
  Description: 	复位tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_tw9912(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, RST_TW9912, 1);

	return 0;
}

/*************************************************
  Function:		hw_reset_fm2018
  Description: 	复位fm2018
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_fm2018(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, RST_FM2018, 1);

	return 0;
}

/*************************************************
  Function:		hw_mc34118_on
  Description:  打开mc34118
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mc34118_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MC34118_CTRL, 1);

	return 0;
}

/*************************************************
  Function:		hw_mc34118_off
  Description:  关闭mc34118
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mc34118_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MC34118_CTRL, 0);

	return 0;
}

/*************************************************
  Function:		hw_tw9912_power_on
  Description: 	打开tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_tw9912_power_on(void)
{
	log_printf("hw_tw9912_power_on\n");
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, PDN_TW9912, 0);

	return 0;
}

/*************************************************
  Function:		hw_tw9912_power_down
  Description: 	关闭tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_tw9912_power_down(void)
{
	log_printf("hw_tw9912_power_down\n");
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, PDN_TW9912, 1);

	return 0;
}
#endif

/*************************************************
  Function:		GetBit
  Description: 	
  Input: 		无	
  Output:		无
  Return:		无
  Others:		
*************************************************/
static uint32 GetBit(uint32 value, uint8 nBit)
{
	uint32 i = 1;
	uint32 BitValue = 0;
	
	BitValue = value & (i<<nBit);
	//log_printf("%s : BitValue = %d\n", __FUNCTION__, BitValue);
	return BitValue;
}

/*************************************************
  Function:		hw_alarm_led_on
  Description: 	报警灯亮
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, ALM_LED, 0);							// 1-灭 0-亮 2-闪烁

	return 0;
}

/*************************************************
  Function:		hw_alarm_led_off
  Description: 	报警灯灭
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, ALM_LED, 1);

	return 0;
}

/*************************************************
  Function:		hw_alarm_led_glint
  Description: 	报警灯闪烁
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_glint(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, ALM_LED, 2);

	return 0;
}

/*************************************************
  Function:		hw_msg_led_on
  Description: 	信息指示灯亮
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MSG_LED, 0);							// 1-灭 0-亮 2-闪烁

	return 0;
}

/*************************************************
  Function:		hw_msg_led_off
  Description: 	信息指示灯灭
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MSG_LED, 1);

	return 0;
}

/*************************************************
  Function:		hw_message_led_glint
  Description: 	信息指示闪烁
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_glint(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, MSG_LED, 2);
	//log_printf("%s : msg led glint\n", __FUNCTION__);
	
	return 0;	
}

/*************************************************
  Function:		hw_key_led_on
  Description:	亮键盘背光灯
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_on(void)
{	
	ioctl(Fd, KEY_LED, 0);
}

/*************************************************
  Function:		hw_key_led_off
  Description:	灭键盘背光灯
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_off(void)
{	
	ioctl(Fd, KEY_LED, 1);
}

/*************************************************
  Function:		hw_key_led_flash
  Description:	键盘背光灯亮1秒灭1秒
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_flash(void)
{
	ioctl(Fd, KEY_LED, 2);
}

/*************************************************
  Function:		hw_ext_on
  Description:	打开外部继电器
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_ext_on(void)
{
	ioctl(Fd, CTRL_JDQ, 1);
}

/*************************************************
  Function:		hw_ext_off
  Description:	关闭外部继电器
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_ext_off(void)
{
	ioctl(Fd, CTRL_JDQ, 0);
}

/*************************************************
  Function:		hw_dianti_ctrl
  Description:	电梯控制
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_dianti_ctrl(void)
{
	#if (SYS_TYPE != E81_FZJB_VER)
	hw_ext_on();
	usleep(100*1000);
	hw_ext_off();
	#endif
}

/*************************************************
  Function:		hw_485_send
  Description:	485发送使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_485_send(void)
{
	ioctl(Fd, CTRL_485, 0);
}

/*************************************************
  Function:		hw_485_recv
  Description:	485接收使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_485_recv(void)
{
	ioctl(Fd, CTRL_485, 1);
}

/*************************************************
  Function:		hw_lcd_power_on
  Description: 	打开屏电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_power_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, LCD_PWR, 0);

	return 0;
}

/*************************************************
  Function:		hw_lcd_power_off
  Description:  关闭屏电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_power_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, LCD_PWR, 1);

	return 0;
}

/*************************************************
  Function:		hw_lcd_back_on
  Description: 	屏背光开
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_back_on(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, LCD_BACK, 0);

	return 0;
}

/*************************************************
  Function:		hw_lcd_back_off
  Description:  屏背光关闭
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_back_off(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, LCD_BACK, 1);

	return 0;
}

/*************************************************
  Function:		hw_speak_on
  Description: 	开SPK
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_speak_on(void)
{
	log_printf("open_speaker\n");
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, SPK_CTRL, 0);

	return 0;
}

/*************************************************
  Function:		hw_speak_off
  Description:  关闭SPK
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_speak_off(void)
{
	log_printf("close_speaker\n");
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, SPK_CTRL, 1);

	return 0;
}

/*************************************************
  Function:		hw_key_beep
  Description:  响蜂鸣器
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_key_beep(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, KEY_BEEP, 0);
	//log_printf("key beep!\n");
	
	return 0;
}

/*************************************************
  Function:		hw_set_volume
  Description: 	设置模拟门前机通话音量(1-8级)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_set_volume(uint8 level)
{
	#ifdef _DOOR_PHONE_
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, TALK_VOLUME_SET, level);
	#endif
	
	return TRUE;
}

/*************************************************
  Function:		hw_switch_digit
  Description: 	数字通路切换
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:	
  				3157_Ctr  0
				SPK_CTL   0
				3005_Ctr  0
				MK_POW	  1
				ON_34118  0
*************************************************/
int32 hw_switch_digit(void)
{
	#ifdef _DOOR_PHONE_
	hw_mic_switch_digit();
	hw_spk_switch_digit();
	//hw_speak_on();
	hw_mk_video_off();
	hw_mk_power_off();
	hw_mc34118_off();
	#endif

	return TRUE;
}

/*************************************************
  Function:		hw_reset_dev
  Description: 	复位外部设备(网卡、电容屏)
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_dev(void)
{
	if (!InitFlag)
	{
		return -1;
	}
	ioctl(Fd, RST_PHY, 0);
	
	return 0;
}

/*************************************************
  Function:		hw_get_zone_status
  Description: 	获取防区状态
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_get_zone_status(void)
{
	if (!InitFlag)
	{
		return -1;
	}

	//sleep(1);
	ioctl(Fd, GET_ZONE_STATUS, 0);
	
	return 0;
}

/*************************************************
  Function:		*check_ioctrl
  Description:	检测IO的输入口
  Input: 
	1.			函数指针
  Output:		无
  Return:		无
  Others:		回调
*************************************************/
static void * check_ioctrl(void* data)
{
	int32 ret;
	
	while (1)
	{
		ret = read(Fd, &ReadBuf, sizeof(ReadBuf));
		if (ret == sizeof(ReadBuf))
		{
			if (IoEventFunc)
			{
				#ifdef _DOOR_PHONE_					// 模拟门前机呼叫处理,一直按住驱动那边只发一次
				if (GetBit(ReadBuf, 0))
				{
					IoEventFunc(MK_CALL_DOWN);
					log_printf("Call Down!\n");
					log_printf("ReadBuf: %d\n", ReadBuf);
				}
				#elif defined _ALARM_IO_			// 8防区检测
				IoEventFunc(ReadBuf&0xFF);
				#elif defined _NET_DOOR_			// 网络门前机输入事件检测
				IoEventFunc(ReadBuf&0x0F);
				#endif
			}
		}
		//sleep(1);
	}
}

/*************************************************
  Function:		init_ioctrl_callback
  Description:	初始化ioctrl回调函数
  Input: 
	1.func		ioctrl回调函数指针
  Output:
  Return:		成功=0，其它值失败
  Others:		回调
*************************************************/
int32 init_ioctrl_callback(IO_HOOK func)
{
	pthread_t th;
	pthread_attr_t new_attr;

	if (!InitFlag)
	{
		Fd = open("/dev/w55fa92_ioctrl", O_RDONLY);
		if (Fd == -1)
		{
			perror("IOCTRL");
			return -1;
		}
		InitFlag = 1;
	}

	if (func != NULL)
	{
		IoEventFunc = func;
		pthread_attr_init (&new_attr);
		pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
		//pthread_attr_setstacksize (&new_attr, 48*1024);
		pthread_create(&th, &new_attr, check_ioctrl, NULL);
	}

	return 0;
}

