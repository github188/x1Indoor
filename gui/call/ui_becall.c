/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_becall.c
  Author:    	yanjl
  Version:   	1.0
  Date: 		2014-10-10
  Description:	被叫界面
				
  History:        
    1. Date:	2014-10-21
       Author:	luofl
       Modification:
       			重新优化和整理
    2. ...
*************************************************/

#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************常量定义************************/
#define GUI_TIMER_ID			101

#if (_LCD_DPI_ == _LCD_800480_)
#define VIDEO_WIN_W				640
#define VIDEO_WIN_H				480
#elif (_LCD_DPI_ == _LCD_1024600_)
#define VIDEO_WIN_W				820
#define VIDEO_WIN_H				600
#endif
#define VIDEO_WIN_X				0	
#define VIDEO_WIN_Y				0
#define INTER_INFO_DIS_Y		15
#define INTER_TIME_W			38
#define INTER_TIME_X			(VIDEO_WIN_X+(VIDEO_WIN_W-INTER_TIME_W))
#define INTER_TIME_Y			(VIDEO_WIN_Y+INTER_INFO_DIS_Y)

static DEVICE_TYPE_E g_DevType;		
static char g_DevNo[50];		
static char g_NewCallNo[50];
static INTER_OPER_TYPE g_OperType; 
static uint8 g_DrawVideo = 0;
static uint8 g_unlock;
static CALL_STATE_E g_InterState = CALL_STATE_NONE;
//static int16 g_RemainTime = 0;
static uint8 g_SetVolume = FALSE;					// 是否处于设置音量状态
static uint8 g_mute = FALSE;						// 是否静音状态
static uint8 g_volume = 4;
static uint8 g_ticks = 0;
static uint8 g_FullScreen = FALSE;					// 是否全屏
static uint32 ImageId[RIGHT_NUM_MAX] = {0, 0, 0, 0, 0};
static uint32 TextId[RIGHT_NUM_MAX] = {0, 0, 0, 0, 0};
static HWND g_RightWin = 0;
static HWND g_BecallWin = 0;
static uint32 g_righteditmode = 0;
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static PGUI_FORM frm = NULL;
#ifdef _AUTO_CAPTURE_
static uint8 g_autocapturetime = 0;
#endif

CREATE_FORM(FrmBeCall);

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
	g_OperType = OperType;
	g_DevType = DevType;
	memset(g_DevNo, 0, sizeof(g_DevNo));
	strcpy(g_DevNo, DevStr);
	memset(g_NewCallNo, 0, sizeof(g_NewCallNo));
	
	if (g_DevType == DEVICE_TYPE_STAIR || g_DevType == DEVICE_TYPE_AREA 
    	|| g_DevType == DEVICE_TYPE_DOOR_NET || g_DevType == DEVICE_TYPE_DOOR_PHONE)
    {
    	g_DrawVideo = TRUE;
		g_unlock = TRUE;
    }
    else
    {
    	g_DrawVideo = FALSE;
		#ifdef _MANAGER_IS_VIDEO_
		if (g_DevType == DEVICE_TYPE_MANAGER)
		{
			g_DrawVideo = TRUE;
		}
		#endif
    	g_unlock = FALSE;
    }
    
	g_InterState = CALL_STATE_CALLING;
	//g_RemainTime = 0;
	
	g_SetVolume = FALSE;
	g_volume = storage_get_ringvolume();
	if (g_volume == 0)
	{
		g_mute = TRUE;
	}
	else
	{
		g_mute = FALSE;
	}
	g_ticks = 0;
	g_FullScreen = FALSE;
	
	#ifdef _AUTO_CAPTURE_
	if (g_DrawVideo)
	{
		g_autocapturetime = 3;
	}
	else
	{
		g_autocapturetime = 0;
	}
	#endif
}

