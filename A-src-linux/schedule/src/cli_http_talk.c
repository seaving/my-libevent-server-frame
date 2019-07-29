#include "includes.h"

typedef enum __cli_http_step_result__
{
	/*
		执行当前step，不立即执行下一个step
	*/
	E_CLI_HTTP_STEP_RESULT_ONCE = 0,

	/*
		执行完当前step，立即再次执行下一个step
	*/
	E_CLI_HTTP_STEP_RESULT_AGAIN,

	/*
		执行完所有步骤
	*/
	E_CLI_HTTP_STEP_RESULT_FINISHED
} cli_http_res_t;
//-------------------------------------------------------------------
/*
* 函数: _cli_http_talk_response
* 功能: 发送响应
* 参数: event_buf				event IO 操作指针
*		cli_http			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline bool _cli_http_talk_response(
		event_buf_t *event_buf, cli_http_t *cli_http)
{
	if (strlen(cli_http->response) > 0 
		&& cli_http->proxy_valid 
		&& cli_http->cli_phone 
		&& cli_http->event_buf)
	{
		LOG_TRACE_NORMAL("proxy to client phone: %s\n", cli_http->response);
		return event_send_data(cli_http->event_buf, 
				cli_http->response, 
				strlen(cli_http->response));
	}

	if (strlen(cli_http->response) > 0)
	{
		LOG_TRACE_ERROR("cli_http->proxy_valid: %s, "
			"cli_http->cli_phone: %s, "
			"cli_http->event_buf: %s", 
			cli_http->proxy_valid ? "true" : "false", 
			cli_http->cli_phone ? "valid" : "NULL", 
			cli_http->event_buf ? "valid" : "NULL");
	}

	return true;
}

/*
* 函数: _cli_http_talk_send_request
* 功能: 发送http请求
* 参数: event_buf				event IO 操作指针
*		cli_http			客户端结构体
* 返回: bool
*       - false             失败
* 说明: 
*/
static inline bool _cli_http_talk_send_request(
		event_buf_t *event_buf, cli_http_t *cli_http)
{
	char header[8192] = {0};
    snprintf(header, sizeof(header), 
    	"%s %s HTTP/1.1\r\n"
    	"Host: %s\r\n"
    	"Content-Length: %d\r\n"
    	"\r\n"
    	"%s", 
    	cli_http->httpRequest.method, 
    	cli_http->httpRequest.url, 
    	cli_http->httpRequest.host, 
    	cli_http->httpRequest.post_datalen, 
    	cli_http->httpRequest.post_buf);

    event_send_data(event_buf, header, strlen(header));

	//LOG_TRACE_NORMAL(">>>>>>>> \n%s", 
		//httpRequest_get_header(&cli_http->httpRequest));
	return true;
}

/*
* 函数: _cli_http_talk_recv_header
* 功能: 读取http头部信息
* 参数: event_buf				event IO 操作指针
*		cli_http			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_http_talk_recv_header(
		event_buf_t *event_buf, cli_http_t *cli_http)
{
	httpRequest_result_t httpRequest_result;
	httpRequest_result = httpRequest_recv_header(
					event_buf, &cli_http->httpRequest);
	if (httpRequest_result != E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS)
	{
		return httpRequest_result;
	}

	LOG_TRACE_NORMAL(">>>>>>>> \n%s", 
		httpRequest_get_header(&cli_http->httpRequest));
	return httpRequest_result;
}

/*
* 函数: _cli_http_talk_check_header
* 功能: 校验头部
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: bool
*		- false			失败
* 说明: 
*/
static inline bool _cli_http_talk_check_header(
		event_buf_t *event_buf, cli_http_t *cli_http)
{
	return true;
}
		
