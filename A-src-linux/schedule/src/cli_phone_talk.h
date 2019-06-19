#ifndef __CLI_PHONE_TALK_H__
#define __CLI_PHONE_TALK_H__

#include "event_executor.h"
#include "cli_phone.h"

typedef enum __cli_phone_talk_result__
{
	E_CLI_PHONE_TALK_CONTINUE,
	E_CLI_PHONE_TALK_FINISHED
} cli_phone_talk_result_t;

/*
* 函数: cli_phone_talk
* 功能: 交互入口
* 参数: executor			执行者ID
*		event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 
*/
cli_phone_talk_result_t cli_phone_talk(evexecutor_t executor, event_buf_t *event_buf, cli_phone_t *cli_phone);

#endif

