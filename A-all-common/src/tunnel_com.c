#include "sys_inc.h"
#include "log_trace.h"
#include "frame.h"
#include "fifo.h"

#include "event_connect.h"
#include "socket.h"
#include "event_executor.h"
#include "user_time.h"
#include "event_service.h"
#if 0
//-----------------------------------------------------------------------------------------------------------------
#define TUNNEL_COM_FRAME_BUFF_SIZE		2 * 1024
static unsigned char _tunnel_com_buffer[TUNNEL_COM_FRAME_BUFF_SIZE];
static fifo_t _tunnel_com_fifo;
//-----------------------------------------------------------------------------------------------------------------
static inline bool _tunnel_com_fifo_init(
	fifo_t *fifo, unsigned char *buff, int buffsize)
{
	int size = 0;

	if (fifo == NULL 
		|| buff == NULL 
		|| buffsize <= 0)
	{
		return false;
	}

	memset(buff, 0, buffsize);
	size = fifo_cal_buffer_size(buffsize);
	fifo_create_by_array(fifo, "", buff, size);
	fifo_enable(fifo);

	return true;
}

static void _evtunnel_com_timer_cb(evutil_socket_t fd, short ev, void *arg)
{
	int timeout = 0;
	int client_fd = -1;

	client_t *client = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb timer.\n");
	if (executor && executor->arg)
	{
		client = (client_t *) executor->arg;
		if (executor->time_count <= 0)
		{
			executor->time_count = SYSTEM_SEC;
		}
		
		if (SYSTEM_SEC - executor->time_count > executor->timeout)
		{
			timeout = executor->timeout;
			client_fd = client->conn_fd;
			
			event_executor_release(executor);
			event_service_job_handling_count(-1);
			LOG_TRACE_NORMAL("client timeout(%d sec) and stoped "
				"client event from event base [client_fd: %d].\n", 
					timeout, client_fd);
		}
	}
}

static void _evtunnel_com_buffered_read_cb(struct bufferevent *bev, void *arg)
{
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb read.\n");
	
	if (bev && executor 
		&& executor->input_buffer 
		&& executor->output_buffer)
	{
		executor->time_count = SYSTEM_SEC;
#if 0
		if (client_talk(executor) == E_CLIENT_TALK_FINISHED)
		{
			event_executor_release(executor);
			event_service_job_handling_count(-1);
		}
		else
		{
			//executor->time_count = SYSTEM_SEC;
		}
#endif
	}
}

static void _evtunnel_com_buffered_write_cb(struct bufferevent *bev, void *arg)
{
	event_executor_t *executor = (event_executor_t *) arg;

	//LOG_TRACE_NORMAL("cb write.\n");
	
	if (bev && executor 
		&& executor->input_buffer 
		&& executor->output_buffer)
	{
		executor->time_count = SYSTEM_SEC;
#if 0
		if (client_talk(executor) == E_CLIENT_TALK_FINISHED)
		{
			event_executor_release(executor);
			event_service_job_handling_count(-1);

		}
		else
		{
			//executor->time_count = SYSTEM_SEC;
		}
#endif
	}
}

static void _evtunnel_com_buffered_event_cb(struct bufferevent *bev, short what, void *arg)
{
	int client_fd = -1;
	client_t *client = NULL;
	event_executor_t *executor = (event_executor_t *) arg;

	if (executor && executor->arg)
	{
		client = (client_t *) executor->arg;
		if (what & BEV_EVENT_CONNECTED)
		{
			LOG_TRACE_NORMAL("connect [domain = %s][ip = %s]:[port = %d] success.\n", 
				client->domain, client->conn_ip, client->conn_port);
		}
		else
		{
			LOG_TRACE_NORMAL("cb error.\n");
			client_fd = client->conn_fd;
			event_executor_release(executor);
			event_service_job_handling_count(-1);
			LOG_TRACE_NORMAL("stoped client event from event base [client_fd: %d].\n", 
						client_fd);
		}
	}
}

bool tunnel_com_init(char *host, int port, unsigned int timer_out)
{
	if (host == NULL 
		|| port <= 0)
	{
		return false;
	}

	if (_tunnel_com_fifo_init(&_tunnel_com_fifo, 
			_tunnel_com_buffer, sizeof(_tunnel_com_buffer)) == false)
	{
		LOG_TRACE_NORMAL("_tunnel_com_fifo_init error!\n");
		return false;
	}

	return evdns_distribute_by_event_service(
				host, port, timer_out, 
				_evtunnel_com_timer_cb, 
				_evtunnel_com_buffered_read_cb, 
				_evtunnel_com_buffered_write_cb, 
				_evtunnel_com_buffered_event_cb);
}

#endif
