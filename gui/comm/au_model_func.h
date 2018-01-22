/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	au_model_func.h
  Author:   	唐晓磊
  Version:   	2.0
  Date: 		08.3.26
  Description:  公共函数头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __AU_MODEL_FUNC_H__
#define __AU_MODEL_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_include.h"

#if (_LCD_DPI_ == _LCD_800480_)
#define SIDEBAR_W				80
#define SIDEBAR_H				428
#define SIDEBAR_DOWN_ICON_Y		389
#define SIDEBAR_ICON_W			30
#define SIDEBAR_ICON_H			70
#define SIDEBAR_YDIS			21
#define SIDEBAR_SCRLEN			384
#elif (_LCD_DPI_ == _LCD_1024600_)
#define SIDEBAR_W				102
#define SIDEBAR_H				535
#define SIDEBAR_DOWN_ICON_Y		496
#define SIDEBAR_ICON_W			36
#define SIDEBAR_ICON_H			87
#define SIDEBAR_YDIS			21
#define SIDEBAR_SCRLEN			480
#endif

#if (_UI_STYLE_ == _E81S_UI_STYLE_)
#undef SIDEBAR_ICON_W
#undef SIDEBAR_YDIS
#undef SIDEBAR_SCRLEN			
#undef SIDEBAR_DOWN_ICON_Y		
#define SIDEBAR_ICON_W			42
#define SIDEBAR_YDIS			60
#define SIDEBAR_SCRLEN			280
#define SIDEBAR_DOWN_ICON_Y		360
#endif

#define SCREENPORTECT_OFF		0x00				// 屏幕保护关
#define SCREENPORTECT_ON		0x01				// 屏幕保护开
#define SCREENPORTECT_NOUSE		0x02				// 屏幕保护不启用

typedef enum
{
	STYLE_MAIN_NEWMESSAGE, 
	STYLE_MAIN_LYLY,
	STYLE_MAIN_NOFAZE,
	STYLE_MAIN_COUNT
} MAIN_ICON_STYLE;									// 更改小图标类型

// 异步操作仲裁: IO触发、感应按键、网络触发、界面入口
// 主要处理页面跳转仲裁, 不处理媒体之间的仲裁
typedef enum
{
	SYS_OPER_ALARMING,								// 报警触发-跳转页面					
	SYS_OPER_ALARM_SOS,								// 报警感应按键
	SYS_OPER_CALLOUT,								// 呼叫中心、住户、分机、回拨记录
	SYS_OPER_CALLIN,								// 被叫
	SYS_OPER_MONITOR								// 监视
}SYS_ASYN_OPER_TYPE;

#define CREATE_FORM(frm)	static CTRLDATA Ctrls[] ={};\
static DLGTEMPLATE DlgInit =\
{ \
	DW_STYLE, \
	DW_STYLE_EX,\
	FORM_X, FORM_Y, FORM_W, FORM_H,	\
	"",\
	0,\
	0,\
	sizeof(Ctrls)/sizeof(CTRLDATA),\
	Ctrls, \
	0\
};\
static int WindowProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam);\
GUI_FORM frm = {0, &FrmMain, &DlgInit, WindowProc, -1, 0, NULL, NULL, 0};\
static PGUI_FORM self = &frm

#define close_page() form_close(self)

#define ip_addr1(ipaddr) ((uint8)(ipaddr >> 24) & 0xff)
#define ip_addr2(ipaddr) ((uint8)(ipaddr >> 16) & 0xff)
#define ip_addr3(ipaddr) ((uint8)(ipaddr >> 8) & 0xff)
#define ip_addr4(ipaddr) ((uint8)(ipaddr) & 0xff)

/********************************************************/
/*			            字体相关函数                    */	
/********************************************************/
/*************************************************
  Function:    		InitLogFont
  Description:		初始化字体
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void InitLogFont(void);

/*************************************************
  Function:    		FreeLogFont
  Description:		释放字体
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void FreeLogFont(void);

/*************************************************
  Function:    		GetFont
  Description:		获得字体
  Input: 			
  	1.size			(FONT_12--->FONT_24)
  Output:			无
  Return:			(正确)逻辑字体，(错误)NULL
  Others:
*************************************************/
PLOGFONT GetFont(int8 size);

