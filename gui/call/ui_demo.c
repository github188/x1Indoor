/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_dame.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-10-22
  Description:  
				测试界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"


#define _AVI_DEFINED_ 1
#define _MP3_DEFINED_ 0
#define _JPG_DEFINED_ 0

/************************变量定义************************/
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_RightWin = 0;
//static HWND g_DameWin =0;
//static uint8 g_flag = 0;



/************************函数定义************************/
CREATE_FORM(FrmDemo);

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
    DrawBmpID(hdc, 0, 0, 0, 0, BID_Inter_VideoWinBK);
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
	EndPaint(hDlg, hdc);
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
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 image = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, lParam, 0);

	switch (image)
	{
		case SID_MainBianli:
			log_printf("333333333.................\n");
			show_msg_hit(hDlg, IDC_FORM, MSG_MESSAGE, SID_Msg_Info_Hit);
			log_printf("444444.................\n");
			break;
			
		case SID_Msg_Yes:
			{
				#if _AVI_DEFINED_
				media_pause_lyly();
				#elif _MP3_DEFINED_
				media_pause_mp3();
				#elif _JPG_DEFINED_
				media_start_show_pict("/mnt/nand1-2/chenbh/jpeg/Normal.jpg", 0, 0, 380, 272);
				#endif
			}
			break;
			
		case SID_Msg_No:
			{
				#if _AVI_DEFINED_
				media_stop_lyly();
				//usleep(1000*500);
				media_play_lyly("/mnt/nand1-2/chenbh/avi/111.avi", NULL);
				#elif _MP3_DEFINED_
				media_play_mp3("/mnt/nand1-2/chenbh/avi/111.mp3", NULL);
				#elif _JPG_DEFINED_
				media_start_show_pict("/mnt/nand1-2/chenbh/jpeg/Normal_1000x1000.jpg", 0, 0, 380, 272);
				#endif
			}
			break;
			
	    case SID_Right_Sure:
			{
				#if _AVI_DEFINED_
				media_stop_lyly();
				//usleep(1000*500);
				media_play_lyly("/mnt/nand1-2/chenbh/avi/222.avi", NULL);
				#elif _MP3_DEFINED_
				media_play_mp3("/mnt/nand1-2/chenbh/avi/222.mp3", NULL);
				#elif _JPG_DEFINED_
				media_start_show_pict("/mnt/nand1-2/chenbh/jpeg/Normal_1000x1000.jpg", 50, 50, 200, 200);
				#endif
			}
			 break;
			 
		case BID_Right_Exit:
			#if _AVI_DEFINED_
			media_stop_lyly();
			#elif _MP3_DEFINED_
			mp3_play_stop();
			#elif _JPG_DEFINED_
			media_stop_show_pict();
			#endif
			exit_page();
		    break;
			 
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
static void OnCommand( HWND hDlg,WPARAM id, LPARAM lParam)
{
	switch(id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {SID_MainBianli, SID_Msg_Yes, SID_Msg_No, SID_Right_Sure, 0};

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
			 create_ctrls(hDlg);			
			break;
		
		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			return 0;
			
       	#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
    		get_key_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	        get_key_up(hDlg, LOWORD(lParam), HIWORD(lParam));
	        break;
        #endif
		
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
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

