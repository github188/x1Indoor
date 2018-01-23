/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_set_date_time.c
  Author:     	wufn
  Version:    	2.0
  Date: 		2014-10-25
  Description:  
				时间设置界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"
#include "storage_include.h"

/************************常量定义************************/

/************************变量定义************************/
static INPUT_INFO g_input[5];
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_RightWin = 0;
static HWND g_InputWin[5] = {0};
static HWND g_KeybordWin = 0;
#define MAX_INPUT				5					//输入框个数

#define INPUT_ICON_W			122
#define INPUT_ICON_H			42
#define INPUT_CTRL_PW_W			INPUT_ICON_W
#define INPUT_CTRL_PW_H			INPUT_ICON_H
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define HEAD_H					84	
#define HEAD_XPOS				0
#define HEAD_YPOS				52

#define INPUT_DISX				30
#define INPUT_LINE1_X			(SCREEN_WIDTH -FORM_RIGHT_W - 3*INPUT_ICON_W -2*INPUT_DISX)/2
#define INPUT_LINE1_Y			74
#define INPUT_LINE2_X			(SCREEN_WIDTH -FORM_RIGHT_W - 2*INPUT_ICON_W -INPUT_DISX)/2
#define INPUT_LINE2_Y			152

#define MAX_YEAR				2030
static ZONE_DATE_TIME g_time;

/************************函数定义************************/
CREATE_FORM(FormSetTimer);

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
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
}

