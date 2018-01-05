/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	au_other_winres.h
  Author:		罗发禄
  Version:		V2.0
  Date: 		2015-02-03
  Description:	资源头文件(颜色,结构体定义,自定义消息)
  
  History:		  
	1. Date:
	   Author:
	   Modification:
	2. ...
*********************************************************/
#ifndef __AU_OTHER_WINRES_H__
#define __AU_OTHER_WINRES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_include.h"
#include "logic_include.h"

/************************常量定义************************/
#define FONT_12				12						// 12号字体
#define FONT_14				14						// 14号字体
#define FONT_16				16						// 16号字体
#define FONT_20				20						// 20号字体
#define FONT_24				24						// 24号字体
#define FONT_26				26						// 26号字体
#define FONT_28				28						// 28号字体

#define SKIN_1				1						// 皮肤方案1
#define SKIN_2				2						// 皮肤方案2
#define SKIN_3				3						// 皮肤方案3
#define SKIN_4				4						// 皮肤方案4    

#ifdef _TFT_7_
#define SCREEN_WIDTH	  		800						// 窗体的宽度
#define SCREEN_HEIGHT     		480						// 窗体的高度
#define DW_STYLE				(WS_VISIBLE | WS_NONE)	// 窗体的风格
//#define DW_STYLE_EX			WS_EX_NONE				// 窗体的扩展风格
#define DW_STYLE_EX				WS_EX_AUTOSECONDARYDC	// 窗体的扩展风格
#define FORM_X					0						// 起点坐标x
#define FORM_Y					0						// 起点坐标y 
#define FORM_W					(SCREEN_WIDTH-FORM_X)	// 窗体的宽度
#define FORM_H					(SCREEN_HEIGHT-FORM_Y)	// 窗体的高度
#define FORM_RIGHT_W			160						// 右边窗体宽度
#define FORM_RIGHT_H			480						// 右边窗体高度

//#define DEFAULT_STYLE	  		(WS_VISIBLE | WS_NONE)	// 默认窗体的风格
//#define DEFAULT_STYLE_EX  	(WS_EX_NONE)			// 默认窗体的扩展风格
#define DEFAULT_BN_STYLE		(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_NOTIFY)

#define TEXT_X              	15

// 输入框宏定义
#define KEYDOWN  				1
#define KEYUP  					0
#define MAX_LEN					8     
#define PASSWORD_LEN 			6

#define INPUT_CTRL_W			356
#define INPUT_CTRL_H			74
#define	INPUT_CTRL_X			((FORM_W-RIGHT_CTRL_W-INPUT_CTRL_W)/2)
#define INPUT_CTRL_Y			90

// 键盘控件宏定义
#define KEYBORD_NUM_MAX         11
#define KEYDOWN  			    1
#define KEYUP  					0

#define KEYBOARD_CTRL_W         640
#define KEYBOARD_CTRL_H         90  
#define KEYBOARD_CTRL_X         0
#define KEYBOARD_CTRL_Y         (FORM_H-KEYBOARD_CTRL_H)

// 右边控件宏定义
#define RIGHT_CTRL_X			640
#define RIGHT_CTRL_Y			0
#define RIGHT_CTRL_W			160
#define RIGHT_CTRL_H			480

// 列表控件宏定义
#define LIST_CTRL_X				0
#define LIST_CTRL_Y				0
#define LIST_CTRL_W				640
#define LIST_CTRL_H				480

// 主叫、被叫、监视提示字位置
#define TEXT_XPOS           	40
#define TEXT_YPOS          		20
#define TEXT_DISX               12
#define TEXT_DISY				30	
#define TEXT_LEN          		200

