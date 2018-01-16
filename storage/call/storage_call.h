/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_call.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--对讲
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_CALL_H__
#define __STORAGE_CALL_H__

//#include "logic_include.h"
#include "storage_include.h"
//#include "storage_comm.h"
//#include "storage_types.h"

#define MAX_RECORD_NUM  		20					// 最大记录
#define MAX_DEVNO_NUM			30
typedef enum
{
    MISSED = 0x00,									// 未接来电	
	OUTGOING,										// 已拨电话
	INCOMING,										// 已接电话
	ALLCALL											// 全部通话
}CALL_TYPE;

// 通话记录的结构
typedef struct
{
	CALL_TYPE Calltype;								// 呼叫类型
	uint8 ReadFlag;									// 已读未读标志 1 - 未读 0 - 已读 
	uint8 LylyFlag;									// 留影留言状态
	char devno[MAX_DEVNO_NUM];						// 设备编号
	DEVICE_TYPE_E Type;								// 设备类型
	ZONE_DATE_TIME Time;		    				// 通话的时间	
}MCALLINFO, * PMCALLINFO;

// 通话记录列表的结构
typedef struct
{
	uint8 CallCount;								// 个数
	PMCALLINFO CallInfo;			    			// 通话记录
}MCALLLISTINFO, * PMCALLLISTINFO;		

/**************************  以下监视列表信息  ***********************************/
#define MAX_MONITOR_NUM  		20					// 最大记录

// 监视信息
typedef struct
{
	uint8 	DeviceType;                         	// 设备类型 
	uint8   index;									// 设备编号
	uint32  IP;										// 设备IP
	
}MONITORINFO, *PMONITORINFO;

// 通话记录列表的结构
typedef struct
{
	uint8 MonitorCount;								// 个数
	PMONITORINFO pMonitorInfo;			    		// 监视信息
}MONITORLISTINFO, *PMONITORLISTINFO;	

/**************************  以上监视列表信息  ***********************************/

/*************************************************
  Function:		storage_get_netdoor_ip
  Description: 	设置IP信息
  Input:			
 	1.Index		索引
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_netdoor_ip(uint8 Index);

/*************************************************
  Function:		storage_get_monitorlist
  Description: 	 获取监视列表信息
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMONITORINFO storage_get_monitorinfo (DEVICE_TYPE_E devtype, int8 index);

/*************************************************
  Function:		storage_get_callrecord
  Description: 	获得通话记录
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMCALLLISTINFO storage_get_callrecord (CALL_TYPE Calltype);

/*************************************************
  Function:		storage_add_callrecord
  Description: 	添加通话记录
  Input:		
  	1.Calltype	记录类型
  	2.Type		设备类型
  	3.devno		设备号码
  	3.Time		时间
  	4.PicState	图片状态
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_callrecord (CALL_TYPE Calltype, DEVICE_TYPE_E Type, char *devno, ZONE_DATE_TIME Time, uint8 LylyFlag);

/*************************************************
  Function:		storage_del_callrecord
  Description: 	删除通话记录
  Input:		
  	1.Calltype	记录类型
  	2.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_callrecord (CALL_TYPE Calltype, uint8 Index);

/*************************************************
  Function:		storage_clear_callrecord
  Description: 	清空通话记录
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_callrecord (CALL_TYPE Calltype);

/*************************************************
  Function:		storage_set_callrecord_lylyflag
  Description: 	设置该记录是否有关联的留影留言
  Input:		
  	1.time
  	2.LylyFlag
  Output:		无
  Return:		无
  Others:
*************************************************/
FLAG_STORAGE storage_set_callrecord_lylyflag(ZONE_DATE_TIME time, uint8 LylyFlag);

/*************************************************
  Function:		storage_clear_all_callrecord
  Description:  清空对讲模块所有通话记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_all_callrecord(void);

/*************************************************
  Function:		storage_get_callrecord_state
  Description:  获取是否有未接来电
  Input:		无
  Output:		无
  Return:		TRUE -- 有未接来电
  				FALSE -- 没有未接来电
  Others:		
*************************************************/
uint8 storage_get_callrecord_state(void);

/*************************************************
  Function:		storage_set_callrecord_state
  Description:  将未接来电设置为已读状态
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void storage_set_callrecord_state(void);

/*********************************************************/
/*						网络设置						 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_netparam
  Description: 	获取IP信息结构
  Input:		
  Output:		无
  Return:		网络参数
  Others:
*************************************************/
PNET_PARAM storage_get_netparam(void);

/*************************************************
  Function:		storage_get_netparam_bytype
  Description: 	获取IP信息
  Input:		
  	1.type		要获取的IP类型
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_netparam_bytype(IP_TYPE type);

/*************************************************
  Function:		storage_set_netparam
  Description: 	设置IP信息
  Input:			
  	1.save		是否要保存
  	1.type		要设置的IP类型
  	2.IP		IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_netparam(uint8 save, IP_TYPE type, uint32 IP);

#ifdef _ETHERNET_DHCP_
/*************************************************
  Function:		storage_set_ip_enable
  Description: 	设置静态IP
  Input:		无	
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void storage_set_ethernetdhcp_enable(uint8 save, uint8 enable);

/*************************************************
  Function:		storage_get_ip_enable
  Description:  获取静态IP
  Input:		无	
  Output:		无
  Return:		无
  Others:		无
*************************************************/
uint8 storage_get_ethernetdhcp_enable(void);

