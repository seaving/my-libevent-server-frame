#include "includes.h"

/*
* 函数: protocol_probe_pack_request
* 功能: 打包探测请求
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_probe_pack_request(probe_req_t *req, char *req_buf, int bufsize)
{	
	const char *context = NULL;
	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;

	if (req == NULL 
		|| req_buf == NULL 
		|| bufsize <= 0)
	{
		return false;
	}

	json_obj = json_object_new_object();
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		return false;
	}

	data_obj = json_object_new_object();
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		json_object_put(json_obj);
		return false;
	}

	json_object_object_add(data_obj, "test", json_object_new_string(req->test));

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_PROBE));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	protocol_pack(req_buf, bufsize, context);

	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_probe_send_request
* 功能: 发送探测请求
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_probe_send_request(event_buf_t *event_buf, probe_req_t *req)
{
	char buf[1024] = {0};

	if (protocol_probe_pack_request(req, buf, sizeof(buf)) == false)
	{
		LOG_TRACE_NORMAL("protocol_probe_pack_request error !\n");
		return false;
	}

	if (event_send_data(event_buf, buf, strlen(buf)) == false)
	{
		LOG_TRACE_NORMAL("event_send_data error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("send data to server: \n");
	LOG_TRACE_NORMAL("%s\n", buf);
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("\n");

	return true;
}

