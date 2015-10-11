/**
 * hdk_utilities.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "assert.h"
#include "hdk_utilities.h"

/*---------------------------------------------------------------------------*/
os_uint32 construct_u32_2(os_uint16 higher, os_uint16 lower)
{
    return ((os_uint32) higher << 16) + lower;
}

os_uint32 construct_u32_4(os_uint8 highest, os_uint8 high,
                          os_uint8 low, os_uint8 lowest)
{
    return ((os_uint32) highest << 24) + ((os_uint32) high << 16) + \
           ((os_uint32) low << 8) + lowest;
}

os_uint16 construct_u16_2(os_uint8 higher, os_uint8 lower)
{
    return (higher << 8) + lower;
}

void decompose_u32_2(os_uint32 i, os_uint16 *higher, os_uint16 *lower)
{
    *higher = (os_uint16) ((i >> 16) & 0xFFFF);
    *lower = (os_uint16) (i & 0xFFFF);
}

void decompose_u32_4(os_uint32 i,
                     os_uint8 *highest, os_uint8 *high,
                     os_uint8 *low, os_uint8 *lowest)
{
    *highest    = (os_uint8) ((i >> 24) & 0xFF);
    *high       = (os_uint8) ((i >> 16) & 0xFF);
    *low        = (os_uint8) ((i >> 8) & 0xFF);
    *lowest     = (os_uint8) (i & 0xFF);
}

void decompose_u16_2(os_uint16 i, os_uint8 *higher, os_uint8 *lower)
{
    *higher = (os_uint8) ((i >> 8) & 0xFF);;
    *lower  = (os_uint8) (i & 0xFF);
}

/*---------------------------------------------------------------------------*/
/**
 * string and bit manipulation.
 * @{
 */

os_boolean is_equal_string(const char *a, const char *b)
{
    os_size_t i = 0;
    for (; a[i] != '\0' && b[i] != '\0'; i++) {
        if (a[i] == b[i])
            continue;
    }
    if (a[i] == b[i] && a[i] == '\0')
        return OS_TRUE;
    else
        return OS_FALSE;
}

/**
 * eg. return 0 for 0x800B, 1 for 0x4030 etc. 
 */
os_size_t find_first_1_bit_uint32(os_uint32 n)
{
    haddock_assert(n != 0);
    os_uint32 mask = 0x80000000;
    os_size_t i = 0;
    while ((n & mask) == 0) {
        mask = mask >> 1;
        i++;
    }
    return i;
}

os_size_t find_any_1_bit_uint32(os_uint32 n)
{
    return find_first_1_bit_uint32(n);
}

/**< @} */
/*---------------------------------------------------------------------------*/
/**
 * The random number generation algorithm is borrowed here.
 * http://goo.gl/BsouUB
 * @{
 */

#define RAND_LOCAL_MAX  2147483647 // ((1<<31) - 1)

static os_int32 next = 1;
static os_boolean is_srand = OS_FALSE;

/**
 * Use some random-likely value as seed first.
 * eg. the device's unique ID.
 */
void hdk_srand(os_int32 seed)
{
    next = seed; 
    is_srand = OS_TRUE;
}

os_int32 hdk_rand(void)
{
    haddock_assert(is_srand);
    return ((next = next * 1103515245 + 12345) % RAND_LOCAL_MAX);
}

/**
 * \return A random value range within [min, max].
 */
os_uint32 hdk_randr(os_uint32 min, os_uint32 max)
{
    haddock_assert(is_srand);
    if (min > max)
        return min;
    return (os_uint32)hdk_rand() % (max - min + 1) + min;
}

/** @} */
/*---------------------------------------------------------------------------*/
/**< @{ All the external communication message refer to network endian (big endian) */

static enum platform_endian _endian;
static os_boolean _platform_endian_checked = OS_FALSE;

/**< The external communication endian-ness: big endian */
static const enum platform_endian EXTERNAL_COMMUNICATION_ENDIAN = PLATFORM_ENDIAN_BIG;

void hdk_init_platform_endian(void)
{
    os_uint32 i = 0x55667788;
    _endian = (*((os_uint8 *) &i) == 0x55) ? PLATFORM_ENDIAN_BIG : PLATFORM_ENDIAN_LITTLE;
    _platform_endian_checked = OS_TRUE;
}

os_uint32 os_hton_u32(os_uint32 i)
{
    haddock_assert(_platform_endian_checked);
    if (_endian == EXTERNAL_COMMUNICATION_ENDIAN)
        return i;
    else {
        os_uint8 b1, b2, b3, b4;
        decompose_u32_4(i, &b1, &b2, &b3, &b4);
        return construct_u32_4(b4, b3, b2, b1);
    }
}

os_uint16 os_hton_u16(os_uint16 i)
{
    haddock_assert(_platform_endian_checked);
    if (_endian == EXTERNAL_COMMUNICATION_ENDIAN)
        return i;
    else {
        os_uint8 b1, b2;
        decompose_u16_2(i, &b1, &b2);
        return construct_u16_2(b2, b1);
    }
}

os_uint32 os_ntoh_u32(os_uint32 i)
{
    haddock_assert(_platform_endian_checked);
    if (_endian == EXTERNAL_COMMUNICATION_ENDIAN)
        return i;
    else {
        os_uint8 b1, b2, b3, b4;
        decompose_u32_4(i, &b1, &b2, &b3, &b4);
        return construct_u32_4(b4, b3, b2, b1);
    }
}

os_uint16 os_ntoh_u16(os_uint16 i)
{
    haddock_assert(_platform_endian_checked);
    if (_endian == EXTERNAL_COMMUNICATION_ENDIAN)
        return i;
    else {
        os_uint8 b1, b2;
        decompose_u16_2(i, &b1, &b2);
        return construct_u16_2(b2, b1);
    }
}

/**< @} */
/*---------------------------------------------------------------------------*/
