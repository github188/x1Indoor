/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	au_language.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-15
  Description:  
				字符处理程序
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"

/*************************************************
  Function:		get_str
  Description: 	根据字符ID获取字符
  Input: 		
	id			图片ID
  Output:		
  	filename	图片的路径
  Return:		无
  Others:
*************************************************/
char * get_str(STRID_E id)
{
	switch (id)
	{
		// 主界面
		case SID_NULL:
			return ESID_NULL;
			
		case SID_MainAlarm:
			return ESID_MainAlarm;

		case SID_MainCall:
			return ESID_MainCall;

		case SID_MainInfo:
			return ESID_MainInfo;

		case SID_MainCenter:
			return ESID_MainCenter;

		case SID_MainMonitor:
			return ESID_MainMonitor;

		case SID_MainRecord:
			return ESID_MainRecord;

		case SID_MainLyLy:
			return ESID_MainLyLy;

		case  SID_MainBianli:
			return ESID_MainBianli;
			
        case  SID_MainBianLi_Dianti:
            return ESID_MainBianLi_Dianti;
			 
		case SID_MainSet:
			return ESID_MainSet;

		case SID_MainEcontrol:
			return ESID_MainEcontrol;
			
		case SID_MainMedia:
			return ESID_MainMedia;

		// 公共文字
		case SID_Registered:
			return ESID_Registered;
			
		case SID_Unregistered:
			return ESID_Unregistered;
			
		case SID_Touch_Calibration:
			return ESID_Touch_Calibration;
			
		case SID_Touch_Disp_Chk_Hit:
			return ESID_Touch_Disp_Chk_Hit;

		case SID_clearscreenhit:
			return ESID_clearscreenhit;

		case SID_Right_Add:
			return ESID_Right_Add;

		case SID_Right_Edit:
			return ESID_Right_Edit;

   		case SID_Right_Del:
			return ESID_Right_Del;
			
		case SID_Right_Clear:
			return ESID_Right_Clear;

		case SID_Right_Save:
			return ESID_Right_Save;

		case SID_Right_View:
			return ESID_Right_View;

		case SID_Right_Sure:
			return ESID_Right_Sure;
			
		case SID_Right_desk:
			return ESID_Right_desk;

		case SID_Right_Rec:
			return ESID_Right_Rec;

		case SID_Right_Listen:
			return ESID_Right_Listen;
			
		case SID_Right_Play:
			return ESID_Right_Play;
			
		case SID_Right_Stop:
			return ESID_Right_Stop;

		case SID_Right_Pause:
			return ESID_Right_Pause;

		case SID_Right_Updata:
			return ESID_Right_Updata;

		case SID_Right_Select:
			return ESID_Right_Select;
			
		case SID_Right_Input:
			 return ESID_Right_Input;
			 
		case SID_Week_Sun:
			return ESID_Week_Sun;
			
		case SID_Week_Mon:
			return ESID_Week_Mon;
			
		case SID_Week_Tue:
			return ESID_Week_Tue;

		case SID_Week_Wed:
			return ESID_Week_Wed;

		case SID_Week_Thu:
			return ESID_Week_Thu;
			
		case SID_Week_Fri:
			return ESID_Week_Fri;
			
		case SID_Week_Sat:
			return ESID_Week_Sat;
			
		case SID_Time_Sec:
			return ESID_Time_Sec;

		case SID_Time_Hour:
			return ESID_Time_Hour;

		case SID_Time_Min:
			return ESID_Time_Min;

		case SID_Time_Never:
			return ESID_Time_Never;
			
		case SID_Set_Time:
			return ESID_Set_Time;

		case SID_Set_Mode:
			return ESID_Set_Mode;
			
		case SID_Set_Used:
			return ESID_Set_Used;

		case SID_Save_OK:
			return ESID_Save_OK;

		case SID_Save_ERR:
			return ESID_Save_ERR;

		case SID_Msg_Yes:
			return ESID_Msg_Yes;
			
		case SID_Msg_No:
			return ESID_Msg_No;

		case SID_Msg_Format:
			return ESID_Msg_Format;
			
		case SID_Oper_OK:
			return ESID_Oper_OK;

		case SID_Oper_ERR:
			return ESID_Oper_ERR;

		case SID_Msg_Calibration:
			return ESID_Msg_Calibration;

		case SID_Msg_OldPass_Err:
			return ESID_Msg_OldPass_Err;
			
		case SID_Msg_SamePass_Err:
			return ESID_Msg_SamePass_Err;

		case SID_Msg_InputBox_Psw_Err:
			return ESID_Msg_InputBox_Psw_Err;

		case SID_Msg_Jrly_rec_tmp:
			return ESID_Msg_Jrly_rec_tmp;
			
		case SID_Msg_Jrly_rec_full:
			return ESID_Msg_Jrly_rec_full;
			
		case SID_Msg_Connect_Outtime:
			return ESID_Msg_Connect_Outtime;
			
		case SID_Msg_Alarm_Hit:
			return ESID_Msg_Alarm_Hit;

		case SID_Msg_Info_Hit:
			return ESID_Msg_Info_Hit;

		case SID_Msg_CallIn_Hit:
			return ESID_Msg_CallIn_Hit;
			
		case SID_Msg_FactoryMode:
			return ESID_Msg_FactoryMode;

		case SID_Msg_Cell_Err:
			return ESID_Msg_Cell_Err;
			
		case SID_Msg_Stair_Err:
			return ESID_Msg_Stair_Err;

		case SID_Msg_Room_Err:
			return ESID_Msg_Room_Err;

		case SID_Msg_RuleAll_Err:
			return ESID_Msg_RuleAll_Err;
			
		case SID_Msg_Len_Err:
			return ESID_Msg_Len_Err;	
			
		case SID_Msg_RoomNo_Err:
			return ESID_Msg_RoomNo_Err;
			
		case SID_Msg_Recover_OK:
			return ESID_Msg_Recover_OK;

		case SID_Msg_Exit:
			return ESID_Msg_Exit;

		case SID_Msg_Dianti_Hit1:
			return ESID_Msg_Dianti_Hit1;
			
		case SID_Msg_Dianti_Hit2:
			return ESID_Msg_Dianti_Hit2;
			
		case SID_Msg_Reset:
			return ESID_Msg_Reset;
			
		case SID_Msg_JD_Is_Updata:
			return ESID_Msg_JD_Is_Updata;

		case SID_Msg_JD_Is_Updata1:
			return ESID_Msg_JD_Is_Updata1;

		case SID_Msg_JD_Updataing:
			return ESID_Msg_JD_Updataing;
			
		case SID_Msg_JD_Unbind:
			return ESID_Msg_JD_Unbind;

		case SID_Msg_Create_BindCode:
			return ESID_Msg_Create_BindCode;
			
		case SID_IP_Unbind:
			return ESID_IP_Unbind;

		case SID_IP_Bind:
			return ESID_IP_Bind;

		case SID_Msg_Jd_State_Updata:
			return ESID_Msg_Jd_State_Updata;
			
		case SID_IP_Online:
			return ESID_IP_Online;

		case SID_IP_Unline:
			return ESID_IP_Unline;
			
		case SID_IP_Con_Addr:
			return ESID_IP_Con_Addr;

		case SID_Jd_Deng1:
			return ESID_Jd_Deng1;

		case SID_Jd_Deng2:
			return ESID_Jd_Deng2;
			
		case SID_Jd_Deng3:
			return ESID_Jd_Deng3;

		case SID_Jd_Deng4:
			return ESID_Jd_Deng4;
			
		case SID_Jd_Deng5:
			return ESID_Jd_Deng5;

		case SID_Jd_Deng6:
			return ESID_Jd_Deng6;

		case SID_Jd_Deng7:
			return ESID_Jd_Deng7;
			
		case SID_Jd_Deng8:
			return ESID_Jd_Deng8;

		case SID_Jd_Deng9:
			return ESID_Jd_Deng9;
			
		case SID_Jd_Deng10:
			return ESID_Jd_Deng10;

		case SID_Jd_TiaoGuangDeng1:
			return ESID_Jd_TiaoGuangDeng1;

		case SID_Jd_TiaoGuangDeng2:
			return ESID_Jd_TiaoGuangDeng2;

		case SID_Jd_TiaoGuangDeng3:
			return ESID_Jd_TiaoGuangDeng3;

		case SID_Jd_TiaoGuangDeng4:
			return ESID_Jd_TiaoGuangDeng4;

		case SID_Jd_TiaoGuangDeng5:
			return ESID_Jd_TiaoGuangDeng5;
															
		case SID_Jd_TiaoGuangDeng6:
			return ESID_Jd_TiaoGuangDeng6;

		case SID_Jd_TiaoGuangDeng7:
			return ESID_Jd_TiaoGuangDeng7;

		case SID_Jd_TiaoGuangDeng8:
			return ESID_Jd_TiaoGuangDeng8;

		case SID_Jd_TiaoGuangDeng9:
			return ESID_Jd_TiaoGuangDeng9;

		case SID_Jd_TiaoGuangDeng10:
			return ESID_Jd_TiaoGuangDeng10;

		case SID_Jd_BaiChiDeng1:
			return ESID_Jd_BaiChiDeng1;

		case SID_Jd_BaiChiDeng2:
			return ESID_Jd_BaiChiDeng2;

		case SID_Jd_BaiChiDeng3:
			return ESID_Jd_BaiChiDeng3;

		case SID_Jd_BaiChiDeng4:
			return ESID_Jd_BaiChiDeng4;

		case SID_Jd_BaiChiDeng5:
			return ESID_Jd_BaiChiDeng5;

		case SID_Jd_BaiChiDeng6:
			return ESID_Jd_BaiChiDeng6;

		case SID_Jd_BaiChiDeng7:
			return ESID_Jd_BaiChiDeng7;

		case SID_Jd_BaiChiDeng8:
			return ESID_Jd_BaiChiDeng8;

		case SID_Jd_BaiChiDeng9:
			return ESID_Jd_BaiChiDeng9;

		case SID_Jd_BaiChiDeng10:
			return ESID_Jd_BaiChiDeng10;

		case SID_Jd_GuangGuan1:
			return ESID_Jd_GuangGuan1;

		case SID_Jd_GuangGuan2:
			return ESID_Jd_GuangGuan2;

		case SID_Jd_GuangGuan3:
			return ESID_Jd_GuangGuan3;

		case SID_Jd_GuangGuan4:
			return ESID_Jd_GuangGuan4;

		case SID_Jd_GuangGuan5:
			return ESID_Jd_GuangGuan5;

		case SID_Jd_GuangGuan6:
			return ESID_Jd_GuangGuan6;

		case SID_Jd_GuangGuan7:
			return ESID_Jd_GuangGuan7;

		case SID_Jd_GuangGuan8:
			return ESID_Jd_GuangGuan8;

		case SID_Jd_GuangGuan9:
			return ESID_Jd_GuangGuan9;

		case SID_Jd_GuangGuan10:
			return ESID_Jd_GuangGuan10;

		case SID_Jd_DiaoDeng1:
			return ESID_Jd_DiaoDeng1;

		case SID_Jd_DiaoDeng2:
			return ESID_Jd_DiaoDeng2;

		case SID_Jd_DiaoDeng3:
			return ESID_Jd_DiaoDeng3;

		case SID_Jd_DiaoDeng4:
			return ESID_Jd_DiaoDeng4;

		case SID_Jd_DiaoDeng5:
			return ESID_Jd_DiaoDeng5;

		case SID_Jd_DiaoDeng6:
			return ESID_Jd_DiaoDeng6;

		case SID_Jd_DiaoDeng7:
			return ESID_Jd_DiaoDeng7;

		case SID_Jd_DiaoDeng8:
			return ESID_Jd_DiaoDeng8;

		case SID_Jd_DiaoDeng9:
			return ESID_Jd_DiaoDeng9;

		case SID_Jd_DiaoDeng10:
			return ESID_Jd_DiaoDeng10;

		case SID_Jd_HuaDeng1:
			return ESID_Jd_HuaDeng1;

		case SID_Jd_HuaDeng2:
			return ESID_Jd_HuaDeng2;

		case SID_Jd_HuaDeng3:
			return ESID_Jd_HuaDeng3;

		case SID_Jd_HuaDeng4:
			return ESID_Jd_HuaDeng4;

		case SID_Jd_HuaDeng5:
			return ESID_Jd_HuaDeng5;

		case SID_Jd_HuaDeng6:
			return ESID_Jd_HuaDeng6;

		case SID_Jd_HuaDeng7:
			return ESID_Jd_HuaDeng7;

		case SID_Jd_HuaDeng8:
			return ESID_Jd_HuaDeng8;

		case SID_Jd_HuaDeng9:
			return ESID_Jd_HuaDeng9;

		case SID_Jd_HuaDeng10:
			return ESID_Jd_HuaDeng10;

		case SID_Jd_BiDeng1:
			return ESID_Jd_BiDeng1;

		case SID_Jd_BiDeng2:
			return ESID_Jd_BiDeng2;

		case SID_Jd_BiDeng3:
			return ESID_Jd_BiDeng3;

		case SID_Jd_BiDeng4:
			return ESID_Jd_BiDeng4;

		case SID_Jd_BiDeng5:
			return ESID_Jd_BiDeng5;

		case SID_Jd_BiDeng6:
			return ESID_Jd_BiDeng6;

		case SID_Jd_BiDeng7:
			return ESID_Jd_BiDeng7;

		case SID_Jd_BiDeng8:
			return ESID_Jd_BiDeng8;

		case SID_Jd_BiDeng9:
			return ESID_Jd_BiDeng9;

		case SID_Jd_BiDeng10:
			return ESID_Jd_BiDeng10;

		case SID_Jd_SheDeng1:
			return ESID_Jd_SheDeng1;

		case SID_Jd_SheDeng2:
			return ESID_Jd_SheDeng2;

		case SID_Jd_SheDeng3:
			return ESID_Jd_SheDeng3;

		case SID_Jd_SheDeng4:
			return ESID_Jd_SheDeng4;

		case SID_Jd_SheDeng5:
			return ESID_Jd_SheDeng5;

		case SID_Jd_SheDeng6:
			return ESID_Jd_SheDeng6;

		case SID_Jd_SheDeng7:
			return ESID_Jd_SheDeng7;

		case SID_Jd_SheDeng8:
			return ESID_Jd_SheDeng8;

		case SID_Jd_SheDeng9:
			return ESID_Jd_SheDeng9;

		case SID_Jd_SheDeng10:
			return ESID_Jd_SheDeng10;

		case SID_Jd_TaiDeng1:
			return ESID_Jd_TaiDeng1;

		case SID_Jd_TaiDeng2:
			return ESID_Jd_TaiDeng2;

		case SID_Jd_TaiDeng3:
			return ESID_Jd_TaiDeng3;

		case SID_Jd_TaiDeng4:
			return ESID_Jd_TaiDeng4;

		case SID_Jd_TaiDeng5:
			return ESID_Jd_TaiDeng5;

		case SID_Jd_TaiDeng6:
			return ESID_Jd_TaiDeng6;

		case SID_Jd_TaiDeng7:
			return ESID_Jd_TaiDeng7;

		case SID_Jd_TaiDeng8:
			return ESID_Jd_TaiDeng8;

		case SID_Jd_TaiDeng9:
			return ESID_Jd_TaiDeng9;

		case SID_Jd_TaiDeng10:
			return ESID_Jd_TaiDeng10;

		case SID_Jd_LouDiDeng1:
			return ESID_Jd_LouDiDeng1;

		case SID_Jd_LouDiDeng2:
			return ESID_Jd_LouDiDeng2;

		case SID_Jd_LouDiDeng3:
			return ESID_Jd_LouDiDeng3;

		case SID_Jd_LouDiDeng4:
			return ESID_Jd_LouDiDeng4;

		case SID_Jd_LouDiDeng5:
			return ESID_Jd_LouDiDeng5;

		case SID_Jd_LouDiDeng6:
			return ESID_Jd_LouDiDeng6;

		case SID_Jd_LouDiDeng7:
			return ESID_Jd_LouDiDeng7;

		case SID_Jd_LouDiDeng8:
			return ESID_Jd_LouDiDeng8;

		case SID_Jd_LouDiDeng9:
			return ESID_Jd_LouDiDeng9;

		case SID_Jd_LouDiDeng10:
			return ESID_Jd_LouDiDeng10;

		case SID_Jd_ZhaoMingDeng1:
			return ESID_Jd_ZhaoMingDeng1;

		case SID_Jd_ZhaoMingDeng2:
			return ESID_Jd_ZhaoMingDeng2;

		case SID_Jd_ZhaoMingDeng3:
			return ESID_Jd_ZhaoMingDeng3;

		case SID_Jd_ZhaoMingDeng4:
			return ESID_Jd_ZhaoMingDeng4;

		case SID_Jd_ZhaoMingDeng5:
			return ESID_Jd_ZhaoMingDeng5;

		case SID_Jd_ZhaoMingDeng6:
			return ESID_Jd_ZhaoMingDeng6;

		case SID_Jd_ZhaoMingDeng7:
			return ESID_Jd_ZhaoMingDeng7;

		case SID_Jd_ZhaoMingDeng8:
			return ESID_Jd_ZhaoMingDeng8;

		case SID_Jd_ZhaoMingDeng9:
			return ESID_Jd_ZhaoMingDeng9;

		case SID_Jd_ZhaoMingDeng10:
			return ESID_Jd_ZhaoMingDeng10;

		case SID_Jd_ShiYingDeng1:
			return ESID_Jd_ShiYingDeng1;

		case SID_Jd_ShiYingDeng2:
			return ESID_Jd_ShiYingDeng2;

		case SID_Jd_ShiYingDeng3:
			return ESID_Jd_ShiYingDeng3;

		case SID_Jd_ShiYingDeng4:
			return ESID_Jd_ShiYingDeng4;

		case SID_Jd_ShiYingDeng5:
			return ESID_Jd_ShiYingDeng5;

		case SID_Jd_ShiYingDeng6:
			return ESID_Jd_ShiYingDeng6;

		case SID_Jd_ShiYingDeng7:
			return ESID_Jd_ShiYingDeng7;

		case SID_Jd_ShiYingDeng8:
			return ESID_Jd_ShiYingDeng8;

		case SID_Jd_ShiYingDeng9:
			return ESID_Jd_ShiYingDeng9;

		case SID_Jd_ShiYingDeng10:
			return ESID_Jd_ShiYingDeng10;

		case SID_Jd_YeDeng1:
			return ESID_Jd_YeDeng1;

		case SID_Jd_YeDeng2:
			return ESID_Jd_YeDeng2;

		case SID_Jd_YeDeng3:
			return ESID_Jd_YeDeng3;

		case SID_Jd_YeDeng4:
			return ESID_Jd_YeDeng4;

		case SID_Jd_YeDeng5:
			return ESID_Jd_YeDeng5;

		case SID_Jd_YeDeng6:
			return ESID_Jd_YeDeng6;

		case SID_Jd_YeDeng7:
			return ESID_Jd_YeDeng7;

		case SID_Jd_YeDeng8:
			return ESID_Jd_YeDeng8;

		case SID_Jd_YeDeng9:
			return ESID_Jd_YeDeng9;

		case SID_Jd_YeDeng10:
			return ESID_Jd_YeDeng10;

		case SID_Jd_ZhuanshiDEng1:
			return ESID_Jd_ZhuanshiDEng1;

		case SID_Jd_ZhuanshiDEng2:
			return ESID_Jd_ZhuanshiDEng2;

		case SID_Jd_ZhuanshiDEng3:
			return ESID_Jd_ZhuanshiDEng3;

		case SID_Jd_ZhuanshiDEng4:
			return ESID_Jd_ZhuanshiDEng4;

		case SID_Jd_ZhuanshiDEng5:
			return ESID_Jd_ZhuanshiDEng5;

		case SID_Jd_ZhuanshiDEng6:
			return ESID_Jd_ZhuanshiDEng6;

		case SID_Jd_ZhuanshiDEng7:
			return ESID_Jd_ZhuanshiDEng7;

		case SID_Jd_ZhuanshiDEng8:
			return ESID_Jd_ZhuanshiDEng8;

		case SID_Jd_ZhuanshiDEng9:
			return ESID_Jd_ZhuanshiDEng9;

		case SID_Jd_ZhuanshiDEng10:
			return ESID_Jd_ZhuanshiDEng10;

		// 设置类
		case SID_Set_Menu_System:
			return ESID_Set_Menu_System;
			
		case SID_Set_Menu_Project:
			return ESID_Set_Menu_Project;

		case SID_Set_Menu_Alarm:
			return ESID_Set_Menu_Alarm;

		case SID_Set_Menu_User:
			return ESID_Set_Menu_User;
			
		case SID_Set_System_language:
			return ESID_Set_System_language;

		case SID_Set_System_Time:
			return ESID_Set_System_Time;
		
		case SID_Set_System_Desk:
			return ESID_Set_System_Desk;
			
		case SID_Set_System_Screen:
			return ESID_Set_System_Screen;

		#if 0
		case SID_Set_System_Light:
			return ESID_Set_System_Light;
		#endif

		case SID_Set_System_Storage:
			return ESID_Set_System_Storage;
			
		case SID_Set_System_Info:
			return ESID_Set_System_Info;

		case SID_Set_Prj_Pass:
			return ESID_Set_Prj_Pass;
			
		case SID_Set_Prj_Alarm:
			return ESID_Set_Prj_Alarm;

		case SID_Set_Prj_Net:
			return ESID_Set_Prj_Net;

		case SID_Set_Prj_ExtMode:
			return ESID_Set_Prj_ExtMode;
			
		case SID_Set_Prj_DevNo:
			return ESID_Set_Prj_DevNo;

		case SID_Set_Prj_NetDoor:
			return ESID_Set_Prj_NetDoor;
			
		case SID_Set_Prj_Default:
			return ESID_Set_Prj_Default;

		case SID_Set_Prj_JiaDian:
			return ESID_Set_Prj_JiaDian;

		#if 0
		case SID_Set_Prj_Mic:
			return ESID_Set_Prj_Mic;
			
		case SID_Set_Prj_Mic_Db:
			return ESID_Set_Prj_Mic_Db;
		#endif

		case SID_Set_Prj_Set_Rtsp:
			return ESID_Set_Prj_Set_Rtsp;
			
		case SID_Set_Prj_Set_IPModule:
			return ESID_Set_Prj_Set_IPModule;

		case SID_Set_Alarm_UserPwd:
			return ESID_Set_Alarm_UserPwd;

		case SID_Set_Alarm_DoorPwd:
			return ESID_Set_Alarm_DoorPwd;
			
		case SID_Set_Alarm_AfLink:
			return ESID_Set_Alarm_AfLink;

		case SID_Set_Alarm_AfGeLi:
			return ESID_Set_Alarm_AfGeLi;
			
		case SID_Set_Alarm_AfJuFang:
			return ESID_Set_Alarm_AfJuFang;

		case SID_Set_UAlarm_Remote:
			return ESID_Set_UAlarm_Remote;

		case SID_Set_Alarm_MsgNum:
			return ESID_Set_Alarm_MsgNum;

		#if 0
		case SID_Set_Alarm_TelNum:
			return ESID_Set_Alarm_TelNum;
		#endif

		case SID_Set_User_Sound:
			return ESID_Set_User_Sound;
			
		case SID_Set_User_Volume:
			return ESID_Set_User_Volume;

		case SID_Set_User_NoFace:
			return ESID_Set_User_NoFace;

		case SID_Set_User_LyLy:
			return ESID_Set_User_LyLy;
			
		case SID_Set_User_PT:
			return ESID_Set_User_PT;

		case SID_Set_Unreg:
			return ESID_Set_Unreg;
			
		case SID_Set_Reg_Code:
			return ESID_Set_Reg_Code;

		case SID_Set_Reging:
			return ESID_Set_Reging;

		case SID_Set_Prj_Net_HostIP:
			return ESID_Set_Prj_Net_HostIP;
			
		case SID_Set_Prj_Net_HostNetMask:
			return ESID_Set_Prj_Net_HostNetMask;

		case SID_Set_Prj_Net_HostGateWay:
			return ESID_Set_Prj_Net_HostGateWay;
			
		case SID_Set_Prj_Net_CenterSever:
			return ESID_Set_Prj_Net_CenterSever;

		case SID_Set_Prj_Net_ManageIP1:
			return ESID_Set_Prj_Net_ManageIP1;

		case SID_Set_Prj_Net_ManageIP2:
			return ESID_Set_Prj_Net_ManageIP2;
			
		case SID_Set_Prj_Net_ManageIP3:
			return ESID_Set_Prj_Net_ManageIP3;

		case SID_Set_Prj_Net_AurineSever:
			return ESID_Set_Prj_Net_AurineSever;

		case SID_Set_Prj_Net_AurineElevator:
			return ESID_Set_Prj_Net_AurineElevator;

		case SID_Set_Prj_Net_Rtsp:
			return ESID_Set_Prj_Net_Rtsp;
			
		case SID_Set_Prj_Net_Stair:
			return ESID_Set_Prj_Net_Stair;

		case SID_Set_Prj_Net_IPModule_IP:
			return ESID_Set_Prj_Net_IPModule_IP;
			
		case SID_Set_Prj_Net_IPModule_Code:
			return ESID_Set_Prj_Net_IPModule_Code;

		case SID_Set_Dev_Rule:
			return ESID_Set_Dev_Rule;
			
		case SID_Set_Dev_Stair_len:
			return ESID_Set_Dev_Stair_len;

		case SID_Set_Dev_Room_len:
			return ESID_Set_Dev_Room_len;

		case SID_Set_Dev_Cell_len:
			return ESID_Set_Dev_Cell_len;
			
		case SID_Set_Dev_UseCell:
			return ESID_Set_Dev_UseCell;

		case SID_Set_Dev_BIT:
			return ESID_Set_Dev_BIT;
			
		case SID_Set_Ext_Alarm:
			return ESID_Set_Ext_Alarm;

		case SID_Set_Ext_JiaDian:
			return ESID_Set_Ext_JiaDian;

		case SID_Set_Ext_Stair:
			return ESID_Set_Ext_Stair;

		case SID_Set_Language:
			return ESID_Set_Language;

		#ifdef _TCPTO485_ELEVATOR_MODE_
		case SID_Set_TCP485Elevator:
			return ESID_Set_TCP485Elevator;
		#endif
			
        // 网络门前机文字
		case SID_Net_Door_First_R:
			return ESID_Net_Door_First_R;
			
		case SID_Net_Door_Second_R:
			return ESID_Net_Door_Second_R;
			
		case SID_Net_Door_First_A:
			return ESID_Net_Door_First_A;
			
		case SID_Net_Door_Second_A:
			return ESID_Net_Door_Second_A;

		case SID_Net_Door:
			return ESID_Net_Door;

		case SID_Net_Door_First:
			return ESID_Net_Door_First;

		case SID_Net_Door_Second:
			return ESID_Net_Door_Second;
			
		case SID_Net_Door_IP:
			return ESID_Net_Door_IP;

		case SID_Net_Door_Sub:
			return ESID_Net_Door_Sub;
			
		case SID_Net_Door_GateWay:
			return ESID_Net_Door_GateWay;

		case SID_Net_Door_GetMAC:
			return ESID_Net_Door_GetMAC;

		case SID_Net_Door_MAC:
			return ESID_Net_Door_MAC;			

		case SID_Net_Door_NetParam:
			return ESID_Net_Door_NetParam;
			
		case SID_Net_Door_LockType:
			return ESID_Net_Door_LockType;

		case SID_Net_Door_CardMan:
			return ESID_Net_Door_CardMan;

		case SID_Net_Door_SysInfo:
			return ESID_Net_Door_SysInfo;
			
		case SID_Net_Door_Other_Set:
			return ESID_Net_Door_Other_Set;

		case SID_Net_Door_Open:
			return ESID_Net_Door_Open;
			
		case SID_Net_Door_Close:
			return ESID_Net_Door_Close;

		case SID_Net_Door_LTime_Mh:
			return ESID_Net_Door_LTime_Mh;

		case SID_Net_Door_LockTime:
			return ESID_Net_Door_LockTime;
			
		case SID_Net_Door_Pic_Up:
			return ESID_Net_Door_Pic_Up;

		case SID_Net_Door_Remote_Moniter:
			return ESID_Net_Door_Remote_Moniter;
			
		case SID_Net_Door_Add_Card:
			return ESID_Net_Door_Add_Card;

		case SID_Net_Door_Del_Card:
			return ESID_Net_Door_Del_Card;

		case SID_Net_Door_Unused_First:
			return ESID_Net_Door_Unused_First;
			
		case SID_Net_Door_Unused_Second:
			return ESID_Net_Door_Unused_Second;

		case SID_Net_Door_Pls_Add_Card:
			return ESID_Net_Door_Pls_Add_Card;
			
		case SID_Net_Door_Sure_Del_Card:
			return ESID_Net_Door_Sure_Del_Card;

		case SID_Net_Door_Video_Fmt:
			return ESID_Net_Door_Video_Fmt;

		case SID_Set_Info_HW:
			return ESID_Set_Info_HW;
			
		case SID_Set_Info_SF:
			return ESID_Set_Info_SF;

		case SID_Set_Info_SDK:
			return ESID_Set_Info_SDK;
			
		case SID_Set_Info_HostMAC:
			return ESID_Set_Info_HostMAC;

		case SID_Set_Info_Room:
			return ESID_Set_Info_Room;

		case SID_Set_Timer_Wanl:
			return ESID_Set_Timer_Wanl;
			
		case SID_Storage_Format:
			return ESID_Storage_Format;

		case SID_Storage_Pic:
			return ESID_Storage_Pic;
			
		case SID_Set_Pass_Server:
			return ESID_Set_Pass_Server;

		case SID_Set_Pass_DoorUser:
			return ESID_Set_Pass_DoorUser;

		case SID_Set_Pass_DoorServer:
			return ESID_Set_Pass_DoorServer;
			
		case SID_Set_Pass_Old:
			return ESID_Set_Pass_Old;

		case SID_Set_Pass_New:
			return ESID_Set_Pass_New;
			
		case SID_Set_Pass_Query:
			return ESID_Set_Pass_Query;

		case SID_Set_volume_ring:
			return ESID_Set_volume_ring;

		case SID_Set_volume_talk:
			return ESID_Set_volume_talk;
			
		case SID_Set_volume_key:
			return ESID_Set_volume_key;

		case SID_Set_ring_Stair:
			return ESID_Set_ring_Stair;
			
		case SID_Set_ring_Area:
			return ESID_Set_ring_Area;

		case SID_Set_ring_Door:
			return ESID_Set_ring_Door;

		case SID_Set_ring_Center:
			return ESID_Set_ring_Center;
			
		case SID_Set_ring_Room:
			return ESID_Set_ring_Room;

		case SID_Set_ring_fenji:
			return ESID_Set_ring_fenji;
			
		case SID_Set_ring_Info:
			return ESID_Set_ring_Info;

		case SID_Set_ring_alarm:
			return ESID_Set_ring_alarm;

		case SID_Set_ring:
			return ESID_Set_ring;
			
		case SID_Set_hit:
			return ESID_Set_hit;

		case SID_Set_Lyly_Mode_Audio:
			return ESID_Set_Lyly_Mode_Audio;
			
		case SID_Set_Lyly_Mode_Both:
			return ESID_Set_Lyly_Mode_Both;

		case SID_Set_Lyly_Mode_Video:
			return ESID_Set_Lyly_Mode_Video;

		case SID_Set_Lyly_Link1:
			return ESID_Set_Lyly_Link1;
			
		case SID_Set_Lyly_Link2:
			return ESID_Set_Lyly_Link2;

		case SID_Set_Lyly_Link3:
			return ESID_Set_Lyly_Link3;
			
		case SID_Set_Lyly_default_tip:
			return ESID_Set_Lyly_default_tip;

		case SID_Set_Lyly_record_tip:
			return ESID_Set_Lyly_record_tip;

		case SID_Set_Screen_InTime:
			return ESID_Set_Screen_InTime;
			
		case SID_Set_Screen_Lcd:
			return ESID_Set_Screen_Lcd;

		case SID_Set_Query_Del_All:
			return ESID_Set_Query_Del_All;

		case SID_Set_Tips_Fail:
			return ESID_Set_Tips_Fail;

		// IP协议转换器
		case SID_Set_binding_status:
			return ESID_Set_binding_status;

		case SID_Set_binding_code:
			return ESID_Set_binding_code;

		case SID_Set_binding_code1:
			return ESID_Set_binding_code1;
		
		case SID_Set_extension_information:
			return ESID_Set_extension_information;

		case SID_Set_ip_ptcAddr:
			return ESID_Set_ip_ptcAddr;

		case SID_Set_generate_binding_code:
			return ESID_Set_generate_binding_code;
			
		case SID_Set_binding:	
			return ESID_Set_binding;
			
		case SID_Set_get_extension:
			return ESID_Set_get_extension;
			
		case SID_Set_bounded:		
			return ESID_Set_bounded;
			
		case SID_Set_unbounded:		
			return ESID_Set_unbounded;

		case SID_Set_ExtenNo: 	
			return ESID_Set_ExtenNo;

		case SID_Set_ExtenState: 	
			return ESID_Set_ExtenState;

		case SID_Set_Online: 				
			return ESID_Set_Online;

		case SID_Set_Offline:
			return ESID_Set_Offline;

		case SID_Set_Bind_OK:
			return ESID_Set_Bind_OK;

		case SID_Set_Bind_Fail:
			return ESID_Set_Bind_Fail;
		// 家电设置文字
		case SID_Jd_Set_Light:
			return ESID_Jd_Set_Light;

		case SID_Jd_Set_Window:
			return ESID_Jd_Set_Window;
			
		case SID_Jd_Set_Kongtiao:
			return ESID_Jd_Set_Kongtiao;

		case SID_Jd_Set_Dianyuan:
			return ESID_Jd_Set_Dianyuan;
			
		case SID_Jd_Set_Gas:
			return ESID_Jd_Set_Gas;

		case SID_Jd_Set_Adrress:
			return ESID_Jd_Set_Adrress;

		case SID_Jd_Set_OpenAddr:
			return ESID_Jd_Set_OpenAddr;
			
		case SID_Jd_Set_CloseAddr:
			return ESID_Jd_Set_CloseAddr;

		case SID_Jd_Set_Dev_Name1:
			return ESID_Jd_Set_Dev_Name1;
			
		case SID_Jd_Set_Dev_Name2:
			return ESID_Jd_Set_Dev_Name2;

		case SID_Jd_Open_All:
			return ESID_Jd_Open_All;

		case SID_Jd_Close_All:
			return ESID_Jd_Close_All;
			
		case SID_Jd_Qingjing:
			return ESID_Jd_Qingjing;

		case SID_Jd_Light:
			return ESID_Jd_Light;
			
		case SID_Jd_KongTiao:
			return ESID_Jd_KongTiao;

		case SID_Jd_Dianyuan:
			return ESID_Jd_Dianyuan;

		case SID_Jd_Meiqi:
			return ESID_Jd_Meiqi;
			
		case SID_Jd_Control_Mode:
			return ESID_Jd_Control_Mode;

		case SID_Jd_Signal_Control:
			return ESID_Jd_Signal_Control;
			
		case SID_Jd_Multi_Control:
			return ESID_Jd_Multi_Control;

		case SID_Jd_Group_No:
			return ESID_Jd_Group_No;

		case SID_Jd_Dev_Set_Attr:
			return ESID_Jd_Dev_Set_Attr;
			
		case SID_Jd_Dev_Set_Adjust:
			return ESID_Jd_Dev_Set_Adjust;

		case SID_Jd_Dev_Set_UnAdjust:
			return ESID_Jd_Dev_Set_UnAdjust;
			
		case SID_Jd_Dev_Set_Light:
			return ESID_Jd_Dev_Set_Light;

		case SID_Jd_Dev_Set_Temp:
			return ESID_Jd_Dev_Set_Temp;

		case SID_Jd_Dev_Set_NoDev:
			return ESID_Jd_Dev_Set_NoDev;
			
		case SID_Jd_Dev_Name:
			return ESID_Jd_Dev_Name;

		case SID_Jd_KongTiao_Learn:
			return ESID_Jd_KongTiao_Learn;
			
		case SID_Jd_KongTiao_Cold:
			return ESID_Jd_KongTiao_Cold;

		case SID_Jd_KongTiao_Hot:
			return ESID_Jd_KongTiao_Hot;

		case SID_Jd_KongTiao_Mode:
			return ESID_Jd_KongTiao_Mode;
			
		case SID_Jd_Learn:
			return ESID_Jd_Learn;

		case SID_Jd_Test:
			return ESID_Jd_Test;
			
		case SID_Jd_KongTiao_Refre:
			return ESID_Jd_KongTiao_Refre;

		case SID_Jd_KongTiao_Warm:
			return ESID_Jd_KongTiao_Warm;

		case SID_Jd_KongTiao_Leisure:
			return ESID_Jd_KongTiao_Leisure;
			
		case SID_Jd_KongTiao_Sleep:
			return ESID_Jd_KongTiao_Sleep;

		case SID_Jd_KongTiao_Close:
			return ESID_Jd_KongTiao_Close;
			
		case SID_Jd_Button_Cold:
			return ESID_Jd_Button_Cold;

		case SID_Jd_Button_Warm:
			return ESID_Jd_Button_Warm;

		case SID_Jd_Button_Mode:
			return ESID_Jd_Button_Mode;
			
		case SID_Jd_Window_Stop:
			return ESID_Jd_Window_Stop;

		case SID_Jd_Lamp:
			return ESID_Jd_Lamp;
			
		case SID_Jd_Qingjing1:
			return ESID_Jd_Qingjing1;

		case SID_Jd_Set_Name_Err:
			return ESID_Jd_Set_Name_Err;

		case SID_Jd_Set_Adr_Err:
			return ESID_Jd_Set_Adr_Err;
			
		case SID_Jd_Set_OpAdr_Err:
			return ESID_Jd_Set_OpAdr_Err;

		case SID_Jd_Set_ClAdr_Err:
			return ESID_Jd_Set_ClAdr_Err;
			
		case SID_Jd_Set_Dev_Failed:
			return ESID_Jd_Set_Dev_Failed;

		case SID_Jd_Set_Dev_Noname:
			return ESID_Jd_Set_Dev_Noname;

		case SID_Jd_Adress_Set_Warning:
			return ESID_Jd_Adress_Set_Warning;
			
		case SID_Jd_Set_Scene_Noname:
			return ESID_Jd_Set_Scene_Noname;

		case SID_Jd_Set_KtLearn_Err:
			return ESID_Jd_Set_KtLearn_Err;
			
		case SID_Jd_Set_KtLearn_Ok:
			return ESID_Jd_Set_KtLearn_Ok;

		case SID_Jd_Set_Adr_Wrong:
			return ESID_Jd_Set_Adr_Wrong;

		case SID_Jd_Sence_Adr:
			return ESID_Jd_Sence_Adr;
			
		case SID_Jd_Aurine_Adress_Set_Warning:
			return ESID_Jd_Aurine_Adress_Set_Warning;

		case SID_Jd_Kongtiao_Learning:
			return ESID_Jd_Kongtiao_Learning;
			
		case SID_Jd_Set_Scene:
			return ESID_Jd_Set_Scene;

		case SID_Jd_Mode_Huike:
			return ESID_Jd_Mode_Huike;

		case SID_Jd_Mode_Jiucan:
			return ESID_Jd_Mode_Jiucan;
			
		case SID_Jd_Mode_Yejian:
			return ESID_Jd_Mode_Yejian;

		case SID_Jd_Mode_Jiedian:
			return ESID_Jd_Mode_Jiedian;
			
		case SID_Jd_Mode_Putong:
			return ESID_Jd_Mode_Putong;

		case SID_Jd_Mode_Self:
			return ESID_Jd_Mode_Self;

		case SID_Jd_Mode_Set_Name1:
			return ESID_Jd_Mode_Set_Name1;
			
		case SID_Jd_Mode_Set_Name2:
			return ESID_Jd_Mode_Set_Name2;

		case SID_Jd_Mode_Out:
			return ESID_Jd_Mode_Out;
			
		case SID_Jd_Mode_Cook:
			return ESID_Jd_Mode_Cook;

		case SID_Jd_Mode_first:
			return ESID_Jd_Mode_first;

		case SID_Jd_Mode_Afater:
			return ESID_Jd_Mode_Afater;
			
		case SID_Jd_Mode_Night:
			return ESID_Jd_Mode_Night;

		case SID_Jd_Mode_Bed:
			return ESID_Jd_Mode_Bed;
			
		case SID_Jd_Mode_QiYe:
			return ESID_Jd_Mode_QiYe;

		case SID_Jd_Mode_Up:
			return ESID_Jd_Mode_Up;

		case SID_Jd_Mode_Muyu:
			return ESID_Jd_Mode_Muyu;
			
		case SID_Jd_Mode_Tea:
			return ESID_Jd_Mode_Tea;

		case SID_Jd_Mode_Drink:
			return ESID_Jd_Mode_Drink;
			
		case SID_Jd_Mode_Read:
			return ESID_Jd_Mode_Read;

		case SID_Jd_Mode_Work:
			return ESID_Jd_Mode_Work;

		case SID_Jd_Mode_Juhui:
			return ESID_Jd_Mode_Juhui;
			
		case SID_Jd_Mode_Play:
			return ESID_Jd_Mode_Play;

		case SID_Jd_Mode_Film:
			return ESID_Jd_Mode_Film;
			
		case SID_Jd_Mode_Sing:
			return ESID_Jd_Mode_Sing;

		case SID_Jd_Mode_Storang:
			return ESID_Jd_Mode_Storang;

		case SID_Jd_Mode_Wenxi:
			return ESID_Jd_Mode_Wenxi;
			
		case SID_Jd_Mode_Langman:
			return ESID_Jd_Mode_Langman;

		case SID_Jd_Mode_Jiqing:
			return ESID_Jd_Mode_Jiqing;
			
		case SID_Jd_Mode_Dianya:
			return ESID_Jd_Mode_Dianya;

		case SID_Jd_Mode_Chlid:
			return ESID_Jd_Mode_Chlid;

		case SID_Jd_Mode_Chenwen:
			return ESID_Jd_Mode_Chenwen;
			
		case SID_Jd_Mode_Think:
			return ESID_Jd_Mode_Think;

		case SID_Jd_Mode_Cool:
			return ESID_Jd_Mode_Cool;
			
		case SID_Jd_Mode_Warm:
			return ESID_Jd_Mode_Warm;

		case SID_Jd_Mode_Mode:
			return ESID_Jd_Mode_Mode;

		case SID_Jd_Mode_Sence:
			return ESID_Jd_Mode_Sence;
			
		case SID_Jd_Mode_Qingdiao:
			return ESID_Jd_Mode_Qingdiao;

		case SID_Jd_Mode_Fenwei:
			return ESID_Jd_Mode_Fenwei;
			
		case SID_Jd_Mode_Time:
			return ESID_Jd_Mode_Time;

		case SID_Jd_Mode_Changjing:
			return ESID_Jd_Mode_Changjing;

		case SID_Jd_LouShang:
			return ESID_Jd_LouShang;
			
		case SID_Jd_LouXia:
			return ESID_Jd_LouXia;

		case SID_Jd_WuNei:
			return ESID_Jd_WuNei;
			
		case SID_Jd_WuWai:
			return ESID_Jd_WuWai;

		case SID_Jd_TianTai:
			return ESID_Jd_TianTai;

		case SID_Jd_GeLou:
			return ESID_Jd_GeLou;
			
		case SID_Jd_HuaYuan:
			return ESID_Jd_HuaYuan;

		case SID_Jd_DiJiao:
			return ESID_Jd_DiJiao;
			
		case SID_Jd_TianJin:
			return ESID_Jd_TianJin;

		case SID_Jd_WuYan:
			return ESID_Jd_WuYan;

		case SID_Jd_QianTing:
			return ESID_Jd_QianTing;
			
		case SID_Jd_HouYuan:
			return ESID_Jd_HouYuan;

		case SID_Jd_DianTiJian:
			return ESID_Jd_DianTiJian;
			
		case SID_Jd_TingCheFang:
			return ESID_Jd_TingCheFang;

		case SID_Jd_DaMen:
			return ESID_Jd_DaMen;

		case SID_Jd_QianMen:
			return ESID_Jd_QianMen;
			
		case SID_Jd_ZhongMen:
			return ESID_Jd_ZhongMen;

		case SID_Jd_PangMen:
			return ESID_Jd_PangMen;
			
		case SID_Jd_HouMen:
			return ESID_Jd_HouMen;

		case SID_Jd_CeMen:
			return ESID_Jd_CeMen;

		case SID_Jd_BianMen:
			return ESID_Jd_BianMen;
			
		case SID_Jd_HenMen:
			return ESID_Jd_HenMen;

		case SID_Jd_YanTai:
			return ESID_Jd_YanTai;
			
		case SID_Jd_Zhoulan:
			return ESID_Jd_Zhoulan;

		case SID_Jd_GuoDao:
			return ESID_Jd_GuoDao;

		case SID_Jd_LouTi:
			return ESID_Jd_LouTi;
			
		case SID_Jd_TiKou:
			return ESID_Jd_TiKou;

		case SID_Jd_TiJian:
			return ESID_Jd_TiJian;
			
		case SID_Jd_TiXia:
			return ESID_Jd_TiXia;

		case SID_Jd_KeTing:
			return ESID_Jd_KeTing;

		case SID_Jd_DaTing:
			return ESID_Jd_DaTing;
			
		case SID_Jd_ZhuTing:
			return ESID_Jd_ZhuTing;

		case SID_Jd_CanTing:
			return ESID_Jd_CanTing;
			
		case SID_Jd_FanTing:
			return ESID_Jd_FanTing;

		case SID_Jd_MenTing:
			return ESID_Jd_MenTing;

		case SID_Jd_ZhuWoShi:
			return ESID_Jd_ZhuWoShi;
			
		case SID_Jd_ZhuRenFang:
			return ESID_Jd_ZhuRenFang;

		case SID_Jd_WoShi:
			return ESID_Jd_WoShi;
			
		case SID_Jd_ShuiFang:
			return ESID_Jd_ShuiFang;

		case SID_Jd_FangJianA:
			return ESID_Jd_FangJianA;

		case SID_Jd_FangJianB:
			return ESID_Jd_FangJianB;
			
		case SID_Jd_FangJianC:
			return ESID_Jd_FangJianC;

		case SID_Jd_FangJianD:
			return ESID_Jd_FangJianD;
			
		case SID_Jd_FangJianE:
			return ESID_Jd_FangJianE;

		case SID_Jd_ChuFang:
			return ESID_Jd_ChuFang;

		case SID_Jd_ShuFang:
			return ESID_Jd_ShuFang;
			
		case SID_Jd_KeFang:
			return ESID_Jd_KeFang;

		case SID_Jd_ZhuYuShi:
			return ESID_Jd_ZhuYuShi;
			
		case SID_Jd_YuShi:
			return ESID_Jd_YuShi;

		case SID_Jd_ErTongFang:
			return ESID_Jd_ErTongFang;

		case SID_Jd_GongRenFang:
			return ESID_Jd_GongRenFang;
			
		case SID_Jd_DuoYongTuJian:
			return ESID_Jd_DuoYongTuJian;

		case SID_Jd_QiJuShi:
			return ESID_Jd_QiJuShi;
			
		case SID_Jd_YouLeShi:
			return ESID_Jd_YouLeShi;

		case SID_Jd_ShuXiShi:
			return ESID_Jd_ShuXiShi;

		case SID_Jd_WeiShengJian:
			return ESID_Jd_WeiShengJian;
			
		case SID_Jd_XieMaoJian:
			return ESID_Jd_XieMaoJian;

		case SID_Jd_GongZuoJian:
			return ESID_Jd_GongZuoJian;
			
		case SID_Jd_DiXiaShi:
			return ESID_Jd_DiXiaShi;

		case SID_Jd_None:
			return ESID_Jd_None;

		case SID_Jd_Deng:
			return ESID_Jd_Deng;
			
		case SID_Jd_TiaoGuangDeng:
			return ESID_Jd_TiaoGuangDeng;

		case SID_Jd_BaiChiDeng:
			return ESID_Jd_BaiChiDeng;
			
		case SID_Jd_GuangGuan:
			return ESID_Jd_GuangGuan;

		case SID_Jd_DiaoDeng:
			return ESID_Jd_DiaoDeng;

		case SID_Jd_HuaDeng:
			return ESID_Jd_HuaDeng;
			
		case SID_Jd_BiDeng:
			return ESID_Jd_BiDeng;

		case SID_Jd_SheDeng:
			return ESID_Jd_SheDeng;
			
		case SID_Jd_TaiDeng:
			return ESID_Jd_TaiDeng;

		case SID_Jd_LouDiDeng:
			return ESID_Jd_LouDiDeng;

		case SID_Jd_ZhaoMingDeng:
			return ESID_Jd_ZhaoMingDeng;
			
		case SID_Jd_ShiYingDeng:
			return ESID_Jd_ShiYingDeng;

		case SID_Jd_YeDeng:
			return ESID_Jd_YeDeng;
			
		case SID_Jd_ZhuanshiDEng:
			return ESID_Jd_ZhuanshiDEng;

		case SID_Jd_DianZhiSuo:
			return ESID_Jd_DianZhiSuo;

		case SID_Jd_XiaoduWanGui:
			return ESID_Jd_XiaoduWanGui;
			
		case SID_Jd_KongQiJinHua:
			return ESID_Jd_KongQiJinHua;

		case SID_Jd_YinShuiJi:
			return ESID_Jd_YinShuiJi;

		case SID_Jd_KongTiaoJi:
			return ESID_Jd_KongTiaoJi;
			
		case SID_Jd_LenQiJi:
			return ESID_Jd_LenQiJi;

		case SID_Jd_CouYouJi:
			return ESID_Jd_CouYouJi;

		case SID_Jd_NuanFenJi:
			return ESID_Jd_NuanFenJi;
			
		case SID_Jd_DianReTan:
			return ESID_Jd_DianReTan;
			
		case SID_Jd_ReShuiLu:
			return ESID_Jd_ReShuiLu;

		case SID_Jd_DianFanBao:
			return ESID_Jd_DianFanBao;

		case SID_Jd_DianZhiWaGuo:
			return ESID_Jd_DianZhiWaGuo;
			
		case SID_Jd_WeiBoLu:
			return ESID_Jd_WeiBoLu;

		case SID_Jd_XiYiJi:
			return ESID_Jd_XiYiJi;

		case SID_Jd_DianShiJi:
			return ESID_Jd_DianShiJi;
			
		case SID_Jd_LuXiangJi:
			return ESID_Jd_LuXiangJi;

		case SID_Jd_YingXiang:
			return ESID_Jd_YingXiang;

		case SID_Jd_DianNao:
			return ESID_Jd_DianNao;
			
		case SID_Jd_FengShan:
			return ESID_Jd_FengShan;
			
		case SID_Jd_CD:
			return ESID_Jd_CD;

		case SID_Jd_LD:
			return ESID_Jd_LD;

		case SID_Jd_VCD:
			return ESID_Jd_VCD;
			
		case SID_Jd_DVD:
			return ESID_Jd_DVD;

		case SID_Jd_ChaZuo1:
			return ESID_Jd_ChaZuo1;
			
		case SID_Jd_ChaZuo2:
			return ESID_Jd_ChaZuo2;

		case SID_Jd_ChaZuo3:
			return ESID_Jd_ChaZuo3;

		case SID_Jd_ChaZuo4:
			return ESID_Jd_ChaZuo4;
			
		case SID_Jd_ChaZuo5:
			return ESID_Jd_ChaZuo5;

		case SID_Jd_XiaoDuSheBe:
			return ESID_Jd_XiaoDuSheBe;
			
		case SID_Jd_SettingMusic:
			return ESID_Jd_SettingMusic;

		case SID_Jd_ChuangLian:
			return ESID_Jd_ChuangLian;

		case SID_Jd_JuanLian:
			return ESID_Jd_JuanLian;
			
		case SID_Jd_DianDongBuLian:
			return ESID_Jd_DianDongBuLian;

		case SID_Jd_DianDongShaLian:
			return ESID_Jd_DianDongShaLian;
			
		case SID_Jd_SceneNameSet:
			return ESID_Jd_SceneNameSet;

		case SID_Jd_SceneActionSet:
			return ESID_Jd_SceneActionSet;

		#ifdef _AIR_VOLUME_MODE_
		case SID_Jd_Air_Volume:
			return ESID_Jd_Air_Volume;

		case SID_Jd_Low_Wind:
			return ESID_Jd_Low_Wind;
			
		case SID_Jd_Medium_Wind:
			return ESID_Jd_Medium_Wind;

		case SID_Jd_Hight_Wind:
			return ESID_Jd_Hight_Wind;
			
		case SID_Jd_Automatic:
			return ESID_Jd_Automatic;
		#endif

		// 对讲类
		case SID_MainCall_User:
			return ESID_MainCall_User;
			
		case SID_MainCall_FengJi:
			return ESID_MainCall_FengJi;

		case SID_MainCenter_1:
			return ESID_MainCenter_1;

		case SID_MainCenter_2:
			return ESID_MainCenter_2;
			
		case SID_MainCenter_3:
			return ESID_MainCenter_3;

		case SID_MainMonitor_Door:
			return ESID_MainMonitor_Door;
			
		case SID_MainMonitor_Stair:
			return ESID_MainMonitor_Stair;

		case SID_MainMonitor_Area:
			return ESID_MainMonitor_Area;

		case SID_MainMonitor_Camera:
			return ESID_MainMonitor_Camera;
		case SID_MainMonitor_Home:
			return ESID_MainMonitor_Room;
			
		case SID_DevManager:
			return ESID_DevManager;

		case SID_DevArea:
			return ESID_DevArea;
			
		case SID_DevStair:
			return ESID_DevStair;

		case SID_DevRoom:
			return ESID_DevRoom;

		case SID_DevPhone:
			return ESID_DevPhone;
			
		case SID_DevDoorPhone:
			return ESID_DevDoorPhone;

		case SID_DevDoorNet:
			return ESID_DevDoorNet;
			
		case SID_DevGateWay:
			return ESID_DevGateWay;

		case SID_DevIPCamera:
			return ESID_DevIPCamera;

		case SID_DevFenJi:
			return ESID_DevFenJi;
			
		case SID_DevMaster:
			return ESID_DevMaster;

		case SID_RecordMiss:
			return ESID_RecordMiss;
			
		case SID_RecordIn:
			return ESID_RecordIn;

		case SID_RecordOut:
			return ESID_RecordOut;

		case SID_Right_Full:
			return ESID_Right_Full;
			
		case SID_Inter_HangUP:
			return ESID_Inter_HangUP;

		case SID_Inter_Answer:
			return ESID_Inter_Answer;
			
		case SID_Inter_Unlock:
			return ESID_Inter_Unlock;

		case SID_Inter_ReDial:
			return ESID_Inter_ReDial;

		case SID_Right_Call:
			return ESID_Right_Call;
			
		case SID_Inter_Capture:
			return ESID_Inter_Capture;

		case SID_Inter_Volume:
			return ESID_Inter_Volume;
			
		case SID_Inter_Next:
			return ESID_Inter_Next;

		case SID_Inter_Talk:
			return ESID_Inter_Talk;

		case SID_Inter_GetList:
			return ESID_Inter_GetList;
			
		case SID_Inter_Snap:
			return ESID_Inter_Snap;

		case SID_Inter_AudioAdd:
			return ESID_Inter_AudioAdd;
			
		case SID_Inter_AudioDel:
			return ESID_Inter_AudioDel;

		case SID_Inter_StopMonitor:
			return ESID_Inter_StopMonitor;

		case SID_Input_OK:
			return ESID_Input_OK;
			
		case SID_Inter_EnterUserNo:
			return ESID_Inter_EnterUserNo;

		case SID_Inter_EnterFJNo:
			return ESID_Inter_EnterFJNo;
			
		case SID_Inter_WrongNo:
			return ESID_Inter_WrongNo;

		case SID_Inter_NoNotFound:
			return ESID_Inter_NoNotFound;

		case SID_Inter_Connecting:
			return ESID_Inter_Connecting;
			
		case SID_Inter_BeCalling:
			return ESID_Inter_BeCalling;

		case SID_Inter_Calling:
			return ESID_Inter_Calling;
			
		case SID_Inter_Talking:
			return ESID_Inter_Talking;

		case SID_Inter_RecordHinting:
			return ESID_Inter_RecordHinting;

		case SID_Inter_Recording:
			return ESID_Inter_Recording;
			
		case SID_Inter_UnlockSuccess:
			return ESID_Inter_UnlockSuccess;

		case SID_Inter_UnlockFailed:
			return ESID_Inter_UnlockFailed;
			
		case SID_Inter_CaptureSuccess:
			return ESID_Inter_CaptureSuccess;

		case SID_Inter_CaptureFailed:
			return ESID_Inter_CaptureFailed;

		case SID_Inter_ManagerNoSet:
			return ESID_Inter_ManagerNoSet;
			
		case SID_Inter_Search:
			return ESID_Inter_Search;

		case SID_Inter_Monitor:
			return ESID_Inter_Monitor;
			
		case SID_Inter_busy:
			return ESID_Inter_busy;

		case SID_Inter_ConnFailed:
			return ESID_Inter_ConnFailed;

		case SID_Inter_SearchFailed:
			return ESID_Inter_SearchFailed;
			
		case SID_Inter_ElevAuthorize:
			return ESID_Inter_ElevAuthorize;

		case SID_Inter_ElevAuthOK:
			return ESID_Inter_ElevAuthOK;
			
		case SID_Inter_ElevAuthFailed:
			return ESID_Inter_ElevAuthFailed;

		case SID_Auto_Capture:
			return ESID_Auto_Capture;

		// 信息文字	
		case SID_Info_Sms:
			return ESID_Info_Sms;
			
		case SID_Info_Centre_Des:
			return ESID_Info_Centre_Des;

       // 留言文字
		case SID_Media_Lyly:
			return ESID_Media_Lyly;
			
		case SID_Media_Snap:
			return ESID_Media_Snap;

		case SID_Media_Num:
			return ESID_Media_Num;

		case SID_Media_Z:
			return ESID_Media_Z;

        // 便利文字	
 		case SID_Bl_Jrly:
			return ESID_Bl_Jrly;

		case SID_Bl_ClearScreen:
			return ESID_Bl_ClearScreen; 

    	// 家电文字	
		case SID_Jd_Mode_Unused:
			return ESID_Jd_Mode_Unused;
			
		case SID_Jd_Factory_Set:
			return ESID_Jd_Factory_Set;

		case SID_Jd_Factory:
			return ESID_Jd_Factory;

		case SID_Jd_Factory_X10:
			return ESID_Jd_Factory_X10;
			
		case SID_Jd_Factory_Suobo:
			return ESID_Jd_Factory_Suobo;

		case SID_Jd_Factory_ACBUS:
			return ESID_Jd_Factory_ACBUS;

       // 报警文字
		case SID_Bj_SOS:
			return ESID_Bj_SOS;
			
		case SID_Bj_Fire:
			return ESID_Bj_Fire;

		case SID_Bj_Gas:
			return ESID_Bj_Gas;

		case SID_Bj_Door:
			return ESID_Bj_Door;
			
		case SID_Bj_Room:
			return ESID_Bj_Room;
			
		case SID_Bj_Big_Window:
			return ESID_Bj_Big_Window;

		case SID_Bj_Window:
			return ESID_Bj_Window;

		case SID_Bj_Balcony:
			return ESID_Bj_Balcony;
			
		case SID_Bj_Bf_Home:
			return ESID_Bj_Bf_Home;

		case SID_Bj_Bf_Out:
			return ESID_Bj_Bf_Out;
			
		case SID_Bj_Bf_Night:
			return ESID_Bj_Bf_Night;

		case SID_Bj_Clean:
			return ESID_Bj_Clean;

		case SID_Bj_Baojing_Record:
			return ESID_Bj_Baojing_Record;
			
		case SID_Bj_Caozuo_Rec:
			return ESID_Bj_Caozuo_Rec;

		case SID_Bj_Alarm_Rec_Clear_F:
			return ESID_Bj_Alarm_Rec_Clear_F;
			
		case SID_Bj_Fangqu1:
			return ESID_Bj_Fangqu1;

		case SID_Bj_Fangqu2:
			return ESID_Bj_Fangqu2;

		case SID_Bj_Fangqu3:
			return ESID_Bj_Fangqu3;
			
		case SID_Bj_Fangqu4:
			return ESID_Bj_Fangqu4;

		case SID_Bj_Fangqu5:
			return ESID_Bj_Fangqu5;

		case SID_Bj_Fangqu6:
			return ESID_Bj_Fangqu6;

		case SID_Bj_Fangqu7:
			return ESID_Bj_Fangqu7;


		case SID_Bj_Fangqu8:
			return ESID_Bj_Fangqu8;

		case SID_Bj_Fangqu:
			return ESID_Bj_Fangqu;

		case SID_Bj_Set_Err:
			return ESID_Bj_Set_Err;
			
		case SID_Bj_Fangqu_Settings:
			return ESID_Bj_Fangqu_Settings;

		case SID_Bj_Security_Settings:
			return ESID_Bj_Security_Settings;

		case SID_Bj_Security_Parameter:
			return ESID_Bj_Security_Parameter;
			
		case SID_Bj_Tantou_Type:
			return ESID_Bj_Tantou_Type;

		case SID_Bj_24Hour_Fangqu:
			return ESID_Bj_24Hour_Fangqu;
			
		case SID_Bj_Normally_Open:
			return ESID_Bj_Normally_Open;

		case SID_Bj_Normally_Close:
			return ESID_Bj_Normally_Close;

		case SID_Bj_Alarm_Delay:
			return ESID_Bj_Alarm_Delay;
			
		case SID_Bj_None:
			return ESID_Bj_None;

		case SID_Bj_Picture:
			return ESID_Bj_Picture;
			
		case SID_Bj_Voice:
			return ESID_Bj_Voice;

		case SID_Bj_Enable:
			return ESID_Bj_Enable;

		case SID_Disable:
			return ESID_Disable;
			
		case SID_Bj_30Seconds:
			return ESID_Bj_30Seconds;

		case SID_Bj_1Minute:
			return ESID_Bj_1Minute;
			
		case SID_Bj_2Minutes:
			return ESID_Bj_2Minutes;

		case SID_Bj_3Minutes:
			return ESID_Bj_3Minutes;

		case SID_Bj_4Minutes:
			return ESID_Bj_4Minutes;
			
		case SID_Bj_5Minutes:
			return ESID_Bj_5Minutes;

		case SID_Bj_10Minutes:
			return ESID_Bj_10Minutes;
			
		case SID_Bj_15Minutes:
			return ESID_Bj_15Minutes;

		case SID_Bj_Alarm_Time:
			return ESID_Bj_Alarm_Time;

		case SID_Bj_Early_Warning_Time:
			return ESID_Bj_Early_Warning_Time;
			
		case SID_Bj_Early_Warning_Voice:
			return ESID_Bj_Early_Warning_Voice;	

		case SID_Bj_Remote_sz:
			return ESID_Bj_Remote_sz;
			
		case SID_Bj_Remote_bf:
			return ESID_Bj_Remote_bf;

		case SID_Bj_Remote_cf:
			return ESID_Bj_Remote_cf;

		case SID_Bj_Remote_jf:
			return ESID_Bj_Remote_jf;
			
		case SID_Bj_Already_Bufang:
			return ESID_Bj_Already_Bufang;

		case SID_Bj_Report_Type_Chufa:
			return ESID_Bj_Report_Type_Chufa;
			
		case SID_Bj_Report_Type_Qiuzhu:
			return ESID_Bj_Report_Type_Qiuzhu;

		case SID_Bj_Alarm:
			return ESID_Bj_Alarm;

		case SID_Bj_Report_Type_Jinru:
			return ESID_Bj_Report_Type_Jinru;
			
		case SID_Bj_Fangquchufa:
			return ESID_Bj_Fangquchufa;

		case SID_Bj_Bengjikongzhi:
			return ESID_Bj_Bengjikongzhi;
			
		case SID_Bj_Fengjikongzhi:
			return ESID_Bj_Fengjikongzhi;

		case SID_Bj_Query_Del_Rec_One:
			return ESID_Bj_Query_Del_Rec_One;

		case SID_Bj_Query_Del_Rec_All:
			return ESID_Bj_Query_Del_Rec_All;
			
		case SID_Bj_Chufa:
			return ESID_Bj_Chufa;

		case SID_Bj_Cannot_Bufang:
			return ESID_Bj_Cannot_Bufang;
			
		case SID_InputBox_Psw_Err:
			return ESID_InputBox_Psw_Err;

		case SID_Isolation:
			return ESID_Isolation;

		case SID_AfJoint_Tk_Chefang:
			return ESID_AfJoint_Tk_Chefang;
			
		case SID_AfJoint_Mk_Chefang:
			return ESID_AfJoint_Mk_Chefang;

		case SID_AfJoint_DoorRing:
			return ESID_AfJoint_DoorRing;
			
		case SID_AfJoint_ScenBf:
			return ESID_AfJoint_ScenBf;

		case SID_AfJoint_ScenCf:
			return ESID_AfJoint_ScenCf;

		case SID_Af_Tel_Num1:
			return ESID_Af_Tel_Num1;
			
		case SID_Af_Tel_Num2:
			return ESID_Af_Tel_Num2;

		case SID_Af_Msg_Num1:
			return ESID_Af_Msg_Num1;
			
		case SID_Af_Msg_Num2:
			return ESID_Af_Msg_Num2;

		case SID_Af_Mode_Unused:
			return ESID_Af_Mode_Unused;

		case SID_Af_Alarm_Cannot_Bf:
			return ESID_Af_Alarm_Cannot_Bf;
			
		case SID_Af_Touch_Cannot_Bf:
			return ESID_Af_Touch_Cannot_Bf;
			
		case SID_Af_Alarm_Cannot_Qc:
			return ESID_Af_Alarm_Cannot_Qc;
			
        // 预约文字
		case SID_Jd_Yuyue:
			return ESID_Jd_Yuyue;
			
		case SID_Jd_Yuyue_Manager:
			return ESID_Jd_Yuyue_Manager;

		case SID_Jd_Yuyue_All:
			return ESID_Jd_Yuyue_All;

		case SID_Jd_Yuyue_DelOne:
			return ESID_Jd_Yuyue_DelOne;
			
		case SID_Jd_Yuyue_Add_Err:
			return ESID_Jd_Yuyue_Add_Err;

		case SID_Jd_Yuyue_EveryDay:
			return ESID_Jd_Yuyue_EveryDay;
			
		case SID_Jd_Yuyue_EveryWeek:
			return ESID_Jd_Yuyue_EveryWeek;

		case SID_Jd_Yuyue_EveryMonth:
			return ESID_Jd_Yuyue_EveryMonth;

		case SID_Jd_Yuyue_Day:
			return ESID_Jd_Yuyue_Day;
			
		case SID_Jd_Yuyue_Action_Open:
			return ESID_Jd_Yuyue_Action_Open;

		case SID_Jd_Yuyue_Action_Close:
			return ESID_Jd_Yuyue_Action_Close;
			
		case SID_Jd_Yuyue_Type:
			return ESID_Jd_Yuyue_Type;

		case SID_Jd_Yuyue_Name:
			return ESID_Jd_Yuyue_Name;

		case SID_Jd_Yuyue_Status:
			return ESID_Jd_Yuyue_Status;
			
		case SID_Jd_Yuyue_Repeat:
			return ESID_Jd_Yuyue_Repeat;

		case SID_Jd_Yuyue_StartTime:
			return ESID_Jd_Yuyue_StartTime;

     	// 社区监视文字	
		case SID_Rtsp_IPC:
			return ESID_Rtsp_IPC;
			
		case SID_Rtsp_Name:
			return ESID_Rtsp_Name;

		case SID_Rtsp_Port:
			return ESID_Rtsp_Port;

		case SID_Rtsp_Channel:
			return ESID_Rtsp_Channel;
			
		case SID_Rtsp_User:
			return ESID_Rtsp_User;

		case SID_Rtsp_Password:
			return ESID_Rtsp_Password;
			
		case SID_Rtsp_IP:
			return ESID_Rtsp_IP;

		case SID_Rtsp_Factory:
			return ESID_Rtsp_Factory;

		// 家居监视文字字符ID
		case SID_Rtsp_Indoor_Camera:
			return ESID_Rtsp_Indoor_Camera;

		case SID_Rtsp_Enable_Indoor_Camera:
			return ESID_Rtsp_Enable_Indoor_Camera;

		case SID_Rtsp_PTZ:
			return ESID_Rtsp_PTZ;
			
		case SID_Rtsp_Direc:
			return ESID_Rtsp_Direc;
		
		case SID_Rtsp_Scaling:
			return ESID_Rtsp_Scaling;
		
		case SID_Rtsp_Point:
			return ESID_Rtsp_Point;

		case SID_Rtsp_Point1:
	 		return ESID_Rtsp_Point1;

 		case SID_Rtsp_Point2:
	 		return ESID_Rtsp_Point2;

 		case SID_Rtsp_Point3:
			return ESID_Rtsp_Point3;
		
		case SID_Rtsp_Point4:
			return ESID_Rtsp_Point4;

		//蓝牙门前机文字字符ID
		#ifdef _ADD_BLUETOOTH_DOOR_
		case SID_Ble_Door_Unused1:
			return ESID_Ble_Door_Unused1;

		case SID_Ble_Door_Unused2:
			return ESID_Ble_Door_Unused2;
			
		case SID_Ble_Door_First_Record:
			return ESID_Ble_Door_First_Record;

		case SID_Ble_Door_Second_Record:
			return ESID_Ble_Door_Second_Record;
			
		case SID_Ble_Door_Operate_Record:
			return ESID_Ble_Door_Operate_Record;

		case SID_Ble_Door_Lock_Record:
			return ESID_Ble_Door_Lock_Record;  

		case SID_Ble_Door_24H_Record:
			return ESID_Ble_Door_24H_Record;  

		case SID_Ble_Door_History_Record:
			return ESID_Ble_Door_History_Record;  

		case SID_Ble_Door_Snap_Record:
			return ESID_Ble_Door_Snap_Record;

		case SID_Ble_Door_First:
			return ESID_Ble_Door_First;

		case SID_Ble_Door_Second:
			return ESID_Ble_Door_Second;

		case SID_Set_Unused:
			return ESID_Set_Unused;   

		case SID_Ble_Door_Obtain:
			return ESID_Ble_Door_Obtain;

		case SID_Ble_Door_Obtain_Err:
			return ESID_Ble_Door_Obtain_Err;

		case SID_Ble_Door_Lock_Key:
			return ESID_Ble_Door_Lock_Key;

		case SID_Ble_Door_Lock_Pwd:
			return ESID_Ble_Door_Lock_Pwd;

		case SID_Ble_Door_Lock_Card:
			return ESID_Ble_Door_Lock_Card;

		case SID_Ble_Door_Lock_Finger:
			return ESID_Ble_Door_Lock_Finger;

		case SID_Ble_Door_Lock_Face:
			return ESID_Ble_Door_Lock_Face;

		case SID_Ble_Door_Lock_Guest:
			return ESID_Ble_Door_Lock_Guest;

		case SID_Ble_Door_Lock_Monitor:
			return ESID_Ble_Door_Lock_Monitor;

		case SID_Ble_Door_Lock_Photo:
			return ESID_Ble_Door_Lock_Photo;

		case SID_Ble_Door_Lock_Linshi:
			return ESID_Ble_Door_Lock_Linshi;

		case SID_Ble_Door_Lock_Custom:
			return ESID_Ble_Door_Lock_Custom;

		case SID_Ble_Door_Operate_Del:
			return ESID_Ble_Door_Operate_Del;

		case SID_Ble_Door_Operate_Open:
			return ESID_Ble_Door_Operate_Open;

		case SID_Ble_Door_Operate_Unopen:
			return ESID_Ble_Door_Operate_Unopen;

		case SID_Ble_Door_Operate_Ring:
			return ESID_Ble_Door_Operate_Ring;

		case SID_Ble_Door_Dev_Host:
			return ESID_Ble_Door_Dev_Host;

		case SID_Ble_Door_Dev_Door:
			return ESID_Ble_Door_Dev_Door;

		case SID_Ble_Door_Dev_Photo:
			return ESID_Ble_Door_Dev_Photo;

		case SID_Ble_Door_ManInduction:
			return ESID_Ble_Door_ManInduction;

		case SID_Ble_Door_ThreeErr:
			return ESID_Ble_Door_ThreeErr;

		case SID_Ble_Door_TamperAlarm:
			return ESID_Ble_Door_TamperAlarm;

		case SID_Ble_Door_Visitor:
			return ESID_Ble_Door_Visitor;

		case SID_Ble_Door1_Snap_Record:
			return ESID_Ble_Door1_Snap_Record;

		case SID_Ble_Door2_Snap_Record:
			return ESID_Ble_Door2_Snap_Record;

		case SID_Ble_Door1_Lock24h_Record:
			return ESID_Ble_Door1_Lock24h_Record;

		case SID_Ble_Door2_Lock24h_Record:
			return ESID_Ble_Door2_Lock24h_Record;

		case SID_Ble_Door1_LockHis_Record:
			return ESID_Ble_Door1_LockHis_Record;

		case SID_Ble_Door2_LockHis_Record:
			return ESID_Ble_Door2_LockHis_Record;

		case SID_Ble_Door1_Operate24h_Record:
			return ESID_Ble_Door1_Operate24h_Record;

		case SID_Ble_Door2_Operate24h_Record:
			return ESID_Ble_Door2_Operate24h_Record;

		case SID_Ble_Door1_OperateHis_Record:
			return ESID_Ble_Door1_OperateHis_Record;

		case SID_Ble_Door2_OperateHis_Record:
			return ESID_Ble_Door2_OperateHis_Record;

		case SID_Ble_Door_Room_List:
			return ESID_Ble_Door_Room_List;

		case SID_Ble_Door_DNS_Address:
			return ESID_Ble_Door_DNS_Address;
		#endif
	
		default:
			return ESID_NULL;
	}	
}