/*************************************************
  Function:			ui_becall_win
  Description: 		becall界面
  Input:			
  	1.hDlg			窗口句柄
  	2.param     	参数
  Output:			无
  Return:			
  Others:		
*************************************************/
uint32 ui_becall_win(HWND hDlg, void * param)
{
	INTER_INFO_S * pInterInfo = (INTER_INFO_S*)param;

	if (pInterInfo != NULL)
	{
		if (pInterInfo->InterType == INTER_CALLIN_E)
		{
			int32 ret = ui_show_win_arbitration(0, SYS_OPER_CALLIN);
			if (ret == FALSE)
			{
				return FALSE;
			}
		}
		set_inter_info(pInterInfo->InterType, pInterInfo->DevType, pInterInfo->DevStr);	
	}
	else
	{
		return FALSE;
	}
	//memset(g_NewCallNo, 0, sizeof(g_NewCallNo));
	//退回主界面
	back_main_page();
	//form_show(&FrmBeCall);
	SendMessage(FrmMain.hWnd, WM_FORM_SHOW, 0, (PGUI_FORM)&FrmBeCall);
	
	return TRUE;
}

/*************************************************
  Function:			change_right_edit_mode
  Description: 		改变右键元素的文字或图片
  Input:			
  	1.hDlg			窗口句柄
  Output:			
  Return:			
  Others:			
*************************************************/
static void change_right_edit_mode(void)
{
	if (DEVICE_TYPE_AREA == g_DevType || DEVICE_TYPE_STAIR == g_DevType 
		|| DEVICE_TYPE_DOOR_PHONE == g_DevType || DEVICE_TYPE_DOOR_NET == g_DevType)
	{
		if (BID_Right_Up == g_righteditmode)
		{
			ImageId[0] = 0 ;
			ImageId[1] = 0;  
			ImageId[2] = 0;
			ImageId[3] = BID_Right_Full;
			ImageId[4] = BID_Right_Down;

			TextId[0] = SID_Inter_Unlock;
			TextId[1] = SID_Inter_HangUP;
			TextId[2] = SID_Inter_Snap;
			TextId[3] = 0;
			TextId[4] = 0;	
		}
		else
		{
			ImageId[0] = 0 ;
			ImageId[1] = 0;  
			ImageId[2] = 0;
			ImageId[3] = 0;
			ImageId[4] = BID_Right_Up;

			TextId[0] = SID_Inter_Unlock ;
			TextId[1] = SID_Inter_HangUP;
			TextId[2] = SID_Inter_AudioAdd;
			TextId[3] = SID_Inter_AudioDel;
			TextId[4] = 0;	
		}
		
		if (g_RightWin)
		{
			int i;
		
			for ( i = 0; i < RIGHT_NUM_MAX; i++)
			{
				SendMessage(g_RightWin, WM_Right_Set_Image, i, (LPARAM)ImageId[i]);
				SendMessage(g_RightWin, WM_Right_Set_TextID, i, (LPARAM)TextId[i]);
			}
		}
	}
}

/*************************************************
  Function:			callin_answer_call
  Description: 		接听被叫时右键元素的文字或图片改变
  Input:			
  	1.hDlg			窗口句柄
  Output:			无
  Return:			
  Others:			
*************************************************/
static void callin_answer_call(HWND hDlg)
{
	int32 ret;
	
	ret = inter_answer_call(NULL);
	if (ret == FALSE)
	{
		return;
	}
	
	switch (g_DevType)
	{
		case DEVICE_TYPE_ROOM:
		case DEVICE_TYPE_FENJI_NET:
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = 0;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = 0;
			TextId[1] = 0;
			TextId[2] = SID_Inter_HangUP;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			break;

		case DEVICE_TYPE_MANAGER:
			#ifdef _MANAGER_IS_VIDEO_
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = BID_Right_Full;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = SID_Inter_Snap;
			TextId[1] = SID_Inter_HangUP;
			TextId[2] = 0;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			#else
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = 0;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = 0;
			TextId[1] = 0;
			TextId[2] = SID_Inter_HangUP;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			#endif
			break;
			
		case DEVICE_TYPE_AREA:
		case DEVICE_TYPE_STAIR:
		case DEVICE_TYPE_DOOR_PHONE:
		case DEVICE_TYPE_DOOR_NET:
			ImageId[0] = 0;
			ImageId[1] = 0;  
			ImageId[2] = 0;
			ImageId[3] = BID_Right_Full;
			ImageId[4] = BID_Right_Down;

			TextId[0] = SID_Inter_Unlock ;
			TextId[1] = SID_Inter_HangUP;
			TextId[2] = SID_Inter_Snap;
			TextId[3] = 0;
			TextId[4] = 0;	
			g_righteditmode = BID_Right_Down;
			break;

		default:
			break;
	}

	if (g_RightWin)
	{
		int i;
		
		for ( i = 0; i < RIGHT_NUM_MAX; i++)
		{
			SendMessage(g_RightWin, WM_Right_Set_Image, i, (LPARAM)ImageId[i]);
			SendMessage(g_RightWin, WM_Right_Set_TextID, i, (LPARAM)TextId[i]);
		}
	}
}

