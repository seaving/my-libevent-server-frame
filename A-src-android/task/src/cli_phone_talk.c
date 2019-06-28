#include "includes.h"

typedef enum __cli_phone_step_result__
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
//-------------------------------------------------------------------
/*
* 函数: _cli_phone_talk_response
* 功能: 发送响应
* 参数: event_buf				event IO 操作指针
*		cli_phone			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline bool _cli_phone_talk_response(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	char response[4096] = {0};

	if (strlen(cli_phone->response) <= 0)
	{
		return true;
	}

	snprintf(response, sizeof(response), "%s", cli_phone->response);

	LOG_TRACE_NORMAL("Response ==> %s\n", response);

	return cli_phone_safe_send_data(cli_phone, response, strlen(response));
}

/*
* 函数: _cli_phone_talk_recv_header
* 功能: 读取http头部信息
* 参数: event_buf				event IO 操作指针
*		cli_phone			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_phone_talk_recv_header(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	httpRequest_result_t httpRequest_result;
	httpRequest_result = httpRequest_recv_header(
					event_buf, &cli_phone->httpRequest);
	if (httpRequest_result != E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS)
	{
		return httpRequest_result;
	}

	//LOG_TRACE_NORMAL(">>>>>>>> \n%s", 
		//httpRequest_get_header(&cli_phone->httpRequest));
	return httpRequest_result;
}

/*
* 函数: _cli_phone_talk_check_header
* 功能: 校验头部
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: bool
*		- false			失败
* 说明: 
*/
static inline bool _cli_phone_talk_check_header(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	char x_auth[128] = {0};
	char *header = httpRequest_get_header(&cli_phone->httpRequest);
	if (header == NULL)
	{
		return false;
	}

	if (straddr_case(header, "X-Auth:") != header)
	{
		LOG_TRACE_NORMAL("can't find \"X-Auth\"!\n");
		return false;
	}

	httpRequest_find_field(&cli_phone->httpRequest, "X-Auth", x_auth, sizeof(x_auth));
	if (strcmp(x_auth, X_AUTH) != 0)
	{
		LOG_TRACE_NORMAL("X-Auth error ! \"%s\"\n", x_auth);
		return false;
	}

	return true;
}
		
