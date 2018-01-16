/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_h264_enc.c
  Author:		chenbh
  Version:		V2.0
  Date: 		2015-06-06
  Description:	视频编码程序
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#include <sys/mman.h>
#include "logic_video.h"
#include "logic_media.h"
#include "ratecontrol.h"
#include "logic_h264_enc.h"

#define DROP_GOP_COUNT				1		// 丢弃前面一个gop 模拟摄像头刚开启视频不稳定
static int g_CloudKeyCount = 0;

#define _VIDEO_ENC_FILE_SAVE_		0

#if _VIDEO_ENC_FILE_SAVE_		
#define 	H264_FILE				CFG_PUBLIC_DRIVE"/enc.h264"
#define 	CLOUD_H264_FILE			CFG_PUBLIC_DRIVE"/cloudenc.h264"

static FILE * encH264FIle = NULL;
static FILE * encCloudH264FIle = NULL;

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
		if (encH264FIle != NULL)
		{
			fclose(encH264FIle);
			encH264FIle = NULL;
		}
	}
	else
	{
		if (encCloudH264FIle != NULL)
		{
			fclose(encCloudH264FIle);
			encCloudH264FIle = NULL;
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
		encH264FIle = fopen(H264_FILE, "wb");
	}
	else
	{
		h264_file_close(1);
		encCloudH264FIle = fopen(CLOUD_H264_FILE, "wb");
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
		if (encH264FIle != NULL)
		{
			//fwrite(data, len, 1, encH264FIle);
			fwrite(data, 1, len, encH264FIle);
			fflush(encH264FIle);
		}	
	}
	else
	{
		if (encCloudH264FIle != NULL)
		{
			fwrite(data, len, 1, encCloudH264FIle);
			fflush(encCloudH264FIle);
		}	
	}
}
#endif

/*************************************************
  Function:		check_nalHead
  Description:	
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
static int check_nalHead(char *data)
{
	int pos =0;
	
	if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 1)
	{
		pos =4;
	}
	else if (data[0] == 0 && data[1] == 0 && data[2] == 1)
	{
		pos =3;
	}
	else 
	{
		pos =0;
	}
	return pos;
}

/*************************************************
  Function:		vframe_filter
  Description: 	根据帧率过滤帧
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int vframe_filter(H264EncState *vip)
{
	int jg = 0, sy = 0, jg1 = 0;
	static int nSyCount = 0;
	static int nRealCount = 0;
	int frameMax = vip->frameCntMax;
	
	if ( vip == 0 )
	{
		printf("frame_filter: vip is 0.\n");
		return -1;
	}

	if (vip->enc_param.framerate >= vip->frameCntMax )
	{
		return 0;
	}

	jg = frameMax/(vip->enc_param.framerate);
	if ( jg != 0 )
	{
		sy = frameMax-(vip->enc_param.framerate)-(frameMax/jg)*(jg-1);
	}
	if ( jg != 0 && sy != 0 )
	{
		jg1 = (frameMax/jg)/sy;
	}

	if (vip->frameCount == 0 )
	{
		nSyCount = 0;
		nRealCount = 0;
	}

	if ((vip->frameCount%frameMax)!=0)
	{		
		if (vip->frameCount%jg !=0) 
		{
			vip->frameCount++;
			return (-1);
		}
		else
		{
			if (sy>0)
			{
				if ( vip->frameCount%jg1 ==0)
				{
					if (nSyCount++<sy)
					{
						vip->frameCount++;
						return (-1);
					}					
				}
			}
		}	 
	}
	else
	{
		nSyCount =0;
	}
	nRealCount++;

	vip->frameCount++;
	return 0;
}

/*************************************************
  Function:		video_send_data
  Description:	
  Input:			
  Output:		
  Return:		
  Others:
*************************************************/
static int video_send_data(unsigned char*data, int size, int keyflg)
{
	mblk_t arg;
	ms_media_blk_init(&arg);
	arg.address = (char *)data;
	arg.len = size;
	arg.marker = keyflg;
	arg.id = MS_RTP_SEND_V_ID;	// 该项记得不能改 否则rtp 发送头字节判断会出错
	#if _VIDEO_ENC_FILE_SAVE_
	h264_file_save(data, size, 0);
	#endif
	ms_media_process(&ms_h264_enc_desc, &arg);
	//ms_media_process_by_id(mMediaStream.VideoEnc, &arg);
}

