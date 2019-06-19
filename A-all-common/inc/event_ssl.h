#ifndef __EVENT_SSL_H__
#define __EVENT_SSL_H__

#include "sys_inc.h"

void event_ssl_lib_init();

SSL_CTX *event_ssl_server_init(char *ca_crt_file, char *svr_crt_file, char *svr_key_file);
SSL_CTX *event_ssl_client_init(char *ca_file, char *crt_file, char *key_file);

#endif

