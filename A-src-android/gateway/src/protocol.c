#include "includes.h"

/*
* 函数: _protocol_task_request_parse
* 功能: 任务请求协议处理
* 参数: cli_phone
*		event_buf
*		route_info		路由信息
* 返回: bool
*		- false		 	失败
* 说明: 
*/
static inline bool _protocol_task_request_parse(
		cli_phone_t *cli_phone, event_buf_t *event_buf, 
		protocol_route_info_t *route_info)
{
	struct json_object *json_obj = NULL;
	if (route_info == NULL)
	{
		return false;
	}

	json_obj = json_tokener_parse(route_info->data);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}

	json_object_put(json_obj);
	return true;
}

/*
* 函数: _protocol_task_response_parse
* 功能: 任务请求协议处理
* 参数: cli_phone
*		event_buf
*		route_info		路由信息
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_task_response_parse(
		cli_phone_t *cli_phone, event_buf_t *event_buf, 
		protocol_route_info_t *route_info)
{
	struct json_object *json_obj = NULL;
	if (route_info == NULL)
	{
		return false;
	}
	
	json_obj = json_tokener_parse(route_info->data);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}
	
	json_object_put(json_obj);
	return true;
}

/*
* 函数: _protocol_probe_parse
* 功能: 探测协议处理
* 参数: cli_phone
*		event_buf
*		route_info		路由信息
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_probe_parse(
		cli_phone_t *cli_phone, event_buf_t *event_buf, 
		protocol_route_info_t *route_info)
{
	char buff[1024] = {0};
	if (route_info == NULL)
	{
		return false;
	}

	//测试代码，当探测帧解析正确，若当前有任务，则直接下发任务
	becon_resp_t resp = {
		.test = "I'm OK !",
	};

	if (protocol_becon_pack_response(&resp, buff, sizeof(buff)) == false)
	{
		LOG_TRACE_ERROR("protocol_becon_pack_response error !\n");
		return false;
	}

	return protocol_route_pack(route_info->dst, route_info->src, 
			buff, cli_phone->response, sizeof(cli_phone->response));
}

/*
* 函数: _protocol_becon_parse
* 功能: 信标协议处理
* 参数: cli_phone
*		event_buf
*		route_info		路由信息
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _protocol_becon_parse(
		cli_phone_t *cli_phone, event_buf_t *event_buf, 
		protocol_route_info_t *route_info)
{
	struct json_object *json_obj = NULL;
	if (route_info == NULL)
	{
		return false;
	}
	
	json_obj = json_tokener_parse(route_info->data);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}
	
	json_object_put(json_obj);
	return true;
}

/*
* 函数: _protocol_data_parse
* 功能: 解析处理内容协议
* 参数: cli_phone
*		event_buf
*		route_info		路由信息
* 返回: bool
*		- false		 	失败
* 说明: 
*/
static inline bool _protocol_data_parse(
		cli_phone_t *cli_phone, event_buf_t *event_buf, 
		protocol_route_info_t *route_info)
{
	if (route_info == NULL)
	{
		return false;
	}

	switch (route_info->cmd)
	{
		case E_PROTOCOL_CMD_TASK_REQUEST:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_TASK_REQUEST \n");
			return _protocol_task_request_parse(cli_phone, event_buf, route_info);
		}
		case E_PROTOCOL_CMD_TASK_RESPONSE:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_TASK_RESPONSE \n");
			return _protocol_task_response_parse(cli_phone, event_buf, route_info);
		}
		case E_PROTOCOL_CMD_PROBE:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_PROBE \n");
			return _protocol_probe_parse(cli_phone, event_buf, route_info);
		}
		case E_PROTOCOL_CMD_BECON:
		{
			LOG_TRACE_NORMAL("CMD = E_PROTOCOL_CMD_BECON \n");
			return _protocol_becon_parse(cli_phone, event_buf, route_info);
		}
		default:
		{
			LOG_TRACE_NORMAL("Unkown Cmd: 0x%04x !\n", route_info->cmd);
		}
	}

	return false;
}

/*
* 函数: protocol_parse
* 功能: 解析处理协议
* 参数:	cli_phone
*		event_buf
* 返回: bool
*		- false		 	失败
* 说明: 如果需要回复内容给客户端，则直接存入到response_buf中
*/
bool protocol_parse(cli_phone_t *cli_phone, event_buf_t *event_buf)
{
	protocol_route_info_t route_info = {0};
	
	const char *context = NULL;
	const char *header = NULL;
	if (cli_phone == NULL 
		|| event_buf == NULL)
	{
		return false;
	}

	header = httpRequest_get_header(&cli_phone->httpRequest);
	context = httpRequest_get_context_buf(&cli_phone->httpRequest);
	if (context == NULL 
		|| *context == '\0')
	{
		return false;
	}

	LOG_TRACE_NORMAL(">>>>>>>> \n%s%s\n", header, context);

	if (protocol_route_parse((char *) context, &route_info) == false)
	{
		LOG_TRACE_ERROR("protocol_route_parse error !\n");
		return false;
	}

	return _protocol_data_parse(cli_phone, event_buf, &route_info);
}

