#include "includes.h"

static server_t _schedule_server = {
	.listen_fd = -1,
	.evbase_server = NULL,
	.server_ok = false,
	.listen_port = SCHEDULE_SERVER_BIND_PORT,
	.listen_count = SCHEDULE_SERVER_LISTEN_QUEUE_SIZE,
	.accept_max = SCHEDULE_CLIENT_ACCEPT_MAX_COUNT,
	.client_cnt = 0,
};

static inline bool _create_schedule_server(SSL_CTX *ctx, server_t *server)
{
	if (ctx == NULL 
		|| server == NULL)
	{
		return false;
	}

	if (event_server_create(ctx, server, 
	        CLIENT_TIMEOUT_SEC, 
			schedule_server_timer_cb, 
			schedule_server_read_cb, 
			schedule_server_write_cb, 
			schedule_server_error_cb, 
			schedule_server_user_data_init_cb) == false)
	{
		LOG_TRACE_NORMAL("create server failt!\n");
		return false;
	}

	LOG_TRACE_NORMAL("create server success.\n");
	return true;
}

static inline bool _schedule_server_repeat_init(SSL_CTX *ctx, int repeat_count)
{
	int i;
	if (ctx == NULL)
	{
		return false;
	}

	for (i = 0; event_server_status_ok(&_schedule_server) == false 
			&& i < repeat_count; i ++)
	{
		LOG_TRACE_NORMAL("repeat init server ... %d\n", i);
		
		if (_create_schedule_server(ctx, &_schedule_server))
		{
			break;
		}

		sleep(1);
	}

	if (i >= repeat_count)
	{
		return false;
	}

	return true;
}

static inline void *_schedule_server_monitor_worker(void *arg)
{
	pthread_detach(pthread_self());

	SSL_CTX *ctx = (SSL_CTX *) arg;

	for ( ; ctx; )
	{
		if (_schedule_server_repeat_init(ctx, 30) == false)
		{
			LOG_TRACE_NORMAL("exit!\n");
			SSL_CTX_free(ctx);
			return NULL;
		}

		sleep(3);
	}

	return NULL;
}

bool schedule_server_start()
{
	pthread_t tid;

	SSL_CTX *ctx = event_ssl_server_init(DEFAULT_CA_CRT_FILE, 
		DEFAULT_SVR_CRT_FILE, DEFAULT_SVR_KEY_FILE);
	if (ctx == NULL)
	{
		return false;
	}

	if (pthread_create(&tid, NULL, 
			_schedule_server_monitor_worker, ctx) != 0)
	{
		LOG_TRACE_PERROR("pthread_create error!\n");
		SSL_CTX_free(ctx);
		return false;
	}

	return true;
}

int schedule_server_get_accept_client_cnt()
{
    return event_server_client_get_counts(&_schedule_server);
}
