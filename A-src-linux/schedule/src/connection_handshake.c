#include "includes.h"

#if 0
event_cb_fn timer_cb, 
bufferevent_data_cb read_cb, 
bufferevent_data_cb write_cb, 
bufferevent_event_cb error_cb

static void _ev_conn_handshake_timer_cb(evutil_socket_t fd, short ev, void *arg)
{
	int timeout = 0;
	int client_fd = -1;

	client_t *client = (client_t *) arg;

	//LOG_TRACE_NORMAL("cb timer.\n");
	if (client)
	{
		if (client->time_count <= 0)
		{
			client->time_count = SYSTEM_SEC;
		}
		
		if (SYSTEM_SEC - client->time_count > client->timeout)
		{
			timeout = client->timeout;
			client_fd = client->fd;
			
			evserver_client_exit(client);
#if defined(SERVER_THREAD_POOL_ENABLE)
			evserver_tpool_client_handling_count(-1);
#endif
#if defined(SERVER_THREAD_POOL_ENABLE)
			LOG_TRACE_NORMAL("client timeout(%d sec) and stoped "
				"client event from event base [client_fd: %d].\n", 
					timeout, client_fd);
#else
			LOG_TRACE_NORMAL("client timeout(%d sec) and stoped "
				"event base for client [client_fd: %d].\n", 
					timeout, client_fd);
#endif
		}
	}
}
#endif

