/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_h264_dec.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机视频解码
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include <sys/mman.h>
#include "logic_video.h"
#include "logic_h264_dec.h"
#include "logic_media.h"
#include "dpdef_linux.h"
#include "dpgpio.h"
#include <linux/fb.h>

#define FAVC_DECODER_DEV		"/dev/w55fa92_264dec"
#define VPE_DEVICE				"/dev/vpe"
#define _USE_FBIOPAN_

//#define DECODE_OUTPUT_PACKET_YUV422		0
#define MAX_IMG_WIDTH			1280
#define MAX_IMG_HEIGHT			720

typedef struct H264DecState
{
	uint32 u32OutputFmt;		// 0: OUTPUT_FMT_YUV420  1: OUTPUT_FMT_YUV422
	PAYLOAD_TYPE_E enType;	
	video_params   video_param;
	V_RECT_S vorect;
}H264DecState;

static struct fb_var_screeninfo g_fb_var;
uint32 	g_u32VpostWidth, g_u32VpostHeight, fb_bpp;
int32   decoded_img_width, decoded_img_height;
uint32 	FB_PAddress;
int32   dec_mmap_addr;
int32 	bitstreamsize;


int vpe_fd;
int fb_fd;
int favc_dec_fd;

unsigned int g_u32VpostBufMapSize = 0;				/* For munmap use */
void * FB_VAddress;
static int32 g_toggle_flag = 0;
static int32 g_i32LastWidth;
static int32 g_i32LastHeight;
static uint8 g_get_Iframe = FALSE;

static int g_hVideoDec = -1;
static DWORD g_SetCscParamTick;

#define _VIDEO_DEC_FILE_SAVE_ 0
#if _VIDEO_DEC_FILE_SAVE_
#define 	H264_FILE			CFG_PUBLIC_DRIVE"/enc.h264"
#define 	YUV_FILE			CFG_PUBLIC_DRIVE"/vi.yuv"

static FILE * decH264FIle = NULL;
static FILE * YUVFIle = NULL;

