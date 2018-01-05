/******************************************************************************
 * queue.c
 * 
 * Copyright (c) 2009 Fuzhou Rockchip Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *     Functions used to manage a FIFO queue .
 *
 * modification history
 * --------------------
 * Keith Lin, Feb 27, 2009,  Initial version
 * --------------------
 ******************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include "logic_queue.h"

/**
 * queue initialize, 
 * return Queue pointer on success and NULL on error.
 */
Queue *queue_init(int32_t max_nb_item, void (*destruct)(void *, void *), void *param)
{
	Queue *q = malloc(sizeof(*q));
	
	if(NULL == q)
		return NULL;
	
	q->items = malloc(max_nb_item * sizeof(*q->items));
	
	if(NULL == q->items)
	{
		free(q);
		return NULL;
	}
	
	q->first = q->last = 0;
	q->nb_item = 0;
	q->max_nb_item = max_nb_item;
	q->size = 0;
    q->destruct = (void (*)(void *, void *))destruct;
    q->param = param;
	
	return q;
}

/**
 * put an element to the queue, 
 * return 0 on success and -1 on error.
 */
int32_t queue_put(Queue *q, void *elem, int32_t size)
{
	if(q->nb_item >= q->max_nb_item)
		return -1;
	
	q->items[q->last].elem = elem;
	q->items[q->last].size = size;
	q->last = (q->last + 1) % q->max_nb_item;
	q->size += size;
	q->nb_item++;
	
	return 0;
}

/**
 * get an element from the queue.
 * return element pointer on success and NULL on error.
 */
void *queue_get(Queue *q)
{
	Item *item;
	
	if(q->nb_item <= 0)
		return NULL;
	
	item = &(q->items[q->first]);
	q->first = (q->first + 1) % q->max_nb_item;
	q->size -= item->size;
	q->nb_item--;
	
	return item->elem;
}

// mmmmchen 2010-01-05
/**
 * 只是获取,没有移动队列
 * 
 */
void *queue_show_first(Queue *q)
{
	Item *item;
	
	if(q->nb_item <= 0)
		return NULL;
	
	item = &(q->items[q->first]);
	
	return item->elem;
}

/**
 * Flush the queue.
 */
void queue_flush(Queue *q)
{
    /* destruct all elements in the queue */
    if (NULL != q->destruct) 
    {
        for(; q->nb_item > 0; q->nb_item --)
        {
            q->destruct(q->param, q->items[q->first].elem);
            q->first = (q->first + 1) % q->max_nb_item;
        }
    }
    
	q->first = q->last = 0;
	q->nb_item = 0;
	q->size = 0;
}

/**
 * Destroy the queue.
 */
void queue_destroy(Queue *q)
{
    /* destruct all elements first */
    if (NULL != q->destruct) 
    {
        for(; q->nb_item > 0; q->nb_item--)
        {
            q->destruct(q->param, q->items[q->first].elem);
            q->first = (q->first + 1) % q->max_nb_item;
        }
    }

	if(NULL != q)
	{
		if(NULL != q->items)
        {   
			free(q->items);
            q->items = NULL;
        }
		free(q);
		q = NULL;
	}
}

int32_t queue_is_full(Queue *q)
{
	return (q->nb_item >= q->max_nb_item) ? 1 : 0;
}

int32_t queue_is_empty(Queue *q)
{
	return (q->nb_item == 0);
}

int32_t queue_size(Queue *q)
{
    return q->size;
}

