/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_jrly.c
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--照片查看
  History:            
*********************************************************/
#include "storage_include.h"

#define PHOTOINFO_SIZE			(sizeof(PHOTO_INFO))

/*************************************************
  Function:		get_photo_from_storage
  Description: 	获得照片记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_photo_from_storage(PPHOTOLIST_INFO list)
{
	if (list)
	{
		list->Count = Fread_common(SNAP_MANAGE_PATH, list->PhotoInfo, PHOTOINFO_SIZE, MAX_PHOTO_NUM);
	}
}

/*************************************************
  Function:		save_photo_storage
  Description: 	按模块存储
  Input:		
  	1.list
  Output:		无
  Return:		无
  Others:
*************************************************/
static ECHO_STORAGE save_photo_storage(PPHOTOLIST_INFO list)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	if (list)
	{
		ret =  Fwrite_common(SNAP_MANAGE_PATH, list->PhotoInfo, PHOTOINFO_SIZE, list->Count);
	}

	return ret;
}

/*************************************************
  Function:		storage_free_photo_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_photo_memory(PPHOTOLIST_INFO *list)
{
	if (*list)
	{
		if ((*list)->PhotoInfo)
		{
			free((*list)->PhotoInfo);
			(*list)->PhotoInfo = NULL;
		}
		free((*list));	
		(*list) = NULL;
	}
}

/*************************************************
  Function:		storage_malloc_photo_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_photo_memory (PPHOTOLIST_INFO *list,uint32 MaxNum)
{
	*list = (PPHOTOLIST_INFO)malloc(sizeof(PHOTOLIST_INFO));	
	if (*list)
	{
		(*list)->Count = 0;
		(*list)->PhotoInfo = (PPHOTO_INFO)malloc(PHOTOINFO_SIZE * MaxNum);
	}
}		

/*************************************************
  Function:		get_photo_path
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_photo_path(char *filename, PZONE_DATE_TIME Time) 
{	
	if (Time && filename)
	{
		memset(filename, 0, sizeof(filename));
		sprintf(filename,"%s\/%04d%02d%02d%02d%02d%02d.JPG",SNAP_DIR_PATH,Time->year,Time->month,Time->day
				,Time->hour,Time->min,Time->sec);
		return filename;
	}
	else
	{
		return NULL;
	}
}

/*************************************************
  Function:		del_photo_file
  Description: 	删除当个记录的文件
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
static ECHO_STORAGE del_photo_file(PPHOTO_INFO PhotoInfo) 
{
	char filename[100];
	
	if (PhotoInfo)
	{
		get_photo_path(filename,&PhotoInfo->Time);
		log_printf("del_photo_file %s\n",filename);
		FSFileDelete(filename);
		return ECHO_STORAGE_OK;
	}
	else
	{
		return ECHO_STORAGE_ERR;
	}
}

/*************************************************
  Function:		storage_add_photo
  Description: 	获取照片列表
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_photo(PPHOTOLIST_INFO *photolist)
{
	storage_malloc_photo_memory(photolist, MAX_PHOTO_NUM);
	get_photo_from_storage(*photolist);
}

/*************************************************
  Function:		storage_add_photo
  Description: 	添加照片记录
  Input:		
  	1.Type		设备类型
  	2.DevNo		设备号
  	3.Time		时间
  Output:		无
  Return:		
  Others:
*************************************************/
ECHO_STORAGE storage_add_photo (DEVICE_TYPE_E Type, char* DevNo, ZONE_DATE_TIME Time)
{
	PPHOTOLIST_INFO photolist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	PHOTO_INFO buf[MAX_PHOTO_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;
	int nlen = 0;

	storage_malloc_photo_memory(&photolist, MAX_PHOTO_NUM);
	get_photo_from_storage(photolist);
	
	// 新记录加在头部
	if (photolist && photolist->PhotoInfo && DevNo)
	{
		nlen = strlen(DevNo);
		
		if (0 == photolist->Count)
		{
			photolist->PhotoInfo[0].Type = Type;
			memcpy(photolist->PhotoInfo[0].DevNo,DevNo,nlen);
			memcpy(&(photolist->PhotoInfo[0].Time), &Time, sizeof(ZONE_DATE_TIME));			
			photolist->Count = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].Type = Type;
			memcpy(buf[0].DevNo,DevNo,nlen);
			memcpy(&(buf[0].Time), &Time, sizeof(ZONE_DATE_TIME));	
			num = photolist->Count;
			if (num >= MAX_PHOTO_NUM)
			{
				num = MAX_PHOTO_NUM-1;
				del_photo_file(&(photolist->PhotoInfo[num]));
			}
			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), photolist->PhotoInfo, num*PHOTOINFO_SIZE);
			photolist->Count++;
			if (photolist->Count > MAX_PHOTO_NUM)
			{
				photolist->Count = MAX_PHOTO_NUM;
			}
			memset(photolist->PhotoInfo, 0, sizeof(photolist->PhotoInfo));
			memcpy(photolist->PhotoInfo, buf, PHOTOINFO_SIZE*photolist->Count);
		}
		ret = save_photo_storage(photolist);
	}

	storage_free_photo_memory(&photolist);
	return ret;
}

