/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_linkage.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  联动
				
  History:        
    1. Date:	2011-08-02
       Author:	luofl
       Modification:
       			增加安防联动功能
    2. ...
*************************************************/
//#include "logic_linkage.h"
#include "logic_include.h"

/*************************************************
  Function:			linkage_get_nodisturb_state
  Description: 		获取系统当前免打扰状态
  Input: 			无
  Output:			无
  Return:			TRUE / FALSE
  Others:			
*************************************************/
int32 linkage_get_nodisturb_state(void)
{	
	// 获取免打扰状态
	int32 isNoDisturb = storage_get_noface();
	
	return isNoDisturb;
}

/*************************************************
  Function:			linkage_get_lvd_state
  Description: 		获取系统当前留影留言状态
  Input: 			无
  Output:			无
  Return:			留影留言状态
  Others:			
*************************************************/
LVEWORD_STATE_TYPE linkage_get_lvd_state(void)
{
	LVEWORD_STATE_TYPE LvdState;
	int32 isNoDisturb = linkage_get_nodisturb_state();
	uint8 isLvdUsed = storage_get_lyly_enable();
	uint8 LvdMode = storage_get_lyly_mode();
	
	//if (isLvdUsed == FALSE || (LvdMode >= LYLY_TYPE_MAX || LvdMode < LYLY_TYPE_AUDIO))
	if (isLvdUsed == FALSE || LvdMode >= LYLY_TYPE_MAX )
	{
		LvdState = LVEWORD_TYPE_NONE;
	}
	else
	{
		// 获取免打扰状态及免打扰启用留影留言
		if (isNoDisturb == TRUE && storage_get_lyly_Linkmode(LYLY_LINK_NOFACE) == TRUE)
		{
			LvdState = LVEWORD_TYPE_AUTO;
		}
		// 获取外出状态及外出启用留影留言
		else if (storage_get_defend_state() == SET_DEFEND && storage_get_lyly_Linkmode(LYLY_LINK_ALARMOUT) == TRUE)
		{
			LvdState = LVEWORD_TYPE_AUTO;
		}
		// 获取夜间状态及夜间启用留影留言
		else if (storage_get_defend_state() == PART_DEFEND && storage_get_lyly_Linkmode(LYLY_LINK_ALARMNIGHT) == TRUE)
		{
			LvdState = LVEWORD_TYPE_AUTO;
		}
		else
		{
			LvdState = LVEWORD_TYPE_TIMEOUT;
		}	
	}
	return LvdState;
}

/*************************************************
  Function:			linkage_alarm_card
  Description: 		安防刷卡处理
  Input: 			
  	1.DevType		设备类型 0-梯口 1-门前
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
int32 linkage_alarm_card(uint8 DevType)
{
	uint8 isStairLink = 0;
	uint8 isDoorLink = 0;
	uint8 AlarmLinkParam = storage_get_link_param();
	isStairLink = AlarmLinkParam & 0x01;
	isDoorLink = AlarmLinkParam & 0x2;
	
	if (DevType == 0)
	{
		// 是否启用梯口刷卡撤防功能
		if (isStairLink)
		{
			// 撤防
			alarm_unset_operator(EXECUTOR_LOCAL_HOST, 0);
		}	

		// 梯口刷卡进行被动召梯
		#ifdef _CARD_CALL_ELEVATOR_
		dianti_set_cmd(ELEVATOR_CALL);
		#endif
	}
	else
	{
		// 是否启用门前刷卡撤防功能
		if (isDoorLink)
		{
			// 撤防		
			alarm_unset_operator(EXECUTOR_LOCAL_HOST, 0);
		}	
	}
	return TRUE;
}

/*************************************************
  Function:			linkage_play_door_ring
  Description: 		播放门前铃
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
void linkage_play_door_ring(void)
{
	#if 0
	char * tmp = NULL;
	uint8 ringVolume, ret = FALSE;
	char ringFile[50] = {0};
	uint8 isRingLink = 0;
	uint8 AlarmLinkParam;

	AlarmLinkParam = storage_get_link_param();
	isRingLink = AlarmLinkParam & 0x04;
	if (isRingLink)
	{
		ringVolume = storage_get_ringvolume();
		tmp = storage_get_ring_by_devtype(DEVICE_TYPE_DOOR_NET);
		sprintf(ringFile, "%s", tmp);
		sys_start_play_audio(SYS_MEDIA_MUSIC, ringFile, FALSE, ringVolume, NULL, NULL);
		ret = TRUE;
	}
	return ret;
	#else
	char * tmp = NULL;
	uint8 ringVolume;
	char ringFile[50] = {0};

	ringVolume = storage_get_ringvolume();
	tmp = storage_get_ring_by_devtype(DEVICE_TYPE_DOOR_NET);
	sprintf(ringFile, "%s", tmp);
	sys_start_play_audio(SYS_MEDIA_MUSIC, ringFile, FALSE, ringVolume, NULL, NULL);
	
	#endif
}

/*************************************************
  Function:			linkage_8area_door
  Description: 		第8防区联动门前铃
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
int32 linkage_8area_door(void)
{
	uint8 ret = FALSE;
	uint8 isRingLink = 0;
	uint8 AlarmLinkParam;

	AlarmLinkParam = storage_get_link_param();
	isRingLink = AlarmLinkParam & 0x04;
	if (isRingLink)
	{
		ret = TRUE;
	}
	
	return ret;
}

#ifdef _JD_MODE_
/*************************************************
  Function:			linkage_alarm_scene
  Description: 		布撤防联动情景
  Input: 			
  	1.state			布撤防操作
  Output:			无
  Return:			无
  Others:			
*************************************************/
void linkage_alarm_scene(DEFEND_STATE state)
{
	uint8 sceneLink[2];
	uint8 isbufanLink = 0, ischefanLink = 0;
	uint8 AlarmLinkParam;
	
	AlarmLinkParam = storage_get_link_param();
	storage_get_link_scene(sceneLink);
	isbufanLink = AlarmLinkParam & 0x08;
	ischefanLink = AlarmLinkParam & 0x10;
	
	if (state ==  SET_DEFEND && isbufanLink)
	{
		exec_jd_scene_mode(sceneLink[0]);
	}
	else if (state ==  DIS_DEFEND && ischefanLink)
	{
		exec_jd_scene_mode(sceneLink[1]);
	}
	log_printf("state :.......... %d \n",state);
}
#endif