// 音量设置
#define MUTE_ICON_XPOS			181
#define MUTE_ICON_YPOS			240
#define VOLUME_ICON_XPOS    	249
#define VOLUME_ICON_YPOS    	240
#define VOLUME_TOTAL_H			48					// 音量图标总高度
#define VOLUME_TOTAL_W			(210+48+20)
#define VOLUME_ALL_ICON_Y		((VIDEO_WIN_H-VOLUME_TOTAL_H)/2+150)
#else
#define SCREEN_WIDTH	  		480						// 窗体的宽度
#define SCREEN_HEIGHT     		272						// 窗体的高度
#define DW_STYLE				(WS_VISIBLE | WS_NONE)	// 窗体的风格
//#define DW_STYLE_EX			WS_EX_NONE				// 窗体的扩展风格
#define DW_STYLE_EX				WS_EX_AUTOSECONDARYDC	// 窗体的扩展风格
#define FORM_X					0						// 起点坐标x
#define FORM_Y					0						// 起点坐标y 
#define FORM_W					(SCREEN_WIDTH-FORM_X)	// 窗体的宽度
#define FORM_H					(SCREEN_HEIGHT-FORM_Y)	// 窗体的高度
#define FORM_RIGHT_W			100						// 右边窗体宽度
#define FORM_RIGHT_H			272						// 右边窗体高度

//#define DEFAULT_STYLE	  		(WS_VISIBLE | WS_NONE)	// 默认窗体的风格
//#define DEFAULT_STYLE_EX  	(WS_EX_NONE)			// 默认窗体的扩展风格
#define DEFAULT_BN_STYLE		(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_NOTIFY)

#define TEXT_X              	15

// 输入框宏定义
#define KEYDOWN  				1
#define KEYUP  					0
#define MAX_LEN					8     
#define PASSWORD_LEN 			6
#define INPUT_CTRL_X			100
#define INPUT_CTRL_Y			41
#define INPUT_CTRL_W			210
#define INPUT_CTRL_H			44

// 键盘控件宏定义
#define KEYBORD_NUM_MAX         11
#define KEYDOWN  			    1
#define KEYUP  					0
#define KEYBOARD_CTRL_X         0
#define KEYBOARD_CTRL_Y         219
#define KEYBOARD_CTRL_W         380
#define KEYBOARD_CTRL_H         53  

// 右边控件宏定义
#define RIGHT_CTRL_X			380
#define RIGHT_CTRL_Y			0
#define RIGHT_CTRL_W			100
#define RIGHT_CTRL_H			272

// 列表控件宏定义
#define LIST_CTRL_X				0
#define LIST_CTRL_Y				0
#define LIST_CTRL_W				380
#define LIST_CTRL_H				272

// 主叫、被叫、监视提示字位置
#define TEXT_XPOS           	12
#define TEXT_YPOS          		5
#define TEXT_DISX               12
#define TEXT_DISY				21	
#define TEXT_LEN          		200

// 音量设置
#define MUTE_ICON_XPOS			103
#define MUTE_ICON_YPOS			225
#define VOLUME_ICON_XPOS    	141
#define VOLUME_ICON_YPOS    	225
#define VOLUME_TOTAL_H			29					// 音量图标总高度
#define VOLUME_TOTAL_W			170
#define VOLUME_ALL_ICON_Y		MUTE_ICON_YPOS
#endif

// 网络门前机定时器时长
#define NETDOOR_TIME			10

/************************常量定义************************/

/************************颜色定义************************/
#define MAKE_RGB16(r, g, b)	(((WORD)((BYTE)(r) & 0xF8) << 8) | ((WORD)((BYTE)(g) & 0xFC) << 3) \
				| ((WORD)((BYTE)(b) & 0xF8) >> 3))

#define MAKE_RGB32(a, r, g, b)	(((DWORD)((BYTE)(a) & 0xFF) << 24) | ((DWORD)((BYTE)(r) & 0xFF) << 16) \
				| ((DWORD)((BYTE)(g) & 0xFF) << 8) | ((DWORD)((BYTE)(b) & 0xFF)))

