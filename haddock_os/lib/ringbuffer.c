/**
 * ringbuffer.c
 *
 * \date   
 * \author jtuki@foxmail.com
 */

#include "kernel/hdk_memory.h"
#include "lib/assert.h"
#include "ringbuffer.h"

struct ringbuffer *rbuf_new(os_size_t capacity, os_size_t elem_size)
{
    haddock_assert(capacity > 0 && elem_size > 0);
    
    struct ringbuffer *rb = haddock_malloc(sizeof(*rb));
    if (!rb)
        return NULL;
        
    rb->buf = haddock_malloc(capacity * elem_size);
    if (!rb->buf) {
        haddock_free(rb);
        return NULL;
    }

    rb->hdr.capacity = capacity;
    rb->hdr.blk_size = elem_size;
    rb->hdr.len = 0;
    rb->hdr.start = 0; rb->hdr.end = 0;
    
    return rb;
}

#define advance_index(idx, capacity) do { \
        (idx) += 1; \
        if ((idx) == (capacity)) (idx) = 0; \
    } while(0)

os_int8 rbuf_push_back(struct ringbuffer *rb, void *data, os_size_t len)
{
    haddock_assert(rb && rb->buf && data && len == rb->hdr.blk_size);
    if (rb->hdr.len == rb->hdr.capacity)
        return -1;

    haddock_memcpy((char *)rb->buf + len * rb->hdr.end, data, len);
    advance_index(rb->hdr.end, rb->hdr.capacity);
    rb->hdr.len += 1;
    
    return 0;
}

void *rbuf_pop_front(struct ringbuffer *rb, void *pop_value, os_size_t len)
{
    haddock_assert(rb && rb->buf && pop_value && len == rb->hdr.blk_size);
    if (rb->hdr.len == 0)
        return NULL;
    
    haddock_memcpy(pop_value, (char *)rb->buf + len * rb->hdr.start, len);
    advance_index(rb->hdr.start, rb->hdr.capacity);
    rb->hdr.len -= 1;

    return pop_value; 
}

void rbuf_delete(struct ringbuffer *rb)
{
    haddock_assert(rb && rb->buf);
    
    haddock_free(rb->buf);
    haddock_free(rb);
}
