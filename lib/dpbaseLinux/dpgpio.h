#pragma once

#ifndef METHOD_BUFFERED
#define METHOD_BUFFERED			0
#endif
#ifndef FILE_ANY_ACCESS
#define FILE_ANY_ACCESS			0
#endif
#ifndef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif

#define IOCTRL_GPIO_INIT 				0x4c434b06
#define IOCTRL_SET_GPIO_VALUE			0x4c434b07
#define IOCTRL_GET_GPIO_VALUE			0x4c434b08

#define IOCTL_START_WATCHDOG		0x12345678
#define IOCTL_REFRESH_WATCHDOG		0x12345679
#define IOCTL_STOP_WATCHDOG			0x1234567a

typedef struct
{
	WORD gpio_num;
	UINT8 gpio_cfg;		//0 input 1 output (0~7)
	UINT8 gpio_pull;	//PULL_DISABLE,PULL_UP,PULL_DOWN	
	UINT8 gpio_driver;	//0~3
} GPIO_CFG;

#define IOCTL_GRAPHIC_DISP			0x200
#define IOCTL_DISP_ADJUST_BKL		CTL_CODE(IOCTL_GRAPHIC_DISP, 0x242, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISP_ADJUST_FE		CTL_CODE(IOCTL_GRAPHIC_DISP, 0x255, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
	DWORD bright;
	DWORD contrast;
	DWORD saturation;	
}BklParam;

typedef struct
{
	DWORD bright;
	DWORD contrast;
	DWORD saturation;	
	DWORD hue;
}CscParam;

#define FILE_DEVICE_SPIUPDATE 32889

#define IOCTL_CHECK_UPDATAFILE CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x925, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UPDATA_IMAGE CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x926, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_UPDATEPRERCET CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x927, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_OTP CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x928, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_OTP CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x929, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ERASE_OTP CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x92a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_SRP CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x92b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_SRP CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x92c, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_NK_UPDATA			CTL_CODE(FILE_DEVICE_SPIUPDATE, 0x92a, METHOD_BUFFERED, FILE_ANY_ACCESS)//更新整个DD文件但不清空USERDEV目录，SD卡更新USERDEV目录会全部清空

#define FILLCOLOR				1
#define BITBLT					2
#define STRETCH_BLT				3
#define ALPHABLEND_BLT			4
#define	ROTATE_BLT				5

#define IOCTL_SPR_BASE 			0x201
#define	IOCTL_SPR_REQUEST		CTL_CODE(IOCTL_SPR_BASE, 0x220, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 申请精灵块
#define	IOCTL_FRM_REQUEST		CTL_CODE(IOCTL_SPR_BASE, 0x221, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 申请layer1显示frame
#define	IOCTL_BAR_REQUEST		CTL_CODE(IOCTL_SPR_BASE, 0x222, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 申请layer0显示frame
#define	IOCTL_BLK_DUP			CTL_CODE(IOCTL_SPR_BASE, 0x223, METHOD_BUFFERED, FILE_ANY_ACCESS)	
#define	IOCTL_BLK_CHANGE		CTL_CODE(IOCTL_SPR_BASE, 0x224, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_BLK_REMOVE		CTL_CODE(IOCTL_SPR_BASE, 0x230, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 释放图层
#define	IOCTL_BLK_SHOW			CTL_CODE(IOCTL_SPR_BASE, 0x231, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 设置图层可显示
#define	IOCTL_BLK_HIDE			CTL_CODE(IOCTL_SPR_BASE, 0x232, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 设置图层不可显示
#define	IOCTL_BLK_MOVE			CTL_CODE(IOCTL_SPR_BASE, 0x233, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 修改图层所在位置
#define	IOCTL_DEV_SHOW			CTL_CODE(IOCTL_SPR_BASE, 0x240, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 显示所有可以显示的图层
#define	IOCTL_DEV_HIDE			CTL_CODE(IOCTL_SPR_BASE, 0x241, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_DEV_TOP			CTL_CODE(IOCTL_SPR_BASE, 0x242, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_B0_HIDE			CTL_CODE(IOCTL_SPR_BASE, 0x243, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_B0_SHOW			CTL_CODE(IOCTL_SPR_BASE, 0x244, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_RANDOM_SHOW		CTL_CODE(IOCTL_SPR_BASE, 0x260, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_TEMP_FRM_REQUEST	CTL_CODE(IOCTL_SPR_BASE, 0x261, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 申请一块硬件图层，DP3000
#define	IOCTL_TEMP_FRM_RELEASE	CTL_CODE(IOCTL_SPR_BASE, 0x262, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 释放一块硬件图层, DP3000
#define	IOCTL_BITBLT			CTL_CODE(IOCTL_SPR_BASE, 0x263, METHOD_BUFFERED, FILE_ANY_ACCESS)	// 进行块操作, DP3000
#define IOCTL_SET_BKGRD			CTL_CODE(IOCTL_SPR_BASE, 0x268, METHOD_BUFFERED, FILE_ANY_ACCESS)	

