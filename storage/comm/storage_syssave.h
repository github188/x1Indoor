/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	storage_syssave.h
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-10-02
  Description:  系统参数存储头文件

  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#define SaveMagicNumber1		0x19821113
#define SaveMagicNumber2		0x852167ab

// 结构体要8 BYTE对齐
typedef struct _SYSCONFIG
{
    int32 Size;
    int32 MagicNumber1;								// 特殊字符，用来查看结构体数据是否正确

    AF_FLASH_DATA AfFlashData;						// 安防模块存储的参数
    SYS_FLASH_DATA SysFlashData;					// 系统参数
    VIDEO_SDP_PARAM VideoSdpData;					// 视频SDP描述
	AUDIO_SDP_PARAM AudioSdpData;					// 音频SDP描述
    
    int32 MagicNumber2;								// 特殊字符，用来查看结构体数据是否正确
    //int16 PAD;										// 填充8字节对齐
} SYSCONFIG, * PSYSCONFIG;

/*************************************************
  Function:		check_sysconfig_ifcorrect
  Description: 	校验数据是否正确
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void check_sysconfig_ifcorrect(void);

/*************************************************
  Function: 	SaveRegInfo
  Description:	保存注册表-系统配置参数
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void SaveRegInfo(void);

/*************************************************
  Function:    	storage_recover_factory
  Description: 	恢复出厂设置
  Input: 		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
void storage_recover_factory(void);

/*************************************************
  Function:    	storage_format_system
  Description: 	格式化NAND1-2盘
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_format_system(void);

/*************************************************
  Function:		storage_init_sysconfig
  Description: 	初始化系统配置
  Input:		无
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_init_sysconfig(void);

