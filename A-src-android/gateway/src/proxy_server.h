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

/*
* 函数: proxy_send_data_to_local_client
* 功能: 发送数据
* 参数: event_buf		event_buf
*		data		data
*		datalen		长度
* 返回: bool
* 说明: 
*/
bool proxy_send_data_to_local_client(event_buf_t *event_buf, char *data, int datalen);

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
bool proxy_send_data_to_module(protocol_route_t route, char *data, int data_len);

/*
* 函数: update_event_buf_array
* 功能: 更新数组
* 参数: route			模块
*		event_buf	event_buf
* 返回: bool
*       - false     失败
* 说明: 
*/
bool update_event_buf_array(protocol_route_t route, event_buf_t *event_buf);

#endif