/*************************************************
  Function:    		get_local_ipaddress
  Description:		获得指定网络设备的IP地址
  Input: 
	1.ifname		设备名称, 如eth0 ifname=null 获得任意地址
  Output:			无
	1.ip			获得的IP地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int get_local_ipaddress(const char * ifname, uint32 *ip);

/*************************************************
  Function:    		get_local_subnetMask
  Description:		获得指定网络设备的子网掩码
  Input: 
	1.ifname		设备名称, 如eth0 ifname=null 获得任意地址
  Output:			无
	1.ip			获得的IP地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int get_local_subnetMask(const char * ifname, uint32 *ip);

/*************************************************
  Function:    		get_local_DefaultGateway
  Description:		获得指定网络设备的网关
  Input: 
	1.gateway		
  Output:			
  Return:			
  Others:
*************************************************/
int get_local_DefaultGateway(char *gateway);
#endif


/*********************************************************/
/*					设备号设备规则设置					 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_devparam
  Description: 	获取设备编号结构
  Input:		
  Output:		无
  Return:		设备编号结构
  Others:
*************************************************/
PFULL_DEVICE_NO storage_get_devparam(void);

/*************************************************
  Function:    		storage_get_devno
  Description:		获得设备编号
  Input: 			无
  Output:			无
  Return:			设备编号
  Others:
*************************************************/
DEVICE_NO storage_get_devno(void);

/*************************************************
  Function:    		get_devno_str
  Description:		获得设备编号字符串
  Input: 			无
  Output:			无
  Return:			设备编号字符串
  Others:
*************************************************/
char * storage_get_devno_str(void);

/*************************************************
  Function:    		storage_get_use_cell
  Description:		读取是否启用单元号
  Input: 			无
  Output:			无
  Return:			1:启用0:未启用
  Others:
*************************************************/
uint32 storage_get_use_cell (void);

/*************************************************
  Function:    		storage_set_areano
  Description:		设置区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 storage_set_areano (uint32 areano);

/*************************************************
  Function:    		storage_get_areano
  Description:		获取区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 storage_get_areano (void);

/*************************************************
  Function:    		set_devno_rule
  Description:		保存设备编号规则
  Input: 
  	1.save			是否保存
	2.rule			设备编号规则
  Output:			无
  Return:			0-成功 1-单元长度非法 2-梯号长度非法 3-房号长度非法
  					4-总长度不能大于18
  Others:
*************************************************/
uint32 storage_set_devno_rule(uint8 save, DEVICENO_RULE Rule);

/*************************************************
  Function:    		storage_set_devno
  Description:		保存设备号
  Input: 
  	1.DEVICE_TYPE_E	设备类型
	2.devno			设备号字符串
  Output:			无
  Return:			
  	0-成功 
  	1-长度非法 
  	2-房号段全为0   
  	3-区口机编号必须在1-40之间 
  Others:
*************************************************/
uint32 storage_set_devno(uint8 save, DEVICE_TYPE_E DeviceType, char* DevNo);

/*************************************************
  Function:    		storage_save_setdevno
  Description:		保存设备编号
  Input: 
  	1.save			是否保存
  	2.rule			设备编号规则
	3.devno			设备号字符串
  Output:			无
  Return:			无
  Others:			无
*************************************************/
uint32 storage_save_setdevno(uint8 save, DEVICENO_RULE Rule, DEVICE_NO* DevNo);

/*************************************************
  Function:    		storage_save_devno
  Description:		保存设备编号
  Input: 
  	1.save			是否保存
  	2.rule			设备编号规则
	3.devno			设备号字符串
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void storage_save_devno(uint8 save, DEVICENO_RULE Rule, char* DevNo);

/*************************************************
  Function:    		storage_get_dev_desc
  Description:		读取设备描述符
  Input: 			无
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
char * storage_get_dev_desc(void);

/*************************************************
  Function:    		storage_set_dev_desc
  Description:		读取设备描述符
  Input: 			无
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
uint8 storage_set_dev_desc(char * str);

/*************************************************
  Function:    		storage_init_get_dev_desc
  Description:		初始化时读取设备描述
  Input:
  	1.DevDesc		描述地址
  	2.language		语言类型
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
void storage_init_get_dev_desc(char * DevDesc, LANGUAGE_E language);

/*************************************************
  Function:		storage_call_init
  Description: 	对讲模块存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_call_init(void);

#if 0
/*************************************************
  Function:			stroage_sdp_param
  Description:		初始化视频编码参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_sdp_param_init(void);

/*************************************************
  Function:			storage_init_sysparam
  Description:		初始化系统参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_init_sysparam(void);
#endif

/*************************************************
  Function:		storage_get_monitorlist
  Description: 	 获取监视列表信息
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMONITORLISTINFO storage_get_monitorlist(DEVICE_TYPE_E devtype);

/*************************************************
  Function:		save_call_storage
  Description: 	按模块存储
  Input:	
  	1.mode
  	2.CallList	
  Output:		无
  Return:		无
  Others:
*************************************************/
void save_monitorlist_storage(DEVICE_TYPE_E Type, PMONITORLISTINFO monitorlist);

/*************************************************
  Function:		free_monitorlist_memory
  Description: 	释放对讲存储内存
  Input:		
  	1.CallList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_monitorlist_memory(PMONITORLISTINFO* monitorlist);

/*************************************************
  Function:		malloc_call_memory
  Description: 	申请对讲存储内存
  Input:
  	1.CallList
  	2.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void malloc_monitorlist_memory (PMONITORLISTINFO *MonitorList,uint32 MaxNum);

/*************************************************
  Function:		storage_get_devrule
  Description: 	获取设备规则结构
  Input:		
  Output:		无
  Return:		设备编号结构
  Others:
*************************************************/
DEVICENO_RULE* storage_get_devrule(void);

/*************************************************
  Function:		storage_clear_monitorlist
  Description: 	清空存储文件内容
  Input:		
  	1.Type		设备类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_clear_monitorlist(DEVICE_TYPE_E Type);

#endif 

