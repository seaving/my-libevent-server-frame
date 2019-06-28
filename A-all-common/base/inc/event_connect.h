#ifndef __EVENT_CONNECT_H__
#define __EVENT_CONNECT_H__

#include "event_worker.h"
#include "event_executor.h"

typedef bool (*connect_timer_cb)(event_buf_t *event_buf, short what, int conn_fd, void *ctx);
typedef bool (*connect_success_cb)(event_buf_t *event_buf, int conn_fd, void *ctx);
typedef bool (*connect_error_cb)(event_buf_t *event_buf, short what, int conn_fd, void *ctx);
typedef bool (*connect_io_cb)(event_buf_t *event_buf, int conn_fd, void *ctx);

typedef enum __connect_type__
{
	E_CONNECT_TYPE_SOCKET_SERVER = 0,
	E_CONNECT_TYPE_LOCALSOCKET_SERVER,
} connect_type_t;

typedef struct __connect_address__
{
	char *domain;
	char *conn_ip;
	int conn_port;
} connect_addr_t;

typedef struct __connect_local_socket_address__
{
	char *name;
	int namespace;
} connect_local_socket_addr_t;

typedef struct __connect__
{
	int conn_fd;
	bool connect_ok;
	SSL_CTX *global_ctx;
	connect_addr_t sockaddr;
	connect_local_socket_addr_t localsockaddr;
	connect_type_t connect_type;
	unsigned int timer_out;
	void *cb_arg;
	void (*free_cb_arg)(void *);
	connect_success_cb success_cb;
	connect_timer_cb timer_cb;
	connect_io_cb read_cb;
	connect_io_cb write_cb;
	connect_error_cb error_cb;
} connect_t;

bool event_connect_distribute_job(
		SSL_CTX *global_ctx, 
		char *server_host, 
		int server_port, 
		unsigned int io_timeout, 
		void *cb_arg, 
		void (*free_cb_arg)(void *), 
		connect_success_cb success_cb, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
		connect_error_cb error_cb);

bool event_connect_local_socket_distribute_job(
		SSL_CTX *global_ctx, 
		int namespace, 
		char *name, 
		unsigned int io_timeout, 
		void *cb_arg, 
		void (*free_cb_arg)(void *), 
		connect_success_cb success_cb, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
		connect_error_cb error_cb);

#endif


