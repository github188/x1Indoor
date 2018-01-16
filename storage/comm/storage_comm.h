/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	storage_comm.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-11-03
  Description:  存储模块公共接口
  				存储模块要用到的公共接口都在这实现
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef __STORAGE_COMM_H__
#define __STORAGE_COMM_H__
#include "storage_types.h"

#if 0
typedef enum
{
	CHINESE = 0x00,
	ENGLISH,
	TAIWAN,
	MAX_LANGUAGE
}LANGUAGE_LIST;
#endif

//extern uint8 g_China;
//extern uint8 g_Big5;

//void set_language(LANGUAGE_E language);
//LANGUAGE_E get_language(void);

typedef enum
{
    FLAG_CALLIN = 0x00,
	FLAG_CALLOUT,
	FLAG_CALLMISS,
	FLAG_AFCZ,
	FLAG_AFBJ,
	FLAG_COUNT
}FLAG_STORAGE;

typedef enum
{
    ECHO_STORAGE_OK = 0x00,
	ECHO_STORAGE_ERR,
	ECHO_STORAGE_NOFILE,
	ECHO_STORAGE_FULL,
	ECHO_STORAGE_SAME_ADDR,
	ECHO_STORAGE_SAME_NAME,
	ECH0_STORAGE_ERR_ADDR
}ECHO_STORAGE;

#define MSG_SYS_START  			0X0000
#define MSG_APP_START 			0X8000

