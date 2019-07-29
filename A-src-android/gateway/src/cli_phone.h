#ifndef __CLI_PHONE_H__
#define __CLI_PHONE_H__

#include "http_request.h"
#include "event_executor.h"

typedef enum __cli_phone_step__
{
	E_CLI_PHONE_STEP_READ_HEADER = 0,
	E_CLI_PHONE_STEP_CHECK_HEADER,
	E_CLI_PHONE_STEP_FRAME_PARSE,
	E_CLI_PHONE_STEP_READ_CONTEXT,
	E_CLI_PHONE_STEP_SEND_SUCCESS,
	E_CLI_PHONE_STEP_SEND_ERROR,
	E_CLI_PHONE_STEP_FINISHED
} cli_phone_step_t;

typedef struct __cli_phone__
{
    httpRequest_t httpRequest;
    cli_phone_step_t step;
    char response[2048];
    unsigned int time_cnt;
    unsigned int probe_interval;
    event_buf_t *event_buf;
    protocol_route_t route;
} cli_phone_t;

/*
* 函数: cli_phone_create
* 功能: 创建一个结构体
* 参数: 无
* 返回: cli_phone_t *
*       - NULL      失败
* 说明: 
*/
cli_phone_t *cli_phone_create();

/*
* 函数: cli_phone_free
* 功能: 释放一个结构体
* 参数: 无
* 返回: 无
* 说明: 
*/
void cli_phone_free(void *cli);

#endif