/*************************************************
  Function:			callin_state_callbak
  Description: 		被叫状态回调
  Input:			
  	1.param1		参数1		
  	2.param2		参数2
  Output:			无
  Return:			
  Others:			
*************************************************/
void callin_state_callbak(uint32 param1, uint32 param2)
{	
	switch (param1)
	{	
		case CALL_STATE_REQUEST:
			break;
			
		case CALL_FORMSHOW_CALLING:
			ui_becall_win(0, (void *)param2);
			break;
			
		case CALL_STATE_CALLING:
			if (g_InterState == CALL_STATE_CALLING)
			{
				g_volume = storage_get_ringvolume();
			}
			else
			{
				g_volume = storage_get_talkvolume();
			}
			g_InterState = param1;
			break;

		case CALL_NEW_CALLING:
			if (0 == param2)
			{
				memset(g_NewCallNo, 0, sizeof(g_NewCallNo));
			}
			else
			{
				memcpy(g_NewCallNo, (char *)param2, 30);
			}
			break;
			
		case CALL_STATE_RECORDHINT:
			g_InterState = param1;
			//g_RemainTime = 0;						// 留言提示不显示时间
			break;
			
		case CALL_STATE_RECORDING:
			g_InterState = param1;
			break;
			
		case CALL_STATE_TALK:
			g_mute = FALSE;							// 转通话时，音量图标重设
			g_volume = storage_get_talkvolume();
			g_InterState = param1;
			break;
			
		case CALL_STATE_END:						// 挂机，返回上一个页面
			g_InterState = CALL_STATE_NONE;
			//SendMessage(GetDlgItem(g_BecallWin, IDC_RIGHT_BUTTON), WM_Right_Clear_BackGround, 0, 0);
			frm = get_cur_form();
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
			else if (frm->hWnd == g_BecallWin) // 当前界面为被叫界面时，才能退出
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

		case CALL_TIMER:
			//g_RemainTime = 0;
			break;
	}

	if (param1 != CALL_TIMER)
	{
		ui_reflash(g_BecallWin);
	}
}

/*************************************************
  Function:			callout_answer_call
  Description: 		主叫接听时右键元素的文字或图片改变
  Input:			无
  Output:			无
  Return:			
  Others:			
*************************************************/
static void callout_answer_call(void)
{
	switch (g_DevType)
	{
		case DEVICE_TYPE_ROOM:
		case DEVICE_TYPE_FENJI_NET:
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = 0;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = 0;
			TextId[1] = 0;
			TextId[2] = SID_Inter_HangUP;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			break;

		case DEVICE_TYPE_MANAGER:
			#ifdef _MANAGER_IS_VIDEO_
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = BID_Right_Full;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = SID_Inter_Snap;
			TextId[1] = SID_Inter_HangUP;
			TextId[2] = 0;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			#else
			ImageId[0] = 0;
			ImageId[1] = 0;
			ImageId[2] = 0;
			ImageId[3] = 0;
			ImageId[4] = 0;

			TextId[0] = 0;
			TextId[1] = 0;
			TextId[2] = SID_Inter_HangUP;
			TextId[3] = SID_Inter_AudioAdd;
			TextId[4] = SID_Inter_AudioDel;	
			#endif
			break;

		default:
			break;
	}
	
	if (g_RightWin)
	{
		int i;
		
		for ( i = 0; i < RIGHT_NUM_MAX; i++)
		{
			SendMessage(g_RightWin, WM_Right_Set_Image, i, (LPARAM)ImageId[i]);
			SendMessage(g_RightWin, WM_Right_Set_TextID, i, (LPARAM)TextId[i]);
		}
	}
}

