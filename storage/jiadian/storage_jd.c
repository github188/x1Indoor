/*********************************************************
  Copyright (C), 2009-2012
  File name:	Storage_jd.c
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.7.10
  Description:  存储模块--家电
  History:            
*********************************************************/
#include "storage_include.h"
// test
#define JDDEVINFO_SIZE			(sizeof(JD_DEV_INFO))
#define SENCEDEVINFO_SIZE		(sizeof(JD_SCENE_INFO))

#ifdef _JD_MODE_
static JD_ADDR_INFO g_alladdr;
static uint8 g_light_last_value_list[MAX_JD_LIGHT]={0};
static uint8 g_kongtiao_last_value_list[MAX_JD_KONGTIAO]={0};

/*************************************************
  Function:		get_dev_max
  Description: 	获得家电总个数
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 get_dev_max(AU_JD_DEV_TYPE dev)
{
	uint16 max = 0;
	switch (dev)
	{
		case JD_DEV_LIGHT:
			max = MAX_JD_LIGHT;
			break;
			
		case JD_DEV_WINDOW:
			max = MAX_JD_WINDOW;
			break;
			
		case JD_DEV_KONGTIAO:
			max = MAX_JD_KONGTIAO;
			break;
			
		case JD_DEV_POWER:
			max = MAX_JD_POWER;
			break;
			
		case JD_DEV_GAS:
			max = MAX_JD_GAS;
			break;

		default:
			max = 0;
			break;
	}

	return max;
}

/*************************************************
  Function:		free_jd_memory
  Description: 	释放家电存储内存
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_jd_memory(PJD_DEV_LIST_INFO* JDList)
{
	if ((*JDList))
	{
		if ((*JDList)->pjd_dev_info)
		{
			free((*JDList)->pjd_dev_info);
			(*JDList)->pjd_dev_info = NULL;
		}
		free((*JDList));	
		(*JDList) = NULL;
	}
}

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
void malloc_jd_memory(PJD_DEV_LIST_INFO *JDList,uint32 MaxNum)
{
	*JDList = (PJD_DEV_LIST_INFO)malloc(sizeof(JD_DEV_LIST_INFO));	

	if (*JDList)
	{
		(*JDList)->nCount = 0;
		(*JDList)->pjd_dev_info = (PJD_DEV_INFO)malloc(JDDEVINFO_SIZE * MaxNum);
	}
}	

/*************************************************
  Function:		malloc_scene_memory
  Description: 	申请情景存储内存
  Input:
  	1.JDList
  	2.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void malloc_scene_memory(PJD_SCENE_INFO_LIST *pSceneList,uint32 MaxNum)
{
	// 申请空间
	*pSceneList = (PJD_SCENE_INFO_LIST)malloc(sizeof(JD_SCENE_INFO_LIST));	
	if (*pSceneList)
	{
		(*pSceneList)->nCount = 0;
		(*pSceneList)->pjd_scene_info = (PJD_SCENE_INFO)malloc(sizeof(JD_SCENE_INFO) * MaxNum);
		if ((*pSceneList)->pjd_scene_info)
		{
			memset((*pSceneList)->pjd_scene_info, 0, sizeof(JD_SCENE_INFO) * MaxNum);
		}
		else
		{
			free(*pSceneList);
			*pSceneList = NULL;
		}
	}
}	

/*************************************************
  Function:		free_scene_memory
  Description: 	释放情景存储内存
  Input:		
  	1.JDList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_scene_memory(PJD_SCENE_INFO_LIST *pSceneList)
{
	if ((*pSceneList))
	{
		if ((*pSceneList)->pjd_scene_info)
		{
			free((*pSceneList)->pjd_scene_info);
			(*pSceneList)->pjd_scene_info = NULL;
		}
		free((*pSceneList));	
		(*pSceneList) = NULL;
	}
}

/*************************************************
  Function:		get_jd_fromstorage
  Description: 	获得家电信息
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_jd_storage(AU_JD_DEV_TYPE dev, PJD_DEV_LIST_INFO JDList, uint8 UsedOnly)
{
	uint8 i = 0;
	uint8 max = 0;
	if (JDList)
	{
		switch (dev)
		{
			case JD_DEV_LIGHT:
				JDList->nCount = Fread_common(JD_DEVICE_LIGHT_PATH, JDList->pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_LIGHT);
				max = MAX_JD_LIGHT;
				break;
				
			case JD_DEV_WINDOW:
				JDList->nCount = Fread_common(JD_DEVICE_WINDOW_PATH, JDList->pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_WINDOW);
				max = MAX_JD_WINDOW;
				break;
				
			case JD_DEV_KONGTIAO:
				JDList->nCount = Fread_common(JD_DEVICE_KONGTIAO_PATH, JDList->pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_KONGTIAO);
				max = MAX_JD_KONGTIAO;
				break;
				
			case JD_DEV_POWER:
				JDList->nCount = Fread_common(JD_DEVICE_POWER_PATH, JDList->pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_POWER);
				max = MAX_JD_POWER;
				break;
				
			case JD_DEV_GAS:
			default:
				JDList->nCount = Fread_common(JD_DEVICE_GAS_PATH, JDList->pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_GAS);
				max = MAX_JD_GAS;
				break;
		}

		if (UsedOnly && JDList->nCount > 0)			// 只显示启用的设备
		{
			PJD_DEV_LIST_INFO TempList = NULL;
			malloc_jd_memory(&TempList, max);
			if (TempList == NULL)
			{
				return;
			}
			TempList->nCount = 0;
			memset(TempList->pjd_dev_info, 0, max*JDDEVINFO_SIZE);
			
			for (i = 0; i < max; i++)
			{
				if (JDList->pjd_dev_info[i].IsUsed)
				{
					memcpy(&TempList->pjd_dev_info[TempList->nCount], &JDList->pjd_dev_info[i], JDDEVINFO_SIZE);
					TempList->nCount++;
					//memcpy(&JDList->pjd_dev_info[i], &JDList->pjd_dev_info[i+1], (max-i-1)*sizeof(JD_DEV_INFO));
					//JDList->nCount--;
				}
			}

			JDList->nCount = TempList->nCount;
			memset(JDList->pjd_dev_info, 0, max*JDDEVINFO_SIZE);
			memcpy(JDList->pjd_dev_info, TempList->pjd_dev_info, JDList->nCount*JDDEVINFO_SIZE);
			free_jd_memory(&TempList);
		}
	}
}

/*************************************************
  Function:		get_jd_storage_scene
  Description: 	获得家电信息
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 get_jd_storage_scene(AU_JD_DEV_TYPE dev, PJD_DEV_INFO pjd_dev_info)
{
	int ret = 0;
	
	switch (dev)
	{
		case JD_DEV_LIGHT:
			ret = Fread_common(JD_DEVICE_LIGHT_PATH, pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_LIGHT);
			break;
			
		case JD_DEV_WINDOW:
			ret = Fread_common(JD_DEVICE_WINDOW_PATH, pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_WINDOW);
			break;
			
		case JD_DEV_KONGTIAO:
			ret = Fread_common(JD_DEVICE_KONGTIAO_PATH, pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_KONGTIAO);
			break;
			
		case JD_DEV_POWER:
			ret = Fread_common(JD_DEVICE_POWER_PATH, pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_POWER);
			break;
			
		case JD_DEV_GAS:
		default:
			ret = Fread_common(JD_DEVICE_GAS_PATH, pjd_dev_info, JDDEVINFO_SIZE, MAX_JD_GAS);
			break;
	}
	return ret;
}


/*************************************************
  Function:		save_jd_storage
  Description: 	按模块存储
  Input:	
  	1.dev
  	2.JDList	
  	3.nCount
  Output:		无
  Return:		无
  Others:
*************************************************/
void save_jd_storage(AU_JD_DEV_TYPE dev, PJD_DEV_INFO JDList, uint8 nCount)
{
	switch (dev)
	{
		case JD_DEV_LIGHT:
			Fwrite_common(JD_DEVICE_LIGHT_PATH, JDList, JDDEVINFO_SIZE, nCount);
			break;
			
		case JD_DEV_WINDOW:
			Fwrite_common(JD_DEVICE_WINDOW_PATH, JDList, JDDEVINFO_SIZE, nCount);
			break;
			
		case JD_DEV_KONGTIAO:
			Fwrite_common(JD_DEVICE_KONGTIAO_PATH, JDList, JDDEVINFO_SIZE, nCount);
			break;
			
		case JD_DEV_POWER:
			Fwrite_common(JD_DEVICE_POWER_PATH, JDList, JDDEVINFO_SIZE, nCount);
			break;
			
		case JD_DEV_GAS:
			Fwrite_common(JD_DEVICE_GAS_PATH, JDList, JDDEVINFO_SIZE, nCount);
			break;

		default:
			break;
	}

	// add by luofl 2011-07-20 等待数据完全写入FLASH
	DelayMs_nops(300);	
}