#define	DECODE_MP4		0x1
#define	DECODE_H263		0x2
#define	DECODE_H264		0x3
#define	DECODE_JPEG		0x4
#define	DECODE_PNG		0x5

#define	DECODE_OUTDIR	0x01
#define	DECODE_OUTRGB	0x02
#define	DECODE_OUTYUV	0x03
#define	DECODE_OUTMEM	0x04

#define	ENCODE_H264		0x01
#define	ENCODE_JPEG		0x02
#define	ENCODE_MP4		0x03

#define	ENCODE_BLANK_ON		0x01
#define	ENCODE_BLANK_OFF	0x02
#define	ENCODE_OSD_ON		0x04
#define	ENCODE_OSD_OFF		0x08
#define	ENCODE_QUALITY		0x10
#define	ENCODE_SKIP			0x20
#define	ENCODE_REALTIME		0x40
#define	ENCODE_MAXKEY		0x80
#define	ENCODE_USESBUF		0x100
#define	ENCODE_SET_SCALER	0x2000
#define ENCODE_CHAR_MAP		0x4000

#define	ENCODE_IFRAME		0x01

#define	PREV_USESBUF		0x100
#define	ENCODE_VFUREQUEST   0x1000


#define DECODE_SHOW_LAYER      0x01
#define	DECODE_HIDE_LAYER		0x02

enum
{
	ENC_ERR_MODE	= 0x10000,		// Not work in ENC mode
	ENC_ERR_TMOUT	= 0x10001,		// Wait new frame timeout
	ENC_ERR_BUFSHORT= 0x10002,		// Enc buffer length short
	ENC_ERR_BUFNULL	= 0x10003,		// Enc buffer is NULL
} ;

typedef struct
{
	DWORD charnum;
	DWORD height;
	BYTE* width;
	BYTE* pixmap;
} CharMap;

typedef struct
{
	DWORD m_outtype;	// 
	DWORD m_winl;	// display window left
	DWORD m_wint;	// display window top
	DWORD m_winw;	// display window width
	DWORD m_winh;	// display window hight
	DWORD property;
} Preview_Info;

typedef struct
{
	DWORD m_dectype;
	DWORD m_outtype;	// 
	DWORD m_winl;		// display window left
	DWORD m_wint;		// display window top
	DWORD m_winw;		// display window width
	DWORD m_winh;		// display window hight
	DWORD m_decw;		//
	DWORD m_dech;		//
	DWORD m_property;	
} Decode_Info;

typedef struct
{
	DWORD m_enctype;
	DWORD m_encw;	// camera resolution width
	DWORD m_ench;	// camera resolution hight
	DWORD property;
	WORD  osd_x;		// osd left
	WORD  osd_y;		// osd top
	DWORD m_quality;	// 1~16
	DWORD m_bcolor;		// when blanc, filled color
	DWORD m_skiptime;
	DWORD m_maxkey;
	DWORD m_scalerinfo;
	DWORD value[100];
} Encode_Info;

