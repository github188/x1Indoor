/*********************************************************
  Copyright (C), 2009-2012
  File name:	Storage_Lyly.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--留影留言
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_LYLY_H__
#define __STORAGE_LYLY_H__

#include "storage_include.h"
//#include "storage_types.h"

#define MAX_LYLY_NUM  		5						// 最大记录

typedef struct
{
	uint8 UnRead;									// 已读未读标志 1 - 未读 0 - 已读 
	char DevNo[20];									// 设备号
	DEVICE_TYPE_E DevType;							// 设备类型
	LYLY_TYPE LyType;								// 留言类型
	ZONE_DATE_TIME Time;		    				// 时间	           
}LYLY_INFO, * PLYLY_INFO;							// 留影留言结构

// 通话记录列表的结构
typedef struct
{
	uint8 Count;									// 个数
	PLYLY_INFO LylyInfo;				    		// 留影留言结构
}LYLYLIST_INFO, * PLYLYLIST_INFO;

/*************************************************
  Function:		storage_free_lyly_memory
  Description: 	释放存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_free_lyly_memory(PLYLYLIST_INFO *list);

/*************************************************
  Function:		storage_malloc_lyly_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_lyly_memory (PLYLYLIST_INFO *list,uint32 MaxNum);

/*************************************************
  Function:		get_lylyrecord_path
  Description: 	得到当前这个文件名
  Input:
  	1.filename
  	2.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_lylyrecord_path(char *filename, PZONE_DATE_TIME Time) ;

/*************************************************
  Function:		get_lylypic_path
  Description: 	得到当前这个图片文件名
  Input:		
  	1.info
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
char* get_lylypic_path(char *filename, PZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_get_lylyrecord
  Description: 	获得留影留言记录
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_lylyrecord(PLYLYLIST_INFO *lylylist);

/*************************************************
  Function:		storage_add_lylyrecord
  Description: 	添加留影留言记录
  Input:
  	1.LyType 	留言类型
  	2.DevType	设备类型
  	3.DevIndex	设备索引
  	4.Time		时间
  Output:		无
  Return:		
  Others:
*************************************************/
ECHO_STORAGE storage_add_lylyrecord (LYLY_TYPE LyType, DEVICE_TYPE_E DevType, char* DevIndex, ZONE_DATE_TIME Time);

/*************************************************
  Function:		storage_del_lylyrecord
  Description: 	删除留影留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_lylyrecord (uint8 Index);

/*************************************************
  Function:		storage_clear_lylyrecord
  Description: 	清空留影留言记录
  Input:		
  	1.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_lylyrecord (void);

/*************************************************
  Function:		storage_set_lylyrecord_flag
  Description: 	设置该记录是否已读
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_set_lylyrecord_flag(uint32 Index, uint8 State);

/*************************************************
  Function:		storage_set_lylyrecord_flag
  Description:  获取是否有未听
  Input:		无
  Output:		无
  Return:		TRUE --  有
  				FALSE -- 没有
  Others:		
*************************************************/
uint8 storage_get_lylyrecord_flag(void);

/*************************************************
  Function:		storage_set_lylyrecord_size
  Description:  获取留影留言大小
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
uint32 storage_set_lylyrecord_size(void);

#endif 



