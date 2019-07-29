#include "includes.h"


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
cli_phone_talk_result_t cli_phone_timer(event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	probe_req_t probe_req = {
		.test = "Are you OK ?",
	};

	//每隔probe_interval秒发送一次探测
	if (cli_phone->probe_interval > 0 
		&& cli_phone->time_cnt % cli_phone->probe_interval == 0)
	{
		if (protocol_probe_send_request(E_PROTOCOL_ROUTE_TASK, 
				E_PROTOCOL_ROUTE_GATEWAY, event_buf, &probe_req) == false)
		{
			LOG_TRACE_NORMAL("protocol_probe_send_request error !\n");
			return E_CLI_PHONE_TALK_FINISHED;
		}
	}

	return E_CLI_PHONE_TALK_CONTINUE;
}

