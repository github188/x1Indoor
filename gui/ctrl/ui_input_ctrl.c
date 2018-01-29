/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_input_ctrl.c
  Author:   	caogw
  Version:   	2.0
  Date: 		2014-10-22
  Description:  
   				输入框控件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define NUM_ICON_XPOS			173    				// 输入框中显示数字时x坐标
#define NUM_ICON_YPOS			12      			// 输入框中显示数字时y坐标
#define NUM_ICON_DISX			29    				// 输入框中显示数字的距离

#define INPUT_TEXT_X            0      
#define INPUT_TEXT_Y            0
#define INPUT_TEXT_W            INPUT_CTRL_W    
#define INPUT_TEXT_H            INPUT_CTRL_H     	// 输入框中提示字位置坐标

#define INPUT_TEXT1_X           10      
#define INPUT_TEXT1_Y           5      
#define INPUT_TEXT1_W           61   
#define INPUT_TEXT1_H           30  				// 输入框中提示字位置坐标
#elif (_LCD_DPI_ == _LCD_1024600_)
#define NUM_ICON_XPOS			217    				// 输入框中显示数字时x坐标
#define NUM_ICON_YPOS			19      			// 输入框中显示数字时y坐标
#define NUM_ICON_DISX			37    				// 输入框中显示数字的距离

#define INPUT_TEXT_X            0      
#define INPUT_TEXT_Y            0
#define INPUT_TEXT_W            INPUT_CTRL_W    
#define INPUT_TEXT_H            INPUT_CTRL_H     	// 输入框中提示字位置坐标

#define INPUT_TEXT1_X           13      
#define INPUT_TEXT1_Y           7       
#define INPUT_TEXT1_W           77   
#define INPUT_TEXT1_H           38  				// 输入框中提示字位置坐标
#endif

