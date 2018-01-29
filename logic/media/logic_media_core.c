/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_media_core.c
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

#include "logic_media_core.h"
#include "logic_tran_rtp.h"

/*
static MSMediaDesc * ms_media_descs[] =
{
	&ms_h264_enc_desc,
	&ms_cloud_h264_enc_desc,
	&ms_h264_dec_desc,
	&ms_audio_dec_desc,
	&ms_rtp_send_v_desc,
	&ms_rtp_send_a_desc, 
	&ms_rtp_recv_v_desc,
	&ms_rtp_recv_a_desc,
	&ms_jpegwrite_desc,
	&ms_file_player_desc,
	&ms_audio_ai_desc,
	&ms_audio_enc_desc,
	NULL,
};
*/
static MSMediaDesc * ms_media_descs[] =
{
	&ms_h264_dec_desc,
	&ms_audio_dec_desc,
	&ms_rtp_recv_v_desc,
	&ms_rtp_send_a_desc, 
	&ms_rtp_recv_a_desc,
	&ms_audio_ai_desc,
	&ms_audio_enc_desc,
	&ms_file_player_desc,
	&ms_audio_ao_desc,
	&ms_jpeg_dec_desc,
	&ms_jpeg_enc_desc,
	&ms_lyly_play_desc,
	&ms_lyly_record_desc,
	&ms_rtsp_play_desc,	
	&ms_wav_record_desc,
	//&ms_mp3_play_desc,
	&ms_lyly_hit_desc,	
	#ifdef _ENABLE_CLOUD_
	&ms_h264_enc_desc,
	&ms_cloud_send_audio_desc,
	&ms_cloud_send_video_desc,
	&ms_cloud_recv_audio_desc,
	&ms_cloud_h264_enc_desc,
	#endif
	NULL,
};

static MSList * desc_list = NULL;
static struct timeval t1, t2;
static int mTimeState = 0;

