#include "includes.h"

/*
* 函数: _protocol_task_request_parse
* 功能: 任务请求协议处理
* 参数: task_obj		json内容协议
*		response_buf	待回复缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
static inline bool _protocol_task_request_parse(
		const struct json_object *task_obj, 
		char *response_buf, int bufsize)
{
	//int task_type = 0;
	//struct json_object *value_obj = NULL;
	//struct json_object *task_result_obj = NULL;
	if (task_obj == NULL)
	{
		return false;
	}

	return true;
}

/*
* 函数: _protocol_task_response_parse
* 功能: 任务请求协议处理
* 参数: task_obj		json内容协议
*		response_buf	待回复缓存
*		bufsize 		缓存大小
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_task_response_parse(
		const struct json_object *task_obj, 
		char *response_buf, int bufsize)
{
	//int task_type = 0;
	//struct json_object *value_obj = NULL;
	//struct json_object *task_result_obj = NULL;
	if (task_obj == NULL)
	{
		return false;
	}

	return true;
}

/*
* 函数: _protocol_probe_parse
* 功能: 探测协议处理
* 参数: task_obj		json内容协议
*		response_buf	待回复缓存
*		bufsize			缓存大小
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_probe_parse(
		const struct json_object *task_obj, 
		char *response_buf, int bufsize)
{
	if (task_obj == NULL)
	{
		return false;
	}

	return true;
}

/*
* 函数: _protocol_becon_parse
* 功能: 信标协议处理
* 参数: task_obj		json内容协议
*		response_buf	待回复缓存
*		bufsize 		缓存大小
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_becon_parse(
		const struct json_object *task_obj, 
		char *response_buf, int bufsize)
{
	if (task_obj == NULL)
	{
		return false;
	}

	return true;
}

/*
* 函数: _protocol_data_parse
* 功能: 解析处理内容协议
* 参数: cmd				命令
*		data_obj		json内容协议
*		response_buf	待回复缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
static inline bool _protocol_data_parse(
		int cmd, const struct json_object *data_obj, 
		char *response_buf, int bufsize)
{
	if (data_obj == NULL)
	{
		return false;
	}

	switch (cmd)
	{
		case E_PROTOCOL_CMD_TASK_REQUEST:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_TASK_REQUEST \n");
			return _protocol_task_request_parse(data_obj, response_buf, bufsize);
		}
		case E_PROTOCOL_CMD_TASK_RESPONSE:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_TASK_RESPONSE \n");
			return _protocol_task_response_parse(data_obj, response_buf, bufsize);
		}
		case E_PROTOCOL_CMD_PROBE:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_PROBE \n");
			return _protocol_probe_parse(data_obj, response_buf, bufsize);
		}
		case E_PROTOCOL_CMD_BECON:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_BECON \n");
			return _protocol_becon_parse(data_obj, response_buf, bufsize);
		}
		default:
		{
			LOG_TRACE_NORMAL("Unkown Cmd: 0x%04x !\n", cmd);
		}
	}

	return false;
}

/*
* 函数: protocol_parse
* 功能: 解析处理协议
* 参数:	httpRequest		http协议
*		response_buf	待回复缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 如果需要回复内容给客户端，则直接存入到response_buf中
*/
bool protocol_parse(const httpRequest_t *httpRequest, char *response_buf, int bufsize)
{
	bool ret = false;
	int cmd = E_PROTOCOL_CMD_UNKOWN;
	struct json_object *json_obj = NULL;
	struct json_object *cmd_obj = NULL;
	struct json_object *data_obj = NULL;
	const char *context = NULL;
	const char *header = httpRequest_get_header(httpRequest);
	if (header == NULL 
		|| response_buf == NULL 
		|| bufsize <= 0)
	{
		return false;
	}

	context = httpRequest_get_context_buf(httpRequest);
	if (context == NULL 
		|| *context == '\0')
	{
		return false;
	}

	LOG_TRACE_NORMAL(">>>>>>>> \n%s%s\n", header, context);

	json_obj = json_tokener_parse(context);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}

	json_object_get(json_obj, "cmd", cmd_obj);
	if (cmd_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(json_obj, "data", data_obj);
	if (data_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	cmd = json_object_get_int(cmd_obj);

	ret = _protocol_data_parse(cmd, data_obj, response_buf, bufsize);
	json_object_put(json_obj);

	return ret;
}

