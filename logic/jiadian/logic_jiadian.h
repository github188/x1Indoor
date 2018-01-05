/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  	logic_jiadian.h
  Author:     	txl
  Version:    	1.0
  Date: 
  Description:  家电逻辑头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _LOGIC_JIADIAN_H_
#define _LOGIC_JIADIAN_H_

#if 0
#include "typedef.h"
#include "include.h"
#include "storage_jd.h"
#include "net_comm_types.h"
#endif
#include "storage_include.h"
#include "logic_include.h"

// 家电状态
#define JD_STATE_OPEN			1
#define JD_STATE_CLOSE			0

#define JD_STATE_JUNE			3	

#define JD_WD_STATE_STOP        2

// 灯光调节最大最小值
#define MAX_LIGHT_VAULE			10
#define MIN_LIGHT_VAULE			1

// 空调调节最大最小值
#ifdef _AURINE_ELEC_RILI_
#define MAX_KONGTIAO_VAULE		12
#elif defined _AIR_VOLUME_MODE_
#define MAX_KONGTIAO_VAULE		15
#else
#define MAX_KONGTIAO_VAULE		10
#endif
#define MIN_KONGTIAO_VAULE		1

#if 0
// 家电厂商
typedef enum
{
	JD_FACTORY_X10 = 0x00,							// 瑞然家电模块
	JD_FACTORY_ACBUS,								// 冠林家电模块
	JD_FACTORY_MAX
}JD_FACTORY_TYPE;

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
#endif

// 数据发送状态
#define SEND_STATE_READY		0					// 数据发送准备
#define SEND_STATE_WAIT			1					// 数据发送等待应答
#define SEND_STATE_OK			2					// 数据发送应答完成

// 数据最大长度
#ifdef _AURINE_ELEC_NEW_
#define	MAX_AURINE_JD_DG_NUM 	 MAX_JD_LIGHT  	    // 最大灯光数量
#define	MAX_AURINE_JD_KT_NUM 	 MAX_JD_KONGTIAO	// 最大空调数量
#define	MAX_AURINE_JD_CL_NUM 	 MAX_JD_WINDOW		// 最大窗帘数量
#define	MAX_AURINE_JD_CZ_NUM 	 MAX_JD_POWER		// 最大插座数量
#define	MAX_AURINE_JD_MQ_NUM 	 MAX_JD_GAS		    // 最大煤气数量
#define MAX_AURINE_JD_SCENE_NUM  MAX_JD_SCENE_NUM	// 最大情景模式数量
#define PORT_TYPE_LIGHT			 0x01	            // 灯光
#define PORT_TYPE_CURTAIN		 0x02	            // 窗帘
#define PORT_TYPE_IR			 0x03	            // 红外
#define PORT_TYPE_POWER			 0x31               // 电源
#define PORT_TYPE_GAS			 0x32	            // 煤气
#define MAX_DATA_LEN			 50

#define	JD_AURINE_SOURE_ADDR		 (0xFF)
#define	JD_AURINE_TUNE_SPEED		 (0)
#define JD_AURINE_TUNE_SPEED_FAST	 (1)
#define JD_AURINE_LIGHT_TUNE_SPEED   (10)
#define	JD_AURINE_LIGHT_LEVEL	     (32)           // 灯光亮度级别

#define	JD_AURINE_CONTROL		    0x0C	// 控制冠林家电模块
#define	JD_LOOP_CONTROL			    0x0D	// 控制冠林家电轮询功能
#else
#define MAX_DATA_LEN			16
#endif

// 串口发送间隔		
// modi by luofl 2011-07-19 下发命令时间改为500毫秒
#define AURINE_COMMTIME_NUM	    4				        // 控制命令下发间隔时间	原来为200*5
#define COMMTIME_NUM			4		                // 控制命令下发间隔时间	原来为200*5

#define MAX_SEND_NUM			3                           

// 数据发送等待超时时间			 
#define MAX_SEND_TIME			((500/(5*AURINE_COMMTIME_NUM))+1)    //(发送超时时间/TICK)

