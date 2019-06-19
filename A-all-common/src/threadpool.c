#include "sys_inc.h"
#include "log_trace.h"
#include "threadpool.h"
#include "user_time.h"

static pthread_mutex_t _tpool_working_count_lock = PTHREAD_MUTEX_INITIALIZER;

static tpool_t *_tpool = NULL;

#define _tpool_working_count(c) \
	do { \
		 if (_tpool) \
		 { \
			pthread_mutex_lock(&_tpool_working_count_lock); \
			_tpool->working_counts += c; \
			pthread_mutex_unlock(&_tpool_working_count_lock); \
		 } \
	} while (0);

static void *thread_routine(void *arg)
{
	pthread_detach(pthread_self());
	
    tpool_work_t *work;

	//LOG_TRACE_NORMAL("thread:%u begin running ...\n", 
		//	(unsigned int) pthread_self());
    
	pthread_mutex_lock(&_tpool_working_count_lock);
	_tpool->worker_counts += 1;
	pthread_mutex_unlock(&_tpool_working_count_lock);
	
    while (1)
    {
        pthread_mutex_lock(&_tpool->queue_lock);
        while (_tpool->queue_head == NULL
        	&& _tpool->shutdown == 0)
        {
            pthread_cond_wait(&_tpool->queue_ready, &_tpool->queue_lock);
        }
        if (_tpool->shutdown)
        {
            pthread_mutex_unlock(&_tpool->queue_lock);
            pthread_exit(NULL);
        }

        work = _tpool->queue_head;
        if (work == NULL)
        {
        	pthread_mutex_unlock(&_tpool->queue_lock);
        	continue;
        }
        
		_tpool->queue_head = _tpool->queue_head->next;
		pthread_mutex_unlock(&_tpool->queue_lock);
		
		_tpool_working_count(1);
		work->routine(work->arg);
		free(work);
		work = NULL;
        _tpool_working_count(-1);
    }

    return NULL;
}

int tpool_create(int max_thr_num)
{
    int i = 0;
 
    _tpool = calloc(sizeof(tpool_t), 1);
    if (_tpool == NULL)
    {
        LOG_TRACE_PERROR("calloc failed!\n");
        return -1;
    }
    
    _tpool->max_thr_num = max_thr_num;
    _tpool->working_counts = 0;
    _tpool->shutdown = 0;
    _tpool->queue_head = NULL;
    if (pthread_mutex_init(&_tpool->queue_lock, NULL) != 0)
    {
        LOG_TRACE_PERROR("pthread_mutex_init failed!\n");
        free(_tpool);
        return -1;
    }
    if (pthread_mutex_init(&_tpool->put_lock, NULL) != 0)
    {
        LOG_TRACE_PERROR("pthread_mutex_init failed!\n");
        free(_tpool);
        return -1;
    }
    if (pthread_cond_init(&_tpool->queue_ready, NULL) != 0)
    {
		LOG_TRACE_PERROR("pthread_cond_init failed!\n");
        free(_tpool);
        return -1;
    }
    
    _tpool->thr_id = calloc(1, max_thr_num * sizeof(pthread_t));
    if (_tpool->thr_id == NULL)
    {
        LOG_TRACE_PERROR("calloc failed!\n");
        free(_tpool);
        return -1;
    }
    for (i = 0; i < max_thr_num; i ++)
    {
        if (pthread_create(&_tpool->thr_id[i], NULL, thread_routine, NULL) != 0)
        {
			LOG_TRACE_PERROR("pthread_create failed!\n");
			free(_tpool->thr_id);
			free(_tpool);
			return -1;
        }
    }    
 
    return 0;
}
 
int tpool_destroy()
{
    int i;
    tpool_work_t *member;
 
    if (_tpool->shutdown)
    {
        return 0;
    }
    _tpool->shutdown = 1;
 
    pthread_mutex_lock(&_tpool->queue_lock);
    pthread_cond_broadcast(&_tpool->queue_ready);
    pthread_mutex_unlock(&_tpool->queue_lock);
    for (i = 0; i < _tpool->max_thr_num; i ++)
    {
        pthread_join(_tpool->thr_id[i], NULL);
    }
    free(_tpool->thr_id);
 
    while (_tpool->queue_head)
    {
        member = _tpool->queue_head;
        _tpool->queue_head = _tpool->queue_head->next;
        free(member);
    }
 
    pthread_mutex_destroy(&_tpool->queue_lock);
    pthread_mutex_destroy(&_tpool->put_lock);
    pthread_cond_destroy(&_tpool->queue_ready);
 
    free(_tpool);

    return 0;
}
 
int tpool_add_work(void *(*routine)(void*), void *arg)
{
	bool ret = false;
    tpool_work_t *work, *member;
	pthread_mutex_lock(&_tpool_working_count_lock);
	ret = _tpool->working_counts >= _tpool->max_thr_num;
	pthread_mutex_unlock(&_tpool_working_count_lock);
	if (ret)
	{
		LOG_TRACE_NORMAL("threads are all busy now! run_counts:%d .\n", 
				_tpool->working_counts);
		return -2;
	}

    if (! routine)
    {
        LOG_TRACE_NORMAL("Invalid argument\n");
        return -1;
    }
    
    work = calloc(sizeof(tpool_work_t), 1);
    if (work == NULL)
    {
        LOG_TRACE_PERROR("calloc failed!\n");
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;
 
    pthread_mutex_lock(&_tpool->queue_lock);
    member = _tpool->queue_head;
    if (member == NULL)
    {
        _tpool->queue_head = work;
    } 
    else
    {
        while (member->next)
        {
            member = member->next;
        }
        member->next = work;
    }
    
    pthread_mutex_unlock(&_tpool->queue_lock);
 	pthread_cond_signal(&_tpool->queue_ready);
 	
    return 0;    
}

int display_worker_counts()
{
	unsigned int now_time_sec = 0;
	static unsigned int old_time_sec = -1;
	static unsigned int diff_cnt = -1;
	now_time_sec = get_system_sec();
	if (now_time_sec != old_time_sec)
	{
		diff_cnt ++;
		old_time_sec = now_time_sec;
	}
	else
	{
		return 0;
	}

	if (diff_cnt >= 1)
	{
		diff_cnt = 0;
		
		LOG_TRACE_NORMAL("\n");
		LOG_TRACE_NORMAL("--------------------------------\n");
		LOG_TRACE_NORMAL("- there are %d worker thread .\n", 
				_tpool->working_counts);
		LOG_TRACE_NORMAL("--------------------------------\n");
		LOG_TRACE_NORMAL("\n");
	}
	
	return 0;
}

int tpool_get_worker_count()
{
	return _tpool->worker_counts;
}

int tpool_get_working_count()
{
	return _tpool->working_counts;
}

