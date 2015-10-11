/**
 * mem_pool.c
 *
 * \date   
 * \author jtuki@foxmail.com
 */

#include "assert.h"
#include "hdk_utilities.h"
#include "mem_pool.h"

#define _calc_pool_bv_num(capacity)  ((capacity)/32 + (((capacity)%32==0)?0:1))

struct mem_pool_hdr *mem_pool_create(os_uint16 capacity, os_uint16 obj_size)
{
    os_size_t blk_size = ALIGNED_SIZE((os_size_t) obj_size);
    os_size_t _bv_num = _calc_pool_bv_num(capacity);
    
    os_size_t _pool_size = capacity * (blk_size + sizeof(struct mem_pool_blk));
    
    os_size_t offset_bv = sizeof(struct mem_pool_hdr);
    os_size_t offset_bv_num = offset_bv + (_bv_num) * sizeof(os_uint32);
    
    os_size_t _mem_size = offset_bv_num
                       + sizeof (os_uint32) // bv_num
                       + _pool_size;
                       
    struct mem_pool_hdr *mem_pool;
    if ((mem_pool = haddock_malloc(ALIGNED_SIZE(_mem_size))) == NULL)
        return NULL;
    haddock_memset(mem_pool, 0, ALIGNED_SIZE(_mem_size));
    
    mem_pool->blk_size = blk_size;
    mem_pool->capacity = capacity;
    mem_pool->size = 0;
    
    os_uint32 *bv_num = (os_uint32 *) ((char *)mem_pool + offset_bv_num);
    *bv_num = _bv_num;
    
    return mem_pool;
}

void mem_pool_destroy(struct mem_pool_hdr *mem_pool)
{
    // we do nothing here. memory poll are not freed.
}

static inline void *get_pool_buffer(struct mem_pool_hdr *hdr)
{
    /**
     * \ref mem_pool_hdr
     */
    return (char*) (hdr->bv) + \
           sizeof(os_uint32) * _calc_pool_bv_num(hdr->capacity) + \
           sizeof(os_uint32);
}

static inline
void *get_blk_buffer(struct mem_pool_hdr *hdr, os_size_t alloc_id)
{
    haddock_assert(alloc_id < hdr->capacity);
    return (char *)get_pool_buffer(hdr) + \
           alloc_id * (sizeof(struct mem_pool_blk) + hdr->blk_size);
}

static inline
void *get_mem_pool_hdr(struct mem_pool_blk *blk)
{
    os_uint32 *bv_num = (os_uint32*) ((char*)blk - \
            (blk->alloc_id)*(sizeof(struct mem_pool_blk) + blk->blk_size) - sizeof(os_uint32));
    os_size_t offset = sizeof(os_uint32) * (*bv_num) + sizeof(struct mem_pool_hdr);
    return (os_uint8 *)bv_num - offset;
}

struct mem_pool_blk *mem_pool_alloc_blk(struct mem_pool_hdr *mem_pool,
                                        os_size_t obj_size)
{
    os_size_t blk_size = ALIGNED_SIZE(obj_size);
    haddock_assert(blk_size == mem_pool->blk_size);
    if (mem_pool->size == mem_pool->capacity)
        return NULL;

    os_uint32 _bv_num = _calc_pool_bv_num(mem_pool->capacity);
    os_uint32 *bv = mem_pool->bv;
    
    os_size_t i = 0;
    os_size_t found_bit;
    for (; i < _bv_num; i++) {
        if (bv[i] != 0xFFFFFFFF) {
            found_bit = find_any_0_bit_uint32(bv[i]);
            bv[i] |= ((os_uint32)1)<<(31-found_bit);
            break;
        }
    }
    
    if (i == _bv_num)
        return NULL;
    
    os_size_t alloc_id = i*32 + found_bit;
    struct mem_pool_blk *blk = get_blk_buffer(mem_pool, alloc_id);
    
    blk->alloc_id = alloc_id;
    blk->blk_size = blk_size;
    haddock_memset(blk->blk, 0, blk_size);
    
    mem_pool->size += 1;
    
    return blk; /** \note not blk->blk */
}

void mem_pool_free_blk(struct mem_pool_blk *blk)
{
    haddock_assert(blk);
    
    struct mem_pool_hdr *hdr = get_mem_pool_hdr(blk);
    haddock_assert(blk->blk_size == hdr->blk_size);
    
    (hdr->bv)[blk->alloc_id / 32] ^= ((os_uint32)1)<<(31 - (blk->alloc_id % 32));
    
    haddock_assert(hdr->size > 0);
    hdr->size -= 1;
}
