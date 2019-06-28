#include "sys_inc.h"
#include "event_executor.h"
#include "log_trace.h"
#include "user_time.h"

extern struct bufferevent *bufferevent_openssl_socket_new(struct event_base *base,
    evutil_socket_t fd, struct ssl_st *ssl, enum bufferevent_ssl_state state, int options);

static inline void _event_executor_free(event_executor_t *executor)
{
	if (executor)
	{
		event_del(&executor->event_timer);
		if (executor->event_buf.buf_ev)
		{
			bufferevent_disable(executor->event_buf.buf_ev, EV_READ);
            bufferevent_disable(executor->event_buf.buf_ev, EV_WRITE);
			bufferevent_free(executor->event_buf.buf_ev);
			executor->event_buf.buf_ev = NULL;
		}

		if (executor->event_buf.input_buffer)
		{
			evbuffer_free(executor->event_buf.input_buffer);
			executor->event_buf.input_buffer = NULL;
		}
		if (executor->event_buf.output_buffer)
		{
			evbuffer_free(executor->event_buf.output_buffer);
			executor->event_buf.output_buffer = NULL;
		}

		if (executor->free_cb)
		{
			executor->free_cb(executor->arg);
			executor->free_cb = NULL;
		}

		free(executor);
	}
}

void event_executor_release(event_executor_t *executor)
{
	_event_executor_free(executor);
}

event_executor_t *event_executor_new(
	struct event_base *evbase, 
	event_worker_t *worker, 
	int fd, 
	SSL_CTX *ctx, 
    bool is_sslser, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int timer_out, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb, 
    bufferevent_event_cb error_cb)
{
	struct timeval tv;
	event_executor_t *executor = NULL;

	if (arg && evbase)
	{
		executor = calloc(sizeof(event_executor_t), 1);
		if (executor == NULL)
		{
			LOG_TRACE_PERROR("calloc error!\n");
			return NULL;
		}

		executor->fd = fd;
		executor->arg = arg;
		executor->free_cb = free_arg_cb;

		executor->worker = worker;
		if ((executor->event_buf.input_buffer = evbuffer_new()) == NULL)
		{
	        LOG_TRACE_NORMAL("executor input buffer allocation failed!\n");
	        _event_executor_free(executor);
	        return NULL;
	    }

	    if ((executor->event_buf.output_buffer = evbuffer_new()) == NULL)
	    {
	        LOG_TRACE_NORMAL("executor output buffer allocation failed!\n");
	        _event_executor_free(executor);
	        return NULL;
    	}

		if (timer_out > 0 
			|| timer_cb)
		{
			executor->timeout = timer_out;
			executor->time_count = SYSTEM_SEC;

			if (timer_cb 
				&& event_assign(&executor->event_timer, 
					evbase, executor->fd, 
					EV_PERSIST, 
					timer_cb, executor) < 0)
			{
				LOG_TRACE_NORMAL("event_assign error!\n");
				_event_executor_free(executor);
				return NULL;
			}

			evutil_timerclear(&tv);
			tv.tv_sec = 1;
			if (event_add(&executor->event_timer, &tv) < 0)
			{
		        LOG_TRACE_NORMAL("event_add failed!\n");
		        _event_executor_free(executor);
		        return NULL;
			}
		}

		if (ctx == NULL)
		{
			executor->event_buf.buf_ev = bufferevent_socket_new(
					evbase, fd, BEV_OPT_CLOSE_ON_FREE);
			if (executor->event_buf.buf_ev == NULL) {
				LOG_TRACE_NORMAL("bufferevent_socket_new failed!\n");
				_event_executor_free(executor);
				return NULL;
			}
		}
		else
		{
            SSL *ssl = SSL_new(ctx);
            if (ssl == NULL)
            {
                LOG_TRACE_NORMAL("SSL_new error !\n");
                _event_executor_free(executor);
                return NULL;
            }
			executor->event_buf.buf_ev = bufferevent_openssl_socket_new(
					evbase, fd, ssl, 
					is_sslser ? BUFFEREVENT_SSL_ACCEPTING 
						: BUFFEREVENT_SSL_CONNECTING, 
                    BEV_OPT_CLOSE_ON_FREE);
			if (executor->event_buf.buf_ev == NULL) {
				LOG_TRACE_NORMAL("bufferevent_openssl_socket_new failed!\n");
                SSL_free(ssl);
				_event_executor_free(executor);
				return NULL;
			}
		}

		bufferevent_setcb(executor->event_buf.buf_ev, 
					read_cb, 
					write_cb,
					error_cb, executor);
		
		if (bufferevent_enable(executor->event_buf.buf_ev, EV_READ /* | EV_WRITE*/) < 0)
		{
			LOG_TRACE_NORMAL("enable bufferevent failed!\n");
			_event_executor_free(executor);
			return NULL;
		}

		executor->evbase = evbase;
	}

	return executor;
}

bool event_send_data(event_buf_t *event_buf, 
		char *user_data, int data_len)
{
	if (user_data 
		&& data_len > 0 
		&& event_buf 
		&& event_buf->buf_ev 
		&& event_buf->output_buffer)
	{
		if (evbuffer_add(event_buf->output_buffer, user_data, data_len) < 0)
		{
			LOG_TRACE_NORMAL("evbuffer_add error!\n");
			return false;
		}
		if (bufferevent_write_buffer(
				event_buf->buf_ev, event_buf->output_buffer) < 0)
		{
			LOG_TRACE_NORMAL("bufferevent_write_buffer error!\n");
			return false;
		}

		return true;
	}

	return false;
}

int event_recv_data(event_buf_t *event_buf, 
		char *user_buf, int buf_size)
{
	int len = 0;
	
	if (user_buf 
		&& buf_size > 0 
		&& event_buf 
		&& event_buf->buf_ev 
		&& event_buf->input_buffer)
	{
		bufferevent_read_buffer(event_buf->buf_ev, event_buf->input_buffer);
		len = evbuffer_get_length(event_buf->input_buffer);
		if (len > 0)
		{
			len = evbuffer_remove(event_buf->input_buffer, user_buf, MIN(len, buf_size));
		}
	}

	return len;
}

