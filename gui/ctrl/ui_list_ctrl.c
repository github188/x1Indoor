/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_list_ctrl.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-12
  Description:  列表控件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"

/*************************************************
  Function:		ui_reflash_ctrl
  Description: 	刷新控件
  Input:	
  	1.hDlg		窗体
  	2.WButObj 	控件结构体		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_ctrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, FALSE);
}

/*************************************************
  Function:		change_select
  Description: 	更改当前选择
  Input: 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void change_select(PLISTCTRL_INFO item, uint16 sel)
{
	uint16 index = sel+1;
	uint8 page;
	int oldsel;
	
	if (index > item->Count)
	{
		index = item->Count;
	}
	page = index / item->PageCount;
	oldsel = index % item->PageCount;

	if (0 == page)
	{
		item->Select = index-1;
		item->Page = page+1;
	}
	else
	{
		if (oldsel == 0)
		{
			item->Page = page;
			item->Select = item->PageCount-1;
		}
		else
		{
			item->Page = page+1;
			item->Select = index%item->PageCount-1;
		}
	}
}

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		move_select
  Description: 	更改当前选择
  Input: 		
	1.hDlg		句柄
	2.pData		PBUTTON_INFO数据
	3.index		按键索引 
	4.TextID	文字ID
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint8 move_select(PLISTCTRL_INFO item, uint16 index)
{
	uint32 oldsel = index+(item->Page-1)*item->PageCount;
	if (oldsel < item->Count)
	{
		PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(item->ListList, oldsel);
		if (pItem && pItem->Enabled)
		{
			change_select(item, oldsel);
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

/*************************************************
  Function:		ui_listctrl_paint
  Description: 	按钮画屏消息
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_paint(HDC hdc, RECT * rc, PLISTCTRL_INFO WObj)
{
	uint32 format;
	uint32 select = -1;
	uint8 index, i, j;
	PLISTITEM_INFO pItem;
	RECT TextRc;
	uint32 xpos, ypos, x_pos, y_pos;
	uint32 Image = 0;
	uint32 ImageIcon = 0;
	uint32 ImageH = 0;	
	PBITMAP bmp, bmpTop, bmpItem;

	bmpTop = get_bmp(WObj->HeadInfo.Image);
	bmpItem = get_bmp(WObj->ImageBk);
	
	if (storage_get_language() != ENGLISH)
	{
		SelectFont(hdc, GetFont(WObj->fonttype));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(WObj->fonttype));
	}
	SetTextColor(hdc, COLOR_BLACK);

	xpos = rc->left;
	ypos = rc->top;
	if (WObj->Caption)
	{
		DrawBmpID(hdc, xpos, ypos, bmpItem->bmWidth, 0, WObj->HeadInfo.Image);
		if (WObj->HeadInfo.TextID > 0)	
		{	
			TextRc.left = xpos+LEFT_ICON_DIS_X;
			TextRc.top = ypos+(bmpTop->bmHeight-WObj->fonttype)/2;
			TextRc.right = TextRc.left+TOP_TEXT_SIZE;
			TextRc.bottom = TextRc.top+WObj->fonttype;
			DrawText(hdc, get_str(WObj->HeadInfo.TextID),-1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		else if (strlen(WObj->HeadInfo.WText) > 0)
		{
			DrawText(hdc, WObj->HeadInfo.WText, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		else if(strlen(WObj->HeadInfo.Text) > 0)
		{
			DrawText(hdc,WObj->HeadInfo.Text, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		//ypos += bmpItem->bmHeight;
		ypos += bmpTop->bmHeight;
	}
	
	select = WObj->Select + (WObj->Page-1)*WObj->PageCount;
	for (i = 0; i < WObj->PageCount; i++)
	{
		pItem = NULL;
		xpos = rc->left;
		if (0 == i)
		{
			Image = WObj->ImageTopBk;
			//ImageH = bmpTop->bmHeight;
			
		}
		else
		{
			Image = WObj->ImageBk;
			//ImageH = bmpTop->bmHeight;	
		}

		ImageH = bmpItem->bmHeight;
		index = i + (WObj->Page-1)*WObj->PageCount;
		if (index > WObj->PageCount-1)
		{
			index = select - (WObj->Page-1)*WObj->PageCount+1+i;
		}
		
		if (index < WObj->Count)
		{
			pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, index);
		}

		if (select > WObj->PageCount-1)
		{
			if (LIST_MODE_LIGHT == WObj->Mode)
			{
				if (index == select && pItem && pItem->Enabled)
				{
					Image += 1; 
				}
			}
			else
			{
				if (index == select)
				{
					Image += 1; 
				}	
			}
		}
		else
		{
			if (LIST_MODE_LIGHT == WObj->Mode)
			{
				if (i == WObj->Select && pItem && pItem->Enabled)
				{
					Image += 1; 
				}
			}
			else
			{
				if (i == WObj->Select)
				{
					Image += 1; 
				}	
			}
		}

		if (WObj->Sidebar)
		{
			DrawBmpID_EX(hdc, xpos, ypos, bmpItem->bmWidth-SIDEBAR_W, 0, Image);
		}
		else
		{
			DrawBmpID_EX(hdc, xpos, ypos, bmpItem->bmWidth, 0, Image);
		}

		xpos += LEFT_ICON_DIS_X;
		if (index >= WObj->Count)
		{
			ypos += bmpItem->bmHeight;
		}
		else
		{
			if (pItem)
			{
				for (j = 0; j < pItem->Num; j++)
				{
					if (pItem->Item[j].Image > 0)
					{	
						if (LIST_MODE_LIGHT == WObj->Mode)
						{
							if (i == WObj->Select && pItem->Enabled)
							{
								if (pItem->Item[j].Change)
								{
									ImageIcon = pItem->Item[j].Image+1;
								}
								else
								{
									ImageIcon = pItem->Item[j].Image;
								}
							}
							else
							{
								ImageIcon = pItem->Item[j].Image;
							}
						}
						else
						{
							if (i == WObj->Select)
							{
								if (pItem->Item[j].Change)
								{
									ImageIcon = pItem->Item[j].Image+1;
								}
								else
								{
									ImageIcon = pItem->Item[j].Image;
								}
							}
							else
							{
								ImageIcon = pItem->Item[j].Image;
							}
						}
						#if 0
						switch(pItem->Item[j].ShowType)
						{
							case STYLE_LEFT:
								x_pos = xpos;
								y_pos = ypos+(bmpItem->bmHeight-LEFT_ICON_H)/2;
								break;	
								
							case STYLE_CENTER:
								x_pos = xpos+(pItem->Item[j].Width-LEFT_ICON_W);
								y_pos = ypos+(bmpItem->bmHeight-LEFT_ICON_H)/2;
								break;
						   
						    case STYLE_RIGHT:
								x_pos = xpos+(pItem->Item[j].Width-LEFT_ICON_W);
								y_pos = ypos+(bmpItem->bmHeight-LEFT_ICON_H)/2;
								break;	
						}
						#endif
						bmp = get_bmp(ImageIcon);
						switch(pItem->Item[j].ShowType)
						{
							case STYLE_LEFT:
								x_pos = xpos;
								y_pos = ypos+(bmpItem->bmHeight-bmp->bmHeight)/2;
								break;	
								
							case STYLE_CENTER:
								x_pos = xpos+(pItem->Item[j].Width-bmp->bmWidth);
								y_pos = ypos+(bmpItem->bmHeight-bmp->bmHeight)/2;
								break;
						   
						    case STYLE_RIGHT:
								x_pos = xpos+(pItem->Item[j].Width-bmp->bmWidth);
								y_pos = ypos+(bmpItem->bmHeight-bmp->bmHeight)/2;
								break;	
						}

						DrawBmpID(hdc, x_pos, y_pos, 0, 0, ImageIcon);
					}
					else
					{
						TextRc.left = xpos;
						TextRc.top = ypos+(ImageH-WObj->fonttype)/2;
						TextRc.right = TextRc.left+pItem->Item[j].Width;
						TextRc.bottom = TextRc.top+WObj->fonttype;
						switch (pItem->Item[j].ShowType)
						{
							case STYLE_CENTER:
								format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
								break;
						   
						    case STYLE_RIGHT:
								format = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
								break;	

							case STYLE_LEFT:
							default:
								format = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
								break;
						}
						
						if (pItem->Item[j].TextID > 0)
						{
							DrawText(hdc, get_str(pItem->Item[j].TextID), -1, &TextRc, format);
						}
						else if ((strlen(pItem->Item[j].Text)) > 0)
						{
							DrawText(hdc, pItem->Item[j].Text, -1, &TextRc, format);
						}
						else if ((strlen(pItem->Item[j].WText)) > 0)
						{
							DrawText(hdc, pItem->Item[j].WText, -1, &TextRc, format);
						}
					}
					xpos += pItem->Item[j].Width;
				}
			}

			if (pItem->Icon > 0)
			{
				bmp = get_bmp(pItem->Icon);

				if(WObj->Sidebar)
				{
					x_pos = (rc->right-RIGHT_ICON_DIS_X - bmp->bmWidth)-WObj->SidebarW;
				}
				else
				{
					x_pos = (rc->right-RIGHT_ICON_DIS_X - bmp->bmWidth);
				}
				y_pos = ypos+(bmpItem->bmHeight - bmp->bmHeight)/2;
				DrawBmpID(hdc, x_pos, y_pos, 0, 0, pItem->Icon);
			}
			ypos += bmpItem->bmHeight;
		}
	}

	#if 0
	if (WObj->Sidebar)
	{
		DrawBmpID_EX(hdc, 0, ypos, bmpItem->bmWidth-SIDEBAR_W, 0, WObj->ImageBk);
	}
	else
	{
		DrawBmpID_EX(hdc, 0, ypos, bmpItem->bmWidth, 0, WObj->ImageBk);
	}
	#else
	DrawBmpID(hdc, 0, ypos, bmpItem->bmWidth, 0, WObj->ImageBk);
	#endif

	if (WObj->Sidebar && WObj->Drawcallback)
	{
		xpos = rc->left+rc->right-WObj->SidebarW;
		//ypos = rc->top+bmpItem->bmHeight;
		ypos = rc->top + bmpTop->bmHeight;
		WObj->Drawcallback(hdc, (rc->bottom-bmpItem->bmHeight), xpos, ypos, 
			WObj->AllPage, WObj->PageCount, WObj->Count, WObj->SidebarState, WObj->SidebarSel,select);
	}
	
    return TRUE;
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
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == WObj)
	{
		log_printf("WButObj is NULL!!!");
		return;
	}
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetBrushColor(hdc, PAGE_BK_COLOR);
	FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	ui_listctrl_paint(hdc, &rc, WObj);
	EndPaint(hDlg, hdc);
}

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_pos
  Description:  获取当前按到哪个按键
  Input:
  	1.item		结构
  	2.x			X坐标
  	3.y			Y坐标	
  Output:		无
  Return:		无
  Others:
*************************************************/
static int16 get_pos(WINDOW* win, PLISTCTRL_INFO item ,int x, int y)
{
	uint32 sy = y-item->HeadInfo.Image.H-win->y;
	int16 index = -1;
	uint32 one_h = item->ImageTopBk.H;

	if (y > item->HeadInfo.Image.H)
	{
		index = sy/one_h;
	}
	return index;
}