/*************************************************
  Function:    		GetFont
  Description:		获得粗体字体
  Input: 			
  	1.size			(FONT_12--->FONT_24)
  Output:			无
  Return:			(正确)逻辑字体，(错误)NULL
  Others:
*************************************************/
PLOGFONT GetBoldFont(int8 size);

/*************************************************
  Function:    		GetFont_ABC
  Description:		获得粗体ABC及数字
  Input: 			
  	1.size			(FONT_24,FONT_26)
  Output:			无
  Return:			(正确)逻辑字体，(错误)NULL
  Others:
*************************************************/
PLOGFONT GetFont_ABC(int8 size);

/********************************************************/
/*			            皮肤相关函数                    */	
/********************************************************/
/*************************************************
  Function:    		set_skin
  Description:		设置皮肤方案
  Input: 			
  	1.mode			方案代号(SKIN_1--->SKIN_4)
  Output:			无
  Return:			设置成功为true，错误为false
  Others:
*************************************************/
uint8 set_skin(int8 mode);

/*************************************************
  Function:    		get_skin_count
  Description:		获得皮肤方案数量
  Input: 			无
  Output:			无
  Return:			皮肤方案的数目，方案图片不存在的返回1
  Others:
*************************************************/
uint8 get_skin_count(void);

/*************************************************
  Function:    		get_skin
  Description:		获得皮肤方案
  Input: 			无
  Output:			无
  Return:			方案代号(SKIN_1--->SKIN_4)
  Others:
*************************************************/
uint8 get_skin(void);

/*************************************************
  Function:    		GetSysColor
  Description:		获得系统颜色
  Input: 			
  	1.id			ID
  Output:			无
  Return:			颜色
  Others:
*************************************************/
uint32 GetSysColor(int id);

/********************************************************/
/*			           窗体相关函数                     */	
/********************************************************/

/*************************************************
  Function:    		get_cur_form
  Description:		得到当前显示的窗体
  Input: 			无
  Output:			无
  Return:			当前显示的窗体
  Others:
*************************************************/
PGUI_FORM get_cur_form(void);

/*************************************************
  Function:    		get_cur_hDlg
  Description:		获取当前窗体句柄
  Input: 			无
  Output:			无
  Return:			当前窗体的句柄
  Others:
*************************************************/
HWND get_cur_hDlg(void);

/*************************************************
  Function:    		form_show
  Description:		显示非模态窗体
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			成功(true)与否(false)
  Others:
*************************************************/
uint8 form_show(PGUI_FORM frm);

/*************************************************
  Function:    		form_hide
  Description:		隐藏窗体(模态时,关闭窗体)
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			无
  Others:
*************************************************/
void form_hide(PGUI_FORM frm);

/*************************************************
  Function:    		form_close
  Description:		关闭窗体
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			无
  Others:
*************************************************/
void form_close(PGUI_FORM frm);

/*************************************************
  Function:    		init_frm_language
  Description:		初始化窗体语言
  Input: 			
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
void init_frm_language(void);

/*************************************************
  Function:    		DefaultMsgProc
  Description:		默认消息处理
  Input: 			
  	1.frm			窗体名称
  	2.hDlg			句柄
  	3.message		消息
  	4.wParam		参数1
  	5.lParam		参数2
  Output:			无
  Return:			处理结果
  Others:
*************************************************/
int DefaultMsgProc(PGUI_FORM frm, HWND hDlg, int message, WPARAM wParam, LPARAM lParam);

/*************************************************
  Function:    		form_showmodal
  Description:		显示窗体(模态,不立即返回)
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			成功(true)与否(false)
  Others:
*************************************************/
int form_showmodal(PGUI_FORM frm);

