#include "includes.h"

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
	int port, char *data, int datalen)
{
    cli_http_t *cli_http = cli_http_create();
    if (cli_http == NULL 
    	|| event_buf == NULL 
    	|| route_info == NULL 
    	|| cli_phone == NULL 
    	|| method == NULL 
    	|| url == NULL 
    	|| host == NULL 
    	|| port <= 0)
    {
        return NULL;
    }

	cli_http->cli_phone = cli_phone;
	cli_http->event_buf = event_buf;
	memcpy(&cli_http->route_info, route_info, 
		sizeof(protocol_route_info_t));

	snprintf(cli_http->httpRequest.method, 
		sizeof(cli_http->httpRequest.method), 
		"%s", method);

	snprintf(cli_http->httpRequest.url, 
		sizeof(cli_http->httpRequest.url), 
		"%s", url);

	snprintf(cli_http->httpRequest.host, 
		sizeof(cli_http->httpRequest.host), 
		"%s", host);

	if (data && datalen > 0)
	{
		snprintf(cli_http->httpRequest.post_buf, 
			sizeof(cli_http->httpRequest.post_buf), 
			"%s", data);
		cli_http->httpRequest.post_datalen = datalen;
	}

    if (event_connect_distribute_job(
			NULL, 
		    host, 
		    port, 
		    60, cli_http, 
		    cli_http_free, 
		    connect_http_server_success_cb, 
		    connect_http_talk_timer_cb, 
		    connect_http_talk_read_cb, 
		    connect_http_talk_write_cb, 
		    connect_http_talk_error_cb) == false)
    {
        LOG_TRACE_NORMAL("event_connect_distribute_job error !\n");
        cli_http_free(cli_http);
        return NULL;
    }

	cli_http->proxy_valid = true;

	return cli_http;
}

