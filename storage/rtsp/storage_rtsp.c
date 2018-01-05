/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_rtsp.c
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  
  History:            
*********************************************************/
#include "storage_include.h"

#define COMMUNITYDEVICE_SIZE		(sizeof(COMMUNITYDEVICE))
#define HOMEDEVICE_SIZE				(sizeof(HOMEDEVICE))
#define NEWCOMMUNITYDEVICE_SIZE		(sizeof(NEWCOMMUNITYDEVICE))
#define NEWHOMEDEVICE_SIZE			(sizeof(NEWHOMEDEVICE))

static uint8 g_index[MAX_HOME_NUM] = {0};

/*************************************************
  Function:		get_monitordev_from_storage
  Description: 	
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_monitordev_from_storage(int type, int max, PMonitorDeviceList *list)
{
	switch(type)
	{
		case HOME_CAMERA:
			if (list)
			{
				if (max)
				{
					(*list)->Homenum = Fread_common(HOME_CAMERA_PATH, (*list)->Homedev, HOMEDEVICE_SIZE, max);
				}
				else
				{
					(*list)->Homenum = 0;
				}
			}
			break;

		case COMMUNITY_CAMERA:
			if (list)
			{
				if (max)
				{
					(*list)->Comnum = Fread_common(COMMUNITY_CAMERA_PATH, (*list)->Comdev, COMMUNITYDEVICE_SIZE, max);
				}
				else
				{
					(*list)->Comnum = 0;
				}
			}
			break;

		default:
			break;
	}
	
}

/*************************************************
  Function:		get_homedev_from_storage
  Description: 	
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int get_homedev_from_storage(HOMEDEVICE* Homedev)
{
	int size = Fread_common(HOME_CAMERA_PATH, Homedev, HOMEDEVICE_SIZE, MAX_HOME_NUM);

	return size;
}

/*************************************************
  Function:		save_monitordev_storage
  Description: 	按模块存储
  Input:		
  	1.list
  Output:		无
  Return:		无
  Others:
*************************************************/
static ECHO_STORAGE save_monitordev_storage(int type, PMonitorDeviceList list)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	switch (type)
	{
		case HOME_CAMERA:
			if (list)
			{
				ret = Fwrite_common(HOME_CAMERA_PATH, list->Homedev, HOMEDEVICE_SIZE, MAX_HOME_NUM);
			}
			break;

		case COMMUNITY_CAMERA:
			if (list)
			{
				ret = Fwrite_common(COMMUNITY_CAMERA_PATH, list->Comdev, COMMUNITYDEVICE_SIZE, list->Comnum);
			}
			break;

		default:
			break;
	}
	
	return ret;
}

/*************************************************
  Function:		storage_free_monitordev_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_monitordev_memory(PMonitorDeviceList *list)
{
	if (*list)
	{
		if ((*list)->Comdev)
		{
			free((*list)->Comdev);
			(*list)->Comdev = NULL;
		}

		if ((*list)->Homedev)
		{
			free((*list)->Homedev);
			(*list)->Homedev = NULL;
		}
		free((*list));	
		(*list) = NULL;
	}
}

/*************************************************
  Function:		storage_malloc_monitordev_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_monitordev_memory (uint32 HomeMaxNum, uint32 CommMaxNum, PMonitorDeviceList *list)
{
	*list = (PMonitorDeviceList)malloc(sizeof(MonitorDeviceList));	
	if (*list)
	{
		(*list)->Comnum = 0;
		if (CommMaxNum)
		{
			(*list)->Comdev = (PCOMMUNITYDEVICE)malloc(COMMUNITYDEVICE_SIZE * CommMaxNum);
		}
		else
		{
			(*list)->Comdev = NULL;
		}

		(*list)->Homenum = 0;
		if (HomeMaxNum)
		{
			(*list)->Homedev = (PHOMEDEVICE)malloc(HOMEDEVICE_SIZE * HomeMaxNum);
		}
		else
		{
			(*list)->Homedev = NULL;
		}		
	}
}		

/*************************************************
  Function:		storage_malloc_new_monitordev_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_new_monitordev_memory (uint32 HomeMaxNum, uint32 CommMaxNum, PNewMonitorDeviceList *list)
{
	*list = (PNewMonitorDeviceList)malloc(sizeof(NewMonitorDeviceList));	
	if (*list)
	{
		(*list)->Comnum = 0;
		if (CommMaxNum)
		{
			(*list)->Comdev = (PNEWCOMMUNITYDEVICE)malloc(NEWCOMMUNITYDEVICE_SIZE * CommMaxNum);
		}
		else
		{
			(*list)->Comdev = NULL;
		}

		(*list)->Homenum = 0;
		if (HomeMaxNum)
		{
			(*list)->Homedev = (PNEWHOMEDEVICE)malloc(NEWHOMEDEVICE_SIZE * HomeMaxNum);
		}
		else
		{
			(*list)->Homedev = NULL;
		}		
	}
}	

/*************************************************
  Function:		storage_get_monitordev
  Description: 	
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_monitordev(PMonitorDeviceList *Devlist)
{
	int MaxCom;
	int ComDevSize = get_size(COMMUNITY_CAMERA_PATH);
	
	MaxCom = ComDevSize/COMMUNITYDEVICE_SIZE;
	storage_malloc_monitordev_memory(MAX_HOME_NUM, MaxCom, Devlist);
	get_monitordev_from_storage(HOME_CAMERA, MAX_HOME_NUM, Devlist);
	get_monitordev_from_storage(COMMUNITY_CAMERA, MaxCom, Devlist);
}

/*************************************************
  Function:		storage_get_home_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:		获取家居监视列表
*************************************************/
void storage_get_home_monitordev(PMonitorDeviceList *Devlist)
{
	storage_malloc_monitordev_memory(MAX_HOME_NUM, 0, Devlist);
	get_monitordev_from_storage(HOME_CAMERA, MAX_HOME_NUM, Devlist);
}