/*************************************************
  Function:			ms_queue_init
  Description:		初始化链表
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_init(MSMediaDesc *f)
{
	ms_return_val_if_fail(f, -1);
	
	int i;
	log_printf("input number == %d name == %s\n", f->ninputs, f->name);
	ms_queue_lock(f);
	if (f->sharebufsize > 0 && f->sharebufblk > 0)
	{
		f->queue.q_pop = 0;
		f->queue.q_push = 0;	
		f->queue.sharebufsize = f->sharebufsize * 1024;
		f->queue.sharebuffer = (char*)malloc(f->sharebufsize*1024);
		f->queue.sharebufnow = f->queue.sharebuffer;
		f->queue.q_blk = (mblk_t *)ms_new(mblk_t, f->sharebufblk);
		for (i = 0; i < f->sharebufblk; i++)
		{
			f->queue.q_blk[i].used = 0;
		}
	}
	
	if (f->sharebufsize > 0 && f->sharebufblk > 0 && f->ninputs == 2) // 两路输入的情况
	{
		if (f->sharebufsize > 0 && f->sharebufblk > 0)
		{
			f->queuesec.q_pop = 0;
			f->queuesec.q_push = 0;	
			f->queuesec.sharebufsize = f->sharebufsize * 1024;
			f->queuesec.sharebuffer = (char*)malloc(f->sharebufsize*1024);
			f->queuesec.sharebufnow = f->queuesec.sharebuffer;
			f->queuesec.q_blk = (mblk_t *)ms_new(mblk_t, f->sharebufblk);
			for (i = 0; i < f->sharebufblk; i++)
			{
				f->queuesec.q_blk[i].used = 0;
			}
		}
	}
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:			ms_queue_uninit
  Description:		初始化链表
  Input: 			无
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_uninit(MSMediaDesc *f)
{
	ms_return_val_if_fail(f, -1);

	int i;
	
	ms_queue_lock(f);
	if (f->sharebufsize > 0 && f->sharebufblk > 0)
	{
		f->queue.q_pop = 0;
		f->queue.q_push = 0;	
		f->queue.sharebufnow = f->queue.sharebuffer;
		f->queue.sharebufsize = f->sharebufsize*1024;
		for (i = 0; i < f->sharebufblk; i++)
		{
			f->queue.q_blk[i].used = 0;
		}
	}

	if (f->ninputs == 2)
	{
		if (f->sharebufsize > 0 && f->sharebufblk > 0)
		{
			f->queuesec.q_pop = 0;
			f->queuesec.q_push = 0;	
			f->queuesec.sharebufnow = f->queuesec.sharebuffer;
			f->queuesec.sharebufsize = f->sharebufsize*1024;
			for (i = 0; i < f->sharebufblk; i++)
			{
				f->queuesec.q_blk[i].used = 0;
			}
		}	
	}
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:			ms_queue_put
  Description:		把数据放入链表
  Input: 			
  	1.address	
  	2.len		
  	3.marker		
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_put(MSMediaDesc *f, mblk_t * arg)
{	
	ms_return_val_if_fail(f, -1);
	ms_return_val_if_fail(arg, -1);
	
	ms_queue_lock(f);
	if (f->queue.q_blk[f->queue.q_push].used == 1)
	{
		ms_queue_unlock(f);
		return -1;
	}

	f->queue.q_blk[f->queue.q_push].used = 1;
	f->queue.q_blk[f->queue.q_push].id = arg->id;
	f->queue.q_blk[f->queue.q_push].marker = arg->marker;
	f->queue.q_blk[f->queue.q_push].len = arg->len;
	f->queue.q_blk[f->queue.q_push].delay = arg->delay;
	f->queue.q_blk[f->queue.q_push].ts = arg->ts;

	if (f->queue.sharebufsize < arg->len)
	{
		f->queue.sharebufnow = f->queue.sharebuffer;
		f->queue.sharebufsize = f->sharebufsize*1024;
	}
	f->queue.q_blk[f->queue.q_push].address = f->queue.sharebufnow;
	memcpy(f->queue.q_blk[f->queue.q_push].address, arg->address, arg->len);
	f->queue.q_push++;
	if (f->queue.q_push >= f->sharebufblk)
	{
		f->queue.q_push = 0;
	}
	f->queue.sharebufnow = f->queue.sharebufnow+arg->len;
	f->queue.sharebufsize -= arg->len;
	ms_queue_unlock(f);
	
	return 0;
}


/*************************************************
  Function:			ms_queue_put_sec
  Description:		把数据放入链表
  Input: 			
  	1.address	
  	2.len		
  	3.marker		
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_put_sec(MSMediaDesc *f, mblk_t * arg)
{	
	ms_return_val_if_fail(f, -1);
	ms_return_val_if_fail(arg, -1);
	
	ms_queue_lock(f);
	if (f->queuesec.q_blk[f->queuesec.q_push].used == 1)
	{
		ms_queue_unlock(f);
		return -1;
	}

	f->queuesec.q_blk[f->queuesec.q_push].used = 1;
	f->queuesec.q_blk[f->queuesec.q_push].id = arg->id;
	f->queuesec.q_blk[f->queuesec.q_push].marker = arg->marker;
	f->queuesec.q_blk[f->queuesec.q_push].len = arg->len;
	f->queuesec.q_blk[f->queuesec.q_push].delay = arg->delay;
	f->queuesec.q_blk[f->queuesec.q_push].ts = arg->ts;

	if (f->queuesec.sharebufsize < arg->len)
	{
		f->queuesec.sharebufnow = f->queuesec.sharebuffer;
		f->queuesec.sharebufsize = f->sharebufsize*1024;
	}
	f->queuesec.q_blk[f->queuesec.q_push].address = f->queuesec.sharebufnow;
	memcpy(f->queuesec.q_blk[f->queuesec.q_push].address, arg->address, arg->len);
	f->queuesec.q_push++;
	if (f->queuesec.q_push >= f->sharebufblk)
	{
		f->queuesec.q_push = 0;
	}
	f->queuesec.sharebufnow = f->queuesec.sharebufnow+arg->len;
	f->queuesec.sharebufsize -= arg->len;
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:			ms_queue_put_t
  Description:		把数据放入链表
  Input: 			
  	1.address	
  	2.len		
  	3.marker		
  Output:			无
  Return:			
  Others:
*************************************************/
static int ms_queue_put_t(MSMediaDesc *f, mblk_t * arg)
{	
	int nlen = 0;
	ms_return_val_if_fail(f, -1);
	ms_return_val_if_fail(arg, -1);
	
	ms_queue_lock(f);
	if (f->queue.q_blk[f->queue.q_push].used == 1)
	{
		ms_queue_unlock(f);
		return -1;
	}
	
	nlen = arg->len+arg->len_t;
	
	f->queue.q_blk[f->queue.q_push].used = 1;
	f->queue.q_blk[f->queue.q_push].id = arg->id;
	f->queue.q_blk[f->queue.q_push].marker = arg->marker;
	f->queue.q_blk[f->queue.q_push].len = nlen;
	f->queue.q_blk[f->queue.q_push].delay = arg->delay;
	f->queue.q_blk[f->queue.q_push].ts = arg->ts;
	
	if (f->queue.sharebufsize < nlen)
	{
		f->queue.sharebufnow = f->queue.sharebuffer;
		f->queue.sharebufsize = f->sharebufsize*1024;
	}
	f->queue.q_blk[f->queue.q_push].address = f->queue.sharebufnow;
	memcpy(f->queue.q_blk[f->queue.q_push].address, arg->address, arg->len);
	memcpy(f->queue.q_blk[f->queue.q_push].address+arg->len, arg->address_t, arg->len_t);	
	f->queue.q_push++;
	if (f->queue.q_push >= f->sharebufblk)
	{
		f->queue.q_push = 0;
	}
	f->queue.sharebufnow = f->queue.sharebufnow+nlen;
	f->queue.sharebufsize -= nlen;
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_get(MSMediaDesc *f, mblk_t ** arg)
{
	ms_return_val_if_fail(f, -1);
	
	ms_queue_lock(f);
	if (f->queue.q_blk[f->queue.q_pop].used == 0)
	{
		ms_queue_unlock(f);
		return -1;
	}
	(* arg)->id = f->queue.q_blk[f->queue.q_pop].id;
	(* arg)->address = f->queue.q_blk[f->queue.q_pop].address;
	(* arg)->len = f->queue.q_blk[f->queue.q_pop].len;
	(* arg)->marker = f->queue.q_blk[f->queue.q_pop].marker;
	(* arg)->delay = f->queue.q_blk[f->queue.q_pop].delay;
	(* arg)->ts = f->queue.q_blk[f->queue.q_pop].ts;
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_get_sec(MSMediaDesc *f, mblk_t ** arg)
{
	ms_return_val_if_fail(f, -1);
	
	ms_queue_lock(f);
	if (f->queuesec.q_blk[f->queuesec.q_pop].used == 0)
	{
		ms_queue_unlock(f);
		return -1;
	}
	(* arg)->id = f->queuesec.q_blk[f->queuesec.q_pop].id;
	(* arg)->address = f->queuesec.q_blk[f->queuesec.q_pop].address;
	(* arg)->len = f->queuesec.q_blk[f->queuesec.q_pop].len;
	(* arg)->marker = f->queuesec.q_blk[f->queuesec.q_pop].marker;
	(* arg)->delay = f->queuesec.q_blk[f->queuesec.q_pop].delay;
	(* arg)->ts = f->queuesec.q_blk[f->queuesec.q_pop].ts;
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_get_double_queue(MSMediaDesc *f, mblk_t ** arg, mblk_t ** argsec)
{
	if (0 == ms_queue_get_sec(f, argsec))
	{
		if (0 == ms_queue_get(f, arg))
		{	
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return -1;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_pop(MSMediaDesc *f)
{
	ms_return_val_if_fail(f, -1);
	
	ms_queue_lock(f);
	if (f->queue.q_blk[f->queue.q_pop].used == 0)
	{
		ms_queue_unlock(f);
		return -1;
	}
	f->queue.q_blk[f->queue.q_pop].used = 0;
	f->queue.q_pop++;
	if (f->queue.q_pop >= f->sharebufblk)
	{
		f->queue.q_pop = 0;
	}
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_pop_sec(MSMediaDesc *f)
{
	ms_return_val_if_fail(f, -1);
	
	ms_queue_lock(f);
	if (f->queuesec.q_blk[f->queuesec.q_pop].used == 0)
	{
		ms_queue_unlock(f);
		return -1;
	}
	f->queuesec.q_blk[f->queuesec.q_pop].used = 0;
	f->queuesec.q_pop++;
	if (f->queuesec.q_pop >= f->sharebufblk)
	{
		f->queuesec.q_pop = 0;
	}
	ms_queue_unlock(f);
	
	return 0;
}

/*************************************************
  Function:		
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_queue_pop_double_queue(MSMediaDesc *f)
{
	ms_queue_pop(f);
	ms_queue_pop_sec(f);
	return 0;
}

/*************************************************
  Function:		ms_process_thread
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * ms_process_thread(void *ptr)
{
	MSMediaDesc *f = (MSMediaDesc *)ptr;
	mblk_t arg, argsec;
	mblk_t * arg1;
	mblk_t *arg2;
	
	while (f->threadprocess.thread_run)
	{
		if (f->ninputs == 0 || f->ninputs == 1) 	// 0和1都当着一路输入
		{
			arg1 = &arg;
			if (0 == ms_queue_get(f, (&arg1)))	
			{
				if (f->process)
				{
					f->process(f, &arg);
				}
				ms_queue_pop(f);
			}
			else
			{
				usleep(20);
			}
		}
		else if (f->ninputs == 2)  					// 2:两路输入  
		{
			arg1 = &arg;
			arg2 = &argsec;
			if (0 == ms_queue_get_double_queue(f, (&arg1), (&arg2)))
			{
				if (f->processsec)
				{
					f->processsec(f, &arg, &argsec);
				}
				ms_queue_pop_double_queue(f);
			}
			else if( 1 == ms_queue_get_double_queue(f, (&arg1), (&arg2)))	// 防止对方未发送数据过来
			{	
				arg.address = NULL;
				if (f->processsec)
				{
					f->processsec(f, &arg, &argsec);
				}
				ms_queue_pop_sec( f);
			}
			else
			{
				usleep(20);
			}	
		}
	}
	
	f->threadprocess.thread	= -1;
	pthread_detach(pthread_self());
    pthread_exit(NULL);
	
	return NULL;
}

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
int ms_queue_get_chunk(MSMediaDesc *f, mblk_t * arg, unsigned char index)
{
	if (index == 1 || index == 0)  // 块位于第一内存中
	{
		if (0 == ms_queue_get(f, (&arg)))
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else if (index == 2)  // 块位于第二内存中
	{
		if (0 == ms_queue_get_sec(f, (&arg)))
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
	
}

/*************************************************
  Function:		ms_queue_pop_chunk
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_queue_pop_chunk(MSMediaDesc *f, unsigned char index)
{
	if (index == 1 || index == 0)  // 块位于第一内存中
	{
		ms_queue_pop(f);
	}
	else if (index == 2)  // 块位于第二内存中
	{
		ms_queue_pop_sec(f);
	}
	else
		return;
}

/*************************************************
  Function:		ms_media_preprocess
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_media_preprocess(struct _MSMediaDesc * f)
{
	if (f->porcessmode == PROCESS_WORK_UNBLOCK)
	{
		ms_queue_uninit(f);
		ms_thread_init(&f->threadprocess, 20);
		ms_thread_create(&f->threadprocess, ms_process_thread, f);
	}
	
	return 0;
}

/*************************************************
  Function:		ms_media_postprocess
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int ms_media_postprocess(struct _MSMediaDesc * f)
{
	if (f->porcessmode == PROCESS_WORK_UNBLOCK)
	{
		ms_thread_quit(&f->threadprocess);
		ms_queue_uninit(f);
	}
	
	return 0;
}

/*************************************************
  Function:		ms_diff_time
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_diff_time(int state)
{
	struct timezone tz;
	mTimeState = state;
	if (1 == mTimeState)
	{
		gettimeofday(&t1, &tz);
	}
	else
	{
		gettimeofday(&t2, &tz);
	}
}

/*************************************************
  Function:		ms_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_free(void * data)
{
	if (data)
	{
		free(data);
		data = NULL;
	}
}

/*************************************************
  Function:		ms_list_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_new(void * data)
{
	MSList * new_elem = (MSList *)ms_new(MSList, 1);
	
	new_elem->prev = new_elem->next = NULL;
	new_elem->data = data;
	
	return new_elem;
}

/*************************************************
  Function:		ms_list_append
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_append(MSList *elem, void * data)
{
	MSList *new_elem = ms_list_new(data);
	MSList *it = elem;
	if (elem == NULL) 
	{
		return new_elem;
	}
	while (it->next!=NULL) 
	{
		it = ms_list_next(it);
	}
	it->next = new_elem;
	new_elem->prev = it;
	return elem;
}

/*************************************************
  Function:		ms_list_prepend
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_prepend(MSList * elem, void * data)
{
	MSList * new_elem = ms_list_new(data);
	
	if (elem != NULL) 
	{
		new_elem->next = elem;
		elem->prev = new_elem;
	}
	
	return new_elem;
}

/*************************************************
  Function:		ms_list_free
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_free(MSList *list)
{
	MSList * elem = list;
	MSList * tmp;
	
	if (list == NULL) 
	{
		return NULL;
	}
	while (elem->next != NULL) 
	{
		tmp = elem;
		elem = elem->next;
		ms_free(tmp);
	}
	ms_free(elem);
	
	return NULL;
}

/*************************************************
  Function:		ms_list_remove_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_remove_link(MSList *list, MSList *elem)
{
	MSList *ret;
	
	if (elem==list)
	{
		ret = elem->next;
		elem->prev = NULL;
		elem->next = NULL;
		if (ret != NULL) 
		{
			ret->prev = NULL;
		}
		ms_free(elem);
		return ret;
	}
	elem->prev->next = elem->next;
	if (elem->next != NULL)
	{
		elem->next->prev = elem->prev;
	}
	elem->next = NULL;
	elem->prev = NULL;
	ms_free(elem);
	
	return list;
}

/*************************************************
  Function:		ms_list_find
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList *ms_list_find(MSList *list, void *data)
{
	for (; list != NULL; list = list->next)
	{
		if (list->data == data)
		{
			return list;
		}
	}
	
	return NULL;
}

/*************************************************
  Function:		ms_list_remove
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSList * ms_list_remove(MSList *first, void *data)
{
	MSList *it;
	it = ms_list_find(first,data);
	if (it) 
	{
		return ms_list_remove_link(first,it);
	}
	else 
	{
		return first;
	}
}

/*************************************************
  Function:		ms_media_get_desc_form_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSMediaDesc* ms_media_get_desc_form_id(MSMediaId id)
{
	MSList * elem;
	
	for (elem = desc_list; elem != NULL; elem = ms_list_next(elem))
	{
		MSMediaDesc *desc = (MSMediaDesc*)elem->data;
		
		if (desc->id == id)
		{
			return desc;
		}
	}

	return NULL;
}

/*************************************************
  Function:		ms_media_new
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
MSMediaDesc * ms_media_new(MSMediaId id)
{
	MSMediaDesc * desc = ms_media_get_desc_form_id(id);

	if (desc)
	{
		log_printf(" [%s]\n",desc->name);
	}

	return desc;
}

/*************************************************
  Function:		ms_media_destory
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_destory(MSMediaDesc ** desc)
{
	ms_return_val_if_fail(desc, -1);
	ms_return_val_if_fail((*desc), -1);
	
	(*desc) = NULL;
	
	return 0;
}

/*************************************************
  Function:		ms_media_link
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_link(MSMediaDesc * f1, MSMediaDesc * f2)
{
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	int i, ret = -1;
	int connect = 0;
	MSMediaDesc * f3 = NULL;
	
	ms_process_lock(f1);
	for (i = 0; i < f1->noutputs; i++)
	{
		if (f1->outputs[i].outputs != NULL)
		{
			f3 = (MSMediaDesc *)f1->outputs[i].outputs;
			if (f3->id == f2->id)
			{
				f1->outputs[i].noutputs++;
				f1->outputs[i].linkinputs = 1;
				connect = 1;
				ret = i;
				break;
			}
		}
	}

	if (connect == 0)
	{
		for (i = 0; i < f1->noutputs; i++)
		{
			if (f1->outputs[i].outputs == NULL)
			{
				f1->outputs[i].outputs = f2;
				f1->outputs[i].noutputs = 1;
				f1->outputs[i].linkinputs = 1;
				ret = i;
				break;
			}
		}
	}
	ms_process_unlock(f1);
	
	if (-1 != ret)
	{
		if (connect == 0)
		{
			log_printf("ms_media_link f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
		else
		{
			log_printf("ms_media_link have connect f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
	}
	else
	{
		log_printf("ms_media_link ERR f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
	}
	
	return ret;
}

/*************************************************
  Function:		ms_media_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_unlink(MSMediaDesc * f1, MSMediaDesc * f2)
{
	int i, ret = -1;
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	ms_process_lock(f1);
	for (i = 0; i < f1->noutputs; i++)
	{
		if (f1->outputs[i].outputs == f2)
		{
			if (f1->outputs[i].noutputs > 0)
			{
				f1->outputs[i].noutputs--;
			}
			if (f1->outputs[i].noutputs == 0)
			{
				f1->outputs[i].outputs = NULL;
				f1->outputs[i].linkinputs = 0;
				ret = 0;
			}
			break;
		}
	}
	ms_process_unlock(f1);
	
	if (-1 != ret)
	{
		log_printf("ms_media_unlink f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
	}
	
	return ret;
}

/*************************************************
  Function:		ms_media_link_chunk
  Description: 	
  Input: 
  	3.chunk		链接后数据存储的块索引
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_link_chunk(MSMediaDesc * f1, MSMediaDesc * f2, unsigned int chunk)
{
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	int i, ret = -1;
	int connect = 0;
	MSMediaDesc * f3 = NULL;
	
	ms_process_lock(f1);
	for (i = 0; i < f1->noutputs; i++)
	{
		if (f1->outputs[i].outputs != NULL)
		{
			f3 = (MSMediaDesc *)f1->outputs[i].outputs;
			if (f3->id == f2->id)
			{
				f1->outputs[i].noutputs++;
				f1->outputs[i].linkinputs = chunk;
				connect = 1;
				ret = i;
				break;
			}
		}
	}

	if (connect == 0)
	{
		for (i = 0; i < f1->noutputs; i++)
		{
			if (f1->outputs[i].outputs == NULL)
			{
				f1->outputs[i].outputs = f2;
				f1->outputs[i].noutputs = 1;
				f1->outputs[i].linkinputs = chunk;
				ret = i;
				break;
			}
		}
	}
	ms_process_unlock(f1);
	
	if (-1 != ret)
	{
		if (connect == 0)
		{
			log_printf("ms_media_link f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
		else
		{
			log_printf("ms_media_link have connect f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
		}
	}
	else
	{
		log_printf("ms_media_link ERR f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
	}
	
	return ret;
}

/*************************************************
  Function:		ms_media_unlink
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_unlink_chunk(MSMediaDesc * f1, MSMediaDesc * f2, unsigned int chunk)
{
	int i, ret = -1;
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(f2, -1);
	
	ms_process_lock(f1);
	for (i = 0; i < f1->noutputs; i++)
	{
		if (f1->outputs[i].outputs == f2)
		{
			if (f1->outputs[i].noutputs > 0)
			{
				f1->outputs[i].noutputs--;
			}
			if (f1->outputs[i].noutputs == 0)
			{
				f1->outputs[i].outputs = NULL;
				f1->outputs[i].linkinputs = 0;
				ret = 0;
			}
			break;
		}
	}
	ms_process_unlock(f1);
	
	if (-1 != ret)
	{
		log_printf("ms_media_unlink f1 [%s] -> f2 [%s]\n",f1->name,f2->name);
	}
	
	return ret;
}

/*************************************************
  Function:		ms_media_blk_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_blk_init(mblk_t * arg)
{
	ms_return_val_if_fail(arg, -1);

	arg->id = MS_FILTER_NOT_SET_ID;
	arg->used = 0;
	arg->marker = 0;
	arg->delay = 0;
	arg->address = NULL;
	arg->len = 0;
    arg->address_t = NULL;
    arg->len_t = 0;
	arg->ts = 0;
	
	return 0;
}

#if TEST_VIDEO
/*************************************************
  Function:		test_ms_media_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int test_ms_media_process(MSMediaDesc * f1, mblk_t * arg)
{
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(arg, -1);	
	int ret = 0;
	ms_process_lock(f1);  

	MSMediaDesc * f = f1;
	if (NULL != f && (arg->id == MS_FILTER_NOT_SET_ID || arg->id == f->id))
	{
		if (PROCESS_WORK_BLOCK == f->porcessmode)
		{
			if (f->process)
			{
				f->process(f, arg);
			}
		}
		else if (PROCESS_WORK_UNBLOCK == f->porcessmode)
		{
			if (arg->len_t == 0)
			{
				ret = ms_queue_put(f, arg);
			}
			else
			{
				ret = ms_queue_put_t(f, arg);
			}
		}
	}
	
	ms_process_unlock(f1);
	return ret;
}
#endif

/*************************************************
  Function:		ms_media_process
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_process(MSMediaDesc * f1, mblk_t * arg)
{
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(arg, -1);
	
	int i, index = 0;
	MSMediaDesc * f;
	
	ms_process_lock(f1);  
	if (f1->noutputs > 0)
	{
		for (i = 0; i < f1->noutputs; i++)
		{
			index = f1->outputs[i].linkinputs;
			//log_printf(" ***************** index : %d\n ", index);
			f = f1->outputs[i].outputs;	
			if (NULL != f && (arg->id == MS_FILTER_NOT_SET_ID || arg->id == f->id))
			{
				if (PROCESS_WORK_BLOCK == f->porcessmode)
				{
					if (f->process)
					{
						f->process(f, arg);
					}
				}
				else if (PROCESS_WORK_UNBLOCK == f->porcessmode)
				{
					if (index == 1)   // 送入第一个缓冲区
					{
						if (arg->len_t == 0)
						{
							ms_queue_put(f, arg);
						}
						else
						{
							ms_queue_put_t(f, arg);
						}
					}
					else if (index == 2 && f->ninputs == 2)  // 送入第二个缓冲区 
					{
						ms_queue_put_sec(f, arg);
					}
				}		
			}
		}
	}
	ms_process_unlock(f1);
	
	return 0;
}

/*************************************************
  Function:		ms_media_process_by_id
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_media_process_by_id(MSMediaDesc * f1, mblk_t * arg)
{
	ms_return_val_if_fail(f1, -1);
	ms_return_val_if_fail(arg, -1);
	ms_process_lock(f1); 
	
	MSMediaDesc * f = ms_media_get_desc_form_id(arg->id);
	if (f)
	{
		//log_printf(" [%s]\n", f->name);
		if (PROCESS_WORK_BLOCK == f->porcessmode)
		{
			if (f->process)
			{
				f->process(f, arg);
			}
		}
		else if (PROCESS_WORK_UNBLOCK == f->porcessmode)
		{			
			if (arg->len_t == 0)
			{
				ms_queue_put(f, arg);
			}
			else
			{
				ms_queue_put_t(f, arg);
			}			
		}	
		ms_process_unlock(f1);
		return 0;
	}

	ms_process_unlock(f1);
	return -1;
}

/*************************************************
  Function:		ms_filter_call_method
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method(MSMediaDesc * f, unsigned int id, void * arg)
{
	ms_return_val_if_fail(f, -1);

	int i;
	MSMediaMethod * methods = f->methods;
	unsigned int magic = MS_MEDIA_METHOD_GET_FID(id);

	if (magic != f->id) 
	{
		log_printf("error  magic != f->id");
		return -1;
	}
	
	for (i = 0; methods!=NULL && methods[i].method != NULL; i++)
	{
		unsigned int mm = MS_MEDIA_METHOD_GET_FID(methods[i].id);
		if (mm != f->id)
		{
			log_printf("error mm != f->id");
			return -1;
		}
		
		if (methods[i].id == id)
		{
			log_printf("goto func name :  %s\n", f->name);
			return methods[i].method(f, arg);
		}
	}
	
	log_printf("error!!\n");
	return -1;
}

/*************************************************
  Function:		ms_filter_call_method_noarg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_call_method_noarg(MSMediaDesc *f, unsigned int id)
{
	return ms_filter_call_method(f, id, NULL);
}

/*************************************************
  Function:		ms_filter_set_notify_callback
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_set_notify_callback(MSMediaDesc *f, MSFilterNotifyFunc fn)
{
	ms_return_val_if_fail(f, -1);
	
	f->notify = fn;
	return 0;
}

/*************************************************
  Function:		ms_filter_notify
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify(MSMediaDesc *f, unsigned int persent, void * arg)
{
	ms_return_val_if_fail(f, -1);
	
	if (f->notify != NULL)
	{
		f->notify(1, 0, persent);
	}

	return 0;
}

/*************************************************
  Function:		ms_filter_notify_no_arg
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int ms_filter_notify_no_arg(MSMediaDesc *f, unsigned int persent)
{
	return ms_filter_notify(f,persent,NULL);
}

/*************************************************
  Function:		__ms_thread_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_init(MSThread * thread, int exittimer)
{
	ms_return_val_if_fail(thread, -1);
	
	thread->thread_exittimer = exittimer;
	return 0;
}

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_quit(MSThread* thread)
{
	ms_return_val_if_fail(thread, -1);
	
	int Timer = 0;

	if (thread->thread == -1)
	{	
		return 0;
	}
	thread->thread_run = 0;
	while (thread->thread != -1)
	{
		if (Timer++ > thread->thread_exittimer)
		{
			break;
		}
		usleep(1000);
	}
	if (Timer >= thread->thread_exittimer)
	{
		log_printf("ms_thread_quit time out\n");
	}
	else
	{
		log_printf("ms_thread_quit on time out\n");
	}
		
	return 0;
}

/*************************************************
  Function:		__ms_thread_create
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int __ms_thread_create(MSThread* thread, void * (*routine)(void*), void * arg)
{
	ms_return_val_if_fail(thread, -1);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	thread->thread_run = 1;
	if (0 != pthread_create(&thread->thread, &attr, (void*)routine, (void*)arg))
	{
		thread->thread_run = 0;
		pthread_attr_destroy(&attr);
		return -1;
	}
	pthread_attr_destroy(&attr);
	
	return 0;
}

/*************************************************
  Function:		ms_media_register
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_register(MSMediaDesc * desc)
{
	int i;
	
	if (desc->id == MS_FILTER_NOT_SET_ID)
	{
		log_printf("MSMediaDesc ID = MS_FILTER_NOT_SET_ID \n");
	}
	
	desc_list = ms_list_prepend(desc_list, desc);
	if (desc->init)
	{
		desc->init(desc);
	}

	if (desc->noutputs > 0)	
	{
		desc->outputs = (void*)ms_new(MSMediaOut, desc->noutputs);
		for (i = 0; i < desc->noutputs; i++)
		{
			desc->outputs[i].noutputs = 0;
			desc->outputs[i].linkinputs = 0;
			desc->outputs[i].outputs = NULL;			
		}
	}
	
	ms_mutex_init(&desc->lock, NULL);
	ms_mutex_init(&desc->lockprocess, NULL);
	ms_mutex_init(&desc->lockqueue, NULL);

	if (desc->porcessmode == PROCESS_WORK_UNBLOCK)
	{
		desc->preprocess = ms_media_preprocess;
		desc->postprocess = ms_media_postprocess;
		ms_queue_init(desc);
	}
	log_printf("ms_media_register for [%s] [%x]\n", desc->name, desc->porcessmode);
}

/*************************************************
  Function:		ms_media_unregister_all
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_unregister_all(void)
{
	if (desc_list != NULL) 
	{
		ms_list_free(desc_list);
	}
}

/*************************************************
  Function:		ms_media_init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_init(void)
{
	int i;
	
	for (i = 0; ms_media_descs[i] != NULL; i++)
	{
		ms_media_register(ms_media_descs[i]);
	}
}

/*************************************************
  Function:		ms_media_exit
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void ms_media_exit(void)
{
	ms_media_unregister_all();
}

/*************************************************
  Function:		ms_err_callback
  Description:	错误回调
  Input: 	
  Output:		无
  Return:		无
  Others:
*************************************************/
void ms_err_callback(MSMediaDesc *f, unsigned int param1, unsigned int param2)
{
	uint32 cmd = 0;				// 错误
	
	if (f->notify != NULL)
	{
		f->notify(cmd, param1, param2);
	}	
}

/*************************************************
  Function:		ms_percent_callback
  Description:	进度回调
  Input: 	
  	1、param1	总时间
  	2、param2	百分比0-100
  Output:		无
  Return:		无
  Others:
*************************************************/
void ms_percent_callback(MSMediaDesc * f, unsigned int param1, unsigned int param2)
{
	uint32 cmd = 1;				// 进度
	
	if (f->notify != NULL)
	{
		f->notify(cmd, param1, param2);
	}
}