typedef struct
{
	BYTE* buf;
	DWORD dwsize;
	DWORD timeout;
	DWORD dec_pts;
} WRITE_INFO;

enum
{
	I_VOP = 0,
	P_VOP = 1
};

typedef struct
{
	BYTE* buf;
	DWORD dwsize;
	DWORD timeout;
	DWORD property;
} READ_INFO;

typedef struct
{
	WRITE_INFO w;
	READ_INFO r;
} WR_INFO;

typedef enum
{
	SIF_BRIGHTNESS	= 0x01,		// Value from 0 ~ 0xff
	SIF_HUE			= 0x02,		// Value from 0 ~ 0xff
	SIF_CONTRAST	= 0x04,		// Value from 0 ~ 0xff
	SIF_SATURATION	= 0x08,		// Value from 0 ~ 0xff
	SIF_CHANGEIN	= 0x10,		// Value from 1 ~ 3
} Sif_Support;

#define	YUV422		1	// jpeg encode
#define	YUV420		2	// mp4 encode

typedef enum
{
	SIF_GETBRIGHTNESS	= 0x01,
	SIF_SETBRIGHTNESS	= 0x02,
	SIF_GETHUE			= 0x03,
	SIF_SETHUE			= 0x04,
	SIF_GETCONTRAST		= 0x05,
	SIF_SETCONTRAST		= 0x06,
	SIF_GETSATURATION	= 0x07,
	SIF_SETSATURATION	= 0x08,
	SIF_GETOUTSIZE 		= 0x09,
	SIF_SETOUTSIZE 		= 0x0A,
	SIF_GETOUTFMT		= 0x0B,
	SIF_SETOUTFMT		= 0x0C,
	SIF_GETWINSIZE		= 0x0D,
	SIF_SETWINSIZE		= 0x0E,
	SIF_GETCHANGEIN 	= 0x0F,
	SIF_SETCHANGEIN 	= 0x10,
	SIF_SETSINGLEBUF	= 0x11,
	SIF_GETREGVAL		= 0x12,
	SIF_SETREGVAL		= 0x13
} Sif_Cmd;

typedef struct 
{
	WORD	Width;
	WORD	Height;
} SIF_SIZE;

typedef struct 
{
	DWORD		Supported;
	SIF_SIZE 	OutSize[6];
} Sensor_Prop;

typedef struct 
{
	DWORD		val;
	DWORD		SifProperty;
}Sensor_Set;

typedef struct
{
	Decode_Info		decode;
	Encode_Info		encode;
}ReEncode_Info;

typedef struct
{
	DWORD len;
	char*buf;
}OSD_String;

typedef union
{
	Preview_Info	preview;
	Decode_Info		decode;
	Encode_Info		encode;
	ReEncode_Info   reencode;
	READ_INFO		rbuf;
	WRITE_INFO		wbuf;
	WR_INFO			wrbuf;
	Sensor_Prop		spop;
	Sensor_Set		sset;
} Vdec_Info;


#define IOCTL_CAMERA_BASE 			0x200
#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0

