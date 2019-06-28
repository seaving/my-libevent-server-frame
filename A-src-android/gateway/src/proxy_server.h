#ifndef __PROXY_H__
#define __PROXY_H__

/*
* 函数: proxy_server_init
* 功能: 初始化代理服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool proxy_server_init();

/*
* 函数: proxy_server_start
* 功能: 启动代理服务器
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool proxy_server_start();

/*
* 函数: proxy_server_stop
* 功能: 停止代理服务器
* 参数: 无
* 返回: 无
* 说明: 
*/
void proxy_server_stop();

/*
* 函数: proxy_server_is_ok
* 功能: 服务器是否正常
* 参数: 无
* 返回: bool
*       - false     不正常
* 说明: 
*/
bool proxy_server_is_ok();

/*
* 函数: proxy_server_get_accept_client_cnt
* 功能: 获取连接数
* 参数: 无
* 返回: int
* 说明: 
*/
int proxy_server_get_accept_client_cnt();

#endif
