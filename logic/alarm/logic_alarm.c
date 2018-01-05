/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_alarm.c
  Author:   	罗发禄
  Version:   	2.0
  Date: 		2014-11-03
  Description:  安防逻辑处理

  				g_alarm_valid_flag：
				单个防区触发有效的条件:
					1.未旁路，未隔离的单防区为布防状态的防区。(除延时防区外)
					2.延时防区无论是处于单个防区布防还是撤防状态，都将被设置为触发无效
			   	触发有效被重新设为无效的条件:
					1.报警声音停止后重新被设为无效.(这样触发可重新报警)
					2.清除警示和撤防后。
				=================================
				g_alarm_delay_flag：
				记忆是否是延时防区报警的条件
					1.该防区为延时的防区被触发。
				被重新设置为0的条件
					1.不在延时时间结束后(避免该防区一直处于触发状态时再做处理)。而是撤防后
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#include "logic_include.h"

#define SOS_TIME       		3						// sos在3秒内持续触发则为SOS触发,否则为误操作
#define DEFINE_SIZE   		21						// 属性字节23位

extern PAF_FLASH_DATA 		gpAfParam;

static struct ThreadInfo 	g_alarm_thread;
static SOUND_TYPE			g_sound_type = NO_SOUND_TYPE;
static ALARMGUI_CALLBACK	g_alarm_callback  = NULL;
static SHOW_SYSEVENHIT		g_show_sysEvenHit = NULL;

static uint8 g_alarm_valid_flag = 0;				// 防区有效标志,每一位代表一个防区
static uint8 g_alarm_delay_flag = 0;				// 延时防区触发标志,每一位代表一个防区
static uint8 g_alarming_flag = 0;					// 防区报警标志,每一位代表一个防区

static uint16 g_alarm_time = 0;						// 报警时间计时
static uint16 g_delay_time = 0;      	 			// 报警延时时间计时
static uint16 g_exit_time = 0;       				// 退出预警时间计时
static uint8 g_sos_time  = 0;						// 紧急按钮有效触发时间
static uint8 g_sos_state = 0;						// sos是否触发

static uint8 g_single_defend = 0;					// 记忆是否为单个防区布防，以防在设置属性的时候，布防状态被破坏
static uint8 g_whole_alarm_state;    				// 整机是否报警
static uint8 g_judge_can_defend; 					// 判断是否能布防所需的数值，还未排除延时防区
static uint8 g_can_hear = 0;						//是否可听  0可听1不可听

static uint8 g_resume_delay_flag = 0;				// 用于延时防区报警时，将g_alarm_delay_flag清回零
static uint8 g_door_ring = 0;						// 门前铃触发标志
static uint8 g_7area_set = 0;						//第七防区联动布防标志
static uint8 g_8area_unset = 0;						//第八防区联动撤防标志

// add by  wufn
static uint8 g_Alarm_No24clear = 0;                 // 非24小时防区清除标志,每一位代表一个防区
static uint8 g_Alarm_No24touch = 0;                 // 非24小时防区触发，每一位代表一个防区
//static uint8 g_Alarm_Unset = 0;                   // 保护非24小时防区报警撤防后的闪烁状态(红黄闪烁)
static uint8 g_Alarm_Set_Operator = 0;				// 布防操作标志

static void deal_alarm_func(uint8 num);
static void on_alarm_timer(void * arg);
static void stop_sound_arbistrator(SOUND_TYPE sound_type);
void alarm_deal(SYS_MSG_ID id, uint8 msg);
//void alarm_set_unset(void);

/*************************************************
  Function:    	set_area_show_info
  Description: 	设置八个防区的界面显示状态
  Input:
  	1.can_write	要写入注册表
  Output:    		
  Return:		false:失败 true:成功	
  Others:           
*************************************************/
static uint8 set_area_show_info(uint8 can_write)
{
   uint8 i;
   
   for (i = 0; i < AREA_AMOUNT; i++)
   {
	   if (0 == (((gpAfParam->enable)>>i)& 0x01))
	   {
			gpAfParam->show_state[i] = UNENABLE_SHOW_STATE;	// 防区未启用
	   }
	   else if (1 == ((gpAfParam->isolation>>i)&0x01))
	   {
			gpAfParam->show_state[i] = ISO_SHOW_STATE;		// 防区隔离
	   }
	   else if (ALARM_SHOW_STATE == gpAfParam->show_state[i])
	   {
			continue;
	   }
	   else
	   {
	   		// 为了防止在设置属性等时，单个防区布防被破坏，多加g_single_defend判断
	   		if (1 == ((gpAfParam->is_24_hour>>i)&0x01) || (1 == ((g_single_defend>>i) & 0x01)))
	   		{
				gpAfParam->show_state[i] = WORK_SHOW_STATE;
			}
	   		else if (DIS_DEFEND == gpAfParam->defend_state)	// 整机在撤防状态
	   		{
				gpAfParam->show_state[i] = NORM_SHOW_STATE;
			}
			else if (SET_DEFEND == gpAfParam->defend_state)	// 整机在布防状态
	   		{
				gpAfParam->show_state[i] = WORK_SHOW_STATE;
			}
			else if (PART_DEFEND == gpAfParam->defend_state)// 整机在局防状态
	   		{
				if (1 == ((gpAfParam->part_valid>>i) & 0x01))
				{
					gpAfParam->show_state[i] = WORK_SHOW_STATE;
				}
				else
				{
					gpAfParam->show_state[i] = NORM_SHOW_STATE;
				}
			}
	   }
   }
   
   // 设置防区有效位标志
   g_alarm_valid_flag = gpAfParam->enable & (~gpAfParam->isolation);
   if (DIS_DEFEND == gpAfParam->defend_state)
   {
   		g_alarm_valid_flag &= gpAfParam->is_24_hour;
		g_alarm_valid_flag |= g_single_defend;		// 单个防区布防的设为有效
   }
   else if (PART_DEFEND == gpAfParam->defend_state)
   {
		g_alarm_valid_flag &= gpAfParam->part_valid;
   }

   // 延时防区被设置为无效。单个防区布防的延时防区也会延时 
   for (i = 0; i < AREA_AMOUNT; i++)
   {
   		if (0 == ((gpAfParam->is_24_hour>>i)&0x01))
   		{
			if (gpAfParam->delay_time[i] > 0)
			{
				g_alarm_valid_flag &= ~(1<<i);
			}
   		}
   }
   
   if (1 == can_write)
   {
   		SaveRegInfo();
   }

   return 0;
}

/*************************************************
  Function:    		alarm_get_area_define
  Description: 		获得属性页面显示信息
  Input:
  Output:    		
  	1.define_info	属性页面显示的信息
  Return:				
  Others:    		       
*************************************************/
void alarm_get_area_define(uint8 * define_info)
{
	if (NULL == define_info)
	{
		return;
	}
	
	memcpy(define_info, (uint8 *)(gpAfParam) + 2, DEFINE_SIZE);
}

/*************************************************
  Function:    		alarm_set_area_define
  Description: 		设置报警属性页面显示信息
  Input:
   	1.define_info	属性的数据
  Output:    		
  Return:			false:失败 true:成功				
  Others:           
*************************************************/
uint8 alarm_set_area_define(uint8 *define_info)
{
	if (NULL == define_info)
	{
		return FALSE;
	}
	
	memcpy((uint8*)(gpAfParam) + 2, define_info,  DEFINE_SIZE);
	
	return set_area_show_info(1);
}

