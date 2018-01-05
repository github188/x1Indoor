/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	storage_netdoor.h
  Author:    	zxc
  Version:   	2.0
  Date: 		2014-11-06
  Description:	蓝牙门前存储
  
  History:                    
    1. Date:		 
       Author:		 
       Modification: 
    2. ...
*********************************************************/
#ifndef _STORAGE_NETDOOR_H_
#define _STORAGE_NETDOOR_H_

#include "storage_include.h"

#ifdef _ADD_BLUETOOTH_DOOR_

#define MAX_BLEDOOR_PIC_NUM				10					// 门前抓拍图片最多10条

#if 0
//抓拍类型
typedef enum
{
	SNAP_BODY_INDUCTION		= 1,		// 人体感应拍照
	SNAP_OPERATE_ERR3		= 2,		// 三次操作失败拍照
	SNAP_TAMPER_ALARM		= 3,		// 防拆报警拍照
	SNAP_VISITOR			= 4,		// 访客呼叫拍照
}SNAP_TYPE_E;
#endif

//抓拍录像类型
typedef enum
{
	ACTION_BODY_INDUCTION	= 1,		// 人体感应
	ACTION_OPERATE_ERR3		= 2,		// 五次操作失败
}SNAP_RECORD_TYPE_E;


// 拍照上传图片结构体
typedef struct
{
	uint8 is_unread;								// 0 - 已读，1 - 未读标志	
	uint32 ID;
	ZONE_DATE_TIME ourtime;								// 时间
	uint8 time[24];									// 时间
	SNAP_RECORD_TYPE_E event_type;					// 事件类型
}BLEDOOR_PIC_DATA, * PBLEDOOR_PIC_DATA;

// 拍照上传图片列表
typedef struct
{
	uint8 ncount;
	BLEDOOR_PIC_DATA pinfo_data[MAX_BLEDOOR_PIC_NUM];
}BLEDOOR_PIC_LIST, * PBLEDOOR_PIC_LIST;


/*************************************************
  Function:    		storage_set_bledoor_pic_readed
  Description:		设置已读，或者未读标志
  Input:
  	1.index			图片索引
  Output:			
  Return:			设置成功TRUE,失败FALSE
  Others:			
*************************************************/
ECHO_STORAGE storage_set_bledoor_pic_readed(uint8 index, uint8 door_index);

/*************************************************
  Function:			storage_read_bledoor_pic_data
  Description: 		获取图片数据
  Input:
  	1.pmsg_data		结构体
  	2.index			索引
  Output:		
  Return:			图片文件名
  Others:
*************************************************/ 
char * storage_read_bledoor_pic_data(BLEDOOR_PIC_DATA *pmsg_data, uint8 index, uint8 doorindex);

/*************************************************
  Function:			storage_read_bledoor_pic_list
  Description: 		获取图片列表
  Input:			无
  Output:			无
  Return:			BLEDOOR_PIC_LIST	结构指针
  Others:			由调用者释放
*************************************************/ 
BLEDOOR_PIC_LIST * storage_read_bledoor_pic_list(uint8 doorindex);

/*************************************************
  Function:			storage_write_bledoor_pic
  Description: 		写入门前拍照上传图片
  Input:		
  Output:		   
  Return:		    
  Others:
*************************************************/
ECHO_STORAGE storage_write_bledoor_pic(uint32 ID, uint8 * data, int size, char doorindex);

/*************************************************
  Function:    		storage_delete_bledoor_pic
  Description:		删除单张图片
  Input:
  	1.index			图片索引
  Output:			无
  Return:			删除成功TRUE, 失败FALSE
  Others:			
*************************************************/
ECHO_STORAGE storage_delete_bledoor_pic(uint8 index, uint8 doorindex);

/*************************************************
  Function:			storage_clear_bledoor_pic
  Description: 		清空图片
  Input:			无
  Output:		 	无
  Return:			TRUE/FALSE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_bledoor_pic(uint8 doorindex);

/*************************************************
  Function:			storage_get_msg_state
  Description: 		获取图片是否有未读标示
  Input:			无
  Output:			无
  Return:			TRUE:有未读 FALSE:无未读
  Others:
*************************************************/
uint8 storage_get_bledoor_pic_state(uint8 doorindex);

/*************************************************
  Function:			storage_get_bledoor_pic_allsize
  Description: 		获取蓝牙门前抓拍上传图片总大小
  Input:			无
  Output:		 	无
  Return:			总容量
  Others:
*************************************************/
int storage_get_bledoor_pic_allsize(uint8 doorindex);
#endif

#endif