/*************************************************
  Function:		ui_listctrl_touch
  Description: 	触摸屏事件
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_touch(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	PUHPOINT touchPoint = (PUHPOINT)lParam;
	int16 index = -1;
	int16 allindex = 0;
	PLISTITEM_INFO pItem;
	uint8 ret = FALSE;

	switch(wParam)
	{
		case TM_STATE_DOWN:
			
			if (WObj->Sidebar && touchPoint->x > (WObj->w-WObj->SidebarW))
			{
				if (WObj->KeyCallBack)
				{
					WObj->SidebarSel = WObj->KeyCallBack((WObj->h-WObj->HeadInfo.Image.H),WObj->SideBarMode,touchPoint->x,(touchPoint->y-WObj->y));
					if (-1 != WObj->SidebarSel)
					{
						sys_key_beep();
						WObj->SidebarState = KEYDOWN;
						ui_reflash_ctrl(hDlg,wParam,lParam);
						return TRUE;
					}
				}
			}
			index = get_pos(hDlg, WObj, touchPoint->x, touchPoint->y);
			if (-1 != index)
			{
				ret = move_select(WObj, index);
				if (ret)
				{
					sys_key_beep();
					WObj->keystate = KEYDOWN;
				}
			}
			break;

		case TM_STATE_UP:
			if (KEYDOWN == WObj->SidebarState)
			{
				WObj->SidebarState = KEYUP;
				if (WObj->SidebarSel)
				{
					ui_go_nextpage(hDlg, 0, NULL);
				}
				else
				{
					ui_go_lastpage(hDlg, 0, NULL);
				}
			}
			else
			{
				if (KEYDOWN == WObj->keystate)
				{
					allindex = WObj->Select+(WObj->Page-1)*WObj->PageCount;
					pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, allindex);				
					if (pItem && pItem->Enabled)
					{
						//WindowSendMessage(win->Parent, WM_COMMAND, ((UINT32)(win->id)) << 16 | allindex, &(WObj->keystate));
					}
					WObj->keystate = KEYUP;
				}
			}
			break;
	}
	ui_reflash_ctrl(hDlg,wParam,lParam);
    return TRUE;
}
#endif

/*************************************************
  Function:		ui_go_lastpage
  Description:  翻上一页
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_go_lastpage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	int oldsel; 
	
	if (WObj && WObj->Page > 1)
	{
		WObj->Page--;	
		if (-1 != WObj->Select)
		{
			oldsel = WObj->Select+(WObj->Page-1)*WObj->PageCount;
			change_select(WObj, oldsel);
		}
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_go_nextpage
  Description:  翻下一页
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_go_nextpage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	int oldsel; 
	
	if (WObj && WObj->Page < WObj->AllPage)
	{
		WObj->Page++;	
		if (-1 != WObj->Select)
		{
			oldsel = WObj->Select+(WObj->Page-1)*WObj->PageCount;
			change_select(WObj, oldsel);
		}
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_Okkey
  Description:  
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_Okkey(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		uint32 allindex = WObj->Select+(WObj->Page-1)*WObj->PageCount;
		PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, allindex);				
		if (pItem && pItem->Enabled)
		{
			//WindowSendMessage(win->Parent, WM_COMMAND, ((UINT32)(win->id)) << 16 | allindex, &(WObj->keystate));
			SendMessage(WObj->Parent, MSG_COMMAND, IDC_LIST_CTRL, (LPARAM)allindex);
		}
		WObj->keystate = KEYUP;
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_go_lastselect
  Description:  翻上一个
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_go_lastselect(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	int oldsel = 0; 
	
	if (WObj && WObj->Count > 1)
	{
		oldsel = WObj->Select+(WObj->Page-1)*WObj->PageCount; 
		if (0 == oldsel)
		{
			oldsel = (WObj->Count-1);
		}
		else
		{
			oldsel--;
		}
		change_select(WObj, oldsel);		
		ui_reflash_ctrl(hDlg, wParam, lParam);
		if (WObj->KeyMode)
		{
			ui_listctrl_Okkey(hDlg, wParam, lParam);
		}
	}

	return TRUE;
}

/*************************************************
  Function:		ui_go_nextselect
  Description:  翻下一个
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_go_nextselect(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	int oldsel; 
	
	if (WObj && WObj->Count > 1)
	{
		oldsel = WObj->Select+(WObj->Page-1)*WObj->PageCount; 
		if (WObj->Count == (oldsel+1))
		{
			oldsel = 0;
		}
		else
		{
			oldsel++;
		}
		change_select(WObj, oldsel);
		ui_reflash_ctrl(hDlg, wParam, lParam);
		if (WObj->KeyMode)
		{
			ui_listctrl_Okkey(hDlg, wParam, lParam);
		}
	}

	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_KeyMode
  Description:  设置按键模式
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_KeyMode(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	WObj->KeyMode = wParam;
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_allpage
  Description:  获取当前多少页
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_allpage(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj)
	{
		return WObj->AllPage;
	}

	return 0;
}

/*************************************************
  Function:		ui_set_page
  Description:  获取当前页
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_page(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	uint16 page = wParam;
	
	if (WObj && WObj->Page != page)
	{
		WObj->Page = page;
		if (WObj->Page < 1)
		{
			WObj->Page = 1;
		}
		else
		{
			if (WObj->Page > WObj->AllPage)
			{
				WObj->Page = WObj->AllPage;
			}
		}
		WObj->Select = 0;
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_page
  Description:  获取当前页
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_page(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj)
	{
		return WObj->Page;
	}

	return 0;
}

/*************************************************
  Function:		ui_set_select
  Description:  设置当前选中项
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_select(HWND hDlg,  WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	
	change_select(WObj, wParam);
	ui_reflash_ctrl(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_select
  Description:  获取当前选中项
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static int32 ui_get_select(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj)
	{
		return (WObj->Select+(WObj->Page-1)*WObj->PageCount);
	}
	else
	{
		return -1;
	}
}

/*************************************************
  Function:		ui_set_enable
  Description:  设置使能
  Input:		
  	1.hDlf		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_enable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	uint8 enable = (*(uint8*)lParam);
	
	if (WObj && wParam < WObj->Count)
	{
		PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, wParam);
		if (pItem && pItem->Enabled != enable)
		{
			pItem->Enabled = enable;
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_enable
  Description:  获取使能
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_enable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	if (WObj && wParam < WObj->Count)
	{
		PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, wParam);
		if (pItem)
		{
			return pItem->Enabled;
		}
	}
	
	return FALSE;
}

/*************************************************
  Function:		ui_set_headtext
  Description:  设置头文字
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_headtext(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj && WObj->HeadInfo.TextID != wParam)
	{
		WObj->HeadInfo.TextID = wParam;
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_headtext
  Description:  获取头文字
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_headtext(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj)
	{
		return WObj->HeadInfo.TextID;
	}
	
	return FALSE;
}

/*************************************************
  Function:		ui_set_text
  Description:  设置侧边栏
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_sidebar(HWND hDlg,  WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj && WObj->Sidebar != wParam)
	{
		WObj->Sidebar = wParam;
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_set_text
  Description:  设置文字
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_text(HWND hDlg,  WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	PCHANGE_LIST pData = (PCHANGE_LIST)lParam;

	if (WObj && pData)
	{
		if (CHECK_HEAD_TEXTID == pData->Check || CHECK_HEAD_CHAR == pData->Check || CHECK_HEAD_WCHAR == pData->Check)
		{
			switch (pData->Check)
			{
				case CHECK_HEAD_TEXTID:
					if (WObj->HeadInfo.TextID != pData->Item.TextID)
					{
						WObj->HeadInfo.TextID = pData->Item.TextID;
					}
					break;
					
				case CHECK_HEAD_CHAR:
					memset(WObj->HeadInfo.Text, 0, CHAR_SIZE);
					memcpy(WObj->HeadInfo.Text, pData->Item.Text, CHAR_SIZE);
					break;

				case CHECK_HEAD_WCHAR:
					memset(WObj->HeadInfo.WText, 0, CHAR_SIZE);
					memcpy(WObj->HeadInfo.WText, pData->Item.WText, CHAR_SIZE);
					break;	

				default:
					break;
					
			}
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
		else
		{
			if (pData->Select < WObj->Count && pData->index < MAX_LIST_ITEM)
			{
				PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, pData->Select);
				if (pItem )
				{
					switch (pData->Check)
					{
						case CHECK_TEXTID:
							if (pItem->Item[pData->index].TextID != pData->Item.TextID)
							{
								pItem->Item[pData->index].TextID = pData->Item.TextID;
							}
							break;

						case CHECK_CHAR:
							memset(pItem->Item[pData->index].Text, 0, CHAR_SIZE);
							memcpy(pItem->Item[pData->index].Text, pData->Item.Text, CHAR_SIZE);
							break;	

						case CHECK_WCHAR:
							memset(pItem->Item[pData->index].WText, 0, CHAR_SIZE);
							memcpy(pItem->Item[pData->index].WText, pData->Item.WText, CHAR_SIZE);
							break;		

						default:
							break;
					}
					ui_reflash_ctrl(hDlg, wParam, lParam);
				}
			}
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_text
  Description:  获取文字
  Input:		
  	1.hdlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_text(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	PCHANGE_LIST pData = (PCHANGE_LIST)lParam;

	if (WObj && pData)
	{
		if (pData->Select < WObj->Count && pData->index < MAX_LIST_ITEM)
		{
			if (CHECK_HEAD_TEXTID == pData->Check || CHECK_HEAD_CHAR == pData->Check)
			{
					pData->Item.TextID = WObj->HeadInfo.TextID;
					memcpy(pData->Item.Text, WObj->HeadInfo.Text, CHAR_SIZE);
					return pData->Item.TextID;
			}
			else
			{
				PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, pData->Select);
				if (pItem)
				{
					pData->Item.TextID = pItem->Item[pData->index].TextID;
					memcpy(pData->Item.Text, pItem->Item[pData->index].Text, CHAR_SIZE);
					return pData->Item.TextID;
				}
			}	
		}
	}
	
	return 0;
}

/*************************************************
  Function:		ui_set_Image
  Description:  设置图片ID
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_Image(HWND hDlg,  WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	PCHANGE_LIST pData = (PCHANGE_LIST)lParam;

	if (WObj && pData)
	{
		if (pData->Select < WObj->Count && pData->index < MAX_LIST_ITEM)
		{
			PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, pData->Select);
			if (CHECK_IMAGE == pData->Check)
			{
				if (pItem && pItem->Item[pData->index].Image != pData->Item.Image)
				{
					pItem->Item[pData->index].Image = pData->Item.Image;
					ui_reflash_ctrl(hDlg, wParam, lParam);
				}
			}
			else
			{
				if (pItem && pItem->Icon != pData->Icon)
				{
					pItem->Icon = pData->Icon;
					ui_reflash_ctrl(hDlg, wParam, lParam);
				}
			}
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_get_Image
  Description:  获取图片ID
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_get_Image(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	PCHANGE_LIST pData = (PCHANGE_LIST)lParam;

	if (WObj && pData)
	{
		if (pData->Select < WObj->Count && pData->index < MAX_LIST_ITEM)
		{
			PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, pData->Select);
			if (pItem)
			{
				if (CHECK_IMAGE == pData->Check)
				{
					return pItem->Item[pData->index].Image;
				}
				else
				{
					return pItem->Icon;
				}
			}
		}
	}
	
	return 0;
}

/*************************************************
  Function:		ui_set_color
  Description:  设置该行颜色
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_color(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	#if 0
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	COLOR color = (*(COLOR*)lParam);
	
	if (WObj && wParam < WObj->Count)
	{
		PLISTITEM_INFO pItem = (PLISTITEM_INFO)ListGetItem(WObj->ListList, wParam);
		if (pItem)
		{
			pItem->color = color;
			ui_reflash_ctrl(hDlg,wParam,lParam);
		}
	}
	#endif
	return TRUE;
}

/*************************************************
  Function:		ui_set_width
  Description:  
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_width(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	int i,num = wParam;
	uint16* Width = (uint16*)lParam;
	
	if (WObj && Width)
	{
		for (i = 0; i < num; i++)
		{
			WObj->Width[i] = *(Width+i);
		}
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_additem
  Description: 	控件添加一项内容
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_additem(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint32 i,j;
    uint32 number = wParam;
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
    PLISTITEM_INFO list = NULL;
	PLISTITEM_INFO gData = (PLISTITEM_INFO)lParam;
	
	if (0 == lParam || NULL == gData)
	{
		return FALSE;
	}
	for (i = 0; i < number; i++)
	{
		list = malloc(sizeof(LISTITEM_INFO));
		if (!list)
		{
			return FALSE;
		}
		
		list->Enabled = TRUE;
		list->Num = gData[i].Num;
		list->Icon = gData[i].Icon;
		memcpy(list->Item, gData[i].Item, (ITEM_SIZE*sizeof(LISTONE_INFO)));
		for (j = 0; j < list->Num; j++)
		{
			#if 0
			ui_get_bmp_info(list->Item[j].Image.ID, &(list->Item[j].Image.W), &(list->Item[j].Image.H));
			ui_get_bmp_info(list->Icon.ID, &(list->Icon.W), &(list->Icon.H));
			#endif
			if (list->Item[j].Width == 0)
			{
				list->Item[j].Width = WObj->Width[j];
			}
		}
		ListAddItem(WObj->ListList, list);
		WObj->Count++;
	}

	WObj->Page = 1;
	if (0 == (WObj->Count%WObj->PageCount))
	{
		WObj->AllPage = (WObj->Count/WObj->PageCount);
	}
	else
	{
		WObj->AllPage = (WObj->Count/WObj->PageCount)+1;
	}
	if (WObj->Count > 0)
	{
		WObj->Select = 0;
	}
	else
	{
		WObj->Select = -1;
	}
    ui_reflash_ctrl(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_delitem
  Description: 	控件删除一项内容
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_delitem(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	uint16 index;
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	if (WObj->Count > 0 && WObj->Select >= 0)
	{
		index = WObj->Select+(WObj->Page-1)*WObj->PageCount;
		if (index < WObj->Count)
		{
			ListDeleteItem(WObj->ListList, index);
			WObj->Count--;
			change_select(WObj, index);
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_clearitem
  Description: 	控件清空内容
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_clearitem(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);

	ListClear(WObj->ListList);
	WObj->Count = 0;
	WObj->Select = -1;
	WObj->Page = 1;
	WObj->AllPage = 1;
	ui_reflash_ctrl(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_creat
  Description: 	控件创建
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam	PLISTCTRL_INFO 结构
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_creat(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	
	if (NULL == WObj)
	{
		return FALSE;
	}

	WObj->ListList = ListCreate(LS_NONE, NULL); 
	if (NULL == WObj->ListList)
	{
		return FALSE;
	}
	WObj->Count = 0;
	WObj->Select = -1;
	WObj->Page = 1;
	WObj->AllPage = 1;
	WObj->SidebarSel = -1;
	WObj->KeyMode = 0;
	WObj->SidebarW = SIDEBAR_W;		
	
	if (WObj->fonttype == 0)
	{
		WObj->fonttype = Font16X16;
	}
	
	ui_reflash_ctrl(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_listctrl_destroy
  Description: 	控件销毁
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_listctrl_destroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PLISTCTRL_INFO item = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	
	if (item)
	{
		ListDestroy(item->ListList);
		// del luofl 2015-12-15 列表控制数据为静态变量,不能释放
    	//FreeClass(item);
		//item = NULL;
	}
    return TRUE;
}

/*************************************************
  Function:		MyControlProc
  Description: 	模板-消息处理函数
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
			ui_listctrl_creat(hDlg, wParam, lParam);
			break;
	
		case MSG_INITDIALOG:
			break;	
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;

		case MSG_KILLFOCUS:
			if (GetCapture() == hDlg) 
			{
                ReleaseCapture();
            }
            InvalidateRect(hDlg, NULL, TRUE);
            break;
			
		case MSG_SETFOCUS:
			InvalidateRect(hDlg, NULL, TRUE);
			break;

		case WM_List_Set_Enable:
			ui_set_enable(hDlg, wParam, lParam);
			break;

		case WM_List_Get_Enable:
			return ui_get_enable(hDlg, wParam, lParam);
					
		case BM_GETCHECK:	
			break;

		case BM_SETCHECK:							
			break;	
			
		case WM_List_Set_Text:
			ui_set_text(hDlg, wParam, lParam);
			break;
			
		case WM_List_Get_Text:	
			return ui_get_text(hDlg, wParam, lParam);

		case WM_List_Set_Image:	
			ui_set_Image(hDlg, wParam, lParam);
			break;

		case WM_List_Get_Image:
			return ui_get_Image(hDlg, wParam, lParam);

		case WM_List_SetT_BKColor:	
			break;	

		case WM_List_OkKey:
			ui_listctrl_Okkey(hDlg, wParam, lParam);
			break;	

		case WM_List_Go_LastSel:
			ui_go_lastselect(hDlg, wParam, lParam);
			break;
			
		case WM_List_Go_NextSel:
			ui_go_nextselect(hDlg, wParam, lParam);
			break;
			
		case WM_List_Add_Item:
			ui_listctrl_additem(hDlg, wParam, lParam);
			break;
			
		case WM_List_Clear_Item:
			ui_listctrl_clearitem(hDlg, wParam, lParam);
			break;
			
		case WM_List_Del_Item:
			ui_listctrl_delitem(hDlg, wParam, lParam);
			break;	

		case WM_List_Go_LastPage:
			ui_go_lastpage(hDlg, wParam, lParam);
			break;
			
		case WM_List_Go_NextPage:
			ui_go_nextpage(hDlg, wParam, lParam);
			break;

		case WM_List_Get_AllPage:
			return ui_get_allpage(hDlg, wParam, lParam);

		case WM_List_Get_Page:
			return ui_get_page(hDlg, wParam, lParam);

		case WM_List_Set_Page:
			ui_set_page(hDlg, wParam, lParam);
			break;

		case WM_List_Get_Sel:
			return ui_get_select(hDlg, wParam, lParam);

		case WM_List_Set_Sel:
			ui_set_select(hDlg, wParam, lParam);
			break;

		case WM_List_Set_ItemColor:
			ui_set_color(hDlg, wParam, lParam);
			break;
			
		case WM_List_Set_HeadText:
			ui_set_headtext(hDlg, wParam, lParam);
			break;
			
		case WM_List_Get_HeadText:
			ui_get_headtext(hDlg, wParam, lParam);
			break;
			
		case WM_List_Set_Side:
			ui_set_sidebar(hDlg, wParam, lParam);
			break;
			
		case WM_List_Set_ItemWidth:
			ui_set_width(hDlg, wParam, lParam);
			break;
				
		case WM_List_SetKeyMode:
			ui_listctrl_KeyMode(hDlg, wParam, lParam);
			break;
		
		case MSG_CLOSE:
			return 0;
		
		case MSG_DESTROY:
			ui_listctrl_destroy(hDlg, wParam, lParam);
			break;	
	}
	
	return DefaultControlProc(hDlg, message, wParam, lParam);
}

/*************************************************
  Function:		register_list_ctrl
  Description: 	注册右边控件
  Input: 		
  Output:		无
  Return:		无
  Others:
*************************************************/
BOOL register_list_ctrl(void)
{
	WNDCLASS MyClass;
	
	MyClass.spClassName = AU_LIST_CTRL;
	MyClass.dwStyle = WS_NONE;
	MyClass.dwExStyle = WS_EX_NONE;
	MyClass.hCursor = GetSystemCursor(IDC_ARROW);
	MyClass.iBkColor = COLOR_lightwhite;
	MyClass.WinProc = MyControlProc;
	
	return RegisterWindowClass(&MyClass);
}

