#ifndef __CLI_HTTP_H__
#define __CLI_HTTP_H__

#include "http_request.h"
#include "cli_phone.h"

typedef struct __cli_phone__ cli_phone_t;

typedef enum __cli_http_step__
{
    E_CLI_HTTP_STEP_SEND_REQUEST = 0,
	E_CLI_HTTP_STEP_READ_HEADER,
	E_CLI_HTTP_STEP_CHECK_HEADER,
	E_CLI_HTTP_STEP_FRAME_PARSE,
	E_CLI_HTTP_STEP_READ_CONTEXT,
	E_CLI_HTTP_STEP_SEND_SUCCESS,
	E_CLI_HTTP_STEP_SEND_ERROR,
	E_CLI_HTTP_STEP_FINISHED
} cli_http_step_t;

typedef struct __cli_http__
{
    httpRequest_t httpRequest;
    cli_http_step_t step;
    unsigned int time_cnt;
    char response[2048];

	/************* 代理相关 ************/
	bool proxy_valid;			//代理是否有效
    cli_phone_t *cli_phone;		//代理结构体指针
    event_buf_t *event_buf;
    protocol_route_info_t route_info;
} cli_http_t;

cli_http_t *cli_http_create();
void cli_http_free(void *cli);

#endif

