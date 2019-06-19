#include "sys_inc.h"
#include "log_trace.h"
#include "event_server.h"
#include "socket.h"
#include "event_server_cb.h"

static inline void _event_server_destroy(server_t *server);

static int _event_server_socket_create(int server_port, int listen_count)
{
	int listen_fd = socket_listen(NULL, server_port, listen_count);
	if (listen_fd > 0)
	{
		if (evutil_make_listen_socket_reuseable(listen_fd) < 0)
		{
			LOG_TRACE_NORMAL("evutil_make_listen_socket_reuseable error!\n");
			socket_close(listen_fd);
			return -1;
		}

	    if (evutil_make_socket_nonblocking(listen_fd) < 0)
	    {
			LOG_TRACE_NORMAL("evutil_make_socket_nonblocking error!\n");
			socket_close(listen_fd);
			return -1;
	    }
	}
	
    return listen_fd;
}

static bool _event_server_create(server_t *server)
{
	if (server == NULL
		|| server->accept_cb == NULL)
	{
		return false;
	}
	
	server->evbase_server = event_base_new();
	if (server->evbase_server == NULL)
	{
		LOG_TRACE_NORMAL("event_base_new error!\n");
		return false;
	}

	server->listen_fd = _event_server_socket_create(
		server->listen_port, server->listen_count);
	if (server->listen_fd < 0)
	{
		_event_server_destroy(server);
		return false;
	}

	if (event_assign(&server->event_accept, 
			server->evbase_server, 
			server->listen_fd, 
			EV_READ | EV_PERSIST, 
			server->accept_cb, server) < 0)
	{
		LOG_TRACE_NORMAL("event_assign error!\n");
		_event_server_destroy(server);
		return false;
	}

	if (event_add(&server->event_accept, NULL) < 0)
	{
		LOG_TRACE_NORMAL("event_add error!\n");
		_event_server_destroy(server);
		return false;
	}

	return true;
}

static inline void _event_server_destroy(server_t *server)
{	
	if (server && server->evbase_server)
	{
		event_del(&server->event_accept);
		event_base_loopexit(server->evbase_server, NULL);
		event_base_free(server->evbase_server);
		shutdown(server->listen_fd, SHUT_RDWR);
		socket_close(server->listen_fd);
		server->listen_fd = -1;
		server->evbase_server = NULL;
		server->server_ok = false;
		server->client_cnt = 0;
		if (server->ctx)
		{
			SSL_CTX_free(server->ctx);
			server->ctx = NULL;
		}
		pthread_spin_destroy(&server->lock);
	}
}

static inline void *_event_server_factory(void *arg)
{
	pthread_detach(pthread_self());

	server_t *server = (server_t *) arg;
	if (server && server->evbase_server)
	{
		server->server_ok = true;
		event_base_dispatch(server->evbase_server);
		server->server_ok = false;
	}

	return NULL;
}

bool event_server_create(
		SSL_CTX *ctx, 
		server_t *server, 
		unsigned int io_timeout, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
	    connect_error_cb error_cb, 
	    talk_user_data_init_cb user_data_init_cb)
{
	pthread_t tid;
	if (server == NULL)
	{
		return false;
	}

	server->ctx = ctx;
	server->io_timeout = io_timeout;
	server->timer_cb = timer_cb;
	server->read_cb = read_cb;
	server->write_cb = write_cb;
	server->error_cb = server->error_cb;
	server->user_data_init_cb = user_data_init_cb;
	server->accept_cb = ctx ? event_server_ssl_accept_cb : event_server_accept_cb;

	pthread_spin_init(&server->lock, PTHREAD_PROCESS_PRIVATE);

	if (_event_server_create(server) == false)
	{
		_event_server_destroy(server);
		return false;
	}

	if (pthread_create(&tid, NULL, 
			_event_server_factory, server) != 0)
	{
		LOG_TRACE_PERROR("pthread_create error!\n");
		_event_server_destroy(server);
		return false;
	}

	return true;
}

void event_server_destroy(server_t *server)
{	
	_event_server_destroy(server);
}

bool event_server_status_ok(server_t *server)
{
	if (server == NULL)
	{
		return false;
	}

	return server->server_ok;
}

void event_server_client_count(server_t *server, int c)
{
	if (server)
	{
		pthread_spin_lock(&server->lock);
		server->client_cnt += c;
		pthread_spin_unlock(&server->lock);
	}
}

int event_server_client_get_counts(server_t *server)
{
	if (server == NULL)
	{
		return 0;
	}
	
	return server->client_cnt;
}

server_talk_t *event_server_talk_create(server_t *server, int conn_fd)
{
	server_talk_t *talk = NULL;
	if (conn_fd <= 0 
		|| server == NULL)
	{
		return NULL;
	}

	talk = calloc(sizeof(server_talk_t), 1);
	if (talk == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return NULL;
	}

	talk->conn_fd = conn_fd;
	if (server->user_data_init_cb)
	{
		server->user_data_init_cb(&talk->user_data);
	}

	talk->server = server;

	return talk;
}

void event_server_talk_free(void *arg)
{
	server_talk_t *talk = (server_talk_t *) arg;
	if (talk)
	{
		if (talk->ssl)
		{
			SSL_free(talk->ssl);
			talk->ssl = NULL;
		}

		socket_close(talk->conn_fd);
        event_server_client_count(talk->server, -1);

		if (talk->user_data.free_arg)
		{
			talk->user_data.free_arg(talk->user_data.arg);
		}

		talk->user_data.free_arg = NULL;
		talk->user_data.arg = NULL;
		free(talk);
	}
}

