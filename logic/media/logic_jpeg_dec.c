/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_jpeg_dec.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机JPEG 图片解码
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/soundcard.h>
#include <sys/poll.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>

#include "jpeg_endec.h"
#include "logic_jpeg_dec.h"


static 	int g_ShowSync;				// TRUE 单buffer直接预览显示  FALSE 需要先填充到缓存buffer然后再显示
static Surface* g_SurFace;


/*************************************************
  Function:		ms_jpeg_dec_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_process(struct _MSMediaDesc * f, void * arg)
{
	ms_return_val_if_fail(arg, -1);
	JpegDecParam *data = (JpegDecParam *)arg;

	JpegDec *dec;
	unsigned int x = data->vorect.x;
	unsigned int y = data->vorect.y;
	unsigned int cx = data->vorect.width;
	unsigned int cy = data->vorect.height;
	log_printf("show rect[%d,%d,%d,%d]\n", x,y,cx,cy);
	
    dec = JpegDecInit(cx, cy);
    if (dec == NULL)
	{
		err_printf(" JpegDecInit return err !!!\n");
        return RT_FAILURE;
    }

    int ret = JpegDecExecute(dec, data->mName);
	if (ret == 0)
	{
		err_printf(" JpegDecExecute return err !!!\n");
		return RT_FAILURE;
	}

    int *ptr = 0, *ptrdst = 0, *viraddr = 0;
	ptr = (g_ShowSync == TRUE) ? (int*)g_SurFace->FbMem : (int*)g_SurFace->FbMem2;
    ptr += x + g_SurFace->FbWidth * y;
    viraddr = (int*)dec->Block->VirAddr;

    int i, j;
    for (j = 0; j < cy; j++) {
        ptrdst = ptr + j * g_SurFace->FbWidth;
        for (i = 0; i < cx; i++) {
            *ptrdst++ = *viraddr++;
        }
    }

	JpegDecDeinit(dec);
	return RT_SUCCESS;
}



/*************************************************
  Function:		ms_jpeg_dec_open
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_open(struct _MSMediaDesc * f, void * arg)
{
	ms_return_val_if_fail(arg, -1);
	
	int ret = RT_FAILURE;
	ms_media_lock(f);

	if (f->mcount == 0)
	{
    	g_SurFace = CreateFB();
	}

	if (g_SurFace != NULL)
	{
		ret = ms_jpeg_dec_process(f, arg);
		if (ret == RT_SUCCESS)
		{
			f->mcount++;
		}
		else
		{
			goto error;
		}
	}

	ms_media_unlock(f);
	return ret;	
	
error:
	if (TRUE == g_ShowSync && g_SurFace)
	{
		DestoryFB(g_SurFace);
	}
	ms_media_unlock(f);
	return RT_FAILURE;
}

/*************************************************
  Function:		ms_jpeg_dec_close
  Description: 	最后关闭才调用，切换图片不用调用
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_close(struct _MSMediaDesc * f, void * arg)
{
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount = 0;		
		DestoryFB(g_SurFace);
		g_ShowSync = TRUE;
	}
	ms_media_unlock(f);
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_jpeg_dec_show
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:		显示解码的图片 一般多张图片显示时用到
*************************************************/
static int ms_jpeg_dec_show(struct _MSMediaDesc * f, void * arg)
{
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		if (FALSE == g_ShowSync)
		{
			ShowFB(g_SurFace);
		}
	}
	ms_media_unlock(f);	
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_jpeg_dec_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_param(struct _MSMediaDesc * f, void * arg)
{
	ms_return_val_if_fail(arg, -1);
	
	ms_media_lock(f);
	if (f->mcount == 0)
	{
		g_ShowSync = *(int *)arg;
		log_printf("g_ShowSync[%d]\n", g_ShowSync);
	}
	ms_media_unlock(f);	
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_jpeg_dec_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		JpegDecParam *data = (JpegDecParam*)malloc(sizeof(JpegDecParam));
		memset(data,0,sizeof(JpegDecParam));


		memset(data->mName, 0XFF, sizeof(data->mName));
		data->vorect.x = 0;
		data->vorect.y = 0;
		data->vorect.width = 640;
		data->vorect.height = 480;
		g_ShowSync = TRUE;
		
		f->private = data;
		f->mcount = 0;
	}

	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_jpeg_dec_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_dec_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}

static MSMediaMethod methods[]={
	{MS_JPEG_DEC_PARAM,	ms_jpeg_dec_param},
	{MS_JPEG_DEC_OPEN,	ms_jpeg_dec_open},
	{MS_JPEG_DEC_CLOSE,	ms_jpeg_dec_close},
	{MS_JPEG_DEC_SHOW,  ms_jpeg_dec_show},
	{0, NULL}
};

MSMediaDesc ms_jpeg_dec_desc={
	.id = MS_JPEG_DEC_ID,
	.name = "MsJpegDec",
	.text = "Jpeg Dec",
	.enc_fmt = "jpeg",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_jpeg_dec_init,
	.uninit = ms_jpeg_dec_uninit,
	.preprocess = NULL,
	.process = NULL,
	.postprocess = NULL,
	.methods = methods,
	.mcount = 0,
	.porcessmode = PROCESS_WORK_NONE,
	.sharebufsize = 512,				// 1024*sharebufsize
	.sharebufblk = 2,
	.private = NULL, 
};	

