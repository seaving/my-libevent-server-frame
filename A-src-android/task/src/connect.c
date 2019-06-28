#include "includes.h"

static cli_phone_t *_cli_phone = NULL;

/*
* 函数: _connect_free
* 功能: 释放参数
* 参数: m           指针
* 返回: 无
* 说明: 
*/
static void _connect_free(void *m)
{
    cli_phone_free(_cli_phone);
    _cli_phone = NULL;
}

/*
* 函数: connect_server
* 功能: 连接服务器
* 参数: 无
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_server()
{
    if (_cli_phone)
    {
        LOG_TRACE_NORMAL("connection exist !\n");
        return false;
    }

    _cli_phone = cli_phone_create();
    if (_cli_phone == NULL)
    {
        return false;
    }
#if 1
    if (event_connect_local_socket_distribute_job(
			NULL, 
		    ANDROID_SOCKET_NAMESPACE_RESERVED, 
		    SOCKET_LOCAL_NAME, 
		    60, _cli_phone, 
		    _connect_free, 
		    connect_server_success_cb, 
		    connect_talk_timer_cb, 
		    connect_talk_read_cb, 
		    connect_talk_write_cb, 
		    connect_talk_error_cb) == false)
#else
    if (event_connect_distribute_job(
			NULL, 
		    "127.0.0.1", 
		    9111, 
		    60, _cli_phone, 
		    _connect_free, 
		    connect_server_success_cb, 
		    connect_talk_timer_cb, 
		    connect_talk_read_cb, 
		    connect_talk_write_cb, 
		    connect_talk_error_cb) == false)
#endif
    {
        LOG_TRACE_NORMAL("event_connect_local_socket_distribute_job error !\n");
        cli_phone_free(_cli_phone);
        _cli_phone = NULL;
        return false;
    }

	return true;
}

/*
* 函数: connect_server_is_ok
* 功能: 连接服务器状态
* 参数: 无
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_server_is_ok()
{
    return _cli_phone != NULL;
}

/*
* 函数: connect_send_data
* 功能: 发送数据到服务器
* 参数: data        数据
*       datalen     长度
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_send_data(char *data, int data_len)
{
    return cli_phone_safe_send_data(_cli_phone, data, data_len);
}

