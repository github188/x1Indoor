/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_alarm.c
  Author:   	wufn
  Version:   	2.0
  Date: 		16-5-12
  Description:  
  				报警界面
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/

#include "gui_include.h"
#include "storage_include.h"
#include "logic_include.h"

#ifdef _TFT_7_
#define EIGHTAREA_CTRL_W		640
#define EIGHTAREA_CTRL_H		480
#else
#define EIGHTAREA_CTRL_W		380
#define EIGHTAREA_CTRL_H		272
#endif
#define EIGHTAREA_CTRL_XPOS 	0
#define EIGHTAREA_CTRL_YPOS 	0

CREATE_FORM(FrmAlarm);
 
static HWND   g_rightwin = 0;
static HWND   g_8AreasCtrlWin = 0;
static uint8  g_is_show_msg = 0;					// 显示对话框中

RIGHTCTRL_INFO g_rightCtrl;
EIGHTAREACTRL_INFO g_eightCtrl;

/*************************************************
  Function:		ui_reflash
  Description: 	刷新屏幕
  Input:	
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash(HWND hDlg)
{
	InvalidateRect(hDlg, NULL, FALSE);
}

/*************************************************
  Function:		get_eightarae_info
  Description: 	获取八防区状态信息
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_eightarae_info(HWND hDlg)
{
	uint8 i = 0;
	uint8 is24hour;
	uint8 no24clear;					// 非24小时防区是否清除警戒标志
	uint8 no24touch;					// 非24小时防区触发标志
	//状态与图标索引的对应
    uint8 state_to_bmpoder[6] = {0,3,4,5,2,1};	
	uint16 BMP_ID[8] = {BID_BJ_Urgency0, BID_BJ_Fire0, BID_BJ_Gas0, BID_BJ_Door0, 
						BID_BJ_Room0, BID_BJ_Window0, BID_BJ_Window0,  BID_BJ_Balcony0};
	AF_FLASH_DATA af_data;
	logic_get_alarm_param((uint8 *)&af_data);
	is24hour = af_data.is_24_hour;
	no24clear = alarm_get_24clear_param();
	no24touch = alarm_get_no24touch_param();
	memset(&g_eightCtrl, 0, sizeof(EIGHTAREACTRL_INFO));
	g_eightCtrl.Index = 0;
	g_eightCtrl.State = AREA_KEYUP;
	g_eightCtrl.MaxNum = 8;

	for (i = 0; i < 8; i++)
	{
		g_eightCtrl.Areas[i].Enabled = TRUE;
		g_eightCtrl.Areas[i].TextID  = af_data.area_type[i] + SID_Bj_SOS;
		g_eightCtrl.Areas[i].ImageBK = BID_FQ_Icon_BJ;
		
		if (ALARM_SHOW_STATE == af_data.show_state[i])
		{
			if((0 == ((is24hour>>i)&0x01)&&(1 == ((no24clear>>i)&0x01))) || (0 == ((is24hour>>i)&0x01)&&(1 ==((no24touch>>i)&0x01))))
			{
				// 蓝红闪烁
				g_eightCtrl.Areas[i].ImageID = state_to_bmpoder[ALARM_SHOW_STATE] + BMP_ID[af_data.area_type[i]];
				g_eightCtrl.Areas[i].Flash_imageID = state_to_bmpoder[NORM_SHOW_STATE] + BMP_ID[af_data.area_type[i]];
				g_eightCtrl.Alarming = 1;
			}
			else
			{	// 黄红闪烁
				g_eightCtrl.Areas[i].ImageID = state_to_bmpoder[ALARM_SHOW_STATE] + BMP_ID[af_data.area_type[i]];
				g_eightCtrl.Areas[i].Flash_imageID = state_to_bmpoder[5] + BMP_ID[af_data.area_type[i]];
				g_eightCtrl.Alarming = 1;
			}
			
		}
		else 
		{
			g_eightCtrl.Areas[i].ImageID = state_to_bmpoder[af_data.show_state[i]] + BMP_ID[af_data.area_type[i]];
			g_eightCtrl.Areas[i].Flash_imageID = 0;
		}
	}
}

/*************************************************
  Function:		update_area_state
  Description: 	改变防区状态
  Input: 		无	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void update_area_state(HWND hDlg)
{
	get_eightarae_info(hDlg);
	DEFEND_STATE defend_state;
	SendMessage(g_8AreasCtrlWin, WM_Eight_Set_Param, 0, (LPARAM)&g_eightCtrl);
	defend_state = storage_get_defend_state();
	SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
	ui_reflash(hDlg);
}

/*************************************************
  Function:		btn_alarm_inputBox_oncommand
  Description: 	密码输入框控件事件处理函数
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
static void InputCtrlCommand(HWND hDlg,  LPARAM lParam)
{
	DEFEND_STATE defend_state;

	switch(lParam)
	{
		case MSG_EVENT_YES:							//密码正确	
			if (AF_SUCCESS == alarm_unset_operator(EXECUTOR_LOCAL_HOST, 0))
			{	
				update_area_state(hDlg);
			}
			else
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
			}
			ui_reflash(hDlg);
			break;
			
		case MSG_EVENT_XIECHI:						//挟持报警
			if (AF_SUCCESS == alarm_unset_operator(EXECUTOR_LOCAL_HOST, 1))
			{	
				update_area_state(hDlg);
			}
			else
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
			}
			ui_reflash(hDlg);
			break;
			
		case MSG_EVENT_NO:							//密码错误
			defend_state = storage_get_defend_state();
			SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
			break;

		case 0:
			defend_state = storage_get_defend_state();
			SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
			ui_reflash(hDlg);
			break;
			
	}
}

/*************************************************
  Function:		btn_alarm_8AreasBox_oncommand
  Description: 	 8防区处理控件事件处理函数
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
static void EightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	AF_RETURN_TYPE ret;
	ret = alarm_single_set_operator(lParam);
	if (AF_SUCCESS == ret)
	{
		update_area_state(hDlg);
	}
}

/*************************************************
  Function:		show_err_msg
  Description:  显示错误的提示信息
  Input: 		
  Output:		无
  Return:		
  Others:
*************************************************/
static void show_err_msg(HWND hDlg, uint32 err)
{
	switch (err)
	{
		case AF_FQCHUFA:
			show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Af_Touch_Cannot_Bf);
			break;
			
		case AF_BAOJINGNOBUFANG:
			show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Af_Alarm_Cannot_Bf);
			break;
			
		case AF_BAOJING:
			show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Af_Alarm_Cannot_Qc);
			break;
	}
}