typedef enum
{
    MS_ALVI_UnloadProcReq = MSG_SYS_START,
    MS_VIVI_ReleaseTaskSemInd , 					// 不同优先级任务之间的控制消息

	// testMsg
    MS_GUAL_HelloInd,
    MS_ALGU_HelloResp,
    MS_ETGU_ActiveETReq,
    MS_GUGU_PowerOff,

	// WD
    MS_ISRWD_TickInd = 50,
    MS_WDAL_PingReq,
    MS_WDAL_PingResp,

	// USB
    MS_USB_DISCONNECT = 70,
    MS_USB_CONNECT,
    MS_USB_POLL,

	// timer
    MS_ISRTM_TickInd = 100,


    // gui
    MS_SYGU_SysInitInd = 200,
    MS_SYGU_SetUpInd,
    MS_ISRGU_KeyInd , 
    MS_TMGU_SecondInd,
    MS_TMGU_KeyTimeOutInd,
    MS_GUGU_PostMsgInd,

    MS_ALGU_SysShutDownReq,

    MS_TMGU_TouchInd,
    MS_TMGU_MSInd,
    MS_TMGU_WinTimerInd,
    MS_GUGU_SwitchAppInd, 							// APPLICATION 切换.

    // 080623,haungsl,增加该消息用于返回主界面,用户可以定义自己不需要关闭的应用.
    MS_AUGU_ExittoDesktop,    						// 在任何情况下,返回主界面

    MS_ISRGU_SdcardInd = 300, 						// msg =1: insert , msg=0:remove 
    MS_ISRGU_USBCableInd , 							// USB 线，1 插入， 0 拔出
    MS_TMGU_AlarmInd,

    // GUI TEST MSG .
    MS_TSGU_GuiMbox = 400,
    MS_TSGU_Gui0Mbox,

    // display
    MS_DIGU_WinMoveEndInd = 500, 					// 滑动窗口显示结束，msg:滑动类型--MOVEOUT , MOVEIN 
    MS_ISRDI_TickInd , 
    MS_ISRDI_FlushInd,
    MS_DIDI_LCDUpdateInd,
    MS_DIDI_LCDFrameInd,
    MS_GUDI_LCDClearInd,  							// for test lcd 
    MS_GUDI_LCDCSetViewAeraAlpha,
    MS_GUDI_LCDCSetPhyPosiAlpha,

	MS_INTAU_BeepStart,								// BeepSound
	MS_INTAU_Beep,
	MS_INTAU_BeepStop,

    // flushed msg
    MS_GIVEN_Msg_Flushed = 600,      				// for test lcd 

   	/* 以下为应用消息 */
    AS_ALL_START = MSG_APP_START,

	// 音频播放控制消息
    AS_AUAU_DecInit,
    AS_AUAU_DecStart,
    AS_GUAU_Playing,
    AS_GUAU_Stop ,
    AS_GUAU_Pause,
    AS_GUAU_Resume,
    AS_AUAU_DecError,
    
    AS_AUAU_TALK_START,	
    AS_AUAU_TALKING_IN,
    AS_AUAU_TALKING_OUT,
    AS_AUAU_TALK_STOP,
    AS_GUAU_Transmit,

	AS_AUAU_LYLY_START,
	AS_AUAU_LYLY_PROCESS,
	AS_AUAU_LYLY_STOP,
	AS_GUAU_LYLYPLAY_START,
	AS_GUAU_LYLYPLAY_STOP,

	AS_AUAU_HINT_START,
	AS_AUAU_HINT_STOP,

	// add by luofl 2013-12-25 解决部份机器视频闪烁问题
	AS_AUAU_H264_SEND,

	AS_GUAU_LEAVE_START,
	AS_GUAU_LEAVE_WRITE,
	AS_GUAU_LEAVE_STOP,

	// 录音
	AS_GUAU_TSStopRecReq = (MSG_APP_START + 200),
	AS_GUAU_RECORDWRITE,          					// 录音写文件
	AS_GUAU_SAVEDATA,
    AS_GUAU_RECORDCLOSE,
    AS_ISRAU_TSRecDMAIntInd,
    AS_GUAU_RecPrepare,

    // video
    AS_GUVI_VideoInit  = (MSG_APP_START + 300),
    AS_GUVI_VideoStart,
    AS_GUVI_VideoEnd,
    AS_GUVI_VideoFillBuf,
    AS_GUVI_VideoStop,
    AS_GUVI_CLOSE,
    AS_GUVI_DEC,
    AS_VIGU_DEC_FALSE,
    AS_GUVI_ZOOM,
    AS_GUVI_ROTATE,
    AS_GUVI_MOV,
    AS_VIGU_ZOOM_OK,
    AS_VIGU_MOVE_OK,
    AS_GUVI_IMG_RESET,
    AS_GUVI_GIF_NEXT_FRAME,
    AS_GUVI_NINE_SCREEN,
    AS_VIVI_BROWSER_DEC_OK,
    AS_VIVI_BROWSER_DEC_FALSE,
    
    // other
    AS_NET_RECV_UDPPACKET_NOTIFY  = (MSG_APP_START + 400),
	AS_NET_RECV_PACKET_DISTRIBUTE,			
	AS_NET_MULTI_SEND_NOTIFY,
	LWIP_MBOX_POST = (MSG_APP_START+1102),

	AS_CALLIN,
	AS_INTER_REQUEST,
	AS_INTER_CALLING,
	AS_INTER_RECORD_HINT,
	AS_INTER_RECORDING,
	AS_INTER_TALK,
	AS_INTER_END,

	AS_MONITOR_SEARCH,
	AS_MONITOR_REQUEST,
	AS_MONITOR_MONITORING,
	AS_MONITOR_TALK,
	AS_MONITOR_END,

	AS_RTSP_REQUEST,
	AS_RTSP_MONITORING,
	AS_RTSP_END,
	AS_RTSP_HEART,
	AS_RTSP_GETLIST,

	AS_ALARM_DEAL_MSGID,
	AS_ALARM_DAIL,
	AS_ALARM_REDAIL,
	AS_ALARM_PROC,
	AS_NEW_MSG,
	AS_ENTER_YUJING,								// 进入预警(延时防区触发)消息

    AS_UART0_REV_DATA,								// 串口0接收到数据

    AS_STORAGE_LIGHT,                               // 家电灯光存储

	AS_UPDATA_JD_DEV,								// 同步家电信息				
	
	LASTMSG											// 必须放在最后
} SYS_MSG_ID;