/*************************************************
  Function:			unregister_list_ctrl
  Description: 		删除右边控件
  Input: 		
  Output:			无
  Return:			无
  Others:
*************************************************/
void unregister_list_ctrl(void)
{
	UnregisterWindowClass(AU_LIST_CTRL);
}

/*************************************************
  Function:		set_list_text
  Description: 	设置某项的内容
  Input: 		
	1.hDlg		控件句柄
	2.select	第几行
	3.index		第几项
	4.str		更改的字符串
	5.wstr		更改的字符串
	6.check		要设置的类型
  Output:		无
  Return:		无
  Others:
*************************************************/
void set_list_text(HWND hDlg, uint16 select, uint8 index, int32 TextID, char* str, uint16 *wstr, CHECK_TYPE check)
{
	CHANGE_LIST item;
	uint16 nlen;
	
	memset(&item,0,sizeof(CHANGE_LIST));
	item.Select = select;
	item.index = index;
	item.Check = check;
	if (str)
	{
		nlen = strlen(str);
		if (nlen > CHAR_SIZE)
		{
			nlen = CHAR_SIZE;
		}
		memcpy(item.Item.Text,str,nlen);
	}

	if (wstr)
	{
		memcpy(item.Item.WText,wstr,CHAR_SIZE);
	}
	item.Item.TextID = TextID;
	SendMessage(hDlg, WM_List_Set_Text, 0, (LPARAM)&item);
}

