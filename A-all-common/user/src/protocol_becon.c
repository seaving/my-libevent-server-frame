#include "includes.h"

/*
* 函数: protocol_becon_pack_response
* 功能: 打包信标帧响应
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_becon_pack_response(becon_resp_t *resp, char *req_buf, int bufsize)
{	
	const char *context = NULL;
	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;

	if (resp == NULL 
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

	json_object_object_add(data_obj, "test", json_object_new_string(resp->test));

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_BECON));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	snprintf(req_buf, bufsize, "%s", context);

	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_becon_send_response
* 功能: 发送信标响应
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_becon_send_response(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, becon_resp_t *resp)
{
	char data[1024] = {0};
	char pack[1124] = {0};
	if (protocol_becon_pack_response(resp, data, sizeof(data)) == false)
	{
		LOG_TRACE_NORMAL("protocol_probe_pack_request error !\n");
		return false;
	}

	if (protocol_route_pack(src, dst, data, pack, sizeof(pack)) == false)
	{
		LOG_TRACE_NORMAL("protocol_route_pack error !\n");
		return false;
	}

	if (event_send_data(event_buf, pack, strlen(pack)) == false)
	{
		LOG_TRACE_NORMAL("event_send_data error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("send data to server: \n");
	LOG_TRACE_NORMAL("%s\n", pack);
	LOG_TRACE_NORMAL("------------------------------------\n");
	LOG_TRACE_NORMAL("\n");

	return true;
}

