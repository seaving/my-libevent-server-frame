#include "sys_inc.h"
#include "log_trace.h"
#include "socket.h"
#include "event_executor.h"
#include "event_service.h"
#include "event_server.h"
#include "event_server_cb.h"

static void _event_server_timer_cb(evutil_socket_t fd, short ev, void *arg)
{
	int timeout = 0;
	int client_fd = -1;

	server_talk_t *talk = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb timer.\n");
	if (executor && executor->arg)
	{
		talk = (server_talk_t *) executor->arg;
		if (executor->time_count <= 0)
		{
			executor->time_count = SYSTEM_SEC;
		}

		if (talk && talk->server 
			&& talk->server->timer_cb)
		{
			if (talk->server->timer_cb((evexecutor_t) executor, &executor->event_buf, 
					ev, talk->conn_fd, talk->user_data.arg) == false)
			{
			    client_fd = talk->conn_fd;
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							client_fd);
				return;
			}
		}

		if (SYSTEM_SEC - executor->time_count > executor->timeout)
		{
			timeout = executor->timeout;
			client_fd = talk->conn_fd;

			event_executor_release(executor);
			event_service_job_handling_count(-1);
			LOG_TRACE_NORMAL("client timeout(%d sec) and stoped "
				"client event from event base [client_fd: %d].\n", 
					timeout, client_fd);
		}
	}
}

static void _event_server_buffered_read_cb(struct bufferevent *bev, void *arg)
{
    int conn_fd = -1;
	server_talk_t *talk = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb read.\n");
	
	if (bev && executor 
		&& executor->event_buf.input_buffer 
		&& executor->event_buf.output_buffer)
	{
		executor->time_count = SYSTEM_SEC;
		talk = (server_talk_t *) executor->arg;
		if (talk && talk->server 
			&& talk->server->read_cb)
		{
			if (talk->server->read_cb((evexecutor_t) executor, &executor->event_buf, 
					talk->conn_fd, talk->user_data.arg) == false)
			{
			    conn_fd = talk->conn_fd;
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							conn_fd);
				return;
			}
		}
		executor->time_count = SYSTEM_SEC;
	}
}

static void _event_server_buffered_write_cb(struct bufferevent *bev, void *arg)
{
    int conn_fd = -1;
	server_talk_t *talk = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb write.\n");
	
	if (bev && executor 
		&& executor->event_buf.input_buffer 
		&& executor->event_buf.output_buffer)
	{
	    executor->time_count = SYSTEM_SEC;
		talk = (server_talk_t *) executor->arg;
		if (talk && talk->server 
			&& talk->server->write_cb)
		{
			if (talk->server->write_cb((evexecutor_t) executor, &executor->event_buf, 
					talk->conn_fd, talk->user_data.arg) == false)
			{
			    conn_fd = talk->conn_fd;
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							conn_fd);
				return;
			}
		}
		executor->time_count = SYSTEM_SEC;
	}
}

static void _event_server_buffered_event_cb(struct bufferevent *bev, short what, void *arg)
{
	int error = -1;
	int client_fd = -1;
	server_talk_t *talk = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	LOG_TRACE_NORMAL("cb error.\n");

	talk = (server_talk_t *) executor->arg;

	if (executor && executor->arg)
	{
		if (what & BEV_EVENT_CONNECTED)
		{
			LOG_TRACE_NORMAL("SSL Client connect success [client_fd: %d].\n", talk->conn_fd);
			return;
		}

		error = EVUTIL_SOCKET_ERROR();
		client_fd = talk->conn_fd;
		if (talk->server && talk->server->error_cb)
		{
			talk->server->error_cb((evexecutor_t) executor, &executor->event_buf, what, client_fd, talk->user_data.arg);
		}

		event_executor_release(executor);
		event_service_job_handling_count(-1);
		LOG_TRACE_NORMAL("error(%d): %s, stoped client event from event base [client_fd: %d].\n", 
					error, evutil_socket_error_to_string(error), client_fd);
	}
}

static bool _event_server_executor(
	struct event_base *evbase, 
	event_worker_t *worker, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int io_timeout, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb,
    bufferevent_event_cb error_cb)
{
	event_executor_t *executor = NULL;
	server_talk_t *talk = (server_talk_t *) arg;
	if (talk == NULL)
	{
		return false;
	}

	executor = event_executor_new(evbase, worker, talk->conn_fd, talk->ssl, talk, 
					free_arg_cb, io_timeout, timer_cb, read_cb, 
					write_cb, error_cb);
	if (executor == NULL)
	{
		return false;
	}

	return true;
}

