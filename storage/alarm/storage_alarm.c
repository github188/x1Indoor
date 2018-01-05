/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	storage_alarm.h
  Author:   	罗发禄
  Version:   	2.0
  Date: 		2010-4-23
  Description:  安防存储
  
  History:                   
    1. Date:	2011-08-03		 
       Author:	luofl		
       Modification:
       			增加安防联动功能
    2. ...
*********************************************************/
#include "storage_include.h"

extern PAF_FLASH_DATA gpAfParam;

// 以下为报警记录与操作记录的存储接口
static PAF_CZ_LIST pAfCzList = NULL;
static PAF_BJ_LIST pAfBjList = NULL;

/*************************************************
  Function:		free_af_memory
  Description: 	释放安防存储内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void free_af_memory(void)
{
	if (pAfCzList)
	{
		free(pAfCzList);
		pAfCzList = NULL;
	}

	if (pAfBjList)
	{
		free(pAfBjList);
		pAfBjList = NULL;
	}
}

/*************************************************
  Function:		malloc_af_memory
  Description: 	申请安防存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
static void malloc_af_memory (void)
{
	pAfCzList = (PAF_CZ_LIST)malloc(sizeof(AF_CZ_LIST));
	pAfBjList = (PAF_BJ_LIST)malloc(sizeof(AF_BJ_LIST));

	if (pAfCzList)
	{
		pAfCzList->nCount = 0;
		pAfCzList->pAfCzRec = (PAF_CZ_REC)malloc(sizeof(AF_CZ_REC) * AF_REC_MAX);
	}

	if (pAfBjList)
	{
		pAfBjList->nCount = 0;
		pAfBjList->pAfBjRec = (PAF_BJ_REC)malloc(sizeof(AF_BJ_REC) * AF_REC_MAX);
	}
}

/*************************************************
  Function:		get_af_list_from_flash
  Description: 	从FLASH中获取安防事件记录列表
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_af_list_from_flash(void)
{
	if (pAfCzList)
	{
		pAfCzList->nCount = Fread_common(AF_CZ_REC_PATH, pAfCzList->pAfCzRec, sizeof(AF_CZ_REC), AF_REC_MAX);
	}
	
	if (pAfBjList)
	{
		pAfBjList->nCount = Fread_common(AF_BJ_REC_PATH, pAfBjList->pAfBjRec, sizeof(AF_BJ_REC), AF_REC_MAX);
	}
}

/*************************************************
  Function:		storage_get_afcz_record
  Description: 	获取安防操作记录列表
  Input:		无
  Output:		无
  Return:		无
  Others:		
  	1.该指针不要释放
*************************************************/
PAF_CZ_LIST storage_get_afcz_record (void)
{
	return pAfCzList;
}

/*************************************************
  Function:		storage_get_afbj_record
  Description: 	获取安防报警记录列表
  Input:		无
  Output:		无
  Return:		无
  Others:		
  	1.该指针不要释放
*************************************************/
PAF_BJ_LIST storage_get_afbj_record (void)
{
	return pAfBjList;
}

