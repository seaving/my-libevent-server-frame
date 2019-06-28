#include "includes.h"

/*
* 函数: connect_server_success_cb
* 功能: 连接服务器成功回调函数
* 参数: event_buf   读写evevnt IO指针
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_server_success_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("connect_server_success_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	return true;
}

/*
* 函数: connect_talk_timer_cb
* 功能: 定时器回调函数（1秒间隔）
* 参数: event_buf   读写evevnt IO指针
*       what        事件类型
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_talk_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("connect_talk_timer_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}
	
	//计时，不清楚为什么localsocket在android下出现不明原因的问题
	//具体问题现象如下：
	//第一次连接loccalsocket服务器成功，task发送数据到服务端，服务端可以正常收发，
	//若此时出现task断开立刻重连服务端，服务端与task建立连接之后，task发送的数据无法到达
	//服务端，但是连接确实已经建立，服务端却没有收到可读时间的回调，
	//此处把计时放到timer回调之前，建立连接后不立即发送数据，稍等片刻再发送
	cli_phone->time_cnt ++;

	if (cli_phone_timer(event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
	{
		return false;
	}

	return true;
}

/*
* 函数: connect_talk_read_cb
* 功能: 读事件回调函数
* 参数: event_buf   读写evevnt IO指针
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_talk_read_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("connect_talk_read_cb\n");

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

/*
* 函数: connect_talk_write_cb
* 功能: 写事件回调函数
* 参数: event_buf   读写evevnt IO指针
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_talk_write_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("connect_talk_write_cb\n");

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

/*
* 函数: connect_talk_error_cb
* 功能: 错误事件回调函数
* 参数: event_buf   读写evevnt IO指针
*       what        事件类型
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_talk_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("connect_talk_error_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	return true;
}
