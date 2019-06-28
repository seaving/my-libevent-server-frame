#ifndef __CLI_PHONE_TALK_TIMER_H__
#define __CLI_PHONE_TALK_TIMER_H__

#include "event_executor.h"
#include "cli_phone.h"
#include "cli_phone_talk.h"

/*
* 函数: cli_phone_timer
* 功能: 定时器下的回调函数
* 参数: event_buf 				event IO 操作指针
*		cli_phone				客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 定时器下只处理要发送的数据，不做其他操作，
*		关于接受数据以及流程处理等全部在cli_phone_talk函数中做
*		不允许在此处做其他处理
*/
cli_phone_talk_result_t cli_phone_timer(event_buf_t *event_buf, cli_phone_t *cli_phone);

#endif