// 轮询命令发送间隔
#define LOOP_SEND_TIME          (MAX_SEND_TIME-6)   //(轮询发送间隔时间/TICK)

// modi by luofl 2013-04-01 修改瑞郞家电下发过快，造成情景无法控制。
#define MAX_OLD_SEND_TIME       25//(((500*2)/(5*AURINE_COMMTIME_NUM))+1)   // 保留原来家电

typedef struct JD_SEND_PACKET_NODE
{
	struct JD_SEND_PACKET_NODE * next;
	struct JD_SEND_PACKET_NODE * prev;
	uint8 SendState;								// 数据发送状态
	uint8 SendTimes;								// 数据发送等待时间
	uint8 SendMaxTimes;                             // 最大发送时间
	uint8 size;										// 数据包大小
	uint8 data[MAX_DATA_LEN];						// 接收数据
}JD_SEND_PACKET, *PJD_SEND_PACKET;

#if 0
typedef enum		
{
	JD_TYPE_QINGJING = 0x00,
	JD_TYPE_DENGGUANG,
	JD_TYPE_CHUANGLIAN,
	JD_TYPE_KONGTIAO,
	JD_TYPE_DIANYUAN,
	JD_TYPE_MEIQI,
}JD_YUYUE_TYPE_E;	
#endif

typedef enum		
{
    JD_CMD_LOOP = 0x00,			// 轮询命令
	JD_CMD_COTROL    			// 控制命令
}JD_CMD_TYPE;

typedef	void (*JD_REPORT_CALLBACK)(uint8 * buf);

#ifdef _JD_MODE_
/*************************************************
  Function:		device_on
  Description: 	设备开
  Input: 
	1.addr		设备地址
	2.val		值
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_on(uint16 addr, uint8 val);

/*************************************************
  Function:		device_off
  Description: 	设备关
  Input: 
	1.addr		设备地址
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_off(uint16 addr);

/*************************************************
  Function:		device_on_all
  Description: 	设备全开
  Input: 
	1.addr		设备类型
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_on_all(AU_JD_DEV_TYPE devtype);

/*************************************************
  Function:		device_off_all
  Description: 	设备全关
  Input: 
	1.addr		设备地址
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_off_all(AU_JD_DEV_TYPE devtype);

/*************************************************
  Function:		device_value_add
  Description: 	设备值增加
  Input: 
	1.addr		设备地址
	2.val		值
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_value_add(uint16 addr, char *val);

/*************************************************
  Function:		device_value_dec
  Description: 	设备值减少
  Input: 
	1.addr		设备地址
	2.val		值
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void device_value_dec(uint16 addr, char *val);

/*************************************************
  Function:		exec_jd_scene_mode
  Description: 	执行家电情景模式
  Input: 
	1.mode		情景模式
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void exec_jd_scene_mode(AU_JD_SCENE_MODE mode);

/*************************************************
  Function:		set_device_logo
  Description: 	设置当前使用设备商标
  Input: 
	1.val		值
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void set_device_logo(uint8 val);

/*************************************************
  Function:		jd_aurine_scene_open
  Description:	控制冠林家电情景模式开
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_scene_open(uint8 scene_id,uint8 group_id, int index);
#endif

//#ifdef _AURINE_REG_
/*************************************************
  Function:			jd_distribute
  Description:		家电接收发送包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否,true/fasle
  Others:
*************************************************/
int32 jd_net_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			jd_responsion
  Description:		家电模块接收应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void jd_net_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);
//#endif

#ifdef _AURINE_ELEC_NEW_

