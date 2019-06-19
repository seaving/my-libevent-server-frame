#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "sys_inc.h"
#include "http_request.h"
#include "event_executor.h"

//------------------------------------------------------------------------

#define X_AUTH	"brush-1990asdklw3323fyksdweb23"

typedef enum __protocol_cmd__
{
	E_PROTOCOL_CMD_UNKOWN = 0x00,
	E_PROTOCOL_CMD_PROBE = 0x01,				//探测帧
	E_PROTOCOL_CMD_BECON = 0x02,				//信标帧
	E_PROTOCOL_CMD_TASK_REQUEST = 0x03,			//任务请求帧
	E_PROTOCOL_CMD_TASK_RESPONSE = 0x04,		//任务响应帧
	E_PROTOCOL_CMD_MAX,
} protocol_cmd_t;
//------------------------------------------------------------------------
#define json_object_get(json_obj, key, value_obj) \
	do { \
		value_obj = NULL; \
		if (json_object_object_get_ex(json_obj, \
				key, &value_obj) == false) \
		{ \
			LOG_TRACE_NORMAL("json_object_object_get_ex error ! key = %s\n", key); \
		} \
	} while (0)

#define protocol_pack(buf, bufsize, context)	\
	do { \
		snprintf(buf, bufsize, \
				"X-Auth: %s \r\n" \
				"Content-Length: %d \r\n" \
				"\r\n" \
				"%s", \
				X_AUTH, \
				(int) strlen(context), \
				context); \
	} while (0)

/*
* 函数: protocol_parse
* 功能: 解析处理协议
* 参数:	executor		执行者ID
*		httpRequest		http协议
*		response_buf	待回复缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 如果需要回复内容给客户端，则直接存入到response_buf中
*/
bool protocol_parse(evexecutor_t executor, httpRequest_t *httpRequest, char *response_buf, int bufsize);

#endif

