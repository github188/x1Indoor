/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	au_pic_winres.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-08-30
  Description:  
				图片资源处理程序
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "AppConfig.h"
#include "au_pic_winres.h"
//#include "gui_include.h"

#define get_file(ID, file)	\
		case ID:\
			strcpy(filename, file);\
			break
			
#define get_fangan_file(ID, file)	\
		case ID:\
			g_SkinBmpID[g_SkinCount++] = ID;\
			sprintf(filename, file, get_skin());\
			break

//extern uint8 get_skin(void);
static PBITMAP g_Bitmaps[BMP_ID_COUNT]; 			// 小图标的图片资源
static uint32 g_SkinBmpID[30];
static uint32 g_SkinCount = 0;
static PBITMAP g_BackBmp = NULL;					// 背景图片

/*************************************************
  Function:		get_filename
  Description: 	根据图片ID获取图片文件名
  Input: 		
	id			图片ID
  Output:		
  	filename	图片的路径
  Return:		无
  Others:
*************************************************/
static void get_filename(BMP_ID_E id, char * filename)
{
	switch (id)
	{		
		// 背景图片
	    get_file(BID_DEFAULT_BK,					UI_PIC_DIR_PATH"/background.gif");
		
        // 主界面
	    get_file(BID_Main1Alarm,					UI_PIC_DIR_PATH"/main/Main1Alarm.png");
		get_file(BID_Main2Call,					    UI_PIC_DIR_PATH"/main/Main2Call.png");
		get_file(BID_Main3Message,                  UI_PIC_DIR_PATH"/main/Main3Message.png");
		get_file(BID_Main5Monitor,                  UI_PIC_DIR_PATH"/main/Main5Monitor.png");
		get_file(BID_Main7_Media,                   UI_PIC_DIR_PATH"/main/Main7_Media.png");
		get_file(BID_Main9Setting,                  UI_PIC_DIR_PATH"/main/Main9Setting.png");
		get_file(BID_Main4Center,				    UI_PIC_DIR_PATH"/main/Main4Center.png");				
	    get_file(BID_MainControl_DianTi,            UI_PIC_DIR_PATH"/main/MainControl_DianTi.png");
		get_file(BID_Main4EControl,                 UI_PIC_DIR_PATH"/main/Main4EControl.png");			// 缺	
		get_file(BID_Main6Record,                   UI_PIC_DIR_PATH"/main/Main6Record.png");          	// 缺
		get_file(BID_Main8BianLi,                   UI_PIC_DIR_PATH"/main/Main8BianLi.png");          	// 缺
	    get_file(BID_Main_Photo,                    UI_PIC_DIR_PATH"/main/Main_Photo.png");
	   	get_file(BID_Main_KeyDown,                  UI_PIC_DIR_PATH"/main/Main_KeyDown.png");		
		get_file(BID_Main_Net_1,                    UI_PIC_DIR_PATH"/main/net/Main_Net_1.gif");
		get_file(BID_Main_Net_2,                    UI_PIC_DIR_PATH"/main/net/Main_Net_2.gif");
		get_file(BID_Main_Net_3,                    UI_PIC_DIR_PATH"/main/net/Main_Net_3.gif");
		get_file(BID_Main_Net_4,	                UI_PIC_DIR_PATH"/main/net/Main_Net_4.gif");
		get_file(BID_Main_Net_5,                    UI_PIC_DIR_PATH"/main/net/Main_Net_5.gif");
		get_file(BID_Ipmodule_Bind_Err,             UI_PIC_DIR_PATH"/main/net/ipmodule_bind_err.png");
        get_file(BID_Ipmodule_Online_Err,           UI_PIC_DIR_PATH"/main/net/ipmodule_online_err.png");
        get_file(BID_Ipmodule_Online_Ok,            UI_PIC_DIR_PATH"/main/net/ipmodule_online_ok.png");

		//e81s需要的主界面图片
	    get_file(BID_Main1Alarm_1,					UI_PIC_DIR_PATH"/main/Main1Alarm_1.png");
	    get_file(BID_Main2Call_1,					UI_PIC_DIR_PATH"/main/Main2Call1.png");
	    get_file(BID_Main3Message_1,				UI_PIC_DIR_PATH"/main/Main3Message_1.png");
	    get_file(BID_Main5Monitor_1,				UI_PIC_DIR_PATH"/main/Main5Monitor_1.png");
	    get_file(BID_MainControl_DianTi_1,			UI_PIC_DIR_PATH"/main/MainControl_DianTi_1.png");
	    get_file(BID_Main4Center_1,					UI_PIC_DIR_PATH"/main/Main4Center_1.png");
	    get_file(BID_MainCall_User_1,				UI_PIC_DIR_PATH"/main/maincall1_user1.png");
	    get_file(BID_MainCall_FengJi_1,				UI_PIC_DIR_PATH"/main/maincall2_fengji_1.png");
	    get_file(BID_Main9Setting_1,				UI_PIC_DIR_PATH"/main/Main9Setting_1.png");
	    get_file(BID_MainMonitor1_Door_1,			UI_PIC_DIR_PATH"/main/mainmonitor1_door_1.png");
	    get_file(BID_MainMonitor2_Stair_1,			UI_PIC_DIR_PATH"/main/mainmonitor2_stair_1.png");
	    get_file(BID_MainMonitor3_Area_1,			UI_PIC_DIR_PATH"/main/mainmonitor3_area_1.png");
		get_file(BID_Main8BianLi_1,					UI_PIC_DIR_PATH"/main/Main8BianLi_1.png");
	    get_file(BID_Main_JRLY_1,					UI_PIC_DIR_PATH"/main/Main_JRLY_1.png");

	    get_file(BID_MAIN_TOP,						UI_PIC_DIR_PATH"/main/MainTop.png");
		get_file(BID_Inter_Volume0_1,               UI_PIC_DIR_PATH"/comm/volume/Volume0.png");
		get_file(BID_Inter_Volume1_1,               UI_PIC_DIR_PATH"/comm/volume/Volume1.png");
		get_file(BID_Inter_Volume2_1,               UI_PIC_DIR_PATH"/comm/volume/Volume2.png");
		get_file(BID_Inter_Volume3_1,               UI_PIC_DIR_PATH"/comm/volume/Volume3.png");
		get_file(BID_Inter_Volume4_1,               UI_PIC_DIR_PATH"/comm/volume/Volume4.png");
		get_file(BID_Inter_Volume5_1,               UI_PIC_DIR_PATH"/comm/volume/Volume5.png");
		get_file(BID_Inter_Volume6_1,               UI_PIC_DIR_PATH"/comm/volume/Volume6.png");
		get_file(BID_Inter_Volume7_1,               UI_PIC_DIR_PATH"/comm/volume/Volume7.png");		
		get_file(BID_Inter_Volume8_1,               UI_PIC_DIR_PATH"/comm/volume/Volume8.png");
	    get_file(BID_Main7LyLy_1,					UI_PIC_DIR_PATH"/main/Main7LyLy_1.png");
	    get_file(BID_Main7_Snap_1,					UI_PIC_DIR_PATH"/main/Main7_Snap_1.png");

		// 对讲类
	    get_file(BID_MainCall_User,                 UI_PIC_DIR_PATH"/main/MainCall_User.png");
	    get_file(BID_MainCall_FengJi,               UI_PIC_DIR_PATH"/main/MainCall_FengJi.png");
	    get_file(BID_MainCenter_1,                  UI_PIC_DIR_PATH"/main/MainCenter_1.png");         	// 缺
	    get_file(BID_MainCenter_2,                  UI_PIC_DIR_PATH"/main/MainCenter_2.png");         	// 缺
	    get_file(BID_MainCenter_3,                  UI_PIC_DIR_PATH"/main/MainCenter_3.png");         	// 缺
	    get_file(BID_MainMonitor1_Door,             UI_PIC_DIR_PATH"/main/MainMonitor1_Door.png");
	    get_file(BID_MainMonitor2_Stair,            UI_PIC_DIR_PATH"/main/MainMonitor2_Stair.png");
		get_file(BID_MainMonitor3_Area,             UI_PIC_DIR_PATH"/main/MainMonitor3_Area.png");     	// 缺
	    get_file(BID_MainMonitor3_Camera,           UI_PIC_DIR_PATH"/main/MainMonitor3_Camera.png");
		//get_file(BID_Inter_VideoWinBK,              UI_PIC_DIR_PATH"/call/Inter_VideoWinBK.png");
		get_file(BID_Inter_VideoWinBK,              UI_PIC_DIR_PATH"/call/Inter_VideoWinBK.gif");
        get_file(BID_Inter_VideoWinBK2,             UI_PIC_DIR_PATH"/call/Inter_VideoWinBK2.png");
		get_file(BID_Inter_VideoWinBK3,             UI_PIC_DIR_PATH"/call/Inter_VideoWinBK3.png");
		get_file(BID_Inter_VideoWinBK4,             UI_PIC_DIR_PATH"/call/Inter_VideoWinBK4.gif");
		get_file(BID_Inter_ListCamera,				UI_PIC_DIR_PATH"/call/Inter_ListCamera.png");
        get_file(BID_Inter_VideoWinBK_Room,         UI_PIC_DIR_PATH"/call/Inter_VideoWinBK_Room.png");
        get_file(BID_Inter_CallHouseIcon,           UI_PIC_DIR_PATH"/call/Inter_CallHouseIcon.png");

		// 设置类
		get_file(BID_Set_AF,                        UI_PIC_DIR_PATH"/set/Set_AF.png");
		get_file(BID_Set_PRJ,                       UI_PIC_DIR_PATH"/set/Set_PRJ.png");
		get_file(BID_Set_SYS,                       UI_PIC_DIR_PATH"/set/Set_SYS.png");
		get_file(BID_Set_USER,                      UI_PIC_DIR_PATH"/set/Set_USER.png");
		get_file(BID_BJ_Set_Alarm_Num1,             UI_PIC_DIR_PATH"/set/af/BJ_Set_Alarm_Num1.png");
		get_file(BID_BJ_Set_ISO_TM,                 UI_PIC_DIR_PATH"/set/af/BJ_Set_ISO_TM.png");
		get_file(BID_BJ_Set_Isolate1,               UI_PIC_DIR_PATH"/set/af/BJ_Set_Isolate1.png");
		get_file(BID_BJ_Set_Joint1,                 UI_PIC_DIR_PATH"/set/af/BJ_Set_Joint1.png");
		get_file(BID_BJ_Set_Lock_Pass1,             UI_PIC_DIR_PATH"/set/af/BJ_Set_Lock_Pass1.png");
		get_file(BID_BJ_Set_Num1,                   UI_PIC_DIR_PATH"/set/af/BJ_Set_Num1.png");
		get_file(BID_BJ_Set_Part_Vald1,             UI_PIC_DIR_PATH"/set/af/BJ_Set_Part_Vald1.png");
		get_file(BID_BJ_Set_Part_Vald,              UI_PIC_DIR_PATH"/set/af/BJ_Set_Part_Vald.png");
		get_file(BID_BJ_Set_Pass1,                  UI_PIC_DIR_PATH"/set/af/BJ_Set_Pass1.png");
		get_file(BID_BJ_Set_Remote1,                UI_PIC_DIR_PATH"/set/af/BJ_Set_Remote1.png");
	   	get_file(BID_Set_AFparam1,                  UI_PIC_DIR_PATH"/set/af/Set_AFparam1.png");
		get_file(BID_Set_AFproperty1,               UI_PIC_DIR_PATH"/set/af/Set_AFproperty1.png");
		get_file(BID_Set_AFproperty,                UI_PIC_DIR_PATH"/set/project/Set_AFproperty.png");
		get_file(BID_Set_Back,                      UI_PIC_DIR_PATH"/set/project/Set_Back.png");
		get_file(BID_Set_Ext,                       UI_PIC_DIR_PATH"/set/project/Set_Ext.png");
		get_file(BID_Set_IP,                        UI_PIC_DIR_PATH"/set/project/Set_IP.png");
	   	get_file(BID_Set_JD,                        UI_PIC_DIR_PATH"/set/project/Set_JD.png");
		get_file(BID_Set_Mic,                       UI_PIC_DIR_PATH"/set/project/Set_Mic.png");
		get_file(BID_Set_Monitor1,                  UI_PIC_DIR_PATH"/set/project/Set_Monitor1.png");
		get_file(BID_Set_Netdoor,                   UI_PIC_DIR_PATH"/set/project/Set_Netdoor.png");
		get_file(BID_Set_Netparam,                  UI_PIC_DIR_PATH"/set/project/Set_Netparam.png");
		get_file(BID_Set_PrjPass,                   UI_PIC_DIR_PATH"/set/project/Set_PrjPass.png");
	   	get_file(BID_Set_Room,                      UI_PIC_DIR_PATH"/set/project/Set_Room.png");
		get_file(BID_Set_Date,                      UI_PIC_DIR_PATH"/set/system/Set_Date.png");
		get_file(BID_Set_Desktop,                   UI_PIC_DIR_PATH"/set/system/Set_Desktop.png");
		get_file(BID_Set_Language,                  UI_PIC_DIR_PATH"/set/system/Set_Language.png");
		get_file(BID_Set_Photo,                     UI_PIC_DIR_PATH"/set/system/Set_Photo.png");
		get_file(BID_Set_Screen_Light,              UI_PIC_DIR_PATH"/set/system/Set_Screen_Light.png");
		get_file(BID_Set_SDcard,                    UI_PIC_DIR_PATH"/set/system/Set_SDcard.png");
		get_file(BID_Set_SYSInfo,                   UI_PIC_DIR_PATH"/set/system/Set_SYSInfo.png");
		get_file(BID_Set_Sel,                       UI_PIC_DIR_PATH"/set/system/Set_Sel.png");
		get_file(BID_Set_BK,                        UI_PIC_DIR_PATH"/set/system/Set_BK.png");
		get_file(BID_Set_CHINESE,                   UI_PIC_DIR_PATH"/set/system/Set_CHINESE.png");
		get_file(BID_Set_CHBIG,                     UI_PIC_DIR_PATH"/set/system/Set_CHBIG.png");
		get_file(BID_Set_ENGLISH,                   UI_PIC_DIR_PATH"/set/system/Set_ENGLISH.png");
		get_file(BID_Set_Distrub,                   UI_PIC_DIR_PATH"/set/user/Set_Distrub.png");
		get_file(BID_Set_LYLY,                      UI_PIC_DIR_PATH"/set/user/Set_LYLY.png");
		get_file(BID_Set_Melody,                    UI_PIC_DIR_PATH"/set/user/Set_Melody.png");
		get_file(BID_Set_PT,                        UI_PIC_DIR_PATH"/set/user/Set_PT.png");
		get_file(BID_Set_Scenem,                    UI_PIC_DIR_PATH"/set/user/Set_Scenem.png");
		get_file(BID_Set_AddVolume,                 UI_PIC_DIR_PATH"/set/user/Set_AddVolume.png");
		get_file(BID_Set_AddVolume_on,              UI_PIC_DIR_PATH"/set/user/Set_AddVolume_on.png");
		get_file(BID_Set_DelVolume,                 UI_PIC_DIR_PATH"/set/user/Set_DelVolume.png");
		get_file(BID_Set_DelVolume_on,              UI_PIC_DIR_PATH"/set/user/Set_DelVolume_on.png");
		get_file(BID_Set_Volume,                    UI_PIC_DIR_PATH"/set/user/Set_Volume.png");
		get_file(BID_Set_VolumeIcon,                UI_PIC_DIR_PATH"/set/user/Set_VolumeIcon.png");
		get_file(BID_Set_IP_protocol,				UI_PIC_DIR_PATH"/set/project/Set_IP.png");

		//网络门前机
        get_file(BID_Set_Netdoor_Second,           	UI_PIC_DIR_PATH"/set/project/Set_Netdoor.png");
        get_file(BID_NetDoor_Netparam,              UI_PIC_DIR_PATH"/set/project/Set_Netparam.png");
		get_file(BID_NetDoor_SysInfo,         		UI_PIC_DIR_PATH"/set/system/Set_SYSInfo.png");
		get_file(BID_NetDoor_LockType,              UI_PIC_DIR_PATH"/set/project/NetDoor_LockType.png");
        get_file(BID_NetDoor_Card,        			UI_PIC_DIR_PATH"/set/project/NetDoor_Card.png");
        get_file(BID_NetDoor_Other,       			UI_PIC_DIR_PATH"/set/project/NetDoor_Other.png");

		// 安防类
		get_file(BID_BJ_Balcony0,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony0.png");
		get_file(BID_BJ_Balcony1,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony1.png");
		get_file(BID_BJ_Balcony2,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony2.png");
		get_file(BID_BJ_Balcony3,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony3.png");
		get_file(BID_BJ_Balcony4,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony4.png");
		get_file(BID_BJ_Balcony5,					UI_PIC_DIR_PATH"/alarm/BJ_Balcony5.png");
		get_file(BID_BJ_Custom0,					UI_PIC_DIR_PATH"/alarm/BJ_Custom0.png");
		get_file(BID_BJ_Custom1,					UI_PIC_DIR_PATH"/alarm/BJ_Custom1.png");
		get_file(BID_BJ_Custom2,					UI_PIC_DIR_PATH"/alarm/BJ_Custom2.png");
		get_file(BID_BJ_Custom3,					UI_PIC_DIR_PATH"/alarm/BJ_Custom3.png");
		get_file(BID_BJ_Custom4,					UI_PIC_DIR_PATH"/alarm/BJ_Custom4.png");
		get_file(BID_BJ_Custom5,					UI_PIC_DIR_PATH"/alarm/BJ_Custom5.png");
		get_file(BID_BJ_Door0,						UI_PIC_DIR_PATH"/alarm/BJ_Door0.png");
		get_file(BID_BJ_Door1,						UI_PIC_DIR_PATH"/alarm/BJ_Door1.png");
		get_file(BID_BJ_Door2,						UI_PIC_DIR_PATH"/alarm/BJ_Door2.png");
		get_file(BID_BJ_Door3,						UI_PIC_DIR_PATH"/alarm/BJ_Door3.png");
		get_file(BID_BJ_Door4,						UI_PIC_DIR_PATH"/alarm/BJ_Door4.png");
		get_file(BID_BJ_Door5,						UI_PIC_DIR_PATH"/alarm/BJ_Door5.png");
		get_file(BID_BJ_Fire0,						UI_PIC_DIR_PATH"/alarm/BJ_Fire0.png");
		get_file(BID_BJ_Fire1,						UI_PIC_DIR_PATH"/alarm/BJ_Fire1.png");
		get_file(BID_BJ_Fire2,						UI_PIC_DIR_PATH"/alarm/BJ_Fire2.png");
		get_file(BID_BJ_Fire3,						UI_PIC_DIR_PATH"/alarm/BJ_Fire3.png");
		get_file(BID_BJ_Fire4,						UI_PIC_DIR_PATH"/alarm/BJ_Fire4.png");
		get_file(BID_BJ_Fire5,						UI_PIC_DIR_PATH"/alarm/BJ_Fire5.png");
		get_file(BID_BJ_Gas0,						UI_PIC_DIR_PATH"/alarm/BJ_Gas0.png");
		get_file(BID_BJ_Gas1,						UI_PIC_DIR_PATH"/alarm/BJ_Gas1.png");
		get_file(BID_BJ_Gas2,						UI_PIC_DIR_PATH"/alarm/BJ_Gas2.png");
		get_file(BID_BJ_Gas3,						UI_PIC_DIR_PATH"/alarm/BJ_Gas3.png");
		get_file(BID_BJ_Gas4,						UI_PIC_DIR_PATH"/alarm/BJ_Gas4.png");
		get_file(BID_BJ_Gas5,						UI_PIC_DIR_PATH"/alarm/BJ_Gas5.png");
		get_file(BID_BJ_Room0,						UI_PIC_DIR_PATH"/alarm/BJ_Room0.png");
		get_file(BID_BJ_Room1,						UI_PIC_DIR_PATH"/alarm/BJ_Room1.png");
		get_file(BID_BJ_Room2,						UI_PIC_DIR_PATH"/alarm/BJ_Room2.png");
		get_file(BID_BJ_Room3,						UI_PIC_DIR_PATH"/alarm/BJ_Room3.png");
		get_file(BID_BJ_Room4,						UI_PIC_DIR_PATH"/alarm/BJ_Room4.png");
		get_file(BID_BJ_Room5,						UI_PIC_DIR_PATH"/alarm/BJ_Room5.png");
		get_file(BID_BJ_Urgency0,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency0.png");
		get_file(BID_BJ_Urgency1,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency1.png");
		get_file(BID_BJ_Urgency2,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency2.png");
		get_file(BID_BJ_Urgency3,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency3.png");
		get_file(BID_BJ_Urgency4,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency4.png");
		get_file(BID_BJ_Urgency5,					UI_PIC_DIR_PATH"/alarm/BJ_Urgency5.png");
		get_file(BID_BJ_Window0,					UI_PIC_DIR_PATH"/alarm/BJ_Window0.png");
		get_file(BID_BJ_Window1,					UI_PIC_DIR_PATH"/alarm/BJ_Window1.png");
		get_file(BID_BJ_Window2,					UI_PIC_DIR_PATH"/alarm/BJ_Window2.png");
		get_file(BID_BJ_Window3,					UI_PIC_DIR_PATH"/alarm/BJ_Window3.png");
		get_file(BID_BJ_Window4,					UI_PIC_DIR_PATH"/alarm/BJ_Window4.png");
		get_file(BID_BJ_Window5,					UI_PIC_DIR_PATH"/alarm/BJ_Window5.png");
		get_file(BID_FQ_Icon_BJ,					UI_PIC_DIR_PATH"/alarm/FQ_Icon_BJ.png");
	
      	// 便利类
   	    get_file(BID_Main_JRLY,                     UI_PIC_DIR_PATH"/main/Main_JRLY.png");               // 缺
	    get_file(BID_Main_CleanScreen,              UI_PIC_DIR_PATH"/main/Main_CleanScreen.png");         // 缺

		// 留影留言类
     	get_file(BID_Main7LyLy,                     UI_PIC_DIR_PATH"/main/Main7LyLy.png");
	    get_file(BID_Main7_Snap,                    UI_PIC_DIR_PATH"/main/Main7_Snap.png");              // 缺
		
		// 输入框
		get_file(BID_Num0,                           UI_PIC_DIR_PATH"/ctrl/input/Num0.png");
		get_file(BID_Num1,                           UI_PIC_DIR_PATH"/ctrl/input/Num1.png");
		get_file(BID_Num2,                           UI_PIC_DIR_PATH"/ctrl/input/Num2.png");
		get_file(BID_Num3,                           UI_PIC_DIR_PATH"/ctrl/input/Num3.png");
		get_file(BID_Num4,                           UI_PIC_DIR_PATH"/ctrl/input/Num4.png");
		get_file(BID_Num5,                           UI_PIC_DIR_PATH"/ctrl/input/Num5.png");
		get_file(BID_Num6,                           UI_PIC_DIR_PATH"/ctrl/input/Num6.png");
	    get_file(BID_Num7,                           UI_PIC_DIR_PATH"/ctrl/input/Num7.png");
		get_file(BID_Num8,                           UI_PIC_DIR_PATH"/ctrl/input/Num8.png");
		get_file(BID_Num9,                           UI_PIC_DIR_PATH"/ctrl/input/Num9.png");
		get_file(BID_NumXin,                         UI_PIC_DIR_PATH"/ctrl/input/NumXin.png");
		get_file(BID_NumJin,                         UI_PIC_DIR_PATH"/ctrl/input/NumXin.png");
		get_file(BID_Input_Ctrl,                     UI_PIC_DIR_PATH"/ctrl/input/Input_Ctrl.png");	
		get_file(BID_Input_Black,                    UI_PIC_DIR_PATH"/ctrl/input/Input_Black.png");
		get_file(BID_InputButton_date1,              UI_PIC_DIR_PATH"/ctrl/input/InputButton_date1.png");
		get_file(BID_InputButton_date2,              UI_PIC_DIR_PATH"/ctrl/input/InputButton_date2.png");
		get_file(BID_InputButton_pass1,              UI_PIC_DIR_PATH"/ctrl/input/InputButton_pass1.png");
	    get_file(BID_InputButton_pass2,              UI_PIC_DIR_PATH"/ctrl/input/InputButton_pass2.png");
		get_file(BID_InputButton_pass3,              UI_PIC_DIR_PATH"/ctrl/input/InputButton_pass3.png");
		get_file(BID_InputButton_num_ON,             UI_PIC_DIR_PATH"/ctrl/input/InputButton_num_ON.png");
		get_file(BID_InputButton_num,                UI_PIC_DIR_PATH"/ctrl/input/InputButton_num.png");
		get_file(BID_InputButton_dev,                UI_PIC_DIR_PATH"/ctrl/input/InputButton_dev.png");

		// 键盘控件
	    get_file(BID_Keyboard_Select,                UI_PIC_DIR_PATH"/ctrl/keybord/Keyboard_Select.png");
        get_file(BID_KeyBoardBK_Pass,                UI_PIC_DIR_PATH"/ctrl/keybord/KeyBoardBK_Pass.png");
		get_file(BID_KeyBoardBK_Pass_1,              UI_PIC_DIR_PATH"/ctrl/keybord/KeyBoardBK_Pass_1.png");
        get_file(BID_KeyBoardBK_Call,                UI_PIC_DIR_PATH"/ctrl/keybord/KeyBoardBK_Call.png");
		
		//右边控件
		get_file(BID_RightCtrlBK,				     UI_PIC_DIR_PATH"/ctrl/right/RightCtrlBK.png");
		get_file(BID_Right_Down,				     UI_PIC_DIR_PATH"/ctrl/right/Right_Down.png");
		get_file(BID_Right_Exit,				     UI_PIC_DIR_PATH"/ctrl/right/Right_Exit.png");
		get_file(BID_Right_Full,			         UI_PIC_DIR_PATH"/ctrl/right/Right_Full.png");
		get_file(BID_Right_Left,				     UI_PIC_DIR_PATH"/ctrl/right/Right_Left.png");
		get_file(BID_Right_Right,			         UI_PIC_DIR_PATH"/ctrl/right/Right_Right.png");
		get_file(BID_Right_Table,			         UI_PIC_DIR_PATH"/ctrl/right/Right_Table.png");
		get_file(BID_Right_Up,				         UI_PIC_DIR_PATH"/ctrl/right/Right_Up.png");
		get_file(BID_RightButtonBK1,             	 UI_PIC_DIR_PATH"/ctrl/right/RightButtonBK1.png");
		get_file(BID_RightButtonBK2,	             UI_PIC_DIR_PATH"/ctrl/right/RightButtonBK2.png");
		get_file(BID_RightButtonBK3,	             UI_PIC_DIR_PATH"/ctrl/right/RightButtonBK3.png");

        //列表控件
        get_file(BID_ListCtrl_BK5_0,				 UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_BK5_0.png");
	    get_file(BID_ListCtrl_BK5_1,				 UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_BK5_1.png");
	    get_file(BID_ListCtrl_Title,			     UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_Title.png");
	    get_file(BID_ListCtrl_TopBK5_0,			     UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_TopBK5_0.png");
	    get_file(BID_ListCtrl_TopBK5_1,			     UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_TopBK5_1.png");
		get_file(BID_ListCtrl_BK8_0,				 UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_BK8_0.png");
	    get_file(BID_ListCtrl_BK8_1,				 UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_BK8_1.png");
	    get_file(BID_ListCtrl_TopBK8_0,			     UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_TopBK8_0.png");
	    get_file(BID_ListCtrl_TopBK8_1,			     UI_PIC_DIR_PATH"/ctrl/list/ListCtrl_TopBK8_1.png");
	    get_file(BID_ListIcon,			             UI_PIC_DIR_PATH"/ctrl/list/ListIcon.png");
	    get_file(BID_ScrollBK2,		                 UI_PIC_DIR_PATH"/ctrl/list/ScrollBK2.png");
	    get_file(BID_ScrollDown,		             UI_PIC_DIR_PATH"/ctrl/list/ScrollDown.png");
	    get_file(BID_ScrollDownON,		             UI_PIC_DIR_PATH"/ctrl/list/ScrollDownON.png");
	    get_file(BID_ScrollIcon,		             UI_PIC_DIR_PATH"/ctrl/list/ScrollIcon.png");
	    get_file(BID_ScrollIconON,		             UI_PIC_DIR_PATH"/ctrl/list/ScrollIconON.png");
	    get_file(BID_ScrollUp,			             UI_PIC_DIR_PATH"/ctrl/list/ScrollUp.png");
	    get_file(BID_ScrollUpON,		             UI_PIC_DIR_PATH"/ctrl/list/ScrollUpON.png");
	    get_file(BID_SidebarBK,			             UI_PIC_DIR_PATH"/ctrl/list/SidebarBK.png");

		// IP控件
		get_file(BID_Spin_BK,		          	     UI_PIC_DIR_PATH"/ctrl/spin/Spin_BK.png");
	    get_file(BID_Spin_Sel,			             UI_PIC_DIR_PATH"/ctrl/spin/Spin_Sel.png");

		// 进度控件
		get_file(BID_Proc1_Bk,			         	 UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc1_Bk.png");
		get_file(BID_Proc2_Bk,			         	 UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc2_Bk.png");
		get_file(BID_Proc1_Left,			         UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc1_Left.png");
		get_file(BID_Proc1_On,			         	 UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc1_On.png");
		get_file(BID_Proc1_Right,			         UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc1_Right.png");
		get_file(BID_Proc2_Left,			         UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc2_Left.png");
		get_file(BID_Proc2_On,			         	 UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc2_On.png");
		get_file(BID_Proc2_Right,			         UI_PIC_DIR_PATH"/ctrl/prcoressctrl/Proc2_Right.png");

		// 公共类
		get_file(BID_Inter_MuteOff1,                 UI_PIC_DIR_PATH"/comm/volume/Inter_MuteOff1.png");
		get_file(BID_Inter_MuteOff2,                 UI_PIC_DIR_PATH"/comm/volume/Inter_MuteOff2.png");
		get_file(BID_Inter_MuteOn1,                  UI_PIC_DIR_PATH"/comm/volume/Inter_MuteOn1.png");
	    get_file(BID_Inter_MuteOn2,                  UI_PIC_DIR_PATH"/comm/volume/Inter_MuteOn2.png");
		get_file(BID_Inter_Volume0,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume0.png");
		get_file(BID_Inter_Volume1,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume1.png");
		get_file(BID_Inter_Volume2,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume2.png");
		get_file(BID_Inter_Volume3,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume3.png");
		get_file(BID_Inter_Volume4,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume4.png");
		get_file(BID_Inter_Volume5,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume5.png");
		get_file(BID_Inter_Volume6,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume6.png");
		get_file(BID_Inter_Volume7,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume7.png");
		get_file(BID_Inter_Volume8,                  UI_PIC_DIR_PATH"/comm/volume/Inter_Volume8.png");
    	get_file(BID_MsgBoxBK,						 UI_PIC_DIR_PATH"/comm/msg/MsgBoxBK.png");
    	get_file(BID_QueryBoxBK,					 UI_PIC_DIR_PATH"/comm/msg/QueryBoxBK.png");
    	get_file(BID_Msg_BK1,						 UI_PIC_DIR_PATH"/comm/msg/Msg_BK1.png");
		get_file(BID_Msg_BK2,						 UI_PIC_DIR_PATH"/comm/msg/Msg_BK2.png");
		get_file(BID_MsgRight,						 UI_PIC_DIR_PATH"/comm/msg/MsgRight.png");
		get_file(BID_MsgBoxAlarm,					 UI_PIC_DIR_PATH"/comm/msg/MsgBoxAlarm.png");
		get_file(BID_MsgBoxMsg,						 UI_PIC_DIR_PATH"/comm/msg/MsgBoxMsg.png");
		get_file(BID_MsgBoxCallIn,					 UI_PIC_DIR_PATH"/comm/msg/MsgBoxCallIn.png");
		get_file(BID_MsgInfoIcon,					 UI_PIC_DIR_PATH"/comm/msg/MsgInfoIcon.png");
		get_file(BID_MsgLock,                        UI_PIC_DIR_PATH"/comm/msg/MsgLock.png");
		get_file(BID_MsgQueryIcon,                   UI_PIC_DIR_PATH"/comm/msg/MsgQueryIcon.png");
		get_file(BID_MsgTalk,                        UI_PIC_DIR_PATH"/comm/msg/MsgTalk.png");
		get_file(BID_QueryButton,                    UI_PIC_DIR_PATH"/comm/msg/QueryButton.png");
        get_file(BID_QueryButtonON,                  UI_PIC_DIR_PATH"/comm/msg/QueryButtonON.png");
		get_file(BID_Un_Read_1,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_1.png");
		get_file(BID_Un_Read_2,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_2.png");
		get_file(BID_Un_Read_3,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_3.png");
		get_file(BID_Un_Read_4,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_4.png");
		get_file(BID_Un_Read_5,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_5.png");
		get_file(BID_Un_Read_6,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_6.png");
		get_file(BID_Un_Read_7,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_7.png");
		get_file(BID_Un_Read_8,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_8.png");
		get_file(BID_Un_Read_9,						 UI_PIC_DIR_PATH"/comm/unread/Un_Read_9.png");
		get_file(BID_Un_Read_10,					 UI_PIC_DIR_PATH"/comm/unread/Un_Read_10.png");
		get_file(BID_Com_Chose,						 UI_PIC_DIR_PATH"/comm/Com_Chose.png");
		get_file(BID_Com_Chose_Unenable,			 UI_PIC_DIR_PATH"/comm/Com_Chose_Unenable.png");
		get_file(BID_Com_UnChose,					 UI_PIC_DIR_PATH"/comm/Com_UnChose.png");
		get_file(BID_Com_UnChose_Unenable,			 UI_PIC_DIR_PATH"/comm/Com_UnChose_Unenable.png");

		// 信息									
        get_file(BID_Message_Read,                	 UI_PIC_DIR_PATH"/message/Message_Read.png");
		get_file(BID_Message_UnRead,                 UI_PIC_DIR_PATH"/message/Message_UnRead.png");
		get_file(BID_Message_ImageBK1,               UI_PIC_DIR_PATH"/message/Message_ImageBK1.png");
		//get_file(BID_Message_ImageBK2,                UI_PIC_DIR_PATH"/message/Message_ImageBK2.png");
		
		// 便利功能
		get_file(BID_Dianti_Call_main,				 UI_PIC_DIR_PATH"/blgn/Dianti_Call_main.png");
		get_file(BID_Jrly_FLag_1,					 UI_PIC_DIR_PATH"/blgn/Jrly_FLag_1.png");
		get_file(BID_Jrly_FLag_2,					 UI_PIC_DIR_PATH"/blgn/Jrly_FLag_2.png");
		get_file(BID_Play,                       	 UI_PIC_DIR_PATH"/blgn/Play.png");
		get_file(BID_Rec,                       	 UI_PIC_DIR_PATH"/blgn/Rec.png");
		
		// 照片
		get_file(BID_Snap_Bk,						UI_PIC_DIR_PATH"/photo/Snap_Bk.png");
		get_file(BID_Hand_Dc,						UI_PIC_DIR_PATH"/photo/Hand_Dc.png");

		// 留影留言
		get_file(BID_LyLy_Read,            		  	UI_PIC_DIR_PATH"/lyly/LyLy_Read.png");
		get_file(BID_LyLy_UnRead,					UI_PIC_DIR_PATH"/lyly/LyLy_UnRead.png");
		get_file(BID_LYLY_BK,						UI_PIC_DIR_PATH"/lyly/LYLY_BK.png");
		
		default:
			break;
	}
}

/*************************************************
  Function:		get_bitmaps
  Description: 	根据图片ID获取位图
  Input: 		
	id			图片所对应的ID
  Output:		无
  Return:		无
  Others:
*************************************************/
static PBITMAP get_bitmaps(BMP_ID_E id)
{
	char filename[255] = {0};
	
	if (id >= BMP_ID_COUNT || id < 1)
	{
		return NULL;
	}

	//log_printf("get_bitmaps\n");
	if (g_Bitmaps[id] == NULL)
	{
		g_Bitmaps[id] = (PBITMAP)malloc(sizeof(BITMAP));
		if (g_Bitmaps[id] == NULL)
		{
			return NULL;
		}
	}	

	get_filename(id, filename);
	//log_printf("filename: %s\n", filename);
	if (strlen(filename) > 0)
	{	
		if (0 == LoadBitmapFromFile((HDC)NULL, g_Bitmaps[id], filename))
		{
			return g_Bitmaps[id];
		}
	}
	free(g_Bitmaps[id]);
	g_Bitmaps[id] = NULL;
	
	return NULL;
}

/*************************************************
  Function:		free_bitmaps
  Description: 	释放图片资源
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_bitmaps(BMP_ID_E id)  
{
	if (g_Bitmaps[id])
	{
		UnloadBitmap(g_Bitmaps[id]);
		free(g_Bitmaps[id]);
		g_Bitmaps[id] = 0;
	}		
}

/*************************************************
  Function:		free_bmp_res
  Description: 	释放图片资源
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_bmp_res(void)  
{
	uint32 i;
	
	for (i = 0; i < BMP_ID_COUNT; i++)
	{
		if (g_Bitmaps[i])
		{
			UnloadBitmap(g_Bitmaps[i]);
			free(g_Bitmaps[i]);
			g_Bitmaps[i] = 0;
		}		
	} 
}

/*************************************************
  Function:		get_bmp
  Description:  根据图片ID获取图片
  Input: 		
	id			图片ID
  Output:		无
  Return:		图片
  Others:
*************************************************/
PBITMAP get_bmp(BMP_ID_E id)
{
	//log_printf("%s id: %d\n", __FUNCTION__, id);
	if (id > BMP_ID_COUNT || id < 1)
	{
		return NULL;
	}
	
	if (g_Bitmaps[id] != NULL)
	{
		//log_printf("g_Bitmaps != NULL\n");
		return g_Bitmaps[id];
	}
	
	return get_bitmaps(id);
}

/*************************************************
  Function:		get_flash
  Description:  根据图片ID获取动画文件
  Input: 		
	id			图片ID
  Output:		无
  Return:		动画文件名
  Others:
*************************************************/
char * get_flash(BMP_ID_E id)
{
	static char filename[255];
	
	memset(filename, 0, sizeof(filename));
	get_filename(id, filename);
	
	return filename;
}

/*************************************************
  Function:		get_bmp_by_file
  Description:  根据全路径获取图片
  Input: 		
	filename  	图片的全路径
  Output:		无
  Return:		获取的图片地址
  Others:
*************************************************/
PBITMAP get_bmp_by_file(char * filename)
{
	uint32 len = strlen(filename);
	
	if (len == 0)
	{
		return NULL;
	}
	
	char * sname = (char*)calloc(len+1,1);	
	if (sname == NULL)
	{
		return NULL;
	}
	sprintf(sname, "%s", filename);
	PBITMAP bmp = (PBITMAP)malloc(sizeof(BITMAP));	
	if (bmp)
	{
		if (0 != LoadBitmapFromFile((HDC)NULL, bmp, sname))
		{
			free(bmp);
			bmp = NULL;
		}	
	}

	if (sname)
	{
		free(sname);
		sname = NULL;
	}
	
	return bmp;
}

/*************************************************
  Function:		get_back_bmp
  Description:  获得主界面背景
  Input: 		无
  Output:		无
  Return:		图片指针
  Others:
*************************************************/
PBITMAP get_back_bmp(void)
{
	if (g_BackBmp)
	{
		return g_BackBmp;
	}	
	
	//g_BackBmp = get_bmp_by_file((char*)get_desktoppic());
	if (g_BackBmp)
	{
		return g_BackBmp;
	}	
	
	return NULL;
}

/*************************************************
  Function:		set_back_bmp
  Description:  设置主页背景
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void set_back_bmp(void)
{
	if (g_BackBmp)
	{
		UnloadBitmap(g_BackBmp);
		g_BackBmp = NULL;
	}	
	
	//g_BackBmp = get_bmp_by_file((char*)get_desktoppic());		
}

/*************************************************
  Function:		reload_bmp_by_skin
  Description:  切换皮肤时,重新加裁与皮肤相关的图标
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void reload_bmp_by_skin(void)
{
	uint32 i;
	uint32 n = g_SkinCount;
	
	g_SkinCount = 0;
	for (i = 0; i < n; i++)
	{
		uint32 id = g_SkinBmpID[i];
		if (g_Bitmaps[id])
		{
			get_bitmaps(g_SkinBmpID[i]);
		}
	}
}