typedef struct
{
	uint8 DevType;      //设备类型
	uint8 DevID;	    //设备索引
	uint8 Active;		//动作状态
	uint8 ActiveParam;	//动作参数
 }AU_JD_STATE,*PAU_JD_STATE;
 
 typedef struct
{
	uint8 			Index;							// 设备索引
	uint16			Address;		                // 设备地址(bit7-15:高;bit0-7:低)
	uint8			IsUsed;			                // 是否启用 1:已用，0:未用
	uint8		    Ischange;                       // 状态是否改变标志 0:状态未改变 1:状态改变	
	AU_JD_STATE     Dev_State;                      // 设备状态
	uint8 			IsTune;			                // 是否启用 亮度调整
}AU_JD_STATE_LIST,*PAU_JD_STATE_LIST;

typedef enum
{
	JD_AU_LIGHT_MODE_TUNE		= 1,				// 开关切换 调光
	JD_AU_LIGHT_MODE_STOP_TUNE	= 4,				// 停止调光
	JD_AU_LIGHT_MODE_ADD		= 5,				// 调亮1个级别
	JD_AU_LIGHT_MODE_DEC		= 7,				// 调暗1个级别
	JD_AU_LIGHT_MODE_OPEN		= 0,				// 开灯到某个亮度级别
	JD_AU_LIGHT_MODE_CLOSE		= 0x0A,			    // 带存储的关灯
	JD_AU_LIGHT_MODE_OPEN_OLD	= 0x09,			    // 开到原来亮度级别
	JD_AU_LIGHT_MODE_CHANGE     = 0x0E,			    // 带保存的开关切换
}JD_AU_LIGHT_CTRL_MODE;

typedef enum
{
	JD_AU_IR_MODE_SENDIR		=1,   			    // 调用
	JD_AU_IR_MODE_LEARNIR		=2,				    // 学习
	JD_AU_IR_MODE_LEARN_SCENE	=4,				    // 情景学习
}JD_AU_IR_CTRL_MODE;

typedef enum
{
	JD_AU_IR_REPORT_MODE_LEARN   =2,			    // 学习返回
	JD_AU_IR_REPORT_MODE_SCENE  =5,				    // 读取情景返回	
}JD_AU_IR_REPORT_MODE;

typedef enum
{
	JD_AU_WINDOW_MODE_OPEN   =1,			        // 开窗帘
	JD_AU_WINDOW_MODE_CLOSE  =2,			        // 关窗帘
	JD_AU_WINDOW_MODE_STOP   =3,			        // 停止动作
}JD_AU_WINDOW_CTRL_MODE;

typedef enum
{
	JD_AU_SCENE_MODE_OPEN   =1,				        // 情景开
	JD_AU_SCENE_MODE_CLOSE   =2,			        // 情景关
	JD_AU_SCENE_MODE_CHANGE  =3,			        // 情景切换
	JD_AU_SCENE_MODE_SAVE   =4,				        // 保存情景
}JD_AU_SCENE_CTRL_MODE;

typedef enum
{
	JD_AU_ADDR_ID				=2,					// ID寻址方式
	JD_AU_GROUP_ID				=3,					// 组寻址方式
	JD_AU_ADDR_BROADCAST		=9,					// 广播寻址方式
	JD_AU_PORT_BROADCAST		=0x0a,				// 端口广播
}JD_AU_ADDR_MODE;

typedef enum
{
	JD_AU_POWER_MODE_OPEN    = 1,				    // 电源开
	JD_AU_POWER_MODE_CLOSE	 = 2,				    // 电源关
}JD_AU_POWER_CTRL_MODE;

typedef enum
{
	JD_AU_GAS_MODE_OPEN    = 1,				        // 煤气开
	JD_AU_GAS_MODE_CLOSE   = 2,				        // 煤气关
}JD_AU_GAS_CTRL_MODE;

