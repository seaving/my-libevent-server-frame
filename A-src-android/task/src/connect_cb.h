#ifndef __CONNECT_CB__
#define __CONNECT_CB__

#include "event_executor.h"

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
bool connect_server_success_cb(event_buf_t *event_buf, int conn_fd, void *arg);

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
bool connect_talk_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);

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
bool connect_talk_read_cb(event_buf_t *event_buf, int conn_fd, void *arg);

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
bool connect_talk_write_cb(event_buf_t *event_buf, int conn_fd, void *arg);

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
bool connect_talk_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);

#endif
