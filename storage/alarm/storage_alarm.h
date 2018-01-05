/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	storage_alarm.h
  Author:   	罗发禄
  Version:   	2.0
  Date: 		2010-4-23
  Description:  安防存储头文件
  
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#ifndef __AU_STORAGE_ALARM_H__
#define __AU_STORAGE_ALARM_H__

//#include "logic_include.h"

#define AREA_AMOUNT 			8
#define TELNUM_LEN          	15
#define AF_REC_MAX 	  			20					// 安防事件最大记录数

#if 0
// 存储在注册表中的数据
typedef struct
{
	uint8 		b_write;							// 判断安防信息是否初始化过	
	uint8   	defend_state;						// 安防状态,在家，外出，夜间

	// 属性显示页面所需数据，为一个字节表示八防区(报警时间,延时报警时间,类型除外)
	uint8 		enable;		   						// 启用 或关闭   
	uint8       is_24_hour;							// 是否24小时防区
	uint8		finder_state;						// 探头状态
	uint8       can_hear;                           // 可听
	uint8       can_see;  							// 可见
	uint8   	area_type[AREA_AMOUNT];				// 防区对应的图标类型
	uint8       delay_time[AREA_AMOUNT];            // 延时报警时间
	uint8   	alarm_time;							// 报警时间

	// 用户设置页面数据
	uint8		exit_alarm_time;				    // 退出预警时间
	uint8 		isolation;							// 暂时隔离	1 = 暂时隔离	
	uint8       part_valid;							// 是否是局防有效 1 = 有效，0 = 无效
	uint8 		remote_set;							// 远程安防设置 bit0:远程安防设置,bit1:远程撤防,bit2:远程布防,bit3:远程局防,bit4:报警是否发送短信,bit5:住户梯口刷卡是否撤防
	uint8       link_param;							// 联动撤防的设置
	uint8  		short_msg[2][TELNUM_LEN];  			// 短信号码

	// 报警显示页面，只用以下一个变量表示
	uint8 		show_state[AREA_AMOUNT];			// 单个防区显示状态
}AF_FLASH_DATA, * PAF_FLASH_DATA;
#endif

// 控制执行者（报警远程控制、家电远程控制）
typedef enum
{
	EXECUTOR_LOCAL_HOST			= 0x01,				// 本机控制
	EXECUTOR_REMOTE_DEVICE		= 0x02,				// 遥控器

	EXECUTOR_LOCAL_FJ			= 0x05,				// 本地网络分机(包含无线终端、家庭PC)
	EXECUTOR_LOCAL_MNFJ			= 0x06,				// 本地模拟分机

	EXECUTOR_MANAGER			= 0x10,				// 管理员机
	EXECUTOR_STAIR				= 0x11,				// 梯口机
	EXECUTOR_AREA				= 0x12, 			// 区口机
	EXECUTOR_IE_USER			= 0x13,				// 远程IE平台
	EXECUTOR_SHORT_MSG			= 0x20,				// 住户短信
	EXECUTOR_PHONE_USER			= 0x21,				// 住户电话
	EXECUTOR_SERVER				= 0x22				// 中心服务器
}EXECUTOR_E;

// 安防状态
typedef enum
{
   DIS_DEFEND = 0x00,								// 撤防
   SET_DEFEND,								    	// 布防
   PART_DEFEND,										// 局防
   RESERVE                            				// 保留
}DEFEND_STATE;

// 按优先级排列，不能更改顺序,线路故障不针对对单个防区
typedef enum
{
   UNENABLE_SHOW_STATE = 0x00,						// 未启用
   ISO_SHOW_STATE,								   	// 隔离
   ALARM_SHOW_STATE,								// 报警
   WORK_SHOW_STATE,                            		// 单个防区布防状态
   NORM_SHOW_STATE									// 单个防区撤防状态
}SHOW_STATE;

typedef struct 
{
	uint8 			type;							// 布防,撤防,局防,旁路,取消旁路
	EXECUTOR_E 		executor;						// 操作者
	ZONE_DATE_TIME 	time;							// 操作时间
}AF_CZ_REC, * PAF_CZ_REC;							// 安防操作记录

typedef struct
{
	uint8 		nCount;
	PAF_CZ_REC	pAfCzRec;
}AF_CZ_LIST, * PAF_CZ_LIST;							// 安防操作记录列表