/*************************************************
  Function:    		alarm_get_area_isolation
  Description: 		获取防区隔离参数
  Input:
  Output:   
  Return:			暂时隔离参数
  Others:           
*************************************************/
uint8 alarm_get_area_isolation(void)
{
	return gpAfParam->isolation;
}

/*************************************************
  Function:    		alarm_set_area_isolation
  Description: 		设置报警暂时隔离显示信息
  Input:
  	1.iso_info		暂时隔离显示信息
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 alarm_set_area_isolation(uint8 iso_info)
{
	gpAfParam->isolation = iso_info;
	
	return set_area_show_info(1);
}

/*************************************************
  Function:    		alarm_get_part_valid
  Description: 		获取防区局防有效参数
  Input:
  Output:   
  Return:			局防有效参数	
  Others:           
*************************************************/
uint8 alarm_get_part_valid(void)
{
	return gpAfParam->part_valid;
}

/*************************************************
  Function:    		alarm_set_area_part_valid
  Description: 		设置报警局防有效页面显示信息
  Input:
  	1.valid			局防有效页面显示信息
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 alarm_set_area_part_valid(uint8 valid)
{
	if (gpAfParam->is_24_hour != (gpAfParam->is_24_hour & valid))
	{
		return 2;
	}
	gpAfParam->part_valid = valid;
	
	return set_area_show_info(1);
}

/*************************************************
  Function:    		start_timerEx
  Description: 		
  Input:   			
  Output:    		无
  Return:			
  Others:           
*************************************************/
static int start_timerEx(uint32 tick, void* proc, struct ThreadInfo* thread)
{
	 return inter_start_thread(thread, proc, (void *)tick, 0);
}

/*************************************************
  Function:    	on_alarm_timer
  Description: 	安防模块的定时器处理函数
  Input:   			
  Output:    	无
  Return:			
  Others:           
*************************************************/
static void deal_alarm_timer(void * arg)
{
	pthread_detach(pthread_self());
	log_printf("g_alarm_thread.running:%d\n",g_alarm_thread.running);
	while (g_alarm_thread.running)
	{
		on_alarm_timer(0);
		sleep((uint32)arg);
	}
	log_printf("end thread running %d\n",__LINE__);
	pthread_exit(NULL);
}

/*************************************************
  Function:    	start_alarm_timer
  Description: 	开始安防模块的定时器, 1秒定时
  Input:   			
  Output:    	无
  Return:			
  Others:           
*************************************************/
static void start_alarm_timer(void)
{
	//OSSchedLock();
	if (g_alarm_thread.thread == 0)
	{	
		log_printf("%d\n",__LINE__);
		start_timerEx(1, deal_alarm_timer, &g_alarm_thread);
	}
	//OSSchedUnlock();
}

/*************************************************
  Function:    		stop_alarm_timer
  Description: 		停止安防模块的定时器
  Input:   			
  Output:    		无
  Return:			
  Others:           
*************************************************/
static void stop_alarm_timer(void)
{
	//OSSchedLock();
	log_printf("%d %ld\n",__LINE__,g_alarm_thread.thread);
	if (g_alarm_thread.thread)
	{
		g_alarm_thread.running = 0;
		g_alarm_thread.thread = 0;
	}
	// 关闭所有报警声音
	stop_sound_arbistrator(STOP_ALL_SOUND_TYPE);
	log_printf("%d %ld\n",__LINE__,g_alarm_thread.thread);
	//OSSchedUnlock();
}


/*************************************************
  Function:    		change_byte_order
  Description: 		转化字节顺序
  Input:  
  	1.byte          原字节
  Output:    		无
  Return:			变换后的字节顺序
  Others:          
*************************************************/
static uint8 change_byte_order(uint8 byte)
{
	int8 i;
	uint8 new_byte = 0;
	
	for (i = AREA_AMOUNT - 1; i >= 0; i--)
	{
		new_byte |= ((byte >> i) & 0x01) << (AREA_AMOUNT - 1 - i);
	}

	return new_byte;
}

/*************************************************
  Function:    		center_get_area_define
  Description: 		中心管理机获取的防区属性
  Input:
  Output:  
   	1.define 		中心管理机获取的属性
  Return:				
  Others:         	
  					中心管理机获取的防区属性有
  					防区数、启用参数、防区类型
*************************************************/
void center_get_area_define(ALARM_DEFINE_NET_DATA * alarm_define)
{
	uint8 bypass;
	
	if (NULL == alarm_define)
	{
		return;
	}
	
	memset(alarm_define, 0, sizeof(ALARM_DEFINE_NET_DATA));
	alarm_define->area_amount = AREA_AMOUNT * 2;	// 防区数

	bypass = gpAfParam->enable;						// 防区启用参数
	alarm_define->bypass[0] = change_byte_order(bypass);
	alarm_define->bypass[1] = change_byte_order(bypass);

	storage_get_area_type(alarm_define->area_type);	// 防区类型
	storage_get_area_type(alarm_define->area_type+AREA_AMOUNT);	// 防区类型
}

/*************************************************
  Function:    		get_single_area_status
  Description: 		中心管理机所需的防区状态
  Input:
  Output:    		
  Return:			布防或撤防
  Others:           
*************************************************/
static uint8 get_single_area_status(void)
{
	uint8 state = gpAfParam->is_24_hour;
	
	if (SET_DEFEND == gpAfParam->defend_state)
	{
		state = 0xFF;
	}
	else if (PART_DEFEND == gpAfParam->defend_state)
	{
		state = gpAfParam->part_valid;
	}
	return state;
}

/*************************************************
  Function:    		center_get_area_state
  Description: 		中心管理机的获取的安防状态
  Input:			无
  Output:		
  	1.status 		中心管理机获取的安防状态结构体
  Return:			无	
  Others:           
*************************************************/
void center_get_area_state(ALARM_STATUES_NET_DATA * status)
{
	if (NULL == status)
	{
		return;
	}
	
	memset(status, 0, sizeof(ALARM_STATUES_NET_DATA));

	status->defend_state = (DEFEND_STATE)gpAfParam->defend_state;
	status->area_amount = AREA_AMOUNT * 2;
	status->area_state[0] = change_byte_order(get_single_area_status());
	status->alarm_state[0] = change_byte_order(g_alarming_flag);
}

/*************************************************
  Function:    		ipad_get_area_state
  Description: 		ipad获取安防状态
  Input:			无
  Output:		
  	1.status 		ipad获取的安防状态结构体
  Return:			无	
  Others:           
*************************************************/
void ipad_get_area_state(ALARM_STATUES_NET_DATA * status)
{
	uint8 i, alarm_state = 0;
	
	if (NULL == status)
	{
		return;
	}
	
	memset(status, 0, sizeof(ALARM_STATUES_NET_DATA));

	status->defend_state = (DEFEND_STATE)gpAfParam->defend_state;
	status->area_amount = AREA_AMOUNT * 2;
	status->area_state[0] = change_byte_order(get_single_area_status());
	for (i = 0; i < AREA_AMOUNT; i++)
	{
		if (ALARM_SHOW_STATE == (SHOW_STATE)gpAfParam->show_state[i])
		{
			alarm_state |= 0x01 << i;
		}
	}
	status->alarm_state[0] = change_byte_order(alarm_state);
}

