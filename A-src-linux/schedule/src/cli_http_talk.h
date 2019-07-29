#ifndef __CLI_HTTP_TALK_H__
#define __CLI_HTTP_TALK_H__

#include "event_executor.h"
#include "cli_http.h"

typedef enum __cli_http_talk_result__
{
	E_CLI_HTTP_TALK_CONTINUE,
	E_CLI_HTTP_TALK_FINISHED
} cli_http_talk_result_t;

/*
* 函数: cli_http_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: cli_http_talk_result_t
* 说明: 
*/
cli_http_talk_result_t cli_http_talk(event_buf_t *event_buf, cli_http_t *cli_http);

#endif

