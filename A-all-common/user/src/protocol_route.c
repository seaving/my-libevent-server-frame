#include "includes.h"

/*
* 函数: protocol_route_pack
* 功能: 打包路由处理协议
* 参数:	src			来源
*		dst			目的
*		data		数据
*		buf			缓存
*		bufsize		长度
* 返回: bool
*		- false 		失败
* 说明: 
*/
bool protocol_route_pack(protocol_route_t src, 
		protocol_route_t dst, char *data, 
		char *buf, int bufsize)
{
	const char *context = NULL;
	struct json_object *json_obj = NULL;
	struct json_object *data_obj = NULL;

	if (data == NULL 
		|| buf == NULL 
		|| bufsize <= 0 
		|| src == E_PROTOCOL_ROUTE_UNKOWN 
		|| dst == E_PROTOCOL_ROUTE_UNKOWN)
	{
		return false;
	}

	data_obj = json_tokener_parse(data);
	if (data_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}

	json_obj = json_object_new_object();
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_new_object error !\n");
		return false;
	}

	json_object_object_add(json_obj, "src", json_object_new_int(src));
	json_object_object_add(json_obj, "dst", json_object_new_int(dst));
	json_object_object_add(json_obj, "data", data_obj);

	context = json_object_to_json_string(json_obj);
	protocol_pack(buf, bufsize, context);

	json_object_put(json_obj);

	return true;
}

/*
* 函数: protocol_route_parse
* 功能: 解析路由处理协议
* 参数:	context			json字符串
*		info			保存信息
* 返回: bool
*		- false 		失败
* 说明: 
*/
bool protocol_route_parse(char *context, protocol_route_info_t *info)
{
	struct json_object *json_obj = NULL;
	struct json_object *src_obj = NULL;
	struct json_object *dst_obj = NULL;
	struct json_object *rdata_obj = NULL;
	struct json_object *data_obj = NULL;
	struct json_object *cmd_obj = NULL;
	if (context == NULL 
		|| info == NULL)
	{
		return false;
	}

	json_obj = json_tokener_parse(context);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_tokener_parse error !\n");
		return false;
	}

	json_object_get(json_obj, "src", src_obj);
	if (src_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(json_obj, "dst", dst_obj);
	if (dst_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(json_obj, "data", rdata_obj);
	if (rdata_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(rdata_obj, "cmd", cmd_obj);
	if (cmd_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	json_object_get(rdata_obj, "data", data_obj);
	if (data_obj == NULL)
	{
		json_object_put(json_obj);
		return false;
	}

	info->src = json_object_get_int(src_obj);
	info->dst = json_object_get_int(dst_obj);
	info->cmd = json_object_get_int(cmd_obj);
	snprintf(info->data, sizeof(info->data), "%s", json_object_to_json_string(data_obj));

	json_object_put(json_obj);

	return true;
}

