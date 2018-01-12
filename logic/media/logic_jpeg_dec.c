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

#include "logic_jpeg_dec.h"

#define SAVE_DEC_BIN        0
#define SHOW_CENTER			1			// 图片居中显示


static void ms_jpeg_dec_unInitFB(void);


/*************************************************
  Function:			ms_h264_dec_InitFB
  Description:	 	初始化FB
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int32 ms_jpeg_dec_InitFB(void)
{
#if 0
	g_JpgDec_FB.lcm_fd = open( FB_DEVICE, O_RDWR );
	if (g_JpgDec_FB.lcm_fd <= 0)
	{
		log_printf( "### Error: cannot open LCM device, returns %d!\n", g_JpgDec_FB.lcm_fd );
		return 0;
	}

	if (ioctl(g_JpgDec_FB.lcm_fd, FBIOGET_VSCREENINFO, &(g_JpgDec_FB.fb_vinfo)))
	{
		log_printf( "ioctl FBIOGET_VSCREENINFO failed!\n" );
		close( g_JpgDec_FB.lcm_fd );
		return 0;
	}
	g_JpgDec_FB.u8bytes_per_pixel = g_JpgDec_FB.fb_vinfo.bits_per_pixel / 8;

	if (ioctl(g_JpgDec_FB.lcm_fd, FBIOGET_FSCREENINFO, &(g_JpgDec_FB.fb_finfo)))
	{
		log_printf( "ioctl FBIOGET_FSCREENINFO failed!\n" );
		close( g_JpgDec_FB.lcm_fd );
        return 0;
    }

	// Map the device to memory
	g_JpgDec_FB.pLCMBuffer = mmap( NULL, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres ), PROT_READ|PROT_WRITE, MAP_SHARED, g_JpgDec_FB.lcm_fd, 0 );
	if ((int)g_JpgDec_FB.pLCMBuffer == -1 )
	{
		log_printf( "### Error: failed to map LCM device to memory!\n" );
		return 0;
	}
	else
	{
		log_printf( "### LCM Buffer at:%p, width = %d, height = %d, line_length = %d.\n\n", g_JpgDec_FB.pLCMBuffer, g_JpgDec_FB.fb_vinfo.xres, g_JpgDec_FB.fb_vinfo.yres, g_JpgDec_FB.fb_finfo.line_length );
	}
	ioctl(g_JpgDec_FB.lcm_fd, VIDEO_FORMAT_CHANGE, DISPLAY_MODE_RGB565);
	memset(g_JpgDec_FB.pLCMBuffer, 0x00, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres ));
	return 1;
#endif
}

/*************************************************
  Function:			ms_jpeg_dec_unInitFB
  Description:	 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void ms_jpeg_dec_unInitFB(void)
{
#if 0
	if (g_JpgDec_FB.lcm_fd > 0) 
	{
		log_printf("\n ******************** ms_jpeg_dec_unInitFB  *****************************  \n");
		ioctl( g_JpgDec_FB.lcm_fd, IOCTL_FB_LOCK, 0);
		memset(g_JpgDec_FB.pLCMBuffer, 0, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres ));
		ioctl( g_JpgDec_FB.lcm_fd, IOCTL_FB_UNLOCK, 0);
		
		munmap(g_JpgDec_FB.pLCMBuffer, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres ));
		close(g_JpgDec_FB.lcm_fd);
		g_JpgDec_FB.lcm_fd = -1;		
	}
#endif
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
#if 0
	JpegDecParam * s = (JpegDecParam*)f->private;
	unsigned char *pStartLCMBuffer = NULL;           // FB 初始地址
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile = NULL;

	if ( (infile = fopen(s->mName, "rb" )) == NULL )
	{
		fprintf( stderr, "can't open %s\n", s->mName );
		return RT_FAILURE;
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
	    jpeg_destroy_decompress(&cinfo);
	    fclose(infile);
	    return RT_FAILURE;
  	}
  
	jpeg_create_decompress( &cinfo );

	#if 1	// FIX ME: file as source
	jpeg_stdio_src( &cinfo, infile );
	#else	// FIX ME: memory as source
	FILE *fpr;
	char * src1;

	fpr = fopen( jpeg_filename, "rb" );
	if ( fpr < 0 )
	{
		log_printf( "Open read file error \n" );
		return;
	}
	fseek( fpr, 0L, SEEK_END );
	int sz2 = ftell( fpr );
	fseek( fpr, 0L, SEEK_SET );
	src1 = malloc( sz2 );
	fread( src1, 1, sz2, fpr );
	fclose( fpr );
	sync();
	jpeg_mem_src( &cinfo, src1, sz2 );
	#endif
	
	jpeg_read_header( &cinfo, TRUE );
	
	int scale_num, scale_denom;
	scale_num = scale_denom = 1;
	//if (enableScaleDown)
	if (1)
	{
		unsigned short u16RealWidth, u16RealHeight, u16RealWidth2, u16RealHeight2;
		(void) jpeg_calc_output_dimensions(&cinfo);
		u16RealWidth2 = u16RealWidth = cinfo.output_width;
		u16RealHeight2 = u16RealHeight = cinfo.output_height;
		log_printf( "\tbefore scale width = %d, before scale height = %d\n", u16RealWidth, u16RealHeight );
		if (u16RealWidth2 <= 0 || u16RealHeight2 <= 0)
		{
			return RT_FAILURE;
		}
		
		int scale_flag;
		float scale_ratio;
		scale_flag = 0;
		scale_num = s->vorect.width;    // scale_num/scale_denom 得到压缩比
		scale_denom = s->vorect.width;

	
_FIT_:
		//if ( (u16RealWidth2 > g_JpgDec_FB.fb_vinfo.xres) || (u16RealHeight2 > g_JpgDec_FB.fb_vinfo.yres) )
		if ((u16RealWidth2 > s->vorect.width) || (u16RealHeight2 > s->vorect.height))
		{
			scale_flag = 1;
			scale_ratio = (float)scale_num / (float)scale_denom;
			u16RealWidth2 = u16RealWidth * scale_ratio;
			u16RealHeight2 = u16RealHeight * scale_ratio;
			log_printf( "\tafter scaled width = %d, after scaled height = %d\n", u16RealWidth2, u16RealHeight2 );
			scale_num -= 2;
			goto _FIT_;
		}
		if ( scale_flag )
		{
			scale_num += 2;
		}
		else
		{
			scale_num = 1;
			scale_denom = 1;
		}
	}

	cinfo.dct_method = JDCT_IFAST;				// don't care, hw not support
//	cinfo.desired_number_of_colors = 256;		// don't care, hw not support
//	cinfo.quantize_colors = TRUE;				// don't care, hw not support
//	cinfo.dither_mode = JDITHER_FS;				// don't care, hw not support
//	cinfo.dither_mode = JDITHER_NONE;			// don't care, hw not support
//	cinfo.dither_mode = JDITHER_ORDERED;		// don't care, hw not support
//	cinfo.two_pass_quantize = FALSE;			// don't care, hw not support
	cinfo.scale_num = scale_num;				// hw just support decode scaling down
	cinfo.scale_denom = scale_denom;			// and hw needs denom great than num
//	cinfo.do_fancy_upsampling = FALSE;			// don't care, hw not support
//	cinfo.do_block_smoothing = TRUE;			// don't care, hw not support

	jpeg_start_decompress( &cinfo );

	log_printf( "%d, %d, %d\n", cinfo.output_width, cinfo.output_height, cinfo.output_components );

	JSAMPARRAY buffer;
	int row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)( (j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1 );

	unsigned char * frame_buffer = NULL;
	unsigned char * _frame_buffer = NULL;
	frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * cinfo.output_components );
	_frame_buffer = frame_buffer;

	while ( cinfo.output_scanline < cinfo.output_height )
	{
		jpeg_read_scanlines( &cinfo, buffer, 1 );
		memcpy( _frame_buffer, buffer[0], row_stride );
		_frame_buffer += row_stride;
	}

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );

	fclose(infile);

	#if SAVE_DEC_BIN
	FILE * outfile;
	if ((outfile = fopen( "t.bin", "wb" )) == NULL)
	{
		fprintf( stderr, "can't open %s\n", s->mName );
		return RT_SUCCESS;
	}
	fwrite( frame_buffer, 1, (cinfo.output_width * cinfo.output_height * cinfo.output_components), outfile );
	fclose( outfile );
	sync();
	#endif
	
	int i, display_width, display_height, display_offset;
	ioctl( g_JpgDec_FB.lcm_fd, IOCTL_FB_LOCK );
	pStartLCMBuffer = g_JpgDec_FB.pLCMBuffer;
	memset( g_JpgDec_FB.pLCMBuffer, 0, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres) );

	// hw decode color format default is RGB565 total 16 bits per pixel
	if ( cinfo.output_components == 2 )
	{
		if ( g_JpgDec_FB.fb_vinfo.bits_per_pixel == 32 )
		{
			unsigned short * pu16Temp;
			unsigned char u8R, u8G, u8B;
			pu16Temp = (unsigned short *)frame_buffer;
			_frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel );
			for ( i = 0; i < cinfo.output_width * cinfo.output_height; ++i )
			{
				u8R = ((pu16Temp[i] & 0xF800) >> 11) << 3;
				u8G = ((pu16Temp[i] & 0x07E0) >> 5) << 2;
				u8B = (pu16Temp[i] & 0x001F) << 3;
				_frame_buffer[i*4] = u8B;
				_frame_buffer[i*4 + 1] = u8G;
				_frame_buffer[i*4 + 2] = u8R;
				_frame_buffer[i*4 + 3] = 0x00;
			}
			free( (unsigned char *)frame_buffer );
			frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel );
			memcpy( frame_buffer, _frame_buffer, (cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel) );
			free( (unsigned char *)_frame_buffer );
		}
	}
	else if ( cinfo.output_components == 3 )
	{
		if ( g_JpgDec_FB.fb_vinfo.bits_per_pixel == 16 )
		{
			for ( i = 0; i < (cinfo.output_width * cinfo.output_height); ++i )
			{
				unsigned short r = frame_buffer[i*3];
				unsigned short g = frame_buffer[i*3 + 1];
				unsigned short b = frame_buffer[i*3 + 2];
				unsigned short rgb = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
				frame_buffer[i*2] = rgb & 0x00FF;
				frame_buffer[i*2+1] = (rgb & 0xFF00) >> 8;
			}
		}
		else
		{
			_frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel );
			for ( i = 0; i < (cinfo.output_width * cinfo.output_height); ++i )
			{
				unsigned short r = frame_buffer[i*3];
				unsigned short g = frame_buffer[i*3 + 1];
				unsigned short b = frame_buffer[i*3 + 2];
				_frame_buffer[i*4] = b;
				_frame_buffer[i*4+1] = g;
				_frame_buffer[i*4+2] = r;
				_frame_buffer[i*4+3] = 0x00;
			}
			free( (unsigned char *)frame_buffer );
			frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel );
			memcpy( frame_buffer, _frame_buffer, (cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel) );
			free( (unsigned char *)_frame_buffer );
		}

	}
	else if ( cinfo.output_components == 4 )
	{
		if ( g_JpgDec_FB.fb_vinfo.bits_per_pixel == 16 )
		{
			for ( i = 0; i < (cinfo.output_width * cinfo.output_height); ++i )
			{
				unsigned short b = frame_buffer[i*4];
				unsigned short g = frame_buffer[i*4 + 1];
				unsigned short r = frame_buffer[i*4 + 2];
				//unsigned short a = frame_buffer[i*4 + 3];
				unsigned short rgb = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
				frame_buffer[i*2] = rgb & 0x00FF;
				frame_buffer[i*2+1] = (rgb & 0xFF00) >> 8;
			}
		}
	}
	else if ( cinfo.output_components == 1 )
	{
		_frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height );
		memcpy( _frame_buffer, frame_buffer, (cinfo.output_width * cinfo.output_height) );
		free( (unsigned char *)frame_buffer );
		frame_buffer = (unsigned char *)malloc( cinfo.output_width * cinfo.output_height * g_JpgDec_FB.u8bytes_per_pixel );
		for ( i = 0; i < (cinfo.output_width * cinfo.output_height); ++i )
		{
			unsigned short y = _frame_buffer[i];
			if ( g_JpgDec_FB.fb_vinfo.bits_per_pixel == 16 )
			{
				unsigned short rgb = ((y >> 3) << 11) | ((y >> 2) << 5) | (y >> 3);
				frame_buffer[i*2] = rgb & 0x00FF;
				frame_buffer[i*2+1] = (rgb & 0xFF00) >> 8;
			}
			else
				frame_buffer[i*4] = frame_buffer[i*4+1] = frame_buffer[i*4+2] = frame_buffer[i*4+3] = y;
		}
		free( (unsigned char *)_frame_buffer );
	}
	
	_frame_buffer = frame_buffer;
	display_offset = 0;		
	
	if (cinfo.output_width > s->vorect.width)
	{
		display_offset = 0;		
		display_width = (s->vorect.width) * (g_JpgDec_FB.u8bytes_per_pixel);
	}
	else
	{
		//display_offset = (cinfo.output_width - s->vorect.width) * g_JpgDec_FB.u8bytes_per_pixel;
		display_offset = ((s->vorect.width - cinfo.output_width)/2) * g_JpgDec_FB.u8bytes_per_pixel;
		display_width = cinfo.output_width * g_JpgDec_FB.u8bytes_per_pixel;
	}

	if (cinfo.output_height > s->vorect.height)
	{
		display_height = s->vorect.height;
	}
	else
	{
		display_height = cinfo.output_height;
		display_offset += ((s->vorect.height - cinfo.output_height)/2) * (g_JpgDec_FB.fb_vinfo.xres * g_JpgDec_FB.u8bytes_per_pixel);
	}

	log_printf("display_offset: %d\n", display_offset);
	g_JpgDec_FB.pLCMBuffer += (s->vorect.y)*(g_JpgDec_FB.fb_vinfo.xres * g_JpgDec_FB.u8bytes_per_pixel); 
	g_JpgDec_FB.pLCMBuffer += (s->vorect.x)*(g_JpgDec_FB.u8bytes_per_pixel);
	g_JpgDec_FB.pLCMBuffer += display_offset;
	
	for ( i = 0; i < display_height; ++i )
	{
		memcpy( g_JpgDec_FB.pLCMBuffer, _frame_buffer, display_width );
		g_JpgDec_FB.pLCMBuffer += (g_JpgDec_FB.fb_vinfo.xres * g_JpgDec_FB.u8bytes_per_pixel);
		_frame_buffer += cinfo.output_width * g_JpgDec_FB.u8bytes_per_pixel;
	}
	ioctl( g_JpgDec_FB.lcm_fd, IOCTL_FB_UNLOCK, 0);
	free( (unsigned char *)frame_buffer );
	frame_buffer = NULL;
	g_JpgDec_FB.pLCMBuffer = pStartLCMBuffer;
#endif
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
	JpegDecParam * s = (JpegDecParam*)f->private;
	JpegDecParam * param = (JpegDecParam *)arg;
	ms_return_val_if_fail(param, -1);
	
	ms_media_lock(f);
	memset(s, 0, sizeof(JpegDecParam));
	memcpy(s->mName, param->mName, sizeof(param->mName));
	s->vorect.x = param->vorect.x;
	s->vorect.y = param->vorect.y;
	s->vorect.width = param->vorect.width;
	s->vorect.height = param->vorect.height;	
	log_printf("s->mName : %s;  s->vorect.x :%d, s->vorect.y :%d  s->vorect.width: %d  s->vorect.height: %d \n", \
		s->mName, s->vorect.x, s->vorect.y, s->vorect.width, s->vorect.height);
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
static int ms_jpeg_dec_open(struct _MSMediaDesc * f, void * arg)
{
	int ret = RT_FAILURE;
#if 0
	ms_media_lock(f);
	ms_jpeg_dec_unInitFB();
	if (!ms_jpeg_dec_InitFB())
	{
		goto error;
	}
	
	ret = ms_jpeg_dec_process(f, arg);
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
	munmap(g_JpgDec_FB.pLCMBuffer, (g_JpgDec_FB.fb_finfo.line_length * g_JpgDec_FB.fb_vinfo.yres ));
	
error:
	if (g_JpgDec_FB.lcm_fd > 0) 
	{
		close(g_JpgDec_FB.lcm_fd);
		g_JpgDec_FB.lcm_fd = -1;
	}

	ms_media_unlock(f);
#endif
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
		ms_jpeg_dec_unInitFB();
	}
	ms_media_unlock(f);
	return RT_SUCCESS;
}

static MSMediaMethod methods[]={
	{MS_JPEG_DEC_PARAM,	ms_jpeg_dec_param},
	{MS_JPEG_DEC_OPEN,	ms_jpeg_dec_open},
	{MS_JPEG_DEC_CLOSE,	ms_jpeg_dec_close},
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

