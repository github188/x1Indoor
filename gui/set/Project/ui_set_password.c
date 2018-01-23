/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_set_password.c.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-10-25
  Description:  
				密码界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define LIST_ITEM_DISX			40
#define LIST_TOP_H				52
#define INPUT_ICON_W			102
#define INPUT_ICON_H			42
#define INPUT_CTRL_X_1	 		(FORM_W - RIGHT_CTRL_W - INPUT_ICON_W)/2
#define INPUT_CTRL_DISX_1		0				    // 一个输入框x坐标和间距
#define INPUT_CTRL_DISX			(FORM_W - RIGHT_CTRL_W - 2*INPUT_ICON_W)/3
#define INPUT_CTRL_DISX_2		INPUT_CTRL_DISX + INPUT_ICON_W	// 两个输入框x坐标和间距
#define INPUT_CTRL_X_2	 		(FORM_W - RIGHT_CTRL_W - 2*INPUT_ICON_W - INPUT_CTRL_DISX)/2					
#define INPUT_CTRL_PW_Y			(LIST_TOP_H + 30)	// password 输入框坐标
#define INPUT_CTRL_PW_W			INPUT_ICON_W
#define INPUT_CTRL_PW_H			INPUT_ICON_H

#define PW_MAX_NUM				2					// 密码输入框最大数
#define	 PW_TEXT_LEFT1			(INPUT_CTRL_X_2 - 50)
#define PW_TEXT_TOP1			INPUT_CTRL_PW_Y + 15//(INPUT_ICON_W -FONT_16)/2 
#define	 PW_TEXT_DISX			(INPUT_CTRL_DISX_2-15)	// 2个密码框时文字的距离
#define	 PW_TEXT_LEFT2			(INPUT_CTRL_X_1 - 50)
#define PW_TEXT_TOP2			PW_TEXT_TOP1
#define PW_TEXT					50
#elif (_LCD_DPI_ == _LCD_1024600_)
#define LIST_ITEM_DISX			50
#define LIST_TOP_H				65
#define INPUT_ICON_W			128
#define INPUT_ICON_H			50
#define INPUT_CTRL_X_1	 		(FORM_W - RIGHT_CTRL_W - INPUT_ICON_W)/2
#define INPUT_CTRL_DISX_1		0				    // 一个输入框x坐标和间距
#define INPUT_CTRL_DISX			(FORM_W - RIGHT_CTRL_W - 2*INPUT_ICON_W)/3
#define INPUT_CTRL_DISX_2		INPUT_CTRL_DISX + INPUT_ICON_W	// 两个输入框x坐标和间距
#define INPUT_CTRL_X_2	 		(FORM_W - RIGHT_CTRL_W - 2*INPUT_ICON_W - INPUT_CTRL_DISX)/2					
#define INPUT_CTRL_PW_Y			(LIST_TOP_H + 38)	// password 输入框坐标
#define INPUT_CTRL_PW_W			INPUT_ICON_W
#define INPUT_CTRL_PW_H			INPUT_ICON_H

#define PW_MAX_NUM				2					// 密码输入框最大数
#define	 PW_TEXT_LEFT1			(INPUT_CTRL_X_2 - 63)
#define PW_TEXT_TOP1			INPUT_CTRL_PW_Y + 19//(INPUT_ICON_W -FONT_16)/2 
#define PW_TEXT_DISX			(INPUT_CTRL_DISX_2-19)	// 2个密码框时文字的距离
#define	 PW_TEXT_LEFT2			(INPUT_CTRL_X_1 - 63)
#define PW_TEXT_TOP2			PW_TEXT_TOP1
#define PW_TEXT					50
#endif

/************************变量定义************************/
static INPUT_INFO g_input[2];
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_PassWin = 0;
static HWND g_RightWin = 0;
static HWND g_InputWin[2] = {0};
static HWND g_KeybordWin = 0;
static char g_PassWord1[10];
static char g_PassWord2[10];
static PASS_TYPE g_type = PASS_TYPE_ADMIN;
static uint8 g_maxinput = 0;						// 输入框个数	

