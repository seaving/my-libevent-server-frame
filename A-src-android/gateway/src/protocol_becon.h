#ifndef __PROTOCOL_BECON_H__
#define __PROTOCOL_BECON_H__


typedef struct __becon_response__
{
	char test[16];
} becon_resp_t;

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
bool protocol_becon_pack_response(becon_resp_t *resp, char *req_buf, int bufsize);

/*
* 函数: protocol_becon_send_response
* 功能: 发送信标响应
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_becon_send_response(event_buf_t *event_buf, becon_resp_t *resp);

#endif