#define INVALID_MSGID   LASTMSG  					// 无效消息ID

/*************************************************
  Function:		storage_get_ring_out
  Description: 	获取回铃声全局路径
  Input:		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_out(void);

/*************************************************
  Function:		storage_get_ring_fenji
  Description: 	获取分机铃声全局路径
  Input:		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_fenji(void);

/*************************************************
  Function:		storage_get_lyly_tip_path
  Description: 	获取LYLY提示音路径
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_lyly_tip_path(void);

/*************************************************
  Function:		SaveRegInfo
  Description: 	保存注册表
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void SaveRegInfo(void);

/*************************************************
  Function:		DelayMs_nops
  Description: 	
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void DelayMs_nops(uint32 count);

/*************************************************
  Function:		IsHaveFile
  Description: 	是否有该文件
  Input:		
  	1.Filename	文件名
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE IsHaveFile(char* Filename);

/*************************************************
  Function:     is_fileexist
  Description:  判断指定路径文件是否存在
  Input:       	
   	1.path		指定文件的路径
  Output:       无
  Return:
				文件存在:TRUE
				文件不存在:FALSE
  Others:
*************************************************/
int32 is_fileexist(char * path);

/*************************************************
  Function:		get_size
  Description: 	获得文件夹大小
  Input:		
  	1.Filename	文件夹名
  Output:		无
  Return:		SIZE
  Others:
*************************************************/
int get_size (char* srcname);

/*************************************************
  Function:    		FSFileDelete
  Description: 		删除指定的文件
  Input: 
  	1.path			文件路径名
  Output:     		无
  Return:
					成功0:失败返回错误号
					错误代码  EROFS 文件存在于只读文件系统内
					EFAULT 参数pathname指针超出可存取内存空间
					ENAMETOOLONG 参数pathname太长
					ENOMEM 核心内存不足
					ELOOP 参数pathname 有过多符号连接问题
					EIO I/O 存取错误
  others:
*************************************************/
int32 FSFileDelete(char * path);

/*************************************************
  Function:     	FSFlush
  Description:  	
  Input:       	
  Output:           无
  Return:
  Others:			文件数据写入磁盘
*************************************************/
void FSFlush(FILE* pFile);

/*************************************************
  Function:		Fwrite_common
  Description: 	写数据
  Input:		
  	1.Filename	记录类型
  	2.Data		设备类型
  	3.DataSize	数据结构大小
  	4.DataNum	数据个数
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE Fwrite_common(char* Filename, void* Data, int DataSize, int DataNum);

/*************************************************
  Function:		Fread_common
  Description: 	读数据
  Input:		
  	1.Filename	记录类型
  	2.Data		设备类型
  	3.DataSize	数据结构大小
  	4.DataMaxNum 数据个数
  Output:		无
  Return:		数据个数
  Others:
*************************************************/
uint32 Fread_common (char* Filename, void* Data, int DataSize, int DataMaxNum);

/*************************************************
  Function:		storage_set_save_flag
  Description: 	设置存储标志
  Input:		
  	1.Flag		类型
  	2.state		状态(TURE -- 定时存储 FALSE -- 无须存储)
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_set_save_flag (FLAG_STORAGE Flag, uint8 state);

/*************************************************
  Function:		storage_get_save_flag
  Description: 	获取存储标志
  Input:		
  	1.Flag		类型
  Output:		无
  Return:		状态
  Others:
*************************************************/
uint8 storage_get_save_flag(FLAG_STORAGE Flag);

/*************************************************
  Function:		storage_comm_model_save
  Description: 	公共定时存储
  Input:		无
  Output:		无
  Return:		无
  Others:		所有模块的定时存储都在这
*************************************************/
void storage_comm_model_save(void);

