#include "sys_inc.h"
#include "log_trace.h"
#include "event_server.h"
#include "socket.h"
#include "event_server_cb.h"
#include "socket_local.h"
#include "user_string.h"

static inline void _event_server_destroy(server_t *server);

static bool _event_server_socket_create(server_t *server)
{
    server->listen_fd = -1;
    switch (server->type)
    {
        case E_SERVER_TYPE_TCP:
            server->listen_fd = socket_listen(NULL, server->listen_port, server->listen_count);
            break;
        case E_SERVER_TYPE_UDP:
            break;
        case E_SERVER_TYPE_LOCAL:
            server->listen_fd = socket_local_server(server->localsockaddr.name, 
            		server->localsockaddr.namespace, server->listen_count);
            break;
        default:
            break;
    }

    if (server->listen_fd > 0)
	{
		if (evutil_make_listen_socket_reuseable(server->listen_fd) < 0)
		{
			LOG_TRACE_NORMAL("evutil_make_listen_socket_reuseable error!\n");
			socket_close(server->listen_fd);
			return false;
		}

	    if (evutil_make_socket_nonblocking(server->listen_fd) < 0)
	    {
			LOG_TRACE_NORMAL("evutil_make_socket_nonblocking error!\n");
			socket_close(server->listen_fd);
			return false;
	    }

        return true;
	}
	
    return false;
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

	if (_event_server_socket_create(server) == false)
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
			//SSL_CTX_free(server->ctx);
			server->ctx = NULL;
		}

		safe_free(server->localsockaddr.name);
		pthread_spin_destroy(&server->lock);
		pthread_mutexattr_destroy(&server->accept_mutexattr);
		pthread_mutex_destroy(&server->accept_lock);
	}
}

static inline void *_event_server_factory(void *arg)
{
	pthread_detach(pthread_self());

	int listen_fd = -1;

	server_t *server = (server_t *) arg;
	if (server && server->evbase_server)
	{
		listen_fd =  server->listen_fd;
		LOG_TRACE_NORMAL("server running success (listen_fd = %d) ...\n", listen_fd);
	
		server->server_ok = true;
		event_base_dispatch(server->evbase_server);
		server->server_ok = false;

		LOG_TRACE_NORMAL("server exit (listen_fd = %d) ...\n", listen_fd);
	}

	return NULL;
}

bool event_server_init(
		SSL_CTX *ctx, 
		server_t *server, 
		unsigned int io_timeout, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
	    connect_error_cb error_cb, 
	    talk_user_data_init_cb user_data_init_cb)
{
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

	pthread_mutexattr_init(&server->accept_mutexattr);
	pthread_mutexattr_setpshared(&server->accept_mutexattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&server->accept_lock, &server->accept_mutexattr);

	if (_event_server_create(server) == false)
	{
		_event_server_destroy(server);
		return false;
	}

	return true;
}

bool event_server_run(server_t *server)
{
	pthread_t tid;
	if (server)
	{
		if (pthread_create(&tid, NULL, 
						_event_server_factory, server) != 0)
		{
			LOG_TRACE_PERROR("pthread_create error!\n");
			_event_server_destroy(server);
			return false;
		}

		return true;
	}

	return false;
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

