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
#include "video_endec.h"

typedef struct H264DecParam
{
	V_RECT_S vorect;
}H264DecParam;

static uint8 g_get_Iframe = FALSE;


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
		if (g_get_Iframe == TRUE)
		{			
			VideoDecFunc(arg->address, arg->len, 0);
		}
		else
		{
			uint8 flag = ((uint8)(arg->address[4]) & 0x1F);
			if (flag == 7)
			{
				g_get_Iframe = TRUE;
				VideoDecFunc(arg->address, arg->len, 0);
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
		H264DecParam *data = (H264DecParam*)malloc(sizeof(H264DecParam));
		memset(data, 0, sizeof(H264DecParam));
		data->vorect.x = 0;
		data->vorect.y = 0;
		data->vorect.width = 640;
		data->vorect.height = 480;
		
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
  Function:		ms_h264_dec_param
  Description: 	
  Input: 
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_dec_param(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	H264DecParam * s = (H264DecParam*)f->private;
	V_RECT_S * param = (V_RECT_S *)arg;
	if (arg == NULL)
	{
		return RT_FAILURE;
	}
	ms_media_lock(f);
	if (f->mcount == 0)
	{
		memcpy(&s->vorect, param, sizeof(V_RECT_S));
		log_printf("s->vorect.x[%d], s->vorect.y[%d], s->vorect.width[%d], s->vorect.height[%d]\n", \
			s->vorect.x, s->vorect.y, s->vorect.width, s->vorect.height);	
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);	
	return ret;
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
	int ret = RT_FAILURE;
	H264DecParam * s = (H264DecParam*)f->private;
	V_RECT_S * param = (V_RECT_S *)arg;
	if (arg == NULL)
	{
		return RT_FAILURE;
	}
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		VideoSetDisplayRect(param->x, param->y, param->width, param->height);	
		ret = RT_SUCCESS;
	}
	ms_media_unlock(f);	
	return ret;
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
	H264DecParam * data = (H264DecParam*)f->private;
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		ret = VideoDecStart(data->vorect.x, data->vorect.y, data->vorect.width, data->vorect.height);
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
			#if _VIDEO_DEC_FILE_SAVE_
			h264_file_open(0);
			#endif
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
	H264DecParam *data = (H264DecParam*)f->private;
	
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
			VideoDecStop();
			#if _VIDEO_DEC_FILE_SAVE_
			h264_file_close(0);
			#endif
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
	{MS_H264_DEC_PARAM,     ms_h264_dec_param},
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

