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
#include "storage_include.h"

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
static HANDLE g_hVideoEnc = INVALID_HANDLE_VALUE;

static pthread_mutex_t g_EncLock;
static pthread_mutex_t g_DecLock;

// linux 编码要设置摄像头分辨率
static void st_bicubic(DWORD *cubic_coeff, DWORD ratio, int tapes)
{
	int i, j;
	float step;
	float s[4];
	float c[4];
	float sum;

	if(ratio < 256)
		ratio = 256;

	for(i=0; i<tapes; i++)
	{
		step = (float)i;
		step /= tapes;
		s[0] = (step + 1)*256/ratio;
		s[1] = (step + 0)*256/ratio;
		s[2] = (1 - step)*256/ratio;
		s[3] = (2 - step)*256/ratio;

		sum = 0;
		for(j=0; j<4; j++)
		{
			if(s[j] <= 1)
			{
				c[j] = (float)(1.5*s[j]*s[j]*s[j] - 2.5*s[j]*s[j] + 1);
			}
			else if(s[j] <= 2)
			{
				c[j] = (float)(-0.5*s[j]*s[j]*s[j] + 2.5*s[j]*s[j] - 4*s[j] + 2);
			}
			else
			{
				c[j] = 0;
			}
			sum += c[j];
		}
		if(sum==1)
		{
			cubic_coeff[2*i+0] = (((int)(c[0]*256+0.5))&0xffff) + (((int)(c[1]*256+0.5))<<16);
			cubic_coeff[2*i+1] = (((int)(c[2]*256+0.5))&0xffff) + (((int)(c[3]*256+0.5))<<16);
		}
		else
		{
			cubic_coeff[2*i+0] = (((int)(c[0]/sum*256+0.5))&0xffff) + (((int)(c[1]/sum*256+0.5))<<16);
			cubic_coeff[2*i+1] = (((int)(c[2]/sum*256+0.5))&0xffff) + (((int)(c[3]/sum*256+0.5))<<16);
		}
	}
}

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
	pthread_mutex_lock(&g_DecLock);
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
				#else	// 底下为调好后参数
				DWORD bright = 30;
				DWORD contrast = 50;
				DWORD saturation = 55;
				DWORD hue = 55;
				
				st_AdjustCscparam(bright, contrast, saturation, hue);
				#endif
			}
		}
	}

	pthread_mutex_unlock(&g_DecLock);
}

void VideoDecStop(void)
{
	if(g_hVideoDec > 0)
	{
		pthread_mutex_lock(&g_DecLock);
		ioctl(g_hVideoDec, IOCTL_DECODE_STOP, NULL);
		close(g_hVideoDec);
		g_hVideoDec = -1;
		pthread_mutex_unlock(&g_DecLock);
	}
}

int VideoDecStart(int left, int top, int width, int height)
{
	Vdec_Info decinfo;
	int ret;

	pthread_mutex_lock(&g_DecLock);
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
			pthread_mutex_unlock(&g_DecLock);
			return -1;
		}

		ret = ioctl(g_hVideoDec, IOCTL_DECODE_START, &decinfo);
		if(ret < 0)
		{
			printf("IOCTL_DECODE_START fail\r\n");
			close(g_hVideoDec);
			g_hVideoDec = -1;
			pthread_mutex_unlock(&g_DecLock);
			return	-1;
		}

		g_SetCscParamTick = 5;
	}	
	pthread_mutex_unlock(&g_DecLock);
	return 0;
}

// 需要验证 property是否返回key帧标志
int VideoEncFunc(char* pData, unsigned int len, int* property)
{		
	if(g_hVideoEnc != INVALID_HANDLE_VALUE)
	{
		Vdec_Info vinfo;
		memset(&vinfo, 0, sizeof(Vdec_Info));
		vinfo.rbuf.dwsize = len;
		vinfo.rbuf.buf = pData;
		vinfo.rbuf.timeout = 1000;
		vinfo.rbuf.property = *property;
		pthread_mutex_lock(&g_EncLock);
		if(ioctl((int)g_hVideoEnc, IOCTL_READ_BUF, &vinfo) < 0)
		{
			//DBGMSG(DPERROR, "VideoEncRead ioctl_read_buf fail\r\n");
			pthread_mutex_unlock(&g_EncLock);
			return 0;
		}
		//DBGMSG(DPINFO, "%s(%d): $$$$$$$$$ enc size %d  property: %d $$$$$$$$$\n", __FILE__, __LINE__, vinfo.rbuf.dwsize, vinfo.rbuf.property);
		*property = vinfo.rbuf.property;
		pthread_mutex_unlock(&g_EncLock);
		return vinfo.rbuf.dwsize;
	}

	return 0;
}

void VideoEncStop(void)
{
	pthread_mutex_lock(&g_EncLock);
	if(g_hVideoEnc != INVALID_HANDLE_VALUE)
	{
		ioctl((int)g_hVideoEnc, IOCTL_ENCODE_STOP, NULL);
		close((int)g_hVideoEnc);
		g_hVideoEnc = INVALID_HANDLE_VALUE;
	}
	pthread_mutex_unlock(&g_EncLock);
}

