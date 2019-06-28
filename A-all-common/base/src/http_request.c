#include "sys_inc.h"
#include "http_request.h"
#include "log_trace.h"
#include "user_string.h"

#define is_http_request_header_buf_full(httpRequest) \
	(httpRequest->header_len >= HTTP_REQUEST_HEADER_BUF_SIZE - 1)

static inline httpRequest_t *_httpRequest_new()
{
	httpRequest_t *httpRequest = calloc(1, sizeof(httpRequest_t));
	if (httpRequest == NULL)
	{
		LOG_TRACE_PERROR("calloc error!");
		return NULL;
	}

	return httpRequest;
}

static inline bool _httpRequest_context_buf_init(httpRequest_t *httpRequest)
{
	if (httpRequest 
		&& httpRequest->context_buf == NULL)
	{
		httpRequest->context_len = 
				httpRequest_get_content_length(httpRequest);
		//LOG_TRACE_NORMAL("httpRequest->context_len = %d\n", 
			//	httpRequest->context_len);
		if (httpRequest->context_len > 0)
		{
			httpRequest->context_buf = calloc(1, httpRequest->context_len + 1);
			if (httpRequest->context_buf == NULL)
			{
				LOG_TRACE_PERROR("calloc error!");
				return false;
			}
		}
	}

	return (httpRequest 
		&& httpRequest->context_buf 
		&& httpRequest->context_len > 0);
}

static inline void _httpRequest_free(httpRequest_t *httpRequest)
{
	if (httpRequest)
	{
		if (httpRequest->context_buf)
			free(httpRequest->context_buf);
		free(httpRequest);
	}
}

static httpRequest_result_t _httpRequest_recv_header(
		event_buf_t *event_buf, httpRequest_t *httpRequest)
{
	int len = 0;
	char ch;

	if (is_http_request_header_buf_full(httpRequest))
	{
		return E_HTTP_REQUEST_RESULT_HEADER_BUF_FULL;
	}

	for ( ; ; )
	{
		len = event_recv_data(event_buf, &ch, 1);
		if (len < 0)
		{
			return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
		}

		if (len == 0)
		{
			return E_HTTP_REQUEST_RESULT_RECV_EMPTY;
		}

		httpRequest->header[httpRequest->header_len] = ch;
		httpRequest->header_len += len;
		if (is_http_request_header_buf_full(httpRequest))
		{
			return E_HTTP_REQUEST_RESULT_HEADER_BUF_FULL;
		}

		if (straddr(httpRequest->header, "\r\n\r\n"))
		{
			return E_HTTP_REQUEST_RESULT_HEADER_RECV_SUCCESS;
		}
	}

	return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
}

static httpRequest_result_t _httpRequest_recv_context(
		event_buf_t *event_buf, httpRequest_t *httpRequest)
{
	int need_read_len = 2048;
	int read_len = 0;
	
	for ( ; ; )
	{
		need_read_len = MIN(need_read_len, 
				httpRequest->context_len - httpRequest->data_len);
		read_len = event_recv_data(event_buf, 
				httpRequest->context_buf + httpRequest->data_len, 
					need_read_len);
		if (read_len < 0)
		{
			return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
		}

		if (read_len == 0)
		{
			return E_HTTP_REQUEST_RESULT_RECV_EMPTY;
		}
		
		httpRequest->data_len += read_len;
		httpRequest->context_buf[httpRequest->data_len] = '\0';
		if (httpRequest->context_len - httpRequest->data_len <= 0)
		{
			return E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS;
		}
	}

	return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
}

static bool _httpRequest_get_url(
		httpRequest_t *httpRequest, 
		char *buf, int buf_size)
{
	char *p = NULL;
	char *q = NULL;
	if (httpRequest 
		&& httpRequest->header_len > 0
		&& buf 
		&& buf_size > 0)
	{
		p = strchr(httpRequest->header, ' ');
		if (p)
		{
			p ++;
			q = straddr(p, "\r\n");
			if (q)
			{
				for ( ; p <= q && *p == ' '; p ++);
				q = strchr(p, ' ');
				if (q)
				{
					if (q - p > 0
						&& MIN(q - p, 
							buf_size -1) == (q - p))
					{
						memcpy(buf, p, q - p);
						return true;
					}
				}
			}
		}
	}

	return false;
}

static inline bool _httpRequest_get_url_parmer(
		httpRequest_t *httpRequest, 
		char *buf, int buf_size)
{
	int i = 0;
	int len = 0;
	char *p = NULL;
	if (_httpRequest_get_url(httpRequest, buf, buf_size) == true)
	{
		p = strchr(buf, '?');
		if (p)
		{
			len = strlen(p);
			for (i = 0; i < len; i ++)
			{
				buf[i] = p[i];
			}
			
			buf[i] = '\0';
			return true;
		}
	}

	return false;
}

