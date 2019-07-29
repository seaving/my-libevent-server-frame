#ifndef __CLI_LOCAL_TALK_H__
#define __CLI_LOCAL_TALK_H__

#include "event_executor.h"
#include "cli_phone.h"

/*
* 函数: cli_local_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 
*/
cli_phone_talk_result_t cli_local_talk(event_buf_t *event_buf, cli_phone_t *cli_phone);

#endif