/*************************************************
  Function:		storage_get_jddev
  Description: 	获得设备信息
  Input:		
  	1.dev
  	2.UsedOnly
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_DEV_LIST_INFO storage_get_jddev(AU_JD_DEV_TYPE dev, uint8 UsedOnly)
{
	PJD_DEV_LIST_INFO info = NULL;
	int max = 0;
	//uint8 i;
	
	max = get_dev_max(dev);
	
	malloc_jd_memory(&info, max);
	if (NULL == info)
	{
		return NULL;
	}
	get_jd_storage(dev,info,UsedOnly);

#if 0
	log_printf("storage_get_jddev: %d\n", info->nCount);
	for (i = 0; i < info->nCount; i++)
	{
		log_printf("Index:%d, IsUsed:%d\n", info->pjd_dev_info[i].Index, info->pjd_dev_info[i].IsUsed);
	}
#endif
	return info;
}

/*************************************************
  Function:		is_same_addr
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static uint8 is_same_addr (AU_JD_DEV_TYPE dev,uint16 addr, uint8 index)
{
	uint16 ret = 0;
	uint8 i,j;
	uint8 maxnum[JD_DEV_MAX] = {MAX_JD_LIGHT,MAX_JD_WINDOW,MAX_JD_KONGTIAO,MAX_JD_POWER,MAX_JD_GAS};
	switch (dev)
	{
		case JD_DEV_LIGHT:
			ret = g_alladdr.light_addr[index];
			break;
			
		case JD_DEV_WINDOW:
			ret = g_alladdr.window_addr[index];
			break;
			
		case JD_DEV_KONGTIAO:
			ret = g_alladdr.kongtiao_addr[index];
			break;
			
		case JD_DEV_POWER:
			ret = g_alladdr.power_addr[index];
			break;
			
		case JD_DEV_GAS:
		default:
			ret = g_alladdr.gas_addr[index];
			break;
	}
	if (ret != 0)
	{
		return 0;
	}
	
	for (i = 0; i < JD_DEV_MAX; i++)
	{
		for (j = 0; j < maxnum[i]; j++)
		{
			switch (i)
			{
				case JD_DEV_LIGHT:
					if (g_alladdr.light_addr[j] == addr)
					{
						ret = 1;
					}
					break;
					
				case JD_DEV_WINDOW:
					if (g_alladdr.window_addr[j] == addr)
					{
						ret = 1;
					}
					break;
					
				case JD_DEV_KONGTIAO:
					if (g_alladdr.kongtiao_addr[j] == addr)
					{
						ret = 1;
					}
					break;
					
				case JD_DEV_POWER:
					if (g_alladdr.power_addr[j] == addr)
					{
						ret = 1;
					}
					break;
					
				case JD_DEV_GAS:
				default:
					if (g_alladdr.gas_addr[j] == addr)
					{
						ret = 1;
					}
					break;
			}

			if (ret)
			{
				return 1;
			}
		}
	}

	return 0;
}

/*************************************************
  Function:		set_addr
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static void set_addr(AU_JD_DEV_TYPE dev, uint8 Index,uint16 addr)
{
	switch (dev)
	{
		case JD_DEV_LIGHT:
			g_alladdr.light_addr[Index] = addr;
			break;
			
		case JD_DEV_WINDOW:
			g_alladdr.window_addr[Index] = addr;
			break;
			
		case JD_DEV_KONGTIAO:
			g_alladdr.kongtiao_addr[Index] = addr;
			break;
			
		case JD_DEV_POWER:
			g_alladdr.power_addr[Index] = addr;
			break;
			
		case JD_DEV_GAS:
		default:
			g_alladdr.gas_addr[Index] = addr;
			break;

	}
}

/*************************************************
  Function:		is_same_addr
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static uint8 is_same_sence_addr (uint16 addr, uint8 index)
{
	uint16 ret = 0;
	uint8 j;
	
	ret = g_alladdr.sence_addr[index];
	if (ret != 0)
	{
	    return 0;
	}
	
	for (j = 0; j < MAX_JD_SCENE_NUM; j++)
	{
		if (g_alladdr.sence_addr[j] == addr)
		{
			ret = 1;
		}
		if (ret)
		{
			return 1;
		}
	}
	return 0;
}

/*************************************************
  Function:		set_sence_addr
  Description: 	设置情景地址
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static void set_sence_addr(uint8 Index, uint16 addr)
{
    g_alladdr.sence_addr[Index] = addr;
}

/*************************************************
  Function:		storage_add_jddev
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_jddev (PJD_DEV_INFO info)
{
	PJD_DEV_LIST_INFO jddevlist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	uint8 i,j;
	uint8 max = 0;

	if (NULL == info)
	{
		return ECHO_STORAGE_ERR;
	}

	if (info->IsUsed && is_same_addr(info->JdType,info->Address,info->Index))
	{
		return ECHO_STORAGE_SAME_ADDR;
	}
	
	max = get_dev_max(info->JdType);
	jddevlist = storage_get_jddev(info->JdType, FALSE);

	if (jddevlist)
	{
		for (i = 0; i < jddevlist->nCount; i++)
		{		
			if (jddevlist->pjd_dev_info[i].IsUsed)
			{
			#ifndef _USERDEFINE_JDNAME_
				if ((jddevlist->pjd_dev_info[i].TextIDPos == info->TextIDPos&& jddevlist->pjd_dev_info[i].TextIDName == info->TextIDName)
							&& jddevlist->pjd_dev_info[i].Index != info->Index)
				{
					ret = ECHO_STORAGE_SAME_NAME;
					break;
				}
			#endif	
			}
		}

		if (ECHO_STORAGE_SAME_ADDR != ret && ECHO_STORAGE_SAME_NAME != ret)
		{
			jddevlist->pjd_dev_info[info->Index].IsUsed = info->IsUsed;
			if (0 == info->IsUsed)
			{
				jddevlist->pjd_dev_info[info->Index].Address = 0;
				jddevlist->pjd_dev_info[info->Index].TextIDName = 0;
				jddevlist->pjd_dev_info[info->Index].TextIDPos= 0;
				jddevlist->pjd_dev_info[info->Index].IsGroup = 0;
				jddevlist->pjd_dev_info[info->Index].IsTune = 0;
				for (j = 0; j < JD_SCENE_MAX; j++)
				{
					jddevlist->pjd_dev_info[info->Index].Commond[j] = 0;
					jddevlist->pjd_dev_info[info->Index].param[j][0] = 0;
				}
			}
			else
			{
				for (i = 0; i < JD_SCENE_MAX; i++)
				{	
					info->Commond[i] = jddevlist->pjd_dev_info[info->Index].Commond[i];
					info->param[i][0] = jddevlist->pjd_dev_info[info->Index].param[i][0];
					if (info->Commond[i] > 1)
					{
						info->Commond[i] = 0;
					}

					if (info->param[i][0] > 10)
					{
						info->param[i][0] = 4;
					}
					info->param[i][1] = jddevlist->pjd_dev_info[info->Index].param[i][1];
					info->param[i][2] = jddevlist->pjd_dev_info[info->Index].param[i][2];
					info->param[i][3] = jddevlist->pjd_dev_info[info->Index].param[i][3];
					info->param[i][4] = jddevlist->pjd_dev_info[info->Index].param[i][4];
					info->param[i][5] = jddevlist->pjd_dev_info[info->Index].param[i][5];
					info->param[i][6] = jddevlist->pjd_dev_info[info->Index].param[i][6];
				}
				memcpy(&jddevlist->pjd_dev_info[info->Index],info,sizeof(JD_DEV_INFO));			
			}
			set_addr(info->JdType, info->Index, jddevlist->pjd_dev_info[info->Index].Address);
			save_jd_storage(info->JdType, jddevlist->pjd_dev_info, max);
			ret = ECHO_STORAGE_OK;
		}
	}

	free_jd_memory(&jddevlist);
	return ret;
}

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
ECHO_STORAGE storage_del_devinfo (AU_JD_DEV_TYPE dev, uint8 Index)
{
	uint8 i;
	PJD_DEV_LIST_INFO jddevlist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	uint16 num = 0;

	jddevlist = storage_get_jddev(dev, FALSE);
	
	if (jddevlist)
	{
		num = jddevlist->nCount;
		for (i = 0; i < jddevlist->nCount; i++)
		{
			if (Index == jddevlist->pjd_dev_info[i].Index)
			{
				memset(&jddevlist->pjd_dev_info[i],0,sizeof(JD_DEV_INFO));
			}
		}
		save_jd_storage(dev, jddevlist->pjd_dev_info, num);
		ret = ECHO_STORAGE_OK;
	}

	free_jd_memory(&jddevlist);
	return ret;
}

/*************************************************
  Function:		storage_set_scene
  Description: 	情景设置
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_set_scene(AU_JD_DEV_TYPE dev,PJD_DEV_LIST_INFO jdlist)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	if (jdlist)
	{
		save_jd_storage(dev, jdlist->pjd_dev_info, jdlist->nCount);
		ret = ECHO_STORAGE_OK;
	}

	return ret;
}

/*************************************************
  Function:		storage_get_scene
  Description: 	获得这个情景的所有家电设备信息
  Input:		
  	1.mode
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_DEV_LIST_INFO storage_get_scene(void)
{
	uint16 i,num,ret = 0;
	uint8 maxnum[JD_DEV_MAX] = {MAX_JD_LIGHT,MAX_JD_WINDOW,MAX_JD_KONGTIAO,MAX_JD_POWER,MAX_JD_GAS};
	PJD_DEV_LIST_INFO jdlist = NULL;
	malloc_jd_memory(&jdlist, MAX_JD_DEVICE_NUM);
	if (jdlist)
	{
		num = 0;
		for (i = 0; i < JD_DEV_MAX; i++)
		{
			ret = get_jd_storage_scene((JD_DEV_LIGHT+i), &jdlist->pjd_dev_info[num]);
			log_printf("i = %d ret = %d\n",i,ret);
			num += maxnum[i];
			jdlist->nCount += ret;
		}

	}
	return jdlist;
}

/*************************************************
  Function:		storage_set_scene_info
  Description: 	情景信息设置
  Input:		
  	1.pSceneList	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_set_scene_info(PJD_SCENE_INFO_LIST pSceneList)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	if (pSceneList)
	{
		Fwrite_common(JD_SCENE_PATH, pSceneList->pjd_scene_info, sizeof(JD_SCENE_INFO), JD_SCENE_MAX);
		ret = ECHO_STORAGE_OK;
		// add by luofl 2011-07-20 等待数据完全写入FLASH
		//DelayMs_nops(500);
	}

	return ret;
}

/*************************************************
  Function:		storage_get_scene_info
  Description: 	获得情景模式信息链表
  Input:		
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_SCENE_INFO_LIST storage_get_scene_info(void)
{
	uint8 i;
	PJD_SCENE_INFO_LIST pSceneList = NULL;
	malloc_scene_memory(&pSceneList, JD_SCENE_MAX);
	if (pSceneList == NULL)
	{
		return NULL;
	}
	pSceneList->nCount = Fread_common(JD_SCENE_PATH, pSceneList->pjd_scene_info, sizeof(JD_SCENE_INFO), JD_SCENE_MAX);
	if (pSceneList->nCount == 0)
	{
		pSceneList->nCount = JD_SCENE_MAX;
		for (i = 0; i < JD_SCENE_MAX; i++)
		{
			pSceneList->pjd_scene_info[i].SceneIndex = i;
			pSceneList->pjd_scene_info[i].IsUsed = TRUE;
			sprintf(pSceneList->pjd_scene_info[i].SceneName, "Scene Mode %d", i+1);
		}
		
	}
	return pSceneList;
}
#ifdef _AURINE_ELEC_NEW_
/*************************************************
  Function:		storage_set_scene_info
  Description: 	情景信息设置
  Input:		
  	1.pSceneList	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_set_aurien_scene_info(PJD_SCENE_INFO pSceneList)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	Fwrite_common(JD_AURINE_SCENE_PATH, pSceneList, sizeof(JD_SCENE_INFO), MAX_JD_SCENE_NUM);
	ret = ECHO_STORAGE_OK;
	// add by luofl 2011-07-20 等待数据完全写入FLASH
	//DelayMs_nops(500);

	return ret;
}

/*************************************************
  Function:		storage_get_scene_info
  Description: 	获得情景模式信息链表
  Input:		
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PJD_SCENE_INFO_LIST storage_get_aurine_scene_info(uint8 UsedOnly)
{
    uint8 i, max, len;
    char  tmp[50];
    char text_tmp1[50];
	char text_tmp2[50];
	
    PJD_SCENE_INFO_LIST TempList = NULL;
	PJD_SCENE_INFO_LIST pSceneList = NULL;
	
	malloc_scene_memory(&pSceneList, MAX_JD_SCENE_NUM);
	if (pSceneList == NULL)
	{
		return NULL;
	}
	max = MAX_JD_SCENE_NUM;
	pSceneList->nCount = Fread_common(JD_AURINE_SCENE_PATH, pSceneList->pjd_scene_info, sizeof(JD_SCENE_INFO),MAX_JD_SCENE_NUM);
   
	if (pSceneList->nCount == 0)
	{
    	pSceneList->nCount = MAX_JD_SCENE_NUM;
    	for (i = 0; i < MAX_JD_SCENE_NUM; i++)
    	{
    	    if (i < 5)
    	    {
    	        pSceneList->pjd_scene_info[i].SceneIndex = i;
    	        pSceneList->pjd_scene_info[i].Address = i+1;
    	        pSceneList->pjd_scene_info[i].IsUsed = TRUE;
    	        pSceneList->pjd_scene_info[i].SceneID = i;
    	    }
    	    else
    	    {
    	        pSceneList->pjd_scene_info[i].TextIDName = 0;
    	        pSceneList->pjd_scene_info[i].TextIDPos = 0;
    	        pSceneList->pjd_scene_info[i].IsUsed = FALSE;
    	        pSceneList->pjd_scene_info[i].Address = 0;
    	    }
	        
	        len = 50;
	        memset((char *)text_tmp2, 0, sizeof(text_tmp2));
		    memset((char *)text_tmp1, 0, sizeof(text_tmp1));
	        //GuiDspGetIDString(SID_Jd_Qingjing, text_tmp1, len);
	        memset(tmp,0,sizeof(tmp));
	        sprintf(tmp,"%d",i+1); 
	        //Gbk2Unicode(text_tmp2, tmp);
	        //Unicodestrcat(text_tmp1, text_tmp2);
		    sprintf(pSceneList->pjd_scene_info[i].SceneName, "%s", text_tmp1);
	    }
    }
	
    if (UsedOnly)
    {
        if (pSceneList->nCount == 0)
    	{
        	pSceneList->nCount = MAX_JD_SCENE_NUM;
        }

		malloc_scene_memory(&TempList, MAX_JD_SCENE_NUM);
		if (TempList == NULL)
		{
			return NULL;
		}
		TempList->nCount = 0;
		memset(TempList->pjd_scene_info, 0, MAX_JD_SCENE_NUM*sizeof(JD_SCENE_INFO));
		for (i = 0; i < MAX_JD_SCENE_NUM; i++)
		{
			if (pSceneList->pjd_scene_info[i].IsUsed == 1)
			{
				memcpy(&TempList->pjd_scene_info[TempList->nCount], &pSceneList->pjd_scene_info[i], SENCEDEVINFO_SIZE);
				TempList->nCount++;
			}
		}

		pSceneList->nCount = TempList->nCount;
		memset(pSceneList->pjd_scene_info, 0, MAX_JD_SCENE_NUM*SENCEDEVINFO_SIZE);
		memcpy(pSceneList->pjd_scene_info, TempList->pjd_scene_info, pSceneList->nCount*SENCEDEVINFO_SIZE);
		free_scene_memory(&TempList);
    }
    for (i = 0; i < 32 ; i++)
    {
        //log_printf("pSceneList = %d\n",pSceneList->pjd_scene_info[i].Address);
    }
    return pSceneList;
}

/*************************************************
  Function:		storage_add_jddev
  Description: 	添加设备类型
  Input:		
  	1.info	
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_aurine_scenemode(PJD_SCENE_INFO info)
{
    PJD_SCENE_INFO_LIST sencelist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	uint8 i;
	uint8 max = 0;

	if (NULL == info)
	{
		return ECHO_STORAGE_ERR;
	}

	if (info->IsUsed && is_same_sence_addr(info->Address,info->SceneID))
	{
		return ECHO_STORAGE_SAME_ADDR;
	}
	
	max = MAX_JD_SCENE_NUM;
	sencelist = storage_get_aurine_scene_info(FALSE);
	if (sencelist)
	{
	    for (i = 5; i < sencelist->nCount; i++)
		{		
			if (sencelist->pjd_scene_info[i].IsUsed)
			{
			#ifndef _USERDEFINE_JDNAME_
				if ((sencelist->pjd_scene_info[i].TextIDPos == info->TextIDPos&& sencelist->pjd_scene_info[i].TextIDName == info->TextIDName)
				&& (sencelist->pjd_scene_info[i].SceneID != info->SceneID))
				{
					ret = ECHO_STORAGE_SAME_NAME;
					break;
				}
			#endif	
			}
		}

		if (ECHO_STORAGE_SAME_ADDR != ret && ECHO_STORAGE_SAME_NAME != ret)
		{
			sencelist->pjd_scene_info[info->SceneID].IsUsed = info->IsUsed;
			if (0 == info->IsUsed)
			{		
			    sencelist->pjd_scene_info[info->SceneID].IsUsed = 0;	
			    sencelist->pjd_scene_info[info->SceneID].Address = 0;
                sencelist->pjd_scene_info[info->SceneID].TextIDName = 0;
				sencelist->pjd_scene_info[info->SceneID].TextIDPos = 0;
							
			}
			else
			{
				memcpy(&sencelist->pjd_scene_info[info->SceneID],info,sizeof(JD_SCENE_INFO));			
			}
			set_sence_addr(info->SceneID, sencelist->pjd_scene_info[info->SceneID].Address);
			
			storage_set_aurien_scene_info(sencelist->pjd_scene_info);
			ret = ECHO_STORAGE_OK;
		}
	}

	free_scene_memory(&sencelist);
	return ret;
}

/*************************************************
  Function:		storage_get_aurine_light_value
  Description: 	
  Input:		
  	1.value_list
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_get_aurine_light_value(uint8 * value_list)
{
    uint16 i, count;
	char tmp[5] = {0};
    char text[2*MAX_JD_LIGHT+1] = {0};
 
	count = Fread_common(JD_LIGHT_VALUE_PATH, text, 1, 2*MAX_JD_LIGHT);
	if (count > 0)
	{
		for (i = 0; i < MAX_JD_LIGHT; i++)
		{
			memset(tmp, 0, sizeof(tmp));
			tmp[0] = text[2*i];
			tmp[1] = text[2*i+1];			
			g_light_last_value_list[i] = atoi(tmp);
			value_list[i] = atoi(tmp);
		}
	}
}

/*************************************************
  Function:		jd_aurine_get_lightlastvalue
  Description: 	
  Input:		
  Output:		无
  Return:		
  Others:
*************************************************/
void jd_aurine_get_lightlastvalue(uint8 *value_list)
{
	int i;
	
	for (i = 0; i < MAX_JD_LIGHT; i++)
	{
		value_list[i] = g_light_last_value_list[i];
	}
}

