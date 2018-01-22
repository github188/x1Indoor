/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_list_ctrl.h
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-12
  Description:  列表控件头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UILISTCTRL_H__
#define __UILISTCTRL_H__

#if (_LCD_DPI_ == _LCD_800480_)
//#define LIST_ITEM_H				84					// 列表项高
//#define LIST_ITEM_W				640

#define TOP_TEXT_SIZE			200
//#define TOP_IMAMG_H				52

//#define LEFT_ICON_H				40					// 列表框里左边小图标高
//#define LEFT_ICON_W				40			
#define LEFT_ICON_DIS_X			40					// 列表框里左边小图标起始间距		

//#define RIGHT_ICON_H			16					// 列表框里右边小图标高
//#define RIGHT_ICON_W			16
#define RIGHT_ICON_DIS_X		15
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TOP_TEXT_SIZE			250	
#define LEFT_ICON_DIS_X			50					// 列表框里左边小图标起始间距		
#define RIGHT_ICON_DIS_X		19
#endif

#define KEYDOWN  				1
#define KEYUP  					0
#define MAX_LIST_ITEM			5
#define CHAR_SIZE				50
#define ITEM_SIZE				4
#define LIST_KEY_OK				1

typedef enum
{
	LIST_MODE_COMMON = 0x00,						// 普通显示 
	LIST_MODE_LIGHT									// 高亮显示  以上图标有变化
}LIST_MODE;										

typedef enum
{
	STYLE_CENTER = 0x00,							// 中间
    STYLE_LEFT, 									// 靠左
    STYLE_RIGHT		      							// 靠右
}SHOW_STYLE;										// 显示风格

typedef enum
{
	CHECK_IMAGE = 0x00,								// 查看图片 
	CHECK_ICON,										// 查看小图标
	CHECK_TEXTID,									// 查看文字ID
	CHECK_CHAR,										// 查看文字字符串
	CHECK_WCHAR,									// 查看WCHAR文字字符串
	CHECK_HEAD_TEXTID,								// 查看标题的文字ID
	CHECK_HEAD_CHAR,								// 查看标题的字符串
	CHECK_HEAD_WCHAR								// 查看标题的字符串	
}CHECK_TYPE;	

typedef enum
{
	SIDEBARICON_NONE,
	SIDEBARICON_DOWNON,
	SIDEBARICON_UPON,
	SIDEBARICON_BOTH
}SIDEBARICON_ON;

typedef struct 
{
	SHOW_STYLE ShowType;							// 显示风格
	uint16 Width;									// 宽度
	uint16 Change;									// 图标是否要变化
	uint32 TextID;									// 文字ID
	uint32 Image;									// 尾部小图标
	char Text[CHAR_SIZE];							// 文字字符
	uint16 WText[CHAR_SIZE];							// 文字字符
}LISTONE_INFO, * PLISTONE_INFO;

typedef struct 
{
	uint8 Enabled;									// 使能
	uint8 Num;										// 当行个数
	uint32 Icon;									// 尾部小图标
	//COLOR color;									// 该行文字颜色
	LISTONE_INFO Item[ITEM_SIZE];					// 内容项
}LISTITEM_INFO, * PLISTITEM_INFO;

//typedef void(*SidebarCallBack)(HWND hDlg, uint32 H, uint32 x, uint32 y, uint8 allpage, uint8 page, uint8 mode, uint8 keystate, uint8 select);
typedef void(*SidebarCallBack)(HWND hDlg, uint32 H, uint32 x, uint32 y, uint8 allpage, uint8 pageCount, uint8 Count, uint8 keystate, uint8 select, uint32 curSelect);
typedef uint8(*SidebarKeyCallBack)(uint32 H, uint8 mode, uint32 x, uint32 y);

typedef struct 
{
	HWND Parent;									// 父窗口ID
	LIST_MODE Mode;									// 列表显示
	uint8 Caption;									// 是否有标题 0没有 1有
	uint8 SideBarMode;								// 侧边栏的模式 长的为0 短的为1
	uint8 KeyMode;									// mode 0 要按确认
	uint8 Sidebar;									// 是否有上下边 0没有 1有
	uint32 SidebarW;								// 宽度
	int8 SidebarSel;  							    // 侧边栏选择哪个
	SIDEBARICON_ON SidebarState;					// 按下状态
	uint8 keystate;									// 按键状态
	uint16 Width[MAX_LIST_ITEM];					// 宽度
	uint16 Count;									// 列表总行数
	int16 Select;									// 选中项
	uint8 AllPage;									// 总页数
	uint8 Page;										// 当前页
	uint8 PageCount;								// 美页多少个内容项
	uint8 MoveSelect;								// 移动及选择
	uint32 ImageBk;								// 背景图片
	uint32 ImageTopBk;							// 头一张背景图片
	FontType fonttype;								// 字体 
	//COLOR HeadTextColor;							// 列表头文字的颜色(仅字符串有效)
	LISTONE_INFO HeadInfo;							// 列表头信息
	SidebarCallBack Drawcallback;					// 侧边栏实现回调
	SidebarKeyCallBack KeyCallBack;					// 侧边栏按键判断实现回调	
	UILIST 	*ListList;   							// 列表
}LISTCTRL_INFO, * PLISTCTRL_INFO;

typedef struct 
{
	CHECK_TYPE Check;								// 查看什么内容
	uint16 Select;									// 当前行
	uint8 index;									// 要修改的第几项
	uint32 Icon;									// 尾部小图标
	LISTONE_INFO Item;								// 内容项
}CHANGE_LIST, * PCHANGE_LIST;

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
void set_list_text(HWND hDlg, uint16 select, uint8 index, int32 TextID, char* str, uint16 *wstr, CHECK_TYPE check);

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
uint32 get_list_text(HWND hDlg, uint16 select, uint8 index, char* str, CHECK_TYPE check);

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
void set_list_Image(HWND hDlg, uint32 select, uint8 index, uint32 ImageID, CHECK_TYPE check);

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
uint32 get_list_Image(HWND hDlg, uint16 select, uint8 index, CHECK_TYPE check);
#endif

