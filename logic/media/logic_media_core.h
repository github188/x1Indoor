/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_media_core.h
  Author:   	陈本惠
  Version:  	2.0
  Date: 		2014-12-11
  Description:  多媒体
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _AU_MEDIA_CORE_H_
#define _AU_MEDIA_CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#define ALIGN_BACK(x, a)		((a) * (((x) / (a))))
#define ms_return_val_if_fail(_expr_,_ret_)\
	if (!(_expr_)) { printf("assert "#_expr_ " failed"); printf("\n"); return (_ret_);}

typedef enum MSMediaId
{
	MS_FILTER_NOT_SET_ID,
	MS_AUDIO_AI_ID,	
	MS_ALAW_SF_ENC_ID,
	MS_ALAW_DEC_ID,
	MS_H264_ENC_ID,
	MS_H264_DEC_ID,
	MS_RTP_SEND_V_ID,	
	MS_RTP_SEND_A_ID,
	MS_RTP_RECV_A_ID,
	MS_RTP_RECV_V_ID,	
	MS_JPEG_DEC_ID,
	MS_JPEG_ENC_ID,
	MS_FILE_PLAYER_ID,
	MS_ALAW_AGC_ID,
    MS_SPEEX_ID,
	MS_MEDIA_MAX_ID,
	MS_ALAW_AO_ID,
	MS_AVI_PLAY_ID,
	MS_AVI_RECORD_ID,
	MS_MP3_PLAY_ID,
	MS_FILE_LYLY_ID,
	MS_RTSP_PLAY_ID
} MSMediaId;

#define MS_MEDIA_METHOD_ID(_id_,_cnt_,_argsize_) \
	(  (((unsigned long)(_id_)) & 0xFFFF)<<16 | (_cnt_<<8) | (_argsize_ & 0xFF ))

#define MS_MEDIA_METHOD(_id_,_count_,_argtype_) \
	MS_MEDIA_METHOD_ID(_id_,_count_,sizeof(_argtype_))

#define MS_MEDIA_METHOD_NO_ARG(_id_,_count_) \
	MS_MEDIA_METHOD_ID(_id_,_count_,0)

#define MS_MEDIA_EVENT(_id_,_count_,_argtype_) \
	MS_MEDIA_METHOD_ID(_id_,_count_,sizeof(_argtype_))

#define MS_MEDIA_EVENT_NO_ARG(_id_,_count_)\
	MS_MEDIA_METHOD_ID(_id_,_count_,0)

#define MS_MEDIA_METHOD_GET_FID(id)	(((id)>>16) & 0xFFFF)
#define MS_MEDIA_METHOD_GET_INDEX(id) ( ((id)>>8) & 0XFF) 

struct _MSMediaDesc;

typedef int (*MSMediaMethodFunc)(struct _MSMediaDesc * f, void * arg);


#define ms_new(type,count)	(type*)malloc(sizeof(type)*(count))

struct _MSMediaMethod
{
	int id;
	MSMediaMethodFunc method;
};

typedef struct _MSMediaMethod MSMediaMethod;

struct _MSList 
{
	struct _MSList * next;
	struct _MSList * prev;
	void * data;
};

typedef struct _MSList MSList;

#define ms_list_next(elem) ((elem)->next)

struct _MSVideoSize
{
	int x;
	int y;
	int w;
	int h;
};

typedef struct _MSVideoSize MSVideoSize;

struct _MSThread
{
	pthread_t thread;
	unsigned int thread_run;
	unsigned int thread_exittimer;
};

typedef struct _MSThread MSThread;

int __ms_thread_quit(MSThread* thread);
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void * arg);
int __ms_thread_init(MSThread* thread, int exittimer);

#define ms_thread_create	__ms_thread_create
#define ms_thread_join		pthread_join
#define ms_thread_exit		pthread_exit
#define ms_thread_quit		__ms_thread_quit
#define ms_thread_init		__ms_thread_init
#define ms_mutex_init		pthread_mutex_init
#define ms_mutex_lock		pthread_mutex_lock
#define ms_mutex_unlock		pthread_mutex_unlock
#define ms_mutex_destroy	pthread_mutex_destroy
#define ms_cond_init		pthread_cond_init
#define ms_cond_signal		pthread_cond_signal
#define ms_cond_broadcast	pthread_cond_broadcast
#define ms_cond_wait		pthread_cond_wait
#define ms_cond_destroy		pthread_cond_destroy

#define ms_media_lock(f)	 ms_mutex_lock(&(f)->lock)
#define ms_media_unlock(f)	 ms_mutex_unlock(&(f)->lock)

#define ms_process_lock(f)	 ms_mutex_lock(&(f)->lockprocess)
#define ms_process_unlock(f) ms_mutex_unlock(&(f)->lockprocess)

#define ms_queue_lock(f)	 ms_mutex_lock(&(f)->lockqueue)
#define ms_queue_unlock(f) 	 ms_mutex_unlock(&(f)->lockqueue)

#define PROCESS_WORK_UNBLOCK	0
#define PROCESS_WORK_BLOCK		1
#define PROCESS_WORK_NONE		2

typedef struct msgb
{
	MSMediaId id;
	unsigned char used;
	unsigned char marker;
	unsigned int delay;
	char *address;
	unsigned int len;
    char *address_t;
    unsigned int len_t;
	unsigned long long ts;
}mblk_t;

typedef struct _queue
{
	unsigned long sharebufsize;
	int q_pop;
	int q_push;
	char* sharebufnow;
	char *sharebuffer;
	mblk_t * q_blk;
}queue_t;

typedef int (*MSMediaFunc)(struct _MSMediaDesc* f);
typedef void (*MSMediaDataFunc)(struct _MSMediaDesc * f, mblk_t * arg);
typedef void (*MSMediaDataFuncsec)(struct _MSMediaDesc * f, mblk_t * arg, mblk_t * argsec);
typedef void (*MSFilterNotifyFunc)(int cmd, int time, int percent);

struct _MSMediaOut
{
	int noutputs;
	int linkinputs;			// 链接目标块索引 默认1 如果有多个输入 需要设置该值
	void* outputs;
};

typedef struct _MSMediaOut MSMediaOut;

struct _MSMediaDesc
{
	MSMediaId id;
	const char * name; 
	const char * text;
	const char * enc_fmt;
	int ninputs;                   //输入的个数 0\1:1个输入  2: 两个输入
	int noutputs;
	MSMediaOut * outputs;
	MSMediaFunc init;
	MSMediaFunc uninit;
	MSMediaFunc preprocess;
	MSMediaDataFunc process;
	MSMediaDataFuncsec processsec;
	MSMediaFunc postprocess;	
	MSMediaMethod * methods;
	int mcount;
	int porcessmode;
	int sharebufsize;				// 1024*sharebufsize
	int sharebufblk;
	void * private;
	void * data;

	MSFilterNotifyFunc notify;
	void* notify_ud;
	queue_t queue;
	queue_t queuesec;
	MSThread thread;
	MSThread threadprocess;
	pthread_mutex_t lock;
	pthread_mutex_t lockprocess;
	pthread_mutex_t lockqueue;
};

typedef struct _MSMediaDesc MSMediaDesc;

extern MSMediaDesc ms_h264_enc_desc;
extern MSMediaDesc ms_h264_dec_desc;
extern MSMediaDesc ms_jpeg_dec_desc;
extern MSMediaDesc ms_jpeg_enc_desc;
extern MSMediaDesc ms_audio_dec_desc;
extern MSMediaDesc ms_rtp_send_v_desc;
extern MSMediaDesc ms_rtp_send_a_desc;
extern MSMediaDesc ms_rtp_recv_a_desc;
extern MSMediaDesc ms_rtp_recv_v_desc;
extern MSMediaDesc ms_jpegwrite_desc;
extern MSMediaDesc ms_file_player_desc;
extern MSMediaDesc ms_alaw_agc_desc;
extern MSMediaDesc ms_echo_cancell_desc;
extern MSMediaDesc ms_audio_ai_desc;
extern MSMediaDesc ms_audio_sf_enc_desc;
extern MSMediaDesc ms_audio_ao_desc;
extern MSMediaDesc ms_avi_play_desc;
extern MSMediaDesc ms_rtsp_play_desc;
extern MSMediaDesc ms_avi_record_desc;
extern MSMediaDesc ms_mp3_play_desc;
extern MSMediaDesc ms_file_lyly_desc;

/*************************************************
  Function:		ms_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_free(void * data);

/*************************************************
  Function:		ms_list_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_new(void *data);

/*************************************************
  Function:		ms_list_append
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_append(MSList *elem, void * data);

/*************************************************
  Function:		ms_list_prepend
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_prepend(MSList *elem, void *data);

/*************************************************
  Function:		ms_list_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_free(MSList *list);

/*************************************************
  Function:		ms_list_remove_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_remove_link(MSList *list, MSList *elem);

/*************************************************
  Function:		ms_list_find
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_find(MSList *list, void *data);

/*************************************************
  Function:		ms_list_remove
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_remove(MSList *first, void *data);

/*************************************************
  Function:		ms_media_get_desc_form_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSMediaDesc* ms_media_get_desc_form_id(MSMediaId id);

/*************************************************
  Function:		ms_media_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSMediaDesc *ms_media_new(MSMediaId id);

/*************************************************
  Function:		ms_media_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_link(MSMediaDesc *f1, MSMediaDesc *f2);

/*************************************************
  Function:		ms_media_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_unlink(MSMediaDesc *f1, MSMediaDesc *f2);

/*************************************************
  Function:		ms_media_link_chunk
  Description: 	
  Input: 
  	3.chunk		链接后数据存储的块索引
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_link_chunk(MSMediaDesc * f1, MSMediaDesc * f2, unsigned int chunk);

/*************************************************
  Function:		ms_media_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_unlink_chunk(MSMediaDesc * f1, MSMediaDesc * f2, unsigned int chunk);

/*************************************************
  Function:		ms_media_blk_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_blk_init(mblk_t * arg);

/*************************************************
  Function:		ms_media_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_process(MSMediaDesc *f1, mblk_t * arg);

/*************************************************
  Function:		ms_media_process_by_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_process_by_id(MSMediaDesc * f1, mblk_t * arg);

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method(MSMediaDesc *f, unsigned int id, void * arg);

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method_noarg(MSMediaDesc *f, unsigned int id);

/*************************************************
  Function:		ms_filter_set_notify_callback
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_set_notify_callback(MSMediaDesc *f, MSFilterNotifyFunc fn/*, void *ud*/);

