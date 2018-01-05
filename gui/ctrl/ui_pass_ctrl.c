/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_pass_ctrl.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-11-20
  Description:  
				密码界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"

#define GUI_TIMER_ID            1     		 		//定时器ID
		
static INPUT_INFO g_input;
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_PassWin = 0;
static HWND g_RightWin = 0;
static HWND g_InputWin = 0;
static HWND g_KeybordWin = 0;
static uint32 g_HintStrID = 0;						// 提示文字ID
static MSG_INFO g_pData;

CREATE_FORM(FrmPass);

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
	DrawBmpID(hdc, 0, 0, 0, 0, BID_DEFAULT_BK);
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
  Function:		password_tick_proc
  Description: 	定时
  Input:		
  	1.hDlg      窗口句柄
  	2.wParam	参数1
  	3.lParam	参数2
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 password_tick_proc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	static uint8 ticks = 0;
	
 	if (g_HintStrID == SID_Msg_InputBox_Psw_Err)
	{
		if (ticks == 2)
		{   
		    g_HintStrID = 0;
			SendMessage(g_InputWin, WM_Input_Set_Hit, 0, 1);		
			ticks = 0;
		}
		ticks++;
	}

	return TRUE;
}

/*************************************************
  Function:		pass_oper
  Description: 	密码校验
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void pass_oper(void)
{
	int nlen = 0;
	int nlen1 = 0;
	char password1[10];
	char password2[10];
	char password3[10];
	char password4[10];
	char password5[10];
	char password6[10];
	
	MSG_EVENT event = MSG_EVENT_YES;
	uint8 quit = FALSE;
	
	memset(password1,0,sizeof(password1));
	memset(password2,0,sizeof(password2));
	memset(password3,0,sizeof(password3));
	memset(password4,0,sizeof(password4));
	
	nlen = strlen(g_pData.Pass);
	nlen1 = strlen(g_pData.adminPass);
	if (nlen > 0)
	{
		memcpy(password1,g_pData.Pass,nlen);
	}

	if (nlen1 > 0)
	{
		memcpy(password3,g_pData.adminPass,nlen1);
	}
	toXiechiPwd(password1, TRUE, password2);
	toXiechiPwd(password3, TRUE, password4);
	
	// 10-7-21，报警需加一个家政的挟持判断
	if (0xff == g_pData.xiechi)
	{
		if (1 == storage_get_doorserver(0))
		{
			memset(password5,0,sizeof(password5));
			memset(password6,0,sizeof(password6));
			sprintf(password5,"%s",storage_get_pass(PASS_TYPE_SERVER));
			
			toXiechiPwd(password5, TRUE, password6);
			if (0 == strcmp(g_pData.InputPass, password6))						
			{
				event = MSG_EVENT_XIECHI;
				quit = TRUE;
			}
			else if (0 == strcmp(g_pData.InputPass, password5))	
			{
				quit = TRUE;
			}
			else
			{
				quit = FALSE;	
			}
		}
		//pData->xiechi = 1;						// del by xiewr 20101214, 防止第二次不检测家政密码
	}

	if ((0 == strcmp(g_pData.InputPass, password2) || 0 == strcmp(g_pData.InputPass, password4)) && g_pData.xiechi)						// 挟持
	{
		event = MSG_EVENT_XIECHI;
		quit = TRUE;
	}

	if (FALSE == quit)
	{
		if (0 == strcmp(g_pData.InputPass, g_pData.Pass) || 0 == strcmp(g_pData.InputPass, g_pData.adminPass))
		{
			quit = TRUE;
		}
		else
		{
			quit = FALSE;	
		}
	}

	
	if (FALSE == quit)
	{
		g_HintStrID = SID_Msg_InputBox_Psw_Err;
		SendMessage(g_InputWin, WM_Input_Set_TextID, 0, g_HintStrID);
		memset(g_pData.InputPass, 0, sizeof(g_pData.InputPass));
	}
	else
	{
		SendMessage(g_pData.parent, MSG_COMMAND, g_pData.ID, event);
		close_page();
	}
}

/*************************************************
  Function:		check_password_valid
  Description: 	检测输入号码合法性
  Input:			
  	1.PassWord	输入密码
  Output:		无
  Return:		无
  Others:		做长度检测
*************************************************/
static void check_password_valid(char *PassWord)
{
	int8 len;
	if (PassWord == NULL)
	{
		return ;
	}
	
	len = strlen(PassWord);
	if (len < 0)
	{
		g_HintStrID = SID_Msg_InputBox_Psw_Err;
		SendMessage(g_InputWin, WM_Input_Set_TextID, 0, g_HintStrID);
	}
	else
	{
		pass_oper();
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
			SendMessage(g_pData.parent, MSG_COMMAND, IDC_FORM, MSG_EVENT_NO);
			close_page();
			break;
		 
		default:
			break;
    }
}

