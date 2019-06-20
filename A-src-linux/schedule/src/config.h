#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
	超时时间(秒):
	在此时间内IO没有任何操作的空闲时间
*/
#define CLIENT_TIMEOUT_SEC			60

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
	schedule服务器端口号
*/
#define SCHEDULE_SERVER_BIND_PORT			    9111

/*
	schedule服务器监听队列长度
*/
#define SCHEDULE_SERVER_LISTEN_QUEUE_SIZE	    32

/*
	schedule服务器最多处理客户端数量
*/
#define SCHEDULE_CLIENT_ACCEPT_MAX_COUNT		65530

/*
	schedule服务器SSL证书
*/
#define SSL_CA_FILE_DIR         "/etc/cloud_ctrl"
#define	DEFAULT_CA_FILE			SSL_CA_FILE_DIR"/ssl-ca/ca.cert"
#define	DEFAULT_CRT_FILE		SSL_CA_FILE_DIR"/ssl-ca/server.crt"
#define	DEFAULT_KEY_FILE		SSL_CA_FILE_DIR"/ssl-ca/server.key"

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

