/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_jpeg_enc.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机jpeg 图片编码
  
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

#include "logic_jpeg_enc.h"

#define JPEG_ENC_QUALITY        75 
#define SAVE_YUV422				0


static uint8 *g_backupVideoBuffer = NULL;

static       int       s_ai32RToY[256];
static       int       s_ai32GToY[256];
static       int       s_ai32BToY[256];
static       int       s_ai32RToCb[256];
static       int       s_ai32GToCb[256];
static       int       s_ai32BToCb[256];
static       int       s_ai32RToCr[256];
static       int       s_ai32GToCr[256];
static       int       s_ai32BToCr[256];

/*************************************************
  Function:			InitColorTable
  Description:	 	颜色转换表 
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void InitColorTable(void)
{
    int       i;
    int       nScale = 1L << 16;             //equal to power(2,16)
    int       CBCR_OFFSET = 128 << 16;
    /*
    nHalf is for (y, cb, cr) rounding, equal to (1L<<16)*0.5
    If (R,G,B)=(0,0,1), then Cb = 128.5, should round to 129
    Using these tables will produce 129 too:
    Cb      = (INT32)((RToCb[0] + GToCb[0] + BToCb[1]) >> 16)
            = (INT32)(( 0 + 0 + 1L<<15 + 1L<<15 + 128 * 1L<<16 ) >> 16)
            = (INT32)(( 1L<<16 + 128 * 1L<<16 ) >> 16 )
            = 129
    */
    int  nHalf = nScale >> 1;

    for (i = 0; i < 256; i += 1)
    {
        s_ai32RToY[i] = (int)(0.29900 * nScale + 0.5) * i;
        s_ai32GToY[i] = (int)(0.58700 * nScale + 0.5) * i;
        s_ai32BToY[i] = (int)(0.11400 * nScale + 0.5) * i + nHalf;

        s_ai32RToCb[i] = (int)(0.16874 * nScale + 0.5) * (-i);
        s_ai32GToCb[i] = (int)(0.33126 * nScale + 0.5) * (-i);
        s_ai32BToCb[i] = (int)(0.50000 * nScale + 0.5) * i + CBCR_OFFSET + nHalf - 1;

        s_ai32RToCr[i] = s_ai32BToCb[i];
        s_ai32GToCr[i] = (int)(0.41869 * nScale + 0.5) * (-i);
        s_ai32BToCr[i] = (int)(0.08131 * nScale + 0.5) * (-i);
    }
}