#define	IOCTL_PREVIEW_START			CTL_CODE(IOCTL_CAMERA_BASE, 0x220, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Preview_Info
#define	IOCTL_PREVIEW_STOP			CTL_CODE(IOCTL_CAMERA_BASE, 0x221, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_ENCODE_START			CTL_CODE(IOCTL_CAMERA_BASE, 0x222, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_ENCODE_STOP			CTL_CODE(IOCTL_CAMERA_BASE, 0x223, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_DECODE_START			CTL_CODE(IOCTL_CAMERA_BASE, 0x224, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Decode_Info
#define	IOCTL_DECODE_STOP			CTL_CODE(IOCTL_CAMERA_BASE, 0x225, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_READ_BUF				CTL_CODE(IOCTL_CAMERA_BASE, 0x226, METHOD_BUFFERED, FILE_ANY_ACCESS)	// READ_INFO
#define	IOCTL_WRITE_BUF				CTL_CODE(IOCTL_CAMERA_BASE, 0x227, METHOD_BUFFERED, FILE_ANY_ACCESS)	// WRITE_INFO
#define	IOCTL_SIF_GETCAP			CTL_CODE(IOCTL_CAMERA_BASE, 0x228, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Sensor_Prop
#define	IOCTL_SIF_CONTROL			CTL_CODE(IOCTL_CAMERA_BASE, 0x229, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Sensor_Set
#define	IOCTL_PREVIEW_SET			CTL_CODE(IOCTL_CAMERA_BASE, 0x22a, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Preview_Info
#define	IOCTL_ENCODE_SET			CTL_CODE(IOCTL_CAMERA_BASE, 0x22b, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_DECODE_SET			CTL_CODE(IOCTL_CAMERA_BASE, 0x22c, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Decode_Info
#define	IOCTL_ENCODE_OSD			CTL_CODE(IOCTL_CAMERA_BASE, 0x22d, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_ENCODE_FRAME			CTL_CODE(IOCTL_CAMERA_BASE, 0x22e, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define IOCTL_GET_CUSTOM_INFO		CTL_CODE(IOCTL_CAMERA_BASE, 0x22f, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_REENCODE_STOP			CTL_CODE(IOCTL_CAMERA_BASE, 0x230, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_REENCODE_RW			CTL_CODE(IOCTL_CAMERA_BASE, 0x231, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_REENCODE_START		CTL_CODE(IOCTL_CAMERA_BASE, 0x232, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info
#define	IOCTL_REENCODE_READ			CTL_CODE(IOCTL_CAMERA_BASE, 0x233, METHOD_BUFFERED, FILE_ANY_ACCESS)	// Encode_Info

#define	GPIO_A0							0
#define	GPIO_A1							1
#define	GPIO_A2							2
#define	GPIO_A3							3
#define	GPIO_A4							4
#define	GPIO_A5							5
#define	GPIO_A6							6
#define	GPIO_A7							7
#define	GPIO_A8							8
#define	GPIO_A9							9
#define	GPIO_A10						10
#define	GPIO_A11						11
#define	GPIO_A12						12
#define	GPIO_A13						13
#define	GPIO_A14						14
#define	GPIO_A15						15
#define	GPIO_A16						16
#define	GPIO_A17						17
#define	GPIO_A18						18
#define	GPIO_A19						19
#define	GPIO_A20						20
#define	GPIO_A21						21
#define	GPIO_A22						22
#define	GPIO_A23						23
#define	GPIO_A24						24
#define	GPIO_A25						25
#define	GPIO_A26						26
#define	GPIO_A27						27
#define	GPIO_A28						28
#define	GPIO_A29						29
#define	GPIO_A30						30
#define	GPIO_A31						31

#define	GPIO_B0							32
#define	GPIO_B1							33
#define	GPIO_B2							34
#define	GPIO_B3							35
#define	GPIO_B4							36
#define	GPIO_B5							37
#define	GPIO_B6							38
#define	GPIO_B7							39
#define	GPIO_B8							40
#define	GPIO_B9							41
#define	GPIO_B10						42
#define	GPIO_B11						43
#define	GPIO_B12						44
#define	GPIO_B13						45
#define	GPIO_B14						46
#define	GPIO_B15						47
#define	GPIO_B16						48
#define	GPIO_B17						49
#define	GPIO_B18						50
#define	GPIO_B19						51
#define	GPIO_B20						52
#define	GPIO_B21						53
#define	GPIO_B22						54
#define	GPIO_B23						55
#define	GPIO_B24						56
#define	GPIO_B25						57
#define	GPIO_B26						58
#define	GPIO_B27						59
#define	GPIO_B28						60
#define	GPIO_B29						61
#define	GPIO_B30						62
#define	GPIO_B31						63

