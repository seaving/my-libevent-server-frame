#include "sys_inc.h"
#include "log_trace.h"
#include "mempool.h"
#include "memalloc.h"

static void *_mem_palloc_block(mem_pool_t *pool, size_t size);
static inline void *_mem_palloc_small(mem_pool_t *pool, size_t size, uintptr_t align);
static void *_mem_palloc_large(mem_pool_t *pool, size_t size);
uintptr_t  mem_pagesize = 0;

/*
 * Linux has memalign() or posix_memalign()
 * Solaris has memalign()
 * FreeBSD 7.0 has posix_memalign(), besides, early version's malloc()
 * aligns allocations bigger than page size at the page boundary
 */
#if (mem_HAVE_POSIX_MEMALIGN || mem_HAVE_MEMALIGN)
#define mem_memalign(p, alignment, size)	\
	do { \
		p = NULL; \
	    if (posix_memalign(&p, alignment, size)) { \
	        LOG_TRACE_PERROR("posix_memalign(%d, %d) failed !\n", (int) alignment, (int) size); \
	        p = NULL; \
	    } \
		else { \
	   		LOG_TRACE_NORMAL("posix_memalign: %p:%lu @%lu\n", p, (int) size, (int) alignment); \
		} \
	} while(0)
#else
#define mem_memalign(p, alignment, size)	\
	do { \
		p = NULL; \
	    p = memalign(alignment, size); \
	    if (p == NULL) { \
	        LOG_TRACE_PERROR("memalign(%d, %d) failed !\n", (int) alignment, (int) size); \
	    } \
		else { \
	   		LOG_TRACE_NORMAL("memalign: %p:%d @%d\n", p, (int) size, (int) alignment); \
		} \
	} while(0)
#endif

//-----------------------------------------------------------------------
static void *_mem_palloc_block(mem_pool_t *pool, size_t size)
{
    unsigned char      *m;
    size_t       psize;
    mem_pool_t  *p, *new;

    psize = (size_t) (pool->d.end - (unsigned char *) pool);

   	mem_memalign(m, MEM_POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    new = (mem_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(mem_pool_data_t);
    m = mem_align_ptr(m, MEM_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}

static inline void *_mem_palloc_small(mem_pool_t *pool, size_t size, uintptr_t align)
{
    unsigned char	*m;
    mem_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = mem_align_ptr(m, MEM_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return _mem_palloc_block(pool, size);
}

static void *_mem_palloc_large(mem_pool_t *pool, size_t size)
{
    void              *p;
    uintptr_t         n;
    mem_pool_large_t  *large;

    p = calloc(1, size);
    if (p == NULL) {
    	LOG_TRACE_PERROR("calloc error !\n");
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = (mem_pool_large_t *) _mem_palloc_small(pool, sizeof(mem_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void mem_pool_init()
{
	mem_pagesize = getpagesize();
	LOG_TRACE_NORMAL("mem_pagesize = %u\n", (unsigned int) mem_pagesize);
}

mem_pool_t *mem_create_pool(size_t size)
{
    mem_pool_t  *p = NULL;

    mem_memalign(p, MEM_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (unsigned char *) p + sizeof(mem_pool_t);
    p->d.end = (unsigned char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(mem_pool_t);
    p->max = (size < MEM_MAX_ALLOC_FROM_POOL) ? size : MEM_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;
    return p;
}

void mem_destroy_pool(mem_pool_t *pool)
{
    mem_pool_t          *p, *n;
    mem_pool_large_t    *l;
    mem_pool_cleanup_t  *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }

#if 0//(mem_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (l = pool->large; l; l = l->next) {
        LOG_TRACE_NORMAL("free: %p\n", l->alloc);
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        LOG_TRACE_NORMAL("free: %p, unused: %uz\n", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}

void mem_reset_pool(mem_pool_t *pool)
{
    mem_pool_t        *p;
    mem_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (unsigned char *) p + sizeof(mem_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->chain = NULL;
    pool->large = NULL;
}

void *mem_palloc(mem_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return _mem_palloc_small(pool, size, 1);
    }

    return _mem_palloc_large(pool, size);
}

void *mem_pnalloc(mem_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return _mem_palloc_small(pool, size, 0);
    }

    return _mem_palloc_large(pool, size);
}

void *mem_pmemalign(mem_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    mem_pool_large_t  *large;

    mem_memalign(p, alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = _mem_palloc_small(pool, sizeof(mem_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

intptr_t mem_pfree(mem_pool_t *pool, void *p)
{
    mem_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            LOG_TRACE_NORMAL("free: %p\n", l->alloc);
            free(l->alloc);
            l->alloc = NULL;

            return 0;
        }
    }

    return -1;
}

void *mem_pcalloc(mem_pool_t *pool, size_t size)
{
    void *p;

    p = mem_palloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

mem_pool_cleanup_t *mem_pool_cleanup_add(mem_pool_t *p, size_t size)
{
    mem_pool_cleanup_t  *c;

    c = mem_palloc(p, sizeof(mem_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = mem_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    LOG_TRACE_NORMAL("add cleanup: %p\n", c);

    return c;
}

