#include "sys_inc.h"
#include "log_trace.h"
#include "event_service.h"
#include "event_executor.h"
#include "event_connect.h"
#include "event_connect_cb.h"

void event_connect_timer_cb(evutil_socket_t fd, short ev, void *arg)
{
	int timeout = 0;
	int client_fd = -1;

	connect_t *connect = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb timer.\n");
	if (executor && executor->arg)
	{
		connect = (connect_t *) executor->arg;
		if (executor->time_count <= 0)
		{
			executor->time_count = SYSTEM_SEC;
		}

		if (connect->timer_cb && connect->connect_ok)
		{
			if (connect->timer_cb(&executor->event_buf, 
					ev, connect->conn_fd, connect->cb_arg) == false)
			{
				client_fd = connect->conn_fd;
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							client_fd);
				return;
			}
		}

		if (executor->timeout > 0 
			&& SYSTEM_SEC - executor->time_count 
					> executor->timeout)
		{
			timeout = executor->timeout;
			client_fd = connect->conn_fd;

			event_executor_release(executor);
			event_service_job_handling_count(-1);
			LOG_TRACE_NORMAL("client timeout(%d sec) and stoped "
				"client event from event base [client_fd: %d].\n", 
					timeout, client_fd);
		}
	}
}

void event_connect_buffered_read_cb(struct bufferevent *bev, void *arg)
{
	connect_t *connect = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb read.\n");
	
	if (bev && executor 
		&& executor->event_buf.input_buffer 
		&& executor->event_buf.output_buffer)
	{
		executor->time_count = SYSTEM_SEC;
		connect = (connect_t *) executor->arg;
		if (connect 
			&& connect->read_cb)
		{
			if (connect->read_cb(&executor->event_buf, 
					connect->conn_fd, connect->cb_arg) == false)
			{
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							connect->conn_fd);
				return;
			}
		}
		executor->time_count = SYSTEM_SEC;
	}
}

void event_connect_buffered_write_cb(struct bufferevent *bev, void *arg)
{
	connect_t *connect = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb write.\n");
	
	if (bev && executor 
		&& executor->event_buf.input_buffer 
		&& executor->event_buf.output_buffer)
	{
		connect = (connect_t *) executor->arg;
		if (connect 
			&& connect->write_cb)
		{
			if (connect->write_cb(&executor->event_buf, 
					connect->conn_fd, connect->cb_arg) == false)
			{
				event_executor_release(executor);
				event_service_job_handling_count(-1);
				LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
							connect->conn_fd);
				return;
			}
		}
		//executor->time_count = SYSTEM_SEC;
	}
}

void event_connect_buffered_event_cb(struct bufferevent *bev, short what, void *arg)
{
	int error = -1;
	int client_fd = -1;
	connect_t *connect = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	LOG_TRACE_NORMAL("cb event.\n");

	if (executor && executor->arg)
	{
		connect = (connect_t *) executor->arg;
		if (what & BEV_EVENT_CONNECTED)
		{
			connect->connect_ok = true;
			if (connect->connect_type == E_CONNECT_TYPE_SOCKET_SERVER)
			{
				LOG_TRACE_NORMAL("connect %s%s%s%s:%d success.\n", 
					connect->sockaddr.domain ? "(" : "", 
					connect->sockaddr.domain ? connect->sockaddr.domain : "", 
					connect->sockaddr.domain ? ")" : "", 
					connect->sockaddr.conn_ip, connect->sockaddr.conn_port);
			}
			else if (connect->connect_type == E_CONNECT_TYPE_LOCALSOCKET_SERVER)
			{
				LOG_TRACE_NORMAL("connect local server namespace: %d name: %s success.\n", 
					connect->localsockaddr.namespace, connect->localsockaddr.name);
			}

			if (connect->success_cb)
			{
				connect->success_cb(&executor->event_buf, connect->conn_fd, connect->cb_arg);
			}
		}
		else if (what & BUFFEREVENT_SSL_OPEN)
		{
			LOG_TRACE_NORMAL("SSL connect success [client_fd: %d].\n", connect->conn_fd);
		}
		else
		{
			LOG_TRACE_NORMAL("cb error.\n");
			error = EVUTIL_SOCKET_ERROR();
			client_fd = connect->conn_fd;
			if (connect->error_cb)
			{
				connect->error_cb(&executor->event_buf, what, client_fd, connect->cb_arg);
			}

			event_executor_release(executor);
			event_service_job_handling_count(-1);
			LOG_TRACE_NORMAL("error(%d): %s, stoped client event from event base [client_fd: %d].\n", 
						error, evutil_socket_error_to_string(error), client_fd);
		}
	}
}