int VideoEncStart(unsigned int width, unsigned int height,
	unsigned int framerate, unsigned int streamrate, unsigned int gop, unsigned int qu)
{
	HANDLE hHandle;
	Vdec_Info   videoinfo;
	Encode_Info encinfo;

	if (g_hVideoEnc != INVALID_HANDLE_VALUE)
	{
		printf(" video enc dev has been init !!!\n");
		return 0;
	}

	pthread_mutex_lock(&g_EncLock);
	hHandle = (HANDLE)open(VIDEO_ENDEC_DEV, O_RDWR, 0);
	if(hHandle < 0)
	{
		printf("%s(%d): $$$$$$$$$ open /dev/encdec fail $$$$$$$$$\n", __FILE__, __LINE__);
		pthread_mutex_unlock(&g_EncLock);
		return -1;
	}
	memset(&videoinfo, 0, sizeof(Vdec_Info));

	encinfo.m_enctype	= ENCODE_H264;
	encinfo.m_encw		= width;
	encinfo.m_ench		= height;
	encinfo.m_quality   = qu;
	encinfo.property    = ENCODE_QUALITY;
	encinfo.m_maxkey    = gop;

	// 获取摄像头分辨率
	if(ioctl((int)hHandle, IOCTL_SIF_GETCAP, &videoinfo) < 0)
	{
		printf("%s(%d): $$$$$$$$$ encdec ioctl_sif_getcap $$$$$$$$$\n", __FILE__, __LINE__);
		close((int)hHandle);
		pthread_mutex_unlock(&g_EncLock);
		return  -1;
	}

	printf("%s(%d): $$$$$$$$$ enc H: %d enc W: %d $$$$$$$$$\n", __FILE__, __LINE__, encinfo.m_ench, encinfo.m_encw);

	SIF_SIZE sifSize;
	sifSize.Width   = encinfo.m_encw;
	sifSize.Height  = encinfo.m_ench;
	int nIndex = 0;
	for(nIndex = 0; nIndex < 6; nIndex++)
	{
		printf("%s [%d]: Width:Height [%d, %d]\n", __FUNCTION__, nIndex, videoinfo.spop.OutSize[nIndex].Width, videoinfo.spop.OutSize[nIndex].Height);
		if(encinfo.m_encw == videoinfo.spop.OutSize[nIndex].Width && encinfo.m_ench == videoinfo.spop.OutSize[nIndex].Height)
			break;
	}
	if(nIndex == 6)
	{
		sifSize.Width   = videoinfo.spop.OutSize[0].Width;
		sifSize.Height  = videoinfo.spop.OutSize[0].Height;
	}

	//设置摄像头分辨率
	memset(&videoinfo, 0, sizeof(videoinfo));
	videoinfo.sset.SifProperty = SIF_SETOUTSIZE;
	videoinfo.sset.val = (sifSize.Width << 16) | sifSize.Height;
	if(ioctl((int)hHandle, IOCTL_SIF_CONTROL, &videoinfo) < 0)
	{
		printf("%s(%d): $$$$$$$$$ encdec ioctl_sif_control $$$$$$$$$\n", __FILE__, __LINE__);
		close((int)hHandle);
		pthread_mutex_unlock(&g_EncLock);
		return  -1;
	}

	if(ioctl((int)hHandle, IOCTL_ENCODE_START, &encinfo) < 0)
	{
		printf("%s(%d): $$$$$$$$$ ioctl_encode_start fail $$$$$$$$$\n", __FILE__, __LINE__);
		close((int)hHandle);
		pthread_mutex_unlock(&g_EncLock);
		return	-1;
	}

	//如果编码和摄像头大小不一样，设置编码缩放参数
	if(encinfo.m_encw != sifSize.Width)
	{
		DWORD horRatio = (sifSize.Width * 256 + encinfo.m_encw / 2) / encinfo.m_encw;
		DWORD scaler_coeff[64] = {0};
		st_bicubic(scaler_coeff, horRatio, 16);
		encinfo.property = ENCODE_SET_SCALER;
		encinfo.m_scalerinfo = (DWORD)scaler_coeff;

		if(ioctl((int)hHandle, IOCTL_ENCODE_SET, &encinfo) < 0)
		{
			printf("%s(%d): $$$$$$$$$ encdec ioctl_encode_set fail $$$$$$$$$\n", __FILE__, __LINE__);
			close((int)hHandle);
			pthread_mutex_unlock(&g_EncLock);
			return -1;
		}
	}

	g_hVideoEnc = hHandle;	
	pthread_mutex_unlock(&g_EncLock);
	printf("VideoEncStart success\r\n");
	return 0;
}

void VideoEncSetQuality(unsigned int qu)
{
	Encode_Info encinfo;
	encinfo.m_quality = qu;
	encinfo.property = ENCODE_QUALITY;
	pthread_mutex_lock(&g_EncLock);
	if(ioctl((int)g_hVideoEnc, IOCTL_ENCODE_SET, &encinfo) < 0)
	{
		printf("%s(%d): $$$$$$$$$ ioctl_encode_set fail $$$$$$$$$\n", __FILE__, __LINE__);
	}
	pthread_mutex_unlock(&g_EncLock);
}

void VideoEncEnable(BOOL bOn)
{
	Encode_Info encinfo = {0};
	if(bOn)
		encinfo.property = ENCODE_BLANK_OFF;
	else
		encinfo.property = ENCODE_BLANK_ON;
	encinfo.m_bcolor = 0x808080;

	pthread_mutex_lock(&g_EncLock);
	if(ioctl((int)g_hVideoEnc, IOCTL_ENCODE_SET, &encinfo) < 0)
	{
		printf("%s(%d): $$$$$$$$$ ioctl_encode_set fail $$$$$$$$$\n", __FILE__, __LINE__);
	}
	pthread_mutex_unlock(&g_EncLock);
}

void VideoSetDisplayRect(int left, int top, int width, int height)
{
	pthread_mutex_lock(&g_EncLock);
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
	pthread_mutex_unlock(&g_EncLock);
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

void VideoMutexUnInit(void)
{
	pthread_mutex_destroy(&g_EncLock);
	pthread_mutex_destroy(&g_DecLock);
}
void VideoMutexInit(void)
{
	pthread_mutex_init(&g_EncLock, NULL);
	pthread_mutex_init(&g_DecLock, NULL);
}
