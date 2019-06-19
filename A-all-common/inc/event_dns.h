#ifndef __EVENT_DNS_H__
#define __EVENT_DNS_H__

#include "event_worker.h"

//dns解析完成后回调
typedef void (*dns_finished_cb)(char *domain, char *ip, void *arg);

bool event_dns_distribute_job(
		char *domain, dns_finished_cb cb, void *arg);

#endif

