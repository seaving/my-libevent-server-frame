#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "http_request.h"
#include "event_executor.h"

bool protocol_parse(event_buf_t *event_buf, const httpRequest_t *httpRequest);

#endif

