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
#ifndef __IOCTRL_API_H__
#define __IOCTRL_API_H__	
#include "AppConfig.h"
#include "au_types.h"

#define ALM_LED				2140
#define MSG_LED				2141
#define MIC_SWITCH			2142
#define SPK_SWITCH			2143
#define MK_LOCK				2144
#define MK_PWR				2145
#define MK_VIDEO_CTRL		2146
#define RST_TW9912			2147
#define LCD_PWR				2148
#define LCD_BACK			2149
#define RST_FM2018			2150
#define SPK_CTRL			2151
#define MC34118_CTRL		2152
#define KEY_BEEP			2153
#define MK_RING				2154
#define PDN_TW9912			2155
#define TALK_VOLUME_SET		2156
#define KEY_LED				2157
#define CTRL_JDQ			2158
#define CTRL_485			2159
#define OPEN_LOCK			2160
#define CARD_PWR			2161
#define RST_PHY				2162
#define GET_ZONE_STATUS		2163

#define	MK_CALL_DOWN		0x01

typedef enum
{
	LED_ALARM = 0x00,			// 安防键背光灯
	LED_CENTER,					// 呼叫中心键背光灯
	LED_MONITOR,				// 监视键背光灯
	LED_LOCK,					// 开锁键背光灯
	LED_TALK,					// 通话键背光灯
	LED_ALL						// 所有键背光灯
}LED_TYPE;

typedef enum
{
	LED_ON,						// 亮
	LED_OFF,					// 灭
	LED_FLASH					// 闪
}LED_STATE;

typedef	void (*IO_HOOK)(uint8 event);

/*************************************************
  Function:		hw_mic_switch_analog
  Description: 	咪头切换到模拟输入(适用于与门前机通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mic_switch_analog(void);

/*************************************************
  Function:		hw_mic_switch_digit
  Description: 	咪头切换到数字输入(适用于网络通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mic_switch_digit(void);

/*************************************************
  Function:		hw_spk_switch_analog
  Description: 	喇叭切换到模拟输出(适用于与门前机通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_spk_switch_analog(void);

/*************************************************
  Function:		hw_spk_switch_digit
  Description: 	喇叭切换到数字输出(适用于网络通话)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_spk_switch_digit(void);

/*************************************************
  Function:		hw_mk_lock
  Description: 	门口机开锁
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_lock(void);

/*************************************************
  Function:		hw_mk_power_on
  Description: 	打开门口机电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_power_on(void);

/*************************************************
  Function:		hw_mk_power_off
  Description: 	关闭门口机电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_power_off(void);

/*************************************************
  Function:		hw_mk_video_on
  Description: 	打开门口机视频
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_video_on(void);

/*************************************************
  Function:		hw_mk_video_off
  Description: 	关闭门口机视频
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_video_off(void);

/*************************************************
  Function:		hw_mk_talk
  Description: 	门口机通话控制
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_talk(void);

/*************************************************
  Function:		hw_mk_start
  Description:  门前机工作控制
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_start(void);

/*************************************************
  Function:		hw_mk_ring_back
  Description:  响门口回铃声
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mk_ring_back(void);

/*************************************************
  Function:		hw_reset_tw9912
  Description: 	复位tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_tw9912(void);

/*************************************************
  Function:		hw_reset_fm2018
  Description: 	复位fm2018
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_fm2018(void);

/*************************************************
  Function:		hw_mc34118_on
  Description:  打开mc34118
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mc34118_on(void);

/*************************************************
  Function:		hw_mc34118_off
  Description:  关闭mc34118
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_mc34118_off(void);

/*************************************************
  Function:		hw_tw9912_power_on
  Description: 	打开tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_tw9912_power_on(void);

/*************************************************
  Function:		hw_tw9912_power_down
  Description: 	关闭tw9912
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_tw9912_power_down(void);

/*************************************************
  Function:		hw_alarm_led_on
  Description: 	报警灯亮
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_on(void);

/*************************************************
  Function:		hw_alarm_led_off
  Description: 	报警灯灭
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_off(void);

/*************************************************
  Function:		hw_alarm_led_glint
  Description: 	报警灯闪烁
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_alarm_led_glint(void);

/*************************************************
  Function:		hw_msg_led_on
  Description: 	信息指示灯亮
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_on(void);

/*************************************************
  Function:		hw_msg_led_off
  Description: 	信息指示灯灭
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_off(void);

/*************************************************
  Function:		hw_message_led_glint
  Description: 	信息指示闪烁
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_message_led_glint(void);

/*************************************************
  Function:		hw_key_led_on
  Description:	亮键盘背光灯
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_on(void);

/*************************************************
  Function:		hw_key_led_off
  Description:	灭键盘背光灯
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_off(void);

/*************************************************
  Function:		hw_key_led_flash
  Description:	键盘背光灯亮1秒灭1秒
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_key_led_flash(void);

/*************************************************
  Function:		hw_ext_on
  Description:	打开外部继电器
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_ext_on(void);

/*************************************************
  Function:		hw_ext_off
  Description:	关闭外部继电器
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_ext_off(void);

/*************************************************
  Function:		hw_dianti_ctrl
  Description:	电梯控制
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_dianti_ctrl(void);

/*************************************************
  Function:		hw_485_send
  Description:	485发送使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_485_send(void);

/*************************************************
  Function:		hw_485_recv
  Description:	485接收使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void hw_485_recv(void);

/*************************************************
  Function:		hw_lcd_power_on
  Description: 	打开屏电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_power_on(void);

/*************************************************
  Function:		hw_lcd_power_off
  Description:  关闭屏电源
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_power_off(void);

/*************************************************
  Function:		hw_lcd_back_on
  Description: 	屏背光开
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_back_on(void);

/*************************************************
  Function:		hw_lcd_back_off
  Description:  屏背光关闭
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_lcd_back_off(void);

/*************************************************
  Function:		hw_speak_on
  Description: 	开SPK
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_speak_on(void);

/*************************************************
  Function:		hw_speak_off
  Description:  关闭SPK
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_speak_off(void);

/*************************************************
  Function:		hw_key_beep
  Description:  响蜂鸣器
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_key_beep(void);

/*************************************************
  Function:		hw_set_volume
  Description: 	设置模拟门前机通话音量(1-8级)
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_set_volume(uint8 level);

/*************************************************
  Function:		hw_switch_digit
  Description: 	数字通路切换
  Input: 		无	
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_switch_digit(void);

/*************************************************
  Function:		hw_reset_dev
  Description: 	复位外部设备(网卡、电容屏)
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_reset_dev(void);

/*************************************************
  Function:		hw_get_zone_status
  Description: 	获取防区状态
  Input: 		无
  Output:		无
  Return:		成功与失败
  Others:		
*************************************************/
int32 hw_get_zone_status(void);

/*************************************************
  Function:		init_ioctrl_callback
  Description:	初始化ioctrl回调函数
  Input: 
	1.func		ioctrl回调函数指针
  Output:
  Return:		成功=0，其它值失败
  Others:		回调
*************************************************/
int32 init_ioctrl_callback(IO_HOOK func);
#endif