#define		JD_AU_CMD_DW_QUERY_STATE		(0x34)				//读取设备状态(灯光窗帘)
#define     JD_AU_CMD_DEV_BATCH_QUERY_STATE	(0x3a)				//批量查询设备状态
#define		JD_AU_CMD_UP_STATE_REPORT		(0x35)				//设备状态上报(灯光窗帘)
#define		JD_AU_CMD_DW_CTRL_SCENE			(0x40)				//控制情景
#define 	JD_AU_CMD_DW_CTRL_POWER			(0x41)				//控制电源
#define		JD_AU_CMD_DW_CTRL_LIGHT			(0x42)				//控制灯光
#define 	JD_AU_CMD_DW_CTRL_GAS			(0x43)				//控制煤气
#define		JD_AU_CMD_DW_CTRL_WINDOW		(0x44)				//控制窗帘
#define		JD_AU_CMD_DW_CTRL_IR			(0x46)				//控制红外设备
#define 	JD_AU_CMD_DW_CTRL_IR_REPORT		(0x47)				//红外设备应答
#define     JD_AU_CMD_DW_CTRL_ALL           (0x48)				//控制指导类型的所有设备



typedef enum
{
	JD_AURINE_UPDATE_IDLE			=0,
	//发送更新命令后，置其状态等待，
	//状态回应后，恢复正常或最少超时等待5s
	JD_AURINE_UPDATE_WAITING,		
	//当进行家电其他命令操作时，则置此状态标志
	// 最少延时2s后恢复正常
	JD_AURINE_UPDATE_PAUSE	,
	JD_AURINE_UPDATE_RUNNING,
	JD_AURINE_UPDATE_STOP,
	JD_AURINE_UPDATE_SINGLE_DEV,
	
}JD_AURINE_DEV_UPDATE_STATE;


/*************************************************
  Function:		jd_aurine_light_ctrlmode
  Description:	控制冠林家电灯光开
  Input: 		
  	2.addr		设备节点地址
  	3.param	亮度级别:0-100%；0xff: 开到原来的亮度级别
  			
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_light_open(uint8 Index, uint16 addr, uint8 param);

/*************************************************
  Function:		jd_aurine_light_ctrlmode
  Description:	控制冠林家电灯光开
  Input: 		
  	2.addr		设备节点地址
  	3.param	亮度级别:0-100%；0xff: 开到原来的亮度级别
  			
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_light_open_by_speed(uint8 Index, uint16 addr,uint8 param);

/*************************************************
  Function:		jd_auine_light_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_light_close(uint8 Index, uint16 addr);

/*************************************************
  Function:		jd_auine_window_open
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_window_open(uint8 Index, uint16 addr );

/*************************************************
  Function:		jd_auine_light_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_window_close(uint8 Index,uint16 addr );

/*************************************************
  Function:		jd_auine_light_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_window_stop(uint8 Index,uint16 addr );

/*************************************************
  Function:		jd_aurine_power_open
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_power_open(uint8 index, uint16 addr , uint8 param);

/*************************************************
  Function:		jd_aurine_power_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_power_close(uint8 index,uint16 addr );

/*************************************************
  Function:		jd_auine_light_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_gas_open(uint8 index,uint16 addr , uint8 param);

/*************************************************
  Function:		jd_aurine_gas_close
  Description:	控制冠林家电灯光关
  Input: 		
  	1.addr		设备节点地址		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_gas_close(uint8 index,uint16 addr );

/*************************************************
  Function:		jd_aurine_get_dev_state
  Description:	获取冠林家电灯光状态
  Input: 		
  	1.inedx		设备索引		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
uint8 jd_aurine_get_dev_state(AU_JD_DEV_TYPE devtype,uint8 index);

/*************************************************
  Function:		jd_aurine_set_light_state
  Description:	保存冠林家电灯光状态
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_set_dev_state(AU_JD_DEV_TYPE devtype,uint8 index,uint8 param);

/*************************************************
  Function:		jd_aurine_get_light_state
  Description:	获取冠林家电灯光状态
  Input: 		
  	1.inedx		设备索引		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
uint8 jd_aurine_get_dev_state_param(AU_JD_DEV_TYPE devtype,uint8 index);

/*************************************************
  Function:		jd_aurine_set_light_state
  Description:	保存冠林家电灯光状态
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_set_dev_state_param(AU_JD_DEV_TYPE devtype,uint8 index,uint8 param);

/*************************************************
  Function:		jd_aurine_get_dev_address
  Description:	保存冠林家电灯光状态
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
uint16 jd_aurine_get_dev_address(AU_JD_DEV_TYPE devtype,uint8 index);

/*************************************************
  Function:		jd_aurine_send_ir_to_dev
  Description:	获取家电状态信息
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_send_ir_to_dev(uint8 index, uint8 addr,uint16 param);

/*************************************************
  Function:		jd_aurine_learn_ir
  Description:	获取家电状态信息
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_learn_ir(uint8 index,uint8 addr,uint16 param);

/*************************************************
  Function:		jd_aurine_learn_ir_scene
  Description:	获取家电状态信息
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_learn_ir_scene(uint8 addr,uint16 param);

/*************************************************
  Function:		jd_aurine_yuyue_oper
  Description:	冠林家电预约处理
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_yuyue_oper(uint16 cmd,uint8 devtype, uint8 index, uint16 address,uint8 param);

/*************************************************
  Function:		jd_aurine_clean_address_list
  Description:	冠林家电预约处理
  Input: 		
  	1.inedx		设备索引
  	2.param		参数
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_clean_address_list(void);

/*************************************************
  Function:		jd_aurine_dev_update
  Description:	查询冠林家电设备状态
  Input: 		
  	2.addr		设备节点地址,=0xFF:则广播查询，否则点对点查询
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_dev_update(AU_JD_DEV_TYPE JDType,uint32 Check,uint16 index, uint16 addr);

/*************************************************
  Function:		jd_aurine_dev_Batchupdate
  Description:	批量查询冠林家电设备状态
  Input: 		
  	2.addr		设备节点地址,=0xFF:则广播查询，否则点对点查询
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_dev_Batchupdate(AU_JD_DEV_TYPE JDType,uint32 Check,uint8 num,uint16 *ID_list, uint8 *index);
/*************************************************
  Function:		jd_aurine_dev_all_open
  Description:	冠林家电设备全开
  Input: 		
  	1.inedx		设备索引		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_dev_all_open(uint8 devtype);

/*************************************************
  Function:		jd_aurine_dev_all_close
  Description:	冠林家电设备全关
  Input: 		
  	1.inedx		设备索引		
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_aurine_dev_all_close(uint8 devtype);

/*************************************************
  Function:		jd_aurine_init
  Description: 	
  Input: 		
  Return:		无  
*************************************************/
void jd_set_state_list(AU_JD_DEV_TYPE devtype, PJD_DEV_LIST_INFO statelist);