/*************************************************
  Function:		storage_add_afcz_record
  Description: 	增加安防操作记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_add_afcz_record(uint8 type, EXECUTOR_E executor)
{
	uint8 			count;
	ECHO_STORAGE	ret = ECHO_STORAGE_ERR;
	AF_CZ_REC		buf[AF_REC_MAX];
	ZONE_DATE_TIME	time;

	get_timer(&time);
	if (pAfCzList && pAfCzList->pAfCzRec)
	{
		if (0 == pAfCzList->nCount)
		{
			pAfCzList->pAfCzRec[0].type = type;
			pAfCzList->pAfCzRec[0].executor = executor;
			memcpy(&(pAfCzList->pAfCzRec[0].time), &time, sizeof(ZONE_DATE_TIME));
			pAfCzList->nCount = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].type = type;
			buf[0].executor = executor;
			memcpy(&(buf[0].time), &time, sizeof(ZONE_DATE_TIME));
			count = pAfCzList->nCount;
			if (count >= AF_REC_MAX)
			{
				count = AF_REC_MAX - 1;				// 超过最大记录数覆盖最早的一条
			}

			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), pAfCzList->pAfCzRec, count * sizeof(AF_CZ_REC));
			pAfCzList->nCount++;
			if (pAfCzList->nCount > AF_REC_MAX)
			{
				pAfCzList->nCount = AF_REC_MAX;
			}
			
			memset(pAfCzList->pAfCzRec, 0, AF_REC_MAX * sizeof(AF_CZ_REC));
			memcpy(pAfCzList->pAfCzRec, buf, pAfCzList->nCount * sizeof(AF_CZ_REC));
		}
		
		//storage_set_save_flag(FLAG_AFCZ, TRUE);
		Fwrite_common(AF_CZ_REC_PATH, pAfCzList->pAfCzRec, sizeof(AF_CZ_REC), pAfCzList->nCount);
		ret = ECHO_STORAGE_OK;
	}
	
	return ret;
}

/*************************************************
  Function:		storage_add_afbj_record
  Description: 	增加安防报警记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
ECHO_STORAGE storage_add_afbj_record(uint8 areaNum, uint8 type)
{
	uint8 			count;
	ECHO_STORAGE	ret = ECHO_STORAGE_ERR;
	AF_BJ_REC		buf[AF_REC_MAX];
	ZONE_DATE_TIME	time;

	get_timer(&time);
	if (pAfBjList && pAfBjList->pAfBjRec)
	{
		if (0 == pAfBjList->nCount)
		{
			pAfBjList->pAfBjRec[0].areaNum = areaNum;
			pAfBjList->pAfBjRec[0].type = type;
			pAfBjList->pAfBjRec[0].bReaded = 1;
			memcpy(&(pAfBjList->pAfBjRec[0].time), &time, sizeof(ZONE_DATE_TIME));
			pAfBjList->nCount = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].areaNum = areaNum;
			buf[0].type = type;
			buf[0].bReaded = 1;
			memcpy(&(buf[0].time), &time, sizeof(ZONE_DATE_TIME));
			count = pAfBjList->nCount;
			if (count >= AF_REC_MAX)
			{
				count = AF_REC_MAX - 1;				// 超过最大记录数覆盖最早的一条
			}

			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), pAfBjList->pAfBjRec, count * sizeof(AF_BJ_REC));
			pAfBjList->nCount++;
			if (pAfBjList->nCount > AF_REC_MAX)
			{
				pAfBjList->nCount = AF_REC_MAX;
			}
			
			memset(pAfBjList->pAfBjRec, 0, AF_REC_MAX * sizeof(AF_BJ_REC));
			memcpy(pAfBjList->pAfBjRec, buf, pAfBjList->nCount * sizeof(AF_BJ_REC));
		}
		
		//storage_set_save_flag(FLAG_AFBJ, TRUE);
		Fwrite_common(AF_BJ_REC_PATH, pAfBjList->pAfBjRec, sizeof(AF_BJ_REC), pAfBjList->nCount);
		ret = ECHO_STORAGE_OK;
	}
	
	return ret;
}

/*************************************************
  Function:		storage_clear_afcz_record
  Description:  清空安防操作记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_afcz_record(void)
{
	if (pAfCzList)
	{
		pAfCzList->nCount = 0;
		memset(pAfCzList->pAfCzRec, 0, sizeof(AF_CZ_REC) * AF_REC_MAX);
		//storage_set_save_flag(FLAG_AFCZ, TRUE);
		Fwrite_common(AF_CZ_REC_PATH, pAfCzList->pAfCzRec, sizeof(AF_CZ_REC), pAfCzList->nCount);
	}
}

/*************************************************
  Function:		storage_clear_afbj_record
  Description:  清空安防报警记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_afbj_record(void)
{
	if (pAfBjList)
	{
		pAfBjList->nCount = 0;
		memset(pAfBjList->pAfBjRec, 0, sizeof(AF_BJ_REC) * AF_REC_MAX);
		//storage_set_save_flag(FLAG_AFBJ, TRUE);
		Fwrite_common(AF_BJ_REC_PATH, pAfBjList->pAfBjRec, sizeof(AF_BJ_REC), pAfBjList->nCount);
	}
}

/*************************************************
  Function:		storage_get_afbj_unread_record
  Description:  清空安防报警记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
PALARM_TOUCH_INFO_LIST storage_get_afbj_unread_record(void)
{
	PAF_BJ_LIST tmp = NULL;
	PALARM_TOUCH_INFO_LIST UnreadList = NULL;
	uint8 i;
	
	tmp = (PAF_BJ_LIST)storage_get_afbj_record();

	UnreadList = (PALARM_TOUCH_INFO_LIST)malloc(sizeof(ALARM_TOUCH_INFO));

	if (UnreadList == NULL)
	{
		return NULL;
	}
	if (UnreadList)
	{
		UnreadList->nCount = 0;
		UnreadList->pAlarmRec = (PALARM_TOUCH_INFO)malloc(sizeof(ALARM_TOUCH_INFO) * AF_REC_MAX);
	}

	if (tmp)
	{
		for (i = 0; i < tmp->nCount; i++)
		{
			if (tmp->pAfBjRec[i].bReaded == TRUE)
			{
				UnreadList->pAlarmRec[UnreadList->nCount].bReaded = 0;
				UnreadList->pAlarmRec[UnreadList->nCount].bClean = 0;
				UnreadList->pAlarmRec[UnreadList->nCount].TouchNum = tmp->pAfBjRec[i].areaNum;
				UnreadList->pAlarmRec[UnreadList->nCount].type = 0;//tmp->pAfBjRec[i].type;
				UnreadList->pAlarmRec[UnreadList->nCount].id = i;
				sprintf((char *)UnreadList->pAlarmRec[UnreadList->nCount].time, "%04d-%02d-%02d %02d:%02d:%02d", 
					tmp->pAfBjRec[i].time.year, tmp->pAfBjRec[i].time.month, tmp->pAfBjRec[i].time.day, 
					tmp->pAfBjRec[i].time.hour, tmp->pAfBjRec[i].time.min, tmp->pAfBjRec[i].time.sec);
				UnreadList->nCount++;
			}
		}
	}
	
	return UnreadList;
}

/*************************************************
  Function:		storage_af_timer_save
  Description: 	定时存储安防模块数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_af_timer_save(void)
{
	// 安防操作记录
	if (storage_get_save_flag(FLAG_AFCZ) == TRUE)
	{
		storage_set_save_flag(FLAG_AFCZ, FALSE);
		Fwrite_common(AF_CZ_REC_PATH, pAfCzList->pAfCzRec, sizeof(AF_CZ_REC), pAfCzList->nCount);
	}

	// 安防报警记录
	if (storage_get_save_flag(FLAG_AFBJ) == TRUE)
	{
		storage_set_save_flag(FLAG_AFBJ, FALSE);
		Fwrite_common(AF_BJ_REC_PATH, pAfBjList->pAfBjRec, sizeof(AF_BJ_REC), pAfBjList->nCount);
	}
}

/*************************************************
  Function:		storage_af_init
  Description: 	安防模块存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_af_init(void)
{
	free_af_memory();
	malloc_af_memory();
	get_af_list_from_flash();
}

// 以下为安防提示音的接口
/*************************************************
  Function:    		storage_get_yj_wc_path
  Description: 		获取"外出请关好门,再见"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yj_wc_path(void)
{
	if (ENGLISH == storage_get_language())
	{
		return AF_YJ_WC_PATH_E;
	}
	
	return AF_YJ_WC_PATH;
}

/*************************************************
  Function:    		storage_get_yjq_path
  Description: 		获取"请注意有警情"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yjq_path(void)
{
	if (ENGLISH == storage_get_language())
	{
		return AF_YJQ_PATH_E;
	}
	
	return AF_YJQ_PATH;
}

/*************************************************
  Function:    		storage_get_fqcf_path
  Description: 		获取"防区触发不能布防"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_fqcf_path(void)
{
	if (ENGLISH == storage_get_language())
	{
		return AF_FQCF_PATH_E;
	}
	
	return AF_FQCF_PATH;
}

/*************************************************
  Function:    		storage_get_jjyjc_path
  Description: 		获取"你好！警戒已解除"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_jjyjc_path(void)
{
	if (ENGLISH == storage_get_language())
	{
		return AF_JJYJC_PATH_E;
	}
	
	return AF_JJYJC_PATH;
}

/*************************************************
  Function:    		storage_get_wanan_path
  Description: 		获取"晚安"的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_wanan_path(void)
{
	if (ENGLISH == storage_get_language())
	{
		return AF_WANAN_PATH_E;
	}
	
	return AF_WANAN_PATH;
}

/*************************************************
  Function:    		storage_get_zone_path
  Description: 		获取防区报警的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_zone_path(uint8 zone)
{
	// 2011-12-05 modi by luofl 增加英文提示音
	switch (zone)
	{
		case 1:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE1_PATH_E;
			}
			#endif
			return AF_ZONE1_PATH;
			break;
			
		case 2:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE2_PATH_E;
			}
			#endif
			return AF_ZONE2_PATH;
			break;

		case 3:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE3_PATH_E;
			}
			#endif
			return AF_ZONE3_PATH;
			break;

		case 4:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE4_PATH_E;
			}
			#endif
			return AF_ZONE4_PATH;
			break;

		case 5:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE5_PATH_E;
			}
			#endif
			return AF_ZONE5_PATH;
			break;

		case 6:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE6_PATH_E;
			}
			#endif
			return AF_ZONE6_PATH;
			break;

		case 7:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE7_PATH_E;
			}
			#endif
			return AF_ZONE7_PATH;
			break;

		case 8:
			#if 1
			if (ENGLISH == storage_get_language())
			{
				return AF_ZONE8_PATH_E;
			}
			#endif
			return AF_ZONE8_PATH;
			break;
			
		default:
			break;
	}
	
	return NULL;
}

#if 0
/*************************************************
  Function:    		storage_get_yj_path
  Description: 		获取预警的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_yj_path(void)
{
	return AF_YJ_PATH;
}
#endif

/*************************************************
  Function:    		storage_get_bj_path
  Description: 		获取报警的声音路径
  Input:			无
  Output:    		无
  Return:			路径
  Others:    		       
*************************************************/
char * storage_get_bj_path(void)
{
	return AF_BJ_PATH;
}

