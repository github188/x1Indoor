/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_jrly.c
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--家人留言
  History:            
*********************************************************/
#include "storage_include.h"

#define JRLYINFO_SIZE			(sizeof(JRLY_INFO))

/*************************************************
  Function:		get_jrlyrecord_from_storage
  Description: 	获得家人留言记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_jrlyrecord_from_storage(PJRLYLIST_INFO list)
{
	if (list)
	{
		list->Count = Fread_common(JRLY_MANAGE_PATH, list->JrlyInfo, JRLYINFO_SIZE, MAX_JRLY_NUM);
	}
}

/*************************************************
  Function:		save_jrly_storage
  Description: 	按模块存储
  Input:		
  	1.list
  Output:		无
  Return:		无
  Others:
*************************************************/
static ECHO_STORAGE save_jrly_storage(PJRLYLIST_INFO list)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	if (list)
	{
		ret =  Fwrite_common(JRLY_MANAGE_PATH, list->JrlyInfo, JRLYINFO_SIZE, list->Count);
	}

	return ret;
}

/*************************************************
  Function:		storage_free_jrly_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_jrly_memory(PJRLYLIST_INFO *list)
{
	if (*list)
	{
		if ((*list)->JrlyInfo)
		{
			free((*list)->JrlyInfo);
			(*list)->JrlyInfo = NULL;
		}
		free((*list));	
		(*list) = NULL;
	}
}

/*************************************************
  Function:		storage_malloc_jrly_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_jrly_memory (PJRLYLIST_INFO *list,uint32 MaxNum)
{
	*list = (PJRLYLIST_INFO)malloc(sizeof(JRLYLIST_INFO));	
	if (*list)
	{
		(*list)->Count = 0;
		(*list)->JrlyInfo = (PJRLY_INFO)malloc(JRLYINFO_SIZE * MaxNum);
	}
}		

/*************************************************
  Function:		get_jrlyrecord_path
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_jrlyrecord_path(char *filename, PZONE_DATE_TIME Time) 
{	
	if (Time && filename)
	{
		memset(filename, 0, sizeof(filename));
		sprintf(filename,"%s/%04d%02d%02d%02d%02d%02d.wav",JRLY_DIR_PATH,Time->year,Time->month,Time->day
				,Time->hour,Time->min,Time->sec);
		return filename;
	}
	else
	{
		return NULL;
	}
}

/*************************************************
  Function:		get_jrlyrecord_file
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_jrlyrecord_file(char *filename, PZONE_DATE_TIME Time) 
{	
	if (Time && filename)
	{
		memset(filename, 0, sizeof(filename));
		sprintf(filename,"%s/%04d%02d%02d%02d%02d%02d",JRLY_DIR_PATH,Time->year,Time->month,Time->day
				,Time->hour,Time->min,Time->sec);
		return filename;
	}
	else
	{
		return NULL;
	}
}

/*************************************************
  Function:		del_jrlyrecord_file
  Description: 	删除当个记录的文件
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static ECHO_STORAGE del_jrlyrecord_file(PJRLY_INFO info) 
{
	char filename[100];
	
	if (info)
	{
		FSFileDelete(get_jrlyrecord_path(filename,&info->Time));
		return ECHO_STORAGE_OK;
	}
	else
	{
		return ECHO_STORAGE_ERR;
	}
}

/*************************************************
  Function:		storage_get_jrlyrecord
  Description: 	获得家人留言记录
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_jrlyrecord(PJRLYLIST_INFO *jrlylist)
{
	storage_malloc_jrly_memory(jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(*jrlylist);
}

/*************************************************
  Function:		storage_add_jrlyrecord
  Description: 	添加家人留言记录
  Input:		
  	3.Time		时间
  Output:		无
  Return:		
  Others:
*************************************************/
ECHO_STORAGE storage_add_jrlyrecord (ZONE_DATE_TIME Time)
{
	PJRLYLIST_INFO jrlylist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	JRLY_INFO buf[MAX_JRLY_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	
	// 新记录加在头部
	if (jrlylist && jrlylist->JrlyInfo)
	{
		if (0 == jrlylist->Count)
		{
			jrlylist->JrlyInfo[0].ReadFlag = TRUE;
			memcpy(&(jrlylist->JrlyInfo[0].Time), &Time, sizeof(ZONE_DATE_TIME));			
			jrlylist->Count = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].ReadFlag= TRUE;
			memcpy(&(buf[0].Time), &Time, sizeof(ZONE_DATE_TIME));	
			num = jrlylist->Count;
			if (num >= MAX_JRLY_NUM)
			{
				num = MAX_JRLY_NUM-1;
				del_jrlyrecord_file(&(jrlylist->JrlyInfo[num]));
			}
			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), jrlylist->JrlyInfo, num*JRLYINFO_SIZE);
			jrlylist->Count++;
			if (jrlylist->Count > MAX_JRLY_NUM)
			{
				jrlylist->Count = MAX_JRLY_NUM;
			}
			memset(jrlylist->JrlyInfo, 0, sizeof(jrlylist->JrlyInfo));
			memcpy(jrlylist->JrlyInfo, buf, JRLYINFO_SIZE*jrlylist->Count);
		}
		ret = save_jrly_storage(jrlylist);
	}

	storage_free_jrly_memory(&jrlylist);
	return ret;
}

