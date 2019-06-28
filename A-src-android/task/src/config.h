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
#define THREAD_POOL_MAX_SIZE		1

/*
	EVENT 工作线程队列长度
*/
#define EVENT_WORKER_QUEUE_SIZE		255

/*
	本地服务配置
*/
#define SOCKET_LOCAL_NAME       "gw@localserver"

/*
	程序运行日志保存目录
*/
#define LOG_OUT_DIR					"/data/local/tmp/log"

/*
	程序运行日志文件名称
	stdout 标准输出
*/
#define LOG_STDOUT_FILE				"task_stdout.log"

/*
	程序运行日志文件名称
	stderr 标准错误输出
*/
#define LOG_STDERR_FILE				"task_stderr.log"

#endif