/*************************************************
  Function:    		get_time_for_timetype
  Description: 		根据时间类型获取时间
  Input:   		
  	1.sound_type	声音类型
  	2.time_type     时间类型
  Output:    		无
  Return:			
  Others:           
*************************************************/
static uint16 get_time_for_timetype(SOUND_TYPE sound_type, uint8 time_type)
{
	uint16 time = 0;
	switch(sound_type)
	{
		case IN_SOUND_TYPE:
			 switch (time_type)
			 {
				case 0: 
					time = 0;
					break;
				case 1:
					time = 30;
					break;
				case 2: 
					time = 60;
					break;
				case 3:
					time = 60*2;
					break;
				case 4: 
					time = 60*5;
					break;
			 }
			 break;
			 
		case OUT_SOUND_TYPE:
			 switch (time_type)
			 {
				case 0:
					time = 30;
					break;
				case 1: 
					time = 60;
					break;
				case 2:
					time = 60*2;
					break;
				case 3: 
					time = 60*3;
					break;
				case 4:
					time = 60*5;
					break;
			 }
			 break;
			 
		case ALARM_SOUND_TYPE:
			switch (time_type)
			 {
				case 0: 
					time = 60;
					break;					
				case 1:
					time = 3*60;
					break;					
				case 2: 
					time = 5*60;
					break;					
				case 3:
					time = 10*60;
					break;					
				case 4: 
					time = 15*60;
					break;
			 }
			 break;
			 
		default:
			break;
	}

	return time;
}

/*************************************************
  Function:    		stop_sound_arbistrator
  Description: 		安防声音的播放逻辑判断
  Input:   		
  	1.sound_type	声音类型
  Output:    		无
  Return:			
  Others:           
*************************************************/
static void stop_sound_arbistrator(SOUND_TYPE sound_type)
{
	SYS_MEDIA_TYPE media_state = sys_get_media_state();
	SYS_MEDIA_TYPE stop_type = 0;
	switch (sound_type)
	{
		// 进入预警
		case IN_SOUND_TYPE:
		case OUT_SOUND_TYPE:
			if (SYS_MEDIA_YUJING != media_state)
			{
				return;
			}
			break;
			
		case ALARM_SOUND_TYPE:
			if (SYS_MEDIA_ALARM != media_state)
			{
				g_sound_type = NO_SOUND_TYPE;
				return;
			}
			break;
			
		case STOP_ALL_SOUND_TYPE:
			if (SYS_MEDIA_ALARM != media_state && SYS_MEDIA_YUJING != media_state)
			{
				g_sound_type = NO_SOUND_TYPE;
				return;
			}
			break;

		default:
			break;
	}
	
	switch (g_sound_type)
	{
		// 进入预警
		case IN_SOUND_TYPE:
		case OUT_SOUND_TYPE:
			stop_type = SYS_MEDIA_YUJING;
			break;
			
		case ALARM_SOUND_TYPE:
			stop_type = SYS_MEDIA_ALARM;
			break;

		default:
			break;
	}
	log_printf("stop_sound_arbistrator media_state: %d,g_sound_type: %d \n", media_state, g_sound_type);

	#if 0
	//	解决预警声关不掉的情况
	if(media_state == SYS_MEDIA_YUJING )
	{
		stop_type = SYS_MEDIA_YUJING;
	}
	#endif
	
	sys_stop_play_audio(stop_type);
	g_sound_type = NO_SOUND_TYPE;
}

