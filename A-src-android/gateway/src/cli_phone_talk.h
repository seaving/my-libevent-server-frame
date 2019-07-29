#ifndef __CLI_PHONE_TALK_H__
#define __CLI_PHONE_TALK_H__

#include "event_executor.h"
#include "cli_phone.h"

typedef enum __cli_local_step_result__
{
	/*
		执行当前step，不立即执行下一个step
	*/
	E_CLI_PHONE_STEP_RESULT_ONCE = 0,

	/*
		执行完当前step，立即再次执行下一个step
	*/
	E_CLI_PHONE_STEP_RESULT_AGAIN,

	/*
		执行完所有步骤
	*/
	E_CLI_PHONE_STEP_RESULT_FINISHED
} cli_phone_res_t;

typedef enum __cli_phone_talk_result__
{
	E_CLI_PHONE_TALK_CONTINUE,
	E_CLI_PHONE_TALK_FINISHED
} cli_phone_talk_result_t;

/*
* 函数: cli_phone_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 
*/
cli_phone_talk_result_t cli_phone_talk(event_buf_t *event_buf, cli_phone_t *cli_phone);

#endif

