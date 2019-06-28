#ifndef __EVENT_WORKER_H__
#define __EVENT_WORKER_H__

#include "sys_inc.h"
#include "queue.h"


typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);
typedef void (*event_cb_fn)(evutil_socket_t, short, void *);
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);
typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);

typedef void (*worker_cb_fn)(void *);

typedef struct __event_worker__
{
	pthread_t tid;
	int read_fd;
    int write_fd;
    int connect_fd;
	struct event_base *evbase;
	struct evdns_base* evdnsbase;
	struct event event;
	int tpool_index;
	void *tpool;
	
	queue_t *queue;

	bool create_by_malloc;

	pthread_mutex_t lock;
} event_worker_t;

typedef bool (*executor_cb)(
	struct event_base *, event_worker_t *, void *, void (*free_arg_cb)(void *), unsigned int, 
	event_cb_fn, bufferevent_data_cb, bufferevent_data_cb, bufferevent_event_cb);

typedef struct __event_worker_job__
{
	void *arg;
	bool processing;
	void (*free_arg_cb)(void *);
	unsigned int timer_out;
	executor_cb executor_cb;
	event_cb_fn timer_cb;
	bufferevent_data_cb read_cb;
	bufferevent_data_cb write_cb;
	bufferevent_event_cb error_cb;
	worker_cb_fn worker_cb;
} event_worker_job_t;


bool event_worker_init(event_worker_t *worker, unsigned int queue_size);
event_worker_t *event_worker_new(unsigned int fifo_size);
void event_worker_free(event_worker_t *worker);
void *event_worker_factory(void *arg);

bool event_worker_add_job(
		event_worker_t *worker, 
		void *arg, 
		void (*free_arg_cb)(void *), 
		unsigned int timer_out, 
		executor_cb executor_cb, 
		event_cb_fn timer_cb, 
		bufferevent_data_cb read_cb, 
		bufferevent_data_cb write_cb,
	    bufferevent_event_cb error_cb);

#endif


