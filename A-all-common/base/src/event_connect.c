#include "sys_inc.h"
#include "log_trace.h"
#include "socket.h"
#include "event_connect.h"
#include "event_executor.h"
#include "event_service.h"
#include "event_connect_cb.h"
#include "event_dns.h"
#include "socket_local.h"

static int _connect_socket_create(char *server_ip, int server_port)
{
	int fd = -1;
	struct sockaddr_in server_addr;
	if (server_ip == NULL 
		|| server_port <= 0)
	{
		return -1;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(server_port);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0)
	{
		LOG_TRACE_PERROR("socket error!\n");
		return -1;
	}

	if (evutil_make_socket_nonblocking(fd) < 0)
	{
		LOG_TRACE_NORMAL("evutil_make_socket_nonblocking error ! sock_fd = %d\n", fd);
		socket_close(fd);
		return -1;
	}

	return fd;
}

static bool _connect_addr_init(connect_t *connect, 
		char *domain, char *server_ip, int server_port)
{
	if (connect == NULL 
		|| server_ip == NULL 
		|| server_port <= 0)
	{
		return false;
	}

	if (domain && strlen(domain) > 0)
	{
		connect->sockaddr.domain = calloc(1, strlen(domain) + 1);
		if (connect->sockaddr.domain == NULL)
		{
			LOG_TRACE_PERROR("calloc error!\n");
			return false;
		}

		memcpy(connect->sockaddr.domain, domain, strlen(domain));
	}

	connect->sockaddr.conn_ip = calloc(1, strlen(server_ip) + 1);
	if (connect->sockaddr.conn_ip == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return false;
	}

	memcpy(connect->sockaddr.conn_ip, server_ip, strlen(server_ip));

	connect->sockaddr.conn_port = server_port;

	connect->conn_fd = _connect_socket_create(connect->sockaddr.conn_ip, connect->sockaddr.conn_port);
	if (connect->conn_fd <= 0)
	{
		return false;
	}

	return true;
}

static bool _connect_local_socket_addr_init(connect_t *connect)
{
	if (connect == NULL 
		|| connect->localsockaddr.name == NULL)
	{
		return false;
	}

	connect->conn_fd = socket_local_socket_create();
	if (connect->conn_fd <= 0)
	{
		return false;
	}

	if (evutil_make_socket_nonblocking(connect->conn_fd) < 0)
	{
		LOG_TRACE_NORMAL("evutil_make_socket_nonblocking error ! conn_fd = %d\n", connect->conn_fd);
		socket_close(connect->conn_fd);
		return -1;
	}

	return true;
}

static void _connect_free(void *arg)
{
	connect_t *connect = (connect_t *) arg;
	if (connect)
	{
		if (connect->conn_fd > 0)
		{
			close(connect->conn_fd);
		}

		if (connect->sockaddr.conn_ip)
		{
			free(connect->sockaddr.conn_ip);
		}

		if (connect->sockaddr.domain)
		{
			free(connect->sockaddr.domain);
		}

		if (connect->localsockaddr.name)
		{
			free(connect->localsockaddr.name);
		}

		if (connect->free_cb_arg)
		{
			connect->free_cb_arg(connect->cb_arg);
		}

		connect->connect_ok = false;

		free(connect);
	}
}

