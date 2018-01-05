/*********************************************************
  Copyright (C), 2009-2012
  File name:	Storage_jrly.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--家人留言
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_JRLY_H__
#define __STORAGE_JRLY_H__

#include "storage_include.h"
//#include "storage_types.h"

#define MAX_JRLY_NUM  		5						// 最大记录

typedef struct
{
	uint8 ReadFlag;									// 已读未读标志 1 - 未读 0 - 已读 
	ZONE_DATE_TIME Time;		    				// 时间	           
}JRLY_INFO, * PJRLY_INFO;							// 家人留言结构

// 通话记录列表的结构
typedef struct
{
	uint8 Count;									// 个数
	PJRLY_INFO JrlyInfo;				    		// 家人留言结构
}JRLYLIST_INFO, * PJRLYLIST_INFO;

/*************************************************
  Function:		get_jrlyrecord_file
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_jrlyrecord_file(char *filename, PZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_free_jrly_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_jrly_memory(PJRLYLIST_INFO *list);

/*************************************************
  Function:		storage_malloc_jrly_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_jrly_memory (PJRLYLIST_INFO *list,uint32 MaxNum);
		
/*************************************************
  Function:		get_jrlyrecord_path
  Description: 	得到当前这个文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_jrlyrecord_path(char *filename, PZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_get_jrlyrecord
  Description: 	获得家人留言记录
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_jrlyrecord(PJRLYLIST_INFO *jrlylist);

/*************************************************
  Function:		storage_add_jrlyrecord
  Description: 	添加家人留言记录
  Input:		
  	3.Time		时间
  Output:		无
  Return:		
  Others:
*************************************************/
ECHO_STORAGE storage_add_jrlyrecord (ZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_del_jrlyrecord
  Description: 	删除家人留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_jrlyrecord (uint8 Index);

/*************************************************
  Function:		storage_clear_jrlyrecord
  Description: 	清空家人留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_jrlyrecord (void);

/*************************************************
  Function:		storage_set_jrlyrecord_flag
  Description: 	设置该记录是否已读
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_set_jrlyrecord_flag(uint32 Index, uint8 State);

/*************************************************
  Function:		storage_set_jrlyrecord_flag
  Description:  获取是否有未听
  Input:		无
  Output:		无
  Return:		TRUE --  有
  				FALSE -- 没有
  Others:		
*************************************************/
uint8 storage_get_jrlyrecord_flag(void);

/*************************************************
  Function:		storage_set_jrlyrecord_size
  Description:  获取家人留言大小
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 storage_set_jrlyrecord_size(void);

#endif 