/*************************************************
  Function:			_BGR565ToYCbCr_PACKET
  Description:	 	RGB565转成YUV422_PACKET格式
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void _BGR565ToYCbCr_PACKET(uint16 *pu16SrcBuf, uint16 *pu16DestBuf, uint16 u16Width, uint16 u16Height)
{
#if 0
    uint16    u16X, u16Y, u16SrcBuf;
    uint8      u8R, u8G, u8B;
    uint8      u8Y0, u8U0, u8V0, u8Y1;
	
	uint8 *pu8DestBuf = (uint8 *)pu16DestBuf;

    for ( u16Y = 0; u16Y < u16Height; u16Y += 1 )
    {
        for ( u16X = 0; u16X < u16Width; u16X += 2 )
        {
            u16SrcBuf = pu16SrcBuf[u16Y * (g_JpgEnc_FB.fb_vinfo.xres) + u16X];
            u8B = (uint8)(u16SrcBuf & 0x001F);  // RGB565 B
            u8B = (u8B << 3) | (u8B >> 2);
			
            u16SrcBuf >>= 5;
            u8G = (uint8)(u16SrcBuf & 0x003F);  // G
            u8G = (u8G << 2) | (u8G >> 4);
			
            u16SrcBuf >>= 6;
            u8R = (uint8)(u16SrcBuf & 0x001F);  // R
            u8R = (u8R << 3) | (u8R >> 2);

            u8Y0 = (s_ai32RToY[u8R] + s_ai32GToY[u8G] + s_ai32BToY[u8B]) >> 16;    // -128
            u8U0 = (s_ai32RToCb[u8R] + s_ai32GToCb[u8G] + s_ai32BToCb[u8B]) >> 16;     // -128
            u8V0 = (s_ai32RToCr[u8R] + s_ai32GToCr[u8G] + s_ai32BToCr[u8B]) >> 16;       // -128

            u16SrcBuf = pu16SrcBuf[u16Y * (g_JpgEnc_FB.fb_vinfo.xres) + u16X + 1];
            u8B = (uint8)(u16SrcBuf & 0x001F);
            u8B = (u8B << 3) | (u8B >> 2);
            u16SrcBuf >>= 5;
            u8G = (uint8)(u16SrcBuf & 0x003F);
            u8G = (u8G << 2) | (u8G >> 4);
            u16SrcBuf >>= 6;
            u8R = (uint8)(u16SrcBuf & 0x001F);
            u8R = (u8R << 3) | (u8R >> 2);

            u8Y1 = (s_ai32RToY[u8R] + s_ai32GToY[u8G] + s_ai32BToY[u8B]) >> 16;    // -128

			pu8DestBuf[(u16Y * u16Width + u16X)*2] = u8Y0;
			pu8DestBuf[(u16Y * u16Width + u16X)*2 + 1] = u8U0;
			pu8DestBuf[(u16Y * u16Width + u16X)*2 + 2] = u8Y1;
			pu8DestBuf[(u16Y * u16Width + u16X)*2 + 3] = u8V0;
        }
    }
	return;
#endif
}

/*************************************************
  Function:			_CHANGE_YCbCr_SIZE
  Description:	 	480*272 转 380*272
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void _CHANGE_YCbCr_SIZE(uint8 *pu16SrcBuf, uint8 *pu16DestBuf, uint16 u16Width, uint16 u16Height)
{
#if 0
    uint16    u16X, u16Y;

    for ( u16Y = 0; u16Y < u16Height; u16Y += 1 )
    {
        for ( u16X = 0; u16X < u16Width; u16X += 1 )
        {
            pu16DestBuf[(u16Y*u16Width + u16X)*2] = pu16SrcBuf[(u16Y * (g_JpgEnc_FB.fb_vinfo.xres) + u16X)*2];
			pu16DestBuf[(u16Y*u16Width + u16X)*2 + 1] = pu16SrcBuf[(u16Y * (g_JpgEnc_FB.fb_vinfo.xres) + u16X)*2 + 1];
         }
    }
	return;
#endif
}

#if SAVE_YUV422
/*************************************************
  Function:			Write2File
  Description:	 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int Write2File(char *filename, char *buffer, int len)
{
	int fd;
	unsigned long totalcnt, wlen;
	int ret = 0;
	
	fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC);
	if(fd < 0)
	{
		ret = -1;
		printf("open file fail\n");
		goto out;
	}

	totalcnt = 0;
	while(totalcnt < len)
	{
		wlen = write(fd, buffer + totalcnt, len - totalcnt);
		if(wlen < 0)
		{
			printf("write file error, errno=%d, len=%x\n", errno, wlen);
			ret = -1;
			break;
		}
		totalcnt += wlen;
	}
	log_printf("write file ok !!!!! \n");
	close(fd);
out:	
	return ret;
}
#endif

/*************************************************
  Function:			ms_jpeg_enc_InitFB
  Description:	 	初始化FB
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int32 ms_jpeg_enc_InitFB(void)
{
#if 0
	g_JpgEnc_FB.lcm_fd = open( FB_DEVICE, O_RDWR );
	if (g_JpgEnc_FB.lcm_fd <= 0)
	{
		log_printf( "### Error: cannot open LCM device, returns %d!\n", g_JpgEnc_FB.lcm_fd );
		return 0;
	}

	if (ioctl(g_JpgEnc_FB.lcm_fd, FBIOGET_VSCREENINFO, &(g_JpgEnc_FB.fb_vinfo)))
	{
		log_printf( "ioctl FBIOGET_VSCREENINFO failed!\n" );
		close( g_JpgEnc_FB.lcm_fd );
		return 0;
	}
	g_JpgEnc_FB.u8bytes_per_pixel = g_JpgEnc_FB.fb_vinfo.bits_per_pixel / 8;

	if (ioctl(g_JpgEnc_FB.lcm_fd, FBIOGET_FSCREENINFO, &(g_JpgEnc_FB.fb_finfo)))
	{
		log_printf( "ioctl FBIOGET_FSCREENINFO failed!\n" );
		close( g_JpgEnc_FB.lcm_fd );
        return 0;
    }

	// Map the device to memory
	g_JpgEnc_FB.pLCMBuffer = mmap( NULL, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ), PROT_READ|PROT_WRITE, MAP_SHARED, g_JpgEnc_FB.lcm_fd, 0 );
	if ((int)g_JpgEnc_FB.pLCMBuffer == -1 )
	{
		log_printf( "### Error: failed to map LCM device to memory!\n" );
		return 0;
	}
	else
	{
		log_printf( "### LCM Buffer at:%p, width = %d, height = %d, line_length = %d.\n\n", g_JpgEnc_FB.pLCMBuffer, g_JpgEnc_FB.fb_vinfo.xres, g_JpgEnc_FB.fb_vinfo.yres, g_JpgEnc_FB.fb_finfo.line_length );
	}
	
	g_backupVideoBuffer = (uint8 *)malloc(g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres );
	memcpy(g_backupVideoBuffer, g_JpgEnc_FB.pLCMBuffer, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ));
	return 1;
#endif

}

/*************************************************
  Function:			ms_jpeg_enc_unInitFB
  Description:	 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_jpeg_enc_unInitFB(void)
{
#if 0
	if (g_JpgEnc_FB.lcm_fd > 0) 
	{
		log_printf("\n ******************** ms_jpeg_dec_unInitFB  *****************************  \n");	
		munmap(g_JpgEnc_FB.pLCMBuffer, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ));
		//ioctl(g_JpgEnc_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_YCBYCR); 
		close(g_JpgEnc_FB.lcm_fd);
		g_JpgEnc_FB.lcm_fd = -1;
		free(g_backupVideoBuffer);
		g_backupVideoBuffer = NULL;
	}
#endif
}

/*************************************************
  Function:		ms_jpeg_enc_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_enc_init(struct _MSMediaDesc * f)
{
	if (NULL == f->private)
	{
		JpegEncParam *data = (JpegEncParam*)malloc(sizeof(JpegEncParam));
		memset(data,0,sizeof(JpegEncParam));


		memset(data->mName, 0XFF, sizeof(data->mName));
		#if (_LCD_DPI_ == _LCD_800480_)
		data->vorect.x = 0;
		data->vorect.y = 0;
		data->vorect.width = 640;
		data->vorect.height = 480;
		#elif (_LCD_DPI_ == _LCD_1024600_)
		data->vorect.x = 0;
		data->vorect.y = 0;
		data->vorect.width = 380;
		data->vorect.height = 272;
		#endif
		
		f->private = data;
		f->mcount = 0;
	}
	
	InitColorTable();
	return RT_SUCCESS;
}

/*************************************************
  Function:		ms_jpeg_enc_uninit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_enc_uninit(struct _MSMediaDesc * f)
{
	if (f->mcount == 0)
	{
		ms_free(f->private);
		return RT_SUCCESS;
	}

	return RT_FAILURE;
}

/*************************************************
  Function:		ms_jpeg_dec_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_enc_process(struct _MSMediaDesc * f, void * arg)
{
#if 0
	FILE * outfile;
	char jpeg_filename[100] = {0};
	unsigned char * image_buffer = NULL;
	JpegEncParam * s = (JpegEncParam*)f->private;
	uint16 Snap_PosX, Snap_SosY,image_width , image_height;
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	
	Snap_PosX = s->vorect.x;
	Snap_SosY = s->vorect.y;
	image_width  = s->vorect.width;
	image_height = s->vorect.height;	
	memcpy(jpeg_filename, s->mName, sizeof(s->mName));
	image_buffer = (unsigned char *)malloc( image_width * image_height * 2 );	// hw is 1 pixel 2 bytes

	log_printf("[%d, %d, %d, %d], filename: %s\n", \
		Snap_PosX, Snap_SosY, image_width, image_height, jpeg_filename);

	log_printf("s->FB_Mode : %x \n", s->FB_Mode);

	if (s->FB_Mode == FB_DISPLAY_MODE_YCBYCR)
	{
		uint8 *addr = NULL;
		media_get_v4l_addr(&addr);
		if (NULL == addr)
		{
			printf(" src buf is null !!!!!\n");
			return RT_FAILURE;
		}
		memset(g_backupVideoBuffer, 0x00, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ));
		addr = addr + Snap_SosY*g_JpgEnc_FB.fb_finfo.line_length + Snap_PosX*2;
		memcpy(g_backupVideoBuffer, addr, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ));
		_CHANGE_YCbCr_SIZE(g_backupVideoBuffer, image_buffer, image_width, image_height);
	}
	else
	{
	    uint16 *address = NULL;	

		#if 1	// add by chenbh 直接从H264 DEC抓取一帧数据 解决抓拍横切条问题
		uint8 *psrc = NULL;
		int8 times = 15;
        while (psrc == NULL && times > 0)
        {
        	usleep(100*1000);
        	ms_h264_dec_get_snap_buf(&psrc);
			times--;
        }
		if (NULL == psrc)
		{
			printf(" src buf is null !!!!!\n");
			return RT_FAILURE;
		}
		address = (uint16 *)(psrc + Snap_SosY*g_JpgEnc_FB.fb_finfo.line_length + Snap_PosX*2);
		#else	// 直接从FB取数据做编码
        address = (uint16 *)(g_backupVideoBuffer + Snap_SosY*g_JpgEnc_FB.fb_finfo.line_length + Snap_PosX*2);        
		#endif
        _BGR565ToYCbCr_PACKET(address, (uint16 *)image_buffer, image_width, image_height);
	}
	
	#if SAVE_YUV422
	Write2File("/mnt/nand1-2/SNAP/yuv422packet.bin", image_buffer, (g_JpgEnc_FB.fb_vinfo.xres)*(g_JpgEnc_FB.fb_vinfo.yres)*2);
	#endif

	if ( (outfile = fopen( jpeg_filename, "wb" )) == NULL )
	{
		fprintf( stderr, "can't open %s\n", jpeg_filename );
		return (-1);
	}

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;	
	/* Establish the setjmp return context for my_error_exit to use. 设置跳转节点*/
  	if (setjmp(jerr.setjmp_buffer)) 
	{
	    /* If we get here, the JPEG code has signaled an error.
	     * We need to clean up the JPEG object, close the input file, and return.
	     */
	    log_printf(" return error \n");
		jpeg_destroy_compress( &cinfo );
	    fclose(outfile);
		if (image_buffer != NULL)
			free( (unsigned char *)image_buffer );
	    return RT_FAILURE;
  	}
	
	jpeg_create_compress( &cinfo );


	jpeg_stdio_dest( &cinfo, outfile );

	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;			// hw don't care, because hw just support 2 bytes per pixel
	cinfo.in_color_space = JCS_YCbCr;	// hw just support input YCbYCr

	jpeg_set_defaults( &cinfo );

	jpeg_set_quality( &cinfo, JPEG_ENC_QUALITY, TRUE );
