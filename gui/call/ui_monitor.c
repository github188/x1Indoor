/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_monitor.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-10-22
  Description:  
				监视界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************常量定义************************/
#define GUI_TIMER_ID       		1       // 定时器ID
#define FULLSCREEN_DELAYTIME	2       // 全屏延时时间

/************************变量定义************************/
static RIGHTCTRL_INFO g_rightCtrl;
static DEVICE_TYPE_E g_DevType;		
static char g_DevNo[50];						
static INTER_OPER_TYPE g_OperType; 
//static uint8 g_DrawVideo = FALSE;
static MONITOR_STATE_E g_InterState = MONITOR_END;
//static uint16 g_RemainTime = 0;
static uint8 g_SetVolume = FALSE;					// 是否处于设置音量状态
static uint8 g_mute = FALSE;						// 是否静音状态
static uint8 g_volume = 4;
static uint8 g_ticks = 0;
static uint32 g_ErrHintTxtID = 0;					// 错误提示文字ID
static uint8 g_ErrHintTicks = 0;					// 错误提示显示计时
static uint8 g_FullScreen = FALSE;
static uint32 g_rightshowmode = 0;                  // 右边控件显示标志位
static HWND g_RightWin = 0;
static HWND g_MonitorWin =0;
static uint8 g_index = -1;
static uint8 g_MonitorBuzy = FALSE;					// 监视忙时，刷新标志

/************************函数定义************************/
CREATE_FORM(FrmMonitor);

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
  Function:		draw_volume
  Description: 	画音量图标
  Input:
   1.hdc		句柄
  Output:	    无
  Return:		无
  Others:
*************************************************/
static void draw_volume(HDC hdc)
{
	if (g_FullScreen == FALSE)
	{
		if (g_SetVolume)
		{
			uint32 VolumeBmpID = 0;
			if (g_mute)
			{
				DrawBmpID(hdc, MUTE_ICON_XPOS, MUTE_ICON_YPOS, 0, 0, BID_Inter_MuteOn1);
			}
			else
			{
				DrawBmpID(hdc, MUTE_ICON_XPOS, MUTE_ICON_YPOS, 0, 0, BID_Inter_MuteOff1);
			}
			
			VolumeBmpID = BID_Inter_Volume0 + g_volume;
			DrawBmpID(hdc, VOLUME_ICON_XPOS, VOLUME_ICON_YPOS, 0, 0, VolumeBmpID);
		}	 
	}
}

