#include "includes.h"

/*
* 函数: schedule_server_init
* 功能: 初始化调度服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_server_init()
{
	if (event_server_init(
			sch_master.ctx, 
			&sch_master.server, 
	        sch_master.server.io_timeout, 
			schedule_server_timer_cb, 
			schedule_server_read_cb, 
			schedule_server_write_cb, 
			schedule_server_error_cb, 
			schedule_server_user_data_init_cb) == false)
	{
		LOG_TRACE_NORMAL("event_server_init error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("event_server_init success.\n");
	return true;
}

/*
* 函数: schedule_server_start
* 功能: 启动调度服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_server_start()
{
	return event_server_run(&sch_master.server);
}

/*
* 函数: schedule_server_stop
* 功能: 停止调度服务器
* 参数: 无
* 返回: 无
* 说明: 
*/
void schedule_server_stop()
{
	event_server_destroy(&sch_master.server);
}

/*
* 函数: schedule_server_get_accept_client_cnt
* 功能: 获取连接数
* 参数: 无
* 返回: int
* 说明: 
*/
int schedule_server_get_accept_client_cnt()
{
    return event_server_client_get_counts(&sch_master.server);
}

