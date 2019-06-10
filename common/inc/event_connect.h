#ifndef __EVENT_CONNECT_H__
#define __EVENT_CONNECT_H__

#include "event_worker.h"
#include "event_executor.h"

typedef bool (*connect_timer_cb)(event_buf_t *event_buf, short what, int conn_fd, void *ctx);
typedef bool (*connect_success_cb)(event_buf_t *event_buf, int conn_fd, void *ctx);
typedef bool (*connect_error_cb)(event_buf_t *event_buf, short what, int conn_fd, void *ctx);
typedef bool (*connect_io_cb)(event_buf_t *event_buf, int conn_fd, void *ctx);

typedef struct __connect_address__
{
	char *domain;
	char *conn_ip;
	int conn_port;
} connect_addr_t;

typedef struct __connect__
{
	int conn_fd;
	connect_addr_t addr;
	unsigned int timer_out;
	void *cb_arg;
	connect_success_cb success_cb;
	connect_timer_cb timer_cb;
	connect_io_cb read_cb;
	connect_io_cb write_cb;
	connect_error_cb error_cb;
} connect_t;

bool event_connect_distribute_job(
		char *server_host, 
		int server_port, 
		unsigned int io_timeout,
		void *cb_arg, 
		connect_success_cb success_cb, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
		connect_error_cb error_cb);

#endif