/*************************************************
  Function:		storage_get_comm_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:		获取社区监视列表
*************************************************/
void storage_get_comm_monitordev(PMonitorDeviceList *Devlist)
{
	int MaxCom;
	int ComDevSize = get_size(COMMUNITY_CAMERA_PATH);
	
	MaxCom = ComDevSize/COMMUNITYDEVICE_SIZE;
	storage_malloc_monitordev_memory(0, MaxCom, Devlist);
	get_monitordev_from_storage(COMMUNITY_CAMERA, MaxCom, Devlist);
}

/*************************************************
  Function:		storage_get_monitordev_used
  Description: 	
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_monitordev_used(PMonitorDeviceList *Devlist)
{
	int MaxCom,Maxhome,i;
	PMonitorDeviceList gDevlist;
	HOMEDEVICE g_homedev[MAX_HOME_NUM];
	int ComDevSize = get_size(COMMUNITY_CAMERA_PATH);
	MaxCom = ComDevSize/COMMUNITYDEVICE_SIZE;
	
	storage_malloc_monitordev_memory(MAX_HOME_NUM, MaxCom, &gDevlist);
	get_monitordev_from_storage(HOME_CAMERA, MAX_HOME_NUM, &gDevlist);

	for (i = 0; i < MAX_HOME_NUM; i++)
	{
		g_index[i] = 0;
	}
	
	Maxhome = 0;
	for (i = 0; i < gDevlist->Homenum; i++)
	{
		if (gDevlist->Homedev[i].EnableOpen)
		{
			memcpy(&g_homedev[Maxhome], &gDevlist->Homedev[i], sizeof(HOMEDEVICE));
			g_index[Maxhome] = i;
			Maxhome++;
		}
	}
	log_printf("ComDev %d HomeDev %d\n",MaxCom,Maxhome);
	storage_malloc_monitordev_memory(Maxhome, MaxCom, Devlist);
	if (Maxhome > 0)
	{
		(*Devlist)->Homenum = Maxhome;
		memcpy((*Devlist)->Homedev, g_homedev, Maxhome*sizeof(HOMEDEVICE));
	}
	get_monitordev_from_storage(COMMUNITY_CAMERA, MaxCom, Devlist);
	storage_free_monitordev_memory(&gDevlist);
}

/*************************************************
  Function:		storage_save_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_save_monitordev(int type, PMonitorDeviceList Devlist)
{
	return save_monitordev_storage(type, Devlist);
}

/*************************************************
  Function:		storage_save_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_free_monitordev(int type)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	switch (type)
	{
		case HOME_CAMERA:
			{
				ret = FSFileDelete(HOME_CAMERA_PATH);
			}
			break;

		case COMMUNITY_CAMERA:
			{
				ret = FSFileDelete(COMMUNITY_CAMERA_PATH);
			}
			break;

		default:
			break;
	}
	
	return ret;
}

/*************************************************
  Function:		storage_get_index
  Description: 	获取设备的编号
  Input:		无
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
uint8 storage_get_index(uint8 index)
{
	return g_index[index];
}

