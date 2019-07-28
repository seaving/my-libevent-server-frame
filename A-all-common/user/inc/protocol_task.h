#ifndef __PROTOCOL_TASK_H__
#define __PROTOCOL_TASK_H__

/****************** 刷机结果上报 *****************/
typedef struct __task_brush_result__
{
	int taskState;				//刷量任务状态，见定义的code
	char taskStrResult[16];		//刷量任务字符串描述
	char taskFinishTime[64];	//完成时间
} task_brush_res_t;

typedef struct __task_brush_result_report__
{
	char 					taskUuid[64];		//任务id
	int 					taskType;			//任务类型:1 刷量任务、2 更新任务
	char 					deviceUuid[64];		//设备uuid
	task_brush_res_t 		taskResult;			//任务执行结果
} task_brush_report_t;
/****************** 更新结果上报 *****************/
typedef struct __task_update_result__
{
	char appName[128];							//应用名称
	char appPackage[128];						//应用包名
	char appMd5[33];							//apkMd5信息
	char appVersion[32];						//版本versionCode
	char downloadUrl[256];						//apk下载地址
	char cpMarket[64];							//对应的应用市场
} task_update_res_t;

typedef struct __task_update_request_report__
{
	char 					taskUuid[64];		//任务id
	int 					taskType;			//任务类型:1 刷量任务、2 更新任务
	char 					deviceUuid[64];		//设备uuid
	task_update_res_t 		taskResult;			//任务执行结果
} task_update_report_t;
/****************** 刷机任务分配 *****************/
typedef struct __task_brush_task_alloc__
{
	char fakeDeviceInfo[128];					//软改信息
	char proxyIP[32];							//代理ip
	int proxyPort;								//代理端口号
	char cpMarket[64];							//对应的应用市场
	char appName[128];							//应用名称
	char appPackage[128];						//应用包名
	char keyword[64];							//搜索关键字
	char startTime[64];							//开始时间
} task_brush_task_alloc_t;

typedef struct __task_brush_alloc__
{
	char 					taskUuid[64];		//任务id
	int 					taskType;			//任务类型:1 刷量任务、2 更新任务
	char 					deviceUuid[64];		//设备id
	task_brush_task_alloc_t task;				//任务信息
} task_brush_alloc_t;
/****************** 更新任务分配 *****************/
typedef struct __task_update_task_alloc__
{
	char appName[128];							//应用名称
	char appPackage[128];						//应用包名
	char appMd5[33];							//apkMd5信息
	char appVersion[32];						//版本versioCode
	char downloadUrl[256];						//apk下载地址
	char cpMarket[64];							//对应的应用市场
} task_update_task_alloc_t;

typedef struct __task_update_response__
{
	char 					taskUuid[64];		//任务id
	int 					taskType;			//任务类型:1 刷量任务、2 更新任务
	char 					deviceUuid[64];		//设备id
	task_update_task_alloc_t task;				//任务信息
} task_update_alloc_t;
/****************** end *****************/

/*
* 函数: protocol_task_pack_brush_result_report
* 功能: 打包刷机结果上报
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_brush_result_report(task_brush_report_t *req, char *req_buf, int bufsize);

/*
* 函数: protocol_task_pack_update_request_report
* 功能: 打包更新结果上报
* 参数: req				请求结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_update_request_report(task_update_report_t *req, char *req_buf, int bufsize);

/*
* 函数: protocol_task_pack_brush_alloc
* 功能: 打包刷机分配
* 参数: resp				响应结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_brush_alloc(task_brush_alloc_t *resp, char *req_buf, int bufsize);

/*
* 函数: protocol_task_pack_update_alloc
* 功能: 打包更新分配
* 参数: resp				响应结构体数据
*		req_buf			缓存
*		bufsize			缓存大小
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_pack_update_alloc(task_update_alloc_t *resp, char *req_buf, int bufsize);

/*
* 函数: protocol_task_send_brush_report
* 功能: 发送刷机结果上报
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_report(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_brush_report_t *req);

/*
* 函数: protocol_task_send_update_report
* 功能: 发送更新结果上报
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_report(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_update_report_t *req);

/*
* 函数: protocol_task_send_brush_alloc
* 功能: 发送刷机任务分配
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_brush_alloc(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_brush_alloc_t *resp);

/*
* 函数: protocol_task_send_update_alloc
* 功能: 发送更新任务分配
* 参数: src				来源
*		dst				目的
*		event_buf		event IO 操作指针
*		req				请求结构体数据
* 返回: bool
*		- false		 	失败
* 说明: 
*/
bool protocol_task_send_update_alloc(
		protocol_route_t src, protocol_route_t dst, 
		event_buf_t *event_buf, task_update_alloc_t *resp);

#endif

