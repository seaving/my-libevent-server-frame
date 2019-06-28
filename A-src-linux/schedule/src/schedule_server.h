#ifndef __SCHEDULE_SERVER_H__
#define __SCHEDULE_SERVER_H__

/*
* 函数: schedule_server_init
* 功能: 初始化调度服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_server_init();

/*
* 函数: schedule_server_start
* 功能: 启动调度服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_server_start();

/*
* 函数: schedule_server_stop
* 功能: 停止调度服务器
* 参数: 无
* 返回: 无
* 说明: 
*/
void schedule_server_stop();

/*
* 函数: schedule_server_get_accept_client_cnt
* 功能: 获取连接数
* 参数: 无
* 返回: int
* 说明: 
*/
int schedule_server_get_accept_client_cnt();

#endif
