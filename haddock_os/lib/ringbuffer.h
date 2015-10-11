/**
 * ringbuffer.h
 *
 * \date   
 * \author jtuki@foxmail.com
 */

#ifndef HADDOCK_RINGBUFFER_H_
#define HADDOCK_RINGBUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

struct ringbuf_hdr
{
    os_size_t capacity;  /**< total capacity */
    os_size_t blk_size;  /**< memory usage == (capacity * blk_size) */
    os_size_t len;       /**< current length */
    os_size_t start;     /**< range [0, capacity) */
    os_size_t end;
};

/**
 * Resemble FIFO queue, with fixed size.
 */
struct ringbuffer
{
    struct ringbuf_hdr hdr;
    void *buf; /**< real buffer part, which contains the data */
};

/**
 * New a struct ringbuffer.
 * @param capacity
 * @param elem_size Equal to struct ringbuf_hdr::blk_size
 * @return NULL if memory allocation failed.
 */
struct ringbuffer *rbuf_new(os_size_t capacity, os_size_t elem_size);

/**
 * Push back @data (with length @len) to @rb.
 * @return -1 if @rb is full.
 */
os_int8 rbuf_push_back(struct ringbuffer *rb, void *data, os_size_t len);

/**
 * Pop front value (FIFO) to @pop_value (with length @len).
 * @param len Must be equal to rb->hdr.blk_size.
 * @return NULL if ringbuffer is empty.
 */
void *rbuf_pop_front(struct ringbuffer *rb, void *pop_value, os_size_t len);

/**
 * Delete ringbuffer @rb.
 */
void rbuf_delete(struct ringbuffer *rb);

#define rbuf_length(rb) ((rb)->hdr.len)

#ifdef __cplusplus
}
#endif

#endif // end of HADDOCK_RINGBUFFER_H_