/*************************************************
  Function:		video_send_data
  Description:	
  Input:			
  Output:		
  Return:		
  Others:
*************************************************/
static int video_cloud_send_data(unsigned char*data, int size, int keyflg)
{	
	// add by chenbh 20171016 丢弃前面视频不稳定	
	if (keyflg)
	{
		g_CloudKeyCount++;
	}
	
	if (g_CloudKeyCount > DROP_GOP_COUNT)
	{
		mblk_t arg;
		ms_media_blk_init(&arg);
		arg.address = (char *)data;
		arg.len = size;
		arg.marker = keyflg;
		arg.id = MS_CLOUD_SEND_VIDEO_ID;
		ms_media_process(&ms_cloud_h264_enc_desc, &arg);
		//ms_media_process_by_id(mMediaStream.VideoEnc, &arg);
		g_CloudKeyCount = DROP_GOP_COUNT+1;

		#if _VIDEO_ENC_FILE_SAVE_
		h264_file_save(data, size, 1);
		#endif	
	}	
}

/*************************************************
  Function:		video_preview_param_init
  Description:	
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
static int video_enc_param_reset(H264EncState * data)
{
	ms_return_val_if_fail(data, -1);
	memset(data, 0, sizeof(H264EncState));

	data->favc_enc_fd = 0;
	data->frameCount = 0;
	data->frameCntMax = 50;
	data->force_Iframe = 0;
	data->enc_param.bit_rate  = DEFAULT_BIT_RATE;
	data->enc_param.framerate = DEFAULT_FRAMERATE;
	data->enc_param.frame_rate_base = 1;
	data->enc_param.width = DEFAULT_WIDTH;
	data->enc_param.height = DEFAULT_HEIGHT;
	data->enc_param.qmax = 30;
	data->enc_param.qmin = 10;
	data->enc_param.quant = 24;
	data->enc_param.gop_size = 15;
	
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_h264_enc_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_enc_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
	H264EncState * data = (H264EncState*)f->private;

	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		if (MS_H264_ENC_ID == f->id)
		{
			//ret = rk_media_start_video_encode(1, (void *)video_send_data);
		}
		else
		{
			g_CloudKeyCount = 0;
			//ret = rk_media_start_video_encode(2, (void *)video_cloud_send_data);
		}
		
		if (ret == -1)
		{
			goto error0;
		}
		f->mcount++;

		#if _VIDEO_ENC_FILE_SAVE_
		if (MS_H264_ENC_ID == f->id)
			h264_file_open(0);
		else
			h264_file_open(1);
		#endif
	}
	else
	{
		f->mcount++;
	}
	ms_media_unlock(f);
	return ret;
	
error0:	
	printf(" ms_h264_enc_open return error!!!! \n");
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_h264_enc_close
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_enc_close(struct _MSMediaDesc * f, void * arg)
{
	uint32 ret = RT_FAILURE;
	H264EncState *data = (H264EncState *)f->private;
	log_printf("f->id[%d], f->mcount[%d]\n", f->id, f->mcount);
	
	ms_media_lock(f);
	if (f->mcount > 0)
	{	
		//f->mcount--;
		f->mcount = 0;
		if (f->mcount == 0)
		{
			if (MS_H264_ENC_ID == f->id)
			{
				//rk_media_stop_video_encode(1);
			}
			else
				//rk_media_stop_video_encode(2);
			video_enc_param_reset(data);
			#if _VIDEO_ENC_FILE_SAVE_
			if (MS_H264_ENC_ID == f->id)
				h264_file_close(0);
			else
				h264_file_close(1);
			#endif

			ret = RT_SUCCESS;
		} 
	}
	ms_media_unlock(f);
	
	return ret;
}

/*************************************************
  Function:		ms_h264_enc_idr
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_enc_idr(struct _MSMediaDesc * f, void * arg)
{
	H264EncState * s = (H264EncState*)f->private;
	if (f->mcount > 0)
	{
 		s->force_Iframe = 1;
		if (MS_H264_ENC_ID == f->id)
		{
			//rk_media_gen_key_frame(1);
		}
		else
		{
			//rk_media_gen_key_frame(2);
		}
		
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}

/*************************************************
  Function:		ms_h264_enc_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_h264_enc_param(struct _MSMediaDesc * f, void * arg)
{
	ms_return_val_if_fail(arg, -1);
	
	int ret = RT_FAILURE;
	H264EncState * data = (H264EncState*)f->private;
	video_params *param = (video_params *)arg;

	ms_media_lock(f);
	if (f->mcount == 0)
	{		
		//memcpy(&data->enc_param, param, sizeof(video_params));
		data->enc_param.bit_rate  = param->bit_rate;
		data->enc_param.framerate = param->framerate;
		data->enc_param.width = param->width;
		data->enc_param.height = param->height;
		log_printf("data->enc_param.bit_rate : %d\n", data->enc_param.bit_rate);
		log_printf("data->enc_param.framerate : %d\n", data->enc_param.framerate);
		log_printf("data->enc_param.width : %d\n", data->enc_param.width);
		log_printf("data->enc_param.height : %d\n", data->enc_param.height);

		if (MS_H264_ENC_ID == f->id)
		{
		/*
			rk_media_set_video_param(1, data->enc_param.width, data->enc_param.height,	
				data->enc_param.framerate, data->enc_param.bit_rate);
				*/
		}
		else //if (MS_CLOUD_H264_ENC_ID == f->id)
		{
		/*
			rk_media_set_video_param(2, data->enc_param.width, data->enc_param.height,	
				data->enc_param.framerate, data->enc_param.bit_rate);
				*/
		}

		ret = RT_SUCCESS;
	}
	
	ms_media_unlock(f);	
	return ret;
}

