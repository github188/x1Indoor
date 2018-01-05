/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_jd.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		10.08.31
  Description:  存储模块--家电
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_JD_H__
#define __STORAGE_JD_H__

#include "storage_include.h"
//#include "storage_types.h"

#define _JD_GROUP_					0

#define MAX_JD_NAME					31				// 家电设备名称最大长度
#ifdef _AURINE_ELEC_NEW_
#define MAX_JD_LIGHT				64				// 灯线路
#else
#define MAX_JD_LIGHT				32				// 灯线路
#endif
#define MAX_JD_WINDOW				16				// 窗帘线路
#ifdef _IP_MODULE_//_AURINE_ELEC_RILI_
#define MAX_JD_KONGTIAO				12				// 空调线路
#else
#define MAX_JD_KONGTIAO				8				// 空调线路
#endif
#define MAX_JD_POWER				16				// 电源线路
#define MAX_JD_GAS					1				// 煤气线路
#define MAX_JD_DEVICE_NUM			(MAX_JD_LIGHT+MAX_JD_WINDOW+MAX_JD_KONGTIAO+MAX_JD_POWER+MAX_JD_GAS)
#define MAX_JD_SCENE_NUM            32

#define SCENE_CMD_CLOSE				0
#define SCENE_CMD_OPEN				1

/*******************************家电设备信息********************************/
// 家电厂商
typedef enum
{
	JD_FACTORY_X10 = 0x00,							// 瑞然家电模块
	JD_FACTORY_ACBUS,								// 冠林家电模块
	JD_FACTORY_MAX
}JD_FACTORY_TYPE;

typedef enum		
{
	JD_TYPE_QINGJING = 0x00,
	JD_TYPE_DENGGUANG,
	JD_TYPE_CHUANGLIAN,
	JD_TYPE_KONGTIAO,
	JD_TYPE_DIANYUAN,
	JD_TYPE_MEIQI,
}JD_YUYUE_TYPE_E;	

// 家电命令
#define JD						0x02				// 家电模块485代码
#define	JD_ON					0x01				// 开
#define	JD_OFF					0x02				// 关
#define	JD_ADD					0x03				// 增加
#define	JD_DEC					0x04				// 减少
#define	JD_INQ_STATUS			0x05				// 查寻节点设备状态
#define	JD_GET_LOGO_NUM			0x06				// 取得品牌总数
#define JD_DOWNLOAD_LOGO		0x08				// 家电模块上电请求选用的品牌号
#define	JD_GET_LOGO				0x0A				// 获取品牌名称(一次获取一条)
#define	JD_GET_VER				0x09				// 获取程序版本号
#define	JD_SET_LOGO				0x0B				// 设定选用的品牌

typedef enum
{
	JD_SCENE = 0x00,
	JD_LIGHT,
	JD_WINDOW,
	JD_KONGTIAO,
	JD_POWER,
	JD_GAS,
	JD_ALL
}JIADIAN_PAGE;

// 家电设备状态
typedef struct
{
	uint8 devlight[MAX_JD_LIGHT][2];				// 包含状态和级数
	uint8 devKongtiao[MAX_JD_KONGTIAO][2];			// 包含状态和级数
	uint8 devwindow[MAX_JD_WINDOW];
	uint8 devpower[MAX_JD_POWER];
	uint8 devgas[MAX_JD_GAS];
}JD_STATE_INFO, * PJD_STATE_INFO;

/* 家电设备类型 */
typedef enum
{
	JD_DEV_LIGHT 			= 0,					// 灯光设备
	JD_DEV_WINDOW 			= 1,					// 窗帘设备
	JD_DEV_KONGTIAO 		= 2,					// 空调设备
	JD_DEV_POWER 			= 3,					// 电源设备
	JD_DEV_GAS 				= 4,					// 煤气设备
	JD_DEV_MAX
}AU_JD_DEV_TYPE;