/*
* 函数: _cli_phone_talk_recv_context
* 功能: 读取正文
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_phone_talk_recv_context(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	return httpRequest_recv_context(event_buf, &cli_phone->httpRequest);
}

/*
* 函数: _cli_phone_frame_parse
* 功能: 解析协议
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _cli_phone_frame_parse(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	memset(cli_phone->response, 0, sizeof(cli_phone->response));
	return protocol_parse(cli_phone, event_buf);
}

/*
* 函数: _cli_phone_step
* 功能: 流程序列
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_res_t
* 说明: 
*/
static cli_phone_res_t _cli_phone_step(event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	httpRequest_result_t httpRequest_result = E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
	switch (cli_phone->step)
	{
		case E_CLI_PHONE_STEP_READ_HEADER:
		{
			LOG_TRACE_NORMAL("_cli_phone_talk_recv_header\n");
			httpRequest_result = _cli_phone_talk_recv_header(event_buf, cli_phone);
			if (httpRequest_result == 
					E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS)
			{
				cli_phone->step = E_CLI_PHONE_STEP_CHECK_HEADER;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			else if (httpRequest_result == 
						E_HTTP_REQUEST_RESULT_RECV_EMPTY)
			{
				return E_CLI_PHONE_STEP_RESULT_ONCE;
			}
			break;
		}
		case E_CLI_PHONE_STEP_CHECK_HEADER:
		{
			LOG_TRACE_NORMAL("_cli_phone_talk_check_header\n");
			if (_cli_phone_talk_check_header(event_buf, cli_phone) == true)
			{
				cli_phone->step = E_CLI_PHONE_STEP_READ_CONTEXT;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			else
			{
				cli_phone->step = E_CLI_PHONE_STEP_SEND_ERROR;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			break;
		}
		case E_CLI_PHONE_STEP_READ_CONTEXT:
		{
			LOG_TRACE_NORMAL("_cli_phone_talk_recv_context\n");
			httpRequest_result = _cli_phone_talk_recv_context(event_buf, cli_phone);
			if (httpRequest_result == 
					E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS)
			{
				cli_phone->step = E_CLI_PHONE_STEP_FRAME_PARSE;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			else if (httpRequest_result == 
						E_HTTP_REQUEST_RESULT_RECV_EMPTY)
			{
				return E_CLI_PHONE_STEP_RESULT_ONCE;
			}
			break;
		}
		case E_CLI_PHONE_STEP_FRAME_PARSE:
		{
			LOG_TRACE_NORMAL("_cli_phone_frame_parse\n");
			if (_cli_phone_frame_parse(event_buf, cli_phone) == true)
			{
				cli_phone->step = E_CLI_PHONE_STEP_SEND_SUCCESS;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			else
			{
				cli_phone->step = E_CLI_PHONE_STEP_SEND_ERROR;
				return E_CLI_PHONE_STEP_RESULT_AGAIN;
			}
			break;
		}
		/************************ Response ************************/
		case E_CLI_PHONE_STEP_SEND_SUCCESS:
		{
			LOG_TRACE_NORMAL("_cli_phone_talk_response\n");
			_cli_phone_talk_response(event_buf, cli_phone);

			cli_phone->step = E_CLI_PHONE_STEP_READ_HEADER;

			memset(&cli_phone->httpRequest, 0, sizeof(httpRequest_t));
			memset(cli_phone->response, 0, sizeof(cli_phone->response));

			//必须返回once也不能返回finished，否则消息会发不出去
			return E_CLI_PHONE_STEP_RESULT_ONCE;
		}
		case E_CLI_PHONE_STEP_SEND_ERROR:
		{
			LOG_TRACE_NORMAL("cli_phone_talk_response error\n");
			//_cli_phone_talk_response(event_buf, cli_phone);

			cli_phone->step = E_CLI_PHONE_STEP_FINISHED;

			memset(&cli_phone->httpRequest, 0, sizeof(httpRequest_t));
			memset(cli_phone->response, 0, sizeof(cli_phone->response));

		#if 0
			//必须返回once也不能返回finished，否则消息会发不出去
			return E_CLI_PHONE_STEP_RESULT_ONCE;
		#else
			//立刻结束
			return E_CLI_PHONE_STEP_RESULT_FINISHED;
		#endif
		}
		case E_CLI_PHONE_STEP_FINISHED:
		default:
		{
			memset(&cli_phone->httpRequest, 0, sizeof(httpRequest_t));

			//结束所有流程
			return E_CLI_PHONE_STEP_RESULT_FINISHED;
		}
	}

	//默认结束所有流程
	return E_CLI_PHONE_STEP_RESULT_FINISHED;
}

/*
* 函数: cli_phone_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 
*/
cli_phone_talk_result_t cli_phone_talk(event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	cli_phone_res_t ret = E_CLI_PHONE_STEP_RESULT_FINISHED;
	if (cli_phone == NULL 
		|| event_buf == NULL)
	{
		//走到这里表示异常，终止服务端与客户端的连接会话
		return E_CLI_PHONE_TALK_FINISHED;
	}

	for ( ; ; )
	{
		ret = _cli_phone_step(event_buf, cli_phone);
		if (ret == E_CLI_PHONE_STEP_RESULT_AGAIN)
		{
			//继续轮询执行步骤
			continue;
		}
		if (ret == E_CLI_PHONE_STEP_RESULT_ONCE)
		{
			//结束本次步骤执行，继续服务端与客户端的连接会话
			return E_CLI_PHONE_TALK_CONTINUE;
		}
		if (ret == E_CLI_PHONE_STEP_RESULT_FINISHED)
		{
			//结束本次步骤执行，结束服务端与客户端的连接会话
			return E_CLI_PHONE_TALK_FINISHED;
		}

		break;
	}

	//走到这里表示异常，终止服务端与客户端的连接会话
	return E_CLI_PHONE_TALK_FINISHED;
}

