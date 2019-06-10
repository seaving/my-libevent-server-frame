#ifndef __EVENT_SERVER_cb_H__
#define __EVENT_SERVER_cb_H__

#include "sys_inc.h"

void event_server_accept_cb(evutil_socket_t listen_fd, short ev, void *arg);

#endif


