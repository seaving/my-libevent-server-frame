#ifndef __PROXY_SERVER_CB_H__
#define __PROXY_SERVER_CB_H__

#include "sys_inc.h"
#include "event_executor.h"

/*
* 函数: proxy_server_user_data_init_cb
* 功能: accept接受到一个客户端进行用户数据初始化回调
* 参数: user_data    带初始化用户数据
* 返回: bool
*       - false     失败
* 说明: 
*/
bool proxy_server_user_data_init_cb(talk_user_data_t *user_data);

/*
* 函数: proxy_server_timer_cb
* 功能: 定时器回调函数（1秒间隔）
* 参数: event_buf   读写evevnt IO指针
*       what        事件类型
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool proxy_server_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);

/*
* 函数: proxy_server_read_cb
* 功能: 读事件回调函数
* 参数: event_buf   读写evevnt IO指针
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool proxy_server_read_cb(event_buf_t *event_buf, int conn_fd, void *arg);

/*
* 函数: proxy_server_write_cb
* 功能: 写事件回调函数
* 参数: event_buf   读写evevnt IO指针
*       conn_fd    连接描述符
*       arg         用户参数
* 返回: bool
*       - false     失败
* 说明: 
*/
bool proxy_server_write_cb(event_buf_t *event_buf, int conn_fd, void *arg);

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
bool proxy_server_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);

#endif


