#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <fcntl.h>   
#include <string.h>   
#include <linux/fb.h>   
#include <sys/mman.h>   
#include <sys/ioctl.h>   
#include <arpa/inet.h>  
#include "dpdef_linux.h"
#include "dpgpio.h"

#define	VIDEO_ENDEC_DEV		"/dev/encdec"
#define __u32 unsigned int
#define __u8 unsigned char
#define DISP_CMD_SET_COLORKEY 0x04
typedef struct
{
	__u8  alpha;__u8 red;__u8 green; __u8 blue; 
}__disp_color_t;

typedef struct
{
    __disp_color_t   ck_max;
    __disp_color_t   ck_min;
    __u32             red_match_rule;//0/1:always match; 2:match if min<=color<=max; 3:match if color>max or color<min 
    __u32             green_match_rule;//0/1:always match; 2:match if min<=color<=max; 3:match if color>max or color<min 
    __u32             blue_match_rule;//0/1:always match; 2:match if min<=color<=max; 3:match if color>max or color<min 
}__disp_colorkey_t;

typedef struct
{
	union
	{
		Preview_Info	preview;
		Decode_Info		decode;
		Encode_Info		encode;
		READ_INFO		rbuf;
		WRITE_INFO		wbuf;
		Sensor_Prop		spop;
		Sensor_Set		sset;
	} params;
} Video_Info;

static int g_hVideoDec = -1;
static int g_SetCscParamTick = 0;

static BOOL st_AdjustCscparam(DWORD bright, DWORD contrast, DWORD saturation, DWORD hue)
{
	CscParam param;
	param.bright = bright;
	param.contrast = contrast;
	param.saturation = saturation;
	param.hue = hue;
	printf("Set Csc %d %d %d %d\r\n",bright, contrast, saturation, hue);
	int h_backlight = open("/dev/backlight", O_RDWR, 0);
	if(h_backlight < 0)
	{	
		printf("Open DIS1 fail error !!!\r\n");
		return FALSE;
	}

	ioctl(h_backlight,IOCTL_DISP_ADJUST_FE,&param);
	close(h_backlight);
	return TRUE;
}

void VideoDecFunc(char * pdata, unsigned int dlen, unsigned int ptr)
{
	Vdec_Info video_info;
	if(g_hVideoDec > 0)
	{
		memset(&video_info, 0, sizeof(Vdec_Info));
		video_info.wbuf.dwsize = dlen;
		video_info.wbuf.buf = (BYTE*)pdata;
		video_info.wbuf.dec_pts = ptr;
		ioctl(g_hVideoDec,IOCTL_WRITE_BUF, &video_info);

		if(g_SetCscParamTick > 0)
		{
			g_SetCscParamTick--;
			if(g_SetCscParamTick == 0)
			{
				#if 0
				// Linux下，只有解码中设置视频色彩才能起到作用
				// 624	显示参数 0 亮度 1 对比度 2 饱和度 3视频亮度 4视频对比度 5视频饱和度 6视频色彩度
				DWORD dwVal[7]; char value[60];
				memset(value, 0, sizeof(value));
				read_ini(VIDEO_PARAM_PATH, "视频参数", "亮度", value, "32");
				dwVal[3] = atoi(value);

				memset(value, 0, sizeof(value));
				read_ini(VIDEO_PARAM_PATH, "视频参数", "对比度", value, "32");
				dwVal[4] = atoi(value);

				memset(value, 0, sizeof(value));
				read_ini(VIDEO_PARAM_PATH, "视频参数", "饱和度", value, "32");
				dwVal[5] = atoi(value);

				memset(value, 0, sizeof(value));
				read_ini(VIDEO_PARAM_PATH, "视频参数", "色彩度", value, "50");
				dwVal[6] = atoi(value);
											
				st_AdjustCscparam(dwVal[3], dwVal[4], dwVal[5], dwVal[6]);
				#endif
			}
		}
	}
}

void VideoDecStop(void)
{
	if(g_hVideoDec > 0)
	{
		ioctl(g_hVideoDec, IOCTL_DECODE_STOP, NULL);
		close(g_hVideoDec);
		g_hVideoDec = -1;
	}
}

int VideoDecStart(int left, int top, int width, int height)
{
	Vdec_Info decinfo;
	int ret;

	if(g_hVideoDec < 0)
	{
		memset(&decinfo, 0, sizeof(Vdec_Info));

		decinfo.decode.m_winl = left;
		decinfo.decode.m_wint = top;
		decinfo.decode.m_winw = width;
		decinfo.decode.m_winh = height;
		decinfo.decode.m_decw = 640;
		decinfo.decode.m_dech = 480;
		decinfo.decode.m_dectype = DECODE_H264;
		decinfo.decode.m_outtype = DECODE_OUTDIR;

		g_hVideoDec = open(VIDEO_ENDEC_DEV, O_RDWR, 0);
		if(g_hVideoDec < 0)
		{
			printf("open /dev/encdec fail\n");
			return -1;
		}

		ret = ioctl(g_hVideoDec, IOCTL_DECODE_START, &decinfo);
		if(ret < 0)
		{
			printf("IOCTL_DECODE_START fail\r\n");
			close(g_hVideoDec);
			g_hVideoDec = -1;
			return	-1;
		}
	}

	g_SetCscParamTick = 5;
	return 0;
}

void VideoSetDisplayRect(int left, int top, int width, int height)
{
	Video_Info video_info;
	if(g_hVideoDec > 0)
	{
		memset(&video_info, 0, sizeof(Video_Info));
		video_info.params.decode.m_winl = left;
		video_info.params.decode.m_wint = top;
		video_info.params.decode.m_winw = width;
		video_info.params.decode.m_winh = height;
		ioctl(g_hVideoDec,IOCTL_DECODE_SET, &video_info);
	}
}

void VideoSetColorKey(unsigned int color)
{
	int fddis;
	unsigned long buffer[4];

	__disp_colorkey_t color_key;
	memset(&color_key,0,sizeof(color_key));
	color_key.ck_max.red = (color >> 16 )&0xff;
	color_key.ck_max.green = (color >> 8 )&0xff;
	color_key.ck_max.blue = color&0xff;
	
	color_key.ck_min.red = (color >> 16 )&0xff;
	color_key.ck_min.green = (color >> 8 )&0xff;
	color_key.ck_min.blue = color&0xff;
	
	color_key.red_match_rule = 2;
	color_key.green_match_rule = 2;
	color_key.blue_match_rule = 2;

			
	buffer[0] = 0;
	buffer[1] = (unsigned long)&color_key;
	printf("open disp\n");
		fddis = open("/dev/disp", O_RDWR);  
   if (!fddis)  
   {  
	   printf("Error: cannot open /dev/disp device.\n");	
	   exit(1);  
   }  
 	printf("DISP_CMD_SET_COLORKEY\n");
   if (ioctl(fddis, DISP_CMD_SET_COLORKEY, (unsigned long)buffer))  
   {  
	   printf("Error：DISP_CMD_SET_COLORKEY.\n");  
	   exit(2);  
   }  
   
   close(fddis);
}
