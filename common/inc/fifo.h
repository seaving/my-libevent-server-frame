
#ifndef __FIFO_H__
#define __FIFO_H__

#include "sys_inc.h"

/**
kfifo提供如下对外功能规格
1.只支持一个读者和一个读者并发操作
2.无阻塞的读写操作，如果空间不够，则返回实际访问空间
*/

#define FIFO_DATA_LEN(fifo) 	(fifo->in - fifo->out)
#define FIFO_FREE_SIZE(fifo) 	(fifo->size - fifo->in + fifo->out)
#define FIFO_BUFF_SIZE(fifo) 	(fifo->size)


typedef pthread_mutex_t fifo_lock_t;

typedef struct __fifo__
{
	char 		*name;		/* fifo的名字 */
    unsigned char *buffer;  /* 用于存放数据的缓存 */
    unsigned int size;    	/* buffer空间的大小，在初化时，将它向上扩展成2的幂 */
    unsigned int in;    	/* in指向buffer中队头 */
    unsigned int out;    	/* 而且out指向buffer中的队尾 */
    fifo_lock_t lock;    	/* 如果使用不能保证任何时间最多只有一个读线程和写线程，需要使用该lock实施同步 */
	bool 		disable;	/* 禁用fifo操作 */
} fifo_t;

typedef fifo_t FT_FIFO;

unsigned int fifo_cal_buffer_size(unsigned int size);

fifo_t *fifo_create_by_array(fifo_t *fifo, char *fifo_name, 
		unsigned char *buffer, unsigned int size);

unsigned int fifo_put(fifo_t *fifo,
             unsigned char *buffer, unsigned int len);

unsigned int fifo_read(fifo_t *fifo,
             unsigned char *buffer, unsigned int len);

 unsigned int fifo_pre_read(fifo_t *fifo, int offset, 
			  unsigned char *buffer, unsigned int len);

unsigned int fifo_seek_read(fifo_t *fifo,
             unsigned char *buffer, unsigned int offset, unsigned int len);

unsigned int fifo_safe_put(fifo_t *fifo,
             unsigned char *buffer, unsigned int len);

unsigned int fifo_set_offset(fifo_t *fifo, unsigned int offset);

void fifo_clean(fifo_t *fifo);

void fifo_enable(fifo_t *fifo);

void fifo_disable(fifo_t *fifo);

#endif