/*************************************************
  Function:    		form_create
  Description:		建立窗体(非模态)
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			窗体句柄
  Others:
*************************************************/
HWND form_create(PGUI_FORM frm);

/*************************************************
  Function:    		exit_page
  Description:		退出页面
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			无
  Others:
*************************************************/
void exit_page(void);

/*************************************************
  Function:    		close_curren_page
  Description:		关闭当前页面
  Input: 			
  	frm				当前窗体地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void close_curren_page(PGUI_FORM frm);

/*************************************************
  Function:    		parent_page
  Description:		返回前一个窗体的句柄
  Input: 			
  	1.frm			操作名称
  Output:			无
  Return:			无
  Others:
*************************************************/
HWND parent_page(void);

/*************************************************
  Function:    		exit_the_page
  Description:		退出当前页面
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			无
  Others:
*************************************************/
void exit_the_page(PGUI_FORM frm);

/*************************************************
  Function:    		close_msg_page
  Description:		关闭消息框页面
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void close_msg_page(PGUI_FORM frm);

/*************************************************
  Function:    		back_main_page
  Description:		退回主页面
  Input: 			
  	1.frm			窗体名称
  Output:			无
  Return:			无
  Others:
*************************************************/
void back_main_page(void);

//*************************************************************//
//***************以下是消息提示框**********************//
//*************************************************************//
/*************************************************
  Function:		set_windowsize
  Description: 	设置窗口大小
  Input:
  	1.frm		窗体名称
  	2.x			起始坐标
  	3.y			
  	4.w			窗口宽
  	5.h			窗口高
  Output:		无
  Return:		无	
  Others:		
*************************************************/
void set_windowsize(PGUI_FORM frm, int x, int y, int w, int h);

/*************************************************
  Function:		show_msg
  Description:  提示框
  Input:		
  	1.hDlg		窗口
  	2.ID		控件ID
  	3.style		风格
  	4.TextID	文字ID
  Output:		无
  Return:		无
  Others:
*************************************************/
void show_msg(HWND hDlg , uint32 ID, AU_MSG_TYPE style, uint32 TextID);

/*************************************************
  Function:     show_msg_video
  Description:  提示框
  Input:        
    1.win       窗口
    2.ID        ID
    3.style     风格
    4.TextID    文字ID
  Output:       无
  Return:       无
  Others:
*************************************************/
void show_msg_video(HWND hDlg , uint32 ID, AU_MSG_TYPE style, uint32 TextID);

/*************************************************
  Function:		show_msg_pass
  Description:  密码提示框
  Input:		
  	1.win		窗口
  	2.ID		ID
  	3.type		密码类型
  	4.xiechi	挟持判断
  Output:		无
  Return:		无
  Others:
*************************************************/
void show_msg_pass(HWND hDlg, uint32 id, PASS_TYPE type, uint8 xiechi);

#if 0
/*************************************************
  Function:     show_msg_jd_addr
  Description:  家电地址提示框
  Input:        
    1.win       窗口
    2.ID        ID
    3.startaddr 起始地址
    4.endaddr   结束地址
  Output:       无
  Return:       无
  Others:
*************************************************/
void show_msg_jd_addr(HWND hDlg , uint32 ID, uint16 startaddr, uint16 endaddr);

/*************************************************
  Function:     show_msg_jd_control
  Description:  家电控制
  Input:        
    1.win       窗口
    2.ID        ID
    3.startaddr 起始地址
    4.endaddr   结束地址
  Output:       无
  Return:       无
  Others:
*************************************************/
void show_msg_jd_control(HWND hDlg , uint32 ID, AU_JD_DEV_TYPE devtype, uint8  state,  uint8 Value, uint16 *text, uint8 factory);

