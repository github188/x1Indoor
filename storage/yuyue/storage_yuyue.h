/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	storage_yuyue.h
  Author:		luofl
  Version:		V2.0
  Date:			2011-4-9
  Description:	家电预约存储头文件

  History:                 
    1. Date:
       Author:
       Modification: 
    2. ...
*********************************************************/
#ifndef __STORAGE_YUYUE_H__
#define __STORAGE_YUYUE_H__

#define FILE_LEN				100 				// 定义文件的保存长度
#define EVENT_COUNT				20					// 事件预约总数
#define CLOCK_COUNT				5					// 闹钟总数
#define JD_COUNT				20					// 家电预约总数
#define SM_COUNT				20					// 情景预约总数
#define BJ_COUNT				20					// 报警预约总数

#define MAX_YUYUE_NUM			20

// 预约类型
typedef enum
{
	EV_BE_EVENT = 0,								// 事件预约
	EV_BE_AF,										// 安防预约
	EV_BE_JD,										// 家电预约
	EV_BE_SM										// 情景模式预约
}AU_BE_TYPE;

// 事件预约类型
typedef enum
{
	BE_BWL = 0,										// 备忘录
	BE_SR,											// 生日
	BE_NZ,											// 闹钟
	BE_HY,											// 会议
	BE_JR,											// 节日
	BE_SJ,											// 事件
	BE_JC											// 就餐
}BESPOKE_EVENT;

// 预约时间类型
typedef enum
{
	BE_DAY = 0,										// 每天
	BE_WEEK,										// 每周
	BE_MONTH,										// 每月
	BE_YEAR,										// 每年
	BE_MON_FRI,										// 周一~五
	BE_MON_SAT,										// 周一~六
	BE_ONE_DAY										// 某天
}BESPOKE_TYPE;

// 预约时间点结构
typedef struct
{
	BESPOKE_TYPE	BeType;
	int8			tm_sec;         				// seconds
  	int8   			tm_min;         				// minutes
  	int8   			tm_hour;        				// hours
  	int16			tm_mday;        				// day of the month
  	int16  			tm_mon;         				// month
  	int16  			tm_year;        				// year
  	int16  			tm_wday;        				// day of the week
  	int16  			tm_yday;        				// day in the year
}BE_DATETIME, * PBE_DATETIME;

// 预约通用结构体
typedef struct
{
	/*安防该值用于标识: 布防, 撤防, 局防
	情景模式该值用于标识: 5种模式*/
	uint8			Index;							// 事件, 家电, 安防, 情景模式的索引值
	BE_DATETIME		BeTime;							// 开始时间
	//BE_DATETIME		EndTime;						// 结束时间
	AU_BE_TYPE 		BeType;							// 预约类型
	uint8			Used;							// 是否启用	
	uint16			Action;							// 执行动作: 开/关						
	uint16 			Address;						// 设备地址
	uint8 			Type;							// 设备类型
	uint8 			TipUsed;						// 事件是否启用提示音
	//uint32			Id;								// 定时器ID
	uint8			IsRun;							// 是否已开始执行
	uint8			Degree;							// 调节的程度
	uint8           IsTune;                         // 设备是否可调
	char			Name[50];						// 设备名称
}BE_COMM, * PBE_COMM;

// 预约数据结构的链表信息
typedef struct
{
	int32		nCount;
	PBE_COMM 	be_comm;
}BE_COMM_LIST, * PBE_COMM_LIST;

#if 0
// 提示音的链表信息
typedef struct __TIPLIST
{
	INT32	nCount;
	int8  	filename[10][FILE_LEN];
}TIPLIST;

// 预约结构体链表
typedef struct __SAVE_BECOMM_LIST{
	BE_COMM		Be_Comm;
	int32 		time_id; 							// 用定时器ID来标识
	struct 		__SAVE_BECOMM_LIST *next;
}SAVE_BECOMM_LIST, * PSAVE_BECOMM_LIST;

// 事件的预约保存数据结构
typedef struct
{
	BE_DATETIME		BeEventTime;
	uint8 			RingID;							// 提示音ID  0:不启用提示音
	uint8 			Type;							// 事件类型
}BE_EVENT_SJ, * PBE_EVENT_SJ;

typedef void (* BE_SJ_CALLBACK)(WINDOW* win, PBE_EVENT_SJ pEvent);
#endif
/*************************************************
  Function:		storage_get_yuyue
  Description: 	获得预约事件
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PBE_COMM_LIST storage_get_yuyue (void);

/*************************************************
  Function:		storage_add_yuyue
  Description: 	添加一条预约事件
  Input:		
  	1.
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_yuyue (PBE_COMM yuyue);

/*************************************************
  Function:		storage_del_yuyue
  Description: 	删除预约
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_yuyue (uint8 Index);

/*************************************************
  Function:		storage_clear_yuyue
  Description: 	清空预约事件
  Input:		无		
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_yuyue (void);

/*************************************************
  Function:		storage_yuyue_init
  Description: 	初始化预约参数
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
void storage_yuyue_init(void);
#endif