/*************************************************
  Function:		InputCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg		句柄
	2.id		返回的控件ID
	3.lParam	返回的私有数据
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void InputCtrlCommand(HWND hDlg, LPARAM lParam)
{
	memset(g_pData.InputPass, 0, sizeof(g_pData.InputPass));
	strcpy(g_pData.InputPass, (char *)lParam);
	check_password_valid(g_pData.InputPass);
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
	1.hDlg		句柄
	2.id		返回的控件ID
	3.lParam	返回的私有数据
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(HWND hDlg,WPARAM id, LPARAM lParam)
{
	switch(id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
            break;
			
		case IDC_INPUT_CTRL:
			InputCtrlCommand(hDlg, lParam);
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
  Description: 	关联按键
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void Init_Key(void)
{
	add_rightctrl_win(g_RightWin, RIGHT_KEY0, g_KeybordWin, WM_Key_Left, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY1, g_KeybordWin, WM_Key_Right, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY2, g_KeybordWin, WM_Key_Ok, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY3, g_InputWin, WM_Input_Send_Value, 0, 0);	
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
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, SID_Right_Input, SID_Right_Sure, 0};

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
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON |WS_EX_TRANSPARENT, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y,
				RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);	
	Init_Key();
}

/*************************************************
  Function:		CreateInputCtrl
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateInputCtrl(HWND hDlg)
{
    memset(&g_input, 0, sizeof(INPUT_INFO));
	g_input.parent = hDlg;
	g_input.mode = INPUT_MODE_PASSWORD;
	g_input.maxlen = PASSWORD_LEN;

	g_InputWin = CreateWindow(AU_INPUT_CTRL, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON |WS_EX_TRANSPARENT, 
				IDC_INPUT_CTRL, 
				INPUT_CTRL_X, INPUT_CTRL_Y,
				INPUT_CTRL_W, INPUT_CTRL_H, 
				hDlg, (DWORD)&g_input);	
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
   	memset(&g_keyboard, 0, sizeof(KEYBOARD_INFO));
	g_keyboard.InputNum = 1;
    g_keyboard.InputhDlg[0] = g_InputWin;
	g_keyboard.Max = KEYBORD_NUM_MAX;
	g_keyboard.Mode = KEYBOARD_MODE2;
	
	g_KeybordWin = CreateWindow(NUM_KEYBORD, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON |WS_EX_TRANSPARENT, 
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
    g_PassWin = hDlg;
   	CreateInputCtrl(hDlg);
	CreateKeyBordCtrl(hDlg);
    CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		ui_passin_win
  Description: 	无	
  Input:			
  	1.info		消息结构体指针
  Output:		无
  Return:			
  Others:		
*************************************************/
uint32 ui_passin_win(PMSG_INFO info)
{
	g_HintStrID = 0;
	memset(&g_pData, 0, sizeof(MSG_INFO));
	memcpy(&g_pData, info, sizeof(MSG_INFO));
	//form_show(&FrmPass);
	SendMessage(FrmMain.hWnd, WM_FORM_SHOW, 0, (PGUI_FORM)&FrmPass);
	
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
			SetTimer (hDlg, GUI_TIMER_ID, 50);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				SendMessage(g_InputWin, WM_Input_Set_Hit, 0, 1);
			}
			else if (wParam == SW_HIDE)
			{
				if (g_LcdState == SYS_LCD_CLOSE)
				{
					SendMessage(g_pData.parent, MSG_COMMAND, IDC_FORM, MSG_EVENT_NO);
				}
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
			if (g_InputWin == wParam)
			{
				wParam = IDC_INPUT_CTRL;
			}
			OnCommand(hDlg, wParam, lParam);
			break;
			
		case MSG_TIMER:	
			password_tick_proc(hDlg, wParam, lParam);
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