/*************************************************
  Function:     show_msg_aurine_jd_control
  Description:  家电控制
  Input:        
    1.win       窗口
    2.ID        ID
    3.startaddr 起始地址
    4.endaddr   结束地址
  Output:       无
  Return:       无
  Others:
*************************************************/
void show_msg_tabus_jd_control(HWND hDlg , uint32 ID, AU_JD_DEV_TYPE devtype, uint8  state,  uint8 Value, uint16 *text, uint8 style,uint8 flag);

/*************************************************
  Function:     show_msg_ipmodule_code
  Description:  验证码提示框
  Input:        
    1.win       窗口
    2.ID        ID
  Output:       无
  Return:       无
  Others:
*************************************************/
void show_msg_ipmodule_code(HWND hDlg , uint32 ID);
#endif

/*************************************************
  Function:		show_msg_hit
  Description:  报警、新信息、新来电提示
  Input:		
  	1.hDlg		窗口
  	2.ID		控件ID
  	3.style		风格
  	4.TextID	文字ID	
  Output:		无
  Return:		无
  Others:
*************************************************/
void show_msg_hit(HWND hDlg , uint32 ID, AU_MSG_TYPE style, uint32 TextID);


/*************************************************
  Function:		show_msg_pass
  Description:  密码提示框
  Input:		
  	1.win		窗口
  	2.ID		ID
  	3.type		密码类型
  	4.xiechi	挟持判断
  Output:		无
  Return:		无
  Others:
*************************************************/
void show_msg_pass(HWND hDlg, uint32 id, PASS_TYPE type, uint8 xiechi);

/*************************************************
  Function:    		toXiechiPwd
  Description:		转换成挟持密码
  Input: 			
  	1.oldPwd		要转换的密码
  	2.bAdd			挟持密码是否+1：true:+1；false: -1;
  Output:			
  	1.newPwd		转换后的密码
  Return:			
*************************************************/
void toXiechiPwd(char * oldPwd, uint8 bAdd, char * newPwd);

/*************************************************
  Function:		change_ip_to_char
  Description:  切换IP地址4个字节
  Input:		
  	1.IP		IP地址
  	2.ipaddr	地址
  Output:		无
  Return:		无
  Others:
*************************************************/
void change_ip_to_char(uint32 IP, uint8* ipaddr);

/*************************************************
  Function:		change_char_to_ip
  Description:  切换4个字节为IP地址
  Input:		
  	1.ipaddr	ip地址
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 change_char_to_ip(uint8* ipaddr);

/*************************************************
  Function:		change_ip_to_str
  Description:  切换IP地址字符串
  Input:		
  	1.IP		IP地址
  	2.ipaddr	地址
  Output:		无
  Return:		无
  Others:
*************************************************/
void change_ip_to_str(uint32 IP,char *ip);

/*************************************************
  Function:			show_sys_event_hint
  Description: 		系统事件提示，如新信息，来电提醒等
  Input:		
  	1.EventType		事件类型
  Output:			无
  Return:			无
  Others:
*************************************************/
void show_sys_event_hint(uint16 EventType);

/*************************************************
  Function:		get_colorkey
  Description: 	
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
int get_colorkey(void);

/*************************************************
  Function:		draw_video
  Description: 	画视频输出
  Input: 		
	1.hdc
	2.x			X地址
	3.y			Y地址
	4.w			宽度
	5.h			高度
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_video(HDC hdc, int x, int y, int w, int h);

/*************************************************
  Function:		get_now_ymd
  Description: 	获取当前日期的年月日
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_now_ymd(int *y,int *m,int *d);

/*************************************************
  Function:		get_houseno_desc
  Description: 	获得房号描述
  Input: 		无
  Output:		无
  Return:		无
  Others:		add by luofl 2008-1-10
*************************************************/
char * get_houseno_desc( char * numtext, char * temp);

/*************************************************
  Function:		get_isFullScreen
  Description: 	
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 get_isFullScreen(void);

/*************************************************
  Function:		set_fullscreen_NormalRc
  Description: 	
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
//void set_fullscreen_NormalRc(RECT_S * pRc);

/*************************************************
  Function:		get_isFullScreen
  Description: 	设置全屏是否有视频
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 set_fullscreen_video(uint8 isVideo);

#if 0
/*************************************************
  Function:		full_screen
  Description: 	打开全屏
  Input: 		
	1.			无 
  Output:		无
  Return:		无
  Others:
*************************************************/
int full_screen(HWND hOwner);
#endif

