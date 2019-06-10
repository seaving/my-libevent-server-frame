#include "includes.h"

static bool _conn_success_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_conn_success_cb\n");
    return true;
}

static bool _talk_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_timer_cb\n");
	char buf[1024] = {0};
	char context[1024] = {0};

	snprintf(context, sizeof(context) - 1, 
		"{\"task\": \"test\"}");

	snprintf(buf, sizeof(buf) - 1, 
			"X-Auth: brush-1990asdklw3323fyksdweb23 \r\n"
			"Content-Length: %d \r\n"
			"\r\n"
			"%s", 
			(int) strlen(context), 
			context);

	if (event_buf == NULL)
	{
		return false;
	}

	if (event_send_data(event_buf, buf, strlen(buf)) == false)
	{
		LOG_TRACE_NORMAL("event_send_data error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("send data to server: \n");
	LOG_TRACE_NORMAL("%s\n", buf);
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("\n");

	return true;
}

static bool _talk_read_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_read_cb\n");
	return true;
}

static bool _talk_write_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_write_cb\n");
	return true;
}

static bool _talk_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("_talk_error_cb\n");
	return true;
}

bool connect_server()
{
	return event_connect_distribute_job(
		    SCHEDULE_SERVER_HOST, 
		    SCHEDULE_SERVER_PORT, 
		    60, NULL, 
		    _conn_success_cb, 
		    _talk_timer_cb, 
		    _talk_read_cb, 
		    _talk_write_cb, 
		    _talk_error_cb);
}

