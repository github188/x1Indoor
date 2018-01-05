/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	storage_ipmodule.c
  Author:   	
  Version:  	
  Date: 		2012-8-24
  Description:  与IP模块之间的存储接口文件
				
  History:        
*************************************************/
#include "storage_ipmodule.h"

#ifdef _IP_MODULE_
static IPAD_EXTENSION_LIST g_ipadList;

/*************************************************
  Function:    		set_ipmodule_bindstate
  Description:		设置与IP模块捆绑的状态
  Input: 			
  	1.state			捆绑的状态
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_bindstate(uint32 state)
{
	gpSysParam->IpModule.State= state;
	log_printf("gpSysParam->IpModule.State = %d\n",gpSysParam->IpModule.State);
	SaveRegInfo();
}

/*************************************************
  Function:    		get_ipmodule_bindcode
  Description:		获得与IP模块捆绑的状态
  Input: 			无
  Output:			无
  Return:			捆绑的状态
  Others:
*************************************************/
uint32 get_ipmodule_bindstate(void)
{
	return gpSysParam->IpModule.State;
}

/*************************************************
  Function:    		set_ipmodule_bindcode
  Description:		设置与IP模块捆绑的验证码
  Input: 			
  	1.code			捆绑的验证码
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_bindcode(uint32 code)
{
	gpSysParam->IpModule.BindCode = code;
	SaveRegInfo();
}

/*************************************************
  Function:    		get_ipmodule_bindcode
  Description:		获得与IP模块捆绑的验证码
  Input: 			无
  Output:			无
  Return:			捆绑的验证码
  Others:
*************************************************/
uint32 get_ipmodule_bindcode(void)
{
	return gpSysParam->IpModule.BindCode;
}

/*************************************************
  Function:    		set_ipmodule_addr
  Description:		设置与IP模块捆绑的IP
  Input: 			
  	1.addr			IP地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_addr(uint32 addr)
{
	gpSysParam->IpModule.IpAddr = addr;
	SaveRegInfo();
}

/*************************************************
  Function:    		get_ipmodule_bindcode
  Description:		获得与IP模块捆绑的IP
  Input: 			无
  Output:			无
  Return:			捆绑的IP
  Others:
*************************************************/
uint32 get_ipmodule_addr(void)
{
	return gpSysParam->IpModule.IpAddr;
}

/*************************************************
  Function:    		set_ipmodule_info
  Description:		设置与IP模块
  Input: 			
  	1.code			捆绑的验证码
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_ipmodule_info(PIPMODULE_INFO IpModule)
{
	memcpy(&gpSysParam->IpModule, IpModule, sizeof(IPMODULE_INFO));
	log_printf("set_ipmodule_info = %d\n",gpSysParam->IpModule.State);
	SaveRegInfo();
}

/*************************************************
  Function:    		set_ipad_extension
  Description:		设置ipad分机信息
  Input: 			
  	1.ipadList		ipad分机信息列表
  Output:			无
  Return:			无
  Others:
*************************************************/
int32 set_ipad_extension(IPAD_EXTENSION_LIST ipadList)
{
	memcpy(&g_ipadList, &ipadList, sizeof(IPAD_EXTENSION_LIST));
	return 1;
}

/*************************************************
  Function:    		get_ipad_extension
  Description:		获取ipad分机信息
  Input: 			无
  Output:			
  	1.ipadList		ipad分机信息列表
  Return:			true/false
  Others:
*************************************************/
uint32 get_ipad_extension(IPAD_EXTENSION_LIST *pIpadList)
{
	if ( NULL == pIpadList )
	{
		return FALSE;
	}

	memcpy(pIpadList, &g_ipadList, sizeof(IPAD_EXTENSION_LIST));
	return TRUE;
}

/*************************************************
  Function:    		get_ipad_extension_addr
  Description:		获取ipad分机信息IP
  Input: 			无
  Output:			
  	1.devno			
  Return:			IP
  Others:
*************************************************/
uint32 get_ipad_extension_addr(uint32 devno)
{
	uint8 i;
	uint32 address = 0;
	
	for (i = 0 ; i < g_ipadList.count; i++)
	{
		if (g_ipadList.ipadData[i].devno == devno)
		{
			if (g_ipadList.ipadData[i].state)
			{
				address = g_ipadList.ipadData[i].ipAddr;
			}
			else
			{
				address = get_ipmodule_addr();
			}
			break;
		}
	}
	
	return address;
}
#endif