//	jpeg_set_colorspace( &cinfo, JCS_GRAYSCALE );	// hw not support
//	cinfo.comp_info->h_samp_factor = 2;
//	cinfo.comp_info->v_samp_factor = 1;
	cinfo.dct_method = JDCT_IFAST;					// hw not support
//	cinfo.scale_num = 8;							// hw just support encode scaling up
//	cinfo.scale_denom = 4;							// and hw needs denom less than num
//	cinfo.smoothing_factor = 100;					// hw not support
//	cinfo.do_fancy_downsampling = TRUE;				// hw not support

	jpeg_start_compress( &cinfo, TRUE );

	#if 1
	JSAMPROW row_pointer[1];
	int row_stride = image_width * 2;	// hw is 1 pixel 2 bytes

	while ( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	#else
	jpeg_write_scanlines( &cinfo, image_buffer, cinfo.image_height );
	#endif

	jpeg_finish_compress( &cinfo );
	fclose( outfile );
	//sync();  // 该函数比较耗时

	#if 1
	free( (unsigned char *)image_buffer );
	image_buffer = NULL;
	#else
	for ( i = 0; i < image_height; ++i )
		free( (unsigned char *)image_buffer[i] );
	#endif

	jpeg_destroy_compress( &cinfo );
	return RT_SUCCESS;
#endif
}

