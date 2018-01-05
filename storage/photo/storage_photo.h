/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_photo.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--照片查看
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_PHOTO_H__
#define __STORAGE_PHOTO_H__

#include "storage_include.h"
//#include "storage_types.h"

#define MAX_PHOTO_NUM  			10					// 最大自动抓拍记录

typedef struct
{
	char DevNo[20];									// 设备号
	DEVICE_TYPE_E Type;								// 设备类型
	ZONE_DATE_TIME Time;		    				// 时间	           
}PHOTO_INFO, * PPHOTO_INFO;							// 照片结构

// 通话记录列表的结构
typedef struct
{
	uint8 Count;									// 个数
	PPHOTO_INFO PhotoInfo;				    		// 照片结构
}PHOTOLIST_INFO, * PPHOTOLIST_INFO;

/*************************************************
  Function:		storage_free_photo_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_photo_memory(PPHOTOLIST_INFO *list);

/*************************************************
  Function:		storage_malloc_photo_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_photo_memory (PPHOTOLIST_INFO *list,uint32 MaxNum);

/*************************************************
  Function:		get_photo_path
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_photo_path(char *filename, PZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_add_photo
  Description: 	获取照片列表
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_photo(PPHOTOLIST_INFO *photolist);

/*************************************************
  Function:		storage_add_photo
  Description: 	添加照片记录
  Input:		
  	1.Type		设备类型
  	2.Index		设备号
  	3.Time		时间
  Output:		无
  Return:		
  Others:
*************************************************/
ECHO_STORAGE storage_add_photo (DEVICE_TYPE_E Type, char* DevNo, ZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_del_photo
  Description: 	删除照片记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_photo (uint8 Index);

/*************************************************
  Function:		storage_clear_photo
  Description: 	清空照片记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_photo (void);

/*************************************************
  Function:		storage_set_photo_size
  Description:  获取照片大小
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 storage_set_photo_size(void);
#endif 