#ifdef _USE_RGB32_
#define COLOR_BLACK		 	(MAKE_RGB32(255, 0, 0, 0))				// 黑色	
#define COLOR_WHITE		 	(MAKE_RGB32(255, 255, 255, 255)) 		// 白色
#define COLOR_RED			(MAKE_RGB32(255, 255, 0, 0))	    	// 红色
#define COLOR_GREEN			(MAKE_RGB32(255, 0, 255, 0))			// 绿色
#define COLOR_BLUE			(MAKE_RGB32(255, 0, 0, 255))			// 蓝色
#define COLOR_YELLOW		(MAKE_RGB32(255, 255, 255, 0))			// 黄色	
#else
#define COLOR_BLACK		 	(MAKE_RGB16(0, 0, 0))					// 黑色	
#define COLOR_WHITE		 	(MAKE_RGB16(255, 255, 255)) 			// 白色
#define COLOR_RED			(MAKE_RGB16(255, 0, 0))	    			// 红色
#define COLOR_GREEN			(MAKE_RGB16(0, 255, 0))					// 绿色
#define COLOR_BLUE			(MAKE_RGB16(0, 0, 255))					// 蓝色
#define COLOR_YELLOW		(MAKE_RGB16(255, 255, 0))				// 黄色		
#endif
#define COLOR_GREY			PIXEL_lightgray							// 灰色
#define PAGE_BK_COLOR	  	COLOR_BLACK								// 默认背景色

// 方案1
#define COLOR_BUTTON_TEXT	GetSysColor(0)			// 按钮上的文字
#define COLOR_COMMON_TEXT	GetSysColor(1)			// 普通文字
#define COLOR_DOWN_TEXT		GetSysColor(2)			// 点击情况下普通文字
#define COLOR_ERR			GetSysColor(3)			// 错误提示信息的颜色
#define COLOR_DEVNO_TEXT	GetSysColor(4)			// 桌面设备编号的文字颜色
#define COLOR_BLUE_TEXT		GetSysColor(5)			// 房号输入的号码文字
#define SYS_COLOR_LIGHT		GetSysColor(6)			// 默认亮色
#define SYS_COLOR_BK		GetSysColor(7)			// 默认背景色
#define SYS_COLOR_DARK		GetSysColor(8)			// 默认暗色
#define SYS_DOWN_LIGHT		GetSysColor(9)			// 点击亮色
#define SYS_DOWN_BK			GetSysColor(10)			// 点击背景色
#define SYS_DOWN_DARK		GetSysColor(11)			// 普通文字点击暗色
#define SYS_INFO_LIGHT		GetSysColor(12)			// 提示框亮色
#define SYS_INFO_BK			GetSysColor(13)			// 提示框背景色
#define SYS_INFO_DARK		GetSysColor(14)			// 提示框暗色
/************************颜色定义************************/

/**********************控件名称定 义**********************/
#define AU_RIGHT_BUTTON		"AU_R_BUTTON"
#define AU_LIST_CTRL		"AU_LIST_CTRL"
#define AU_CHOSE_BUTTON     "AU_C_BUTTON"
#define AU_AREA_BUTTON		"AU_AREA_BUTTON"
#define AU_KEY_BUTTON       "AU_KEY_BUTTON"
#define AU_SPIN_EDIT		"AU_SPIN_EDIT"
#define NUM_KEYBORD			"NUM_KEYBORD"
#define AU_MSG_BUTTON       "AU_MSG_BUTTON"
#define AU_SET_VOLUME       "AU_SET_VOLUME"
#define AU_FLAT_EDIT        "AU_FLAT_EDIT"
#define AU_INPUT_CTRL       "AU_INPUT_CTRL"
#define AU_EIGHTAREA_CTRL	"AU_EIGHT_CTRL"
#define AU_SPIN_CTRL		"AU_SPIN_CTRL"
#define AU_PROC_CTRL		"AU_PROC_CTRL"
/**********************控件名称定义**********************/