static bool _event_connect_executor(
	struct event_base *evbase, 
	event_worker_t *worker, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int timer_out, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb,
    bufferevent_event_cb error_cb)
{
	event_executor_t *executor = NULL;
	struct sockaddr_in server_addr;
	connect_t *connect = (connect_t *) arg;
	if (connect == NULL 
		|| connect->sockaddr.conn_ip == NULL 
		|| connect->sockaddr.conn_port <= 0)
	{
		return false;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(connect->sockaddr.conn_ip);
	server_addr.sin_port = htons(connect->sockaddr.conn_port);

	executor = event_executor_new(evbase, worker, connect->conn_fd, 
                    connect->global_ctx, false, connect, 
					free_arg_cb, timer_out, timer_cb, read_cb, write_cb, error_cb);
	if (executor == NULL)
	{
		return false;
	}

	LOG_TRACE_NORMAL("connect server: %s:%d\n", 
		connect->sockaddr.conn_ip, connect->sockaddr.conn_port);
	bufferevent_socket_connect(executor->event_buf.buf_ev, 
		(struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
	return true;
}

static bool _event_connect_local_socket_executor(
	struct event_base *evbase, 
	event_worker_t *worker, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int timer_out, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb,
	bufferevent_event_cb error_cb)
{
	event_executor_t *executor = NULL;

	struct sockaddr_un server_addr;
	socklen_t alen = 0;

	connect_t *connect = (connect_t *) arg;
	if (connect == NULL)
	{
		return false;
	}

	if (socket_make_sockaddr_un(connect->localsockaddr.name, 
			connect->localsockaddr.namespace, &server_addr, &alen) < 0)
	{
		LOG_TRACE_NORMAL("socket_make_sockaddr_un error ! "
			"namespace = %d, name = %s\n", 
			connect->localsockaddr.namespace, 
			connect->localsockaddr.name);
		return false;
	}

	executor = event_executor_new(evbase, worker, connect->conn_fd, 
					connect->global_ctx, false, connect, 
					free_arg_cb, timer_out, timer_cb, read_cb, write_cb, error_cb);
	if (executor == NULL)
	{
		return false;
	}

	LOG_TRACE_NORMAL("connect localsocket: namespace = %d, name = %s\n", 
			connect->localsockaddr.namespace, 
			connect->localsockaddr.name);
	bufferevent_socket_connect(executor->event_buf.buf_ev, 
		(struct sockaddr *) &server_addr, alen);
	return true;
}

static void _dns_finished_cb(char *domain, char *ip, void *arg)
{
	connect_t *connect = (connect_t *) arg;
	if (connect == NULL)
	{
		return;
	}

	if (_connect_addr_init(connect, domain, ip, 
			connect->sockaddr.conn_port) == false)
	{
		_connect_free(connect);
		return;
	}

	if (event_service_distribute_job(connect, _connect_free, connect->timer_out, 
			_event_connect_executor, event_connect_timer_cb, 
			event_connect_buffered_read_cb, event_connect_buffered_write_cb, 
			event_connect_buffered_event_cb) == false)
	{
		_connect_free(connect);
	}
}

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
		connect_error_cb error_cb)
{
	connect_t *connect = calloc(sizeof(connect_t), 1);
	if (connect == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return false;
	}

	connect->connect_type = E_CONNECT_TYPE_SOCKET_SERVER;

	connect->global_ctx = global_ctx;
	connect->sockaddr.conn_port = server_port;
	connect->timer_out = io_timeout;
	connect->cb_arg = cb_arg;
	connect->free_cb_arg = free_cb_arg;
	connect->success_cb = success_cb;
	connect->timer_cb = timer_cb;
	connect->read_cb = read_cb;
	connect->write_cb = write_cb;
	connect->error_cb = error_cb;

	if (event_dns_distribute_job(server_host, 
			_dns_finished_cb, connect) == false)
	{
		_connect_free(connect);
		return false;
	}

	return true;
}

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
		connect_error_cb error_cb)
{
	connect_t *connect = NULL;

	if (name == NULL)
	{
		return false;
	}

	connect = calloc(sizeof(connect_t), 1);
	if (connect == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return false;
	}

	connect->localsockaddr.name = strdup(name);
	connect->localsockaddr.namespace = namespace;

	connect->connect_type = E_CONNECT_TYPE_LOCALSOCKET_SERVER;

	connect->global_ctx = global_ctx;
	connect->timer_out = io_timeout;
	connect->cb_arg = cb_arg;
	connect->free_cb_arg = free_cb_arg;
	connect->success_cb = success_cb;
	connect->timer_cb = timer_cb;
	connect->read_cb = read_cb;
	connect->write_cb = write_cb;
	connect->error_cb = error_cb;

	if (_connect_local_socket_addr_init(connect) == false)
	{
		_connect_free(connect);
		return false;
	}

	if (event_service_distribute_job(connect, _connect_free, connect->timer_out, 
			_event_connect_local_socket_executor, event_connect_timer_cb, 
			event_connect_buffered_read_cb, event_connect_buffered_write_cb, 
			event_connect_buffered_event_cb) == false)
	{
		_connect_free(connect);
	}

	return true;
}

