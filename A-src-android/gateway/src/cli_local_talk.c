#include "includes.h"

//-------------------------------------------------------------------
/*
* 函数: _cli_local_talk_response
* 功能: 发送响应
* 参数: event_buf				event IO 操作指针
*		cli_phone			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline bool _cli_local_talk_response(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	char response[4096] = {0};

	if (strlen(cli_phone->response) <= 0)
	{
		return true;
	}

	snprintf(response, sizeof(response), "%s", cli_phone->response);

	char *data = strstr(response, "\r\n\r\n");
	if (data)
	{
		data = data + strlen("\r\n\r\n");
	}
	else
	{
		data = response;
	}

	LOG_TRACE_NORMAL("Response ==> %s\n", data);

	return proxy_send_data_to_local_client(event_buf, data, strlen(data));
}

/*
* 函数: _cli_local_talk_recv_header
* 功能: 读取http头部信息
* 参数: event_buf				event IO 操作指针
*		cli_phone			客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_local_talk_recv_header(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	int ret = 0;
	for ( ; ; )
	{
		ret = event_recv_data(event_buf, 
				cli_phone->httpRequest.header + 
					cli_phone->httpRequest.header_len, 1);
		if (ret < 0)
		{
			return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
		}
		else if (ret == 0)
		{
			return E_HTTP_REQUEST_RESULT_RECV_EMPTY;
		}

		cli_phone->httpRequest.header_len += 1;
		if (cli_phone->httpRequest.header_len >= sizeof(int))
		{
			cli_phone->httpRequest.context_len = cli_phone->httpRequest.header[0] << 24 
									| cli_phone->httpRequest.header[1] << 16 
									| cli_phone->httpRequest.header[2] << 8 
									| cli_phone->httpRequest.header[3] << 0;
			cli_phone->httpRequest.trans_type = E_TRANS_TYPE_STATIC;
			LOG_TRACE_NORMAL("cli_phone->httpRequest.context_len = %d\n", 
					cli_phone->httpRequest.context_len);
			if (cli_phone->httpRequest.context_len > 1024)
			{
				break;
			}

			return E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS;
		}
	}

	return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
}

/*
* 函数: _cli_local_talk_check_header
* 功能: 校验头部
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: bool
*		- false			失败
* 说明: 
*/
static inline bool _cli_local_talk_check_header(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	return true;
}
		
/*
* 函数: _cli_local_talk_recv_context
* 功能: 读取正文
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: httpRequest_result_t
* 说明: 
*/
static inline httpRequest_result_t _cli_local_talk_recv_context(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	return httpRequest_recv_context(event_buf, &cli_phone->httpRequest);
}

/*
* 函数: _cli_local_frame_parse
* 功能: 解析协议
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _cli_local_frame_parse(
		event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	memset(cli_phone->response, 0, sizeof(cli_phone->response));
	return protocol_parse(cli_phone, event_buf);
}

/*
* 函数: _cli_local_step
* 功能: 流程序列
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_res_t
* 说明: 
*/
static cli_phone_res_t _cli_local_step(event_buf_t *event_buf, cli_phone_t *cli_phone)
{
	httpRequest_result_t httpRequest_result = E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
	switch (cli_phone->step)
	{
		case E_CLI_PHONE_STEP_READ_HEADER:
		{
			LOG_TRACE_NORMAL("_cli_local_talk_recv_header\n");
			httpRequest_result = _cli_local_talk_recv_header(event_buf, cli_phone);
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
			LOG_TRACE_NORMAL("_cli_local_talk_check_header\n");
			if (_cli_local_talk_check_header(event_buf, cli_phone) == true)
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
			LOG_TRACE_NORMAL("_cli_local_talk_recv_context\n");
			httpRequest_result = _cli_local_talk_recv_context(event_buf, cli_phone);
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
			LOG_TRACE_NORMAL("_cli_local_frame_parse\n");
			if (_cli_local_frame_parse(event_buf, cli_phone) == true)
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
			LOG_TRACE_NORMAL("_cli_local_talk_response\n");
			_cli_local_talk_response(event_buf, cli_phone);

			cli_phone->step = E_CLI_PHONE_STEP_READ_HEADER;

			memset(&cli_phone->httpRequest, 0, sizeof(httpRequest_t));
			memset(cli_phone->response, 0, sizeof(cli_phone->response));

			//必须返回once也不能返回finished，否则消息会发不出去
			return E_CLI_PHONE_STEP_RESULT_ONCE;
		}
		case E_CLI_PHONE_STEP_SEND_ERROR:
		{
			LOG_TRACE_NORMAL("cli_phone_talk_response error\n");
			//_cli_local_talk_response(event_buf, cli_phone);

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
* 函数: cli_local_talk
* 功能: 交互入口
* 参数: event_buf 		event IO 操作指针
*		cli_phone		客户端结构体
* 返回: cli_phone_talk_result_t
* 说明: 
*/
cli_phone_talk_result_t cli_local_talk(event_buf_t *event_buf, cli_phone_t *cli_phone)
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
		ret = _cli_local_step(event_buf, cli_phone);
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