/*******************用户定义公共控件ID*******************/
typedef enum
{
	IDC_RIGHT_BUTTON = 901,	
	IDC_KEYBOARD_CTRL,
	IDC_LIST_CTRL,		
	IDC_INPUT_CTRL,          
	IDC_INPUT1_CTRL,
	IDC_MSG_CTRL,
	IDC_EIGHTAREA_CTRL,
	IDC_SPIN_CTRL,
	IDC_FORM,
	IDC_Msg_Admin,
	IDC_Msg_User,
	IDC_Msg_Notice,
	IDC_Set_Back,
	IDC_SEARCH_LIST, 			// 搜索列表时用的特殊IDC
	IDC_PROC_CTRL,
}IDC_TYPE;

/*********************用户定义消息***********************/
typedef enum
{
    // 右边控件消息
	WM_Right_Set_TextID = MSG_USER+1,
	WM_Right_Get_TextID,
	WM_Right_Set_Image,
	WM_Right_Get_Image,
	WM_Right_Get_ImageOrTextID,
	WM_Right_Set_enable,
	WM_Right_Get_enable,
	WM_Right_Set_VideoMode,
	WM_Right_Get_VideoMode,
	WM_Right_SetTextLen,
	WM_Right_Set_Select,
	WM_Right_Get_Select,
	WM_Right_Set_TextLen,
	WM_Right_Add_Win,
	WM_Right_Query_Key,
	WM_Right_Set_ALLImage,
	WM_Right_Clear_Win,
	WM_Right_Clear_BackGround,
	WM_Right_reflash,

	// 列表控件消息
	WM_List_Set_Enable,
	WM_List_Get_Enable,
	WM_List_Set_Text,
	WM_List_Get_Text,
	WM_List_Set_Image,
	WM_List_SetT_BKColor,
	WM_List_Go_LastSel,
	WM_List_Go_NextSel,
	WM_List_Add_Item,
	WM_List_Clear_Item,
	WM_List_Del_Item,
	WM_List_Go_LastPage,
	WM_List_Go_NextPage,
	WM_List_Get_AllPage,
	WM_List_Get_Page,
	WM_List_Set_Page,
	WM_List_Get_Sel,
	WM_List_Set_ItemColor,
	WM_List_Get_Image,
	WM_List_OkKey,
	WM_List_Set_Sel,
	WM_List_Set_HeadText,
	WM_List_Get_HeadText,
	WM_List_Set_Side,
	WM_List_Set_ItemWidth,
	WM_List_SetKeyMode,
	
	// 键盘控件消息
	WM_Key_Set_Select,
	WM_Key_Get_Select,
	WM_Key_Set_enable,
	WM_Key_Get_enable,
	WM_Key_Left,
	WM_Key_Right,
	WM_Key_Ok,
	WM_Key_Exit,
	WM_Key_Set_Flag,

	// 输入框控件
	WM_Input_Set_enable,
	WM_Input_Get_enable,
	WM_Input_Set_Text,
	WM_Input_Get_Text,
	WM_Input_Set_Textlen,
	WM_Input_Set_Hit,
	WM_Input_Set_TextID,
	WM_Input_Set_Flag,
	WM_Input_Clear,
	WM_Input_del_Num,
	WM_Input_add_Text,
	WM_Input_Set_Active,
	WM_Input_Get_Active,
	WM_Input_Get_Value,
	WM_Input_Send_Value,	
	WM_Input_Set_ShowType,
	WM_Input_Get_Text_Value,

	// 八防区控件
	WM_Eight_Set_Text,
	WM_Eight_Get_Text,
	WM_Eight_Set_TextW,
	WM_Eight_Get_TextW,
	WM_Eight_Set_Image,
	WM_Eight_Set_MidImage,
	WM_Eight_Get_Image,
	WM_Eight_Set_enable,
	WM_Eight_Get_enable,
	WM_Eight_Set_Param,
	WM_Eight_End_Timer,
	WM_Eight_Set_MaxNum,
	WM_Eight_Set_BKImage,

	// IP控件消息
	WM_Spin_Set_Value,
	WM_Spin_Get_IPValue,
	WM_Spin_Get_Value,
	WM_Spin_Set_MaxValue,
	WM_Spin_Get_MaxValue,
	WM_Spin_Add_Item,
	WM_Spin_Clear_Item,
	WM_Spin_Set_enable,
	WM_Spin_Get_enable,
	WM_Spin_Set_SelAll,
	WM_Spin_Del,
	WM_Spin_Right,
	WM_Spin_Left,
	WM_Spin_Active,

	// 其它消息
	WM_Change_RightKey,								// query时消息框重刷右边控件
	WM_MSG_VIEW,									// 信息查看
	WM_RELOAD,										// 重载
	WM_MSG_Close,									// 关闭消息框
	WM_CENTER_HANDUP,								// 中心挂断消息
	WM_NETDOOR,                      			   	// 网络门前机用的消息
	WM_FORM_SHOW,									// 显示窗口
	
	// 进度消息
	WM_Procress_Set_Proc,			
	WM_Procress_Set_Allproc,
}WM_TYPE;
/*********************用户定义消息***********************/

