#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "queue.h"
#include "log_trace.h"

#define QUEUE_MAX_DEPTH		200

void *memdup(void *src, size_t len)
{
    void *dst = calloc(1, len);
    if (dst == NULL)
    {
        LOG_TRACE_PERROR("calloc error!\n");
        return NULL;
    }
    
    memcpy(dst, src, len);
    return dst;
}

item_t *item_alloc(queue_t *q, void *data, size_t len)
{
    if (q == NULL)
    {
        return NULL;
    }
    
    item_t *item = calloc(1, sizeof(item_t));
    if (item == NULL)
    {
        LOG_TRACE_PERROR("calloc failed!\n");
        return NULL;
    }
    
    if (q->alloc_hook)
    {
        item->data.iov_base = (q->alloc_hook)(data, len);
        item->data.iov_len = len;
    }
    else
    {
        item->data.iov_base = memdup(data, len);
        item->data.iov_len = len;
    }
    
    return item;
}

void item_free(queue_t *q, item_t *item)
{
    if (q == NULL)
    {
        return;
    }
    if (item == NULL)
    {
        return;
    }
    
    if (q->free_hook)
    {
        (q->free_hook)(item->data.iov_base);
        item->data.iov_len = 0;
    }
    else
    {
    	if (item->data.iov_base)
    	{
        	free(item->data.iov_base);
        }
    }
    
    free(item);
}

void *item_get_data(queue_t *q, item_t *item)
{
	if (q && item)
	{
		return item->data.iov_base;
	}

	return NULL;
}

int item_get_data_len(queue_t *q, item_t *item)
{
	if (q && item)
	{
		return item->data.iov_len;
	}

	return -1;
}

int queue_set_mode(queue_t *q, queue_mode_t mode)
{
    if (q == NULL)
    {
        return -1;
    }

    q->mode = mode;
    return 0;
}

int queue_set_hook(queue_t *q, alloc_hook *alloc_cb, free_hook *free_cb)
{
    if (q == NULL)
    {
        return -1;
    }
    
    q->alloc_hook = alloc_cb;
    q->free_hook = free_cb;
    return 0;
}

int queue_set_depth(queue_t *q, int depth)
{
    if (q == NULL)
    {
        return -1;
    }
    q->max_depth = depth;
    return 0;
}

queue_t *queue_create()
{
    queue_t *q = calloc(1, sizeof(queue_t));
    if (q == NULL)
    {
        LOG_TRACE_PERROR("calloc failed!\n");
        return NULL;
    }
    INIT_LIST_HEAD(&q->head);
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->depth = 0;
    q->max_depth = QUEUE_MAX_DEPTH;
    q->mode = QUEUE_FULL_FLUSH;
    q->alloc_hook = NULL;
    q->free_hook = NULL;
    return q;
}

int queue_flush(queue_t *q)
{
    if (q == NULL)
    {
        return -1;
    }
    
    item_t *item, *next;
    pthread_mutex_lock(&q->lock);
    list_for_each_entry_safe(item, next, &q->head, entry)
    {
        list_del(&item->entry);
        item_free(q, item);
    }
    
    q->depth = 0;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

void queue_destroy(queue_t *q)
{
    if (q == NULL)
    {
        return;
    }
    queue_flush(q);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->cond);
    free(q);
}

void queue_pop_free(queue_t *q)
{
    item_t *tmp = queue_pop(q);
    if (tmp)
    {
        item_free(q, tmp);
    }
}

int queue_push(queue_t *q, item_t *item)
{
    if (q == NULL || item == NULL)
    {
        return -1;
    }
    
    if (q->depth >= q->max_depth)
    {
        if (q->mode == QUEUE_FULL_FLUSH)
        {
            queue_flush(q);
        }
        else if (q->mode == QUEUE_FULL_RING)
        {
            queue_pop_free(q);
        }
    }
    pthread_mutex_lock(&q->lock);
    list_add_tail(&item->entry, &q->head);
    ++ (q->depth);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
    if (q->depth > q->max_depth)
    {
        LOG_TRACE_NORMAL("queue depth reach max depth %d\n", q->depth);
    }
    //LOG_TRACE_NORMAL("push queue depth is %d\n", q->depth);
    return 0;
}

item_t *queue_pop(queue_t *q)
{
	int ret = -1;
    item_t *item = NULL;

	struct timeval now;
	struct timespec outtime;

    if (q == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&q->lock);
    while (list_empty(&q->head))
    {
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + 1;
        outtime.tv_nsec = now.tv_usec * 1000;
        ret = pthread_cond_timedwait(&q->cond, &q->lock, &outtime);
        if (ret == 0)
        {
            break;
        }
        switch (ret)
        {
	        case ETIMEDOUT:
	            //LOG_TRACE_NORMAL("the condition variable was not signaled "
	            //       "until the timeout specified by abstime.\n");
	            break;
	        case EINTR:
	            LOG_TRACE_NORMAL("pthread_cond_timedwait was interrupted by a signal.\n");
	            break;
	        default:
	            LOG_TRACE_NORMAL("pthread_cond_timedwait error:%s.\n", strerror(ret));
	            break;
        }
    }

    item = list_first_entry_or_null(&q->head, item_t, entry);
    if (item)
    {
        list_del(&item->entry);
        -- (q->depth);
    }
    
    pthread_mutex_unlock(&q->lock);
    //LOG_TRACE_NORMAL("pop queue depth is %d\n", q->depth);
    return item;
}

int queue_get_depth(queue_t *q)
{
    return q->depth;
}