/************************函数定义************************/
CREATE_FORM(FrmSetpassword);

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
	uint8 i;
	uint32 Title[PASS_TYPE_MAX] = {SID_Set_Prj_Pass,SID_Set_Alarm_UserPwd,
								   SID_Set_Pass_DoorUser,SID_Set_Pass_DoorServer,SID_Set_Pass_Server};

	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(WIN_COMM_FONT));
	TextRc.left = LIST_ITEM_DISX;
	TextRc.top = (LIST_TOP_H - WIN_COMM_FONT)/2;
	TextRc.right =TextRc.left + PW_TEXT;
	TextRc.bottom = TextRc.top + WIN_COMM_FONT;
	DrawText(hdc, get_str(Title[g_type]), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, get_str(Title[g_type]), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	switch (g_type)
	{
		case PASS_TYPE_ADMIN:
		case PASS_TYPE_USER:
			for (i = 0; i < PW_MAX_NUM; i++)
			{
				TextRc.left = PW_TEXT_LEFT1 + (i * PW_TEXT_DISX);
				TextRc.top = PW_TEXT_TOP1;
				TextRc.right =TextRc.left + PW_TEXT;
				TextRc.bottom = TextRc.top + WIN_COMM_FONT;
				DrawText(hdc, get_str(SID_Set_Pass_New +i), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
				DrawText(hdc, get_str(SID_Set_Pass_New +i), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			break;
		
		case PASS_TYPE_DOOR_USER:	
		case PASS_TYPE_DOOR_SERVER:
		case PASS_TYPE_SERVER:
			TextRc.left = PW_TEXT_LEFT2;
			TextRc.top = PW_TEXT_TOP2;
			TextRc.right = TextRc.left + PW_TEXT;
			TextRc.bottom = TextRc.top + WIN_COMM_FONT;
			DrawText(hdc, get_str(SID_Set_Pass_New), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			DrawText(hdc, get_str(SID_Set_Pass_New), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			break;

		default:
			break;
	}	
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
	char adminx[10];
	char userx[10];
	char admin[10];
	char user[10];
	char userdoor[10];
	char jzdoor[10];
	char userdoorx[10];
	char jzdoorx[10];
	char jzpwd[10];
	char jzpwdx[10];

	char adminx_1[10];
	char userx_1[10];
	char userdoorx_1[10];
	char jzdoorx_1[10];
	char jzpwdx_1[10];
	
	memset(adminx,0,sizeof(adminx));
	memset(userx,0,sizeof(userx));
	memset(admin,0,sizeof(admin));
	memset(user,0,sizeof(user));
	sprintf(admin,"%s",storage_get_pass(PASS_TYPE_ADMIN));
	sprintf(user,"%s",storage_get_pass(PASS_TYPE_USER));

	memset(userdoor,0,sizeof(userdoor));
	memset(jzdoor,0,sizeof(jzdoor));
	memset(userdoorx,0,sizeof(userdoorx));
	memset(jzdoorx,0,sizeof(jzdoorx));
	memset(jzpwd,0,sizeof(jzpwd));
	memset(jzpwdx,0,sizeof(jzpwdx));

	memset(adminx_1,0,sizeof(adminx_1));
	memset(userx_1,0,sizeof(userx_1));
	memset(userdoorx_1,0,sizeof(userdoorx_1));
	memset(jzdoorx_1,0,sizeof(jzdoorx_1));
	memset(jzpwdx_1,0,sizeof(jzpwdx_1));
	
	memset(g_PassWord1,0,sizeof(g_PassWord1));
	memset(g_PassWord2,0,sizeof(g_PassWord2));

	sprintf(jzpwd,"%s",storage_get_pass(PASS_TYPE_SERVER));
	sprintf(userdoor,"%s",storage_get_pass(PASS_TYPE_DOOR_USER));
	sprintf(jzdoor,"%s",storage_get_pass(PASS_TYPE_DOOR_SERVER));
	
	toXiechiPwd(admin, TRUE, adminx);
	toXiechiPwd(user, TRUE, userx);
	toXiechiPwd(userdoor, TRUE, userdoorx);
	toXiechiPwd(jzdoor, TRUE, jzdoorx);
	toXiechiPwd(jzpwd, TRUE, jzpwdx);

	toXiechiPwd(admin, FALSE, adminx_1);
	toXiechiPwd(user, FALSE, userx_1);
	toXiechiPwd(userdoor, FALSE, userdoorx_1);
	toXiechiPwd(jzdoor, FALSE, jzdoorx_1);
	toXiechiPwd(jzpwd, FALSE, jzpwdx_1);
	
	switch (g_type)
	{
		case PASS_TYPE_ADMIN:
		case PASS_TYPE_USER:
			SendMessage(g_InputWin[0], WM_Input_Get_Text, 0, (LPARAM)g_PassWord1);
			SendMessage(g_InputWin[1], WM_Input_Get_Text, 0, (LPARAM)g_PassWord2);
			if (strlen(g_PassWord1) != strlen(g_PassWord2))
			{
				SendMessage(g_InputWin[0], WM_Input_Clear, 0, 0);
				SendMessage(g_InputWin[1], WM_Input_Clear, 0, 0);	
				show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Msg_SamePass_Err);
				return;
			}
			if (0 == strcmp(g_PassWord1, g_PassWord2))
			{
				if (g_type == PASS_TYPE_ADMIN)
				{
					if (0 == strcmp(g_PassWord1, userx) || 0 == strcmp(g_PassWord1, jzpwdx)
						 || 0 == strcmp(g_PassWord1, userdoorx) || 0 == strcmp(g_PassWord1, jzdoorx))
					{
						log_printf("strcmp(g_PassWord1, userx) = %d \n strcmp(g_PassWord1, jzpwdx) = %d \n strcmp(g_PassWord1, userdoorx)= %d \n strcmp(g_PassWord1, jzdoorx)= %d\n",
							     strcmp(g_PassWord1, userx),strcmp(g_PassWord1, jzpwdx),strcmp(g_PassWord1, userdoorx),strcmp(g_PassWord1, jzdoorx));
						show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
						return;
					}

					if (0 == strcmp(g_PassWord1, userx_1) || 0 == strcmp(g_PassWord1, jzpwdx_1)
						 || 0 == strcmp(g_PassWord1, userdoorx_1) || 0 == strcmp(g_PassWord1, jzdoorx_1))
					{
						log_printf("strcmp(g_PassWord1, userx_1) = %d \n strcmp(g_PassWord1, jzpwdx_1) = %d \n strcmp(g_PassWord1, userdoorx_1)= %d \n strcmp(g_PassWord1, jzdoorx_1)= %d\n",
							     strcmp(g_PassWord1, userx_1),strcmp(g_PassWord1, jzpwdx_1),strcmp(g_PassWord1, userdoorx_1),strcmp(g_PassWord1, jzdoorx_1));
						show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
						return;
					}
				}
				else
				{
					if (0 == strcmp(g_PassWord1, adminx) || 0 == strcmp(g_PassWord1, jzpwdx)
						 || 0 == strcmp(g_PassWord1, userdoorx) || 0 == strcmp(g_PassWord1, jzdoorx))
					{
						show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
						return;
					}

					if (0 == strcmp(g_PassWord1, adminx_1) || 0 == strcmp(g_PassWord1, jzpwdx_1)
						 || 0 == strcmp(g_PassWord1, userdoorx_1) || 0 == strcmp(g_PassWord1, jzdoorx_1))
					{
						show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
						return;
					}
				}
				if (g_type == PASS_TYPE_USER)
				{
					storage_set_pass(g_type, g_PassWord1);	
					show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
				}
				else
				{
					storage_set_pass(g_type, g_PassWord1);	
					show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
				}
				
			}
			else
			{
				SendMessage(g_InputWin[0], WM_Input_Clear, 0, 0);
				SendMessage(g_InputWin[1], WM_Input_Clear, 0, 0);	
				show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Msg_SamePass_Err);
			}
		break;
			
		case PASS_TYPE_DOOR_USER:	
		case PASS_TYPE_DOOR_SERVER:
		case PASS_TYPE_SERVER:
			SendMessage(g_InputWin[0], WM_Input_Get_Text, 0, (LPARAM)g_PassWord1);
			#if 0
			if (strlen(g_PassWord1) == 0)
			{
				SendMessage(g_InputWin[0], WM_Input_Clear, 0, 0);	
				show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
				return;
			}
			#endif
			if (g_type == PASS_TYPE_DOOR_USER)
			{
				if (0 == strcmp(g_PassWord1, adminx) || 0 == strcmp(g_PassWord1, userx) || 0 == strcmp(g_PassWord1, jzpwdx)
					|| 0 == strcmp(g_PassWord1, jzdoorx))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}

				if (0 == strcmp(g_PassWord1, adminx_1) || 0 == strcmp(g_PassWord1, userx_1) || 0 == strcmp(g_PassWord1, jzpwdx_1)
					|| 0 == strcmp(g_PassWord1, jzdoorx_1))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}
			}else if (g_type == PASS_TYPE_DOOR_SERVER)
			{
				if (0 == strcmp(g_PassWord1, adminx) || 0 == strcmp(g_PassWord1, userx) || 0 == strcmp(g_PassWord1, jzpwdx)
						 || 0 == strcmp(g_PassWord1, userdoorx))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}

				if (0 == strcmp(g_PassWord1, adminx_1) || 0 == strcmp(g_PassWord1, userx_1) || 0 == strcmp(g_PassWord1, jzpwdx_1)
						 || 0 == strcmp(g_PassWord1, userdoorx_1))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}
			}
			else
			{
				if (0 == strcmp(g_PassWord1, adminx) || 0 == strcmp(g_PassWord1, userx)
						 || 0 == strcmp(g_PassWord1, userdoorx) || 0 == strcmp(g_PassWord1, jzdoorx))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}

				if (0 == strcmp(g_PassWord1, adminx_1) || 0 == strcmp(g_PassWord1, userx_1)
						 || 0 == strcmp(g_PassWord1, userdoorx_1) || 0 == strcmp(g_PassWord1, jzdoorx_1))
				{
					show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Oper_ERR);
					return;
				}
			}
	
			storage_set_pass(g_type, g_PassWord1);
			show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);	
			break;

		default:
			break;
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