bool httpRequest_get_url_parmer_value(
		httpRequest_t *httpRequest, 
		char *parmer_key, int parmer_key_len, 
		char *parmer_value_buf, int parmer_value_buf_size)
{
	char *q = NULL;
	char *n = NULL;
	char *p = NULL;
	int url_parmer_len = 0;
	char key[64] = {0};
	char buf[1024] = {0};
	if (parmer_key 
		&& parmer_key_len > 0
		&& parmer_value_buf 
		&& parmer_value_buf_size > 0)
	{
		if (_httpRequest_get_url_parmer(
				httpRequest, buf, sizeof(buf) - 1) == true)
		{
			//LOG_TRACE_NORMAL("url parmer: %s\n", buf);
			
			if (buf[0] == '?')
				p = &buf[1];
			else
				p = buf;

			url_parmer_len = strlen(p);
			if (url_parmer_len > 0)
			{
				snprintf(key, sizeof(key) - 1, "%s=", parmer_key);
				q = straddr(p, key);
				if (q)
				{
					if (q == p
						|| *(q - 1) == '&')
					{
						q += strlen(key);
						n = strchr(q, '&');
						if (n)
						{
							if (n - q > 0 
								&& MIN(n - q, 
									parmer_value_buf_size) == (n - q))
							{
								memcpy(parmer_value_buf, q, n - q);
								return true;
							}
						}
						else
						{
							if (url_parmer_len - (q - p) > 0
								&& MIN(url_parmer_len - (q - p), 
									parmer_value_buf_size) 
										== (url_parmer_len - (q - p)))
							{
								memcpy(parmer_value_buf, 
									q, url_parmer_len - (q - p));
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool httpRequest_get_url_path(
		httpRequest_t *httpRequest, 
		char *buf, int buf_size)
{
	char *p = NULL;
	if (_httpRequest_get_url(httpRequest, buf, buf_size) == true)
	{
		p = strchr(buf, '?');
		if (p)
		{
			*p = '\0';
		}

		return true;
	}

	return false;
}

bool httpRequest_find_field(httpRequest_t *httpRequest, 
		char *find, char *value_buf, int value_buf_size)
{
	char find_str[255] = {0};
	int find_str_len = 0;
	
	char *p = NULL;
	char *q = NULL;
	
	if (httpRequest 
		&& httpRequest->header 
		&& httpRequest->header_len > 0
		&& find && strlen(find) > 0 
		&& strlen(find) < sizeof(find_str) - 10
		&& value_buf && value_buf_size > 0)
	{
		find_str_len = snprintf(find_str, 
				sizeof(find_str) - 1, "\r\n%s:", find);
		p = straddr_case(httpRequest->header, find_str);
		if (p == NULL)
		{
            find_str_len = snprintf(find_str, 
                    sizeof(find_str) - 1, "%s:", find);
            p = straddr_case(httpRequest->header, find_str);
            if (p == NULL 
                || p != httpRequest->header)
            {
                p = NULL;
            }
		}

		if (p)
		{
			p += find_str_len;
			q = straddr(p, "\r\n");
			if (q)
			{
			    q --;
                for ( ; q - p > 0; )
				{
					if (*q != ' ')
						break;

					q --;
				}
				q ++;
			}

            for ( ; q - p > 0; )
            {
                if (*p != ' ')
                    break;
            
                p ++;
            }

			if (q && q - p > 0 && q - p < value_buf_size)
			{
				memcpy(value_buf, p, q - p);
				value_buf[q - p] = '\0';

				return true;
			}
		}
	}

	return false;
}

int httpRequest_get_content_length(httpRequest_t *httpRequest)
{
	int i;
	char value_buf[32] = {0};

	if (httpRequest == NULL)
	{
		return 0;
	}
	
	if (httpRequest_find_field(httpRequest, 
		"Content-Length", value_buf, 
		sizeof(value_buf) - 1) == false)
	{
		return 0;
	}
	
	LOG_TRACE_NORMAL("--> %s\n", value_buf);
	for (i = 0; i < strlen(value_buf); i ++)
	{
		if (! ('0' <= value_buf[i] 
			&& value_buf[i] <= '9'))
		{
			break;
		}
	}

	if (i >= strlen(value_buf))
	{
		return atoi(value_buf);
	}

	return 0;
}

httpRequest_result_t httpRequest_recv_header(
		event_buf_t *event_buf, httpRequest_t *httpRequest)
{
	return _httpRequest_recv_header(event_buf, httpRequest);
}

httpRequest_result_t httpRequest_recv_context(
		event_buf_t *event_buf, httpRequest_t *httpRequest)
{
	if (_httpRequest_context_buf_init(httpRequest))
	{
		return _httpRequest_recv_context(event_buf, httpRequest);
	}

	if (httpRequest && httpRequest->context_len <= 0)
	{
		return E_HTTP_REQUEST_RESULT_CONTEXT_RECV_SUCCESS;
	}

	return E_HTTP_REQUEST_RESULT_SOCKET_ERROR;
}

httpRequest_t *httpRequest_new()
{
	return _httpRequest_new();
}

void httpRequest_free(httpRequest_t *httpRequest)
{
	if (httpRequest)
	{
		_httpRequest_free(httpRequest);
	}
}

void httpRequest_context_free(httpRequest_t *httpRequest)
{
	if (httpRequest && httpRequest->context_buf)
	{
		free(httpRequest->context_buf);
		httpRequest->context_buf = NULL;
		httpRequest->data_len = 0;
	}
}

