#ifndef __PROTOCOL_TASK_H__
#define __PROTOCOL_TASK_H__

/****************** 刷机请求 *****************/
typedef struct __task_brush_result_request__
{
	char taskState[16];
	char taskStrResult[16];
	char taskFinishTime[64];
} task_brush_res_req_t;

typedef struct __task_brush_request__
{
	char 					taskUuid[64];
	char 					taskGroupId[64];
	char 					taskType[16];
	char 					deviceUuid[64];
	task_brush_res_req_t 	result;
} task_brush_req_t;
/****************** 更新请求 *****************/
typedef struct __task_update_result_request__
{
	char appName[128];
	char appPackage[128];
	char appMd5[33];
	char appVersion[32];
	char downloadUrl[256];
	char cpMarket[64];
} task_update_res_req_t;

typedef struct __task_update_request__
{
	char 					taskUuid[64];
	char 					taskType[16];
	char 					deviceUuid[64];
	task_update_res_req_t 	result;
} task_update_req_t;
/****************** 刷机响应 *****************/
typedef struct __task_brush_task_response__
{
	char fakeDeviceInfo[128];
	char proxyIP[32];
	int proxyPort;
	char cpMarket[64];
	char appName[128];
	char appPackage[128];
	char keyword[64];
	char startTime[64];
} task_brush_task_resp_t;

typedef struct __task_brush_response__
{
	char 					taskUuid[64];
	char 					taskGroupId[64];
	char 					taskType[16];
	char 					deviceUuid[64];
	task_brush_task_resp_t 	task;
} task_brush_resp_t;
/****************** 更新响应 *****************/
typedef struct __task_update_task_response__
{
	char appName[128];
	char appPackage[128];
	char appMd5[33];
	char appVersion[32];
	char downloadUrl[256];
	char cpMarket[64];
} task_update_task_resp_t;

typedef struct __task_update_response__
{
	char 					taskUuid[64];
	char 					taskType[16];
	char 					deviceUuid[64];
	task_update_task_resp_t task;
} task_update_resp_t;
/****************** end *****************/

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
bool protocol_task_pack_brush_request(task_brush_req_t *req, char *req_buf, int bufsize);

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
bool protocol_task_pack_update_request(task_update_req_t *req, char *req_buf, int bufsize);

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
bool protocol_task_pack_brush_response(task_brush_resp_t *resp, char *req_buf, int bufsize);

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
bool protocol_task_pack_update_response(task_update_resp_t *resp, char *req_buf, int bufsize);

/*
* 函数: protocol_task_send_brush_request
* 功能: 发送刷机请求
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_request(event_buf_t *event_buf, task_brush_req_t *req);

/*
* 函数: protocol_task_send_update_request
* 功能: 发送更新请求
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_request(event_buf_t *event_buf, task_update_req_t *req);

/*
* 函数: protocol_task_send_brush_response
* 功能: 发送刷机响应
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_response(event_buf_t *event_buf, task_brush_resp_t *resp);

/*
* 函数: protocol_task_send_update_response
* 功能: 发送更新响应
* 参数: event_buf			event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_response(event_buf_t *event_buf, task_update_resp_t *resp);

#endif


