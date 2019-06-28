#ifndef __EVENT_SERVER_H__
#define __EVENT_SERVER_H__

#include "sys_inc.h"
#include "event_connect.h"

typedef struct __server_talk_user_data__ talk_user_data_t;
typedef bool (*talk_user_data_init_cb)(talk_user_data_t *);

typedef enum __server_type__
{
    E_SERVER_TYPE_TCP = 0,              //tcp服务器
    E_SERVER_TYPE_UDP,                  //udp服务器
    E_SERVER_TYPE_LOCAL                 //本地服务器localsocket
} ser_type_t;

typedef struct __server_localsocket_addr__
{
	char *name;
	int namespace;
} ser_localsocket_addr_t;

typedef struct __server__
{
	int listen_fd;
	int listen_port;
	int listen_count;
	int queue_size;
    ser_type_t type;
    ser_localsocket_addr_t localsockaddr;
	struct event event_accept;
	struct event_base *evbase_server;
	bool server_ok;
	int client_cnt;
	int accept_max;
	pthread_spinlock_t lock;
	pthread_mutexattr_t accept_mutexattr;
	pthread_mutex_t accept_lock;
	event_cb_fn accept_cb;
	//-----------------------
	SSL_CTX *ctx;
	//----------------------------------
	unsigned int io_timeout;
	connect_timer_cb timer_cb;
	connect_io_cb read_cb;
	connect_io_cb write_cb;
	connect_error_cb error_cb;
	talk_user_data_init_cb user_data_init_cb;
} server_t;

struct __server_talk_user_data__
{
	void *arg;
	void (*free_arg)(void *);
};

typedef struct __server_talk__
{
	int conn_fd;
	server_t *server;
	talk_user_data_t user_data;
} server_talk_t;

bool event_server_init(
		SSL_CTX *ctx, 
		server_t *server, 
		unsigned int io_timeout, 
		connect_timer_cb timer_cb, 
		connect_io_cb read_cb, 
		connect_io_cb write_cb, 
	    connect_error_cb error_cb, 
	    talk_user_data_init_cb user_data_init_cb);

bool event_server_run(server_t *server);

void event_server_destroy(server_t *server);
bool event_server_status_ok(server_t *server);

void event_server_client_count(server_t *server, int c);
int event_server_client_get_counts(server_t *server);

server_talk_t *event_server_talk_create(server_t *server, int conn_fd);
void event_server_talk_free(void *arg);

#endif