/*************************************************
  Function:		storage_clear_devinfo
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_save_aurine_light_value(uint8 * value_list)
{
    uint16 i;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

    char text[2*MAX_JD_LIGHT+1]={0};
	for(i = 0; i < MAX_JD_LIGHT; i++)
	{
		g_light_last_value_list[i] = value_list[i];
		if (g_light_last_value_list[i] >= 10)
		{
			text[i*2] = '0'+g_light_last_value_list[i]/10;
			text[i*2+1] = '0'+g_light_last_value_list[i]%10;
		}
		else
		{
			text[i*2] = '0';
			text[i*2+1] = '0'+g_light_last_value_list[i];
		}
	}

    ret = Fwrite_common(JD_LIGHT_VALUE_PATH, text, 1, 2*MAX_JD_LIGHT);
    DelayMs_nops(300);
	
	return ret;
}

/*************************************************
  Function:		storage_get_aurine_kongtiao_value
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_aurine_kongtiao_value(uint8 *value_list)
{
    uint16 i, count;
	char tmp[5]={0};
    char text[2*MAX_JD_KONGTIAO+1]={0};
 
	count = Fread_common(JD_KONGTIAO_VALUE_PATH, text, 1, 2*MAX_JD_KONGTIAO);
	if (count > 0)
	{
		for (i = 0; i < MAX_JD_KONGTIAO; i++)
		{
			memset(tmp, 0, sizeof(tmp));
			tmp[0] = text[2*i];
			tmp[1] = text[2*i+1];			
			g_kongtiao_last_value_list[i] = atoi(tmp);
			value_list[i] = atoi(tmp);
		}
	}
}

/*************************************************
  Function:		storage_save_aurine_kongtiao_value
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_save_aurine_kongtiao_value(uint8 * value_list)
{
    uint16 i;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

    char text[2*MAX_JD_KONGTIAO+1]={0};
	for(i = 0; i < MAX_JD_KONGTIAO; i++)
	{
		g_kongtiao_last_value_list[i] = value_list[i];

		if (g_kongtiao_last_value_list[i] >= 10)
		{
			text[i*2] = '0'+g_kongtiao_last_value_list[i]/10;
			text[i*2+1] = '0'+g_kongtiao_last_value_list[i]%10;
		}
		else
		{
			text[i*2] = '0';
			text[i*2+1] = '0'+g_kongtiao_last_value_list[i];
		}
	}
    ret = Fwrite_common(JD_KONGTIAO_VALUE_PATH, text, 1, 2*MAX_JD_KONGTIAO);
    DelayMs_nops(300);
	
	return ret;
}

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
void storage_get_kongtiao_air_volume(uint8 *value_list)
{
    uint16 i, count;
    uint8 text[MAX_JD_KONGTIAO]={0};
 
	count = Fread_common(JD_KONGTIAO_VOLUME_PATH, text, sizeof(uint8), MAX_JD_KONGTIAO);
	if (count > 0)
	{
		for (i = 0; i < MAX_JD_KONGTIAO; i++)
		{
			value_list[i] = text[i];
		}
	}
}

/*************************************************
  Function:			storage_save_kongtiao_air_volume
  Description: 		保存风量
  Input:		
  	1.value_list	
  Output:			无
  Return:			ECHO_STORAGE
  Others:			无
*************************************************/
ECHO_STORAGE storage_save_kongtiao_air_volume(uint8 * value_list)
{
    uint16 i;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

    uint8 text[MAX_JD_KONGTIAO]={0};
	for(i = 0; i < MAX_JD_KONGTIAO; i++)
	{
		text[i] = value_list[i];
	}
    ret = Fwrite_common(JD_KONGTIAO_VOLUME_PATH, text, sizeof(uint8), MAX_JD_KONGTIAO);
    DelayMs_nops(300);
	
	return ret;
}
#endif
#endif
/*************************************************
  Function:		storage_clear_devinfo
  Description: 	清空
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_devinfo(AU_JD_DEV_TYPE dev)
{
	JD_DEV_INFO buf[MAX_JD_LIGHT];
	uint16 num = 0;	
	save_jd_storage(dev, buf, num);
	return ECHO_STORAGE_OK;
}

/*************************************************
  Function:		storage_clear_all_devinfo
  Description:  清空
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_all_devinfo(void)
{
	uint8 i;
	for (i = 0; i < JD_DEV_MAX; i++)
	{
		storage_clear_devinfo(JD_DEV_LIGHT+i);
	}
}

/*************************************************
  Function:		storage_jd_init
  Description:  初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_jd_init(void)
{
	uint8 i, j;
	uint8 maxnum[JD_DEV_MAX] = {MAX_JD_LIGHT, MAX_JD_WINDOW, MAX_JD_KONGTIAO, MAX_JD_POWER, MAX_JD_GAS};
	JD_DEV_INFO buf[MAX_JD_LIGHT];
	uint8 value_list[MAX_JD_LIGHT];
	for (i = 0; i < JD_DEV_MAX; i++)
	{
		get_jd_storage_scene((JD_DEV_LIGHT+i), buf);
		for (j = 0; j < maxnum[i]; j++)
		{
			switch (i)
			{
				case JD_DEV_LIGHT:
					g_alladdr.light_addr[j] = buf[j].Address;
					break;
					
				case JD_DEV_WINDOW:
					g_alladdr.window_addr[j] = buf[j].Address;
					break;
					
				case JD_DEV_KONGTIAO:
					g_alladdr.kongtiao_addr[j] = buf[j].Address;
					break;
					
				case JD_DEV_POWER:
					g_alladdr.power_addr[j] = buf[j].Address;
					break;
					
				case JD_DEV_GAS:
				default:
					g_alladdr.gas_addr[j] = buf[j].Address;
					break;
			}
		}
	}

	#ifdef _AURINE_ELEC_NEW_
	storage_get_aurine_light_value(value_list);
	#endif
}
#else
/*************************************************
  Function:		storage_jd_init
  Description:  初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_jd_init(void)
{
}
#endif