#define	GPIO_C0							64
#define	GPIO_C1							65
#define	GPIO_C2							66
#define	GPIO_C3							67
#define	GPIO_C4							68
#define	GPIO_C5							69
#define	GPIO_C6							70
#define	GPIO_C7							71
#define	GPIO_C8							72
#define	GPIO_C9							73
#define	GPIO_C10						74
#define	GPIO_C11						75
#define	GPIO_C12						76
#define	GPIO_C13						77
#define	GPIO_C14						78
#define	GPIO_C15						79
#define	GPIO_C16						80
#define	GPIO_C17						81
#define	GPIO_C18						82
#define	GPIO_C19						83
#define	GPIO_C20						84
#define	GPIO_C21						85
#define	GPIO_C22						86
#define	GPIO_C23						87
#define	GPIO_C24						88
#define	GPIO_C25						89
#define	GPIO_C26						90
#define	GPIO_C27						91
#define	GPIO_C28						92
#define	GPIO_C29						93
#define	GPIO_C30						94
#define	GPIO_C31						95


#define	GPIO_D0							96
#define	GPIO_D1							97
#define	GPIO_D2							98
#define	GPIO_D3							99
#define	GPIO_D4							100
#define	GPIO_D5							101
#define	GPIO_D6							102
#define	GPIO_D7							103
#define	GPIO_D8							104
#define	GPIO_D9							105
#define	GPIO_D10						106
#define	GPIO_D11						107
#define	GPIO_D12						108
#define	GPIO_D13						109
#define	GPIO_D14						110
#define	GPIO_D15						111
#define	GPIO_D16						112
#define	GPIO_D17						113
#define	GPIO_D18						114
#define	GPIO_D19						115
#define	GPIO_D20						116
#define	GPIO_D21						117
#define	GPIO_D22						118
#define	GPIO_D23						119
#define	GPIO_D24						120
#define	GPIO_D25						121
#define	GPIO_D26						122
#define	GPIO_D27						123
#define	GPIO_D28						124
#define	GPIO_D29						125
#define	GPIO_D30						126
#define	GPIO_D31						127

#define	GPIO_E0							128
#define	GPIO_E1							129
#define	GPIO_E2							130
#define	GPIO_E3							131
#define	GPIO_E4							132
#define	GPIO_E5							133
#define	GPIO_E6							134
#define	GPIO_E7							135
#define	GPIO_E8							136
#define	GPIO_E9							137
#define	GPIO_E10						138
#define	GPIO_E11						139
#define	GPIO_E12						140
#define	GPIO_E13						141
#define	GPIO_E14						142
#define	GPIO_E15						143
#define	GPIO_E16						144
#define	GPIO_E17						145
#define	GPIO_E18						146
#define	GPIO_E19						147
#define	GPIO_E20						148
#define	GPIO_E21						149
#define	GPIO_E22						150
#define	GPIO_E23						151
#define	GPIO_E24						152
#define	GPIO_E25						153
#define	GPIO_E26						154
#define	GPIO_E27						155
#define	GPIO_E28						156
#define	GPIO_E29						157
#define	GPIO_E30						158
#define	GPIO_E31						159

#define	GPIO_F0							160
#define	GPIO_F1							161
#define	GPIO_F2							162
#define	GPIO_F3							163
#define	GPIO_F4							164
#define	GPIO_F5							165
#define	GPIO_F6							166
#define	GPIO_F7							167
#define	GPIO_F8							168
#define	GPIO_F9							169
#define	GPIO_F10						170
#define	GPIO_F11						171
#define	GPIO_F12						172
#define	GPIO_F13						173
#define	GPIO_F14						174
#define	GPIO_F15						175
#define	GPIO_F16						176
#define	GPIO_F17						177
#define	GPIO_F18						178
#define	GPIO_F19						179
#define	GPIO_F20						180
#define	GPIO_F21						181
#define	GPIO_F22						182
#define	GPIO_F23						183
#define	GPIO_F24						184
#define	GPIO_F25						185
#define	GPIO_F26						186
#define	GPIO_F27						187
#define	GPIO_F28						188
#define	GPIO_F29						189
#define	GPIO_F30						190
#define	GPIO_F31						191

