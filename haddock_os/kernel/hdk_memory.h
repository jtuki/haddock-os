/**
 * hdk_memory.h
 *
 * \date   
 * \author jtuki@foxmail.com
 */
#ifndef HADDOCK_MEMORY_H_
#define HADDOCK_MEMORY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

void *haddock_malloc(os_size_t size);

void haddock_free(void *ptr);

/**
 * Copy @len bytes from source @src to destination @dest.
 * Note: there should be NO memory overlap between src and dest. 
 * 
 * @return dest address.
 */
void *haddock_memcpy(void *dest, const void *src, os_size_t len);

/**
 * @return dest address 
 */
void *haddock_memset(void *dest, char value, os_size_t len);

#ifdef __cplusplus
}
#endif

#endif /* HADDOCK_MEMORY_H_ */
