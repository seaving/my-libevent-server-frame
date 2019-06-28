#ifndef __PROTOCOL_ROUTE_H__
#define __PROTOCOL_ROUTE_H__

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

typedef enum __protocol_route__
{
	E_PROTOCOL_ROUTE_SCHEDULE = 0,
	E_PROTOCOL_ROUTE_GATEWAY,
	E_PROTOCOL_ROUTE_TASK,
	E_PROTOCOL_ROUTE_UNKOWN
} protocol_route_t;

typedef struct __protocol_route_info__
{
	protocol_route_t src;
	protocol_route_t dst;
	int cmd;
	char data[2048];
} protocol_route_info_t;

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
		char *buf, int bufsize);

/*
* 函数: protocol_route_parse
* 功能: 解析路由处理协议
* 参数:	context 		json字符串
*		info			保存信息
* 返回: bool
*		- false 		失败
* 说明: 
*/
bool protocol_route_parse(char *context, protocol_route_info_t *info);

#endif

