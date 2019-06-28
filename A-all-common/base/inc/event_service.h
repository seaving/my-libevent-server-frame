#ifndef __EVENT_SERVICE_H__
#define __EVENT_SERVICE_H__

#include "sys_inc.h"
#include "event_worker.h"

typedef struct __event_tpool__
{
	event_worker_t *workers;
	int worker_size;
	int working_count;
	unsigned int current_worker;
	
	int job_wait_count;		//等待处理的数量
	int job_handling_count;	//正在处理的数量
	pthread_mutex_t lock;
} event_tpool_t;

bool event_service_init(
		int worker_size, unsigned int worker_queue_size);

void event_service_destroy();
int event_service_get_working_count();
int event_service_get_job_wait_count();
int event_service_get_job_handling_count();
void event_service_working_count(int c);
void event_service_job_wait_count(int c);
void event_service_job_handling_count(int c);

bool event_service_distribute_job(
		void *arg, 
		void (*free_arg_cb)(void *), 
		unsigned int timer_out, 
		executor_cb executor_cb, 
		event_cb_fn timer_cb, 
		bufferevent_data_cb read_cb, 
		bufferevent_data_cb write_cb, 
	    bufferevent_event_cb error_cb);

#endif