/*************************************************
  Function:		ms_enc_h264_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_enc_h264_init(struct _MSMediaDesc * f)
{
	ms_return_val_if_fail(f, -1);
	
	if (NULL == f->private)
	{		
		H264EncState * data = (H264EncState*)malloc(sizeof(H264EncState));	
		video_enc_param_reset(data);
		f->private= data;		
		f->mcount = 0;
	}
	
	return 0;
}

/*************************************************
  Function:		ms_enc_h264_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_enc_h264_uninit(struct _MSMediaDesc * f)
{
	ms_return_val_if_fail(f, -1);
	
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return 0;
	}
	
	return -1;
}

static MSMediaMethod methods[] =
{
	{MS_H264_ENC_OPEN,		ms_h264_enc_open},
	{MS_H264_ENC_CLOSE,		ms_h264_enc_close},
	{MS_H264_ENC_IDR,		ms_h264_enc_idr},
	{MS_H264_ENC_PARAM,		ms_h264_enc_param},
	{0,						NULL}
};

MSMediaDesc ms_h264_enc_desc =
{
	.id = MS_H264_ENC_ID,
	.name = "MsH264Enc",
	.text = "Encoder using RV1108 Hw",
	.enc_fmt = "H264",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_enc_h264_init,
	.uninit = ms_enc_h264_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};

static MSMediaMethod cloud_methods[] =
{
	{MS_CLOUD_H264_ENC_OPEN,		ms_h264_enc_open},
	{MS_CLOUD_H264_ENC_CLOSE,		ms_h264_enc_close},
	{MS_CLOUD_H264_ENC_IDR,			ms_h264_enc_idr},
	{MS_CLOUD_H264_ENC_PARAM,		ms_h264_enc_param},
	{0,						NULL}
};

MSMediaDesc ms_cloud_h264_enc_desc =
{
	.id = MS_CLOUD_H264_ENC_ID,
	.name = "MsCloudH264Enc",
	.text = "Encoder using RV1108 Hw",
	.enc_fmt = "H264",
	.noutputs = 1,
	.outputs = NULL,
	.init = ms_enc_h264_init,
	.uninit = ms_enc_h264_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = cloud_methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_BLOCK,
	.sharebufsize = 0,			// 1024*sharebufsize
	.sharebufblk = 0,
	.private = NULL, 
};
