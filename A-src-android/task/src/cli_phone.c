#include "includes.h"

static pthread_mutex_t _io_lock = PTHREAD_MUTEX_INITIALIZER;

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
		httpRequest_context_free(&cli_phone->httpRequest);
		memset(cli_phone, 0, sizeof(cli_phone_t));
		free(cli_phone);
	}
}

/*
* 函数: cli_phone_safe_send_data
* 功能: 发送数据到服务器
* 参数: cli_phone		cli_phone
*		data        数据
*       datalen     长度
* 返回: bool
*       - false     失败
* 说明: 
*/
bool cli_phone_safe_send_data(cli_phone_t *cli_phone, char *data, int data_len)
{
	bool ret = false;
    if (data == NULL 
        || data_len <= 0 
        || cli_phone == NULL 
        || cli_phone->event_buf == NULL)
    {
        return false;
    }

	pthread_mutex_lock(&_io_lock);
	ret = event_send_data(cli_phone->event_buf, data, data_len);
	pthread_mutex_unlock(&_io_lock);
    return ret;
}

