#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
	超时时间(秒):
	在此时间内IO没有任何操作的空闲时间
*/
#define CLIENT_TIMEOUT_SEC			25

/*
	线程池中线程数量
*/
#define THREAD_POOL_MAX_SIZE		32

/*
	EVENT 工作线程队列长度
*/
#define EVENT_WORKER_QUEUE_SIZE		32

/*
	EVENT 工作线程数量
*/
#define EVENT_WORKER_MAX_SIZE		THREAD_POOL_MAX_SIZE - 10

/*
	schedule服务器地址
*/
#define SCHEDULE_SERVER_HOST	    "127.0.0.1"

/*
	schedule服务器端口号
*/
#define SCHEDULE_SERVER_PORT	    9111

/*
	程序运行日志保存目录
*/
#define LOG_OUT_DIR					"./log"

/*
	程序运行日志文件名称
	stdout 标准输出
*/
#define LOG_STDOUT_FILE				"schedule_server_stdout.log"

/*
	程序运行日志文件名称
	stderr 标准错误输出
*/
#define LOG_STDERR_FILE				"schedule_server_stderr.log"

#endif