/*************************************************
  Function:    		MediaStopedCallback
  Description: 		安防声音的播放逻辑判断
  Input:   		
  Output:    		无
  Return:			
  Others:           
*************************************************/
static void alarm_time_stop_callback(void)
{
	switch (g_sound_type)
	{
		case IN_SOUND_TYPE:
			//g_delay_time = 0;
			break;
		case ALARM_SOUND_TYPE:
			//g_alarm_time = 0;
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:    		sound_arbistrator
  Description: 		安防声音的播放逻辑判断
  Input:   		
  	1.sound_type	声音类型
  	2.time_type     时间类型
	3.num			防区号 add by wufn 2016.4.22 处理报警后延时防区触发上报问题
  Output:    		无
  Return:			无
  Others:           		无
*************************************************/
static void sound_arbistrator(SOUND_TYPE sound_type, uint8 time_type ,uint8 num)
{
	uint16 time = get_time_for_timetype(sound_type, time_type);
	SYS_MEDIA_TYPE MediaType = sys_get_media_state();
	
	switch(sound_type)
	{
		// 进入预警
		case IN_SOUND_TYPE:
			if (0 == g_delay_time)
			{
				g_delay_time = time;
			}
			else if(g_delay_time > time)			// 取短的时间作为延时时间
			{
				g_delay_time = time;
				return;
			}
			else if (IN_SOUND_TYPE == g_sound_type)
			{
				return;
			}

			if (DIS_DEFEND == gpAfParam->defend_state)
			{
				// 对延时防区单防区布防时，延时防区触发，要响声音
				if (0 == (g_alarm_delay_flag&g_single_defend))
				{
					return;
				}
			}
			// 0 == g_exit_time 保证等到退出预警结束，进入预警才开始响
			log_printf("======g_sound_type :%d====\n",g_sound_type);
			if (ALARM_SOUND_TYPE != g_sound_type && 0 == g_exit_time)
			{
				if (1 == gpAfParam->used_exit_sound)
				{
					//if (SYS_MEDIA_ECHO_OK != sys_start_play_audio(SYS_MEDIA_YUJING, NULL, TRUE, AF_HIT_VOL, NULL, alarm_time_stop_callback))
					if (SYS_MEDIA_ECHO_OK != sys_start_play_audio(SYS_MEDIA_YUJING, (char *)storage_get_yj_path(), TRUE, AF_HIT_VOL, NULL, alarm_time_stop_callback))
					{
						if (g_show_sysEvenHit)
						{
							g_show_sysEvenHit(AS_ENTER_YUJING);
						}
					}
				}
				g_sound_type = IN_SOUND_TYPE;
			}
			break;

		// 退出预警
		case OUT_SOUND_TYPE:
			if (1 == gpAfParam->used_exit_sound)
			{
				sys_start_play_audio(SYS_MEDIA_YUJING, (void *)storage_get_yj_path(), TRUE, AF_HIT_VOL, NULL, alarm_time_stop_callback); 
				sys_set_no_deal(TRUE);
			}
			g_exit_time = time;
			g_sound_type = OUT_SOUND_TYPE;
			break;

		// 报警
		case ALARM_SOUND_TYPE:				//modify by wufn 2016.4.22 
			log_printf("=g_sound_type: %d==\n",g_sound_type);
			if (ALARM_SOUND_TYPE != g_sound_type)
			{
				if(0 == ((g_can_hear >> num) & 0x01))		
				{
					sys_start_play_audio(SYS_MEDIA_ALARM, storage_get_bj_path(), TRUE, AF_HIT_VOL, NULL, alarm_time_stop_callback); 
				}
  				g_alarm_time = time;
				g_sound_type = ALARM_SOUND_TYPE;
			}
			else 
			{
				if (time > g_alarm_time)
				{
					g_alarm_time = time;
				}
				log_printf("==MediaType :%d===\n",MediaType);
				if(SYS_MEDIA_ALARM != MediaType)
				{
					log_printf("==g_can_hear=: %d====(g_can_hear>>num) & 0x01):%d\n",g_can_hear,((g_can_hear>>num) & 0x01));
					if(0 == ((g_can_hear>>num) & 0x01))
					{
						sys_start_play_audio(SYS_MEDIA_ALARM, storage_get_bj_path(), TRUE, AF_HIT_VOL, NULL, alarm_time_stop_callback); 
					}
				}
				else
				{
					return;
				}
			}
			break;

		default:
			break;
	}
	
	log_printf("sound_arbistrator g_alarm_time: %d, g_sound_type; %d  media_state: %d\n", g_alarm_time, g_sound_type, sys_get_media_state());
	start_alarm_timer();
}

/*************************************************
  Function:    		get_min_delay_time
  Description: 		获得有触发延时防区的最小值
  Input:   			无
  Output:    		无
  Return:			无			
  Others:
*************************************************/
static uint8 get_min_delay_time(void)
{
	uint8 i;
	uint8 tmp = 254;
	
	for (i = 0; i < AREA_AMOUNT; i++)
	{
		if (0 != ((g_alarm_delay_flag>>i) & 0x01))
		{
			if (gpAfParam->delay_time[i] < tmp)
			{
				tmp = gpAfParam->delay_time[i];
			}
		}
	}
	return tmp;
}

/*************************************************
  Function:    		on_alarm_timer
  Description: 		安防模块的定时器处理函数
  Input:   			
  Output:    		无
  Return:			
  Others:           
*************************************************/
static void on_alarm_timer(void * arg)
{
	// 第一防区单独处理
	if (1 == g_sos_state && (g_sos_time > 0)) 
	{
		log_printf("g_sos_time: %d\n",g_sos_time);
		g_sos_time--;
		if (0 == g_sos_time)
		{
			g_sos_state = 0;
			g_alarming_flag |= 0x01;
			alarm_deal(AS_ALARM_PROC, 0);
		}
	}
	else
	{
		g_sos_time = 0;		
	}

	// 停止报警声处理
	if (ALARM_SOUND_TYPE == g_sound_type)
	{
		log_printf("g_alarm_time: %d\n",g_alarm_time);
		if (g_alarm_time > 0)
		{
			g_alarm_time--;
		}
		else
		{
			stop_sound_arbistrator(ALARM_SOUND_TYPE);
		}
	}
	
	// 退出预警处理, 8个防区共用一个时间
	if (g_exit_time > 0)
	{
		g_exit_time--;
		log_printf("g_exit_time %d\n",g_exit_time);

		if ((0 != g_alarm_time) || 0 == g_exit_time)			//外出后触发延时有报警的话立即上报
		{
			if (0 != g_exit_time)
			{
				g_exit_time = 0;
			}
			if (OUT_SOUND_TYPE == g_sound_type)
			{
				stop_sound_arbistrator(OUT_SOUND_TYPE);
			}
			// 退出预警时间结束,此时如果有延时防区触发,则进入预警提示
			if (g_alarm_delay_flag > 0)
			{
				sound_arbistrator(IN_SOUND_TYPE, get_min_delay_time(), 0);
			}
		}
	}
	// 进入预警处理
	else if (0 == g_delay_time)						// 进入预警时间到
	{
	}
	else
	{
		if (DIS_DEFEND == gpAfParam->defend_state)
		{
			// 对延时防区未单防区布防时，不计时
			//if (0 == (g_alarm_delay_flag&g_single_defend))
			// 修改7-27，单防区布防时，撤销触发,g_alarm_delay_flag将为0
			if (0 == (g_single_defend))
			{
				return;
			}
		}
		log_printf("g_delay_time:%d\n",g_delay_time);
		g_delay_time--;
		if ((0 == g_delay_time) || (0 != g_alarm_time))	
				// 进入预警时间到
		{			
			if (0 != g_delay_time)
			{
				g_delay_time = 0;
			}
			// 预警时间到则停止预警声
			if (IN_SOUND_TYPE == g_sound_type)
			{
				//sys_stop_play_audio();
				stop_sound_arbistrator(IN_SOUND_TYPE);
			}

			// 如预警防区还处在触发状态,则将预警转为正式报警
			if (0 != g_alarm_delay_flag)
			{
				uint8 i;
				for (i = 0; i < AREA_AMOUNT; i++)
				{
					if ((1 == ((g_alarm_delay_flag>>i) & 0x01))&& (0 == ((g_alarming_flag>>i) & 0x01)))
					{
						// 撤防的状态，单个防区布防的防区才转
						if (DIS_DEFEND == gpAfParam->defend_state)
						{
							if (0 == (0x01&(g_single_defend>>i)))
							{
								continue;
							}
						}
						if (1 == ((g_resume_delay_flag>>i)&0x01))
						{
							g_alarm_delay_flag &= ~(1>>i);
							g_resume_delay_flag &= ~(1>>i);
						}
						g_alarming_flag |= 1<<i;
						alarm_deal(AS_ALARM_PROC, i);
					}
				}
			}
		}
	}

	if (0 == g_sos_time && 0 == g_exit_time && 0 == g_delay_time && 0 == g_alarm_time)
	{
		log_printf("%d\n",__LINE__);
		stop_alarm_timer();
		return;
	}
}

/*************************************************
  Function:			judge_can_be_defend
  Description: 		获取是否能布防
  Input: 
  Output:			无
  Return:			无
  Others:			无
*************************************************/
uint8 judge_can_be_defend(void)
{
	uint8 i;
	uint8 tmp = g_judge_can_defend;
	
	// 为了是否能布防的判断
    for (i = 0; i < AREA_AMOUNT; i++)
    {
 		if (gpAfParam->delay_time[i] > 0)
 		{
 			tmp &= ~(1<<i);
 		}
    }
	return tmp;
}

/*************************************************
  Function:			check_defend_oper
  Description:		安防状态处理
  Input: 
  	1.state			安防状态
  	2.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE check_defend_oper(DEFEND_STATE state, EXECUTOR_E exec)
{
	if (EXECUTOR_LOCAL_HOST != exec)
	{
		// 判断是否有远程控制权限
		if (0 == ((gpAfParam->remote_set >> state) & 0x01))
		{
			return AF_NO_REMOTE_CTRL;
		}

		// 判断报警模块是否启用
		if (FALSE == storage_get_extmode(EXT_MODE_ALARM))
		{
			return AF_SETTING_ERR;
		}
	}
	
	if (DIS_DEFEND == state)
	{
		return AF_SUCCESS;
	}
	
	if (DIS_DEFEND != gpAfParam->defend_state)
	{
		if (gpAfParam->defend_state == state)
		{
			return AF_STATE_SAME;
		}
		else
		{
			return AF_CANNOT_OVERLEAP;
		}
	}
	else
	{
		// 触发后，可能被设置使能或盘路，所以要重新判断下
		if (g_alarm_time > 0)
		{
			sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_fqcf_path(), FALSE, AF_HIT_VOL, NULL, NULL); 
			return 	AF_BAOJINGNOBUFANG;
		}
		else if(0 != judge_can_be_defend())
		{
			sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_fqcf_path(), FALSE, AF_HIT_VOL, NULL, NULL); 
			return  AF_FQCHUFA;
		}
	}

	return AF_SUCCESS;
}

/*************************************************
  Function:			on_alarm_sound
  Description:		安防声音回调
  Input: 		
  Output:			无
  Return:			无
  Others:			无
*************************************************/
static void *on_alarm_sound(int32 param1, int32 param2 ,int32 state)
{
	log_printf("param1 = %d,param2 = %d state = %d\n",param1,param2,state);
	if (FALSE == state)
	{
		
	}
	else if (100 == param2)
	{
		g_exit_time = 0;
		
		if (ALARM_SOUND_TYPE == g_sound_type)
		{
 			return 0;
		}
		// 布防后马上撤防，有可能会响该声音，所以要做SET_DEFEND == gpAfParam->defend_state判断
		if (SET_DEFEND == gpAfParam->defend_state)
		{
			log_printf("%d\n",__LINE__);
			sound_arbistrator(OUT_SOUND_TYPE, gpAfParam->exit_alarm_time, 0);
		}
	}

	return 0;
}

/*************************************************
  Function:    		alarm_gui_callback
  Description: 		执行安防回调
  Input:
  	1.Param1		参数1		
  	2.Param2		参数2
  Output: 			无   		
  Return:			无
  Others:           
*************************************************/
void alarm_gui_callback(int32 Param1, int32 Param2)
{
	if (g_alarm_callback)
	{
		g_alarm_callback(Param1, Param2);
	}
}

/*************************************************
  Function:			alarm_clear_alerts_operator
  Description:		清除警示操作
  Input: 		
  Output:			无
  Return:			
  Others:			无
*************************************************/
void alarm_clear_alerts_operator(void)
{
	uint8 i;
	uint8 state;

	state = gpAfParam->is_24_hour ;
	for (i = 0; i < AREA_AMOUNT; i++)
	{

		if(gpAfParam->show_state[i] == ALARM_SHOW_STATE)
		{
			// 处理非24小时防区触发报警后撤防探头未恢复清除警示的状态闪烁 add by wufn 2015-8-15 
			if (0 == ((state>>i)&0x01))
			{
				if(1 == g_Alarm_Set_Operator)
				{
					gpAfParam->show_state[i] = NORM_SHOW_STATE;
					continue;
				}

				if(1 == ((g_Alarm_No24clear>>i) & 0x01))
				{
					continue;
				}
				else
				{
					g_Alarm_No24clear |= (1<<i);
					continue;
				}
				g_Alarm_No24touch&= ~(1<<i);
			}
			else
			{
				gpAfParam->show_state[i] = NORM_SHOW_STATE;
			}
		}
		
		else 
		{
			gpAfParam->show_state[i] = NORM_SHOW_STATE;				
		}
	}
	
	g_Alarm_Set_Operator = 0;
	
	// 上面的代码只是把所有防区设为非报警状态,真正的判断还是下面的函数
	set_area_show_info(1);

	if (DIS_DEFEND == gpAfParam->defend_state)
	{
		//g_alarm_time = 0;
		if (0 == g_alarming_flag)
		{
			sys_set_hint_state(SYS_HINT_ALARM_WARNING, FALSE);	// 关闭报警灯
		}
		g_whole_alarm_state = 0;
		g_alarming_flag = 0;									// 撤防后24小时防区将可重新触发				
	}

}

/*************************************************
  Function:			alarm_set_operator
  Description:		布防操作
  Input: 
   	1.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_set_operator(EXECUTOR_E exec)
{
	AF_RETURN_TYPE retype = check_defend_oper(SET_DEFEND, exec);
 	if (AF_SUCCESS != retype)
 	{
		return retype;
	}

	gpAfParam->defend_state = SET_DEFEND;
	g_Alarm_Set_Operator = 1;						
	alarm_clear_alerts_operator();
	g_alarm_delay_flag = 0;							// 只有在不断上报防区状态机制下下能这样用
	g_resume_delay_flag = 0;
	if (EXECUTOR_LOCAL_HOST == exec)				// 本机控制,播放声音
	{
		g_exit_time = get_time_for_timetype(OUT_SOUND_TYPE, gpAfParam->exit_alarm_time);  
		sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_yj_wc_path(), FALSE, AF_HIT_VOL, on_alarm_sound, NULL); 
	}
	else
	{
		alarm_gui_callback(BAOJING_FRESH, 0);		// 刷新界面
	}

	set_event_report(exec);
	storage_add_afcz_record(SET_DEFEND, exec);		// 写安防操作事件
	sys_set_hint_state(SYS_HINT_ALARM_STATE, TRUE);	

	#ifdef _JD_MODE_
	linkage_alarm_scene(SET_DEFEND);				// add by luofl 2011-08-03 布防联动
	#endif
	hw_get_zone_status();		
	return AF_SUCCESS;
}

/*************************************************
  Function:			alarm_single_set_operator
  Description:		单防区布防操作
  Input: 		
  	1.num			防区号
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_single_set_operator(uint8 num)
{
	uint8 state, tmp, i;
	
	if (DIS_DEFEND != gpAfParam->defend_state)
	{
		return AF_SETTING_ERR;
	}
	
	state = gpAfParam->is_24_hour | gpAfParam->isolation | (~gpAfParam->enable);
	// 隔离,未启用,24小时防区不处理
	if (1 == ((state>>num)&0x01))
	{
		return AF_NO_REMOTE_CTRL;
	}
	
	tmp = judge_can_be_defend();
	if (1 == ((tmp>>num)&0x01))
	{
		return AF_FQCHUFA;
	}
	g_single_defend |= 1<<num;
	set_area_show_info(1);

	// 延时防区，则要开始计时
	i = num;
	if (gpAfParam->delay_time[i] > 0 && (1 == ((g_alarm_delay_flag>>i) & 0x01)))
	{
		g_alarm_delay_flag &= ~(1<<i);				// 置防区预警标志
	}
	for (i = 0; i < AREA_AMOUNT; i++)
	{
		if (NORM_SHOW_STATE == gpAfParam->show_state[i])
		{
			break;
		}
	}
	if (AREA_AMOUNT == i)
	{
		alarm_set_operator(EXECUTOR_LOCAL_HOST);
		alarm_gui_callback(BAOJING_FRESH, 0);		// 刷新界面
	}
	
	return AF_SUCCESS;
}

/*************************************************
  Function:			alarm_unset_operator
  Description:		撤防命令
  Input: 
  	1.exec			执行者
  	2.force_type    1:挟持 0:正常
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_unset_operator(EXECUTOR_E exec, uint8 force_type)
{
	AF_RETURN_TYPE retype;
	
	if (1 == force_type)
	{
		force_alarm_report();
	}

	retype = check_defend_oper(DIS_DEFEND, exec);
 	if (AF_SUCCESS != retype)
 	{
		return retype;
	}

	//alarm_set_unset();
	stop_alarm_timer();
	g_alarm_delay_flag = 0;
	g_resume_delay_flag = 0;
	g_exit_time = 0;
	g_delay_time = 0;
	g_single_defend = 0;							// 原单防区布防恢复原状态
	// 延时防区时间停止，所有声音停止，其他数据的恢复
	
	g_sound_type = NO_SOUND_TYPE;

 	if (EXECUTOR_LOCAL_HOST == exec)				// 本机控制,播放声音
	{
		if (1 == g_whole_alarm_state)				// "注意有警情"
		{
			sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_yjq_path(), FALSE, AF_HIT_VOL, NULL, NULL); 
			g_whole_alarm_state = 0;
		}
		else										// "警戒已解除"
		{
			if (gpAfParam->defend_state != DIS_DEFEND)
			{
				sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_jjyjc_path(), FALSE, AF_HIT_VOL, NULL, NULL); 
			}
		}
	}
		
	g_whole_alarm_state = 0;					// 远程撤防也置为 0 
	gpAfParam->defend_state = DIS_DEFEND;
	set_area_show_info(1);
	alarm_gui_callback(BAOJING_FRESH,0);			// 刷新界面

	g_alarm_time = 0;

	unset_event_report(exec);						// 撤防操作事件上报
	storage_add_afcz_record(DIS_DEFEND, exec);		// 写安防操作事件
	if (0 == g_alarming_flag)
	{
		sys_set_hint_state(SYS_HINT_ALARM_STATE, FALSE);// 关闭报警灯
	}
    g_alarming_flag = 0;
	
	//ld_action_deal(LD_ACTION_AF_HOME,0,NULL);		// 撤防联动	
	#ifdef _JD_MODE_
	linkage_alarm_scene(DIS_DEFEND);				// add by luofl 2011-08-03 撤防联动
	#endif
	hw_get_zone_status();							//add by wufn 主动获取状态2015-9-07
	return AF_SUCCESS;
}

/*************************************************
  Function:			alarm_partset_operator
  Description:		局防操作
  Input: 	
  	1.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_partset_operator(EXECUTOR_E exec)
{
	AF_RETURN_TYPE retype = check_defend_oper(PART_DEFEND, exec);
 	if (AF_SUCCESS != retype)
 	{
		return retype;
	}
	gpAfParam->defend_state = PART_DEFEND;
	g_Alarm_Set_Operator = 1;						// 触发没布防的延时防区，夜间时图标处理 add by wufn
	alarm_clear_alerts_operator();
	g_alarm_delay_flag = 0;							// 只有在不断上报防区状态机制下下能这样用
	g_resume_delay_flag = 0;

	if (EXECUTOR_LOCAL_HOST == exec)				// 本机控制,播放声音
	{
		sys_start_play_audio(SYS_MEDIA_YUJING, storage_get_wanan_path(), FALSE, AF_HIT_VOL, NULL, NULL); 
	}
	else
	{
		alarm_gui_callback(BAOJING_FRESH, 0);		// 刷新界面
	}

	partset_event_report(exec);						// 局防操作事件上报
	storage_add_afcz_record(PART_DEFEND, exec);		// 写安防操作事件
	sys_set_hint_state(SYS_HINT_ALARM_STATE, TRUE);	// 打开报警灯
	hw_get_zone_status();		
	return AF_SUCCESS;
}

// 增加布防、撤防、局防操作同步到分机的操作处理 add by luofl 2011-4-27
#ifdef _ALARM_OPER_SYNC_
/*************************************************
  Function:			alarm_check_devno
  Description:		检查是不是同一个房号的分机
  Input: 			无
  Output:			无
  Return:			TRUE, FALSE
  Others:			无
*************************************************/
uint8 alarm_check_devno(const PRECIVE_PACKET RecvPacket)
{
	EXECUTOR_E exec;
	PNET_HEAD head;
	PFULL_DEVICE_NO devparam;
	uint32 remoteDevno1, remoteDevno2, localDevno1, localDevno2;

	head = (PNET_HEAD)(RecvPacket->data + MAIN_NET_HEAD_SIZE);
	remoteDevno1 = head->SrcDeviceNo.DeviceNo1;
	remoteDevno2 = head->SrcDeviceNo.DeviceNo2  / 10;		// 去掉分机号
 	devparam = storage_get_devparam();
 	localDevno1 = devparam->DeviceNo.DeviceNo1;
 	localDevno2 = devparam->DeviceNo.DeviceNo2  / 10; 	// 去掉分机号
 	
 	exec = (EXECUTOR_E)(* (RecvPacket->data + NET_HEAD_SIZE));

 	if ((exec == EXECUTOR_REMOTE_DEVICE) && (localDevno1 == remoteDevno1))
	{
		if (localDevno2 == remoteDevno2)
		{
			return TRUE;
		}
 	}
 	return FALSE;
}

/*************************************************
  Function:			alarm_set_response
  Description:		对布防操作事件命令的应答
  Input: 			无
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_set_response(void)
{
	gpAfParam->defend_state = SET_DEFEND;
	alarm_clear_alerts_operator();
	g_alarm_delay_flag = 0;					// 只有在不断上报防区状态机制下下能这样用
	g_resume_delay_flag = 0;

	alarm_gui_callback(BAOJING_FRESH, 0);	// 刷新界面
	#ifdef _ALARM_OPER_SYNC_
	sync_comm_event_send(CMD_RP_SET_EVENT, EXECUTOR_LOCAL_HOST);
	#endif
	storage_add_afcz_record(SET_DEFEND, EXECUTOR_REMOTE_DEVICE);	// 写安防操作事件
	sys_set_hint_state(SYS_HINT_ALARM_STATE, TRUE);	
}

/*************************************************
  Function:			alarm_unset_response
  Description:		对撤防操作事件命令的应答
  Input: 			无
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_unset_response(void)
{
	stop_alarm_timer();
	g_alarm_delay_flag = 0;
	g_resume_delay_flag = 0;
	g_exit_time = 0;
	g_delay_time = 0;
	g_single_defend = 0;							// 原单防区布防恢复原状态
	// 延时防区时间停止，所有声音停止，其他数据的恢复
	gpAfParam->defend_state = DIS_DEFEND;
	set_area_show_info(1);

	g_sound_type = NO_SOUND_TYPE;
	alarm_gui_callback(BAOJING_FRESH, 0);			// 刷新界面
	g_alarm_time = 0;

	storage_add_afcz_record(DIS_DEFEND, EXECUTOR_REMOTE_DEVICE);	// 写安防操作事件
	if (0 == g_alarming_flag)
	{
		sys_set_hint_state(SYS_HINT_ALARM_STATE, FALSE);// 关闭报警灯
	}

	#ifdef _ALARM_OPER_SYNC_
	sync_comm_event_send(CMD_RP_UNSET_EVENT, EXECUTOR_LOCAL_HOST);  //同步撤防操作上报
	#endif

	g_alarming_flag = 0;
}

/*************************************************
  Function:			alarm_partset_response
  Description:		对局防操作事件命令的应答
  Input: 			无
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_partset_response(void)
{
	gpAfParam->defend_state = PART_DEFEND;
	alarm_clear_alerts_operator();
	g_alarm_delay_flag = 0;							// 只有在不断上报防区状态机制下下能这样用
	g_resume_delay_flag = 0;

	alarm_gui_callback(BAOJING_FRESH, 0);			                  // 刷新界面
	storage_add_afcz_record(PART_DEFEND, EXECUTOR_REMOTE_DEVICE);	  // 写安防操作事件
	sys_set_hint_state(SYS_HINT_ALARM_STATE, TRUE);	                  // 打开报警灯
	#ifdef _ALARM_OPER_SYNC_
	sync_comm_event_send(CMD_RP_PARTSET_EVENT, EXECUTOR_LOCAL_HOST);  // 局防事件同步上报
	#endif
}
#endif

/*************************************************
  Function:			port_status_callback
  Description: 		防区状态回调,从硬件驱动中回调
  Input: 
	1.rcvbuf		防区状态,一个位代表一个防区
  Output:			无
  Return:			无
  Others:			
*************************************************/
void port_status_callback(uint8 PortLevel)
{
	uint8 i, area_num = AREA_AMOUNT;
	uint8 touch;									// 记录触发防区
	uint8 is_alarm = 0;
	uint8 un_use = 0xFF;							//用于处理隔离和未启用的状态保护
	g_Alarm_No24clear = 0;							
	g_Alarm_No24touch = 0;                          

	// 2011-10-12 更改为第8防区当门铃使用只支持常开,按下响铃声.一直按住只响一次
	if (linkage_8area_door() == TRUE)
	{
		area_num = AREA_AMOUNT - 1;
		if ((PortLevel & 0x80) == 0)
		{
			if (!g_door_ring)
			{
				g_door_ring = 1;					// 门前铃按下时响铃声
				linkage_play_door_ring();
			}
		}
		else
		{
			g_door_ring = 0;						// 门前铃抬起
		}
	}
	// yanjl 2014-6-24 增加第八防区联动撤防
	#ifdef _LINKAGE_78AREA_
	else if (linkage_8area_unset() == TRUE )
	{
		area_num = AREA_AMOUNT - 1;
		if ((PortLevel & 0x80) == 0)
		{
			if (!g_8area_unset)
			{
				g_8area_unset = 1;
				alarm_unset_operator(EXECUTOR_LOCAL_HOST,0);
			}	
		}
		else
		{
			g_8area_unset = 0;
		}
	}
	#endif
	else
	{
		area_num = AREA_AMOUNT;
	}
	// yanjl 2014-6-24 增加第七防区联动布防
	#ifdef _LINKAGE_78AREA_
	if (linkage_7area_set() == TRUE )
	{
		area_num = AREA_AMOUNT - 2;
		if ((PortLevel & 0x40) == 0 )
		{
			if (!g_7area_set)   
			{
				g_7area_set = 1;
				alarm_set_operator(EXECUTOR_LOCAL_HOST);
				alarm_gui_callback(BAOJING_FRESH,0);// 刷新界面
			}				
		}
		else
		{
			g_7area_set = 0;
		}
	}
	#endif
	
	if (FALSE == storage_get_extmode(EXT_MODE_ALARM))
	{
		return;
	}
	
	touch = PortLevel ^ gpAfParam->finder_state;	// 与探头类型相同状态不处理
	log_printf("PortLevel:%x touch:%x\n", PortLevel, touch);
	// 屏蔽未启用和暂时隔离防区
	touch = touch & (gpAfParam->enable) & (~gpAfParam->isolation);
	// 未启用和隔离的防区改变状态时不处理
	un_use = un_use & (gpAfParam->enable) & (~gpAfParam->isolation);					// add by wufn 2015.9.15
	g_judge_can_defend = touch;

	// 第一防区的处理
	if (1 == (touch & 0x01))
	{
		// modi by luofl 2013-09-03解决2-8防区触发导至1防区报警不上报中心
		if ((0 == g_sos_state) && (0 == (g_alarming_flag&0x01)))
		{
			g_sos_time = SOS_TIME;
			g_sos_state = 1;
			start_alarm_timer();
		}
	}
	else
	{
		g_alarming_flag &= 0xFE;
		g_sos_state = 0;
	}

	// 二到八防区处理
	//for (i = 1; i < AREA_AMOUNT; i++)
	for (i = 1; i < area_num; i++)
	{
		if (1 == ((touch>>i) & 0x01))
		{
			// 已经发生报警或预警,则不重复处理
			if (1 == ((g_alarming_flag>>i) & 0x01) || (1 == ((g_alarm_delay_flag>>i) & 0x01)))
			{
				touch &= ~(1<<i);
				if (1 == ((g_alarm_delay_flag>>i) & 0x01))
				{
					// 延时防区触发后，排除后，再次触发情况
					g_resume_delay_flag &= ~(1<<i);
				}
				continue;
			}
			// 进入预警处理
			if (gpAfParam->delay_time[i] > 0 && (0 == ((g_alarm_valid_flag>>i) & 0x01)))
			{
				// 局防无效的防区，预警不处理
				if ((PART_DEFEND == gpAfParam->defend_state) && (0 ==((gpAfParam->part_valid>>i)&0x01)))
				{
					//局防无效防区的闪烁处理add by wufn 2016.5.11
					gpAfParam->show_state[i] = ALARM_SHOW_STATE;
					g_Alarm_No24touch |= (1<<i);
					touch &= ~(1<<i);
					continue;
				}
				//  撤防的防区不处理
				else if ((DIS_DEFEND == gpAfParam->defend_state) && (0 ==((g_single_defend>>i)&0x01)))
				{
					// 增加撤防状态触发 刷新闪烁  add by wufn
					gpAfParam->show_state[i] = ALARM_SHOW_STATE;
					//if(0 == ((g_Alarm_Unset>>i) & 0x01))					// 防止主动获取状态时红黄闪烁状态被改变
					{
						g_Alarm_No24touch |= (1<<i);
					}
					touch &= ~(1<<i);
					continue;
				}
				g_alarm_delay_flag |= 1<<i;			// 置防区预警标志
			}
			// 进入报警处理
			else
			{
				if (1 == ((g_alarm_valid_flag>>i) & 0x01))
				{
					g_alarming_flag |= (1<<i);		// 置防区报警标志
					g_Alarm_No24clear &= ~(1<<i);     // 将清楚警示标志置为0
				}
				else
				{	
					// 增加撤防状态触发 刷新闪烁  add by wufn
					gpAfParam->show_state[i] = ALARM_SHOW_STATE;
					//if(0 == ((g_Alarm_Unset>>i) & 0x01))					// 防止主动获取状态时红黄闪烁状态被改变
					{
						g_Alarm_No24touch |= (1<<i);
					}
					continue;
				}
			}
			is_alarm = 1;							// 有新的防区触发
		}
		else
		{
			//处理撤防状态恢复  add by wufn
			if (DIS_DEFEND == gpAfParam->defend_state)
			{
				if (1 == ((un_use>>i) & 0x01))
				{
					if(0 == ((gpAfParam->is_24_hour>>i)&0x01))
					{
						if (0 == ((g_single_defend>>i) & 0x01))
						{
							gpAfParam->show_state[i] = NORM_SHOW_STATE;
						}
					}
				}
			}
			
			//局防无效防区的闪烁处理add by wufn 2016.5.11
			if (PART_DEFEND == gpAfParam->defend_state)
			{
				if (1 == ((un_use>>i) & 0x01))
				{
					if(0 == ((gpAfParam->is_24_hour>>i)&0x01))
					{
						if(0 ==((gpAfParam->part_valid>>i)&0x01))
						{
							gpAfParam->show_state[i] = NORM_SHOW_STATE;
						}
					}
				}
			}

			g_alarming_flag &= ~(1<<i);				// 清除防区报警标志

			// 如有延时防区触发, 在预警其间探头状态恢复则清除预警标志
			if ((1 == ((g_alarm_delay_flag>>i) & 0x01)) && gpAfParam->delay_time[i] > 0)
			{
				// 只在退出预警时间内恢复，才不处理延时报警，否则只能通过撤防才能取消延时报警
				if (g_exit_time > 0)
				{
					if (0 == g_alarm_delay_flag)
					{
						g_delay_time = 0;					
					}
					g_alarm_delay_flag &= ~(1<<i);
				}
				else 
				{
					g_resume_delay_flag |= (1<<i);
					log_printf("g_resume_delay_flag:%x\n",g_resume_delay_flag);
				}
				// 10-7-21,修改为预警声只在撤防时，才可停止
				#if 0
				if (0 == g_alarm_delay_flag)
				{
					g_delay_time = 0;					
				}
				#endif
			}
		}
	}

	if (1 == is_alarm)
	{
		alarm_deal(AS_ALARM_DEAL_MSGID, touch);
	}
	//g_Alarm_Unset = 0; 
	alarm_gui_callback(BAOJING_FRESH, 0);
}

/*************************************************
  Function:			deal_alarm_func
  Description: 		处理报警事件
  Input: 
	1.num			防区号
  Output:			无
  Return:			无
  Others:			无
*************************************************/
static void deal_alarm_func(uint8 num)
{
	storage_add_afbj_record(num+1, 1);				// 记录报警事件
	if (0 == num)
	{
		sos_alarm_report();							// 上报紧急求救事件
		// add by luofl 2014-03-10第一次烧写第一防区也要闪烁
		if (gpAfParam->b_write)
		{
			return;
		}
	}
	else
	{
		break_alarm_report(num+1);					// 上报防区触发事件
	}
	
	g_whole_alarm_state = 1;
	gpAfParam->show_state[num] = ALARM_SHOW_STATE;
	SaveRegInfo();

	sys_set_hint_state(SYS_HINT_ALARM_WARNING, 1);	// 闪报警灯

	if (1 == ((gpAfParam->can_hear>>num) & 0x01))	// 可听
	{
		if(1 == ((g_can_hear>>num)& 0x01))
		{
			g_can_hear &= ~(1<<num);
		}
		sound_arbistrator(ALARM_SOUND_TYPE, gpAfParam->alarm_time, num);
	}
	else
	{
		if(0 == ((g_can_hear>>num)& 0x01))
		{
			g_can_hear |= (1<<num);//不可听
		}
		sound_arbistrator(ALARM_SOUND_TYPE, gpAfParam->alarm_time, num);
	}
	
	if (1 == ((gpAfParam->can_see>>num) & 0x01))	// 可见
	{
		alarm_gui_callback(BAOJING_SHOW, 0);
	}
	else
	{
		alarm_gui_callback(BAOJING_FRESH, 0);
	}
}

/*************************************************
  Function:			alarm_logic_init
  Description:		报警逻辑初始化
  Input: 		
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_logic_init(void)
{
	#ifdef _DOOR_PHONE_
	init_alarm_callback(port_status_callback);		// 初始化硬件防区状态回调
	#else
	init_ioctrl_callback(port_status_callback);
	#endif
	set_area_show_info(0);
}

/*************************************************
  Function:    		alarm_init_gui_callback
  Description: 		安防GUI 回调函数
  Input:
  	1.func			回调函数
  Output:    		无
  Return:			无
  Others:           
*************************************************/
void alarm_init_gui_callback(ALARMGUI_CALLBACK func, SHOW_SYSEVENHIT func1)
{	
	g_alarm_callback = func;	
	g_show_sysEvenHit = func1;
}

/*************************************************
  Function:    		logic_get_area_isolation
  Description: 		获取防区隔离参数
  Input:
  Output:   
  Return:			暂时隔离参数
  Others:           
*************************************************/
uint8 logic_get_area_isolation(void)
{
	return gpAfParam->isolation;
}

/*************************************************
  Function:    		logic_set_area_isolation
  Description: 		设置防区暂时隔离参数
  Input:
  	1.iso_info		1- 暂时隔离
  Output:    		
  Return:			false:失败 true:成功
  Others:           每一位代表一个防区
*************************************************/
uint8 logic_set_area_isolation(uint8 iso_info)
{
	gpAfParam->isolation = iso_info;
	// 10-7-21，修改为可上报
	bypass_event_report(8, change_byte_order(iso_info));
	return set_area_show_info(1);
}

/*************************************************
  Function:    		logic_get_part_valid
  Description: 		获取防区局防有效参数
  Input:
  Output:   
  Return:			局防有效参数	
  Others:           
*************************************************/
uint8 logic_get_part_valid(void)
{
	return gpAfParam->part_valid;
}

/*************************************************
  Function:    		logic_set_part_valid
  Description: 		设置防区局防有效参数
  Input:
  	1.valid			局防有效参数
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 logic_set_part_valid(uint8 valid)
{
	if (gpAfParam->is_24_hour != (gpAfParam->is_24_hour & valid))
	{
		return 2;
	}
	
	gpAfParam->part_valid = valid;
 	return set_area_show_info(1);
}

/*************************************************
  Function:    		logic_get_alarm_param
  Description: 		获得安防参数
  Input:
  Output:    		
  	1.param_info	安防参数
  Return:				
  Others:    		       
*************************************************/
void logic_get_alarm_param(uint8 * param_info)
{
	if (NULL == param_info)
	{
		return;
	}

	memcpy(param_info, (uint8 *)(gpAfParam), sizeof(AF_FLASH_DATA));
}

/*************************************************
  Function:    		logic_set_alarm_param
  Description: 		设置安防参数
  Input:
   	1.param_info	安防参数
  Output:    		
  Return:			false:失败 true:成功				
  Others:           
*************************************************/
uint8 logic_set_alarm_param(uint8 *param_info)
{
	if (NULL == param_info)
	{
		return FALSE;
	}
	
	memcpy((uint8*)(gpAfParam), param_info, sizeof(AF_FLASH_DATA));
	alarm_define_report();
	return set_area_show_info(1);
}

/*************************************************
  Function:			alarm_deal
  Description: 		报警处理函数
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_deal(SYS_MSG_ID id, uint8 msg)
{
	uint8 touch, i;
	touch = msg;
	
	switch(id)
	{
		case AS_ALARM_DEAL_MSGID:
			for (i = 1; i < AREA_AMOUNT; i++)
			{
				if (1 == ((touch>>i)&0x01))
				{
					// 进入预警处理
					if (1 == ((g_alarm_delay_flag>>i) & 0x01))
					{
						sound_arbistrator(IN_SOUND_TYPE, gpAfParam->delay_time[i], 0);
					}
					// 进入报警处理
					else
					{
						if (1 == ((g_alarming_flag>>i) & 0x01))
						{
							alarm_deal(AS_ALARM_PROC, i);
						}
					}
				}
			}
			break;

		case AS_ALARM_PROC:
			deal_alarm_func(touch);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:    		alarm_get_24clear_param  
  Description: 		获得非24小时防区清除后标志
  Input:
  Output:    		
  	1.param	非24小时防区清除后标志参数
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 alarm_get_24clear_param(void)
{
	return g_Alarm_No24clear;
}

/*************************************************
  Function:    		alarm_get_24clear_param  
  Description: 		获得非24小时防区触发
  Input:
  Output:    		
  	1.param	非24小时防区触发标志参数
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 alarm_get_no24touch_param(void)
{
	return g_Alarm_No24touch;
}

/*************************************************
  Function:    	logic_get_g_alarming_flag  
  Description: 		获取整机报警标志
  Input:
  Output:    		
  	1.param	整机报警标志
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 logic_get_g_whole_alarm_state_param(void)
{
	return 	g_whole_alarm_state;
}

#if 0
/*************************************************
  Function:			alarm_set_unset
  Description:		设置撤防后红黄闪烁保护标志
  Input: 
  Output:			无
  Return:			无
  Others:			add by wufn	
*************************************************/
void alarm_set_unset(void)
{
	uint8 i;
	uint8 state;
	
	state = gpAfParam->is_24_hour ;
	for (i = 0; i < AREA_AMOUNT; i++)
	{
		if(gpAfParam->show_state[i] == ALARM_SHOW_STATE)
		{
			if (0 == ((state>>i)&0x01))		
			{
				if((1 == ((g_single_defend>>i) & 0x01)))
				{
					g_Alarm_Unset |= (1<<i);
				}
				else
				{
					g_Alarm_Unset &= ~(1<<i);
				}
			}
		}
	}
}
#endif