/*************************************************
  Function:		ui_reflash_inputctrl
  Description: 	刷新控件
   Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2 	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_inputctrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, TRUE);
}

/*************************************************
  Function:		ui_inputctrl_del_num
  Description: 	删除数字
   Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2 
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_del_num(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

	if (pData)
	{
		if (pData->len > 0)
		{
			pData->text[pData->len - 1] = 0;
			pData->len--;
			ui_reflash_inputctrl(hDlg, wParam, lParam);
		}
		
		if (pData->mode == INPUT_MODE_CALL)
		{
			if (0 == pData->len)
			{
			   pData->hit = 0;
			}
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		ui_InputCtrl_clear_num
  Description: 	清空数字
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_clear_num(HWND hDlg, WPARAM wParam, LPARAM lParam)
{ 
	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
   
	if (pData)
	{
		if (pData->len > 0)
		{
			memset(pData->text, 0, sizeof(pData->text));
			pData->len = 0;
			pData->hit = 0;
			ui_reflash_inputctrl(hDlg, wParam, lParam);
			
			return TRUE;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		draw_inputctrl_num
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.hdc		绘图句柄
  	3.wParam	参数1
  	4.lParam 	参数2 
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 draw_inputctrl_num(HWND hDlg,HDC hdc, WPARAM wParam, LPARAM lParam)
{   
	uint8 i;
    RECT TextRc;
    char num[50];
	uint32 xpos,ypos;
	uint32 g_strid[1];
	uint8 numlen = 0,start = 0;
	
   	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

	if (0 == pData->videomode)
	{
	  	DrawBmpID(hdc, 0, 0, 0, 0, BID_Input_Ctrl);
		g_strid[0] = pData->textid;
		if (0 == pData->hit)
		{
		    SetTextColor(hdc, COLOR_COMMON_TEXT);
			#if (_LCD_DPI_ == _LCD_800480_)
			SelectFont(hdc, GetFont(FONT_20));
			#elif (_LCD_DPI_ == _LCD_1024600_)
			SelectFont(hdc, GetFont(FONT_22));
			#endif
			TextRc.left = INPUT_TEXT_X;
			TextRc.top = INPUT_TEXT_Y;
			TextRc.right = INPUT_TEXT_W;
			TextRc.bottom = INPUT_TEXT_H;
			DrawTextFont(hdc, get_str(g_strid[0]), -1, &TextRc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);			
		}
			
		if (pData->len > 0)
		{
		    numlen = pData->len;
			memset(num,0,sizeof(num));
			memcpy(num,&(pData->text[start]),numlen);	    
		
	        xpos = (INPUT_CTRL_W - NUM_ICON_DISX*(pData->len))/2 +\
				NUM_ICON_DISX*(pData->len) - NUM_ICON_DISX;
			ypos = NUM_ICON_YPOS;
			for (i = numlen; i > 0; i--)
			{
				switch (num[i-1]-'0')
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
						if (pData->mode == INPUT_MODE_PASSWORD)
						{
							DrawBmpID(hdc, xpos, ypos, 0, 0, BID_NumXin);
						}
						else
						{
							DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Num0 + num[i-1] - '0');
						}						
						break;

                    default:
						break;
				}
				xpos -= NUM_ICON_DISX;
			}
		}
	}

	return TRUE;
}

/*************************************************
  Function:		draw_inputctrl_common
  Description: 	无
  	1.hDlg		窗体句柄
  	2.hdc		绘图句柄
  	3.wParam	参数1
  	4.lParam 	参数2 
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 draw_inputctrl_common(HWND hDlg,HDC hdc, WPARAM wParam, LPARAM lParam)
{
	RECT TextRc;
	PBITMAP bmp;
    char num[50], str[50];
	uint8 i;
	uint8 numlen = 0, start = 0;
	
   	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (0 == pData->videomode)
	{
		if (pData->active)
		{
			DrawBmpID(hdc, 0, 0, 0, 0, pData->BkID);
		}
		else
		{
			DrawBmpID(hdc, 0, 0, 0, 0, pData->BkOnID);
		}	
		pData->len = strlen(pData->text);
		if (pData->len > 0)
		{
		    numlen = pData->len;
			memset(num, 0, sizeof(num));
			memset(str, 0, sizeof(str));	    	

			if (0 == pData->showtype)
			{
				for (i = 0; i < numlen; i++)
				{
					str[i] = '*';
				}
				strcpy(num, str);
			}
			else
			{
				memcpy(num, &(pData->text[start]), numlen);
			}

			SetTextColor(hdc, COLOR_BLACK);
			SelectFont(hdc, GetFont(FONT_24));
			bmp = get_bmp(pData->BkID);
			TextRc.left = 0;
			TextRc.top = 0;
			TextRc.right = bmp->bmWidth;
			TextRc.bottom = bmp->bmHeight;
			DrawTextFont(hdc, num, -1, &TextRc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);	
		}	
  	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_paint
  Description: 	画屏消息
   Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_paint(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData->videomode)
	{
		return TRUE;
	}
	
   	HDC hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rect);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	
	switch (pData->mode)
	{
		case INPUT_MODE_PASSWORD:
		case INPUT_MODE_CALL:
			draw_inputctrl_num(hDlg,hdc, wParam, lParam);
			break;
			
		case INPUT_MODE_COMMON:
		case INPUT_MODE_NEW:
		case INPUT_MODE_TIME:
			draw_inputctrl_common(hDlg,hdc, wParam, lParam);
			break;

		default:
			break;
	}
    EndPaint(hDlg, hdc);
	
    return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_send_value
  Description:  发送输入值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_send_value(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	// 根据hDlg唯一性来判断是那个窗口发送的消息
	SendMessage(pData->parent, MSG_COMMAND, hDlg, (LPARAM)pData->text); 
	ui_inputctrl_clear_num(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_get_value
  Description:  获取当前值
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_get_value(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	switch(wParam)
	{
		case IDC_KEYBOARD_CTRL:
			
			switch(lParam)
			{	
			    case 0:
			    case 1:
			    case 2:
			    case 3:
			    case 4:
			    case 5:
			    case 6:
			    case 7:
			    case 8:
			    case 9:
					pData->hit = 1;
					if (pData->len < pData->maxlen)
					{
						pData->text[pData->len] = (char)(lParam +48); // 0字符ASCII码值为48
						
						if (pData->mode == INPUT_MODE_TIME)
						{
							if (atoi(pData->text) > pData->maxdata)
							{
								sprintf(pData->text, "%d", pData->maxdata);
							}
						}
						(pData->len)++;
					}
					
					ui_reflash_inputctrl(hDlg, wParam, lParam);
					break;
					
		    	default:
		  			break;			   	
			}
			break;

    	default:
		  	break;
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_set_text
  Description:  设置提示字
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_textid(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
    if (pData)
    {
        pData->hit = 0;
		pData->textid = lParam;
		
		if (pData->len == 0)
		{
			ui_reflash_inputctrl(hDlg, wParam, lParam);
		}
		else
		{
        	ui_inputctrl_clear_num(hDlg, wParam, lParam);
		}
    }
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_set_textlen
  Description:  输入数字的长度
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_textlen(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		pData->maxlen = (uint8)lParam;
    }
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_set_textlen
  Description:  输入数字的长度
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_text(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		strcpy(pData->text,(char *)lParam);
		ui_reflash_inputctrl(hDlg, wParam, lParam);
    }
	
	return TRUE;
}
/*************************************************
  Function:		ui_inputctrl_set_hit
  Description:  设置是否显示提示字
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_hit(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		pData->hit = (uint8)lParam;
		ui_reflash_inputctrl(hDlg, wParam, lParam);
    }
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_set_showtype
  Description:  在_NEW和_COMMON模式下设置明文或密文
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_showtype(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		pData->showtype = (uint8)lParam;
		ui_reflash_inputctrl(hDlg, wParam, lParam);
    }
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_set_active
  Description:  设置输入框活动窗口
    Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_set_active(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		pData->active = (uint8)lParam;
		ui_reflash_inputctrl(hDlg, wParam, lParam);
    }
	
	return TRUE;
}

/*************************************************
  Function:		ui_inputctrl_get_text
  Description:  获取字符串
    Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_get_text(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
	char *text = (char*)lParam;
	//int nlen = 0;
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		if (pData->len > 0)
		{
			memcpy(text,pData->text,pData->len);
		}
    }
	
	return TRUE;
}


/*************************************************
  Function:		ui_Input_destroy
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_inputctrl_destroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
	if (pData)
	{
    	free(pData);
		pData = NULL;		
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_input_get_text_value
  Description:  获取字符串
    Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_input_get_text_value(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
 	PINPUT_INFO pData = (PINPUT_INFO)GetWindowAdditionalData(hDlg);
	char temp[10];
	uint32 data = 0;
	memset(temp, 0 ,sizeof(temp));
 
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}

    if (pData)
    {
		if (pData->len > 0)
		{
			memcpy(temp,pData->text,pData->len);
		}
    }

	data = atoi(temp);
		
	return data;
}



/*************************************************
  Function:			MyControlProc
  Description: 		模板-消息处理函数
  Input: 		
	1.hwnd			句柄
	2.message		消息类型
	3.wParam		附加数据
	4.lParam		附加数据
  Output:			无
  Return:			无
  Others:
*************************************************/
static int MyControlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_CREATE:
			break;
						
		case MSG_PAINT:
			ui_inputctrl_paint(hWnd, wParam, lParam);
			break;				
			
		 case WM_Input_Get_Value:
		 	ui_inputctrl_get_value(hWnd, wParam, lParam);
			break;
			
		 case WM_Input_Clear:
			ui_inputctrl_clear_num(hWnd, wParam, lParam);
			break;
			
   		 case WM_Input_del_Num:
		 	ui_inputctrl_del_num(hWnd, wParam, lParam);
			break;
			
         case WM_Input_Send_Value:
		 	ui_inputctrl_send_value(hWnd, wParam, lParam);
		 	break;
			
		 case WM_Input_Set_Text:
		 	ui_inputctrl_set_text(hWnd, wParam, lParam);
		 	break;

		 case WM_Input_Set_TextID:
		 	ui_inputctrl_set_textid(hWnd, wParam, lParam);
		 	break;
			
		 case WM_Input_Set_Textlen:
		 	ui_inputctrl_set_textlen(hWnd, wParam, lParam);
			break;
			
		case WM_Input_Set_Hit:
			ui_inputctrl_set_hit(hWnd, wParam, lParam);
		 	break;

		case WM_Input_Set_ShowType:
			ui_inputctrl_set_showtype(hWnd, wParam, lParam);
		 	break;
			
		case WM_Input_Set_Active:
			ui_inputctrl_set_active(hWnd, wParam, lParam);
		 	break;
			
		case WM_Input_Get_Text:
			ui_inputctrl_get_text(hWnd, wParam, lParam);
			break;
			
		case WM_Input_Get_Text_Value:
			return ui_input_get_text_value(hWnd, wParam, lParam);

		case MSG_CLOSE:
			return 0;

		case MSG_DESTROY:
			return 0;
	}
	
	return DefaultControlProc (hWnd, message, wParam, lParam);
}

/*************************************************
  Function:		register_input_ctrl
  Description: 	注册控件
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
BOOL register_input_ctrl(void)
{
	WNDCLASS MyClass;

	MyClass.spClassName = AU_INPUT_CTRL;
	MyClass.dwStyle     = WS_NONE;
	MyClass.dwExStyle   = WS_EX_NONE;
	MyClass.hCursor     = GetSystemCursor (IDC_ARROW);
	MyClass.iBkColor    = PAGE_BK_COLOR;
	MyClass.WinProc     = MyControlProc;
	
	return RegisterWindowClass (&MyClass);
}

/*************************************************
  Function:		unregister_input_ctrl
  Description: 	取消注册
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
void unregister_input_ctrl(void)
{
	UnregisterWindowClass (AU_INPUT_CTRL);
}

