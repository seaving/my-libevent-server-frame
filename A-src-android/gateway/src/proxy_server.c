#include "includes.h"

typedef struct __proxy_server__
{
    server_t server;
    hashtable_t *ht;
} proxy_server_t;

static proxy_server_t _proxy_server;

#define EVENT_BUF_SIZE			16
static event_buf_t *_event_buf[EVENT_BUF_SIZE] = {NULL};

/*
* 函数: proxy_server_init
* 功能: 初始化代理服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool proxy_server_init()
{
	memset(&_proxy_server, 0, sizeof(proxy_server_t));
#if 1
    _proxy_server.server.type = E_SERVER_TYPE_LOCAL;
    _proxy_server.server.localsockaddr.namespace = ANDROID_SOCKET_NAMESPACE_RESERVED;
    _proxy_server.server.localsockaddr.name = strdup(SOCKET_LOCAL_NAME);
#else
    _proxy_server.server.type = E_SERVER_TYPE_TCP;
    _proxy_server.server.listen_port = 9111;
#endif
	_proxy_server.server.listen_count = 10;
	_proxy_server.server.accept_max = 32;

    //不超时，因为做任务过程中可能会耗时比较久
    _proxy_server.server.io_timeout = 0;

	if (event_server_init(
			NULL, 
			&_proxy_server.server, 
	        _proxy_server.server.io_timeout, 
			proxy_server_timer_cb, 
			proxy_server_read_cb, 
			proxy_server_write_cb, 
			proxy_server_error_cb, 
			proxy_server_user_data_init_cb) == false)
	{
		LOG_TRACE_NORMAL("event_server_init error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("event_server_init success.\n");
	return true;
}

/*
* 函数: proxy_server_start
* 功能: 启动代理服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool proxy_server_start()
{
	return event_server_run(&_proxy_server.server);
}

/*
* 函数: proxy_server_stop
* 功能: 停止代理服务器
* 参数: 无
* 返回: 无
* 说明: 
*/
void proxy_server_stop()
{
	event_server_destroy(&_proxy_server.server);
}

/*
* 函数: proxy_server_is_ok
* 功能: 服务器是否正常
* 参数: 无
* 返回: bool
*       - false     不正常
* 说明: 
*/
bool proxy_server_is_ok()
{
    return event_server_status_ok(&_proxy_server.server);
}

/*
* 函数: proxy_server_get_accept_client_cnt
* 功能: 获取连接数
* 参数: 无
* 返回: int
* 说明: 
*/
int proxy_server_get_accept_client_cnt()
{
    return event_server_client_get_counts(&_proxy_server.server);
}

/*
* 函数: proxy_send_data_to_local_client
* 功能: 发送数据
* 参数: event_buf		event_buf
*		data		data
*		datalen		长度
* 返回: bool
* 说明: 
*/
bool proxy_send_data_to_local_client(event_buf_t *event_buf, char *data, int datalen)
{
	char len[4] = {0};
	if (event_buf == NULL)
	{
		return false;
	}

	len[0] = (datalen >> 24) & 0xff;
	len[1] = (datalen >> 16) & 0xff;
	len[2] = (datalen >> 8) & 0xff;
	len[3] = (datalen >> 0) & 0xff;

	event_send_data(event_buf, len, MIN(sizeof(int), sizeof(len)));
	return event_send_data(event_buf, data, datalen);
}

/*
* 函数: proxy_send_data_to_module
* 功能: 发送数据到模块
* 参数: route			模块
*		data        数据
*       datalen     长度
* 返回: bool
*       - false     失败
* 说明: 
*/
bool proxy_send_data_to_module(protocol_route_t route, char *data, int data_len)
{
    if (data == NULL 
        || data_len <= 0 
        || route < 0 
        || route >= EVENT_BUF_SIZE 
        || route == E_PROTOCOL_ROUTE_UNKOWN 
        || _event_buf[route] == NULL)
    {
        return false;
    }

	LOG_TRACE_NORMAL("###### route = %d, %s\n", route, data);

	return proxy_send_data_to_local_client(_event_buf[route], data, data_len);
}

/*
* 函数: update_event_buf_array
* 功能: 更新数组
* 参数: route			模块
*		event_buf	event_buf
* 返回: bool
*       - false     失败
* 说明: 
*/
bool update_event_buf_array(protocol_route_t route, event_buf_t *event_buf)
{
    if (route < 0 
        || route >= EVENT_BUF_SIZE 
        || route == E_PROTOCOL_ROUTE_UNKOWN)
    {
        return false;
    }

    _event_buf[route] = event_buf;
    return true;
}

//本地模块连接上本地服务后，需要立即发送注册命令进行注册，通过注册可以区分哪个eventbuf对应的是哪个模块从而进行路由转发
