#include "fifo.h"

#define FIFO_DEBUG_ON 1

#if FIFO_DEBUG_ON
#define FIFO_DEBUG(fifo, fun_name, first...) {\
	_fifo_log_debug(fifo, fun_name, first); \
}
#else
#define FIFO_DEBUG(fifo, fun_name, first...) ;
#endif

#define _fifo_min(x, y) (x < y ? x : y)

static void _fifo_lock(fifo_t *fifo)
{
	if (fifo)
	{
		pthread_mutex_lock(&fifo->lock);
	}
}

static void _fifo_unlock(fifo_t *fifo)
{
	if (fifo)
	{
		pthread_mutex_unlock(&fifo->lock);
	}
}

#if FIFO_DEBUG_ON
static void _fifo_log_debug(fifo_t *fifo, char *fun_name, char *fmt, ...)
{
	#define DEBUG_SIZE 520
	char __DEBUG_STR[DEBUG_SIZE] = {0};
	snprintf(__DEBUG_STR, DEBUG_SIZE, "[fifo: %s] %s: ", fifo->name ? fifo->name : "null", fun_name ? fun_name : "null");
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(__DEBUG_STR + strlen(__DEBUG_STR), DEBUG_SIZE - strlen(__DEBUG_STR), fmt, ap);
	va_end(ap);
	
	snprintf(__DEBUG_STR + strlen(__DEBUG_STR), DEBUG_SIZE - strlen(__DEBUG_STR), "\r\n");
	
	printf("%s", __DEBUG_STR);
}
#endif

static int _fls(int x)
{
	int position = 0;
	int i;
	if (0 != x)
	{
		for (i = (x >> 1), position = 0; i != 0; ++ position)
		{
			i >>= 1;
		}
	}
	else
	{
		position = -1;
	} 

	return position + 1;
}


static unsigned int _roundup_pow_of_two(unsigned int x)
{
    return 1UL << _fls(x - 1);
}

unsigned int fifo_cal_buffer_size(unsigned int size)
{
	/**使用size & (size – 1)来判断size 是否为2幂*/
    if (size & (size - 1)) {
    	size = _roundup_pow_of_two(size);
	}

	return size;
}

fifo_t *fifo_create_by_array(fifo_t *fifo, char *fifo_name, 
		unsigned char *buffer, unsigned int size)
{
	memset(fifo, 0, sizeof(fifo_t));

    fifo->buffer = buffer;
    fifo->in = 0;
    fifo->out = 0;
	fifo->size = size;
	pthread_mutex_init(&fifo->lock, NULL);
	fifo->disable = true;
	
	fifo->name = fifo_name;

	FIFO_DEBUG(fifo, "fifo_create_by_array", "create finished.");
	
    return fifo;
}

unsigned int fifo_put(fifo_t *fifo,
             unsigned char *buffer, unsigned int len)
{
	if (fifo->disable)
	{
		FIFO_DEBUG(fifo, "fifo_put", "fifo disable ... ");
		return 0;
	}
	
	unsigned int data_len = len;
	
    unsigned int l;

	/**这个表达式计算当前写入的空间，换成人可理解的语言就是：
		l = kfifo可写空间和预期写入空间的最小值
	*/
    len = _fifo_min(len, fifo->size - fifo->in + fifo->out);
	if (len != data_len)
	{
		FIFO_DEBUG(fifo, "fifo_put", "no free space!(dl:%d, fl:%d)", data_len, len);
		return 0;
	}

    /* first put the data starting from fifo->in to buffer end */
    l = _fifo_min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(fifo->buffer, buffer + l, len - l);

    fifo->in += len;

    return len;
}

//这个是内核函数
unsigned int fifo_read(fifo_t *fifo,
             unsigned char *buffer, unsigned int len)
{
    unsigned int l;

    len = _fifo_min(len, fifo->in - fifo->out);

    /* first get the data from fifo->out until the end of the buffer */
    l = _fifo_min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);

    fifo->out += len;

    return len;
}

//预读，不进行移位
unsigned int fifo_pre_read(fifo_t *fifo, int offset, 
             unsigned char *buffer, unsigned int len)
{
    unsigned int l;

	unsigned int data_len = fifo->in - fifo->out;
	if (offset >= data_len)
	{
		return 0;
	}

	data_len -= offset;

    len = _fifo_min(len, data_len);

    /* first get the data from fifo->out until the end of the buffer */
    l = _fifo_min(len, fifo->size - ((fifo->out + offset) & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + ((fifo->out + offset) & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);

    return len;
}

#if 0
//linux 内核只有出站和入栈 ，没有这个移位读取，这个是我自己在linux基础上修改下的
unsigned int fifo_seek_read(fifo_t *fifo,
             unsigned char *buffer, unsigned int offset, unsigned int len)
{
    unsigned int l;

	unsigned int data_len = fifo->in - fifo->out;
	if (offset >= data_len)
	{
		return 0;
	}

	data_len -= offset;
	
    len = _fifo_min(len, data_len);

    /* first get the data from fifo->out until the end of the buffer */
    l = _fifo_min(len, fifo->size - ((fifo->out + offset) & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + ((fifo->out + offset) & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);

    return len;
}
#endif

unsigned int fifo_safe_put(fifo_t *fifo,
             unsigned char *buffer, unsigned int len)
{
	_fifo_lock(fifo);
	len = fifo_put(fifo, buffer, len);
	_fifo_unlock(fifo);

    return len;
}

unsigned int fifo_set_offset(fifo_t *fifo, unsigned int offset)
{
	unsigned int data_len = fifo->in - fifo->out;
	if (offset > data_len)
	{
		FIFO_DEBUG(fifo, "fifo_set_offset", "offset: %d, data_len: %d", offset, data_len);
		return 0;
	}
	
	fifo->out += offset;

    return offset;

}

void fifo_clean(fifo_t *fifo)
{
	if (fifo)
	{
		fifo->in = fifo->out = 0;
	}
}

void fifo_enable(fifo_t *fifo)
{
	fifo->disable = false;
}

void fifo_disable(fifo_t *fifo)
{
	fifo->disable = true;
}