#if 0
/*************************************************
  Function:		InputCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void InputCtrlCommand1(HWND hDlg, LPARAM lParam)
{
	memset(g_PassWord1, 0, sizeof(g_PassWord1));
    strcpy(g_PassWord1,(char *)lParam);
	log_printf("g_PassWord: %s\n",g_PassWord1);
	//save_param(hDlg);
	show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
}

/*************************************************
  Function:		InputCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void InputCtrlCommand2(HWND hDlg, LPARAM lParam)
{
	memset(g_PassWord2, 0, sizeof(g_PassWord2));
    strcpy(g_PassWord2,(char *)lParam);
	log_printf("g_PassWord1==== %s\n",g_PassWord2);
}
#endif

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
	#if 0
	for (i = 0; i < g_maxinput; i++)
	{
		add_rightctrl_win(g_RightWin, RIGHT_KEY3, g_InputWin[i], WM_Input_Send_Value, 0, 0);	
	}
	#endif
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
    uint8 i, max;
	uint8 showtype = 0, maxlen = 0;
	uint32 xpos = 0, disx = 0;
	INPUT_MODE mode;
	
	switch (g_type)
	{
		case PASS_TYPE_ADMIN:
			maxlen = MAX_ADMIN_LEN;
			max = 2;
			mode = INPUT_MODE_NEW;
			showtype = 0;
			xpos = INPUT_CTRL_X_2;
			disx = INPUT_CTRL_DISX_2;
			break;
			
		case PASS_TYPE_USER:
			maxlen = MAX_USER_LEN;
			max = 2;
			mode = INPUT_MODE_NEW;
			showtype = 0;
			xpos = INPUT_CTRL_X_2;
			disx = INPUT_CTRL_DISX_2;
			break;
			
		case PASS_TYPE_DOOR_USER:	
			maxlen = MAX_DOOR_USER_LEN;
			max = 1;
			mode = INPUT_MODE_COMMON;
			showtype = 1;
			xpos = INPUT_CTRL_X_1;
			disx = INPUT_CTRL_DISX_1;
			break;

		case PASS_TYPE_DOOR_SERVER:
			maxlen = MAX_DOOR_SERVER_LEN;
			max = 1;
			mode = INPUT_MODE_COMMON;
			showtype = 1;
			xpos = INPUT_CTRL_X_1;
			disx = INPUT_CTRL_DISX_1;
			break;
			
		case PASS_TYPE_SERVER:
			maxlen = MAX_DOOR_SERVER_LEN;
			max = 1;
			mode = INPUT_MODE_COMMON;
			showtype = 1;
			xpos = INPUT_CTRL_X_1;
			disx = INPUT_CTRL_DISX_1;
			break;
			
		default:
			break;
	}
	
	g_maxinput = max;
	for (i = 0; i < max; i++)
	{
	    memset(&g_input[i], 0, sizeof(INPUT_INFO));
		g_input[i].parent = hDlg;
		g_input[i].mode = mode;
		g_input[i].maxlen = maxlen;
		g_input[i].BkID = BID_InputButton_pass3;
		g_input[i].BkOnID = BID_InputButton_pass2;
		g_input[i].showtype = showtype;

		g_InputWin[i] = CreateWindow(AU_INPUT_CTRL, "", 
					WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
					IDC_INPUT_CTRL, 
					xpos + (i * disx), INPUT_CTRL_PW_Y,
					INPUT_CTRL_PW_W, INPUT_CTRL_PW_H, 
					hDlg, (DWORD)&g_input[i]);	
	}
	if (max == 2)
	{
		SendMessage(g_InputWin[1], WM_Input_Set_Active, 0, 1);
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
	
	for (i = 0; i < g_maxinput; i++)
	{
    	g_keyboard.InputhDlg[i] = g_InputWin[i];
	}
	g_keyboard.InputNum = g_maxinput;
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
    g_PassWin = hDlg;
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
uint32 ui_password_win(PASS_TYPE type)
{
	g_type = type;
	g_maxinput = 0;
	form_show(&FrmSetpassword);
	
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