/**********************结构体定义************************/
typedef void (*PROC)(void);							// 窗体相关结构体

typedef enum
{
	MSG_INFORMATION = 0x01,							// 消息
	MSG_WARNING,									// 警告
	MSG_ERROR,										// 错误
	MSG_RIGHT,										// 正确
	MSG_QUERY,										// 确认
	MSG_PASS,										// 密码框
	MSG_ALARM,										// 报警
	MSG_MESSAGE,									// 新信息
	MSG_CALL_MESSAGE,								// 来电信息
	MSG_TS,											// 校正
	MSG_JDADDR,										// 家电
	MSG_JD_TJ,										// 家电调解
	MSG_AURINE_JD_TJ,                               // 冠林家电调节
	MSG_AURINE_JD_MODE,                             // 冠林家电模式
	MSG_HIT,                                        // 提示
	MSG_IPCODE,										// IP模块地址
	MSG_JD_HIT,
	MSG_NEW_PASS,
} AU_MSG_TYPE;	

typedef struct
{
	int year;										// 年
	int mon;										// 月
	int mday;										// 日
	int wday;										// 星期
	int hour;										// 小时
	int min;										// 分	
	int sec;										// 秒
}GUI_TIME,*PGUI_TIME;

typedef struct _GUI_FORM
{
	HWND hWnd;
	struct _GUI_FORM * Parent; 						//窗体父亲
	PDLGTEMPLATE pDlgTemplate; 						//窗体模板
	WNDPROC	WinProc;								//消息处理函数
	int language;									//窗体当前语言
	int skin;										//皮肤方案
	PROC ChangeLanguage;							//改变语言函数
	PROC ChangeSkin;								//改变皮肤函数
	int IsDlg;										//是否模态窗体
}GUI_FORM, *PGUI_FORM;

#if 0
typedef struct
{
	int TextID;                                     // 按键文字
	int ImageID;                                    // 按键图片
	uint8 enable;                                   // 按键使能
	uint8 mode;                                     // 按键模式    1:为按下抬起后变回原来的状态  0:按下换种状态
	uint8 state;                                    // 按键状态
}BUTTON_INFO, *PBUTTON_INFO;

typedef struct
{
	PGUI_FORM ParentFrm;                             // 父窗口
	int8 ButtonNum;                                  // 按键个数
	BUTTON_INFO Buttons[BUTTON_COUNT];               // 单个按键结构体
	int8 select;                                     // 按键选择状态
	char text[60];                                   // 文字信息
	int8 On_ID;                                      
}RIGHT_BUTTON, *PRIGHT_BUTTON;
#endif

typedef struct
{
	PGUI_FORM ParentFrm;                             // 父窗口
	uint32 TextID;                                   // 按键文字
	uint32 ImageID;                                  // 按键图片
	uint8 enable;                                    // 按键使能
	uint8 keystate;
	uint8 state;                                     // 按键状态 0:没被选择 1:选择
	uint8 check;                                     // 0:选择后按键图片不变   1:选择后按键图片变为被选中图片
}KEY_BUTTON, *PKEY_BUTTON;