/*************************************************
  Function:			callout_state_callbak
  Description: 		主叫状态回调
  Input:			
  	1.param1		参数1		
  	2.param2		参数2
  Output:			无
  Return:			
  Others:			
*************************************************/
void callout_state_callbak(uint32 param1, uint32 param2)
{
	frm = get_cur_form();
	if (frm->hWnd == FrmCall.hWnd)
	{
		request_callno_addr_callbak(param1, param2);
		return;
	}
	
	switch (param1)
	{
		case CALL_STATE_REQUEST:
			break;
			
		case CALL_STATE_CALLING:
			g_InterState = param1;
			break;
			
		case CALL_NEW_CALLING:
			if (0 == param2)
			{
				memset(g_NewCallNo, 0, sizeof(g_NewCallNo));
			}
			else
			{
				memcpy(g_NewCallNo, (char *)param2, 30);
			}
			break;
			
		case CALL_STATE_RECORDHINT:
			g_InterState = param1;
			//g_RemainTime = 0;						// 留言提示不显示时间
			break;
			
		case CALL_STATE_RECORDING:
			g_InterState = param1;
			break;
			
		case CALL_STATE_TALK:
			g_volume = storage_get_talkvolume();
			g_InterState = param1;
			callout_answer_call();
			break;
			
		case CALL_STATE_END:						// 挂机，返回上一个页面
			g_InterState = CALL_STATE_NONE;
			//if (frm->hWnd == FrmBeCall.hWnd)
			//{
			//	exit_page();
			//}
			close_page();						// 回调后就关闭被叫界面
			return;
			break;

		case CALL_TIMER:
		{	
			//g_RemainTime = 0;
			break;
		}	
	}

	if (CALL_TIMER != param1)
	{
		ui_reflash(FrmBeCall.hWnd);
	}
}

/*************************************************
  Function:			draw_bk
  Description: 		画背景
  Input:			
  1.hDlg			窗口句柄
  Output:			无
  Return:			无
  Others:
*************************************************/
static void draw_bk(HDC hdc)
{
	if (g_DrawVideo)
    {	
    	FillBoxWithBitmap(hdc, 0, 0,VIDEO_WIN_W, VIDEO_WIN_H, get_bmp(BID_Inter_VideoWinBK));
    }
    else
    {
    	if (g_DevType == DEVICE_TYPE_MANAGER)
    	{
    		FillBoxWithBitmap(hdc, 0, 0,VIDEO_WIN_W, VIDEO_WIN_H, get_bmp(BID_Inter_VideoWinBK2));
    	}
    	else
    	{
    		FillBoxWithBitmap(hdc, 0, 0,VIDEO_WIN_W, VIDEO_WIN_H, get_bmp(BID_Inter_VideoWinBK_Room));
    	}
    }
}

/*************************************************
  Function:			draw_volume
  Description: 		画音量图片
  Input:			
  1.hDlg			窗口句柄
  Output:			无
  Return:			无
  Others:
*************************************************/
static void draw_volume(HDC hdc)
{
	if (g_FullScreen == FALSE)
	{
		// 画音量调节
		if (g_SetVolume)
		{
			uint32 VolumeBmpID = 0;
			uint32 VolumeIconY = MUTE_ICON_YPOS;
			
			if (!g_DrawVideo)
			{
				VolumeIconY = VOLUME_ALL_ICON_Y;
			}
			if (g_mute)
			{
				DrawBmpID(hdc, MUTE_ICON_XPOS, VolumeIconY, 0, 0, BID_Inter_MuteOn1);
			}
			else
			{
				DrawBmpID(hdc, MUTE_ICON_XPOS, VolumeIconY, 0, 0, BID_Inter_MuteOff1);
			}
		
			VolumeBmpID = BID_Inter_Volume0 + g_volume;
			DrawBmpID(hdc, VOLUME_ICON_XPOS, VolumeIconY, 0, 0, VolumeBmpID);
		}
	}
}

