#ifndef __EVENT_CONNECT_cb_H__
#define __EVENT_CONNECT_cb_H__

#include "event_worker.h"

void event_connect_timer_cb(evutil_socket_t fd, short ev, void *arg);
void event_connect_buffered_read_cb(struct bufferevent *bev, void *arg);
void event_connect_buffered_write_cb(struct bufferevent *bev, void *arg);
void event_connect_buffered_event_cb(struct bufferevent *bev, short what, void *arg);

#endif