/*************************************************
  Function:		draw_string
  Description: 	画背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_string(HDC hdc)
{
	RECT TextRc;
	
	if (storage_get_language() != ENGLISH)
	{
		SelectFont(hdc, GetFont(FONT_16));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(FONT_16));
	}
	SetTextColor(hdc, COLOR_BLACK);

	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	TextRc.left = TOP_TEXT_XPOS ;
	TextRc.top =  (TOP_TEXT_H - WIN_COMM_FONT)/2 ;
	TextRc.right = TextRc.left + 300;
	TextRc.bottom = TextRc.top + WIN_COMM_FONT;
	DrawText(hdc, get_str(SID_Set_System_Time), -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	DrawBmpID(hdc, HEAD_XPOS, HEAD_YPOS, 0, 0, BID_ListCtrl_TopBK5_0);
	DrawBmpID(hdc, HEAD_XPOS, HEAD_YPOS+HEAD_H, 0, 0, BID_ListCtrl_BK5_0);
	
	return TRUE;
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
	draw_string(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		save_param
  Description: 	画桌面
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void save_param(HWND hDlg)
{
	g_time.year = SendMessage(g_InputWin[0], WM_Input_Get_Text_Value, 0, 0);
	g_time.month = SendMessage(g_InputWin[1], WM_Input_Get_Text_Value, 0, 0);
	g_time.day = SendMessage(g_InputWin[2], WM_Input_Get_Text_Value, 0, 0);
	g_time.hour = SendMessage(g_InputWin[3], WM_Input_Get_Text_Value, 0, 0);
	g_time.min = SendMessage(g_InputWin[4], WM_Input_Get_Text_Value, 0, 0);
	g_time.sec = 0;

	if(1971 > g_time.year || 1 > g_time.month || 1 > g_time.day ||
		1 > g_time.hour || 1 > g_time.min)
	{		
		show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Save_ERR);
	}
	else
	{		
		set_rtc_timer(&g_time);
		show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Save_OK);	
	}	
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 image = SendMessage(g_RightWin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch(image)
	{
	    case BID_Right_Exit:
			close_page();
			break;
			
	    case SID_Right_Save:
			save_param(hDlg);
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
	2.hWnd		控件句柄
	3.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(HWND hDlg, WPARAM id, LPARAM lParam)
{
	switch(id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
            break;
			
		case IDC_INPUT_CTRL:
			//InputCtrlCommand1(hDlg, lParam);
			break;

		case IDC_INPUT1_CTRL:
			//InputCtrlCommand2(hDlg, lParam);
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
  Function:		Init_Key
  Description: 	
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Init_Key(void)
{
	add_rightctrl_win(g_RightWin, RIGHT_KEY0, g_KeybordWin, WM_Key_Left, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY1, g_KeybordWin, WM_Key_Right, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY2, g_KeybordWin, WM_Key_Ok, 0, 0);
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
	uint32 ImageID[RIGHT_NUM_MAX] = {BID_Right_Left, BID_Right_Right, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, SID_Right_Input, SID_Right_Save, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
	
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
  Function:		CreateInputCtrl
  Description: 	初始化输入框控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateInputCtrl(HWND hDlg)
{
	uint8 i;
	uint16 maxdata[5] = {MAX_YEAR, 12, 31, 23, 59};
	uint32 data[5];
	char tem[10];
	uint32 xpos, ypos;
	
	get_timer(&g_time);
	data[0] = g_time.year;
	data[1] = g_time.month;	
	data[2] = g_time.day;	
	data[3] = g_time.hour;	
	data[4] = g_time.min;	
	
	xpos = INPUT_LINE1_X;
	ypos = INPUT_LINE1_Y;	
	for (i = 0; i < 3; i++)
	{
		memset(&g_input[i], 0, sizeof(INPUT_INFO));

		g_input[i].parent = hDlg;
		g_input[i].mode = INPUT_MODE_TIME;
		
		if (i == 0)
		{
			g_input[i].maxlen = 4;
		}
		else
		{
			g_input[i].maxlen = 2;
		}
		
		g_input[i].BkID = BID_InputButton_date1;
		g_input[i].BkOnID = BID_InputButton_date2;
		g_input[i].showtype = 1;
		
		g_input[i].maxdata = maxdata[i];

		g_InputWin[i] = CreateWindow(AU_INPUT_CTRL, "", 
					WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
					IDC_INPUT_CTRL, 
					xpos, ypos,
					INPUT_CTRL_PW_W, INPUT_CTRL_PW_H, 
					hDlg, (DWORD)&g_input[i]);	
		
		xpos += INPUT_DISX + INPUT_ICON_W;
		
		if (i > 0)
		{
			SendMessage(g_InputWin[i], WM_Input_Set_Active, 0, 1);
		}
		memset(tem,0,sizeof(tem));
		sprintf(tem ,"%d",data[i]);
		SendMessage(g_InputWin[i], WM_Input_Set_Text, 0, tem);
	}

	xpos = INPUT_LINE2_X;
    ypos = INPUT_LINE2_Y;		
	for (i = 3; i < 5; i++)
	{
		memset(&g_input[i], 0, sizeof(INPUT_INFO));

		g_input[i].parent = hDlg;
		g_input[i].mode = INPUT_MODE_TIME;
		g_input[i].maxlen = 2;
		g_input[i].BkID = BID_InputButton_date1;
		g_input[i].BkOnID = BID_InputButton_date2;
		g_input[i].showtype = 1;
		
		g_input[i].maxdata = maxdata[i];

		g_InputWin[i] = CreateWindow(AU_INPUT_CTRL, "", 
					WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
					IDC_INPUT_CTRL, 
					xpos, ypos,
					INPUT_CTRL_PW_W, INPUT_CTRL_PW_H, 
					hDlg, (DWORD)&g_input[i]);	
		
		xpos += INPUT_DISX + INPUT_ICON_W;
		SendMessage(g_InputWin[i], WM_Input_Set_Active, 0, 1);
		memset(tem,0,sizeof(tem));
		sprintf(tem ,"%d",data[i]);
		SendMessage(g_InputWin[i], WM_Input_Set_Text, 0, tem);
	}	
}

/*************************************************
  Function:		CreateKeyBord
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateKeyBordCtrl(HWND hDlg)
{
	uint8 i;
   	memset(&g_keyboard, 0, sizeof(KEYBOARD_INFO));
	
	for (i = 0; i < MAX_INPUT; i++)
	{
    	g_keyboard.InputhDlg[i] = g_InputWin[i];
	}
	g_keyboard.InputNum = MAX_INPUT;
	g_keyboard.Max = KEYBORD_NUM_MAX;
	g_keyboard.Mode = KEYBOARD_MODE0;
	
	g_KeybordWin = CreateWindow(NUM_KEYBORD, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_KEYBOARD_CTRL, 
				KEYBOARD_CTRL_X, KEYBOARD_CTRL_Y,
				KEYBOARD_CTRL_W, KEYBOARD_CTRL_H, 
				hDlg, (DWORD)&g_keyboard);	
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
   	CreateInputCtrl(hDlg);
	CreateKeyBordCtrl(hDlg);
    CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		ui_passin_win
  Description: 		
  Input:			
  	1.win		窗体
  	2.filePath  文件名
  	3.param     
  Output:		无
  Return:			
  Others:		
*************************************************/
uint32 ui_datetime_win(HWND hDlg)
{	
	form_show(&FormSetTimer);
	return TRUE;
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
			if (wParam == SW_SHOWNORMAL)
			{
				Init_Key();
			}
			#if 0
			if (wParam == SW_HIDE)
			{
				uint8 i;
				for (i = 0; i < g_maxinput; i++)
				{
					SendMessage(g_InputWin[i], WM_Input_Clear, 0, 0);	
				}
				SendMessage(g_KeybordWin, WM_Key_Exit, 0, 0);
			}
			#endif
			else if (wParam == SW_HIDE)
			{
				close_page();
			}
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
			SendMessage(g_RightWin, MSG_KEYDOWN, wParam, lParam);
        	break;

		case MSG_KEYUP:
			SendMessage(g_RightWin, MSG_KEYUP, wParam, lParam);
			break;
			
		case MSG_COMMAND:
			#if 0
			if (g_InputWin[0] == wParam)
			{
				wParam = IDC_INPUT_CTRL;
			}
			if (g_InputWin[1] == wParam)
			{
				wParam = IDC_INPUT1_CTRL;
			}
			#endif
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

