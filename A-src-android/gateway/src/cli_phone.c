#include "includes.h"

/*
* 函数: cli_phone_create
* 功能: 创建一个结构体
* 参数: 无
* 返回: cli_phone_t *
*       - NULL      失败
* 说明: 
*/
cli_phone_t *cli_phone_create()
{
    cli_phone_t *cli_phone = calloc(sizeof(cli_phone_t), 1);
    if (cli_phone == NULL)
    {
        LOG_TRACE_PERROR("calloc error !\n");
        return NULL;
    }

    cli_phone->step = E_CLI_PHONE_STEP_READ_HEADER;
    cli_phone->probe_interval = 20;
    cli_phone->route = E_PROTOCOL_ROUTE_UNKOWN;
    return cli_phone;
}

/*
* 函数: cli_phone_free
* 功能: 释放一个结构体
* 参数: 无
* 返回: 无
* 说明: 
*/
void cli_phone_free(void *cli)
{
	cli_phone_t *cli_phone = (cli_phone_t *) cli;
	if (cli_phone)
	{
		update_event_buf_array(cli_phone->route, NULL);
		httpRequest_context_free(&cli_phone->httpRequest);
		memset(cli_phone, 0, sizeof(cli_phone_t));
		free(cli_phone);
	}
}