/*************************************************
  Function:		get_list_text
  Description: 	获得某项的内容
  Input: 		
	1.hDlg		控件句柄
	2.select	第几行
	3.index		第几项
	4.str		更改的字符串
	5.check		要查询的类型
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 get_list_text(HWND hDlg, uint16 select, uint8 index, char* str, CHECK_TYPE check)
{
	CHANGE_LIST item;
	uint32 TextID = 0;
	
	memset(&item,0,sizeof(CHANGE_LIST));
	item.Select = select;
	item.index = index;
	item.Check = check;
	TextID = SendMessage(hDlg, WM_List_Get_Text, 0, (LPARAM)&item);
	if (CHECK_CHAR == check)
	{
		if (str)
		{
			memcpy(str,item.Item.Text,sizeof(item.Item.Text));
		}
		TextID = strlen(item.Item.Text);
	}
	return TextID;
}

/*************************************************
  Function:		set_list_Image
  Description: 	设置某项的图片
  Input: 		
	1.hDlg		控件句柄
	2.select	第几行
	3.index		第几项
	4.ImageID	图片ID
	5.check		要设置的类型
  Output:		无
  Return:		无
  Others:
*************************************************/
void set_list_Image(HWND hDlg, uint32 select, uint8 index, uint32 ImageID, CHECK_TYPE check)
{
	CHANGE_LIST item;
	
	memset(&item,0,sizeof(CHANGE_LIST));
	item.Select = select;
	item.index = index;
	item.Check = check;
	if (CHECK_ICON == check)
	{
		item.Icon = ImageID;
	}
	else
	{
		item.Item.Image = ImageID;
	}
	SendMessage(hDlg, WM_List_Set_Image, 0, (LPARAM)&item);
}

/*************************************************
  Function:		get_list_Image
  Description: 	获得某项的图片 
  Input: 		
	1.hDlg		控件句柄
	2.select	第几行
	3.index		第几项
	4.str		更改的字符串
	5.check		要查询的类型
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 get_list_Image(HWND hDlg, uint16 select, uint8 index, CHECK_TYPE check)
{
	CHANGE_LIST item;
	
	memset(&item,0,sizeof(CHANGE_LIST));
	item.Select = select;
	item.index = index;
	item.Check = check;
	return SendMessage(hDlg, WM_List_Get_Image, 0, (DWORD)&item);
}

