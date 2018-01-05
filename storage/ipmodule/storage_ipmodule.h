/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	storage_ipmodule.h
  Author:		
  Version:		
  Date:	  		2012-8-27
  Description: 	与IP模块之间的存储接口头文件	 
  
  History:				  
*************************************************/
#ifndef __STORAGE_IPMODULE_H__
#define	__STORAGE_IPMODULE_H__

#include "storage_include.h"
//#include "storage_types.h"

#define IPAD_EXTENSION_MAX			6 // ipad分机最大数量

typedef struct
{
	uint32 devno;				// 分机号
	uint32 state;				// 分机状态: 0-离线 1-在线
	uint32 ipAddr;				// 分机IP地址
}IPAD_DATA, *PIPAD_DATA;		// ipad分机信息


typedef struct
{
	uint32 count;							// 分机数量
	IPAD_DATA ipadData[IPAD_EXTENSION_MAX];	// 分机信息
}IPAD_EXTENSION_LIST, *PIPAD_EXTENSION_LIST;// ipad分机信息列表



/*************************************************
  Function:    		get_ipmodule_bindcode
  Description:		获得与IP模块捆绑的验证码
  Input: 			无
  Output:			无
  Return:			捆绑的验证码
  Others:
*************************************************/
uint32 get_ipmodule_bindcode(void);

/*************************************************
  Function:    		get_ipmodule_bindcode
  Description:		获得与IP模块捆绑的IP
  Input: 			无
  Output:			无
  Return:			捆绑的IP
  Others:
*************************************************/
uint32 get_ipmodule_addr(void);

/*************************************************
  Function:    		get_ipmodule_bindstate
  Description:		获得与IP模块捆绑的状态
  Input: 			无
  Output:			无
  Return:			捆绑的状态
  Others:
*************************************************/
uint32 get_ipmodule_bindstate(void);

/*************************************************
  Function:    		set_ipmodule_bindstate
  Description:		设置与IP模块捆绑的状态
  Input: 			
  	1.state			捆绑的状态
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_bindstate(uint32 state);

/*************************************************
  Function:    		set_ipmodule_info
  Description:		设置与IP模块
  Input: 			
  	1.code			捆绑的验证码
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_info(PIPMODULE_INFO IpModule);

/*************************************************
  Function:    		set_ipad_extension
  Description:		设置ipad分机信息
  Input: 			
  	1.ipadList		ipad分机信息列表
  Output:			无
  Return:			无
  Others:
*************************************************/
int32 set_ipad_extension(IPAD_EXTENSION_LIST ipadList);

/*************************************************
  Function:    		get_ipad_extension_addr
  Description:		获取ipad分机信息IP
  Input: 			无
  Output:			
  	1.devno			
  Return:			IP
  Others:
*************************************************/
uint32 get_ipad_extension_addr(uint32 devno);

#endif /*__STORAGE_IPMODULE_H__*/