typedef void (*PAuJiaDianStateChanged)(uint8 devtype, uint8 devindex , uint8 param2);

/*************************************************
  Function:    	jd_state_report_init
  Description: 	家电数据处理回调家电界面	
  Input:
  	1.			回调
  Output: 		无   		
  Return:		无
  Others:           
*************************************************/
void jd_state_report_init(PAuJiaDianStateChanged proc);

/*************************************************
  Function:		jd_load_devinfo
  Description: 	加载设备信息
  Input: 		
  Return:		无  
*************************************************/
void jd_load_devinfo(void);

#endif

/*************************************************
  Function:		init_jiadian_callback
  Description:	初始化家电模块分发回调函数
  Input: 
	1.func		数据处理函数指针
	2.timeout	超时处理函数指针
  Output:		无
  Return:		无
  Others:
*************************************************/
int32 init_jiadian_callback(JD_REPORT_CALLBACK func);

/*************************************************
  Function:		jiadian_responsion
  Description: 	家电模块应答包处理
  Input: 
	1.mode		情景模式
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jiadian_responsion(uint8 *rcvbuf);

/*************************************************
  Function:		jd_stop_feet_dog
  Description:  家电模块中停止看门狗
  Input:
  	1.buf		数据
  	2.len		长度
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_stop_feet_dog(uint8 * buf, uint8 len);

/*************************************************
  Function:		jd_logic_init
  Description:  家电初始化
  Input: 
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void jd_logic_init(void);
#endif