// 以下为安防模块存储的接口
/*************************************************
  Function:    		storage_get_gsm_num
  Description: 		获得短信号码
  Input:
  Output:
   	1.num1 			电话号码1
   	2.num2			电话号码2
  Return:			
  Others:           
*************************************************/
void storage_get_gsm_num(uint8 *num1, uint8 *num2)
{
	if (NULL == num1 || NULL == num2)
	{
		return;
	}
	memcpy(num1, gpAfParam->short_msg[0], TELNUM_LEN);
	memcpy(num2, gpAfParam->short_msg[1], TELNUM_LEN);
}

/*************************************************
  Function:    		storage_set_gsm_num
  Description: 		设置短信号码
  Input:
  	1.num1         	1:短信号码1 
  	2.num2			2:短信号码2
  Output:    		
  Return:			TRUE - 成功, FALSE - 失败
  Others:           
*************************************************/
uint32 storage_set_gsm_num(uint8 *num1, uint8 *num2)
{
	if (NULL == num1 || NULL == num2)
	{
		return FALSE;
	}
	memcpy(gpAfParam->short_msg[0], num1, TELNUM_LEN);
	memcpy(gpAfParam->short_msg[1], num2, TELNUM_LEN);
	SaveRegInfo();
	//log_printf("gpAfParam->short_msg[0]: %s, gpAfParam->short_msg[1]; %s \n", gpAfParam->short_msg[0],gpAfParam->short_msg[1]);
	return TRUE;
}

