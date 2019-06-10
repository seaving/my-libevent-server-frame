#include "sys_inc.h"
#include "log_trace.h"
#include "event_service.h"
#include "event_dns.h"
#include "event_connect.h"

typedef struct __dns_cb__
{
	dns_finished_cb cb;
	void *arg;
} dns_cb_t;

typedef struct __dns__
{
	char *domain;
	dns_cb_t cb;
} dns_t;

static void _dns_node_free(void *arg);

static dns_t *_dns_node_create(char *domain, dns_finished_cb cb, void *arg)
{
	dns_t *dns = NULL;

	if (domain == NULL)
	{
		return NULL;
	}

	dns = calloc(sizeof(dns_t), 1);
	if (dns == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return NULL;
	}

	dns->domain = calloc(1, strlen(domain) + 1);
	if (dns->domain == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		free(dns);
		return NULL;
	}

	memcpy(dns->domain, domain, strlen(domain));

	dns->cb.cb = cb;
	dns->cb.arg = arg;

	return dns;
}

static void _dns_node_free(void *arg)
{
	dns_t *dns = (dns_t *) arg;
	if (dns)
	{
		if (dns->domain)
		{
			free(dns->domain);
		}

		free(dns);
	}
}

static void _event_dns_cb(int errcode, struct evutil_addrinfo *addr, void *ptr)
{
	char buf[128] = {0};
	const char *s = NULL;
	const char *ai_canonname = NULL;
	struct sockaddr_in *sin = NULL;
	struct sockaddr_in6 *sin6 = NULL;
	struct evutil_addrinfo *ai = NULL;
	dns_t *dns = (dns_t *) ptr;

	if (dns == NULL 
		|| dns->domain == NULL)
	{
		event_service_job_handling_count(-1);
		return;
	}

	if (errcode != 0)
	{
		LOG_TRACE_NORMAL("get <%s> dns error! %s\n", dns->domain, evutil_gai_strerror(errcode));
	}
	else
	{
		if (addr && addr->ai_canonname)
		{
			ai_canonname = addr->ai_canonname;
		}

		for (ai = (struct evutil_addrinfo *) addr; ai != NULL; 
				ai = (struct evutil_addrinfo *) ai->ai_canonname)
		{
			if (ai->ai_family == AF_INET)
			{
				sin = (struct sockaddr_in *) ai->ai_addr;
				s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
			}
			else if(ai->ai_family == AF_INET6)
			{
				sin6 = (struct sockaddr_in6 *) ai->ai_addr;
				s = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, buf, 128);
			}
			else
			{

			}

			if (s)
			{
				LOG_TRACE_NORMAL("[domain = %s][ai_canonname = %s] -> %s\n", 
						dns->domain, ai_canonname ? ai_canonname : "", s);
				break;
			}
		}

		if (dns->cb.cb)
		{
			dns->cb.cb(dns->domain, (char *) s, dns->cb.arg);
		}

		if (addr)
		{
			evutil_freeaddrinfo(addr);
		}
	}

	_dns_node_free(dns);
	event_service_job_handling_count(-1);
}

static bool _event_dns_executor(
	struct event_base *evbase, 
	event_worker_t *worker, 
	void *arg, 
	void (*free_arg_cb)(void *), 
	unsigned int timer_out, 
	event_cb_fn timer_cb, 
	bufferevent_data_cb read_cb, 
	bufferevent_data_cb write_cb, 
    bufferevent_event_cb error_cb)
{
	dns_t *dns = (dns_t *) arg;
	struct evutil_addrinfo hints;
	struct evdns_getaddrinfo_request *req = NULL;

	if (dns == NULL 
		|| dns->domain == NULL)
	{
		return false;
	}

	memset(&hints, 0, sizeof(struct evutil_addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = EVUTIL_AI_CANONNAME;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	req = evdns_getaddrinfo(worker->evdnsbase, 
			dns->domain, NULL, &hints, _event_dns_cb, dns);
	if (req == NULL)
	{
		//LOG_TRACE_NORMAL("evdns_getaddrinfo error!\n");
		//dns_free(dns); -->不管evdns_getaddrinfo返回错误还是成功，
							//都会调用_event_dns_cb回调函数
		//return false; //因此dns_free不需要再此执行，同时return ture，当成是正常的一次任务
	}

	return true;
}

bool event_dns_distribute_job(
		char *domain, dns_finished_cb cb, void *arg)
{
	dns_t *dns = _dns_node_create(domain, cb, arg);
	if (dns == NULL)
	{
		return false;
	}

	if (event_service_distribute_job(dns, _dns_node_free, 0, 
			_event_dns_executor, NULL, NULL, NULL, NULL) == false)
	{
		_dns_node_free(dns);
		return false;
	}

	return true;
}