/* 家电情景类型 */
typedef enum
{
	JD_SCENE_RECEPTION 		= 0x00,					// 会客模式
	JD_SCENE_MEALS,									// 就餐模式
	JD_SCENE_NIGHT,									// 夜间模式
	JD_SCENE_POWERSAVING,							// 节电模式
	JD_SCENE_GENERAL,								// 普通模式
	JD_SCENE_MAX,
	JD_SCENE_NONE,
}AU_JD_SCENE_MODE;									// 家电情景模式

/* 家电设备信息结构 */
typedef struct
{
	AU_JD_DEV_TYPE 	JdType;							// 设备类型
	uint8			Index;							// 设备索引号 从0开始
	uint16			Address;						// 设备地址(bit7-15:高;bit0-7:低)
	uint32 			TextIDPos;						// 位置名称
	uint32 			TextIDName;						// 设备类型名称
	uint32  		TextID;							// 设备类型id
	char			Name[MAX_JD_NAME];				// 名称
	uint8			IsUsed;							// 是否启用 1:已用，0:未用
	uint8 			IsTune;							// 是否可调节，对于灯光为：是否可调光，对于空调是否可调温
	uint8			IsGroup;						// 是否组控
	uint8 			Commond[JD_SCENE_MAX];			// 情景命令	
	uint8 			param[JD_SCENE_MAX][7];			// 情景命令值	
//	uint8			IsSigOper;						// 是否单控
}JD_DEV_INFO, *PJD_DEV_INFO;

/* 家电设备列表信息 */
typedef struct
{
	int32 nCount;
	PJD_DEV_INFO pjd_dev_info;
}JD_DEV_LIST_INFO,*PJD_DEV_LIST_INFO;

/* 家电设备列表信息 */
typedef struct
{
	uint16 light_addr[MAX_JD_LIGHT];
	uint16 window_addr[MAX_JD_WINDOW];
	uint16 kongtiao_addr[MAX_JD_KONGTIAO];
	uint16 power_addr[MAX_JD_POWER];
	uint16 gas_addr[MAX_JD_GAS];
	uint16 sence_addr[MAX_JD_SCENE_NUM];
}JD_ADDR_INFO,*PJD_ADDR_INFO;

// 情景模式信息
typedef struct 
{
	AU_JD_SCENE_MODE SceneIndex;
	uint8 IsUsed;
	char SceneName[MAX_JD_NAME];
	#ifdef _AURINE_ELEC_NEW_
	uint32 	  TextIDPos;		// 位置名称
	uint32    TextIDName;		// 设备类型名称
	uint8     SceneID;
	uint16    Address;			// 设备地址(bit7-15:高;bit0-7:低)	
	#endif
}JD_SCENE_INFO, *PJD_SCENE_INFO;

// 情景模式信息链表
typedef struct
{
	int32 nCount;
	PJD_SCENE_INFO pjd_scene_info;
}JD_SCENE_INFO_LIST, *PJD_SCENE_INFO_LIST;

#ifdef _AURINE_ELEC_NEW_
typedef struct
{
	uint16	Address;			// 设备地址(bit7-15:高;bit0-7:低)	
	uint8	IsUsed;				// 是否启用 1:已用，0:未用
	char	Name[30];			// 设备名称
}AU_JD_SCENE_PARAM,*PAU_JD_SCENE_PARAM;
#endif

typedef struct
{
	unsigned char Index;
	unsigned char Addr;
	unsigned char State;
	unsigned char IsUsed;	
}AU_JDDEV_STATE, *PAU_JDDEV_STATE;

typedef struct
{
	uint32 Count;
	PAU_JDDEV_STATE JdDev;	
}AU_JDDEV_LIST, *PAU_JDDEV_LIST;


#ifdef _JD_MODE_
/*******************************家电设备信息********************************/

