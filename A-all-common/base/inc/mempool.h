#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#define mem_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define mem_align_ptr(p, a)                                                   \
		(unsigned char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define MEM_ALIGNMENT   sizeof(unsigned long)    /* platform word */
//------------------------------------------------------------------------------

/*
 * MEM_MAX_ALLOC_FROM_POOL should be (mem_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define MEM_MAX_ALLOC_FROM_POOL  (mem_pagesize - 1)

#define MEM_DEFAULT_POOL_SIZE    (16 * 1024)

#define MEM_POOL_ALIGNMENT       16
#define MEM_MIN_POOL_SIZE                                                     \
    mem_align((sizeof(mem_pool_t) + 2 * sizeof(mem_pool_large_t)),            \
              MEM_POOL_ALIGNMENT)

typedef struct __mem_pool__	mem_pool_t;
typedef struct __mem_pool_large__  mem_pool_large_t;
typedef struct __mem_pool_cleanup__  mem_pool_cleanup_t;
typedef struct __mem_chain__ mem_chain_t;
typedef struct __mem_buf__  mem_buf_t;
typedef struct __mem_pool_data__ mem_pool_data_t;
typedef void				*mem_buf_tag_t;

typedef void (*mem_pool_cleanup_pt)(void *data);

struct __mem_buf__
{
    unsigned char	*pos;
    unsigned char	*last;
    off_t            file_pos;
    off_t            file_last;

    unsigned char	*start;         /* start of buffer */
    unsigned char	*end;           /* end of buffer */
    mem_buf_tag_t    tag;
    mem_buf_t       *shadow;


    /* the buf's content could be changed */
    unsigned         temporary:1;

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;

    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;

    unsigned         recycled:1;
    unsigned         in_file:1;
    unsigned         flush:1;
    unsigned         sync:1;
    unsigned         last_buf:1;
    unsigned         last_in_chain:1;

    unsigned         last_shadow:1;
    unsigned         temp_file:1;

    /* STUB */ int   num;
};

struct __mem_chain__
{
    mem_buf_t    *buf;
    mem_chain_t  *next;
};

struct __mem_pool_cleanup__
{
    mem_pool_cleanup_pt   handler;
    void                 *data;
    mem_pool_cleanup_t   *next;
};

struct __mem_pool_large__
{
    mem_pool_large_t     *next;
    void                 *alloc;
};

struct __mem_pool_data__
{
    unsigned char		*last;			//指向未使用内存的首部地址
    unsigned char		*end;			//指向未使用内存的尾部地址
    mem_pool_t			*next;			//多个mem_pool_t通过next连接成链表
    uintptr_t			failed;			//每当剩余空间不足以分配出小块内存时，failed成员就会加1。
    									//failed成员大于4后，mem_pool_t的current将移向下一个小块内存池
};

struct __mem_pool__
{
    mem_pool_data_t       d;			//当前内存块
    size_t                max;			//全部可使用内存的大小，包含已使用和未使用内存。
    									//区别小块和大块内存的标准，小于等于max为小块内存，大于max为大块内存
    mem_pool_t           *current;		//在多个mem_pool_t连成的链表中，current指向分配内存时遍历的第一个mem_pool_t
    mem_chain_t          *chain;
    mem_pool_large_t     *large;		//指向多个mem_pool_large_t连成的链表
    mem_pool_cleanup_t   *cleanup;		//指向多个mem_pool_cleanup_t连成的链表
};

void mem_pool_init();
mem_pool_t *mem_create_pool(size_t size);
void mem_destroy_pool(mem_pool_t *pool);
void mem_reset_pool(mem_pool_t *pool);

void *mem_palloc(mem_pool_t *pool, size_t size);
void *mem_pnalloc(mem_pool_t *pool, size_t size);
void *mem_pcalloc(mem_pool_t *pool, size_t size);
void *mem_pmemalign(mem_pool_t *pool, size_t size, size_t alignment);
intptr_t mem_pfree(mem_pool_t *pool, void *p);

mem_pool_cleanup_t *mem_pool_cleanup_add(mem_pool_t *p, size_t size);

#endif