/*************************************************
  Function:		storage_get_language
  Description: 	获取语言
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
LANGUAGE_E storage_get_language(void);

/*************************************************
  Function:		storage_set_language
  Description: 	设置语言
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_set_language(LANGUAGE_E language);

/*************************************************
  Function:		storage_get_openscreen
  Description: 	获取开屏状态
  Input:		无
  Output:		无
  Return:		0开屏 1关屏
  Others:
*************************************************/
uint8 storage_get_openscreen(void);

/*************************************************
  Function:		stroage_set_openscreen
  Description: 	设置关屏状态
  Input:		无
  Output:		无
  Return:		0开屏 1关屏
  Others:
*************************************************/
void stroage_set_openscreen(uint8 state);

/*************************************************
  Function:		storage_get_extmode
  Description: 	获取外部模块
  Input:		无
  Output:		无
  Return:		TRUE 启用 FALSE 不启用
  Others:
*************************************************/
uint32 storage_get_extmode(EXT_MODE_TYPE type);

/*************************************************
  Function:		storage_set_extmode
  Description: 	设置外部模块
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_extmode(uint8 save, EXT_MODE_TYPE type, uint8 state);

/*************************************************
  Function:		storage_get_bright
  Description: 	获取背光
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_bright(void);

/*************************************************
  Function:		storage_set_bright
  Description: 	设置背光
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_bright(uint8 save, uint8 Bright);

/*************************************************
  Function:		storage_get_desk
  Description: 	获取桌面ID
  Input:		无
  Output:		无
  Return:		桌面ID
  Others:
*************************************************/
uint32 storage_get_desk(void);

/*************************************************
  Function:		storage_set_desk
  Description: 	设置桌面ID
  Input:		无
  Output:		无
  Return:		正确与否
  Others:
*************************************************/
uint32 storage_set_desk(uint8 save, uint8 ID);

/*************************************************
  Function:		storage_get_mac
  Description: 	获取MAC
  Input:		
  	1.type		MAC类型
  Output:		无
  Return:		密码
  Others:
*************************************************/
uint8 * storage_get_mac(MAC_TYPE type);

/*************************************************
  Function:		storage_set_mac
  Description: 	设置MAC
  Input:		
  	1.type		MAC类型
  	2.data		MAC码
  Output:		无
  Return:		TRUE、FALSE
  Others:
*************************************************/
uint32 storage_set_mac(MAC_TYPE type, char * data);

/*********************************************************/
/*					 	密码设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_pass
  Description: 	获取密码
  Input:		
  	1.type		密码类型
  Output:		无
  Return:		密码
  Others:
*************************************************/
uint8 * storage_get_pass(PASS_TYPE type);

/*************************************************
  Function:		storage_get_pass
  Description: 	获取密码
  Input:		
  	1.type		密码类型
  	2.data		密码
  Output:		无
  Return:		长度
  Others:
*************************************************/
uint32 storage_set_pass(PASS_TYPE type, char* data);

/*************************************************
  Function:		storage_get_doorserver
  Description: 	获取家政开门密码有效
  Input:		无
  Output:		无
  Return:		
  Others:
*************************************************/
uint8 storage_get_doorserver(uint8 pass_type);

/*************************************************
  Function:		storage_set_doorserver
  Description: 	设置家政开门密码有效
  Input:		
  	1.enable
  Output:		无
  Return:		
  Others:
*************************************************/
uint32 storage_set_doorserver(uint8 enable, uint8 pass_type);

