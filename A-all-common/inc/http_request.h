#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "event_executor.h"

#define HTTP_REQUEST_HEADER_BUF_SIZE	1024

typedef enum __http_request_result__
{
	E_HTTP_REQUEST_RESULT_HEADER_BUF_FULL = 0,
	E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS,
	E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS,
	E_HTTP_REQUEST_RESULT_RECV_EMPTY,
	E_HTTP_REQUEST_RESULT_SOCKET_ERROR
} httpRequest_result_t;

typedef struct __http_request__
{
	char header[8192];
	int header_len;
	char *context_buf;
	int context_len;
	int data_len;
} httpRequest_t;

bool httpRequest_find_field(httpRequest_t *httpRequest, 
		char *find, char *value_buf, int value_buf_size);

bool httpRequest_get_url_parmer_value(
		httpRequest_t *httpRequest, 
		char *parmer_key, int parmer_key_len, 
		char *parmer_value_buf, int parmer_value_buf_size);

bool httpRequest_get_url_path(
		httpRequest_t *httpRequest, 
		char *buf, int buf_size);

int httpRequest_get_content_length(httpRequest_t *httpRequest);

#define httpRequest_get_header(httpRequest) (httpRequest)->header
#define httpRequest_get_header_len(httpRequest) (httpRequest)->header_len
#define httpRequest_get_context_buf(httpRequest) (httpRequest)->context_buf
#define httpRequest_get_context_len(httpRequest) \
		(httpRequest)->context_len <= 0 ? \
			httpRequest_get_content_length(httpRequest) : \
				(httpRequest)->context_len

#define httpRequest_get_context_data_len(httpRequest) (httpRequest)->data_len

#define httpRequest_get_header_parmer(httpRequest, key, value_buf, buf_size) \
			httpRequest_find_field(httpRequest, key, value_buf, buf_size)

#define httpRequest_get_url_parmer(httpRequest, key, value_buf, buf_size) \
			httpRequest_get_url_parmer_value( \
				httpRequest, key, key ? strlen(key) : 0, value_buf, buf_size)

httpRequest_result_t httpRequest_recv_header(
		event_buf_t *event_buf, httpRequest_t *httpRequest);

httpRequest_result_t httpRequest_recv_context(
		event_buf_t *event_buf, httpRequest_t *httpRequest);

httpRequest_t *httpRequest_new();
void httpRequest_free(httpRequest_t *httpRequest);

void httpRequest_context_free(httpRequest_t *httpRequest);

#endif

