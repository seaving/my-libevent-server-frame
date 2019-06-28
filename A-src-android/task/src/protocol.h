#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "sys_inc.h"
#include "http_request.h"
#include "event_executor.h"

/*
* 函数: protocol_parse
* 功能: 解析处理协议
* 参数:	cli_phone
*		event_buf
* 返回: bool
*		- false		 	失败
* 说明: 如果需要回复内容给客户端，则直接存入到response_buf中
*/
bool protocol_parse(cli_phone_t *cli_phone, event_buf_t *event_buf);

#endif