/*************************************************
  Function:		free_jd_memory
  Description: 	释放家电存储内存
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_jd_memory(PJD_DEV_LIST_INFO* JDList);

/*************************************************
  Function:		malloc_jd_memory
  Description: 	申请家电存储内存
  Input:
  	1.JDList
  	2.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void malloc_jd_memory(PJD_DEV_LIST_INFO *JDList,uint32 MaxNum);

/*************************************************
  Function:		storage_get_jddev
  Description: 	获得设备信息
  Input:		
  	1.dev
  	2.UsedOnly	只获取启用的设备
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_DEV_LIST_INFO storage_get_jddev (AU_JD_DEV_TYPE dev, uint8 UsedOnly);

/*************************************************
  Function:		storage_add_jddev
  Description: 	添加通话记录
  Input:		
  	1.Calltype	记录类型
  	2.Type		设备类型
  	3.devno		设备号码
  	3.Time		时间
  	4.LylyFlag	图片状态
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_jddev (PJD_DEV_INFO info);

/*************************************************
  Function:		storage_del_devinfo
  Description: 	删除
  Input:		
  	1.Calltype	记录类型
  	2.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_devinfo (AU_JD_DEV_TYPE dev, uint8 Index);

/*************************************************
  Function:		storage_clear_devinfo
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_devinfo(AU_JD_DEV_TYPE dev);

/*************************************************
  Function:		storage_clear_all_callrecord
  Description:  清空
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_all_devinfo(void);

/*************************************************
  Function:		get_dev_max
  Description: 	获得家电总个数
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 get_dev_max(AU_JD_DEV_TYPE dev);

/*************************************************
  Function:		free_scene_memory
  Description: 	释放情景存储内存
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_scene_memory(PJD_SCENE_INFO_LIST *pSceneList);

#ifdef _AURINE_ELEC_NEW_
/*************************************************
  Function:		storage_get_aurine_light_value
  Description:	获得灯亮度状态
  Input: 		
  	2.addr		设备节点地址,=0xFF:则广播查询，否则点对点查询
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void storage_get_aurine_light_value(uint8 *value_list);

/*************************************************
  Function:		jd_aurine_read_lightvalue
  Description:	从文件中读取灯光设备上一次亮度
  Input: 		
  	2.addr		设备节点地址,=0xFF:则广播查询，否则点对点查询
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_read_lightvalue(void);

/*************************************************
  Function:		storage_save_aurine_light_value
  Description:	保存灯光设备上一次亮度
  Input: 		
  	2.addr		设备节点地址,=0xFF:则广播查询，否则点对点查询
  Output:		无
  Return:		无
  Others:		无
*************************************************/
ECHO_STORAGE storage_save_aurine_light_value(uint8 *value_list);

/*************************************************
  Function:		storage_set_scene_info
  Description: 	情景信息设置
  Input:		
  	1.pSceneList	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_set_aurien_scene_info(PJD_SCENE_INFO pSceneList);

/*************************************************
  Function:		storage_get_scene_info
  Description: 	获得情景模式信息链表
  Input:		
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_SCENE_INFO_LIST storage_get_aurine_scene_info(uint8 UsedOnly);

/*************************************************
  Function:		storage_add_jddev
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_aurine_scenemode(PJD_SCENE_INFO info);

/*************************************************
  Function:		storage_get_aurine_kongtiao_value
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_aurine_kongtiao_value(uint8 *value_list);

/*************************************************
  Function:		storage_save_aurine_kongtiao_value
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_save_aurine_kongtiao_value(uint8 * value_list);

#ifdef _AIR_VOLUME_MODE_
/*************************************************
  Function:			storage_get_kongtiao_air_volume
  Description: 		获取空调风量
  Input:		
  	1.value_list	
  Output:			无
  Return:			ECHO_STORAGE
  Others:			无
*************************************************/
void storage_get_kongtiao_air_volume(uint8 *value_list);

/*************************************************
  Function:			storage_save_kongtiao_air_volume
  Description: 		保存风量
  Input:		
  	1.value_list	
  Output:			无
  Return:			ECHO_STORAGE
  Others:			无
*************************************************/
ECHO_STORAGE storage_save_kongtiao_air_volume(uint8 * value_list);
#endif
#endif
#endif

/*************************************************
  Function:		storage_jd_init
  Description:  初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_jd_init(void);
#endif