void event_server_accept_cb(evutil_socket_t listen_fd, short ev, void *arg)
{
	int client_fd = -1;
	struct sockaddr_in client_addr;
	server_t *server = (server_t *) arg;
	socklen_t client_len = sizeof(client_addr);
	server_talk_t *talk = NULL;
	if (server == NULL)
	{
		LOG_TRACE_NORMAL("the arg of server is null!\n");
		socket_close(listen_fd);
		return;
	}

	client_fd = accept(listen_fd, 
					(struct sockaddr *) &client_addr, 
					&client_len);
	if (client_fd < 0)
	{
		LOG_TRACE_PERROR("accept error!\n");
		event_server_destroy(server);
		return;
	}

	if (server->client_cnt > server->accept_max)
	{
		LOG_TRACE_NORMAL("connection denied. "
				"%d clients connected, max client counts: %d!\n", 
					server->client_cnt, server->accept_max);
		socket_close(client_fd);
		return;
	}

	if (evutil_make_socket_nonblocking(client_fd) < 0)
	{
		LOG_TRACE_NORMAL("failed to set client socket to non-blocking!\n");
		socket_close(client_fd);
		return;
	}

	talk = event_server_talk_create(server, client_fd);
	if (talk == NULL)
	{
		LOG_TRACE_NORMAL("event_server_talk_create error!\n");
		socket_close(client_fd);
		return;
	}

	if (event_service_distribute_job(
			talk, event_server_talk_free, 
			server->io_timeout, 
			_event_server_executor, 
			_event_server_timer_cb, 
			_event_server_buffered_read_cb, 
			_event_server_buffered_write_cb, 
			_event_server_buffered_event_cb) == false)
	{
		LOG_TRACE_NORMAL("event_service_distribute_job error!\n");
		socket_close(client_fd);
		return;
	}

    event_server_client_count(server, 1);
	LOG_TRACE_NORMAL("accept new client connection, fd = %d\n", client_fd);
}

void event_server_ssl_accept_cb(evutil_socket_t listen_fd, short ev, void *arg)
{
	int client_fd = -1;
	struct sockaddr_in client_addr;
	server_t *server = (server_t *) arg;
	socklen_t client_len = sizeof(client_addr);
	server_talk_t *talk = NULL;
	if (server == NULL 
		|| server->ctx == NULL)
	{
		LOG_TRACE_NORMAL("the arg of server is null!\n");
		socket_close(listen_fd);
		return;
	}

	client_fd = accept(listen_fd, 
					(struct sockaddr *) &client_addr, 
					&client_len);
	if (client_fd < 0)
	{
		LOG_TRACE_PERROR("accept error!\n");
		event_server_destroy(server);
		return;
	}

	if (server->client_cnt > server->accept_max)
	{
		LOG_TRACE_NORMAL("connection denied. "
				"%d clients connected, max client counts: %d!\n", 
					server->client_cnt, server->accept_max);
		socket_close(client_fd);
		return;
	}

	if (evutil_make_socket_nonblocking(client_fd) < 0)
	{
		LOG_TRACE_NORMAL("failed to set client socket to non-blocking!\n");
		socket_close(client_fd);
		return;
	}

	talk = event_server_talk_create(server, client_fd);
	if (talk == NULL)
	{
		LOG_TRACE_NORMAL("event_server_talk_create error!\n");
		socket_close(client_fd);
		return;
	}

	talk->ssl = SSL_new(server->ctx);
	if (talk->ssl == NULL)
	{
		LOG_TRACE_NORMAL("SSL_new error !\n");
		event_server_talk_free(talk);
		return;
	}

	if (event_service_distribute_job(
			talk, event_server_talk_free, 
			server->io_timeout, 
			_event_server_executor, 
			_event_server_timer_cb, 
			_event_server_buffered_read_cb, 
			_event_server_buffered_write_cb, 
			_event_server_buffered_event_cb) == false)
	{
		LOG_TRACE_NORMAL("event_service_distribute_job error!\n");
		socket_close(client_fd);
		return;
	}

    event_server_client_count(server, 1);
	LOG_TRACE_NORMAL("accept new client connection, fd = %d\n", client_fd);
}

