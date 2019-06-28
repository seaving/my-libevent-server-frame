#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
 
#include <pthread.h>
 
/* 要执行的任务链表 */
typedef struct __tpool_work__
{
	void*				(*routine)(void*);		 /* 任务函数 */
	void				*arg;					 /* 传入任务函数的参数 */
	struct __tpool_work__	*next;					  
} tpool_work_t;

typedef struct __tpools__
{
	int 			shutdown;					 /* 线程池是否销毁 */
	int 			max_thr_num;				/* 最大线程数 */
	int				worker_counts;				/* 创建成功线程数量 */
	int 			working_counts;				/* 正在执行任务的线程数量 */
	pthread_t		*thr_id;					/* 线程ID数组 */
	tpool_work_t	*queue_head;				/* 线程链表 */
	pthread_mutex_t queue_lock;					/* 线程等信号锁 */
	pthread_cond_t	queue_ready;
	pthread_mutex_t put_lock;					/* 放置/取任务锁 */
} tpool_t;


int tpool_create(int max_thr_num);
int tpool_destroy();
int tpool_add_work(void *(*routine)(void*), void *arg);

int display_worker_counts();
int tpool_get_working_count();
int tpool_get_worker_count();

#endif