typedef struct
{
	PGUI_FORM ParentFrm;                             // 父窗口
	int TextID;                                      // 按键文字
	uint8 enable;                                    // 按键使能
	uint8 state;                                     // 按键状态 0:没被选择 1:选择
	uint8 mode;                                      // 0:为圆框按键 1:为方框按键
}CHOSE_BUTTON, * PCHOSE_BUTTON;

typedef enum
{
	SECU_MODE 	 = 0,								// 报警模式
	ISO_MODE,										// 隔离模式
	PART_MODE,										// 局防有效模式
	DEFIND_MODE,									// 属性模式
	AREA_TYPE_MODE,									// 防区类型模式
	JIADIAN_MODE,  									// 家电模式
}SECU_SHOW_MODE;

typedef struct
{
	uint8			  	area_info[8];				// 防区信息
	uint8				area_type[8];				// 防区类型
	uint8				other_info;					// 隔离或局防有效的信息
	PGUI_FORM 			ParentFrm;					// 父窗体	
	SECU_SHOW_MODE  	mode;						// 选择的模式
	int8				select;						// 当前选中的状态
	uint8               amount;						// 按钮总数，最多8个
	uint8               column;						// 每行有几个
	uint8               always;						// 常亮显示
	uint8               text[8][20];				// 文字id
	uint32              imageid[8];                 // 图片id
}SECU_BOTTON_INFO, *PSECU_BOTTON_INFO;

typedef struct
{
	PGUI_FORM ParentFrm;							// 父窗体	
	NT_BOOL  	mode;								// 返回模式 0 立即返回 1 弹起返回 
	NT_BOOL 	enable;							 
	uint16 	value;
	uint16  max_val;								// 最大值
	uint16  min_val;								// 最小值
	uint32  text_id;								// 起始文字ID
	uint32   *ptext;								// 全局或静态数组
}SPIN_EDIT_INFO, *PSPIN_EDIT_INFO;

typedef enum
{
	KEYBOARD_MODE0 = 0,					     		// 白底密码按键
	KEYBOARD_MODE1,							 		// 对讲按键	
	KEYBOARD_MODE2,                          		// 黑底密码按键
}KEYBOARD_STYLE;									

typedef struct
{
	//PGUI_FORM ParentFrm;							// 父窗体
	uint8 InputNum;									// 输入框个数
	#ifdef _E81S_UI_STYLE_
	HWND InputhDlg[5];                         		// 输入框句柄
	#else	
	HWND InputhDlg[2];                         		// 输入框句柄
	#endif
	int8 Max;							    		// 最大
	KEYBOARD_STYLE Mode;							// 模式 
	uint8 Enable;									// 使能
	int8 KeyValue;						    		// 键值	
	uint8 State;									// 按下状态
	uint8 IPmode;									// IP模式
}KEYBOARD_INFO, *PKEYBOARD_INFO;


typedef struct
{
	uint32 TextID;									// 数据的文字ID
	uint32 ImageID;								    // 数据的图片ID
	char name[30];									// 数据的文字字符串
	char *text;                                     // 信息内容
	char date[80];                                  // 数据的时间字符串
}EVE_INFO, *PEVE_INFO;

#if 0
typedef struct
{
	list_t list;
	EVE_INFO Buttons;							// 数据
}LIST_MSG_DATE, * PLIST_MSG_DATE;

typedef struct
{
	uint32	MaxListCount;							// 总共显示行数
	uint32  Page_Col;                               // 每页显示列数
	uint32  Page_Count;								// 每页显示的行数
	uint32  Select;									// 当前选中的个数
	int		one_w;									// 单行的宽度
	int		one_h;									// 单行的高度
	uint8   del_enable;								// 是否允许删除
	uint32   TextID;                                // 标题的名字
	list_t listItem;								// 列表项
}MSG_DATE, *PMSG_DATE;
#endif

typedef struct
{
	uint8 MaxValue;
	uint8 NowValue;
	uint8 enable;
}VOLUME_INFO, *PVOLUME_INFO;