/*************************************************
  Function:		close_full_screen
  Description: 	关闭全屏
  Input: 		
	1.			无
  Output:		无
  Return:		无
  Others:
*************************************************/
int close_full_screen(int SetVideo);
/*************************************************
  Function:    		show_msg_byID
  Description:		消息框(显示字符串ID)
  Input: 			
  　1.ParentFrm		父窗口
  	２.TitleID		标题ID
  	３.TextId		文本内容ID
  	４.msgtype		信息框类型
  	5.pData         数据结构体
  Output:			0:用户取消操作返回 1:用户确认操作返回
  Return:			无
  Others: 
*************************************************/
uint32 show_msg_byID(PGUI_FORM ParentFrm,  int TitleID,int TextId,AU_MSG_TYPE msgtype, PJIADIAN_ITEM pData);

/*************************************************
  Function:    		show_msg_bytext
  Description:		消息框(显示字符串)
  Input: 			
  　1.ParentFrm		父窗口
  	２.TitleID		标题ID
  	３.text			文本内容
  	４.msgtype		信息框类型
  Output:			0:用户取消操作返回 1:用户确认操作返回
  Return:			无
  Others: 
*************************************************/
uint32 show_msg_bytext(PGUI_FORM ParentFrm,  int TitleID,char *text,AU_MSG_TYPE msgtype);

/********************************************************/
/*			           其他相关函数                     */	
/********************************************************/
/*************************************************
  Function:    		change_mainIcon_state
  Description:		改变主界面小图标闪烁状态
  Input: 			
  	1.check_state	当前要检查的图标状态
  	2.pflash		当前状态 0 暗 1 亮 呼叫转移，免打扰 不闪烁
  Output:			
  Return:			无
  Others:
*************************************************/
void change_mainIcon_state(MAIN_ICON_STYLE check_state, uint8 pflash);

/*************************************************
  Function:    		get_mainIcon_state
  Description:		获得小图标状态
  Input: 			
  	1.public_state	设置公共环境
  	2.state:   		状态
  Output:			无
*************************************************/
//void get_mainIcon_state(PUBLIC_STATE public_state);

/*************************************************
  Function:    		draw_backbmp
  Description:		在某个区域中画图片
  Input: 			
  	1.hdc			hdc
  	2.rect			要画的具体位置大小
  	3.bmp			图片
  	4.bmpMode		画模式：0:拉伸；1：居中；2：平铺
  	5.isFreeBmp 	true:释放图片资源
  Output:			无 
  Others:
*************************************************/
void draw_backbmp(HDC hdc,RECT rect ,PBITMAP bmp ,int bmpMode,uint8 isFreeBmp);

/*************************************************
  Function:    utf8_2_gb2312
  Description: 将utf8字符串转化为gb2312字符串
  Input:
  1.待转化的字符串指针
  Return:
  转化成功与否
*************************************************/
int utf8_2_gb2312(char *p);

/*************************************************
  Function:		ConvertFileName
  Description: 	把全路径名转换成文件名称 
  Input: 		
	1.fullpathname: 要转换的文件全路径名称	
	2.finame:  	转换后的文件名称 
  Output:		无
  Return:		
  Others:
*************************************************/
void ConvertFileName(char * fullpathname,char * filename);