#define	GPIO_G0							192 
#define	GPIO_G1							193 
#define	GPIO_G2							194 
#define	GPIO_G3							195  
#define	GPIO_G4							196 
#define	GPIO_G5							197 
#define	GPIO_G6							198 
#define	GPIO_G7							199 
#define	GPIO_G8							200 
#define	GPIO_G9							201
#define	GPIO_G10						202
#define	GPIO_G11						203 
#define	GPIO_G12						204
#define	GPIO_G13						205 
#define	GPIO_G14						206 
#define	GPIO_G15						207
#define	GPIO_G16						208
#define	GPIO_G17						209
#define	GPIO_G18						210
#define	GPIO_G19						211
#define	GPIO_G20						212 
#define	GPIO_G21						213
#define	GPIO_G22						214 
#define	GPIO_G23						215 
#define	GPIO_G24						216
#define	GPIO_G25						217 
#define	GPIO_G26						218 
#define	GPIO_G27						219
#define	GPIO_G28						220 
#define	GPIO_G29						221 
#define	GPIO_G30						222 
#define	GPIO_G31						223  


#define	GPIO_H0							224
#define	GPIO_H1							225  
#define	GPIO_H2							226  
#define	GPIO_H3							227  
#define	GPIO_H4							228 
#define	GPIO_H5							229  
#define	GPIO_H6							230  
#define	GPIO_H7							231  
#define	GPIO_H8							232  
#define	GPIO_H9							233  
#define	GPIO_H10						234  
#define	GPIO_H11						235  
#define	GPIO_H12						236  
#define	GPIO_H13						237  
#define	GPIO_H14						238  
#define	GPIO_H15						239  
#define	GPIO_H16						240  
#define	GPIO_H17						241  
#define	GPIO_H18						242  
#define	GPIO_H19						243  
#define	GPIO_H20						244  
#define	GPIO_H21						245  
#define	GPIO_H22						246  
#define	GPIO_H23						247  
#define	GPIO_H24						248  
#define	GPIO_H25						249  
#define	GPIO_H26						250  
#define	GPIO_H27						251  
#define	GPIO_H28						252  
#define	GPIO_H29						253  
#define	GPIO_H30						254  
#define	GPIO_H31						255  


#define	GPIO_I0							256  
#define	GPIO_I1							257  
#define	GPIO_I2							258  
#define	GPIO_I3							259  
#define	GPIO_I4							260 
#define	GPIO_I5							261  
#define	GPIO_I6							262  
#define	GPIO_I7							263  
#define	GPIO_I8							264 
#define	GPIO_I9							265  
#define	GPIO_I10						266  
#define	GPIO_I11						267  
#define	GPIO_I12						268  
#define	GPIO_I13						269  
#define	GPIO_I14						270  
#define	GPIO_I15						271  
#define	GPIO_I16						272  
#define	GPIO_I17						273  
#define	GPIO_I18						274  
#define	GPIO_I19						273  
#define	GPIO_I20						276  
#define	GPIO_I21						277  
#define	GPIO_I22						278  
#define	GPIO_I23						279  
#define	GPIO_I24						280  
#define	GPIO_I25						281  
#define	GPIO_I26						282  
#define	GPIO_I27						283  
#define	GPIO_I28						284  
#define	GPIO_I29						285  
#define	GPIO_I30						286  
#define	GPIO_I31						287 

#define	PULL_DISABLE					0
#define	PULL_UP							1
#define	PULL_DOWN						2

