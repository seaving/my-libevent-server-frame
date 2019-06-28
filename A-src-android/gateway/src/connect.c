#include "includes.h"

static SSL_CTX *_global_ctx  = NULL;

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
* 函数: global_ctx_init
* 功能: 初始化ssl ctx
* 参数: 无
* 返回: bool
*       - false     失败
* 说明: 
*/
bool global_ctx_init()
{
	_global_ctx = event_ssl_client_init(
			DEFAULT_CA_FILE, 
			DEFAULT_CRT_FILE, DEFAULT_KEY_FILE);

	return _global_ctx != NULL;
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

    if (event_connect_distribute_job(
			_global_ctx, 
		    SCHEDULE_SERVER_HOST, 
		    SCHEDULE_SERVER_PORT, 
		    60, _cli_phone, 
		    _connect_free, 
		    connect_server_success_cb, 
		    connect_talk_timer_cb, 
		    connect_talk_read_cb, 
		    connect_talk_write_cb, 
		    connect_talk_error_cb) == false)
    {
        LOG_TRACE_NORMAL("event_connect_distribute_job error !\n");
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
* 函数: send_data_to_server
* 功能: 发送数据到服务器
* 参数: data        数据
*       datalen     长度
* 返回: bool
*       - false     失败
* 说明: 
*/
bool send_data_to_server(char *data, int data_len)
{
    if (data == NULL 
        || data_len <= 0 
        || _cli_phone == NULL 
        || _cli_phone->event_buf == NULL)
    {
        return false;
    }

    return event_send_data(_cli_phone->event_buf, data, data_len);
}