/*************************************************
  Function:		storage_del_photo
  Description: 	删除照片记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_photo (uint8 Index)
{
	uint8 i, j;
	PPHOTOLIST_INFO photolist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	PHOTO_INFO buf[MAX_PHOTO_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_photo_memory(&photolist, MAX_PHOTO_NUM);
	get_photo_from_storage(photolist);
	
	if (photolist && photolist->PhotoInfo)
	{
		num = photolist->Count;
		if (num > 0 && Index <= (num-1))
		{
			if (Index == (num-1))
			{
				del_photo_file(photolist->PhotoInfo+Index);
				memset((photolist->PhotoInfo+Index), 0, PHOTOINFO_SIZE);
			}
			else
			{	
				memset(buf, 0, PHOTOINFO_SIZE*MAX_PHOTO_NUM);
				memcpy(buf, photolist->PhotoInfo, PHOTOINFO_SIZE*num);
				memset(photolist->PhotoInfo, 0, PHOTOINFO_SIZE*MAX_PHOTO_NUM);
				j = 0;
				for(i = 0; i < num; i++)
				{
					if (Index != i)	
					{
						memcpy(photolist->PhotoInfo+j, buf+i, PHOTOINFO_SIZE);
						j++;
					}
					else
					{
						del_photo_file(buf+i);
					}
				}
			}
			photolist->Count--;
			ret = save_photo_storage(photolist);
			
		}
	}
	storage_free_photo_memory(&photolist);
	
	return ret;
}

/*************************************************
  Function:		storage_clear_photo
  Description: 	清空照片记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_photo (void)
{
	uint8 i;
	PPHOTOLIST_INFO photolist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	PHOTO_INFO buf[MAX_PHOTO_NUM];
	uint32 num = 0;
	//FLAG_STORAGE flag;

	storage_malloc_photo_memory(&photolist, MAX_PHOTO_NUM);
	get_photo_from_storage(photolist);
	
	if (photolist && photolist->PhotoInfo)
	{
		num = photolist->Count;
		if (num > 0)
		{
			memset(buf, 0, PHOTOINFO_SIZE*MAX_PHOTO_NUM);
			memcpy(buf, photolist->PhotoInfo, PHOTOINFO_SIZE*num);
			memset(photolist->PhotoInfo, 0, PHOTOINFO_SIZE*MAX_PHOTO_NUM);
			for(i = 0; i < num; i++)
			{
				del_photo_file(buf+i);
			}
			photolist->Count = 0;
			ret = save_photo_storage(photolist);
		}
		else
		{
			ret = ECHO_STORAGE_OK;
		}
	}
	storage_free_photo_memory(&photolist);
	//system("sync");
	//DelayMs_nops(200);
	
	return ret;
}

/*************************************************
  Function:		storage_set_photo_size
  Description:  获取照片大小
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 storage_set_photo_size(void)
{
	PPHOTOLIST_INFO photolist = NULL;
	uint32 i,ret = 0;
	//PHOTO_INFO buf[MAX_PHOTO_NUM];
	char filename[100];
	
	storage_malloc_photo_memory(&photolist, MAX_PHOTO_NUM);
	get_photo_from_storage(photolist);
	
	// 新记录加在头部
	if (photolist && photolist->PhotoInfo)
	{
		if (photolist->Count > 0)
		{
			for (i = 0; i < photolist->Count; i++)
			{
				ret += get_size(get_photo_path(filename,&(photolist->PhotoInfo[i].Time)));
			}
		}
	}
	storage_free_photo_memory(&photolist);
	return ret;
}