/*********************************************************/
/*					 	音量设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_ringvolume
  Description: 	获取铃声大小
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_ringvolume(void);

/*************************************************
  Function:		storage_get_talkvolume
  Description: 	获取通话大小
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_talkvolume(void);

/*************************************************
  Function:		storage_get_keykeep
  Description: 	获取按键音
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_keykeep(void);

/*************************************************
  Function:		storage_set_volume
  Description: 	获取声音大小
  Input:		
  	1.ring
  	2.talk
  	3.key
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_volume(uint8 ring, uint8 talk, uint8 key);

/*************************************************
  Function:			stroage_sdp_param
  Description:		初始化视频编码参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void stroage_sdp_param_init(void);

/*************************************************
  Function:		storage_get_noface
  Description: 	免打扰状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_noface(void);

/*************************************************
  Function:		storage_get_ring_by_devtype
  Description: 	获取铃声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_by_devtype(DEVICE_TYPE_E type);

/*************************************************
  Function:		storage_get_msg_hit
  Description: 	获取信息提示声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_msg_hit(void);

/*************************************************
  Function:		storage_get_lyly_enable
  Description: 	获取LYLY使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_enable(void);

/*************************************************
  Function:		storage_get_lyly_mode
  Description: 	获取LYLY模式
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
LYLY_TYPE storage_get_lyly_mode(void);

/*************************************************
  Function:		storage_get_lyly_Linkmode
  Description: 	获取LYLY联动状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_Linkmode(LYLY_LINK_TYPE type);

/*************************************************
  Function:		storage_get_lyly_tip
  Description: 	获取LYLY提示音状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_tip(void);

/*************************************************
  Function:		storage_set_lyly_tip
  Description: 	设置LYLY提示音状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_lyly_tip(uint8 tip);

/*************************************************
  Function:		storage_get_screenparam
  Description: 	获得屏幕保护数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
PEPHOTO_PARAM storage_get_screenparam(void);

/*************************************************
  Function:		storage_set_screenparam
  Description: 	设置屏幕保护数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_set_screenparam(PEPHOTO_PARAM param);

/*************************************************
  Function:		storage_init
  Description: 	存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_init(void);

/*************************************************
  Function:		storage_set_noface_enable
  Description: 	设置免打扰状态
  Input:		
  	1.enable
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_set_noface_enable(uint8 enable);

/*************************************************
Function:   	 	storage_set_ipaddr
Description: 		初始化ip
Input:              无
Output:				无
Return:				成功－ture 失败－false
Others:
*************************************************/
uint32 storage_set_ipaddr(void);

/*************************************************
Function:   	 	storage_netdoor_set_ipaddr
Description: 		初始化ip
Input:              无
Output:				无
Return:				成功－ture 失败－false
Others:
*************************************************/
uint32 storage_netdoor_set_ipaddr(uint32 ip, uint32 netmack, uint32 gateway);

/*************************************************
  Function:		storage_get_closelcd_time
  Description: 	关屏幕时间
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_closelcd_time(void);

/*************************************************
  Function:		storage_get_lyly_param
  Description: 	获取LYLY使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
PLYLY_PARAM storage_get_lyly_param(void);

/*************************************************
  Function:		storage_get_lyly_param
  Description: 	设置LYLY使能
  Input:		
  	1.param		结构
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_lyly_param(PLYLY_PARAM param);

/*************************************************
  Function:		storage_get_ring_id
  Description: 	获取铃声ID
  Input:		
  	1.index		(梯口 区口 门口 中心 住户 分机 信息 预警)
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_ring_id(uint8 index);

/*************************************************
  Function:		storage_get_ring_id
  Description: 	获取铃声ID
  Input:		
  	1.index		(梯口 区口 门口 中心 住户 分机 信息 预警)
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_ring_id(uint8 index, uint8 id);

/*************************************************
  Function:		storage_get_msg_hit_byID
  Description: 	获取信息提示声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_msg_hit_byID(uint8 id);

/*************************************************
  Function:		storage_get_yj_path_byID
  Description: 	获取预警声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_yj_path_byID(uint8 id);

/*************************************************
  Function:		storage_get_ring_by_id
  Description: 	获取铃声全局路径
  Input:		
  	1.id		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_by_id(uint8 id);

#ifdef _USE_NEW_CENTER_

/*************************************************
  Function:			storage_set_predevno
  Description:		保存注册成功设备编号
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
DEVICE_NO storage_get_predevno(void);

/*************************************************
  Function:			storage_set_predevno
  Description:		保存注册成功设备编号
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int storage_set_predevno(PDEVICE_NO DeviceNo);

#endif

#endif