/*************************************************
  Function:		btn_alarm_rightwin_oncommand
  Description:  右边控件事件处理函数
  Input: 		无
  Output:		无
  Return:		端口名称
  Others:
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 ret = 0;
	uint32 id = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	DEFEND_STATE defend_state;
	uint8 alarm_time;
	
	//alarm_time = logic_get_alarm_time();
	alarm_time = logic_get_g_whole_alarm_state_param();
	
	switch (id)
	{
		case SID_Bj_Bf_Home:
				show_msg_pass(hDlg, IDC_FORM, PASS_TYPE_USER, 0xff);
			break;
			
		case SID_Bj_Bf_Out:							
			ret = alarm_set_operator(EXECUTOR_LOCAL_HOST);
			if (AF_SUCCESS == ret)
			{
				update_area_state(hDlg);
			}
			else
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
				show_err_msg(hDlg, ret);
			}
			ui_reflash(hDlg);
			break;
			
		case SID_Bj_Bf_Night:							
			ret = alarm_partset_operator(EXECUTOR_LOCAL_HOST);
			if (AF_SUCCESS == ret)
			{
				update_area_state(hDlg);
			}
			else
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
				show_err_msg(hDlg, ret);
			}
			ui_reflash(hDlg);
			break;

		case SID_Bj_Clean:
			if (storage_get_defend_state() != DIS_DEFEND)
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
				break;
			}
			
			if(alarm_time > 0)
			{
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
				show_err_msg(hDlg, AF_BAOJING);
			}
			else
			{
				alarm_clear_alerts_operator();
				update_area_state(hDlg);
				defend_state = storage_get_defend_state();
				SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
			}
			ui_reflash(hDlg);
			break;
			
		case BID_Right_Exit:						
			//SendMessage(g_8AreasCtrlWin, WM_Eight_End_Timer, 0, 0);
			close_page();
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
  	1.hDlg		窗口句柄
  	2.wParam	控件ID
  	3.lParam	参数
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case IDC_FORM:
			InputCtrlCommand(hDlg, lParam);
			break;
				
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
			break;
			
		case IDC_EIGHTAREA_CTRL:
			EightCtrlCommand(hDlg, lParam);
			break;
	}
	
    return TRUE;
}

/*************************************************
  Function:		CreateRightCtrl
  Description: 	创建右边控件
  Input: 		
  Output:		无
  Return:		
  Others:
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	int i;
	uint16 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint16 TextID[RIGHT_NUM_MAX] = {SID_Bj_Bf_Home, SID_Bj_Bf_Out, SID_Bj_Bf_Night,SID_Bj_Clean,0};
	uint16 Hold[RIGHT_NUM_MAX] = {1, 1, 1, 0, 0};

	DEFEND_STATE defend_state = storage_get_defend_state();

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.holdmode = 1;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	g_rightCtrl.parent = hDlg;
	
	for(i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
		g_rightCtrl.buttons[i].Hold = Hold[i];
	}

	g_rightwin = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);	

	SendMessage(g_rightwin, WM_Right_Set_Select, defend_state, 0);
}

/*************************************************
  Function:		Create8AreasBoxCtrl
  Description: 	创建防区控件
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
static void Create8AreasBoxCtrl(HWND hDlg)
{
	get_eightarae_info(hDlg);

	g_8AreasCtrlWin = CreateWindow(AU_EIGHTAREA_CTRL, "", 
			 	WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON,  
				IDC_EIGHTAREA_CTRL, 
				EIGHTAREA_CTRL_XPOS, EIGHTAREA_CTRL_YPOS, EIGHTAREA_CTRL_W, EIGHTAREA_CTRL_H, 
				hDlg, (DWORD)&g_eightCtrl);
}

/*************************************************
  Function:		create_ctrls
  Description: 	创建控件
  Input: 		
	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void create_ctrls(HWND hDlg)	
{
	CreateRightCtrl(hDlg);
	Create8AreasBoxCtrl(hDlg);
}

/*************************************************
  Function:		AlarmWin
  Description:  报警进入窗口
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
uint32 AlarmWindow(HWND hDlg)
{
	if (FALSE == storage_get_extmode(EXT_MODE_ALARM))
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Af_Mode_Unused);
		return FALSE;
	}
	
	//form_show(&FrmAlarm);
	SendMessage(FrmMain.hWnd, WM_FORM_SHOW, 0, (PGUI_FORM)&FrmAlarm);
	update_area_state(FrmAlarm.hWnd);
	return TRUE;
}

/*************************************************
  Function:		af_callback_gui
  Description:  报警回调函数
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
int32 af_callback_gui(int32 Param1,int32 Param2)
{
	DEFEND_STATE defend_state = storage_get_defend_state();
	HWND  parent = parent_page();
	PGUI_FORM frm = get_cur_form();
	if(frm == NULL)
	{
		log_printf("get_cur_form is NULL!\n");
		return FALSE;
	}
	
	if (FALSE == storage_get_extmode(EXT_MODE_ALARM))
	{
		return FALSE;
	}
	
	if (BAOJING_SHOW == Param1)
	{
		if (FALSE == ui_show_win_arbitration(0, SYS_OPER_ALARMING))
		{
			return FALSE;
		}
		
		if ((0 == FrmAlarm.hWnd)||(FrmAlarm.hWnd != frm->hWnd)&&(FrmAlarm.hWnd != parent))
		{	
			back_main_page();				
			AlarmWindow(0);
		}
		else
		{
			//exit_record_win();
			update_area_state(FrmAlarm.hWnd);
		}
	}
	else if (BAOJING_FRESH == Param1)
	{
		if (FrmAlarm.hWnd == frm->hWnd)
		{
			update_area_state(FrmAlarm.hWnd);
		}
	}
	else if (BAOJING_KEY == Param1)
	{
		if (FALSE == ui_show_win_arbitration(0, SYS_OPER_ALARM_SOS))
		{
			return FALSE;
		}
		if (DIS_DEFEND == defend_state)
		{
			alarm_set_operator(EXECUTOR_LOCAL_HOST);
			if(FrmAlarm.hWnd == frm->hWnd)
			{
				//exit_record_win();
				update_area_state(FrmAlarm.hWnd);
			}
			return TRUE;
		}
		if ((0 == FrmAlarm.hWnd)||(FrmAlarm.hWnd != frm->hWnd)&&(FrmAlarm.hWnd != parent))
		{
			back_main_page();	
			AlarmWindow(0);
		}
		else
		{
			//exit_record_win();
			update_area_state(FrmAlarm.hWnd);
		}
		
		if (DIS_DEFEND != defend_state)
		{
				show_msg_pass(FrmAlarm.hWnd, IDC_FORM, PASS_TYPE_USER, 1);
		}
	}

	return TRUE;
}

#if 0
/*************************************************
  Function:		af_gui_reaload
  Description:  
  Input: 		无
  Output:		无
  Return:		
  Others:
*************************************************/
int32 af_gui_reaload()
{
	if (FrmAlarm.hWnd)
	{
		update_area_state(FrmAlarm.hWnd);
		return TRUE;
	}

	return FALSE;
}
#endif 

/*************************************************
  Function:		MyControlProc
  Description: 	控制消息处理函数
  Input: 		
	1.hDlg		句柄
	2.message	消息类型
	3.wParam	附加数据
	4.lParam	附加数据
  Output:		无
  Return:		无
  Others:
*************************************************/
static int WindowProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case MSG_CREATE:
			create_ctrls(hDlg);
			break;

		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				// 防止通话有视频全屏状态下切换到报警界面，右边控件显示不出问题
				SendMessage(g_rightwin, WM_Right_Clear_BackGround, 0, 0);
				update_area_state(hDlg);
			}
			else if (wParam == SW_HIDE)
			{
				close_page();
			}
			break;
			
		case MSG_PAINT:
			break;

		case MSG_LBUTTONDOWN:
   			break;
   			
	    case MSG_LBUTTONUP:
	        break;

		case MSG_KEYDOWN:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
        	break;

		case MSG_KEYUP:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
			break;

		case MSG_COMMAND:
			OnCommand(hDlg, wParam, lParam);
			break;

		case MSG_TIMER:
			break;	
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return  0;
		
		case MSG_DESTROY:
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

