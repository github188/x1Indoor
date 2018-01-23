/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_eightarea_ctrl.c
  Author:   	wufn
  Version:   	2.0
  Date: 		16-5-12
  Description:  
  				八防区控件
  History:                              
     1. Date:
        Author:
        Modification:
     2. ...
*********************************************************/
#include "gui_include.h"

#if (_LCD_DPI_ == _LCD_800480_)
#define ICON_BIG				88
#define ICON_SMALL				45
#define ICON_DIS_X				120
#define ICON_DIS_Y				80
#define ICON_TOLETT_DIS_X		90
#define TXT_TO_TOPAREA			15
#elif (_LCD_DPI_ == _LCD_1024600_)
#define ICON_BIG				88
#define ICON_SMALL				45
#define ICON_DIS_X				120
#define ICON_DIS_Y				80
#define ICON_TOLETT_DIS_X		90
#define TXT_TO_TOPAREA			15
#endif

#define GUI_TIMER_ID  			2
static uint8 g_Timer_On = 0;					// 定时器开启状态 1 开 0 未开

/*************************************************
  Function:		ui_reflash_ctrl
  Description: 	刷新控件
  Input:		无	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_ctrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, TRUE);
}

/*************************************************
  Function:		draw_eightareas
  Description: 	画防区图标名称
  Input:		无	
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_eightareas(HDC hdc,PEIGHTAREACTRL_INFO WObj)
{
	int i,y,x;
	RECT textRect;
	
	for (i = 0; i < WObj->MaxNum; i++)
	{	
		y = ICON_DIS_Y + (ICON_BIG + WIN_COMM_FONT) * (i/4) + ICON_DIS_Y * (i/4);
		x =	ICON_TOLETT_DIS_X + ICON_DIS_X * (i%4);

		if (WObj->Index == i && WObj->State == AREA_KEYDOWN)
		{
			DrawBmpID(hdc, x, y, ICON_BIG, ICON_BIG, WObj->Areas[i].ImageBK + 1);	
		}
		else
		{
			DrawBmpID(hdc, x, y, ICON_BIG, ICON_BIG, WObj->Areas[i].ImageBK);	
		}
		
		if(TRUE == WObj->Areas[i].Enabled)
		{
			if (0 != WObj->Areas[i].Flash_imageID)
			{
				if (0 == WObj->Areas[i].Flash_state)
				{
					DrawBmpID(hdc, x + (ICON_BIG-ICON_SMALL)/2, y + (ICON_BIG-ICON_SMALL)/2, ICON_SMALL, ICON_SMALL, WObj->Areas[i].ImageID);
				}
				else
				{
					DrawBmpID(hdc, x + (ICON_BIG-ICON_SMALL)/2, y + (ICON_BIG-ICON_SMALL)/2, ICON_SMALL, ICON_SMALL, WObj->Areas[i].Flash_imageID);
				}
			}
			else
			{
				DrawBmpID(hdc, x + (ICON_BIG-ICON_SMALL)/2, y + (ICON_BIG-ICON_SMALL)/2, ICON_SMALL, ICON_SMALL, WObj->Areas[i].ImageID);
			}
		}
		else
		{
			DrawBmpID(hdc, x + (ICON_BIG-ICON_SMALL)/2, y + (ICON_BIG-ICON_SMALL)/2, ICON_SMALL, ICON_SMALL, WObj->Areas[i].ImageID);//need modify later
		}

		if ((strlen(get_str(WObj->Areas[i].TextID)) * WIN_COMM_FONT/2) > ICON_BIG)
		{
			textRect.left = x - ((strlen(get_str(WObj->Areas[i].TextID)) * WIN_COMM_FONT/2) - ICON_BIG)/2;
		}
		else
		{
			textRect.left = x;
		}
		textRect.top= y + ICON_BIG + TXT_TO_TOPAREA;
		if ((strlen(get_str(WObj->Areas[i].TextID)) * WIN_COMM_FONT/2) > ICON_BIG)
		{
			textRect.right= textRect.left + strlen(get_str(WObj->Areas[i].TextID)) * WIN_COMM_FONT/2;
		}
		else
		{
			textRect.right= textRect.left +ICON_BIG ;
		}
		textRect.bottom= textRect.top + WIN_COMM_FONT;
		if(WObj->Areas[i].TextID != 0)
		{
			SetTextColor(hdc, COLOR_BUTTON_TEXT);
			SelectFont(hdc, GetFont(WIN_COMM_FONT));
			DrawText(hdc, get_str(WObj->Areas[i].TextID), -1, &textRect, DT_CENTER | DT_BOTTOM | DT_VCENTER);
		}
	}
}

/*************************************************
  Function:		OnPaint
  Description: 	画屏消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void OnPaint(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);

	if (NULL == WObj)
	{
		log_printf("WObj is NULL!!!");
		return;
	}

	HDC hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rect);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetBrushColor(hdc, PAGE_BK_COLOR);
	FillBox(hdc, rect.left, rect.top, rect.right, rect.bottom);
	draw_eightareas(hdc,WObj);
	EndPaint(hDlg, hdc);
}

#if 0
/*************************************************
  Function:		get_pos
  Description: 	获取选中的防区号
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int8 get_pos(HWND hDlg ,int x, int y)
{
	int i;
	RECT rect;
	GetClientRect(hDlg, &rect); 
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	
	for(i=0; i<WObj->MaxNum; i++)
	{
		if((x >= rect.left+ ICON_DIS_X*(i%4))
			&& (x <= rect.left + ICON_DIS_X*(i%4) + ICON_BIG)
			&& (y >= rect.top + ICON_DIS_Y*(i/4))
			&& (y <= rect.top + ICON_DIS_Y*(i/4) + ICON_BIG))
		{
			if(TRUE == WObj->Areas[i].Enabled)
			{
				return i;
			}
		}
	}
	
	return -1;
}
#endif

/*************************************************
  Function:		EightAreaCtrlSetText
  Description:  设置文字
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetText(HWND hDlg ,  WPARAM wParam, LPARAM lParam)
{
	int index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
		if (index < AREA_NUM_MAX && WObj->Areas[index].TextID != *(uint16*)lParam)
		{
			WObj->Areas[index].TextID = *(uint16*)lParam;
			WObj->Areas[index].Enabled = TRUE;
			ui_reflash_ctrl(hDlg, wParam, lParam);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlGetText
  Description:  获取文字
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlGetText(HWND hDlg,  WPARAM wParam, LPARAM lParam)
{
	int index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
		if (index < AREA_NUM_MAX)
		{
			return WObj->Areas[index].TextID;
		}
	}
	
	return 0;
}

/*************************************************
  Function:		EightAreaCtrlSetTextW
  Description:  设置文字
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetTextW(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
			
		if (index < AREA_NUM_MAX)
		{
			memcpy(WObj->Areas[index].Text,(uint16*)lParam, EADLG_TXTLEN);
			WObj->Areas[index].TextID = 0;
			WObj->Areas[index].Enabled = TRUE;
			ui_reflash_ctrl(hDlg, wParam, lParam);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlGetTextW
  Description:  获取文字
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlGetTextW(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	
	if (WObj)
	{
		index = (int)wParam;
		if (index < AREA_NUM_MAX)
		{
			return (uint32)WObj->Areas[index].Text;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlSetImage
  Description:  设置图片
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetImage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
		if (index < AREA_NUM_MAX && WObj->Areas[index].ImageID != *(uint16*)lParam)
		{
			WObj->Areas[index].ImageID = *(uint16*)lParam;
			WObj->Areas[index].Enabled = TRUE;
			ui_reflash_ctrl(hDlg, wParam, lParam);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlSetMidImage
  Description:  设置中间层图片
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	设置的项的序号(从1开始)，如果为0则设置全部
  	3.lParam	0-7位对应0-7项的状态
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32  EightAreaCtrlSetMidImage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	uint8 data = 0;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
		data  = *(uint32*)lParam;

		if(WObj->Areas[index].MidBgImage != data)
		{
			WObj->Areas[index].MidBgImage = data;
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
		return TRUE;
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlGetImage
  Description:  获取图片
  Input			无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlGetImage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (int)wParam;
		if (index < AREA_NUM_MAX)
		{
			return WObj->Areas[index].ImageID;
		}
	}
	
	return 0;
}

/*************************************************
  Function:		EightAreaCtrlSetEnable
  Description:  设置使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetEnable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	
	if (WObj)
	{
		index = wParam;
		if (index < AREA_NUM_MAX && WObj->Areas[index].Enabled != *(uint16*)lParam)
		{
			WObj->Areas[index].Enabled = *(uint16*)lParam;
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlGetEnable
  Description:  获取使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32  EightAreaCtrlGetEnable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint32 index;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = (uint32)wParam;
		if (index < AREA_NUM_MAX)
		{
			return WObj->Areas[index].Enabled;
		}
	}
	
	return 0;
}

/*************************************************
  Function:		EightAreaCtrlGetEnable
  Description:  获取最大防区数
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetMaxNum(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int maxnum;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		maxnum = wParam;
		if (maxnum <= AREA_NUM_MAX && WObj->MaxNum != maxnum)
		{
			WObj->MaxNum = maxnum;
			ui_reflash_ctrl(hDlg, wParam, lParam);
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlSetBKImage
  Description:  设置防区背景图片
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 EightAreaCtrlSetBKImage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int index;
	uint32 Image;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		index = wParam;
		Image = *(uint32*)lParam;
		if (index < AREA_NUM_MAX && WObj->Areas[index].ImageBK != Image)
		{
			WObj->Areas[index].ImageBK = Image;
			ui_reflash_ctrl(hDlg, wParam, lParam);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		EightAreaCtrlSetParam
  Description:  设置参数
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void  EightAreaCtrlSetParam(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 i;
	uint8 timer = 0;
	
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		memcpy(WObj,(PEIGHTAREACTRL_INFO)lParam,sizeof(EIGHTAREACTRL_INFO));
		for (i = 0; i < AREA_NUM_MAX; i++)
		{
			if (0 != WObj->Areas[i].Flash_imageID)
			{
				WObj->Areas[i].Flash_state = (WObj->Areas[i].Flash_state+1)%2;
				ui_reflash_ctrl(hDlg, wParam, lParam);
				timer = 1;
			}
		}
	}
	
	if (1 == timer)
	{
		if(0 == g_Timer_On)
		{
			g_Timer_On = 1;
			SetTimer(hDlg, GUI_TIMER_ID, 50);
		}
	}
	else if(1 == g_Timer_On)
	{
		g_Timer_On = 0;
		KillTimer(hDlg, GUI_TIMER_ID);
	}
}

/*************************************************
  Function:		EightAreaWinTick
  Description:  
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void  EightAreaWinTick(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 i; 
	PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		for (i = 0; i < AREA_NUM_MAX; i++)
		{
			if (0 != WObj->Areas[i].Flash_imageID)
			{
				WObj->Areas[i].Flash_state = (WObj->Areas[i].Flash_state+1)%2;
				ui_reflash_ctrl(hDlg, wParam, lParam);
			}
		}
	}
}

/*************************************************
  Function:		creat_ctrls
  Description: 	无 
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	UHRECT 区域
  	3.lParam	PRIGHTCTRL_INFO 结构
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 creat_ctrls(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 i;
	
	PEIGHTAREACTRL_INFO WObj = NULL;
	PEIGHTAREACTRL_INFO WObjTmp = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	WObj = malloc(sizeof(EIGHTAREACTRL_INFO));
	if (NULL == WObjTmp || WObj == NULL)
	{
		if (WObj)
		{
			
	    	free(WObj);
			WObj = NULL;
		}
		return FALSE;
	}
	
	memset(WObj,0,sizeof(EIGHTAREACTRL_INFO));
	memcpy(WObj,WObjTmp,sizeof(EIGHTAREACTRL_INFO));
	for (i = 0; i < AREA_NUM_MAX; i++)
	{
		WObj->Areas[i].ImageBK = BID_FQ_Icon_BJ;
	}

	return TRUE;
}

/*************************************************
  Function:		EightAreaCtrlDestroy
  Description: 	无 
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 EightAreaCtrlDestroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEIGHTAREACTRL_INFO WObj = (PEIGHTAREACTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		KillTimer(hDlg, WObj->TimerID);
		g_Timer_On = 0;
    	//free(WObj);
		//WObj = NULL;
	}

    return TRUE;
}

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
static int MyControlProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
		case MSG_CREATE:
			creat_ctrls(hDlg, wParam, lParam);
			break;

		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;	
			
		case MSG_PAINT:
			OnPaint(hDlg, wParam, lParam);
			break;
		
		#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
   			break;
   			
	    case MSG_LBUTTONUP:
	        break;
		#endif

		case MSG_KEYDOWN:
			break;
			
		case MSG_KEYUP:
        	break;
			
		case MSG_TIMER:
			EightAreaWinTick(hDlg, wParam, lParam);
			break;

		case WM_Eight_Set_Text:
			EightAreaCtrlSetText(hDlg, wParam, lParam);
			break;

		case WM_Eight_Get_Text:
			return EightAreaCtrlGetText(hDlg, wParam, lParam);

		case WM_Eight_Set_TextW:
			EightAreaCtrlSetTextW(hDlg, wParam, lParam);
			break;

		case WM_Eight_Get_TextW:
			return EightAreaCtrlGetTextW(hDlg, wParam, lParam);

		case WM_Eight_Set_Image:
			EightAreaCtrlSetImage(hDlg, wParam, lParam);
			break;

		case WM_Eight_Set_MidImage:
			EightAreaCtrlSetMidImage(hDlg, wParam, lParam);
			break;

		case WM_Eight_Get_Image:
			return EightAreaCtrlGetImage(hDlg, wParam, lParam);

		case WM_Eight_Set_enable:
			EightAreaCtrlSetEnable(hDlg, wParam, lParam);
			break;

		case WM_Eight_Get_enable:
			return EightAreaCtrlGetEnable(hDlg, wParam, lParam);

		case WM_Eight_Set_Param:
			EightAreaCtrlSetParam(hDlg, wParam, lParam);
			break;

		case WM_Eight_Set_MaxNum:
			EightAreaCtrlSetMaxNum(hDlg, wParam, lParam);
			break;

		case WM_Eight_Set_BKImage:
			EightAreaCtrlSetBKImage(hDlg, wParam, lParam);
			break;

		case WM_Eight_End_Timer:
			KillTimer(hDlg, GUI_TIMER_ID);
			break;
			
		case MSG_DESTROY:
			EightAreaCtrlDestroy(hDlg, wParam, lParam);
			break;
	}

	return DefaultControlProc(hDlg, message, wParam, lParam);
}	

/*************************************************
  Function:		register_eightarea_ctrl
  Description: 	注册八防区控件
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
BOOL register_eightarea_ctrl(void)
{
	WNDCLASS MyClass;

	MyClass.spClassName = AU_EIGHTAREA_CTRL;
	MyClass.dwStyle = WS_NONE;
	MyClass.dwExStyle = WS_EX_NONE;
	MyClass.hCursor = GetSystemCursor(IDC_ARROW);
	MyClass.iBkColor = COLOR_lightwhite;
	MyClass.WinProc = MyControlProc;

	return RegisterWindowClass(&MyClass);
}

/*************************************************
  Function:		unregister_eightarea_ctrl
  Description: 	删除八防区控件
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void unregister_eightarea_ctrl(void)
{
	UnregisterWindowClass(AU_EIGHTAREA_CTRL);
}

