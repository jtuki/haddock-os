/**
 * string_utils.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef HDK_UTILITIES_H_
#define HDK_UTILITIES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

/**
 * @example
 *      BV(0) - 0x0001
 *      BV(3) - 0x1000
 */
#define BV(n)  (((os_uint32)1)<<(n))

#define is_boolean(x) ((((os_uint8)(x)) & 0xFE) == 0)

os_boolean is_equal_string(const char *a, const char *b);


os_size_t find_first_1_bit_uint32(os_uint32 n);
os_size_t find_any_1_bit_uint32(os_uint32 n);
#define find_first_0_bit_uint32(n)  find_first_1_bit_uint32(~(n))
#define find_any_0_bit_uint32(n)    find_any_1_bit_uint32(~(n))

void hdk_srand(os_int32 seed);
os_int32 hdk_rand(void);
os_uint32 hdk_randr(os_uint32 min, os_uint32 max);

#ifndef offsetof
#define offsetof(st, m) ((os_size_t) (& ((st *)0)->m))
#endif

/**
 * Construction of uint32 contents. From 2 uint16 or 4 uint8.
 */
os_uint32 construct_u32_2(os_uint16 higher, os_uint16 lower);
os_uint32 construct_u32_4(os_uint8 highest, os_uint8 high,
                          os_uint8 low, os_uint8 lowest);
os_uint16 construct_u16_2(os_uint8 higher, os_uint8 lower);

void decompose_u32_2(os_uint32 i, os_uint16 *higher, os_uint16 *lower);
void decompose_u32_4(os_uint32 i,
                     os_uint8 *highest, os_uint8 *high,
                     os_uint8 *low, os_uint8 *lowest);
void decompose_u16_2(os_uint16 i, os_uint8 *higher, os_uint8 *lower);


enum platform_endian {
    PLATFORM_ENDIAN_LITTLE,
    PLATFORM_ENDIAN_BIG,
};

void hdk_init_platform_endian(void);

os_uint32 os_hton_u32(os_uint32 i);
os_uint16 os_hton_u16(os_uint16 i);
os_uint32 os_ntoh_u32(os_uint32 i);
os_uint16 os_ntoh_u16(os_uint16 i);

#ifdef __cplusplus
}
#endif

#endif /* HDK_UTILITIES_H_ */