typedef struct
{
	uint8			areaNum;		 				// 防区号
	uint8     		bReaded;		 				// 是否已读
	uint8     		type;			 				// 报警的类型 故障、触发
	ZONE_DATE_TIME	time;	 						// 报警触发时间
}AF_BJ_REC, * PAF_BJ_REC;							// 报警触发记录

typedef struct
{
	uint32 		nCount;
	PAF_BJ_REC	pAfBjRec;
}AF_BJ_LIST, * PAF_BJ_LIST;							// 报警触发记录列表

typedef struct tagTOUCH_INFO{
	uint8 	 time[20];   							//报警触发时间 yyyy-mm-dd hh:mm:ss 
	uint8     TouchNum;								//防区号
	uint8     bReaded;								//是否已读
	uint8     bClean;								//是否被清除了
	uint32     id;									//本条报警记录的ID
	uint8     type;									//报警的类型
}ALARM_TOUCH_INFO,*PALARM_TOUCH_INFO;

typedef struct
{
	uint32 		nCount;
	PALARM_TOUCH_INFO	pAlarmRec;
}ALARM_TOUCH_INFO_LIST, *PALARM_TOUCH_INFO_LIST;							// 报警触发记录列表

/*************************************************
  Function:		storage_get_afcz_record
  Description: 	获取安防操作记录列表
  Input:		无
  Output:		无
  Return:		无
  Others:		
  	1.该指针不要释放
*************************************************/
PAF_CZ_LIST storage_get_afcz_record (void);

/*************************************************
  Function:		storage_get_afbj_record
  Description: 	获取安防报警记录列表
  Input:		无
  Output:		无
  Return:		无
  Others:		
  	1.该指针不要释放
*************************************************/
PAF_BJ_LIST storage_get_afbj_record (void);

/*************************************************
  Function:		storage_add_afcz_record
  Description: 	增加安防操作记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_add_afcz_record(uint8 type, EXECUTOR_E executor);

/*************************************************
  Function:		storage_add_afbj_record
  Description: 	增加安防报警记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_add_afbj_record(uint8 areaNum, uint8 type);

/*************************************************
  Function:		storage_clear_afcz_record
  Description:  清空安防操作记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_afcz_record(void);

/*************************************************
  Function:		storage_clear_afbj_record
  Description:  清空安防报警记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_afbj_record(void);

/*************************************************
  Function:		storage_af_timer_save
  Description: 	定时存储安防模块数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_af_timer_save(void);

/*************************************************
  Function:		storage_af_init
  Description: 	安防模块存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_af_init(void);

/*************************************************
  Function:    		storage_get_yj_wc_path
  Description: 		获取"外出请关好门,再见"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yj_wc_path(void);

/*************************************************
  Function:    		storage_get_yjq_path
  Description: 		获取"请注意有警情"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yjq_path(void);

/*************************************************
  Function:    		storage_get_fqcf_path
  Description: 		获取"防区触发不能布防"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_fqcf_path(void);

/*************************************************
  Function:    		storage_get_jjyjc_path
  Description: 		获取"你好！警戒已解除"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_jjyjc_path(void);

/*************************************************
  Function:    		storage_get_wanan_path
  Description: 		获取"晚安"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_wanan_path(void);

#if 0
/*************************************************
  Function:    		storage_get_yj_path
  Description: 		获取预警的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yj_path(void);
#endif

/*************************************************
  Function:    		storage_get_bj_path
  Description: 		获取报警的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_bj_path(void);

/*************************************************
  Function:    		storage_get_area_enable
  Description: 		获得防区启用参数
  Input:			无
  Output:    		无
  Return:			防区启用参数
  Others:    		       
*************************************************/
uint8 storage_get_area_enable(void);

/*************************************************
  Function:    		storage_set_area_enable
  Description: 		设置防区启用参数
  Input:
  	1.time 			报警时间
  Output:    		
  Return:			false:失败 true:成功			
  Others:    		       
*************************************************/
uint32 storage_set_area_enable(uint8 time);
/*************************************************
  Function:    		storage_get_gsm_num
  Description: 		获得短信号码
  Input:
  Output:
   	1.num1 			电话号码1
   	2.num2			电话号码2
  Return:			
  Others:           
*************************************************/
void stroage_get_gsm_num(uint8 *num1, uint8 *num2);

/*************************************************
  Function:    		storage_set_gsm_num
  Description: 		设置短信号码
  Input:
  	1.num1         	1:短信号码1 
  	2.num2			2:短信号码2
  Output:    		
  Return:			TRUE - 成功, FALSE - 失败
  Others:           
*************************************************/
uint32 storage_set_gsm_num(uint8 *num1, uint8 *num2);

