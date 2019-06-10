#ifndef __SCHEDULE_SERVER_cb_H__
#define __SCHEDULE_SERVER_cb_H__

#include "sys_inc.h"
#include "event_executor.h"

bool schedule_server_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);
bool schedule_server_read_cb(event_buf_t *event_buf, int conn_fd, void *arg);
bool schedule_server_write_cb(event_buf_t *event_buf, int conn_fd, void *arg);
bool schedule_server_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg);

bool schedule_server_user_data_init_cb(talk_user_data_t *user_data);

#endif


