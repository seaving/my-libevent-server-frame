#include "includes.h"

/*
* 函数: protocol_task_pack_brush_request
* 功能: 打包刷机请求
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_brush_request(task_brush_req_t *req, char *req_buf, int bufsize)
{	
	const char *context = NULL;

	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;
	struct json_object *task_result_obj = NULL;

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

	task_result_obj = json_object_new_object();
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		json_object_put(data_obj);
		json_object_put(json_obj);
		return false;
	}

	json_object_object_add(task_result_obj, "taskState", json_object_new_string(req->result.taskState));
	json_object_object_add(task_result_obj, "taskStrResult", json_object_new_string(req->result.taskStrResult));
	json_object_object_add(task_result_obj, "taskFinishTime", json_object_new_string(req->result.taskFinishTime));

	json_object_object_add(data_obj, "taskUuid", json_object_new_string(req->taskUuid));
	json_object_object_add(data_obj, "taskGroupId", json_object_new_string(req->taskGroupId));
	json_object_object_add(data_obj, "taskType", json_object_new_string(req->taskType));
	json_object_object_add(data_obj, "deviceUuid", json_object_new_string(req->deviceUuid));
	json_object_object_add(data_obj, "taskResult", task_result_obj);

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_TASK_REQUEST));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	snprintf(req_buf, bufsize, "%s", context);
	
	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_task_pack_update_request
* 功能: 打包更新请求
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_update_request(task_update_req_t *req, char *req_buf, int bufsize)
{	
	const char *context = NULL;

	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;
	struct json_object *task_result_obj = NULL;

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

	task_result_obj = json_object_new_object();
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		json_object_put(data_obj);
		json_object_put(json_obj);
		return false;
	}

	json_object_object_add(task_result_obj, "appName", json_object_new_string(req->result.appName));
	json_object_object_add(task_result_obj, "appPackage", json_object_new_string(req->result.appPackage));
	json_object_object_add(task_result_obj, "appMd5", json_object_new_string(req->result.appMd5));
	json_object_object_add(task_result_obj, "appVersion", json_object_new_string(req->result.appVersion));
	json_object_object_add(task_result_obj, "downloadUrl", json_object_new_string(req->result.downloadUrl));
	json_object_object_add(task_result_obj, "cpMarket", json_object_new_string(req->result.cpMarket));

	json_object_object_add(data_obj, "taskUuid", json_object_new_string(req->taskUuid));
	json_object_object_add(data_obj, "taskType", json_object_new_string(req->taskType));
	json_object_object_add(data_obj, "deviceUuid", json_object_new_string(req->deviceUuid));
	json_object_object_add(data_obj, "taskResult", task_result_obj);

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_TASK_REQUEST));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	snprintf(req_buf, bufsize, "%s", context);

	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_task_pack_brush_response
* 功能: 打包刷机响应
* 参数: resp				响应结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_brush_response(task_brush_resp_t *resp, char *req_buf, int bufsize)
{	
	const char *context = NULL;

	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;
	struct json_object *task_obj = NULL;

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

	task_obj = json_object_new_object();
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		json_object_put(data_obj);
		json_object_put(json_obj);
		return false;
	}

	json_object_object_add(task_obj, "fakeDeviceInfo", json_object_new_string(resp->task.fakeDeviceInfo));
	json_object_object_add(task_obj, "proxyIP", json_object_new_string(resp->task.proxyIP));
	json_object_object_add(task_obj, "proxyPort", json_object_new_int(resp->task.proxyPort));
	json_object_object_add(task_obj, "cpMarket", json_object_new_string(resp->task.cpMarket));
	json_object_object_add(task_obj, "appName", json_object_new_string(resp->task.appName));
	json_object_object_add(task_obj, "appPackage", json_object_new_string(resp->task.appPackage));
	json_object_object_add(task_obj, "keyword", json_object_new_string(resp->task.keyword));
	json_object_object_add(task_obj, "startTime", json_object_new_string(resp->task.startTime));

	json_object_object_add(data_obj, "taskUuid", json_object_new_string(resp->taskUuid));
	json_object_object_add(data_obj, "taskGroupId", json_object_new_string(resp->taskGroupId));
	json_object_object_add(data_obj, "taskType", json_object_new_string(resp->taskType));
	json_object_object_add(data_obj, "deviceUuid", json_object_new_string(resp->deviceUuid));
	json_object_object_add(data_obj, "task", task_obj);

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_TASK_RESPONSE));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	snprintf(req_buf, bufsize, "%s", context);
	
	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_task_pack_update_response
* 功能: 打包更新响应
* 参数: resp				响应结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_update_response(task_update_resp_t *resp, char *req_buf, int bufsize)
{	
	const char *context = NULL;

	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;
	struct json_object *task_result_obj = NULL;

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

	task_result_obj = json_object_new_object();
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		json_object_put(data_obj);
		json_object_put(json_obj);
		return false;
	}

	json_object_object_add(task_result_obj, "appName", json_object_new_string(resp->task.appName));
	json_object_object_add(task_result_obj, "appPackage", json_object_new_string(resp->task.appPackage));
	json_object_object_add(task_result_obj, "appMd5", json_object_new_string(resp->task.appMd5));
	json_object_object_add(task_result_obj, "appVersion", json_object_new_string(resp->task.appVersion));
	json_object_object_add(task_result_obj, "downloadUrl", json_object_new_string(resp->task.downloadUrl));
	json_object_object_add(task_result_obj, "cpMarket", json_object_new_string(resp->task.cpMarket));

	json_object_object_add(data_obj, "taskUuid", json_object_new_string(resp->taskUuid));
	json_object_object_add(data_obj, "taskType", json_object_new_string(resp->taskType));
	json_object_object_add(data_obj, "deviceUuid", json_object_new_string(resp->deviceUuid));
	json_object_object_add(data_obj, "task", task_result_obj);

	json_object_object_add(json_obj, "cmd", json_object_new_int(E_PROTOCOL_CMD_TASK_RESPONSE));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	snprintf(req_buf, bufsize, "%s", context);

	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_task_send_brush_request
* 功能: 发送刷机请求
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_request(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_brush_req_t *req)
{
	char data[1024] = {0};
	char pack[1124] = {0};

	if (event_buf == NULL 
		|| req == NULL)
	{
		return false;
	}

	if (protocol_task_pack_brush_request(req, data, sizeof(data)) == false)
	{
		LOG_TRACE_NORMAL("protocol_task_brush_pack_request error !\n");
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

/*
* 函数: protocol_task_send_update_request
* 功能: 发送更新请求
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_request(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_update_req_t *req)
{
	char data[1024] = {0};
	char pack[1124] = {0};

	if (event_buf == NULL 
		|| req == NULL)
	{
		return false;
	}

	if (protocol_task_pack_update_request(req, data, sizeof(data)) == false)
	{
		LOG_TRACE_NORMAL("protocol_task_update_pack_request error !\n");
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

/*
* 函数: protocol_task_send_brush_response
* 功能: 发送刷机响应
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_response(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_brush_resp_t *resp)
{
	char data[1024] = {0};
	char pack[1124] = {0};

	if (event_buf == NULL 
		|| resp == NULL)
	{
		return false;
	}

	if (protocol_task_pack_brush_response(resp, data, sizeof(data)) == false)
	{
		LOG_TRACE_NORMAL("protocol_task_pack_brush_response error !\n");
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

/*
* 函数: protocol_task_send_update_response
* 功能: 发送更新响应
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_response(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_update_resp_t *resp)
{
	char data[1024] = {0};
	char pack[1124] = {0};

	if (event_buf == NULL 
		|| resp == NULL)
	{
		return false;
	}

	if (protocol_task_pack_update_response(resp, data, sizeof(data)) == false)
	{
		LOG_TRACE_NORMAL("protocol_task_pack_update_response error !\n");
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

