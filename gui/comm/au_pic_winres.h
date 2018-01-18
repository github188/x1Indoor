/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	au_pic_winres.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-08-30
  Description:  
				图片资源头文件
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __UI_PIC_WINRES_H__
#define __UI_PIC_WINRES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_include.h"

// 皮肤及默认背景
#define SKIN_FILE							UI_PIC_DIR_PATH"/skin.bmp"					


// 图片资源ID
typedef enum
{
	BID_NULL = 1000,
	BID_DEFAULT_BK,
	
	// 主界面
   	BID_Main1Alarm,		
	BID_Main2Call,
	BID_Main3Message,
	BID_Main5Monitor,
	BID_Main7_Media,
	BID_Main9Setting,
	BID_Main4Center,										
    BID_MainControl_DianTi,
    BID_Main4EControl,
    BID_Main6Record,
    BID_Main8BianLi,
    BID_Main_Photo,
   	BID_Main_KeyDown,
	BID_Main_Net_1,
	BID_Main_Net_2,
	BID_Main_Net_3,
	BID_Main_Net_4,	
	BID_Main_Net_5,
	BID_Ipmodule_Bind_Err,  
    BID_Ipmodule_Online_Err,
    BID_Ipmodule_Online_Ok,
    
 	//e81s需要的主界面图片
	BID_Main1Alarm_1,
	BID_Main2Call_1,
	BID_Main3Message_1,
	BID_Main5Monitor_1,
	BID_MainControl_DianTi_1,
	BID_Main4Center_1,	
    BID_Main7LyLy_1,
   	BID_Main9Setting_1,
   	BID_MainCall_User_1,   
    BID_MainCall_FengJi_1,
    BID_MainMonitor1_Door_1,
    BID_MainMonitor2_Stair_1,
    BID_MainMonitor3_Area_1,
    BID_Main7_Snap_1,
    BID_Main8BianLi_1,
    BID_Main_JRLY_1,
    BID_MAIN_TOP,
    BID_Inter_Volume0_1,
	BID_Inter_Volume1_1,
	BID_Inter_Volume2_1,
	BID_Inter_Volume3_1,
	BID_Inter_Volume4_1,
	BID_Inter_Volume5_1,
	BID_Inter_Volume6_1,
	BID_Inter_Volume7_1,
	BID_Inter_Volume8_1,
    
    // 对讲类
    BID_MainCall_User,                 
    BID_MainCall_FengJi,
    BID_MainCenter_1,
    BID_MainCenter_2,
    BID_MainCenter_3,
    BID_MainMonitor1_Door,
    BID_MainMonitor2_Stair,
    BID_MainMonitor3_Area,
    BID_MainMonitor3_Camera,
    BID_Inter_VideoWinBK,
    BID_Inter_VideoWinBK2,
    BID_Inter_VideoWinBK3,
    BID_Inter_VideoWinBK4,
    BID_Inter_ListCamera,
    BID_Inter_VideoWinBK_Room,
    BID_Inter_CallHouseIcon,

	// 设置类
	BID_Set_AF,
	BID_Set_PRJ,
	BID_Set_SYS,
	BID_Set_USER,
	BID_BJ_Set_Alarm_Num1,
	BID_BJ_Set_ISO_TM,
	BID_BJ_Set_Isolate1,
	BID_BJ_Set_Joint1,
	BID_BJ_Set_Lock_Pass1,
	BID_BJ_Set_Num1,
	BID_BJ_Set_Part_Vald1,
	BID_BJ_Set_Part_Vald,
	BID_BJ_Set_Pass1,
	BID_BJ_Set_Remote1,
   	BID_Set_AFparam1,
	BID_Set_AFproperty1,
	BID_Set_AFproperty,
	BID_Set_Back,
	BID_Set_Ext,
	BID_Set_IP,
   	BID_Set_JD,
	BID_Set_Mic,
	BID_Set_Monitor1,
	BID_Set_Netdoor,
	BID_Set_Netparam,
	BID_Set_PrjPass,
   	BID_Set_Room,
	BID_Set_Date,
	BID_Set_Desktop,
	BID_Set_Language,
	BID_Set_Photo,
	BID_Set_Screen_Light,
	BID_Set_SDcard,
	BID_Set_SYSInfo,
	BID_Set_Sel,
	BID_Set_BK,
	BID_Set_CHINESE,
	BID_Set_CHBIG,
	BID_Set_ENGLISH,
	BID_Set_Distrub,
	BID_Set_LYLY,
	BID_Set_Melody,
	BID_Set_PT,
	BID_Set_Scenem,
	BID_Set_AddVolume,
	BID_Set_AddVolume_on,
	BID_Set_DelVolume,
	BID_Set_DelVolume_on,
	BID_Set_Volume,
	BID_Set_VolumeIcon,
	BID_Set_IP_protocol,

	// 网络门前机
	BID_Set_Netdoor_Second,
    BID_NetDoor_Netparam,
	BID_NetDoor_SysInfo,
	BID_NetDoor_LockType,
    BID_NetDoor_Card,
    BID_NetDoor_Other,

	// 安防类
	BID_BJ_Balcony0,
	BID_BJ_Balcony1,
	BID_BJ_Balcony2,
	BID_BJ_Balcony3,
	BID_BJ_Balcony4,
	BID_BJ_Balcony5,
	BID_BJ_Custom0,
	BID_BJ_Custom1,
	BID_BJ_Custom2,
	BID_BJ_Custom3,
	BID_BJ_Custom4,
	BID_BJ_Custom5,
	BID_BJ_Door0,
	BID_BJ_Door1,
	BID_BJ_Door2,
	BID_BJ_Door3,
	BID_BJ_Door4,
	BID_BJ_Door5,
	BID_BJ_Fire0,
	BID_BJ_Fire1,
	BID_BJ_Fire2,
	BID_BJ_Fire3,
	BID_BJ_Fire4,
	BID_BJ_Fire5,
	BID_BJ_Gas0,
	BID_BJ_Gas1,
	BID_BJ_Gas2,
	BID_BJ_Gas3,
	BID_BJ_Gas4,
	BID_BJ_Gas5,
	BID_BJ_Room0,
	BID_BJ_Room1,
	BID_BJ_Room2,
	BID_BJ_Room3,
	BID_BJ_Room4,
	BID_BJ_Room5,
	BID_BJ_Urgency0,
	BID_BJ_Urgency1,
	BID_BJ_Urgency2,
	BID_BJ_Urgency3,
	BID_BJ_Urgency4,
	BID_BJ_Urgency5,
	BID_BJ_Window0,
	BID_BJ_Window1,
	BID_BJ_Window2,
	BID_BJ_Window3,
	BID_BJ_Window4,
	BID_BJ_Window5,
	BID_FQ_Icon_BJ,
	
	// 便利类
	BID_Main_JRLY,
	BID_Main_CleanScreen,
	BID_Jrly_FLag_1,	
	BID_Jrly_FLag_2,
	BID_Play,                   
	BID_Rec, 

	// 留影留言类
	BID_Main7LyLy,
	BID_Main7_Snap,
    
   	//输入框控件
	BID_Num0,
	BID_Num1,
	BID_Num2,
	BID_Num3,
	BID_Num4,
	BID_Num5,
	BID_Num6,
    BID_Num7,
	BID_Num8,
	BID_Num9,
	BID_NumXin,
	BID_NumJin,
	BID_Input_Ctrl,
	BID_Input_Black,
	BID_InputButton_date1,
	BID_InputButton_date2,
	BID_InputButton_pass1,
	BID_InputButton_pass2,
	BID_InputButton_pass3,
	BID_InputButton_num_ON,
	BID_InputButton_num,
	BID_InputButton_dev,
					 
    // 键盘控件
    BID_Keyboard_Select,
    BID_KeyBoardBK_Pass,
    BID_KeyBoardBK_Pass_1,
    BID_KeyBoardBK_Call,
    
	//右边控件
	BID_RightCtrlBK,
	BID_Right_Down,
	BID_Right_Exit,
	BID_Right_Full,
	BID_Right_Left,
	BID_Right_Right,
	BID_Right_Table,
	BID_Right_Up,
	BID_RightButtonBK1,
	BID_RightButtonBK2,
	BID_RightButtonBK3,

    //列表控件
	BID_ListCtrl_BK5_0,
	BID_ListCtrl_BK5_1,
	BID_ListCtrl_Title,
	BID_ListCtrl_TopBK5_0,
	BID_ListCtrl_TopBK5_1,
	BID_ListCtrl_BK8_0,
	BID_ListCtrl_BK8_1,
	BID_ListCtrl_TopBK8_0,
	BID_ListCtrl_TopBK8_1,
	BID_ListIcon,
	BID_ScrollBK2,
	BID_ScrollDown,
	BID_ScrollDownON,
	BID_ScrollIcon,
	BID_ScrollIconON,
	BID_ScrollUp,
	BID_ScrollUpON,
	BID_SidebarBK,

	// IP控件
	BID_Spin_BK,
	BID_Spin_Sel,

	// 进度控件
	BID_Proc1_Bk,			         
	BID_Proc2_Bk,			         	
	BID_Proc1_Left,			       
	BID_Proc1_On,			         
	BID_Proc1_Right,			      
	BID_Proc2_Left,			       
	BID_Proc2_On,			         	
	BID_Proc2_Right,
	
	// 公共类
	BID_Inter_MuteOff1,
	BID_Inter_MuteOff2,
	BID_Inter_MuteOn1,
    BID_Inter_MuteOn2,
	BID_Inter_Volume0,
	BID_Inter_Volume1,
	BID_Inter_Volume2,
	BID_Inter_Volume3,
	BID_Inter_Volume4,
	BID_Inter_Volume5,
	BID_Inter_Volume6,
	BID_Inter_Volume7,
	BID_Inter_Volume8,
	BID_MsgBoxBK,
    BID_QueryBoxBK,
	BID_Msg_BK1,
	BID_Msg_BK2,
	BID_MsgBoxAlarm,
	BID_MsgBoxCallIn,
	BID_MsgBoxMsg,
	BID_MsgInfoIcon,
	BID_MsgLock,
	BID_MsgQueryIcon,
	BID_MsgRight,
	BID_MsgTalk,
	BID_QueryButton,
	BID_QueryButtonON,
	BID_Un_Read_1,
	BID_Un_Read_2,
	BID_Un_Read_3,
	BID_Un_Read_4,
	BID_Un_Read_5,
	BID_Un_Read_6,
	BID_Un_Read_7,
	BID_Un_Read_8,
	BID_Un_Read_9,
	BID_Un_Read_10,
	BID_Com_Chose,
	BID_Com_Chose_Unenable,
	BID_Com_UnChose,
	BID_Com_UnChose_Unenable,

	// 信息
   	BID_Message_Read,
	BID_Message_UnRead,
	BID_Message_ImageBK1,
	//BID_Message_ImageBK2,
	
	// 便利功能
	BID_Dianti_Call_main,
	
	// 照片
	BID_Snap_Bk,
	BID_Hand_Dc,
	
	// 留影留言
	BID_LyLy_Read,
	BID_LyLy_UnRead,
	BID_LYLY_BK,
	
	BMP_ID_COUNT,
}BMP_ID_E;

void free_bmp_res(void);
PBITMAP get_bmp(BMP_ID_E id);
char * get_flash(BMP_ID_E id);
PBITMAP get_bmp_by_file(char * filename);
PBITMAP get_back_bmp(void);
void set_back_bmp(void);
void reload_bmp_by_skin(void);
void free_bitmaps(BMP_ID_E id);

#ifdef __cplusplus
}
#endif
#endif