/*************************************************
  Function:			draw_string_hint
  Description: 		画状态提示
  Input:		
  	1.hDlg			窗口句柄
  Output:			无
  Return:			无
  Others:		
*************************************************/
static void draw_string_hint(HDC hdc)
{
	//char time[20] = {0};
	uint16 StrId = 0;
	char DevDesc[50] = {0};
	RECT TextRc;

	switch (g_InterState)
	{
		case CALL_STATE_CALLING:
			if (g_OperType == INTER_CALLOUT_E)
			{
				StrId = SID_Inter_Calling;
			}
			else if (g_OperType == INTER_CALLIN_E)
			{
				StrId = SID_Inter_BeCalling;
			}
			break;
			
		case CALL_STATE_RECORDHINT:
			StrId = SID_Inter_RecordHinting;
			break;
			
		case CALL_STATE_RECORDING:
			StrId = SID_Inter_Recording;
			break;
			
		case CALL_STATE_TALK:
			StrId = SID_Inter_Talking;
			break;
			
		case CALL_STATE_END:
			StrId = 0;
			break;
			
		default:
			StrId = 0;
			break;
	}
	// 画设备描述
	if (strlen(g_DevNo) > 0)
	{   
		get_dev_description(g_DevType, g_DevNo, DevDesc, 50); // 获得设备描述
	}
	
    // 画设备描述和状态提示
	SetTextColor(hdc, COLOR_green);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	TextRc.left = TEXT_XPOS;
	TextRc.top = TEXT_YPOS;
	TextRc.right = TextRc.left + TEXT_LEN;
	TextRc.bottom = TextRc.top + WIN_COMM_FONT;
	DrawText(hdc, DevDesc, -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, DevDesc, -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	log_printf("StrId = %ld\n",StrId);
	if (StrId)
	{
		TextRc.left = TextRc.right + TEXT_DISX;
		TextRc.top = TEXT_YPOS;
		TextRc.right = TextRc.left + TEXT_LEN;
		TextRc.bottom = TextRc.top + WIN_COMM_FONT;
		DrawText(hdc, get_str(StrId), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawText(hdc, get_str(StrId), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);	
	}	

	#if 0
	// 画时间提示: time dec
	if (g_RemainTime > 0)
	{
		sprintf(time, "%02d:%02d", g_RemainTime/60, g_RemainTime%60);
		TextRc.left = INTER_TIME_X;
		TextRc.top = INTER_TIME_Y;
		TextRc.right = INTER_TIME_X + INTER_TIME_W;
		TextRc.bottom = INTER_TIME_Y + Font16X16;
		DrawText(hdc, time, -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}
	#endif

	// 画新来电提示
	if (strlen(g_NewCallNo) > 0)
	{
		uint16 DevType = g_NewCallNo[29];
		char NewCallNo[30] = {0};
		
		sprintf(NewCallNo, "%s", g_NewCallNo);
		memset(DevDesc, 0, sizeof(DevDesc));
		get_dev_description(DevType, NewCallNo, DevDesc, 50);
		StrId = SID_Inter_BeCalling;
		
	    // 画设备描述和状态提示
		SetTextColor(hdc, COLOR_green);
		SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
		TextRc.left = TEXT_XPOS;
		TextRc.top = TEXT_YPOS + TEXT_DISY;
		TextRc.right = TextRc.left + TEXT_LEN;
		TextRc.bottom = TextRc.top + WIN_COMM_FONT;
		DrawText(hdc, DevDesc, -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawText(hdc, DevDesc, -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		
		if (StrId)
		{
			TextRc.left = TextRc.right + TEXT_DISX;
			TextRc.top = TEXT_YPOS + TEXT_DISY;
			TextRc.right = TextRc.left + TEXT_LEN;
			TextRc.bottom = TextRc.top + WIN_COMM_FONT;
			DrawText(hdc, get_str(StrId), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			DrawText(hdc, get_str(StrId), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);	
		}
	}

}

/*************************************************
  Function:		OnPaint
  Description: 	显示窗口
  Input: 		
	1.hDlg		窗口句柄
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
	draw_string_hint(hdc);
	draw_volume(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:			full_screen
  Description: 		全屏操作
  Input:		    无
  Output:			无
  Return:			无
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
  Function:			change_volume
  Description: 		调整音量
  Input:		
  	1.OperType		1-mute 2-dec 3-add
  Output:			无
  Return:			无
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
		// 最低音量为1
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

	if (OperType == VOLUME_DEL || OperType == VOLUME_ADD)
	{
		// 设置音量存储
		g_volume = volume;
	}
}

/*************************************************
  Function:			modi_volume
  Description: 		设置音量
  Input:		
  	1.OperType		0-mute 2-dec 3-add
  Output:			无
  Return:			无
  Others:		
*************************************************/
static void modi_volume(VOLUME_TYPE oper)
{
	if ((g_OperType == INTER_CALLIN_E && g_InterState == CALL_STATE_CALLING)
		|| g_InterState == CALL_STATE_TALK)
	{
		if (FALSE == g_SetVolume)
		{
			g_SetVolume = TRUE;
		}
		else
		{
			change_volume(oper);
		}
		g_ticks = 0;
	}
}

/*************************************************
  Function:			RightCtrlCommand
  Description: 		右边控件按下的处理函数
  Input:		
  	1.hDlg			窗口句柄
  	2.index			控件元素索引
  Output:			无
  Return:			TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, WPARAM index)
{
	uint32 Id = 0;

	if (g_FullScreen)
	{
		full_screen(hDlg);
		return;
	}
	
	Id = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, index, 0);
	switch (Id)
	{
		case BID_Right_Full:
			full_screen(hDlg);
			break;

		case BID_Right_Down:
		case BID_Right_Up:	
			g_righteditmode = Id;
			g_SetVolume = FALSE;
			change_right_edit_mode();
			ui_reflash(hDlg);
			break;

		case SID_Inter_HangUP:
		case BID_Right_Exit:
			inter_hand_down();
			//exit_page();
			break;
			
		case SID_Inter_Snap:
			{
				int32 ret = inter_video_snap();
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
		
		case SID_Inter_AudioAdd:
			modi_volume(VOLUME_ADD);
			ui_reflash(hDlg);
			break;
			
		case SID_Inter_AudioDel:
			modi_volume(VOLUME_DEL);
			ui_reflash(hDlg);
			break;				

		case SID_Inter_Answer:
		{
			//g_righteditmode = BID_Right_Up;
			//change_right_edit_mode();                 //接听后 右边控件变化
			callin_answer_call(hDlg);
			break;
		}
		
		case SID_Inter_Unlock:
			if (g_unlock == TRUE)
			{
				int32 ret = inter_unlock();
				if (ret == TRUE)
				{
					show_msg(hDlg, IDC_Msg_Notice, MSG_RIGHT, SID_Inter_UnlockSuccess);
					#ifdef _USE_ELEVATOR_
					if (g_DevType == DEVICE_TYPE_STAIR)
					{
						#ifdef _ALAV_LINGKE_
						dianti_set_call_custom(g_DevNo);
						#elif defined _DEL_DT_BECALL_
						#else
						dianti_set_cmd(ELEVATOR_CALL);
						#endif
					}
					#endif
				}
				else
				{
					// 提示开锁失败
					show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Inter_UnlockFailed);
				}
			}
			break;
	
		default:
			break;
	}
}

/*************************************************
  Function:			OnCommand
  Description:		控件事件处理函数 	
  Input:		
  	1.id			返回的控件id
  	2.index			控件元素索引
  	3.hDlg			窗口句柄
  Output:			无
  Return:			TRUE 是 FALSE 否
  Others:	
  	
*************************************************/
static uint32 OnCommand(WPARAM id, WPARAM index, HWND hDlg)
{
	switch (id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, index);
			break;
		
		default:
			return FALSE;
	}
	
    return TRUE;
}

/*************************************************
  Function:			call_tick_proc
  Description: 		定时
  Input:		
  	1.hDlg			窗口句柄
  	2.wParam		参数1
  	3.lParam		参数2
  Output:			无
  Return:			TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 call_tick_proc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	// 显示音量时，定时消失，触摸音量按钮时，定时初始为0
	if (g_SetVolume == TRUE)
	{
		if (g_ticks == 4)
		{
			uint16 VolumeIconY = MUTE_ICON_YPOS;
			g_SetVolume = FALSE;
			g_ticks = 0;
			
			// 隐藏音量图标
			if (!g_DrawVideo)
			{
				VolumeIconY = VOLUME_ALL_ICON_Y;
			}			

			RECT rect = {MUTE_ICON_XPOS, VolumeIconY, 
						 MUTE_ICON_XPOS + VOLUME_TOTAL_W , VolumeIconY + VOLUME_TOTAL_H};
			InvalidateRect(hDlg, &rect, TRUE);
			
			// 保存铃声或通话音量
			if (g_InterState == CALL_STATE_CALLING)
			{
				storage_set_volume(g_volume, storage_get_talkvolume(), storage_get_keykeep());
			}
			else
			{
				storage_set_volume(storage_get_ringvolume(), g_volume, storage_get_keykeep());
			}
		}
		g_ticks++;
	}	

	#ifdef _AUTO_CAPTURE_
	if (g_autocapturetime > 0)
	{
		g_autocapturetime --;
		if (g_autocapturetime == 0)
		{
			uint32 ret = linkage_auto_capture();
			if (ret == TRUE)
			{
				ret = inter_video_snap();
				if (ret == FALSE)
				{
					show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Inter_CaptureFailed);
				}
				else
				{
					show_msg(hDlg,IDC_Msg_Notice, MSG_RIGHT, SID_Inter_CaptureSuccess);
				}

			}
		}
	}
	#endif
	
	return TRUE;
}

/*************************************************
  Function:			init_right_edit_mode
  Description: 		设置右边控件的初始值
  Input:			无
  Output:			无
  Return:			无
  Others:		
*************************************************/
static void init_right_edit_mode(void)
{
	if (g_OperType == INTER_CALLOUT_E)
	{
		ImageId[0] = 0;
		ImageId[1] = 0;
		ImageId[2] = 0;
		ImageId[3] = 0;
		ImageId[4] = BID_Right_Exit;

		TextId[0] = 0;
		TextId[1] = 0;
		TextId[2] = SID_Inter_HangUP; 
		TextId[3] = 0;
		TextId[4] = 0;		
	}
	else if (g_OperType == INTER_CALLIN_E)
	{
		switch (g_DevType)
		{
			case DEVICE_TYPE_ROOM:
			case DEVICE_TYPE_FENJI_NET:
				ImageId[0] = 0;
				ImageId[1] = 0;
				ImageId[2] = 0;
				ImageId[3] = 0;
				ImageId[4] = BID_Right_Exit;

				TextId[0] = 0;
				TextId[1] = 0;
				TextId[2] = SID_Inter_Answer;
				TextId[3] = 0;
				TextId[4] = 0;	
				break;

			case DEVICE_TYPE_MANAGER:
				#ifdef _MANAGER_IS_VIDEO_
				ImageId[0] = 0;
				ImageId[1] = 0;
				ImageId[2] = BID_Right_Full;
				ImageId[3] = 0;
				ImageId[4] = BID_Right_Exit;

				TextId[0] = SID_Inter_Snap;
				TextId[1] = SID_Inter_Answer;
				TextId[2] = 0;
				TextId[3] = 0;
				TextId[4] = 0;	
				#else
				ImageId[0] = 0;
				ImageId[1] = 0;
				ImageId[2] = 0;
				ImageId[3] = 0;
				ImageId[4] = BID_Right_Exit;

				TextId[0] = 0;
				TextId[1] = 0;
				TextId[2] = SID_Inter_Answer;
				TextId[3] = 0;
				TextId[4] = 0;
				#endif
				break;
				
			case DEVICE_TYPE_AREA:
			case DEVICE_TYPE_STAIR:
			case DEVICE_TYPE_DOOR_PHONE:
			case DEVICE_TYPE_DOOR_NET:
				ImageId[0] = 0;
				ImageId[1] = 0;  
				ImageId[2] = 0;
				ImageId[3] = BID_Right_Full;
				ImageId[4] = BID_Right_Exit;

				TextId[0] = SID_Inter_Unlock;
				TextId[1] = SID_Inter_Answer;
				TextId[2] = SID_Inter_Snap;
				TextId[3] = 0;
				TextId[4] = 0;	
				break;

			default:
				break;
		}
	}

	if (g_RightWin)
	{
		int i;
		
		for ( i = 0; i < RIGHT_NUM_MAX; i++)
		{
			SendMessage(g_RightWin, WM_Right_Set_Image, i, (LPARAM)ImageId[i]);
			SendMessage(g_RightWin, WM_Right_Set_TextID, i, (LPARAM)TextId[i]);
		}
	}
}

/*************************************************
  Function:			CreateRightCtrl
  Description: 		初始化右边控件
  Input:		
  	1.hDlg			窗口句柄
  Output:			无
  Return:			TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;
	
	init_right_edit_mode();
	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 1;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
	g_rightCtrl.parent = hDlg;
	for (i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageId[i];
		g_rightCtrl.buttons[i].TextID = TextId[i]; 
	}

	g_RightWin = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);	
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
}

/*************************************************
  Function:		ui_becall_create
  Description: 	被叫初始化
  Input:			
  	1.hDlg      窗口句柄
  	2.wParam	参数1
  	3.lParam    参数2
  Output:		无
  Return:			
  Others:		
*************************************************/
static uint32 ui_becall_create(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
    g_BecallWin = hDlg; 
	create_ctrls(hDlg);
	
	return TRUE;
}

/*************************************************
  Function:		WindowProc
  Description: 	消息处理函数
  Input: 		
  	1.hDlg		窗口句柄
  	2.message	消息
  	3.wParam	参数1
  	4.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static int WindowProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	log_printf("message: %04x\n", message);
	switch (message) 
	{
		case MSG_CREATE:
			ui_becall_create(hDlg, wParam, lParam);
			break;

		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			//SetTimer(hDlg, GUI_TIMER_ID, 50);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				//init_right_edit_mode();
				SetTimer(hDlg, GUI_TIMER_ID, 50);
			}
			else
			{
				// 逻辑会回调发关闭命令
				//close_page();
			}
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;

		case MSG_LBUTTONDOWN:
   			break;
   			
	    case MSG_LBUTTONUP:
	        break;

		case MSG_KEYDOWN:
			switch((int)wParam)
			{
				case SCANCODE_F8:					// 开锁
					wParam = RIGHT_SCANCODE_F1;
					break;
					
				case SCANCODE_F9:					// 通话
					if (g_DrawVideo)
					{
						wParam = RIGHT_SCANCODE_F2;
					}
					else
					{
						wParam = RIGHT_SCANCODE_F3;
					}
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
					if (g_DrawVideo)
					{
						wParam = RIGHT_SCANCODE_F2;
					}
					else
					{
						wParam = RIGHT_SCANCODE_F3;
					}
					break;
					 
				default:
					break;
			}
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
			break;

		case MSG_COMMAND:
			OnCommand(wParam, (HWND)lParam, hDlg);
			break;

		case MSG_TIMER:
			call_tick_proc(hDlg, wParam, lParam);
			break;	
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
			
		case WM_CENTER_HANDUP:
			if (g_DevType == DEVICE_TYPE_MANAGER)
			{
				inter_hand_down();
			}
			break;
			
		case MSG_DESTROY:
			if (hDlg)
			{
				KillTimer(hDlg, GUI_TIMER_ID);
			}
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

