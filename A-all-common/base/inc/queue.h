#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <sys/uio.h>
#include <pthread.h>
#include "kernel_list.h"

typedef enum __queue_mode__
{
    QUEUE_FULL_FLUSH = 0,
    QUEUE_FULL_RING,
    QUEUE_FULL_RETURN
} queue_mode_t;

typedef struct __item__
{
    struct iovec data;
    struct list_head entry;
} item_t;

typedef void *(alloc_hook)(void *data, size_t len);
typedef void (free_hook)(void *data);

typedef struct __queue__
{
    struct list_head head;
    int depth;
    int max_depth;
    pthread_mutex_t lock;
    //pthread_cond_t cond;
    queue_mode_t mode;
    alloc_hook *alloc_hook;
    free_hook *free_hook;
} queue_t;

item_t *item_alloc(queue_t *q, void *data, size_t len);
void item_free(queue_t *q, item_t *item);
void *item_get_data(queue_t *q, item_t *item);
int item_get_data_len(queue_t *q, item_t *item);

queue_t *queue_create();
void queue_destroy(queue_t *q);
int queue_set_depth(queue_t *q, int depth);
int queue_get_depth(queue_t *q);
int queue_set_mode(queue_t *q, queue_mode_t mode);
int queue_set_hook(queue_t *q, alloc_hook *alloc_cb, free_hook *free_cb);
item_t *queue_pop(queue_t *q);
int queue_push(queue_t *q, item_t *item);
int queue_flush(queue_t *q);
int queue_is_full(queue_t *q);
void item_free_just(item_t *item);

#endif


