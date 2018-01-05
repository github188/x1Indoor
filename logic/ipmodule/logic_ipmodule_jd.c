/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_jiadian_net.c
  Author:   	txl
  Version:   	2.0
  Date: 		09.5.21
  Description:  家电函数
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/

//#include "include.h"
//#include "logic_jiadian.h"
#include "logic_include.h"
#include "storage_include.h"

#define JD_WAIT_TIME		3

#ifdef _IP_MODULE_JD_

/*************************************************
  Function:    		jd_send_net_open
  Description:		网络发送家电控制命令
  Input: 
  	1.SendIP:		发送IP
  	2.oper: 		执行者
  	3.index:		索引
  	4.param			参数
  	5.len			参数长度
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
static ECHO_VALUE_E jd_send_net_com(NET_COMMAND Cmd, unsigned char Oper, unsigned char JDType, unsigned char * SendData,uint32 len, char * RecData, uint32 * ReciSize)
{
	ECHO_VALUE_E Echo = ECHO_ERROR;
	char bindcode[6] = {0};
	uint8 EchoValue = 0;
	uint32 ret = FALSE;
	char data[300] = {0};
	int i,nlen = 0;
	uint32 SendIP = get_ipmodule_address();
	
	if (0 == SendIP)
	{
		return ECHO_ERROR;
	}
	memset(data, 0,sizeof(data));
	memcpy(data, &Oper, 1);
	nlen = 1;

	
	sprintf(bindcode, "%06d", get_ipmodule_bindcode());	
	memcpy(data+nlen, bindcode, 6);
	nlen += 6;

	memcpy(data+nlen, &JDType, 1);
	nlen += 1;

	if (len > 0 && SendData)
	{
		memcpy(data+nlen, SendData, len);
		nlen += len;
	}
	
	set_nethead(G_CENTER_DEVNO, PRIRY_HIGHEST);
	net_direct_send(Cmd, data, nlen,  SendIP, NETCMD_UDP_PORT);
	Echo = ECHO_OK;
	return Echo;
}

/*************************************************
  Function:    		ipmodule_jd_net_open
  Description:		网络发送家电控制命令
  Input: 
  	1.SendIP:		发送IP
  	2.JDType: 		家电类型
  	3.index:		索引
  	4.param			参数
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_open(uint8 JDType, uint8 Index, uint8* param, uint32 len)
{	
	uint8 data[100]={0};
	uint8 g_DevType = JDType;
	
	data[0] = Index+1;
	if (len > 0 && param)
	{
		memcpy(&data[1], param, len);
	}

	switch (JDType)
	{
		case JD_DEV_WINDOW:
			g_DevType = 2;							// 由于家电设备定义相反,UI改动很大
			break;

		case JD_DEV_KONGTIAO:	
			g_DevType = 1;
			break;

		default:
			break;
	}

	log_printf("ipmodule_jd_net_open JDType %x Index  %d data %d\n",JDType, Index, data[1]);
	
	return jd_send_net_com(CMD_JD_DEV_OPEN, EXECUTOR_LOCAL_HOST, g_DevType, data, (len+1), NULL, NULL);
}

/*************************************************
  Function:    		ipmodule_jd_net_close
  Description:		网络发送家电控制命令
  Input: 
  	1.JDType: 		家电类型
  	2.index:		索引
  	2.param:		数据
  	2.len:			长度
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_close(uint8 JDType, uint8 Index, uint8* param, uint32 len)
{	
	uint8 data[100]={0};
	uint8 g_DevType = JDType;
	
	data[0] = Index+1;
	if (len > 0 && param)
	{
		memcpy(&data[1], param, len);
	}

	switch (JDType)
	{
		case JD_DEV_WINDOW:
			g_DevType = 2;							// 由于家电设备定义相反,UI改动很大
			break;

		case JD_DEV_KONGTIAO:	
			g_DevType = 1;
			break;

		default:
			break;
	}


	log_printf("ipmodule_jd_net_close JDType %x Index %d\n",g_DevType, Index);
	
	return jd_send_net_com(CMD_JD_DEV_CLOSE, EXECUTOR_LOCAL_HOST, g_DevType, data, (len+1), NULL, NULL);
}

/*************************************************
  Function:    		ipmodule_jd_net_scene
  Description:		网络发送家电控制命令
  Input: 
  	1.SendIP:		发送IP
  	2.JDType: 		情景类型
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_scene(uint8 Scene)
{	
	log_printf("ipmodule_jd_net_scene Scene %x\n",Scene);
	
	return jd_send_net_com(CMD_JD_SCENE_MODE, EXECUTOR_LOCAL_HOST, Scene, NULL, 0, NULL, NULL);
}

/*************************************************
  Function:    		ipmodule_jd_net_get_state
  Description:		获取状态
  Input: 
  	1.JDType: 		情景类型
  	2.check
  	3.Index,
  	4.num
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_get_state(uint8 JDType, uint32 check, uint8 *Index, uint8 Count)
{
	uint8 i;
	uint8 SendData[100]={0};
	uint32 SendSize = 0;
	uint8 num = Count;
	uint8 g_DevType = JDType;
	uint32 data1 = 0, data2 = 0;
	
	if (num == 0 || NULL == Index)
	{
		return ECHO_ERROR;
	}

	switch (JDType)
	{
		case JD_DEV_WINDOW:
			g_DevType = 2;							// 由于家电设备定义相反,UI改动很大
			break;

		case JD_DEV_KONGTIAO:	
			g_DevType = 1;
			break;

		default:
			break;
	}
	memset(SendData, 0, sizeof(SendData));
	memcpy(SendData, (uint8*)&(check),4);
	SendSize = 4;

	for (i = 0; i < num; i++)
	{
		if (Index[i] >= 32)
		{
			data2 += (0x00000001<<Index[i]);
		}
		else
		{
			data1 += (0x00000001<<Index[i]);
		}
	}
	
	for (i = 0; i < 4; i++)
	{
		SendData[4+i] = (uint8)(data1>>(i*8) & 0xff);
		SendData[8+i] = (uint8)(data2>>(i*8) & 0xff);
	}
	log_printf("data1 %x data2 %x num %x\n",data1,data2,num);
	
	SendSize += 8;

	log_printf("ipmodule_jd_net_get_state g_DevType %x %x %x %x %x %x %x %x %x\n",g_DevType,
		SendData[4],SendData[5],SendData[6],SendData[7],SendData[8],SendData[9],SendData[10],SendData[11]);
	
	return jd_send_net_com(CMD_JD_GET_JD_INFO, EXECUTOR_LOCAL_HOST, g_DevType, SendData, SendSize, NULL, 0);
}


/*************************************************
  Function:    		ipmodule_jd_net_get_state
  Description:		获取状态
  Input: 
  	1.Type: 		情景或家电类型
  	2.start
  	3.end
  	4.num
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_get_info(uint8 Type)
{
	uint8 EchoValue = ECHO_OK;
	int ReciSize = 0;
	char ReciData[2000];
	char bindcode[6] = {0};
	char data[20] = {0};
	uint32 ipModuleAddr = 0;
	int cmd = 0, ret = 0;
	int i = 0,j = 0, isexist;
	AU_JDDEV_INFO auDevInfo[MAX_AURINE_JD_DG_NUM] = {0};
	uint32 count = 0;
	int start,end, sendagain = 0;
	uint32 g_DevType = (uint32)Type;
	
	if (g_DevType > 5)
	{
		log_printf(">>>ipmodule_jd_net_get_info: devtype=%d error\n", g_DevType);
		return ECHO_ERROR;
	}

	start = 0;
	switch (Type)
	{
		case JD_DEV_LIGHT:
			end = 31;
			sendagain = 1;
			break;

		case JD_DEV_WINDOW:
			g_DevType = 2;							// 由于家电设备定义相反,UI改动很大
			end = MAX_JD_WINDOW-1;
			break;

		case JD_DEV_KONGTIAO:	
			g_DevType = 1;
			end = MAX_JD_KONGTIAO-1;
			break;
			
		case JD_DEV_POWER:
			end = MAX_JD_POWER-1;
			break;
			
		case JD_DEV_GAS:
			end = MAX_JD_GAS-1;
			break;

		case 5:
			end = MAX_JD_SCENE_NUM-1;
			break;
	}
	
	sprintf(bindcode, "%06d", get_ipmodule_bindcode());
	ipModuleAddr = get_ipmodule_address();
	memset(data, 0, sizeof(data));
	memcpy(data, bindcode, 6);
	memcpy(data+6, &g_DevType, 4);
	memcpy(data+10, &start, 4);
	memcpy(data+14, &end, 4);
	
	if (sendagain)
	{
		set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
		ret = net_send_command(CMD_JD_GET_DEVINFO, data, 18, ipModuleAddr, NETCMD_UDP_PORT, \
						5, &EchoValue, ReciData, &ReciSize);	
		
		if(EchoValue == ECHO_OK && ReciData != NULL && ReciSize > 0)
		{
			memcpy(&count, ReciData, 4);
			log_printf("ipmodule_jd_net_get_info 1 count %d\n",count);
			
			if (count > 0 && count <= 32)
			{
				memcpy(auDevInfo, ReciData+4, sizeof(AU_JDDEV_INFO)*count);
			}		
		}
		start = 32;
		end = 63;
		memcpy(data+10, &start, 4);
		memcpy(data+14, &end, 4);
	}
	
	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	ret = net_send_command(CMD_JD_GET_DEVINFO, data, 18, ipModuleAddr, NETCMD_UDP_PORT, \
							5, &EchoValue, ReciData, &ReciSize);
	if (ret == FALSE)
	{
		log_printf(">>>ipmodule_jd_net_get_info: send_command error.\n");
		return ECHO_ERROR;
	}

	if(EchoValue == ECHO_OK && ReciData != NULL && ReciSize > 0)
	{
		uint32 g_count = 0;
		//解析数据
		memcpy(&g_count, ReciData, 4);
		log_printf("ipmodule_jd_net_get_info 2 count %d\n",g_count);		
		
		log_printf(">>>ipmodule_jd_net_get_info: Type=%d, g_count=%d\n", Type, g_count);
		if (g_count > MAX_AURINE_JD_DG_NUM || g_count < 0)
		{
			log_printf(">>>ipmodule_jd_net_get_info: count > 64, error.\n");
			return ECHO_ERROR;
		}		
		
		memcpy(&auDevInfo[count], ReciData+4, sizeof(AU_JDDEV_INFO)*g_count);
		count += g_count;
			
		if (Type == 5)
		{
			//处理本地启用但IP模块上未启用的设备
			PJD_SCENE_INFO_LIST g_scenelist = storage_get_aurine_scene_info(0);
			uint8 g_save = 0;

			if (NULL == g_scenelist)
			{
				return ECHO_ERROR;
			}

			#if 0
			log_printf("*********************************************\n");
			log_printf("g_scenelist->nCount %d\n",g_scenelist->nCount);

			log_printf("*********************************************\n");
			for(j = 0; j < count; j++)
			{
				log_printf("auDevInfo[%d].Index %d\n",j,auDevInfo[j].Index);
				log_printf("auDevInfo[%d].IsUsed %d\n",j,auDevInfo[j].IsUsed);
				log_printf("auDevInfo[%d].IsTune %d\n",j,auDevInfo[j].IsTune);
				log_printf("auDevInfo[%d].Addr %d\n",j,auDevInfo[j].Addr);
				log_printf("auDevInfo[%d].Name %s\n",j,auDevInfo[j].Name);
			}		
			log_printf("*********************************************\n");	
			#endif
			
			for(i = 0; i < g_scenelist->nCount; i++)
			{
				if (g_scenelist->pjd_scene_info[i].IsUsed)
				{
					isexist = 0;
					for(j=0; j<count; j++)
					{
						if (i == auDevInfo[j].Index-1)
						{
							isexist = 1;
							break;
						}
					}
					
					if (isexist == 0)
					{
						g_scenelist->pjd_scene_info[i].IsUsed = 0;
					}
				}
			}
			
			for (i = 0; i < count; i++)
			{
				for(j = 0; j < g_scenelist->nCount; j++)
				{
					if (j == auDevInfo[i].Index-1)
					{
						g_scenelist->pjd_scene_info[j].SceneID = j;
						g_scenelist->pjd_scene_info[j].SceneIndex = j;
						g_scenelist->pjd_scene_info[j].Address = auDevInfo[i].Addr;
						g_scenelist->pjd_scene_info[j].IsUsed = auDevInfo[i].IsUsed;
						sprintf(g_scenelist->pjd_scene_info[j].SceneName, "%s" ,auDevInfo[i].Name);
						if (j >= 5)
						{
							g_scenelist->pjd_scene_info[j].TextIDPos = SID_Jd_Mode_Out;
							g_scenelist->pjd_scene_info[j].TextIDName = SID_Jd_Mode_Mode;
						}
					}
				}	
			}

			storage_set_aurien_scene_info(g_scenelist->pjd_scene_info);
			if (g_scenelist)
			{
				if (g_scenelist->pjd_scene_info)
				{
					free(g_scenelist->pjd_scene_info);
					g_scenelist->pjd_scene_info = NULL;
				}

				free(g_scenelist);
				g_scenelist = NULL;
			}
		}
		else
		{
			PJD_DEV_LIST_INFO g_jdList = storage_get_jddev(Type, 0);
			if (g_jdList == NULL)
			{
				return ECHO_ERROR;
			}

			g_jdList->nCount = get_dev_max(Type);

			#if 0
			log_printf("*********************************************\n");
			log_printf("g_jdList->nCount %d\n",g_jdList->nCount);

			log_printf("*********************************************\n");
			for(j = 0; j < count; j++)
			{
				log_printf("auDevInfo[%d].Index %d\n",j,auDevInfo[j].Index);
				log_printf("auDevInfo[%d].IsUsed %d\n",j,auDevInfo[j].IsUsed);
				log_printf("auDevInfo[%d].IsTune %d\n",j,auDevInfo[j].IsTune);
				log_printf("auDevInfo[%d].Addr %d\n",j,auDevInfo[j].Addr);
				log_printf("auDevInfo[%d].Name %s\n",j,auDevInfo[j].Name);
			}		
			log_printf("*********************************************\n");	
			#endif
			for(i = 0; i < g_jdList->nCount; i++)
			{
				isexist = 0;
				for(j = 0; j < count; j++)
				{
					if (g_jdList->pjd_dev_info[j].Index == (auDevInfo[j].Index))
					{
						isexist = 1;
						break;
					}
				}

				if (isexist == 0)
				{
					g_jdList->pjd_dev_info[i].IsUsed = 0;
				}
			}

			log_printf("*********************************************\n");	
			for (i = 0; i < count; i++)
			{
				for(j = 0; j < g_jdList->nCount; j++)
				{
					if (j == (auDevInfo[i].Index-1) && auDevInfo[i].IsUsed)
					{
						g_jdList->pjd_dev_info[j].Index = j;
						g_jdList->pjd_dev_info[j].Address = auDevInfo[i].Addr;
						g_jdList->pjd_dev_info[j].IsUsed = auDevInfo[i].IsUsed;
						g_jdList->pjd_dev_info[j].IsTune = auDevInfo[i].IsTune;
						sprintf(g_jdList->pjd_dev_info[j].Name, "%s", auDevInfo[i].Name);
					
						switch (Type)
						{
							case JD_DEV_LIGHT:
								g_jdList->pjd_dev_info[j].TextIDPos = SID_Jd_LouShang;
								g_jdList->pjd_dev_info[j].TextIDName = 	SID_Jd_Deng;
								break;

							case JD_DEV_WINDOW:
								g_jdList->pjd_dev_info[j].TextIDPos = SID_Jd_LouShang;
								g_jdList->pjd_dev_info[j].TextIDName = 	SID_Jd_ChuangLian;
								break;

							case JD_DEV_KONGTIAO:	
								g_jdList->pjd_dev_info[j].TextIDPos = SID_Jd_LouShang;
								g_jdList->pjd_dev_info[j].TextIDName = 	SID_Jd_KongTiaoJi;
								break;
								
							case JD_DEV_POWER:
								g_jdList->pjd_dev_info[j].IsTune = 0;
								g_jdList->pjd_dev_info[j].TextIDPos = SID_Jd_LouShang;
								g_jdList->pjd_dev_info[j].TextIDName = 	SID_Jd_ChaZuo1;
								break;
								
							case JD_DEV_GAS:
								g_jdList->pjd_dev_info[j].IsTune = 0;
								g_jdList->pjd_dev_info[j].TextIDPos = SID_Jd_LouShang;
								g_jdList->pjd_dev_info[j].TextIDName = 	SID_Jd_Meiqi;
								break;
						}
	
						#if 0
						log_printf("g_jdList->pjd_dev_info[%d].JdType %d\n",j,g_jdList->pjd_dev_info[j].JdType);
						log_printf("g_jdList->pjd_dev_info[%d].Index %d\n",j,g_jdList->pjd_dev_info[j].Index);
						log_printf("g_jdList->pjd_dev_info[%d].IsTune %d\n",j,g_jdList->pjd_dev_info[j].IsTune);
						log_printf("g_jdList->pjd_dev_info[%d].Addr %d\n",j,g_jdList->pjd_dev_info[j].Address);
						log_printf("g_jdList->pjd_dev_info[%d].Name %s\n",j,g_jdList->pjd_dev_info[j].Name);
						log_printf("g_jdList->pjd_dev_info[%d].TextIDPos %x\n",j,g_jdList->pjd_dev_info[j].TextIDPos);
						log_printf("g_jdList->pjd_dev_info[%d].TextIDName %x\n",j,g_jdList->pjd_dev_info[j].TextIDName);
						#endif
					}
				}	
			}
			log_printf("*********************************************\n");

			save_jd_storage(Type, g_jdList->pjd_dev_info, g_jdList->nCount);

			if (g_jdList)
			{
				if (g_jdList->pjd_dev_info)
				{
					free(g_jdList->pjd_dev_info);
					g_jdList->pjd_dev_info = NULL;
				}

				free(g_jdList);
				g_jdList = NULL;
			}
		}		
	}
	
	return ECHO_OK;
}
	
/*************************************************
  Function:    		ipmodule_jd_net_get_all_info
  Description:		获取状态
  Input: 
  	1.Type: 		情景和家电类型
  	2.start
  	3.end
  	4.num
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  Others:
*************************************************/
uint32 ipmodule_jd_net_get_all_info(void)
{
	uint8 i;
	uint8 ret = ECHO_ERROR;
	
	for (i = 0; i < 6; i++)
	{
		ret = ipmodule_jd_net_get_info(i);
		if (ret == ECHO_ERROR)
		{
			return ret;
		}
	}
	return ret;
}
#endif