/*************************************************
  Function:    		storage_get_alarm_num
  Description: 		获得报警号码
  Input:
  Output:
   	1.num1 			电话号码1
   	2.num2			电话号码2
  Return:			
  Others:           
*************************************************/
void storage_get_alarm_num(uint8 * num1, uint8 * num2)
{
	if (NULL == num1 || NULL == num2)
	{
		return;
	}
	
	memcpy(num1, gpAfParam->alarm_num[0], TELNUM_LEN);
	memcpy(num2, gpAfParam->alarm_num[1], TELNUM_LEN);
	log_printf("gpAfParam->alarm_num[0]: %s, gpAfParam->alarm_num[1]; %s \n", gpAfParam->alarm_num[0],gpAfParam->alarm_num[1]);
}

/*************************************************
  Function:    		storage_set_alarm_num
  Description: 		设置报警号码
  Input:
  	1.num1         	1:短信号码1 
  	2.num2			2:短信号码2
  Output:    		
  Return:			TRUE - 成功, FALSE - 失败
  Others:           
*************************************************/
uint32 storage_set_alarm_num(uint8 * num1, uint8 * num2)
{
	if (NULL == num1 || NULL == num2)
	{
		return FALSE;
	}
	
	memcpy(gpAfParam->alarm_num[0], num1, TELNUM_LEN);
	memcpy(gpAfParam->alarm_num[1], num2, TELNUM_LEN);
	SaveRegInfo();
	log_printf("gpAfParam->alarm_num[0]: %s, gpAfParam->alarm_num[1]; %s \n", gpAfParam->alarm_num[0],gpAfParam->alarm_num[1]);
	return TRUE;
}