/*************************************************
  Function:		ms_filter_notify
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify(MSMediaDesc *f, unsigned int id, void * arg);

/*************************************************
  Function:		ms_filter_notify_no_arg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify_no_arg(MSMediaDesc *f, unsigned int id);

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_quit(MSThread* thread);

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void * arg);

/*************************************************
  Function:		ms_media_register
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_register(MSMediaDesc *desc);

/*************************************************
  Function:		ms_media_unregister_all
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_unregister_all(void);

/*************************************************
  Function:		ms_media_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_init(void);

/*************************************************
  Function:		ms_media_exit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_exit(void);

/*************************************************
  Function:			ms_err_callback
  Description:		错误回调
  Input: 	
  Output:			无
  Return:			无
  Others:
*************************************************/
void ms_err_callback(MSMediaDesc *f, unsigned int param1, unsigned int param2);

/*************************************************
  Function:			ms_percent_callback
  Description:		进度回调
  Input: 	
  Output:			无
  Return:			无
  Others:
*************************************************/
void ms_percent_callback(MSMediaDesc *f, unsigned int param1, unsigned int param2);

/*************************************************
  Function:		ms_queue_get_chunk
  Description: 	获取内存块中数据
  Input: 
  	1. f			媒体描述
  	3. index		块内存索引
  Output:
  	2.arg
  Return:		 
  Others:
*************************************************/
int ms_queue_get_chunk(MSMediaDesc *f, mblk_t * arg, unsigned char index);

/*************************************************
  Function:		ms_queue_pop_chunk
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_queue_pop_chunk(MSMediaDesc *f, unsigned char index);
#endif

