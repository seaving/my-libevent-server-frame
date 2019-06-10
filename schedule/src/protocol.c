#include "includes.h"


bool protocol_parse(event_buf_t *event_buf, const httpRequest_t *httpRequest)
{
	const char *context = NULL;
	const char *header = httpRequest_get_header(httpRequest);
	if (header == NULL)
	{
		return false;
	}

	context = httpRequest_get_context_buf(httpRequest);
	if (context == NULL)
	{
		return false;
	}

	LOG_TRACE_NORMAL(">>>>>>>> \n%s%s\n", header, context);

	return true;
}