/*************************************************
  Function:		draw_string_hint
  Description: 	画状态提示
  Input:
   1.hdc		句柄
  Output:		无
  Return:			
  Others:		
*************************************************/
static void draw_string_hint(HDC hdc)
{
	uint32 StrId = 0;
    char DevDesc[50] = {0};
	RECT Textrc;
	
	switch (g_InterState)
	{
		case MONITOR_SEARCH:
			StrId = SID_Inter_Search;
			break;
			
		case MONITOR_REQUEST:
			StrId = SID_Inter_Connecting;
			break;
			
		case MONITOR_MONITORING:
			StrId = SID_Inter_Monitor;
			break;
			
		case MONITOR_TALKING:
			StrId = SID_Inter_Talking;
			break;
			
		case MONITOR_END:
			StrId = 0;
			break;
			
		default:
			break;
	}

	// 获得设备描述
	if (strlen(g_DevNo) > 0)
	{
		get_dev_description(g_DevType, g_DevNo, DevDesc, 50); 					
	}						
	
    // 画设备描述和状态提示: 若有出错信息，画出错信息，否则画监视状态
	SetTextColor(hdc, COLOR_green);
	SelectFont(hdc, GetBoldFont(FONT_16));
	Textrc.left = TEXT_XPOS;
	Textrc.top = TEXT_YPOS;
	Textrc.right = Textrc.left + TEXT_LEN;
	Textrc.bottom = Textrc.top + Font16X16;
	DrawText(hdc, DevDesc, -1, &Textrc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, DevDesc, -1, &Textrc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	if (strlen(DevDesc) == 0)
	{
		Textrc.left = 0;
		Textrc.top = TEXT_YPOS;
		Textrc.right = FORM_W - RIGHT_CTRL_W;
		Textrc.bottom = Textrc.top + Font16X16;
		if (g_ErrHintTxtID)
		{
			DrawText(hdc, get_str(g_ErrHintTxtID), -1, &Textrc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		else if (StrId)
		{
			DrawText(hdc, get_str(g_ErrHintTxtID), -1, &Textrc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
	}
	else
	{
		Textrc.left = Textrc.right + TEXT_DISX;
		Textrc.top = TEXT_YPOS;
		Textrc.right = Textrc.left + TEXT_LEN;
		Textrc.bottom = Textrc.top + Font16X16;
		if (g_ErrHintTxtID)
		{
			DrawText(hdc, get_str(g_ErrHintTxtID), -1, &Textrc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			DrawText(hdc, get_str(g_ErrHintTxtID), -1, &Textrc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
		else if (StrId)
		{
			DrawText(hdc, get_str(StrId), -1, &Textrc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			DrawText(hdc, get_str(StrId), -1, &Textrc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
	}
}

/*************************************************
  Function:		draw_bk
  Description: 	画背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_bk(HDC hdc)
{
	// modi by luofl 2015-03-24 监视的时候只显示黑色背景
	#if 0
   	if (g_DrawVideo)
    {	
    	DrawBmpID(hdc, 0, 0, 0, 0, BID_Inter_VideoWinBK);
    }
    else
    {
    	DrawBmpID(hdc, 0, 0, 0, 0, BID_Inter_VideoWinBK3);
    }	
	#else
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Inter_VideoWinBK);
	#endif
}

/*************************************************
  Function:		OnPaint
  Description: 	画控件
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnPaint(HWND hDlg)
{
	HDC hdc;
	RECT rc;
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);
	SetBkMode(hdc, BM_TRANSPARENT);
    draw_bk(hdc);
	draw_volume(hdc);
	draw_string_hint(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		change_right_show_mode
  Description: 	通话主叫右边控件设置
  Input:		无
  Output:		无
  Return:			
  Others:			
*************************************************/
static void change_right_show_mode(void)
{
	uint32 ImageID[RIGHT_NUM_MAX];
	uint32 TextID[RIGHT_NUM_MAX];
	
	if (g_InterState == MONITOR_TALKING)
	{
		if (BID_Right_Up == g_rightshowmode)
		{
			ImageID[0] = 0;
			ImageID[1] = 0;  
			ImageID[2] = 0;
			ImageID[3] = BID_Right_Full;
			ImageID[4] = BID_Right_Down;

			TextID[0] = SID_Inter_Unlock;
			TextID[1] = SID_Inter_HangUP;
			TextID[2] = SID_Inter_Snap;
			TextID[3] = 0;
			TextID[4] = 0;
		}
		else
		{
			ImageID[0] = 0;
			ImageID[1] = 0;  
			ImageID[2] = 0;
			ImageID[3] = 0;
			ImageID[4] = BID_Right_Up;

			TextID[0] = SID_Inter_Unlock;
			TextID[1] = SID_Inter_HangUP;
			TextID[2] = SID_Inter_AudioAdd;
			TextID[3] = SID_Inter_AudioDel;
			TextID[4] = 0;
		}
		if (g_RightWin)
		{
			change_rightctrl_allimagetext(g_RightWin, ImageID, TextID);
		}
	}
	else
	{
		if (BID_Right_Up == g_rightshowmode)
		{
			ImageID[0] = 0;
			ImageID[1] = 0;  
			ImageID[2] = 0;
			ImageID[3] = BID_Right_Full;
			ImageID[4] = BID_Right_Down;

			TextID[0] = SID_Inter_Unlock ;
			TextID[1] = SID_Inter_Talk;
			TextID[2] = SID_Inter_Snap;
			TextID[3] = 0;
			TextID[4] = 0;
		}
		else
		{
			ImageID[0] = 0;
			ImageID[1] = 0;  
			ImageID[2] = 0;
			ImageID[3] = 0;
			ImageID[4] = BID_Right_Up;

			TextID[0] = SID_Inter_Unlock ;
			TextID[1] = SID_Inter_Talk;
			TextID[2] = SID_Inter_StopMonitor;
			TextID[3] = SID_Inter_Next;
			TextID[4] = 0;
		}
		
		if (g_RightWin)
		{
			change_rightctrl_allimagetext(g_RightWin, ImageID, TextID);
		}
	}
}

/*************************************************
  Function:		ui_monitor_talkbusy_key
  Description: 	接听被叫
  Input:		无
  Output:		无
  Return:			
  Others:			
*************************************************/
static void ui_monitor_talkbusy_key(void)
{
	SendMessage(g_RightWin, WM_Right_Set_TextID, 0, SID_Inter_Unlock);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 1, SID_Inter_Talk);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 2, SID_Inter_Snap);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 3, SID_Inter_Next);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 4, SID_Inter_StopMonitor);
}

/*************************************************
  Function:		ui_monitor_talk
  Description: 	主叫
  Input:        无 
  Output:		无
  Return:			
  Others:			
*************************************************/
static void ui_monitor_talk(void)
{
	SendMessage(g_RightWin, WM_Right_Set_TextID, 0, SID_Inter_Unlock);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 1, SID_Inter_HangUP);
	SendMessage(g_RightWin, WM_Right_Set_TextID, 2, SID_Inter_Snap);
	SendMessage(g_RightWin, WM_Right_Set_Image, 3, BID_Right_Full);
	SendMessage(g_RightWin, WM_Right_Set_Image, 4, BID_Right_Down);
}

#if 0
/*************************************************
  Function:		ui_monitor_start
  Description: 	监视开始时初始化右键
  Input:        无 
  Output:		无
  Return:			
  Others:			
*************************************************/
static void ui_monitor_start(void)
{
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, BID_Right_Full, BID_Right_Down};
	uint32 TextID[RIGHT_NUM_MAX] = {SID_Inter_Unlock, SID_Inter_Talk, SID_Inter_Snap, 0, 0};

	change_rightctrl_allimagetext(g_RightWin, ImageID, TextID);
}
#endif

/*************************************************
  Function:		monitor_state_callbak
  Description: 	被叫状态回调
  Input:			
  	1.param1		
  	2.param2
  Output:		无
  Return:			
  Others:			
*************************************************/
void monitor_state_callbak(uint32 param1, uint32 param2)
{	
	switch (param1)
	{
		case MONITOR_SEARCH:
			g_InterState = param1;			 
			break;
			
		case MONITOR_REQUEST:
		{
			char DevStr[5] = {0};
			int32 index = (int32)param2;
			
			// 获取设备号
			memset(g_DevNo, 0, sizeof(g_DevNo));
			memset(DevStr, 0, sizeof(DevStr));
			sprintf(DevStr, "%d", index);
			switch (g_DevType)
			{
				case DEVICE_TYPE_STAIR:
				{
					// 获得梯号
					get_stair_fullno(g_DevNo, DevStr);	
					break;
				}	
				
				case DEVICE_TYPE_AREA:
					strcpy(g_DevNo, DevStr);
					break;
					
				case DEVICE_TYPE_DOOR_NET:
				case DEVICE_TYPE_DOOR_PHONE:
					strcpy(g_DevNo, DevStr);
					break;
					
				default:
					break;
			}
			g_InterState = param1;
			break;
		}
		
		case MONITOR_MONITORING:
		{	
			if (param2 == MONITOR_BUSY)
			{
				g_ErrHintTxtID = SID_Inter_busy;
				g_ErrHintTicks = 3;
				g_MonitorBuzy = TRUE;
				ui_monitor_talkbusy_key();
			}
			else
			{
				#if 0
				uint8 enable = TRUE;
				
				// 使能接听按钮
				SendMessage(g_RightWin, WM_Right_Set_enable, 0, (LPARAM)enable);
				// 使能开锁按钮
				SendMessage(g_RightWin, WM_Right_Set_enable, 1, (LPARAM)enable);
				// 2s后使能监视下一个
				// 使能监视抓拍
				SendMessage(g_RightWin, WM_Right_Set_enable, 3, (LPARAM)enable);
				#endif
				g_InterState = param1;
			}
			break;
		}
		
		case MONITOR_TALKING:
		{
			g_InterState = param1;
			ui_monitor_talk();
			break;
		}	

		case MONITOR_END:				
		{	
			//g_DrawVideo = FALSE;
			g_InterState = param1;
			switch (param2)
			{
				case MONITOR_SEARCH_ERR:			// 搜索设备失败
					g_ErrHintTxtID = SID_Inter_SearchFailed;
					g_ErrHintTicks = 3;				// 显示提示的时间
					break;
					
				case MONITOR_REQUEST_TIMEOUT:		// 连接失败
					g_ErrHintTxtID = SID_Inter_ConnFailed;
					g_ErrHintTicks = 3;				// 显示提示的时间
					break;
					
				case MONITOR_BUSY:					// 对方忙
					g_ErrHintTxtID = SID_Inter_busy;
					g_ErrHintTicks = 3;				// 显示提示的时间
					break;
					
				default:
					SendMessage(GetDlgItem(g_MonitorWin, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 0);
					PGUI_FORM frm = get_cur_form();
					#if 0
					if (frm->hWnd == FrmMsgCtrl.hWnd)
					{
						// 通话中按抓拍/开锁，对方马上挂机则关闭消息框
						SendMessage(FrmMsgCtrl.hWnd, WM_MSG_Close, 0, 0);
						usleep(20*1000);
						exit_page();
					}
					else if (frm->hWnd == FrmMsgHit.hWnd)
					{
						// 通话中按抓拍/开锁，对方马上挂机则关闭系统消息框
						SendMessage(FrmMsgHit.hWnd, WM_MSG_Close, 0, 0);
						usleep(20*1000);
						exit_page();
					}
					else if (frm->hWnd == FrmMonitor.hWnd)
					{
				   		exit_page();
					}
					break;
					#else
					if (frm->hWnd == FrmMsgCtrl.hWnd)
					{
						// 通话中按抓拍/开锁，对方马上挂机则关闭消息框
						SendMessage(FrmMsgCtrl.hWnd, WM_MSG_Close, 0, 0);
						usleep(20*1000);
					}
					close_page();
					return;
					#endif
			}
			
			break;
		}
		
		case MONITOR_TIMER:
		{	
			//g_RemainTime = 0;
			break;
		}	
	}
	
	if (param1 != MONITOR_TIMER)
	{
		ui_reflash(g_MonitorWin);
	}
}

/*************************************************
  Function:		change_volume
  Description: 	调整音量
  Input:		
  	1.OperType	1-mute 2-del 3-add
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void change_volume(uint8 OperType)
{
	// 获取音量
	uint8 volume = g_volume;
	
	if (OperType == VOLUME_MUTE)
	{
		g_mute = !g_mute;
	}
	else if (OperType == VOLUME_DEL)
	{
		if (volume > 0)
		{
			volume--;
		}	
	}
	else if (OperType == VOLUME_ADD)
	{
		if (volume < 8)
		{
			volume++;
		}
	}
	else
	{	
		return;
	}

	// 音量为0时，自动为静音
	if (volume == 0)
	{
		g_mute = TRUE;
	}
	else
	{
		if (g_mute == TRUE)
		{
			g_mute = FALSE;
		}
	}

	if (g_mute == TRUE)
	{
		// 设置静音音量
		//media_set_output_volume(0);
		media_set_talk_volume(g_DevType, 0);
	}
	else
	{
		// 设置volume音量
		//media_set_output_volume(volume);
		media_set_talk_volume(g_DevType, volume);
	}

	if (OperType == VOLUME_ADD || OperType == VOLUME_DEL)
	{
		g_volume = volume;
	}
}

/*************************************************
  Function:		full_screen
  Description: 	全屏操作
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void full_screen(HWND hDlg)
{
	int32 ret = FALSE;
	
	if (g_FullScreen)
	{
		g_FullScreen = FALSE;
		ret = media_full_dispaly_video(g_DevType, g_FullScreen);
		if (ret == TRUE)
		{
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 0);
		}
	}
	else
	{
		g_FullScreen = TRUE;
		ret = media_full_dispaly_video(g_DevType, g_FullScreen);
		if (ret == TRUE)
		{
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 1);
		}
	}
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:
  	1.lParam    按键位置
  	2.hDlg      窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void RightCtrlCommand(LPARAM lParam, HWND hDlg)
{
	uint32 image = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, lParam, 0);

	if (g_FullScreen)
	{
		full_screen(hDlg);
		return;
	}

	switch (image)
	{
		case SID_Inter_Unlock:
			if (g_InterState == MONITOR_MONITORING || g_InterState == MONITOR_TALKING)
			{
				int32 ret = monitor_unlock();
				if (ret == TRUE)
				{
					show_msg(hDlg, IDC_Msg_Notice, MSG_RIGHT, SID_Inter_UnlockSuccess);
				}
				else
				{
					// 提示开锁失败
					show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Inter_UnlockFailed);
				}
			}
			break;

		case SID_Inter_Talk:
			if (g_InterState == MONITOR_MONITORING)
			{
				// 监视转通话
				monitor_talk();
			}
			break;

		case SID_Inter_Snap:
			{
				int32 ret = monitor_video_snap();
				if (ret == FALSE)
				{
					// 抓拍失败提示
					show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Inter_CaptureFailed);
				}
				else
				{
					// 抓拍成功提示
					show_msg(hDlg,IDC_Msg_Notice, MSG_RIGHT, SID_Inter_CaptureSuccess);
				}
			}
			break;

		case BID_Right_Full:
			if (g_InterState == MONITOR_MONITORING || g_InterState == MONITOR_TALKING)
			{
				full_screen(hDlg);
			}
			break;

		case BID_Right_Down:
		case BID_Right_Up:
			g_rightshowmode = image;
			change_right_show_mode();
			break;

		case SID_Inter_AudioAdd:
			if (FALSE == g_SetVolume)
			{
				g_SetVolume = TRUE;
			}
			else
			{
				change_volume(VOLUME_ADD);
			}
			g_ticks = 0;
			ui_reflash(hDlg);
			break;

		case SID_Inter_AudioDel:
			if (FALSE == g_SetVolume)
			{
				g_SetVolume = TRUE;
			}
			else
			{
				change_volume(VOLUME_DEL);
			}
			g_ticks = 0;
			ui_reflash(hDlg);
			break;

		case SID_Inter_StopMonitor:
		case SID_Inter_HangUP:
			ui_monitor_stop();
			break;

		case SID_Inter_Next:
			if (g_InterState == MONITOR_MONITORING)
			{
				g_ErrHintTxtID = 0;
				g_ErrHintTicks = 0;
				monitor_next(TRUE);
			}
			break;

		#if 0
		case SID_Inter_GetList:
			g_ErrHintTxtID = 0;
			sys_set_monitor_state(FALSE);
			SendMessage(GetDlgItem(g_MonitorWin, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 0);
			close_page();
			ui_monitor_list_appstart(g_DevType, hDlg, TRUE);
			break;

		case BID_Right_Exit:
			ui_monitor_stop();
			break;
		#endif

		default:
			break;
	}
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
	1.id		返回的控件ID
	2.lParam	按键位置
	3.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(WPARAM id, LPARAM lParam, HWND hDlg)
{
	switch(id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(lParam, hDlg);
            break;
			
		default:
			break;
	}
	return;
}

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_key_down
  Description: 	按键按下
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_down(HWND hDlg, int x, int y)
{
}

/*************************************************
  Function:		get_key_up
  Description: 	按键抬起
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_up(void)
{ 
}
#endif

/*************************************************
  Function:		video_win_tick_proc
  Description: 	定时
  Input:		
  	1.hDlg      窗口句柄
  	2.wParam    参数1
  	3.lParam    参数2
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 video_win_tick_proc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	// 显示音量时，定时消失，触摸音量按钮时，定时初始为0
	if (g_SetVolume == TRUE)
	{
		if (g_ticks == 4)
		{
			g_SetVolume = FALSE;
			g_ticks = 0;
			ui_reflash(hDlg);
			// 存储设置音量
			storage_set_volume(storage_get_ringvolume(), g_volume, storage_get_keykeep());
		}
		g_ticks++;
	}
	
	if (g_ErrHintTicks > 0)
	{
		g_ErrHintTicks--;
		if (g_MonitorBuzy) // add by caogw 2016-04-08 防止监视忙时刷新异常现象
		{
			g_MonitorBuzy = FALSE;
			ui_reflash(hDlg);
		}
		if (g_ErrHintTicks == 0)
		{
			if (g_InterState == MONITOR_END)
			{
				SendMessage(GetDlgItem(g_MonitorWin, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 0);
				PGUI_FORM frm = get_cur_form();
				if (frm->hWnd == FrmMsgCtrl.hWnd)
				{
					// 连接中按抓拍，对方马上挂机则关闭消息框
					SendMessage(FrmMsgCtrl.hWnd, WM_MSG_Close, 0, 0);
					usleep(150*1000);
				}
				close_page();
				return FALSE;
			}
			g_ErrHintTxtID = 0;
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		CreateRightCtrl
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, BID_Right_Full, BID_Right_Down};
	uint32 TextID[RIGHT_NUM_MAX] = {SID_Inter_Unlock, SID_Inter_Talk, SID_Inter_Snap, 0, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	for(i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}
	g_RightWin = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y, 
				RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);
}

/*************************************************
  Function:		create_ctrls
  Description: 	创建控件
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void create_ctrls(HWND hDlg)	
{
	CreateRightCtrl(hDlg);
}

/*************************************************
  Function:			set_inter_info
  Description: 		设置对讲信息
  Input:			
  	1.DevType		设备类型
  	2.DevStr		设备编号
  Output:			无
  Return:			
  Others:		
*************************************************/
static void set_inter_info(INTER_OPER_TYPE OperType, DEVICE_TYPE_E DevType, char * DevStr)
{
	memset(g_DevNo, 0, sizeof(g_DevNo));
	g_InterState = MONITOR_REQUEST;
	g_SetVolume = FALSE;
	g_volume = storage_get_talkvolume();
	if (g_volume == 0)
	{
		g_mute = TRUE;
	}
	else
	{
		g_mute = FALSE;
	}
	g_ticks = 0;
	g_ErrHintTxtID = 0;
	g_ErrHintTicks = 0;
	g_FullScreen = FALSE;
}

/*************************************************
  Function:		ui_monitor_win
  Description: 	监视初始化
  Input:			
  	1.hDlg      窗口句柄
  	2.wParam	参数1
  	3.lParam    参数2
  Output:		无
  Return:			
  Others:		
*************************************************/
static uint32 ui_monitor_win(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
    g_MonitorWin = hDlg; 
	create_ctrls(hDlg);	
	
	return TRUE;
}

/*************************************************
  Function:		ui_monitor_appstart
  Description: 	启动监视应用
  Input:			
  	1.DevType	监视设备类型
  Output:		无
  Return:			
  Others:		
*************************************************/
void ui_monitor_appstart(DEVICE_TYPE_E DevType, uint8 index)
{
	int32 ret = ui_show_win_arbitration(0, SYS_OPER_MONITOR);
	if (ret == TRUE)
	{	
		#if 0
		PGUI_FORM frm;
		frm = get_cur_form();
		if (FrmMonitor.hWnd == frm->hWnd)
		{
			return;
		}
		#endif
		back_main_page();
		g_index = index;
		g_OperType = INTER_MONITOR_E;
		g_DevType = DevType;
		form_show(&FrmMonitor);
	}
}

/*************************************************
  Function:		monitor_search_fail
  Description: 	监视搜索列表失败
  Input:			
  	1.hDlg      窗口句柄
  Output:		无
  Return:			
  Others:		
*************************************************/
static void monitor_search_fail(HWND hDlg)
{
	g_ErrHintTxtID = SID_Inter_SearchFailed;
	g_InterState = MONITOR_END;
	g_ErrHintTicks = 3;	
	ui_reflash(hDlg);
	#if 0
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, SID_Inter_GetList, 0, 0};
	change_rightctrl_allimagetext(g_RightWin, ImageID, TextID);
	#endif
}

/*************************************************
  Function:		WindowProc
  Description: 	消息处理函数
  Input: 		
  	1.hDlg		句柄
  	2.message	消息
  	3.wParam	参数1
  	4.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static int WindowProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case MSG_CREATE:
			ui_monitor_win(hDlg, wParam, lParam);		
			break;
		
		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				SetTimer(hDlg, GUI_TIMER_ID, 50);
				//ui_monitor_start();
				set_inter_info(g_OperType, g_DevType, NULL);
				if (monitor_start(g_DevType, g_index) == FALSE)
				{
					monitor_search_fail(hDlg);
				}
			}
			else if (wParam == SW_HIDE)
			{
				// 逻辑会回调发关闭命令
				//close_page();
			}
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;
			
       	#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
    		get_key_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	        get_key_up(hDlg, LOWORD(lParam), HIWORD(lParam));
	        break;
        #endif
		
	    case MSG_KEYDOWN:
			switch((int)wParam)
			{
				case SCANCODE_F8:					// 开锁
					wParam = RIGHT_SCANCODE_F1;
					break;
					
				case SCANCODE_F9:					// 通话
					wParam = RIGHT_SCANCODE_F2;
					 break;
					 
				default:
					break;
			}
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
        	break;

		case MSG_KEYUP:
			switch((int)wParam)
			{
				case SCANCODE_F8:					// 开锁
					wParam = RIGHT_SCANCODE_F1;
					break;
					
				case SCANCODE_F9:					// 通话
					wParam = RIGHT_SCANCODE_F2;
					 break;
					 
				default:
					break;
			}
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
			break;
			
		case MSG_COMMAND:
			OnCommand(wParam, lParam, hDlg);
			break;
			
		case MSG_TIMER:	
			video_win_tick_proc(hDlg, wParam, lParam);
			break;
		
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			if (hDlg)
			{
				KillTimer(hDlg, GUI_TIMER_ID);
			}
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