/*************************************************
  Function:		ms_jpeg_dec_param
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_enc_param(struct _MSMediaDesc * f, void * arg)
{
	JpegEncParam * s = (JpegEncParam*)f->private;
	JpegEncParam * param = (JpegEncParam *)arg;
	ms_return_val_if_fail(param, -1);
	ms_media_lock(f);
	memset(s, 0, sizeof(JpegEncParam));
	memcpy(s->mName, param->mName, sizeof(param->mName));
	s->vorect.x = param->vorect.x;
	s->vorect.y = param->vorect.y;
	s->vorect.width = param->vorect.width;
	s->vorect.height = param->vorect.height;
	s->FB_Mode = param->FB_Mode;
	s->src_mode = param->src_mode;
	log_printf("s->mName : %s;  s->vorect.x :%d, s->vorect.y :%d  s->vorect.width: %d  s->vorect.height: %d s->FB_Mode: %x, s->src_mode: %d\n", \
		s->mName, s->vorect.x, s->vorect.y, s->vorect.width, s->vorect.height, s->FB_Mode, s->src_mode);
	ms_media_unlock(f);	
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
static int ms_jpeg_enc_open(struct _MSMediaDesc * f, void * arg)
{
#if 0
	int ret = RT_FAILURE;

	ms_media_lock(f);
	if (f->mcount == 0)
	{
		if (!ms_jpeg_enc_InitFB())
		{
			goto error;
		}
	}
	
	ret = ms_jpeg_enc_process(f, arg);
	if (ret == RT_SUCCESS)
	{
		f->mcount++;
	}
	else
	{
		goto error1;
	}
	ms_media_unlock(f);
	return RT_SUCCESS;	

error1:
	munmap(g_JpgEnc_FB.pLCMBuffer, (g_JpgEnc_FB.fb_finfo.line_length * g_JpgEnc_FB.fb_vinfo.yres ));
	
error:
	if (g_JpgEnc_FB.lcm_fd > 0) 
	{
		close(g_JpgEnc_FB.lcm_fd);
		g_JpgEnc_FB.lcm_fd = -1;
	}

	ms_media_unlock(f);
	return RT_FAILURE;
#endif
}

/*************************************************
  Function:		ms_jpeg_dec_close
  Description: 	最后关闭才调用，切换图片不用调用
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_jpeg_enc_close(struct _MSMediaDesc * f, void * arg)
{
	ms_media_lock(f);
	if (f->mcount > 0)
	{
		f->mcount = 0;
		ms_jpeg_enc_unInitFB();
	}
	ms_media_unlock(f);
	return RT_SUCCESS;
}


static MSMediaMethod methods[]={
	{MS_JPEG_ENC_PARAM,	ms_jpeg_enc_param},
	{MS_JPEG_ENC_OPEN,	ms_jpeg_enc_open},
	{MS_JPEG_ENC_CLOSE,	ms_jpeg_enc_close},
	{0, NULL}
};

MSMediaDesc ms_jpeg_enc_desc={
	.id = MS_JPEG_ENC_ID,
	.name = "MsJpegEnc",
	.text = "Jpeg Enc",
	.enc_fmt = "jpeg",
	.ninputs = 0,
	.noutputs = 0,
	.outputs = NULL,
	.init = ms_jpeg_enc_init,
	.uninit = ms_jpeg_enc_uninit,
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
	