/*************************************************
  Function:    		storage_get_remote_set
  Description: 		获取远程设置参数
  Input:			无
  Output:    		无
  Return:			远程设置参数	
  Others:           bit0 = 1: 远程安防设置有效
  					bit1 = 1: 允许远程撤防
  					bit2 = 1: 允许远程布防
  					bit3 = 1: 允许远程局防
*************************************************/
uint8 storage_get_remote_set(void)
{
	return gpAfParam->remote_set;
}

/*************************************************
  Function:    		storage_set_remote_set
  Description: 		设置远程设置参数
  Input:
  	1.remote		远程设置参数
  Output:    		
  Return:			false:失败 true:成功	
  Others:           
  					bit0 = 1: 远程安防设置有效
  					bit1 = 1: 允许远程撤防
  					bit2 = 1: 允许远程布防
  					bit3 = 1: 允许远程局防
*************************************************/
uint8 storage_set_remote_set(uint8 remote)
{
	gpAfParam->remote_set = remote;
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_get_link_param
  Description: 		获取联动参数
  Input:
  Output:    		
  Return:			联动参数	
  Others:          	bit0 = 1 梯口刷卡撤防
					bit1 = 1 门口刷卡撤防
					bit2 = 1 第8防区当门前铃
					bit3 = 1 启用布防联动情景模式
					bit5 = 1 启用撤防联动情景模式         
*************************************************/
uint8 storage_get_link_param(void)
{
	return gpAfParam->link_param;
}

/*************************************************
  Function:    		storage_set_link_param
  Description: 		设置联动参数
  Input:
  	1.link			联动参数
  Output:    		
  Return:			false:失败 true:成功	
  Others:           bit0 = 1 梯口刷卡撤防
  					bit1 = 1 门口刷卡撤防
  					bit2 = 1 第8防区当门前铃
  					bit3 = 1 启用布防联动情景模式
  					bit5 = 1 启用撤防联动情景模式
*************************************************/
uint8 storage_set_link_param(uint8 link)
{
	gpAfParam->link_param = link;
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_get_link_scene
  Description: 		获取安防联动情景模式
  Input:
  Output:  
   	1.type			联动情景模式的索引
  Return:				
  Others:           
*************************************************/
void storage_get_link_scene(uint8 * type)
{
	if (NULL == type)
	{
		return;
	}
	
	memcpy(type, gpAfParam->link_scene, sizeof(gpAfParam->link_scene));
}

/*************************************************
  Function:    		storage_set_link_scene
  Description: 		设置安防联动情景模式
  Input:
  Output:  
   	1.type			联动情景模式的索引
  Return:				
  Others:           
*************************************************/
uint8 storage_set_link_scene(uint8 * type)
{
	if (NULL == type)
	{
		return FALSE;
	}
	
	memcpy(gpAfParam->link_scene, type, sizeof(gpAfParam->link_scene));
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_get_area_type
  Description: 		获取防区图标类型
  Input:
  Output:  
   	1.area_type		防区图标类型,每个防区一个字节
  Return:				
  Others:           
*************************************************/
void storage_get_area_type(uint8 * area_type)
{
	if (NULL == area_type)
	{
		return;
	}
	
	memcpy(area_type, gpAfParam->area_type, AREA_AMOUNT);
}

/*************************************************
  Function:    		storage_get_area_type_byindex
  Description: 		获取防区图标类型
  Input:
  Output:  
   	1.area_type		
  Return:				
  Others:           
*************************************************/
uint8 storage_get_area_type_byindex(uint8 Index)
{
	return gpAfParam->area_type[Index];
}

/*************************************************
  Function:    		storage_get_delay_time
  Description: 		获取延时报警时间
  Input:
  Output:  
   	1.delay_time	防区延时报警时间,每个防区一个字节
  Return:				
  Others:           
*************************************************/
void storage_get_delay_time(uint8 * delay_time)
{
	if (NULL == delay_time)
	{
		return;
	}
	
	memcpy(delay_time, gpAfParam->delay_time, AREA_AMOUNT);
}

/*************************************************
  Function:    		storage_get_defend_state
  Description: 		获取整机安防状态
  Input:
  Output:    		
  Return:			安防状态
  Others:           
*************************************************/
DEFEND_STATE storage_get_defend_state(void)
{
	return (DEFEND_STATE)gpAfParam->defend_state;
}

/*************************************************
  Function:    		storage_get_time_param
  Description: 		获取声音的参数
  Input:
  Output: 
  	1.para			参数 1;报警时间 2:退出预警时间 3:是否启用预警声
  Return:			是否启用退出预警声
  Others:           
*************************************************/
void storage_get_time_param(uint8 param[3])
{
	param[0] = gpAfParam->alarm_time;
	param[1] = gpAfParam->exit_alarm_time;
	param[2] = gpAfParam->used_exit_sound;
}

/*************************************************
  Function:    		storage_set_time_param
  Description: 		设置声音的参数
  Input:
  	1.para			参数 1;报警时间 2:退出预警时间 3:是否启用预警声
  Output: 
  Return:			是否启用退出预警声
  Others:           
*************************************************/
uint8 storage_set_time_param(uint8 param[3])
{
	gpAfParam->alarm_time = param[0];
	gpAfParam->exit_alarm_time = param[1];
	gpAfParam->used_exit_sound = param[2];
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_get_validate_code
  Description: 		验证码
  Input:
  Output: 
  Return:			验证码
  Others:           
*************************************************/
uint32 storage_get_validate_code(void)
{
	return gpAfParam->validate_code;
}

/*************************************************
  Function:    		storage_set_validate_code
  Description: 		设置验证码
  Input:
  	1.validate_code	验证码
  Output: 
  Return:			
  Others:           
*************************************************/
uint8 storage_set_validate_code(uint32 validate_code)
{
	gpAfParam->validate_code = validate_code;
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_get_alarm_state
  Description: 		获取安防状态
  Input:			无
  Output:    		无
  Return:			0: 撤防 1:布防 2:报警
  Others:           
*************************************************/
uint8 storage_get_alarm_state(void)
{
	uint8 i;
	
	for (i = 0; i < AREA_AMOUNT; i++)
	{
		if (ALARM_SHOW_STATE == (SHOW_STATE)gpAfParam->show_state[i])
		{
			return 2;
		}
	}
	if (DIS_DEFEND == gpAfParam->defend_state)
	{
		return 0;
	}
	return 1;
}
