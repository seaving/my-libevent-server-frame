#include "includes.h"

static SSL_CTX *_global_ctx  = NULL;

static bool _conn_success_cb(evexecutor_t executor, event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_conn_success_cb\n");

	return true;
}

static bool _talk_timer_cb(evexecutor_t executor, event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_timer_cb\n");

	probe_req_t probe_req = {
		.test = "test"
	};

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	//每隔20秒发送一次探测
	if (cli_phone->time_cnt % 20 == 0)
	{
		protocol_probe_send_request(event_buf, &probe_req);
	}

	//计时
	cli_phone->time_cnt ++;

	return true;
}

static bool _talk_read_cb(evexecutor_t executor, event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_read_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	if (cli_phone_talk(event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
	{
		return false;
	}

	return true;
}

static bool _talk_write_cb(evexecutor_t executor, event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_write_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	if (cli_phone_talk(event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
	{
		return false;
	}

	return true;
}

static bool _talk_error_cb(evexecutor_t executor, event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_error_cb\n");
	return true;
}

bool global_ctx_init()
{
	_global_ctx = event_ssl_client_init(
			DEFAULT_CA_FILE, 
			DEFAULT_CRT_FILE, DEFAULT_KEY_FILE);

	return _global_ctx != NULL;
}

bool connect_server()
{
    cli_phone_t *cli_phone = cli_phone_create();
    if (cli_phone == NULL)
    {
        return false;
    }

	return event_connect_distribute_job(_global_ctx, 
		    SCHEDULE_SERVER_HOST, 
		    SCHEDULE_SERVER_PORT, 
		    60, cli_phone, 
		    cli_phone_free, 
		    _conn_success_cb, 
		    _talk_timer_cb, 
		    _talk_read_cb, 
		    _talk_write_cb, 
		    _talk_error_cb);
}