/*************************************************
  Function:		draw_text_by_myfont
  Description: 	
  Input: 		
	1.			hdc
	2.			x
	3.			y
	4.			TextID
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_text_by_myfont(HDC hdc, int x, int y, uint32 TextID);

/*************************************************
  Function:		DrawBmpID
  Description: 	根据图片ID在指定位置画图片,宽高根据
  				实际图片大小决定
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void DrawBmpID(HDC hdc, int x, int y, int w, int h, int id);

/*************************************************
  Function:		DrawBmpID_EX
  Description: 	根据图片ID在指定位置画图片
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void DrawBmpID_EX(HDC hdc, int x, int y, int w, int h, int id);

/*************************************************
  Function:		draw_Sidebar
  Description:  画侧边栏
  Input:		
  	1.win		
  	2.x			
  	3.y			
  	4.allpage
  	5.page
  	6.mode
  	7.keystate
  	8.select
  Output:		无
  Return:		无
  Others:
*************************************************/
//void draw_Sidebar(HWND hDlg, uint32 H,uint32 x, uint32 y, uint8 allpage, uint8 page, uint8 mode, uint8 keystate, uint8 select);
void draw_Sidebar(HWND hDlg, uint32 H, uint32 x, uint32 y, uint8 allpage, uint8 pageCount, uint8 Count, uint8 keystate, uint8 select, uint32 curSelect);

/*************************************************
  Function:		get_Sidebar_key
  Description:  侧边栏触摸屏
  Input:		
  	1.win		
  	2.x			已经减过起始坐标的X坐标	
  	3.y			已经减过起始坐标的Y坐标	
  Output:		无
  Return:		无
  Others:
*************************************************/
int8 get_Sidebar_key(uint32 H, uint8 mode, uint32 x, uint32 y);

/*************************************************
  Function:			ui_show_win_arbitration
  Description: 		显示页面时仲裁
  Input: 		
  	1.hDlg			当前页面
  	2.OperType		操作类型
  Output:			无
  Return:			TRUE / FALSE
  Others:			
*************************************************/
int32 ui_show_win_arbitration(HWND hDlg, SYS_ASYN_OPER_TYPE OperType);

/*************************************************
  Function:		ScreenOnTimer
  Description: 	屏幕和LCD定时
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
void ScreenOnTimer(void);

/*************************************************
  Function:		SetScreenTimer
  Description: 	恢复屏幕和LCD定时时长
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void RestoreScreenTimer(void);

/*************************************************
  Function:		SetScreenTimer
  Description: 	恢复屏幕和LCD定时时长
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
uint8 SetScreenTimer(void);

/*************************************************
  Function:		InitScreenTimer
  Description: 	初始化屏幕和LCD定时
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void InitScreenTimer(void);

/*************************************************
  Function:			get_houseno_desc
  Description: 		获得房号描述
  Input: 			
  	1.numtext		设备编号
  	2.temp			设备描述
  Output:			无
  Return:			无
  Others:		
*************************************************/
char * get_houseno_desc(char * numtext, char * temp);

/*************************************************
  Function:			get_dev_description
  Description: 		获得设备描述
  Input: 			
  	1.DevType		设备类型
  	2.DevStr		设备编号
 	3.StrOutLen		输出缓冲大小
  Output:			
  	1.DevStrOut		设备描述
  Return:			无
  Others:		
*************************************************/
uint32 get_dev_description(DEVICE_TYPE_E DevType, char * DevStr, char * DevStrOut, uint32 StrOutLen);

/*************************************************
  Function:		OnPaintBack
  Description: 	画背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
void OnPaintBack(HWND hDlg);

/*************************************************
  Function:		OnPaintWhite
  Description: 	画白色背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
void OnPaintWhite(HWND hDlg);

#ifdef _TIMER_REBOOT_NO_LCD_
/*************************************************
  Function:     timer_reboot_LCD_func
  Description:  定时重启屏背光控制切换
  Input:        
    1.flag       1:单片机控制屏背光 
                 0: N32926控制屏背光
  Output:       无
  Return:       无
  Others:   
  夜间重启时不能亮屏、不能响开机声音
  ，以免用户察觉异常。
*************************************************/
void timer_reboot_control(uint8 flag);
#endif

#ifdef __cplusplus 
}
#endif

#endif /* __AU_MODEL_FUNC_H__ */