/*************************************************
  Function:		h264_file_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void h264_file_close(int mode)
{
	if (mode == 0)
	{
		if (decH264FIle != NULL)
		{
			fclose(decH264FIle);
			decH264FIle = NULL;
		}
	}
	else
	{
		if (YUVFIle != NULL)
		{
			fclose(YUVFIle);
			YUVFIle = NULL;
		}
	}
}

/*************************************************
  Function:		h264_file_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void h264_file_open(int mode)
{
	if (mode == 0)
	{
		h264_file_close(0);
		decH264FIle = fopen(H264_FILE, "wb");
	}
	else
	{
		h264_file_close(1);
		YUVFIle = fopen(YUV_FILE, "wb");
	}
}

/*************************************************
  Function:		g711_file_save
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void h264_file_save(uint8 *data, int len, int mode)
{
	if (mode == 0)
	{
		if (decH264FIle != NULL)
		{
			//fwrite(data, len, 1, decH264FIle);
			fwrite(data, 1, len, decH264FIle);
			fflush(decH264FIle);
		}	
	}
	else
	{
		if (YUVFIle != NULL)
		{
			fwrite(data, len, 1, YUVFIle);
			fflush(YUVFIle);
		}	
	}
}
#endif

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

static void st_H264DecStop(void)
{
	if(g_hVideoDec > 0)
	{
		ioctl(g_hVideoDec, IOCTL_DECODE_STOP, NULL);
		close(g_hVideoDec);
		g_hVideoDec = -1;
	}
	#if _VIDEO_DEC_FILE_SAVE_
	h264_file_close(0);
	#endif
}

static int st_H264DecStart(int left, int top, int width, int height)
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
		decinfo.decode.m_decw = 640;//720;
		decinfo.decode.m_dech = 480;//576;
		decinfo.decode.m_dectype = DECODE_H264;
		decinfo.decode.m_outtype = DECODE_OUTDIR;

		g_hVideoDec = open("/dev/encdec", O_RDWR, 0);
		if(g_hVideoDec < 0)
		{
			printf("open /dev/encdec fail\n");
			return RT_FAILURE;
		}

		ret = ioctl(g_hVideoDec, IOCTL_DECODE_START, &decinfo);
		if(ret < 0)
		{
			printf("IOCTL_DECODE_START fail\r\n");
			close(g_hVideoDec);
			g_hVideoDec = -1;
			return	RT_FAILURE;
		}
	}

	g_SetCscParamTick = 5;
	#if _VIDEO_DEC_FILE_SAVE_
	h264_file_open(0);
	#endif
	return RT_SUCCESS;
}

static void st_H264WriteData(char * pdata,DWORD dlen, DWORD ptr)
{
	Vdec_Info video_info;
	if(g_hVideoDec > 0)
	{
		memset(&video_info, 0, sizeof(Vdec_Info));
		video_info.wbuf.dwsize = dlen;
		video_info.wbuf.buf = (BYTE*)pdata;
		video_info.wbuf.dec_pts = ptr;
		video_info.wbuf.timeout = 50;
		ioctl(g_hVideoDec,IOCTL_WRITE_BUF, &video_info);

		if(g_SetCscParamTick > 0)
		{
			g_SetCscParamTick--;
			if(g_SetCscParamTick == 0)
			{
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
			}
		}
	}
}

/*************************************************
  Function:		ms_rtp_send_process
  Description: 	解码执行函数
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_h264_dec_process(struct _MSMediaDesc * f, mblk_t * arg)
{
	//log_printf("video[%d]\n", arg->len);
	if (NULL != arg)
	{
		H264DecState *data = (H264DecState*)f->private;
		if (g_get_Iframe == TRUE)
		{			
			st_H264WriteData(arg->address, arg->len, 0);
		}
		else
		{
			uint8 flag = ((uint8)(arg->address[4]) & 0x1F);
			if (flag == 7)
			{
				g_get_Iframe = TRUE;
				st_H264WriteData(arg->address, arg->len, 0);
			}
		}

	#if _VIDEO_DEC_FILE_SAVE_
	h264_file_save(arg->address, arg->len, 0);
	#endif

	}
}

/*************************************************
  Function:		ms_dec_h264_init
  Description: 	解码初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		H264DecState *data = (H264DecState*)malloc(sizeof(H264DecState));
		memset(data, 0, sizeof(H264DecState));
		data->enType = PT_H264;
		//data->u32OutputFmt = OUTPUT_FMT_YUV420;
		data->video_param.width = 640;
		data->video_param.height = 480;
		data->video_param.targetw = 640;
		data->video_param.targeth = 480;
	
		
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_dec_h264_uninit
  Description: 	解码反初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		f->private = NULL;
		return 0;
	}

	return RT_FAILURE;
}

/*************************************************
  Function:		ms_h264_dec_level
  Description: 	设置视频位置参数
  Input: 
  	1.f			解码描述
  	2.arg		视频位置参数
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_level(struct _MSMediaDesc * f, void * arg)
{
	H264DecState * s = (H264DecState*)f->private;
	V_RECT_S * param = (V_RECT_S *)arg;
	if (arg == NULL)
	{
		return RT_FAILURE;
	}
	ms_media_lock(f);
		
	memcpy(&s->vorect, param, sizeof(V_RECT_S));
	s->video_param.targetw = param->width;
	s->video_param.targeth = param->height;
	log_printf("s->video_param.targetw :%d, s->video_param.targeth :%d\n", s->video_param.targetw, s->video_param.targeth);
	ms_media_unlock(f);	
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_h264_dec_snap
  Description: 	视频抓拍
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_snap(struct _MSMediaDesc * f, void * arg)
{	
	uint32 ret = RT_FAILURE;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		#if 0
		if (g_SnapBuf)
		{
			free(g_SnapBuf);
			g_SnapBuf = NULL;
		}
		g_SnapFlg = 1;
		ret = RT_SUCCESS;
		#endif
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_h264_dec_open
  Description: 	视频解码开启
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_open(struct _MSMediaDesc * f, void * arg)
{
	int32 ret;
	H264DecState * data = (H264DecState*)f->private;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		ret = st_H264DecStart(0, 0, data->video_param.targetw, data->video_param.targeth);
		if (RT_SUCCESS != ret)
		{
			goto error0;
		}
		else
		{
			f->mcount++;
			if (f->preprocess)
			{
				f->preprocess(f);
			} 
		}
	}
	else
	{
		f->mcount++;
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);
	return ret;


error0: 
	log_printf("return error !\n");	    

	ms_media_unlock(f);
	return ret;
}

/*************************************************
  Function:		ms_h264_dec_close
  Description: 	关闭视频解码 
  Input: 	
  	1.f			描述
  	2.arg		参数 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_close(struct _MSMediaDesc * f, void * arg)
{	
	uint32 ret = RT_FAILURE;
	H264DecState *data = (H264DecState*)f->private;
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{					
			if (f->postprocess)
			{
				f->postprocess(f);
			} 
			g_get_Iframe = FALSE;
			st_H264DecStop();
			ret = RT_SUCCESS;
			log_printf(":RT_SUCCESS \n ");
		}
	}
	ms_media_unlock(f);
	
	return ret;
}

static MSMediaMethod methods[] =
{
	{MS_H264_DEC_OPEN,		ms_h264_dec_open},
	{MS_H264_DEC_LEVEL,     ms_h264_dec_level},
	{MS_H264_DEC_CLOSE,		ms_h264_dec_close},
	{MS_H264_DEC_SNAP,		ms_h264_dec_snap},
	{0,						NULL}
};

MSMediaDesc ms_h264_dec_desc =
{
	.id = MS_H264_DEC_ID,
	.name = "MsH264Dec",
	.text = "Decoder using N32926 Hw",
	.enc_fmt = "H264",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_h264_dec_init,
	.uninit = ms_h264_dec_uninit,
	.preprocess = NULL,
	.process = ms_h264_dec_process,
	.postprocess = NULL,	
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_UNBLOCK,
	.sharebufsize = 2000,		// 1024*sharebufsize
	.sharebufblk = 500,
	.private = NULL, 
};