/*************************************************
  Function:		storage_del_jrlyrecord
  Description: 	删除家人留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_jrlyrecord (uint8 Index)
{
	uint8 i, j;
	PJRLYLIST_INFO jrlylist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	JRLY_INFO buf[MAX_JRLY_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	
	if (jrlylist && jrlylist->JrlyInfo)
	{
		num = jrlylist->Count;
		if (num > 0 && Index <= (num-1))
		{
			if (Index == (num-1))
			{
				del_jrlyrecord_file(jrlylist->JrlyInfo+Index);
				memset((jrlylist->JrlyInfo+Index), 0, JRLYINFO_SIZE);
			}
			else
			{	
				memset(buf, 0, JRLYINFO_SIZE*MAX_JRLY_NUM);
				memcpy(buf, jrlylist->JrlyInfo, JRLYINFO_SIZE*num);
				memset(jrlylist->JrlyInfo, 0, JRLYINFO_SIZE*MAX_JRLY_NUM);
				j = 0;
				for(i = 0; i < num; i++)
				{
					if (Index != i)	
					{
						memcpy(jrlylist->JrlyInfo+j, buf+i, JRLYINFO_SIZE);
						j++;
					}
					else
					{
						del_jrlyrecord_file(buf+i);
					}
				}
			}
			jrlylist->Count--;
			ret = save_jrly_storage(jrlylist);
		}
	}
	storage_free_jrly_memory(&jrlylist);
	return ret;
}

/*************************************************
  Function:		storage_clear_jrlyrecord
  Description: 	清空家人留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_jrlyrecord (void)
{
	uint8 i;
	PJRLYLIST_INFO jrlylist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	JRLY_INFO buf[MAX_JRLY_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	
	if (jrlylist && jrlylist->JrlyInfo)
	{
		num = jrlylist->Count;
		if (num > 0)
		{
			memset(buf, 0, JRLYINFO_SIZE*MAX_JRLY_NUM);
			memcpy(buf, jrlylist->JrlyInfo, JRLYINFO_SIZE*num);
			memset(jrlylist->JrlyInfo, 0, JRLYINFO_SIZE*MAX_JRLY_NUM);
			for(i = 0; i < num; i++)
			{
				del_jrlyrecord_file(buf+i);
			}
			jrlylist->Count = 0;
			ret = save_jrly_storage(jrlylist);
		}
		else
		{
			ret = ECHO_STORAGE_OK;
		}
	}
	storage_free_jrly_memory(&jrlylist);
	return ret;
}


/*************************************************
  Function:		storage_set_jrlyrecord_flag
  Description: 	设置该记录是否已读
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_set_jrlyrecord_flag(uint32 Index, uint8 State)
{
	uint8 i;
	PJRLYLIST_INFO jrlylist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	//JRLY_INFO buf[MAX_JRLY_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	
	if (jrlylist && jrlylist->JrlyInfo)
	{
		num = jrlylist->Count;
		if (num > 0)
		{
			for(i = 0; i < num; i++)
			{
				if (Index == i)
				{
					jrlylist->JrlyInfo[i].ReadFlag = State;
				}
			}
			ret = save_jrly_storage(jrlylist);
		}
	}
	storage_free_jrly_memory(&jrlylist);
	return ret;
}

/*************************************************
  Function:		storage_set_jrlyrecord_flag
  Description:  获取是否有未听
  Input:		无
  Output:		无
  Return:		TRUE --  有
  				FALSE -- 没有
  Others:		
*************************************************/
uint8 storage_get_jrlyrecord_flag(void)
{
	PJRLYLIST_INFO jrlylist = NULL;
	uint8 ret = FALSE;
	uint8 i;
	
	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	if (jrlylist && jrlylist->Count > 0)
	{
		for (i = 0; i < jrlylist->Count; i++)
		{
			if (jrlylist->JrlyInfo[i].ReadFlag == TRUE)
			{
				ret = TRUE;
				break;
			}
		}
	}
	storage_free_jrly_memory(&jrlylist);
	return ret;
}

/*************************************************
  Function:		storage_set_jrlyrecord_size
  Description:  获取家人留言大小
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 storage_set_jrlyrecord_size(void)
{
	PJRLYLIST_INFO jrlylist = NULL;
	uint32 i,ret = 0;
	//JRLY_INFO buf[MAX_JRLY_NUM];
	char filename[100];
	
	storage_malloc_jrly_memory(&jrlylist, MAX_JRLY_NUM);
	get_jrlyrecord_from_storage(jrlylist);
	
	// 新记录加在头部
	if (jrlylist && jrlylist->JrlyInfo)
	{
		if (jrlylist->Count > 0)
		{
			for (i = 0; i < jrlylist->Count; i++)
			{
				ret += get_size(get_jrlyrecord_path(filename,&(jrlylist->JrlyInfo[i].Time)));
			}
		}
	}
	storage_free_jrly_memory(&jrlylist);
	return ret;
}

