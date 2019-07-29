#ifndef __CONNECT_HTTP_H__
#define __CONNECT_HTTP_H__

#include "event_executor.h"
#include "protocol_route.h"

/*
* 函数: connect_http_server
* 功能: 连接服务器
* 参数: route_info		路由信息
*		cli_phone		cli_phone
*		event_buf		转发用的event_buf
*		method			方法
*		url				url
*		host			host
*		port			端口
*		data			需要发送的数据
*		datalen			数据长度
* 返回: cli_http_t *
*       - NULL     	失败
* 说明: 
*/
cli_http_t *connect_http_server(protocol_route_info_t *route_info, 
	cli_phone_t *cli_phone, event_buf_t *event_buf, 
	char *method, char *url, char *host, 
	int port, char *data, int datalen);

#endif