#if 0
typedef struct
{
	DWORD   status;         	// status of box
	RECT    rcText;          	// content rect
	char	numtext[31];    	// string text buffer
	int		numlen;
	uint32  BKColor;			
	char *  hint;				//hint为NULL时为输入模式
	int		mode;				//0：一般格式  1:数字格式,2：时间格式;3:IP输入格式
	int 	selpos;				//当前光标的位置；当mode=3 时为IP四段中的某段 Add by hxf 2006.10.14
	int 	Maxchar;			//最多字符个数
	int 	ippos[4];				//
	char 	ip[4][3];			//4段ｉｐ值
	int 	selposBegin;
	int 	selposEnd;				
}LFLAT_EDIT_DATA, *PLFLAT_EDIT_DATA;
#endif

typedef enum
{
	PASS_ADMIN,										// 管理员密码
	PASS_USER,										// 用户密码
	PASS_DOOR,										// 开门密码
}AU_PASS_TYPE;

typedef enum
{
	SYTLE_NONE			= 0x00,						// 无								
	STYLE_PICTURE_SOUND = 0x11,						// 声音+图片
	STYLE_ONLY_SOUND    = 0x12,
}ITEM_STYLE;

#if 0
typedef struct
{
	list_t list;
	uint8 IsRead;									// 是否读过 0未读 1读过
	ITEM_STYLE style;								// 留言用该类型
	char *text;										// 数据内容(全局路径)
	char *name;										// 数据内容(文件名)
}EXPLORER_ITEM, *PEXPLORER_ITEM;					// 用于更改单个行内结构
#endif

typedef enum
{
	STATE_NONE = 0x10,
	STATE_STOP,
	STATE_PLAY,
	STATE_REC,
}PLAY_STATE;

typedef struct
{
	uint8 kind;                                     // 家电类型  0:灯光调节  1:温度调节
	uint8 value;                                    // 当前的值
	uint8 state;                                    // 开关状态  0:关  1:开
	uint8 MaxValue;                                 // 最大的值
	uint16 addr;                                    // 设备地址
}JIADIAN_ITEM, *PJIADIAN_ITEM;

typedef enum
{ 
	MODE_NONE      =   0,							// 不显示
	MODE_COMMON    =   1,							// 显示可以调节(<-->)	
	MODE_LIGHT     =   2,							// 灯光可调节
	MODE_AIR	   =   3,							// 空调可调节
} SINGLELINE_MODE;									// 单行的类型

typedef enum
{
	VOLUME_MUTE = 1,                                // 静音
	VOLUME_DEL,                                     // 音量减
	VOLUME_ADD,	                                    // 音量加
} VOLUME_TYPE;

/**********************结构体定义************************/

/************************外部定义************************/
// 注册控件
BOOL register_input_ctrl(void);
BOOL register_right_button(void);
BOOL register_num_keybord(void);
BOOL register_list_ctrl(void);
BOOL register_eightarea_ctrl(void);
BOOL register_spin_ctrl(void);
BOOL register_procress_ctrl(void);
#if 0
BOOL register_chose_button(void);
BOOL register_area_button(void);
BOOL register_key_button(void);
BOOL register_spin_edit(void);
BOOL register_msg_button(void);
BOOL register_volume_button(void);
BOOL register_au_flat_edit(void);
#endif

// 卸载控件
void unregister_input_ctrl(void);
void unregister_right_button(void);
void unregister_num_keybord(void);
void unregister_list_ctrl(void);
void unregister_eightarea_ctrl(void);
void unregister_spin_ctrl(void);
void unregister_procress_ctrl(void);
#if 0
void unregister_chose_button(void);
void unregister_area_button(void);
void unregister_key_button(void);
void unregister_spin_edit(void);
void unregister_msg_button(void);
void unregister_volume_button(void);
void unregister_au_flat_edit(void);
#endif

// 函数定义
extern void ui_callout_appstart(DEVICE_TYPE_E DevType, char *DevNo);
extern void ui_monitor_appstart(DEVICE_TYPE_E DevType, uint8 index);

#ifdef __cplusplus
}
#endif
#endif