/*************************************************
  Function:    		storage_get_remote_set
  Description: 		获取远程设置参数
  Input:			无
  Output:    		无
  Return:			远程设置参数	
  Others:           bit0 = 1: 远程安防设置有效
  					bit1 = 1: 允许远程撤防
  					bit2 = 1: 允许远程布防
  					bit3 = 1: 允许远程局防
*************************************************/
uint8 storage_get_remote_set(void);

/*************************************************
  Function:    		storage_set_remote_set
  Description: 		设置远程设置参数
  Input:
  	1.remote		远程设置参数
  Output:    		
  Return:			false:失败 true:成功	
  Others:           
  					bit0 = 1: 远程安防设置有效
  					bit1 = 1: 允许远程撤防
  					bit2 = 1: 允许远程布防
  					bit3 = 1: 允许远程局防
*************************************************/
uint8 storage_set_remote_set(uint8 remote);

/*************************************************
  Function:    		storage_get_link_param
  Description: 		获取联动参数
  Input:
  Output:    		
  Return:			联动参数	
  Others:           bit0 = 1 梯口刷卡撤防
  					bit1 = 1 门口刷卡撤防
  					bit3 = 1 启用布防联动情景模式
  					bit5 = 1 启用撤防联动情景模式
*************************************************/
uint8 storage_get_link_param(void);

/*************************************************
  Function:    		storage_set_link_param
  Description: 		设置联动参数
  Input:
  	1.link			联动参数
  Output:    		
  Return:			false:失败 true:成功	
  Others:           bit0 = 1 梯口刷卡撤防
  					bit1 = 1 门口刷卡撤防
  					bit3 = 1 启用布防联动情景模式
  					bit5 = 1 启用撤防联动情景模式
*************************************************/
uint8 storage_set_link_param(uint8 link);

/*************************************************
  Function:    		storage_get_link_scene
  Description: 		获取安防联动情景模式
  Input:
  Output:  
   	1.type			联动情景模式的索引
  Return:				
  Others:           
*************************************************/
void storage_get_link_scene(uint8 * type);

/*************************************************
  Function:    		storage_set_link_scene
  Description: 		设置安防联动情景模式
  Input:
  Output:  
   	1.type			联动情景模式的索引
  Return:				
  Others:           
*************************************************/
uint8 storage_set_link_scene(uint8 * type);

/*************************************************
  Function:    		storage_get_area_type
  Description: 		获取防区图标类型
  Input:
  Output:  
   	1.area_type		防区图标类型,每个防区一个字节
  Return:				
  Others:           
*************************************************/
void storage_get_area_type(uint8 * area_type);

/*************************************************
  Function:    		storage_get_defend_state
  Description: 		获取整机安防状态
  Input:
  Output:    		
  Return:			安防状态
  Others:           
*************************************************/
DEFEND_STATE storage_get_defend_state(void);

/*************************************************
  Function:    		storage_get_time_param
  Description: 		获取声音的参数
  Input:
  Output: 
  	1.para			参数 1;报警时间 2:退出预警时间 3:是否启用预警声
  Return:			是否启用退出预警声
  Others:           
*************************************************/
void storage_get_time_param(uint8 param[3]);

/*************************************************
  Function:    		storage_set_time_param
  Description: 		设置声音的参数
  Input:
  	1.para			参数 1;报警时间 2:退出预警时间 3:是否启用预警声
  Output: 
  Return:			是否启用退出预警声
  Others:           
*************************************************/
uint8 storage_set_time_param(uint8 param[3]);

/*************************************************
  Function:    		storage_get_validate_code
  Description: 		验证码
  Input:
  Output: 
  Return:			验证码
  Others:           
*************************************************/
uint32 storage_get_validate_code(void);

/*************************************************
  Function:    		storage_set_validate_code
  Description: 		设置验证码
  Input:
  	1.validate_code	验证码
  Output: 
  Return:			
  Others:           
*************************************************/
uint8 storage_set_validate_code(uint32 validate_code);

/*************************************************
  Function:    		storage_get_alarm_state
  Description: 		获取安防状态
  Input:			无
  Output:    		无
  Return:			0: 撤防 1:布防 2:报警
  Others:           
*************************************************/
uint8 storage_get_alarm_state(void);
#endif