/*
* 函数: _cli_http_talk_recv_context
* 功能: 读取正文
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_http_talk_recv_context(
		event_buf_t *event_buf, cli_http_t *cli_http)
{
	httpRequest_result_t ret;
	ret = httpRequest_recv_context(event_buf, &cli_http->httpRequest);
	if (ret == E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS 
	 && cli_http->httpRequest.context_buf != NULL)
	{
	    /*LOG_TRACE_NORMAL(">>>>>(len: %d) %s\n", 
	    	(int) strlen(cli_http->httpRequest.context_buf), 
	    	cli_http->httpRequest.context_buf);*/
    }
	return ret;
}

/*
* 函数: _cli_http_frame_parse
* 功能: 解析协议
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _cli_http_frame_parse(event_buf_t *event_buf, cli_http_t *cli_http)
{
	const char *context = NULL;
	const char *header = NULL;

	//int code = -1;
	//const char *msg = NULL;
	const char *data = NULL;

	struct json_object *json_obj = NULL;
	struct json_object *code_obj = NULL;
	struct json_object *msg_obj = NULL;
	struct json_object *data_obj = NULL;

	if (cli_http == NULL 
		|| event_buf == NULL 
		|| cli_http->httpRequest.context_buf == NULL)
	{
		return false;
	}

	header = httpRequest_get_header(&cli_http->httpRequest);
	context = httpRequest_get_context_buf(&cli_http->httpRequest);

	LOG_TRACE_NORMAL(">>>>>>>> \n%s%s\n", header, context);

	json_obj = json_tokener_parse(context);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}

	json_object_get(json_obj, "code", code_obj);
	if (code_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(json_obj, "msg", msg_obj);
	if (msg_obj == NULL)
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

	//code = json_object_get_int(code_obj);
	//msg = json_object_get_string(msg_obj);
	data = json_object_to_json_string(data_obj);
	if (strlen(data) <= 0)
	{
		json_object_put(json_obj);
		return false;
	}

	memset(&cli_http->response, 0, sizeof(cli_http->response));

	
	protocol_route_pack(cli_http->route_info.dst, 
		cli_http->route_info.src, E_PROTOCOL_CMD_TASK_RESPONSE, 
		(char *) data, cli_http->response, sizeof(cli_http->response));

	json_object_put(json_obj);
	return true;
}

/*
* 函数: _cli_http_step
* 功能: 流程序列
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: cli_http_res_t
* 说明: 
*/
static cli_http_res_t _cli_http_step(event_buf_t *event_buf, cli_http_t *cli_http)
{
	httpRequest_result_t httpRequest_result = E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
	switch (cli_http->step)
	{
        case E_CLI_HTTP_STEP_SEND_REQUEST:
        {
            LOG_TRACE_NORMAL("_cli_http_talk_send_request\n");
			if (_cli_http_talk_send_request(event_buf, cli_http) == true)
			{
				cli_http->step = E_CLI_HTTP_STEP_READ_HEADER;
				return E_CLI_HTTP_STEP_RESULT_ONCE;
			}
            break;
        }
		case E_CLI_HTTP_STEP_READ_HEADER:
		{
			LOG_TRACE_NORMAL("_cli_http_talk_recv_header\n");
			httpRequest_result = _cli_http_talk_recv_header(event_buf, cli_http);
			if (httpRequest_result == 
					E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS)
			{
				cli_http->step = E_CLI_HTTP_STEP_CHECK_HEADER;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			else if (httpRequest_result == 
						E_HTTP_REQUEST_RESULT_RECV_EMPTY)
			{
				return E_CLI_HTTP_STEP_RESULT_ONCE;
			}
			break;
		}
		case E_CLI_HTTP_STEP_CHECK_HEADER:
		{
			LOG_TRACE_NORMAL("_cli_http_talk_check_header\n");
			if (_cli_http_talk_check_header(event_buf, cli_http) == true)
			{
				cli_http->step = E_CLI_HTTP_STEP_READ_CONTEXT;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			else
			{
				cli_http->step = E_CLI_HTTP_STEP_SEND_ERROR;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			break;
		}
		case E_CLI_HTTP_STEP_READ_CONTEXT:
		{
			LOG_TRACE_NORMAL("_cli_http_talk_recv_context\n");
			httpRequest_result = _cli_http_talk_recv_context(event_buf, cli_http);
			if (httpRequest_result == 
					E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS)
			{
				cli_http->step = E_CLI_HTTP_STEP_FRAME_PARSE;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			else if (httpRequest_result == 
						E_HTTP_REQUEST_RESULT_RECV_EMPTY)
			{
				return E_CLI_HTTP_STEP_RESULT_ONCE;
			}
			break;
		}
		case E_CLI_HTTP_STEP_FRAME_PARSE:
		{
			LOG_TRACE_NORMAL("_cli_http_frame_parse\n");
			if (_cli_http_frame_parse(event_buf, cli_http) == true)
			{
				cli_http->step = E_CLI_HTTP_STEP_SEND_SUCCESS;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			else
			{
				cli_http->step = E_CLI_HTTP_STEP_SEND_ERROR;
				return E_CLI_HTTP_STEP_RESULT_AGAIN;
			}
			break;
		}
		/************************ Response ************************/
		case E_CLI_HTTP_STEP_SEND_SUCCESS:
		{
			LOG_TRACE_NORMAL("_cli_http_talk_response\n");
			_cli_http_talk_response(event_buf, cli_http);

			cli_http->step = E_CLI_HTTP_STEP_FINISHED;

			memset(&cli_http->httpRequest, 0, sizeof(httpRequest_t));
			memset(&cli_http->response, 0, sizeof(cli_http->response));

			return E_CLI_HTTP_STEP_RESULT_FINISHED;
		}
		case E_CLI_HTTP_STEP_SEND_ERROR:
		{
			LOG_TRACE_NORMAL("cli_http_talk_response error\n");
			//_cli_http_talk_response(event_buf, cli_http);

			cli_http->step = E_CLI_HTTP_STEP_FINISHED;

			memset(&cli_http->httpRequest, 0, sizeof(httpRequest_t));
			memset(&cli_http->response, 0, sizeof(cli_http->response));

		#if 0
			//必须返回once也不能返回finished，否则消息会发不出去
			return E_CLI_HTTP_STEP_RESULT_ONCE;
		#else
			//立刻结束
			return E_CLI_HTTP_STEP_RESULT_FINISHED;
		#endif
		}
		case E_CLI_HTTP_STEP_FINISHED:
		default:
		{
			memset(&cli_http->httpRequest, 0, sizeof(httpRequest_t));
			memset(&cli_http->response, 0, sizeof(cli_http->response));

			//结束所有流程
			return E_CLI_HTTP_STEP_RESULT_FINISHED;
		}
	}

	//默认结束所有流程
	return E_CLI_HTTP_STEP_RESULT_FINISHED;
}

/*
* 函数: cli_http_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_http		客户端结构体
* 返回: cli_http_talk_result_t
* 说明: 
*/
cli_http_talk_result_t cli_http_talk(event_buf_t *event_buf, cli_http_t *cli_http)
{
	cli_http_res_t ret = E_CLI_HTTP_STEP_RESULT_FINISHED;
	if (cli_http == NULL 
		|| event_buf == NULL)
	{
		//走到这里表示异常，终止服务端与客户端的连接会话
		return E_CLI_HTTP_TALK_FINISHED;
	}

	for ( ; ; )
	{
		ret = _cli_http_step(event_buf, cli_http);
		if (ret == E_CLI_HTTP_STEP_RESULT_AGAIN)
		{
			//继续轮询执行步骤
			continue;
		}
		if (ret == E_CLI_HTTP_STEP_RESULT_ONCE)
		{
			//结束本次步骤执行，继续服务端与客户端的连接会话
			return E_CLI_HTTP_TALK_CONTINUE;
		}
		if (ret == E_CLI_HTTP_STEP_RESULT_FINISHED)
		{
			//结束本次步骤执行，结束服务端与客户端的连接会话
			return E_CLI_HTTP_TALK_FINISHED;
		}

		break;
	}

	//走到这里表示异常，终止服务端与客户端的连接会话
	return E_CLI_HTTP_TALK_FINISHED;
}
