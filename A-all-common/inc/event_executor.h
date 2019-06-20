#ifndef __EVENT_EXECUTOR_H__
#define __EVENT_EXECUTOR_H__

#include "sys_inc.h"
#include "event_worker.h"


typedef void (*free_arg_cb)(void *);

typedef struct __event_buf__
{
	struct bufferevent *buf_ev;
	struct evbuffer *input_buffer;
    struct evbuffer *output_buffer;
} event_buf_t;

typedef struct __event_exeutor__ {
	int fd;
    unsigned int time_count;
    unsigned int timeout;
    struct event_base *evbase;
    struct event	event_timer;
	event_buf_t		event_buf;
    event_worker_t *worker;
    void *arg;
    free_arg_cb free_cb;
} event_executor_t;

typedef intptr_t evexecutor_t;

void evserver_executor_exit(evexecutor_t executor);
void event_executor_release(event_executor_t *executor);

event_executor_t *event_executor_new(
	struct event_base *evbase, 
	event_worker_t *worker, 
	int fd, 
    SSL_CTX *ctx, 
    bool is_sslser, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int timer_out, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb,
    bufferevent_event_cb error_cb);

bool event_send_data(event_buf_t *event_buf, char *user_data, int data_len);
int event_recv_data(event_buf_t *event_buf, char *user_buf, int buf_size);

#endif


