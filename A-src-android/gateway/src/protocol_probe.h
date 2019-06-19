#ifndef __PROTOCOL_HEART_H__
#define __PROTOCOL_HEART_H__

typedef struct __probe_request__
{
	char test[16];
} probe_req_t;

/*
* 函数: protocol_probe_pack_request
* 功能: 打包探测请求
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_probe_pack_request(probe_req_t *req, char *req_buf, int bufsize);

/*
* 函数: protocol_probe_send_request
* 功能: 发送探测请求
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_probe_send_request(event_buf_t *event_buf, probe_req_t *req);

#endif

