/**
 * types.h
 *
 * \date    
 * \author  jtuki@foxmail.com
 */

#ifndef HADDOCK_TYPES_H_
#define HADDOCK_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#define __HADDOCK

typedef unsigned char os_boolean;
#define OS_TRUE    1
#define OS_FALSE   0

#ifndef NULL
#define NULL    ((void *)0)
#define nullptr ((void *)0)
#endif

#if defined     __ARM_CORTEX_M
#define __processor_32bit__
#elif defined   __TI_MSP430
#define __processor_16bit__
#elif defined   __MCU_8051
#define __processor_8bit__
#endif

typedef unsigned char os_uint8;
typedef signed char os_int8;

#if defined __processor_32bit__
typedef unsigned int os_uint32;
typedef signed int os_int32;
typedef unsigned short os_uint16;
typedef signed short os_int16;

#elif defined   __processor_16bit__
typedef unsigned long os_uint32;
typedef signed long os_int32;
typedef unsigned int os_uint16;
typedef signed int os_int16;

#elif defined   __processor_8bit__
typedef unsigned long os_uint32;
typedef signed long os_int32;
typedef unsigned short os_uint16;
typedef signed short os_int16;

#endif /* __processor_32bit__ */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef os_uint32 os_size_t;

/**
 * For 4-byte alignment under 32-bit architecture.
 * @size here should be size_t type.
 */
#define ALIGNED_SIZE(size)     (((size)%4 == 0) ? (size) : (4*((size)/4+1)))

#ifdef __cplusplus
}
#endif

#endif /* HADDOCK_TYPES_H_ */
